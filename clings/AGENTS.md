# clings/ 包 — 架构指南

## 模块依赖图

```
cli.py                 # 入口：argparse 定义 + 命令分发
├── config.py          # 叶节点：路径、TOML 加载、发现、异常定义
├── compiler.py        # 编译测试：依赖 config
├── state.py           # 进度状态：依赖 config
├── utils.py           # 共享工具：依赖 config
├── renderer.py        # UI 渲染：依赖 state, utils, config
└── commands/          # 子命令：依赖上述模块
    ├── watch.py       # 依赖 compiler, config, state, renderer, utils
    ├── run.py         # 依赖 compiler, config, state
    ├── check.py       # 依赖 compiler, config
    ├── init.py        # 依赖 config
    ├── reset.py       # 依赖 config, state, utils
    ├── list.py        # 依赖 config, state, utils
    ├── hint.py        # 依赖 config, state
    └── doctor.py      # 依赖 config
```

## 模块职责 (一句话)

| 模块          | 职责                                                     |
| ------------- | -------------------------------------------------------- |
| `__init__.py` | 包入口，导出 `main`                                      |
| `__main__.py` | 支持 `python -m clings`                                  |
| `cli.py`      | argparse 参数定义 + 命令分发                             |
| `config.py`   | 路径常量、TOML 加载、练习发现、编译器检测、Unit 范围解析 |
| `compiler.py` | 编译 C 源码、运行测试用例、验证返回值                    |
| `state.py`    | `WatchState` 类：进度持久化（读写 `.clings-state.txt`）  |
| `renderer.py` | `WatchRenderer` 类：Watch 模式的终端 UI                  |
| `utils.py`    | ANSI 颜色常量、terminal 工具函数、`reset_exercise`       |
| `commands/`   | 8 个子命令各自独立文件（详见 `commands/AGENTS.md`）      |

## 核心规则

### 依赖方向严格单向

```
commands/* → compiler / state / renderer / utils → config
```

**禁止反向依赖**：`config.py` 不得导入 `compiler`/`state`/`commands` 等上层模块。

### 循环导入禁止

如果出现循环导入，说明职责划分有问题，需要重新拆分——不要用 lazy import hack 绕过。

### editable 路径 fallback

`config.py` 中的 `EXERCISES_DIR` 和 `PKG_CONFIG` 使用双路径检测：

- 优先检测包内路径（`pip install` 正式安装）
- 若不存在则 fallback 到仓库顶层（`pip install -e .` 开发模式）

修改路径相关代码时必须同时验证两种安装模式。

### 新增模块规范

1. 放在 `clings/` 顶层（与 `config.py` 同级）
2. 文件名使用 snake_case
3. 模块顶部写 docstring 说明职责
4. 只从 `config.py` / 同级模块导入，不从 `commands/` 导入

### ClingsError

唯一的自定义异常类，定义在 `config.py` 中。所有用户可见的错误都抛此异常，由 `cli.py` 的 `main()` 统一捕获输出。

## 测试体系（tests_py/）

clings 自身的 pytest 测试位于 `tests_py/`（与 `tests/` 严格隔离——后者是 C 练习题的测试数据，被 force-include 进 wheel；前者不打包）。

### 目录结构

```
tests_py/
├── conftest.py           # 共享 fixture（isolated_workspace 隔离模块级常量）
├── test_compiler.py      # normalize / _collect_cases / _make_source_mtime_signature
├── test_config.py        # select_exercises / find_exercise / discover / load_toml / test_files_for
├── test_state.py         # WatchState 全类 / next_pending_exercise
├── test_utils.py         # progress_bar / get_mtime / terminal_hyperlink / source_files_for
├── test_score.py         # _detect_status
└── test_integration.py   # 端到端：真实编译 C + run_cases 全断言 + check_one dispatch
```

### 开发命令

```bash
pip install -e ".[dev]"                  # 安装 pytest（dev 依赖，不进发布包）
pytest tests_py/ -v                      # 跑全部测试
pytest tests_py/test_compiler.py -v      # 跑单个模块
pytest tests_py/ --cov=clings            # 覆盖率报告
pytest tests_py/ -k normalize            # 按关键字筛选
```

### 关键设计：模块级常量值拷贝陷阱

`config.py` 顶部的模块级常量（`ROOT`/`STATE_FILE`/`BUILD_DIR`/`PUBLIC_TEST_DIR` 等）被 `state.py`/`utils.py`/`compiler.py`/`reset.py` 通过 `from .config import X` **值拷贝**（绑定了 import 时的 Path 对象）。

测试用 `monkeypatch` 替换这些常量时，**必须 patch 每个消费者模块的本地引用**，仅 patch `clings.config` 模块不够。`conftest.py` 的 `isolated_workspace` fixture 统一处理此问题——新增依赖这些常量的模块时，若测试报"路径不对"，检查是否需要在 conftest 补 patch。

### 测试分层

| 层级 | 文件 | 特点 |
|------|------|------|
| 单元测试（纯函数） | test_compiler/test_config/test_utils/test_score | 秒级，无 IO |
| 文件系统测试 | test_config(续)/test_state/test_compiler(续) | 秒级，用 tmp_path |
| 集成测试（真实编译） | test_integration | 分钟级，需 gcc |

### Known Gaps（暂未覆盖）

- `renderer.py` / `commands/watch.py`：TUI 交互，需 pty + 模拟按键，ROI 不足
- `compiler.py` 的 `check_make` / `check_make_stdout`：make 流程需真实 Makefile 集成测试
- Windows 特定路径（MinGW 检测、`.exe` 后缀）：CI 在 Linux

### 约束

- **pytest 是 dev 依赖**：`[project.optional-dependencies] dev`，`pip install clings`（终端用户）不装 pytest，零运行时依赖原则不受影响
- **`tests_py/` 不打包**：`pyproject.toml` 的 `force-include` 只含 `tests`（C 数据），`tests_py` 不进 wheel
