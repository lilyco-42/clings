# Clings — C 语言练习系统 CLI

[![PyPI][badge-pypi]][pypi]
[![Python][badge-python]][pypi]
[![Downloads][badge-downloads]][pypi]
[![License][badge-license]][pypi]

> **C 语言版 [Rustlings](https://github.com/rust-lang/rustlings)**：通过修复编译错误、补全代码来学习 C 语言。
> 配套 [OpenCamp C 2026 Summer](https://cnb.cool/q.qq/opencamp-c-2026-summer) 课程 Unit 0–4，共 **121 道练习**。

## 分支说明

| 分支                   | 用途                                                 |
| ---------------------- | ---------------------------------------------------- |
| **cli** (本分支，默认) | CLI 工具源码 + 练习数据，CI 自动发布 pip 包          |
| main                   | 练习仓库 (学生 clone 用，已废弃，改用 `clings init`) |

## 快速开始

```sh
# 1. 安装 uv (推荐)
curl -LsSf https://astral.sh/uv/install.sh | sh

# 2. 初始化练习 + 开始做题 (一行搞定，无需安装 clings)
uvx clings init unit0
uvx clings
```

### 其他安装方式

```sh
# pipx (隔离安装到独立环境)
pipx install clings

# pip + 虚拟环境
python3 -m venv .venv && source .venv/bin/activate
pip install clings
```

> **注意**: Ubuntu 23.04+ / Python 3.11+ 禁止直接 `pip install` 到系统环境（PEP 668）。
> 请使用上述 uvx / pipx / venv 方式，避免 `--break-system-packages`。

## 命令参考

```sh
clings                        # 直接进入 watch 模式 (默认)
clings init [unit0|...|all]   # 初始化练习 (从包内释放到当前目录)
clings watch [selector]       # 交互式 watch (保存即验证)
clings run [exercise]         # 运行练习 (省略=下一道，'random'=随机)
clings hint [exercise]        # 查看提示 (省略=下一道)
clings list [selector]        # 列出练习 + ✔/• 进度状态
clings check [selector]       # 批量验证
clings reset <exercise>       # 重置练习文件
clings reset progress         # 清除全部进度
clings doctor                 # 检查环境
clings -v                     # 显示版本号
```

## 开发

```sh
git clone https://cnb.cool/q.qq/opencamp-c-2026-summer/clings.git
cd clings/                     # 默认在 cli 分支
pip install -e .
clings check unit1 --solutions # 验证 49/49
```

## CI/CD

push 到 `cli` 分支自动触发：

1. **Install & Test**: pip install → clings init all → check unit1 --solutions
2. **Build & Publish**: python -m build → twine upload 到 PyPI

## 架构

```
clings/ (cli 分支)
├── clings.py              # CLI 核心 (单文件, 零依赖, Python 3.11+)
├── pyproject.toml         # pip 打包 (exercises/ 嵌入 wheel)
├── clings.toml            # units 配置
├── .cnb.yml               # CI: cli 分支 push → test → publish
├── Dockerfile.ci          # Ubuntu 24.04 CI 环境
├── exercises/             # 121 道练习 (嵌入 pip 包, clings init 释放)
│   ├── 01_simplest_c_program/
│   │   ├── exercises.toml # 练习元数据 + 测试用例
│   │   ├── *.c            # 学生源码模板
│   │   └── README.md      # 课程参考
│   └── ...
├── solutions/             # Unit 1 参考答案 (CI 测试用, 不打包)
└── tools/                 # 维护脚本
```

<!-- badges -->

[badge-pypi]: https://img.shields.io/pypi/v/clings?style=flat-square
[badge-python]: https://img.shields.io/pypi/pyversions/clings?style=flat-square
[badge-downloads]: https://img.shields.io/pypi/dm/clings?style=flat-square
[badge-license]: https://img.shields.io/pypi/l/clings?style=flat-square

<!-- links -->

[pypi]: https://pypi.org/project/clings/
