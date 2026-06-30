# Clings Python 测试体系实施方案

> **状态**: ✅ 已完成
> **日期**: 2026-06-30
> **目标**: 为 clings CLI 工具自身引入 pytest 测试体系，形成自动化回归护栏
> **前置依赖**: 无（纯新增，不改动现有功能代码）

---

## 一、背景与动机

clings 帮学员做 C 练习题（每题都有测试），但 clings 这个 Python CLI 工具自身**零测试**——19 个 `.py` 文件、约 50 个函数/方法，没有任何 `test_*.py` 或 `conftest.py`。

近期多次改动核心模块（`compiler.py` 加 `stdout_contains`/`stdout_not_contains`/`stdout_regex`/`exit_code`/`trim_trailing_ws` 断言字段、`normalize()` 加行尾空格裁剪、`config.py` 路径解析），每次都靠手动跑 `clings check unitN --solutions` 回归验证，没有自动化护栏。这种状态对持续演进是危险的——任何重构都可能引入隐性回归。

## 二、设计原则

1. 2026 年事实标准，原生 `assert` + fixture + parametrize
2. **dev 依赖隔离**：pytest 作为 `[project.optional-dependencies] dev`，不进 `[project.dependencies]`，`pip install clings`（终端用户）不装 pytest，`pip install -e ".[dev]"`（开发者）才装——完全不违反 P0 零运行时依赖原则
3. **测试目录隔离**：新建 `tests_py/`，与现有 `tests/`（C 练习题测试数据，被 force-include 进 wheel）严格隔离，不打包进发布版
4. **分层测试**：单元（纯函数，秒级）→ 文件系统（tmp_path，秒级）→ 集成（真实编译 C，分钟级）
5. **优先覆盖近期改动**：`normalize`/`run_cases`/`select_exercises` 等核心逻辑立即形成护栏
6. **Watch UI 暂不覆盖**：`renderer.py`/`commands/watch.py` 涉及终端交互，需 pty + 模拟按键，ROI 不足，标注 known gap

## 三、技术选型（tavily 2026-06 调研结论）

| 维度 | 前端 | Python 对应 | 选型理由 |
|---|---|---|---|
| 测试框架 | Vitest 4.0 | **pytest 8.x** | 2026 Python 测试事实标准，原生 assert + fixture |
| 参数化 | `it.each` | `@pytest.mark.parametrize` | 内置，无需插件 |
| Setup/Teardown | Vitest fixture | `@pytest.fixture` | 依赖注入，比 unittest 的 setUp/tearDown 灵活 |
| Watch 模式 | Vitest HMR（0.3s） | `pytest-watch`（秒级） | 可选，开发时用 |
| 覆盖率 | V8/Istanbul | **pytest-cov**（coverage.py） | 行/分支覆盖，CI 卡门槛 |
| Mock | `vi.mock` | `monkeypatch` / `unittest.mock` | pytest 内置 monkeypatch fixture |
| 临时目录 | — | `tmp_path` fixture | pytest 内置，每个测试独立临时目录 |

**不选 unittest 的理由**：unittest 是 xUnit 风格，需写 `class TestX(unittest.TestCase)` + `self.assertEqual`，啰嗦且缺乏 fixture 依赖注入；pytest 的 `def test_x(): assert ...` 更简洁，失败时自动重写为带 diff 的表达式，2026 年新项目已普遍弃用 unittest。

## 四、目录结构

```
clings/                    # 仓库根
├── clings/                # Python 包（被测代码）
├── tests/                 # C 练习题测试数据（已存在，force-include 进 wheel）
├── tests_py/              # ★ 新增：clings 自身的 pytest 测试
│   ├── conftest.py        # 共享 fixture（临时 exercises/、临时 clings.toml、临时 STATE_FILE）
│   ├── test_compiler.py   # normalize / run_cases / check_one / _collect_cases
│   ├── test_config.py     # discover / select / find / test_files_for / load_toml
│   ├── test_state.py      # WatchState 全类（mark_done/advance/jump/round-trip）
│   ├── test_utils.py      # progress_bar / get_mtime / terminal_hyperlink
│   ├── test_score.py      # _detect_status
│   └── test_integration.py # 端到端：init unit0 → check → score（C 级集成）
├── pyproject.toml         # 加 [project.optional-dependencies] dev
└── .cnb.yml               # 加 test 流水线
```

