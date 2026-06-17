# Clings — AI Agent Instructions

Rustlings-style C exercises CLI，配套 OpenCamp C 2026 Summer 课程。

## 技术栈

- **语言**: Python 3.11+，**零第三方依赖**（仅 stdlib）
- **打包**: hatchling（pyproject.toml）
- **CI**: CNB 流水线（.cnb.yml）→ PyPI 发布
- **分支**: `cli`（默认且唯一活跃分支）

## 命令

```bash
# 开发
pip install -e .                    # editable 安装
python -m clings -v                 # 查看版本
python -m clings doctor             # 检查环境

# 测试验证
python -m clings check unit0 --solutions   # Unit 0 (9 题)
python -m clings check unit1 --solutions   # Unit 1 (40 题)

# 构建发布
python -m build                     # 构建 wheel + sdist
twine upload dist/*                 # 发布到 PyPI

# CI 打分
python -m clings score unit0              # 跑完全部题目, 输出 JSON + ##[set-output score=N]
python -m clings score unit0 --json       # 同时在 stdout 输出 JSON 报告

# 学生使用
uvx clings init unit0              # 初始化 Unit 0 练习
clings                              # 进入交互式 watch 模式
```

## 项目结构

```
clings/                    # 仓库根目录 (cli 分支)
├── clings/                # Python 包 (详见 clings/AGENTS.md)
│   ├── cli.py             # 入口 + argparse
│   ├── config.py          # 配置、发现、路径
│   ├── compiler.py        # 编译、测试执行
│   ├── state.py           # 进度状态管理
│   ├── renderer.py        # Watch UI 渲染
│   ├── utils.py           # 共享工具 (ANSI/terminal/reset)
│   └── commands/          # 子命令 (详见 commands/AGENTS.md)
├── exercises/             # 练习数据 (打包时 force-include 到 clings/exercises/)
├── solutions/             # 参考答案 (不打包)
├── clings.toml            # Unit 配置 (打包时 force-include 到 clings/clings.toml)
├── pyproject.toml         # 包元数据 + hatchling 构建配置
├── .cnb.yml               # CI 流水线
└── CURRICULUM.md          # 课程大纲
```

## 开发规范

### 零依赖原则 [P0]

**绝不引入第三方依赖**。所有功能仅使用 Python 标准库实现。这是项目的核心设计约束——保证 `pip install clings` 在任何 Python 3.11+ 环境即装即用。

### 包发布检查清单

每次修改包代码（`clings/` 目录或 `pyproject.toml`）时：

1. **bump 版本号**（`pyproject.toml` 中的 `version`）
   - 新功能：minor bump（如 4.0 → 4.1）
   - bug 修复 / 重构：patch bump（如 4.0.1 → 4.0.2）
   - 破坏性变更：major bump（如 4.x → 5.0）
2. **运行测试**: `python -m clings check unit0 --solutions && python -m clings check unit1 --solutions`
3. **验证版本**: `python -m clings -v`
4. PyPI 不允许覆盖已发布的同版本文件

### Unit 配置

新增/修改 Unit 只需编辑 `clings.toml`，CLI 自动适配（`_load_unit_lesson_ranges()` 动态解析）。

### 打包机制

- 仓库中 `exercises/` 和 `clings.toml` 位于顶层（方便编辑）
- wheel 中通过 `force-include` 映射到 `clings/exercises/` 和 `clings/clings.toml`
- `config.py` 中 `EXERCISES_DIR` / `PKG_CONFIG` 自动检测两种路径（pip install vs editable）

### Commit 规范

格式：`<emoji> <type>(<scope>): <中文主题>`

| 类型     | Emoji | 适用场景        |
| -------- | ----- | --------------- |
| feat     | ✨    | 新功能          |
| fix      | 🐛    | 缺陷修复        |
| refactor | ♻️    | 结构调整        |
| docs     | 📝    | 文档改动        |
| build    | 📦    | 构建/打包改动   |
| chore    | 🔧    | 工具链/日常维护 |
| ci       | 👷    | CI/CD 配置      |
| test     | ✅    | 测试            |

scope 必填（如 cli, init, watch, config, ci, pypi 等）。

## 安全规则

- 不提交 PyPI token 或任何凭据到仓库
- `solutions/` 不打入 pip 包
- `.cnb.yml` 中 token 通过 `imports` 从加密仓库获取
