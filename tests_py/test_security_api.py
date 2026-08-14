"""API 安全测试：Host/Origin 校验 + per-launch token + binary 路径隔离。

覆盖 issue #1 的核心攻击面：
- DNS rebinding（非回环 Host）→ 403
- 无 token / 错 token 的写请求 → 401
- 跨域 Origin 的写请求 → 403
- 正确 token + 回环 Host → 放行
- /api/run 不能执行任意系统二进制
"""

from starlette.testclient import TestClient

from clings.api import app
from clings.security import TOKEN_HEADER, security


def client() -> TestClient:
    return TestClient(app, base_url="http://127.0.0.1:3000")


def test_index_contains_token() -> None:
    """首页注入本次启动的 token 供前端 JS 读取。"""
    r = client().get("/")
    assert r.status_code == 200
    assert security.token in r.text


def test_non_loopback_host_rejected() -> None:
    """DNS rebinding：Host 是攻击者域名 → 403。"""
    r = client().get("/", headers={"Host": "rebind.test:3000"})
    assert r.status_code == 403


def test_post_without_token_rejected() -> None:
    r = client().post("/api/compile", json={"source": "int main(void){return 0;}"})
    assert r.status_code == 401


def test_post_wrong_token_rejected() -> None:
    r = client().post(
        "/api/compile",
        headers={TOKEN_HEADER: "wrong-token"},
        json={"source": "x"},
    )
    assert r.status_code == 401


def test_post_foreign_origin_rejected() -> None:
    """跨站请求（Origin 非回环）→ 403，即使带 token。"""
    r = client().post(
        "/api/compile",
        headers={TOKEN_HEADER: security.token, "Origin": "http://evil.com"},
        json={"source": "x"},
    )
    assert r.status_code == 403


def test_post_with_token_and_loopback_origin_allowed() -> None:
    """合法同源请求（回环 Host + 回环 Origin + token）→ 放行。"""
    r = client().post(
        "/api/compile",
        headers={
            TOKEN_HEADER: security.token,
            "Origin": "http://127.0.0.1:3000",
        },
        json={"source": "int main(void){return 0;}"},
    )
    assert r.status_code == 200


def test_get_routes_need_no_token() -> None:
    """只读路由不需要 token（读不到敏感数据）。"""
    r = client().get("/api/health")
    assert r.status_code == 200


def test_run_rejects_system_binary() -> None:
    """/api/run 不能执行任意系统二进制（如 /bin/echo）。"""
    r = client().post(
        "/api/run",
        headers={TOKEN_HEADER: security.token},
        json={"binary_id": "/bin/echo", "args": ["MARKER"]},
    )
    assert r.status_code == 200
    body = r.json()
    assert body["exit_code"] == -1
    assert "escapes" in body["stderr"]


def test_run_rejects_path_traversal() -> None:
    """../ 穿越到 build 目录之外 → 拒绝。"""
    r = client().post(
        "/api/run",
        headers={TOKEN_HEADER: security.token},
        json={"binary_id": "../../etc/passwd"},
    )
    body = r.json()
    assert body["exit_code"] == -1
    assert "escapes" in body["stderr"]