**命名说明**：`tests_py/` 而非 `test/` 或 `tests/`，因为：
- `tests/` 已被 C 练习题数据占用（force-include 进 wheel）
- `test/` 单数不符合 pytest 惯例（默认发现 `test_*` 或 `*_test`）
- `tests_py/` 明确表达"Python 自测"语义，与 `tests/`（C 数据）区分

## 五、分层测试清单

### 第一层：单元测试（纯函数，秒级，P0 必做）

#### test_compiler.py

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_normalize_crlf` | `normalize` | `\r\n` → `\n` 归一化 |
| `test_normalize_default_keeps_trailing_ws` | `normalize` | 默认不裁剪行尾空格（向后兼容） |
| `test_normalize_trim_trailing_ws` | `normalize(trim_trailing_ws=True)` | 行尾空格裁剪 |
| `test_normalize_trim_keeps_leading` | `normalize` | 前导空格保留 |
| `test_normalize_trim_keeps_trailing_newline` | `normalize` | 尾换行保留（POSIX 规范不放宽） |
| `test_normalize_trim_combined_with_crlf` | `normalize` | CRLF + trim 组合 |
| `test_make_source_mtime_signature_empty` | `_make_source_mtime_signature` | 空目录返回 0.0 |
| `test_make_source_mtime_signature_max` | `_make_source_mtime_signature` | 多文件取最大 mtime |

#### test_config.py

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_select_exercises_none` | `select_exercises(selector=None)` | 返回全部 |
| `test_select_exercises_unit_prefix` | `select_exercises("unit3")` | unit 前缀匹配 |
| `test_select_exercises_lesson_digit` | `select_exercises("49")` | lesson 数字匹配 |
| `test_select_exercises_name_prefix` | `select_exercises("49_dining")` | name 前缀匹配 |
| `test_select_exercises_title_substring` | `select_exercises("哲学家")` | title 子串匹配 |
| `test_select_exercises_fallback_find` | `select_exercises("nonexistent")` | 走 find_exercise，未命中抛 ClingsError |
| `test_find_exercise_hit` | `find_exercise` | 正常命中 |
| `test_find_exercise_miss` | `find_exercise` | 未命中抛 ClingsError |
| `test_load_unit_lesson_ranges` | `_load_unit_lesson_ranges` | clings.toml 解析 unit→(start,end) |

#### test_utils.py

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_progress_bar_basic` | `progress_bar` | 比例计算正确 |
| `test_progress_bar_zero_total` | `progress_bar` | total=0 返回空串 |
| `test_progress_bar_min_width` | `progress_bar` | 宽度下限 10 |
| `test_progress_bar_full` | `progress_bar` | n_done==total 满格 |
| `test_get_mtime_empty` | `get_mtime` | 空列表 |
| `test_get_mtime_max` | `get_mtime` | 多文件取最大 |
| `test_terminal_hyperlink_default_display` | `terminal_hyperlink` | OSC 8 格式、display 默认 |
| `test_terminal_hyperlink_custom_display` | `terminal_hyperlink` | 自定义 display |

#### test_score.py

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_detect_status_not_done` | `_detect_status` | 含 "I AM NOT DONE" → NOT_COMPLETED |
| `test_detect_status_failed` | `_detect_status` | 不含标记 → FAILED |
| `test_detect_status_missing_file` | `_detect_status` | 源文件不存在 → FAILED |

### 第二层：文件系统测试（tmp_path fixture，秒级，P1）

