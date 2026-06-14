# Clings CLI

> **C 语言版 [Rustlings](https://github.com/rust-lang/rustlings)** 的 CLI 工具。

这是 clings 练习系统的命令行工具仓库。学生使用的练习仓库在 [clings](https://cnb.cool/q.qq/opencamp-c-2026-summer/clings)。

## 安装

```sh
# 从 CNB PyPI 制品库安装
pip install clings --index-url https://pypi.cnb.cool/q.qq/opencamp-c-2026-summer/clings-cli/-/packages/simple

# 或从源码安装 (开发模式)
pip install -e .
```

## 使用

```sh
cd <练习仓库目录>
clings              # 进入交互式 watch 模式
clings doctor       # 检查环境
clings list unit1   # 查看练习列表
clings run          # 运行下一道 pending 练习
clings hint         # 查看提示
```

## 开发

```sh
git clone <此仓库>
cd clings-cli/
pip install -e .
clings check unit1 --solutions   # 验证 CLI + 练习一致性
```

## 发布

CI 自动构建并发布到 CNB PyPI 制品库（push to main 触发）。

手动发布：
```sh
python -m build
twine upload dist/* --repository-url https://pypi.cnb.cool/q.qq/opencamp-c-2026-summer/clings-cli/-/packages/simple
```

## 架构

```
clings-cli/
├── clings.py          # CLI 核心 (单文件, 零依赖, Python 3.11+)
├── pyproject.toml     # pip 打包配置
├── .cnb.yml           # CI: test → build → publish
├── exercises/         # 练习文件 (用于 CI 测试, 不打包)
├── solutions/         # 参考答案 (用于 CI 测试, 不打包)
└── clings.toml        # 全局配置 (用于 CI 测试, 不打包)
```
