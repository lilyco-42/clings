# commands/ — 子命令开发规范

## 现有命令

| 文件        | 命令                      | 功能              |
| ----------- | ------------------------- | ----------------- |
| `check.py`  | `clings check [selector]` | 批量验证练习 (遇到失败立即退出) |
| `doctor.py` | `clings doctor`           | 检查开发环境                    |
| `hint.py`   | `clings hint [exercise]`  | 显示练习提示                    |
| `init.py`   | `clings init [unit]`      | 初始化练习文件                  |
| `list.py`   | `clings list [selector]`  | 列出练习 + 进度                 |
| `reset.py`  | `clings reset <exercise>` | 重置练习文件                    |
| `run.py`    | `clings run [exercise]`   | 运行单个练习                    |
| `score.py`  | `clings score [selector]` | CI 打分 (跑完全部, 输出 JSON + set-output) |
| `watch.py`  | `clings watch [selector]` | 交互式 watch 模式               |

## 新增命令模板

```python
"""clings <name> — <一句话描述>."""

import argparse
import sys

from ..config import load_config  # 按需导入
# from ..compiler import ...
# from ..state import ...
# from ..utils import ...


def cmd_<name>(args: argparse.Namespace) -> int:
    """<命令功能描述>."""
    config = load_config()
    # ... 实现 ...
    return 0
```

## 注册新命令流程

1. 在 `commands/` 下创建 `<name>.py`，实现 `cmd_<name>(args) -> int`
2. 在 `cli.py` 中：
   - 添加 import: `from .commands.<name> import cmd_<name>`
   - 添加 argparse subparser 定义
   - 设置 `p.set_defaults(func=cmd_<name>)`
3. 运行测试验证不破坏现有功能

## 命令函数约定

- **签名**: `def cmd_xxx(args: argparse.Namespace) -> int`
- **返回值**: 0 = 成功，1 = 失败，130 = 用户中断
- **输出**: 正常输出到 stdout，错误输出到 stderr
- **异常**: 抛出 `ClingsError` 交由 `cli.py` 统一处理，不要在命令内 `sys.exit()`
- **导入**: 运行时才需要的重模块用 lazy import（如 `cmd_run` 中的 `import random`）

## 导入路径约定

```python
# 从包顶层模块导入
from ..config import ...
from ..compiler import ...
from ..state import ...
from ..utils import ...
from ..renderer import ...

# 绝不在 commands/ 之间互相导入
# 错误：from .watch import WatchState  ← 不要这样做
# 正确：from ..state import WatchState
```

## ANSI 颜色使用

从 `..utils` 导入预定义常量，不要硬编码 escape 序列：

```python
from ..utils import ANSI_BOLD_GREEN, ANSI_RESET
print(f"{ANSI_BOLD_GREEN}success{ANSI_RESET}")
```