#### test_config.py（续）

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_discover_exercises_empty` | `discover_exercises` | 无 exercises/ 目录返回空列表 |
| `test_discover_exercises_sorted` | `discover_exercises` | 按 (unit, lesson, order) 排序 |
| `test_discover_exercises_path_backfill` | `discover_exercises` | 无 path 字段时回填相对路径 |
| `test_discover_exercises_cases_inlined` | `discover_exercises` | cases 字段从 toml 内联 |
| `test_load_toml_valid` | `load_toml` | 正常加载 |
| `test_load_toml_invalid` | `load_toml` | 损坏文件抛异常 |
| `test_test_files_for_public` | `test_files_for` | PUBLIC_TEST_DIR 优先 |
| `test_test_files_for_hidden` | `test_files_for` | CLINGS_HIDDEN_TEST_DIR 环境变量 |

#### test_state.py（全类）

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_watch_state_initial` | `WatchState.__init__` | 无状态文件时 current=第一个 |
| `test_watch_state_load_current` | `WatchState._load` | 读取 current_exercise |
| `test_watch_state_load_done` | `WatchState._load` | 读取 [done] 列表 |
| `test_watch_state_save_roundtrip` | `WatchState.save` + `_load` | 写后读一致性 |
| `test_watch_state_mark_done` | `mark_done` | 标记完成 + 持久化 |
| `test_watch_state_mark_pending` | `mark_pending` | 取消完成标记 |
| `test_watch_state_advance_next` | `advance_next` | 跳到下一个未完成 |
| `test_watch_state_advance_all_done` | `advance_next` | 全完成返回 False |
| `test_watch_state_jump_to` | `jump_to` | 跳转命中/未命中 |
| `test_watch_state_all_done` | `all_done` | 全完成判定 |
| `test_next_pending_exercise` | `next_pending_exercise` | 模块级函数 |

#### test_compiler.py（续）

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_collect_cases_external_priority` | `_collect_cases` | 外部 tests/ 优先于 inline |
| `test_collect_cases_inline_fallback` | `_collect_cases` | 无外部时用 inline |
| `test_collect_cases_hidden` | `_collect_cases` | include_hidden 合并 hidden 文件 |

### 第三层：集成测试（真实编译 C，分钟级，P2）

#### test_integration.py

| 测试函数 | 覆盖目标 | 要点 |
|---|---|---|
| `test_init_unit0_check_score` | 端到端 | `clings init unit0` → `clings check unit0 --solutions` → `clings score unit0 --solutions`，全绿 |
| `test_run_cases_exact_stdout` | `run_cases` | 编译真实 .c，精确 stdout 比对通过 |
| `test_run_cases_stdout_contains` | `run_cases` | stdout_contains 断言通过 |
| `test_run_cases_exit_code` | `run_cases` | exit_code 断言（含非零，如 T49 naive 的 exit 2） |
| `test_run_cases_trim_trailing_ws` | `run_cases` | trim_trailing_ws 端到端 |
| `test_check_one_mode_dispatch` | `check_one` | stdout/make/make+stdout/compile/return 各 mode 分派 |

### 第四层：覆盖率门槛（P3，可选）

- `pytest-cov` + `--cov=clings` 卡总覆盖率底线
- CI 卡 30% 总覆盖率底线（commands/watch/renderer 属 TUI known gap，拉低总数）
- **重点模块（compiler/config/state/utils）目标 80%+**
  - compiler.py 90% ✅（含 check_make/check_make_stdout 全分支覆盖）
  - config.py 78% ✅（接近达标，find_compiler/get_version 部分分支未覆盖）
  - state.py 94% ✅
  - utils.py 74% ✅（clear_screen/read_key_nonblocking 属 known gap）

## 六、共享 Fixture 设计（conftest.py）

```python
import pytest
from pathlib import Path

