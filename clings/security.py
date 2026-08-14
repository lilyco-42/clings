"""Security hardening for the Clings loopback API.

The Clings service binds to ``127.0.0.1`` and compiles/runs arbitrary C code.
Binding to loopback alone is insufficient: a malicious website can use DNS
rebinding to make the browser treat a request to the attacker's hostname as
same-origin while it actually lands on ``127.0.0.1:3000``.

This module defends the service with three layers:

1. **Loopback ``Host`` validation** — every request must carry a ``Host``
   header naming a loopback address. DNS rebinding preserves the attacker's
   hostname in the ``Host`` header, so the request is rejected before any
   handler runs.
2. **Loopback ``Origin`` validation** — state-changing requests that carry an
   ``Origin`` header are rejected unless that origin is loopback, blocking
   cross-site form/fetch attacks.
3. **Per-launch token** — every state-changing ``/api/*`` request must present
   a cryptographically random token in the ``X-Clings-Token`` header. The
   token is only handed to the page served from the local origin, so a remote
   page can never learn it.
"""

from __future__ import annotations

import ipaddress
import secrets
from urllib.parse import urlsplit

from starlette.middleware.base import BaseHTTPMiddleware
from starlette.requests import Request
from starlette.responses import JSONResponse

TOKEN_HEADER = "X-Clings-Token"

_LOOPBACK_NAMES = {"localhost", "127.0.0.1", "::1", "[::1]"}

_STATE_CHANGING_METHODS = {"POST", "PUT", "PATCH", "DELETE"}


def _strip_port(host: str) -> str:
    """Return the host portion of a possibly port-qualified host string."""
    host = host.strip().lower()
    if host.startswith("["):
        # IPv6 bracket notation: "[::1]:3000"
        return host[1:].split("]", 1)[0]
    if host.count(":") == 1:
        # "127.0.0.1:3000" / "localhost:3000"
        return host.rsplit(":", 1)[0]
    return host


def is_loopback_host(host: str | None) -> bool:
    """Return True if ``host`` (with or without a port) is a loopback address."""
    if not host:
        return False
    name = _strip_port(host)
    if name in _LOOPBACK_NAMES:
        return True
    try:
        return ipaddress.ip_address(name).is_loopback
    except ValueError:
        return False


def is_loopback_origin(origin: str | None) -> bool:
    """Return True if the ``Origin`` header's host is a loopback address."""
    if not origin:
        return False
    try:
        host = urlsplit(origin).hostname
    except ValueError:
        return False
    return host is not None and is_loopback_host(host)


def generate_token() -> str:
    """Return a cryptographically random URL-safe token."""
    return secrets.token_urlsafe(32)


class ClingsSecurity:
    """Holds the per-launch token and compares candidate tokens safely."""

    def __init__(self) -> None:
        self.token = generate_token()

    def check(self, token: str | None) -> bool:
        if not token:
            return False
        return secrets.compare_digest(token, self.token)


security = ClingsSecurity()


class SecurityMiddleware(BaseHTTPMiddleware):
    """Enforce loopback Host/Origin and the per-launch token."""

    def __init__(self, app, security_obj: ClingsSecurity) -> None:
        super().__init__(app)
        self._security = security_obj

    async def dispatch(self, request: Request, call_next):
        host = request.headers.get("host")
        if not is_loopback_host(host):
            return JSONResponse({"detail": "Forbidden"}, status_code=403)

        path = request.url.path
        if request.method in _STATE_CHANGING_METHODS and path.startswith("/api/"):
            origin = request.headers.get("origin")
            if origin and not is_loopback_origin(origin):
                return JSONResponse({"detail": "Forbidden"}, status_code=403)

            if not self._security.check(request.headers.get(TOKEN_HEADER)):
                return JSONResponse({"detail": "Unauthorized"}, status_code=401)

        return await call_next(request)
