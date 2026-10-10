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

## 各平台使用教程

### Windows

#### 方式一：uv（推荐）

```powershell
# 安装 uv（如果尚未安装）
powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"

# 安装 Clings
uv tool install clings-win

# 启动
clings
```

#### 方式二：pip

```powershell
# 安装 Python（从 https://www.python.org/downloads/ 下载，勾选 Add to PATH）

# 安装 Clings
pip install clings-win

# 启动
clings
```

#### 方式三：GitHub Codespaces

在浏览器中打开 https://github.com/lilyco-42/clings → Code → Codespaces → Create codespace，然后在终端运行：

```bash
pip install clings-win
clings --host 0.0.0.0 --port 3000
```

通过 Codespaces 端口转发访问 Web UI。

---

### macOS

#### 方式一：uv（推荐）

```bash
# 安装 uv
curl -LsSf https://astral.sh/uv/install.sh | sh

# 安装 Clings
uv tool install clings-win

# 启动
clings
```

#### 方式二：Homebrew + pip

```bash
# 安装 Python
brew install python

# 安装 Clings
pip3 install clings-win

# 启动
clings
```

---

### Linux

#### Ubuntu / Debian

```bash
# 安装 Python
sudo apt update
sudo apt install python3 python3-pip

# 安装 Clings
pip3 install clings-win

# 启动
clings
```

#### Fedora / RHEL

```bash
# 安装 Python
sudo dnf install python3 python3-pip

# 安装 Clings
pip3 install clings-win

# 启动
clings
```

#### Arch Linux

```bash
# 安装 Python
sudo pacman -S python python-pip

# 安装 Clings
pip install clings-win

# 启动
clings
```

#### Docker

```bash
docker run -p 3000:3000 --rm -it python:3.12-slim bash -c \
  "pip install clings-win && clings --host 0.0.0.0 --port 3000"
```

浏览器打开 `http://localhost:3000`。

---

### Android (Termux)

从 [F-Droid](https://f-droid.org/packages/com.termux/) 下载安装 Termux（推荐 F-Droid 版本，Google Play 版本较旧）。

```bash
# 更新包管理器
pkg update && pkg upgrade

# 安装 Python
pkg install python

# 安装 Clings
pip install clings-win

# 启动
clings
```

手机浏览器打开 `http://localhost:3000`。

> **提示**：WASM 模式下无需安装 GCC，所有编译在浏览器端完成。如需服务端编译，还需安装 `pkg install clang`。

---

### Chromebook (Crostini)

```bash
# 启用 Linux 容器（设置 → 高级 → 开发者 → Linux 开发环境）

# 安装 Python
sudo apt update
sudo apt install python3 python3-pip

# 安装 Clings
pip3 install clings-win

# 启动
clings
```

浏览器打开 `http://localhost:3000`。

---

### GitHub Codespaces

适合任何设备（包括 iPad、Chromebook 等无法直接安装 Python 的设备）。

1. 打开 https://github.com/lilyco-42/clings
2. 点击 **Code** → **Codespaces** → **Create codespace**
3. 等待环境初始化，在终端运行：

```bash
pip install clings-win
clings --host 0.0.0.0 --port 3000
4
```

4. 点击 **Ports** 面板中的 **Open in Browser** 访问 Web UI

---

## 功能

- **双编译模式**：浏览器 WASM（无需 GCC）+ 服务端 GCC/Clang
- **Web UI**：CodeMirror 编辑器、C 语言自动补全、深色主题
- **CLI**：init / watch / list / hint / score
- **响应式设计**：支持桌面、平板、手机浏览器
- **跨平台**：Windows / macOS / Linux / Android / Chromebook / Docker

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