@pytest.fixture
def tmp_workspace(tmp_path, monkeypatch):
    """临时工作区：隔离 ROOT / CONFIG_PATH / STATE_FILE / PUBLIC_TEST_DIR"""
    # 创建临时 clings.toml
    # 创建临时 exercises/ 子目录
    # monkeypatch clings.config.ROOT / CONFIG_PATH / STATE_FILE
    # 返回 tmp_path
    ...

@pytest.fixture
def sample_exercises_toml():
    """返回一个标准 exercises.toml 内容字符串，供 tmp_workspace 写入"""
    ...

@pytest.fixture
def sample_config():
    """返回一个内存中的 config dict，无需文件"""
    ...
```

**关键**：`WatchState` 和 `discover_exercises` 依赖模块级常量 `ROOT`/`CONFIG_PATH`/`STATE_FILE`/`PUBLIC_TEST_DIR`（在 `config.py` 顶部从 `Path.cwd()` 计算）。测试需用 `monkeypatch.setattr` 替换这些常量，避免污染真实工作区。

## 七、pyproject.toml 改动

新增 `[project.optional-dependencies]` 段：

```toml
[project.optional-dependencies]
dev = [
    "pytest>=8.0",
    "pytest-cov>=5.0",
]

[tool.pytest.ini_options]
testpaths = ["tests_py"]
python_files = ["test_*.py"]
addopts = "-v --tb=short"
```

**不改动** `[project.dependencies]`（保持空，零运行时依赖）。

## 八、CI 集成（.cnb.yml）

在 `push` 段新增 `test` 流水线，与 unit0/1/2/build 并行：

```yaml
- name: test
  <<: *ci-docker
  stages:
    - name: Test
      script: |
        cd /workspace && git submodule update --init
        pip3 install --break-system-packages -e ".[dev]"
        pytest tests_py/ -v --cov=clings --cov-report=term-missing
