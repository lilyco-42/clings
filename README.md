# Clings

[![PyPI version](https://img.shields.io/pypi/v/clings-win.svg)](https://pypi.org/project/clings-win/)
[![Python](https://img.shields.io/pypi/pyversions/clings-win.svg)](https://pypi.org/project/clings-win/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

> C 语言交互式练习平台 — 浏览器内 WASM 编译 + 服务端 GCC 双模式

---

## 安装

```bash
# 推荐：uv tool install（全局可用，隔离环境）
uv tool install clings-win

# 或：uvx 临时运行（无需安装）
uvx clings

# 或：pip
pip install clings-win
```

安装后直接使用：

```bash
clings              # 启动 Web 服务，浏览器打开 http://localhost:3000
clings init unit0   # 初始化 Unit 0 练习文件
clings watch        # 终端交互式 watch 模式
```

---

## 功能

- **双编译模式**：浏览器 WASM（无需 GCC）+ 服务端 GCC/Clang
- **Web UI**：CodeMirror 编辑器、C 语言自动补全、深色主题
- **CLI**：init / watch / list / hint / score

| 命令 | 说明 |
|------|------|
| `clings` | 启动 Web 服务（默认端口 3000） |
| `clings init <unit>` | 初始化练习文件 |
| `clings watch` | 交互式 watch 模式 |
| `clings list` | 列出练习 + 进度 |
| `clings hint` | 查看提示 |
| `clings score` | CI 评分 |

---

## 课程

| Unit | 名称 | 题数 |
|:----:|------|:----:|
| 0 | C Primer | 9 |
| 1 | C Fundamentals | 40 |
| 2 | C Essentials | 24 |
| 3 | C Classicals | 24 |

---

## 开发

```bash
git clone https://github.com/lilyco-42/clings.git
cd clings
uv sync
npm install
uv run python -m clings
```

---

## License

[MIT](LICENSE) © 2026 OpenCamp C Camp Team
