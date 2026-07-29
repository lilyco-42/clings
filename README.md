# Clings

> **C 语言交互式练习平台** — 浏览器内 WASM 编译 + 服务端 GCC 双模式

> 🏕️ **[Learning C Camp 2026](https://opencamp.cn/C/camp/2026)** 练习工具 &nbsp;|&nbsp; 4 个 Unit · 72 课 · 约 97 道练习

---

## 快速开始

```bash
# 方式一：setup 脚本（推荐）
setup.bat          # Windows
bash setup.sh      # Linux / macOS

# 方式二：手动安装
pip install -e .
npm install
clings
```

启动后自动打开浏览器 `http://localhost:3000`，开始做题。

> **环境要求**：Python 3.11+ &nbsp;|&nbsp; Node.js &nbsp;|&nbsp; GCC 或 Clang

---

## 功能特性

### 浏览器 WASM 编译（默认）

- 基于 [browsercc](https://www.npmjs.com/package/browsercc) 的 Clang/LLD WASM 工具链
- 无需本地 GCC，浏览器内直接编译 C 代码
- 首次加载约 95MB（clang.wasm + lld.wasm + sysroot），后续使用浏览器缓存
- WASI 运行时执行编译产物

### 服务端编译（备选）

- 使用本地 GCC/Clang 编译
- 支持 stdout 比对、return 退出码检查
- GCC/Clang 诊断解析：错误/警告数量 + 首条错误信息 Toast 提示

### Web UI

- **CodeMirror 编辑器**：深色 Dracula 主题，行号、括号匹配
- **C 语言自动补全**：关键字、标准库函数、`#include` 头文件补全
- **响应式设计**：桌面 / 平板 / 手机自适应，移动端侧边栏抽屉
- **Toast 通知**：编译成功/失败、错误详情、运行结果
- **练习导航**：左侧 Unit/Lesson 树形列表，点击切换
- **进度追踪**：完成状态持久化

### CLI 命令

| 命令 | 说明 |
|------|------|
| `clings` | 启动 Web 服务（默认端口 3000） |
| `clings init unit0` | 初始化练习文件 |
| `clings watch` | 交互式 watch 模式（终端） |
| `clings list` | 列出练习 + 进度 |
| `clings hint` | 查看提示 |
| `clings score` | CI 评分 |

---

## 项目结构

```
clings/
├── clings/                     # Python 包
│   ├── api.py                  # FastAPI Web 服务
│   ├── main.py                 # CLI/GUI 入口
│   ├── core/                   # 核心模块
│   │   ├── compiler.py         # GCC/Clang 编译器封装
│   │   ├── config.py           # 练习发现 + 配置
│   │   └── state.py            # 进度状态
│   ├── routes/                 # API 路由
│   │   ├── compile.py          # /api/compile-and-run
│   │   ├── exercises.py        # /api/exercises
│   │   └── progress.py         # /api/progress
│   └── static/                 # 前端资源
│       ├── index.html          # 主页面
│       ├── css/style.css       # 深色主题样式
│       ├── js/app.js           # 前端逻辑
│       ├── js/c-hint.js        # C 语言自动补全
│       └── wasm/               # WASM 编译器
│           ├── clingswasm.js   # 编译器接口
│           ├── browsercc.js    # browsercc 入口
│           ├── clang.js        # Clang Emscripten 胶水
│           ├── lld.js          # LLD Emscripten 胶水
│           ├── wasi.js         # WASI 运行时
│           └── fs_mem.js       # 内存文件系统
├── exercises/                  # 练习数据
├── clings.toml                 # Unit 配置
├── pyproject.toml              # Python 包配置
├── package.json                # npm 依赖
├── setup.bat / setup.sh        # 一键安装脚本
└── README.md
```

---

## 安装部署

### 开发环境

```bash
git clone https://github.com/lilyco-42/clings.git
cd clings
pip install -e .
npm install
python -m clings
```

### 生产部署

```bash
pip install -e .
npm install
python -m clings --port 3000
```

### Docker（可选）

```dockerfile
FROM python:3.12-slim
RUN apt-get update && apt-get install -y gcc nodejs npm
COPY . /app
WORKDIR /app
RUN pip install -e . && npm install
CMD ["python", "-m", "clings"]
```

---

## 练习系统

### 评测模式

| 模式 | 说明 | 示例 |
|------|------|------|
| `return` | 检查退出码 | `expected_return = 0` |
| `stdout` | 标准输出比对 | 九九乘法表逐行匹配 |
| `compile` | 编译通过即可 | 语法练习 |

### 课程路线图

| Unit | 名称 | 题数 | 核心内容 |
|:----:|------|:----:|----------|
| 0 | C Primer | 9 | 最简程序、printf、循环、条件、累加 |
| 1 | C Fundamentals | 40 | 函数、数组、指针、结构体、位运算、状态机 |
| 2 | C Essentials | 24 | 链表、栈、队列、树、排序、查找 |
| 3 | C Classicals | 24 | 17 个 CS 子领域经典问题 |

---

## 常见问题

**Q: 浏览器编译很慢？**
首次加载 WASM 约 95MB，后续使用浏览器缓存。也可切换到服务端编译模式。

**Q: Windows 能用吗？**
可以。支持原生 Windows（GCC/Clang via MSVC/Scoop）和 WSL2。

**Q: 能离线使用吗？**
服务端编译模式可完全离线。WASM 模式首次需要网络下载。

---

## License

[MIT](LICENSE) © 2026 OpenCamp C Camp Team