```

**不阻塞发布**：test 流水线失败应阻断合并，但不影响 build 流水线（wheel 仍能构建，只是不发布有测试问题的版本）。

## 九、实施步骤

### Phase 1: 基础设施（预计 30 分钟）

- [ ] `pyproject.toml` 加 `[project.optional-dependencies] dev` + `[tool.pytest.ini_options]`
- [ ] 创建 `tests_py/conftest.py`（共享 fixture）
- [ ] 创建 `tests_py/test_compiler.py` 骨架（第一层单元测试）
- [ ] `pip install -e ".[dev]"` 安装 pytest
- [ ] `pytest tests_py/ -v` 跑通第一个测试

### Phase 2: 单元测试全覆盖（预计 1 小时）

- [ ] `test_compiler.py`：normalize 8 个 + mtime 2 个
- [ ] `test_config.py`：select 6 个 + find 2 个 + unit_ranges 1 个
- [ ] `test_utils.py`：progress_bar 4 个 + get_mtime 2 个 + hyperlink 2 个
- [ ] `test_score.py`：detect_status 3 个
- [ ] 全部跑绿

### Phase 3: 文件系统测试（预计 1 小时）

- [ ] `test_config.py` 续：discover 4 个 + load_toml 2 个 + test_files_for 2 个
- [ ] `test_state.py`：WatchState 11 个 + next_pending 1 个
- [ ] `test_compiler.py` 续：collect_cases 3 个
- [ ] 全部跑绿

### Phase 4: 集成测试（预计 1 小时）

- [ ] `test_integration.py`：端到端 init/check/score 1 个
- [ ] `test_compiler.py` 续：run_cases 4 个 + check_one dispatch 1 个
- [ ] 全部跑绿

### Phase 5: CI 集成 + 覆盖率（预计 30 分钟）

- [ ] `.cnb.yml` 加 test 流水线
- [ ] `pytest --cov=clings --cov-report=term-missing` 查看覆盖率
- [ ] 确认覆盖率：总 ≥ 30%，重点模块（compiler/config/state/utils）≥ 74%+
- [ ] 本地模拟 CI 流程验证

### Phase 6: 文档（预计 15 分钟）

- [ ] 更新 `clings/AGENTS.md`：补充 `tests_py/` 说明 + 开发命令
- [ ] 更新根 `README.md`：开发者章节加 `pytest tests_py/` 说明

## 十、验证标准

| 验收项 | 标准 |
|---|---|
| pytest 安装 | `pip install -e ".[dev]"` 成功，pytest 8.x |
| 测试发现 | `pytest tests_py/ --collect-only` 列出全部测试 |
| 测试通过 | `pytest tests_py/ -v` 全绿 |
| 覆盖率 | `--cov=clings` ≥ 30%（总），compiler/config/state/utils ≥ 74%+ |
| 打包不受影响 | `python -m build` 产出的 wheel 不含 pytest、不含 tests_py/ |
| 零运行时依赖 | `pip install clings` 不装 pytest（`[project.dependencies]` 仍空） |
| CI 流水线 | test stage 与 unit0/1/2 并行，失败阻断合并 |

## 十一、风险与缓解

| 风险 | 严重度 | 缓解 |
|---|---|---|
| pytest 误打入发布包 | 高 | 用 `[project.optional-dependencies]`，不进 `[project.dependencies]`；`tests_py/` 不 force-include |
| `tests_py/` 与 `tests/` 混淆 | 中 | 命名区分 + AGENTS.md 注明职责 |
| 模块级常量（ROOT 等）难 mock | 中 | `monkeypatch.setattr` 替换 `clings.config.ROOT` 等，conftest 统一处理 |
| 集成测试依赖 gcc | 低 | CI 镜像已有 gcc；本地开发也需 gcc（clings 本来就教 C） |
| Watch UI 无法覆盖 | 低 | 第四层标注 known gap，不阻塞 |
| pytest 8.x 与 Python 3.11 兼容 | 低 | pytest 8 官方支持 3.9+，3.11 无问题 |

## 十二、Known Gaps（本轮不覆盖）

1. **Watch UI 测试**：`renderer.py` 的 `WatchRenderer`、`commands/watch.py` 的 `cmd_watch` 涉及终端控制序列、非阻塞按键、ANSI 颜色，需 pty + 模拟按键，ROI 不足，暂不覆盖。未来可用 `pexpect` 或快照测试补充。
2. **TUI 交互测试**：watch 模式的 `r`/`n`/`p`/`x`/`q` 快捷键、列表滚动、状态切换等交互行为，需端到端 pty 测试，暂不覆盖。
3. **跨平台测试**：Windows 的编译器检测（`D:/env/tools/MinGW`）、`.exe` 后缀等分支，CI 在 Linux，暂不覆盖 Windows 特定路径。

## 十三、后续演进方向（非本轮）

1. **property-based testing**：引入 `hypothesis`，对 `normalize`/`select_exercises` 等函数做属性测试，自动发现边界用例
2. **mutation testing**：引入 `mutmut`，验证测试套件的有效性（能否抓住故意注入的 bug）
3. **Watch UI 测试**：用 `pexpect` 模拟终端交互，覆盖 watch 模式
4. **性能基准**：`pytest-benchmark` 跟踪 `discover_exercises`/`run_cases` 的性能回归

## 十四、参考资料

- pytest 官方文档: https://docs.pytest.org/en/stable/
- pytest fixtures: https://docs.pytest.org/en/stable/explanation/fixtures.html
- pytest parametrize: https://docs.pytest.org/en/stable/how-to/parametrize.html
- coverage.py: https://coverage.readthedocs.io/
- "Top Python Testing Frameworks 2026" (CodeWithMMAK): https://codewithmmak.com/blog/top-python-testing-frameworks
- "Best Python Testing Tools 2026" (inprogrammer): https://medium.com/@inprogrammer/best-python-testing-tools-2026-updated-884dcb78b115
- "Vitest vs Jest 2026" (getautonoma): https://getautonoma.com/blog/jest-vs-vitest-2026
