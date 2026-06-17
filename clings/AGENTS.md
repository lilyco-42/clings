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
