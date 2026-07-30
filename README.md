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

## Android (Termux)

在手机上使用 Clings，需要先安装 Termux（推荐从 F-Droid 安装，版本更新）。

### 1. 安装 Termux

从 [F-Droid](https://f-droid.org/packages/com.termux/) 下载安装 Termux。

### 2. 安装依赖

```bash
pkg update && pkg upgrade
pkg install python git
```

### 3. 安装 Clings

```bash
pip install clings-win
```

### 4. 启动 Web 服务

```bash
clings
```

启动后在手机浏览器打开 `http://localhost:3000`。

> **提示**：WASM 模式下无需安装 GCC，所有编译在浏览器端完成。如需服务端编译（`--no-wasm`），还需安装 `pkg install clang`。

### 5. 配合 GitHub Codespaces（可选）

如果本地算力不足，可使用 GitHub Codespaces 在云端运行：

```bash
# 在 Codespaces 中
pip install clings-win
clings --host 0.0.0.0 --port 3000
```

然后通过 Codespaces 端口转发访问 Web UI。

---

## 功能

- **双编译模式**：浏览器 WASM（无需 GCC）+ 服务端 GCC/Clang
- **Web UI**：CodeMirror 编辑器、C 语言自动补全、深色主题
- **CLI**：init / watch / list / hint / score
- **响应式设计**：支持桌面、平板、手机浏览器
- **跨平台**：Linux / macOS / Windows / Android (Termux)

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
uv run python -m clings
```

---

## License

[MIT](LICENSE) © 2026 OpenCamp C Camp Team
