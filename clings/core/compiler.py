"""Async C compilation engine."""

import asyncio
import os
import re
import shutil
import tempfile
import uuid
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class CompileResult:
    success: bool
    binary_path: str | None = None
    binary_id: str | None = None
    stdout: str = ""
    stderr: str = ""
    errors: list[dict] = field(default_factory=list)
    error_count: int = 0
    warning_count: int = 0


@dataclass
class RunResult:
    stdout: str
    stderr: str
    exit_code: int


class CCompiler:
    """Async C compiler wrapper."""

    def __init__(self):
        self.compiler = self._find_compiler()
        self.build_dir = Path(tempfile.gettempdir()) / "clings_build"
        self.build_dir.mkdir(exist_ok=True)

    @staticmethod
    def _sanitize_stem(filename: str) -> str:
        """Return a safe filename stem (no path separators, no traversal)."""
        stem = Path(filename).stem
        stem = re.sub(r"[^A-Za-z0-9_.-]", "_", stem)
        return stem or "exercise"

    def _resolve_binary(self, binary_id: str) -> Path:
        """Resolve a binary reference to a path confined inside the build dir.

        Accepts either an opaque build id (a bare filename) or an absolute
        path, but rejects anything that resolves outside the private build
        directory. This prevents ``/api/run`` from executing arbitrary
        system binaries.
        """
        candidate = Path(binary_id)
        if not candidate.is_absolute():
            candidate = self.build_dir / candidate
        resolved = candidate.resolve()
        if not resolved.is_relative_to(self.build_dir.resolve()):
            raise ValueError("binary reference escapes build directory")
        return resolved

    def _find_compiler(self) -> str | None:
        """Find available C compiler."""
        env_cc = os.environ.get("CC")
        if env_cc and shutil.which(env_cc):
            return env_cc

        for name in ["gcc", "clang", "cc", "gcc.exe", "clang.exe"]:
            found = shutil.which(name)
            if found:
                return found

        # Windows common paths
        if os.name == "nt":
            for path in [
                "D:/app/scoop/apps/llvm/current/bin/clang.exe",
                "C:/msys64/mingw64/bin/gcc.exe",
                "C:/mingw64/bin/gcc.exe",
            ]:
                if Path(path).exists():
                    return path

        return None

    @property
    def available(self) -> bool:
        return self.compiler is not None

    @staticmethod
    def _parse_diagnostics(stderr: str) -> tuple[list[dict], int, int]:
        """Parse GCC/Clang stderr into structured diagnostics.

        Matches lines like:
          file.c:10:5: error: undeclared identifier 'x'
          file.c:10:5: warning: unused variable 'x' [-Wunused-variable]
          file.c:10:5: note: did you mean 'y'?
        Returns (diagnostics, error_count, warning_count).
        """
        # Pattern: file:line:col: severity: message
        pattern = re.compile(
            r'^(.+?):(\d+):(\d+):\s+(error|warning|note|fatal error|related)\s*:\s*(.+)$',
            re.MULTILINE,
        )
        diagnostics = []
        error_count = 0
        warning_count = 0

        for m in pattern.finditer(stderr):
            file, line, col, severity, message = m.groups()
            # Skip temp build dir noise, keep the message
            diag = {
                "file": Path(file).name,
                "line": int(line),
                "col": int(col),
                "severity": severity,
                "message": message.strip(),
            }
            diagnostics.append(diag)
            if severity in ("error", "fatal error"):
                error_count += 1
            elif severity == "warning":
                warning_count += 1

        # Fallback: detect summary lines like "2 errors generated." or "1 warning, 2 errors"
        if not diagnostics:
            summary = re.search(r'(\d+)\s+error', stderr)
            if summary:
                error_count = int(summary.group(1))
            summary = re.search(r'(\d+)\s+warning', stderr)
            if summary:
                warning_count = int(summary.group(1))

        return diagnostics, error_count, warning_count

    async def compile(
        self,
        source: str,
        filename: str = "exercise.c",
        cflags: list[str] | None = None,
    ) -> CompileResult:
        """Compile C source code asynchronously."""
        if not self.available:
            return CompileResult(
                success=False, stderr="No C compiler found. Install gcc or clang."
            )

        if cflags is None:
            cflags = ["-std=c11", "-Wall", "-Wextra", "-pedantic", "-O2"]

        # Write source to a unique, sanitized temp file (the user-supplied
        # filename is reduced to a safe stem to prevent path traversal).
        stem = self._sanitize_stem(filename)
        build_id = uuid.uuid4().hex
        src_file = self.build_dir / f"{build_id}_{stem}.c"
        src_file.write_text(source, encoding="utf-8")

        # Output binary
        suffix = ".exe" if os.name == "nt" else ""
        binary = self.build_dir / f"{build_id}_{stem}{suffix}"

        cmd = [self.compiler] + cflags + [str(src_file), "-o", str(binary)]

        try:
            proc = await asyncio.create_subprocess_exec(
                *cmd,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.PIPE,
            )
            stdout, stderr = await proc.communicate()

            stderr_str = stderr.decode("utf-8", errors="replace")
            stdout_str = stdout.decode("utf-8", errors="replace")
            diagnostics, error_count, warning_count = self._parse_diagnostics(stderr_str)

            if proc.returncode == 0:
                return CompileResult(
                    success=True,
                    binary_path=str(binary),
                    binary_id=binary.name,
                    stdout=stdout_str,
                    stderr=stderr_str,
                    errors=diagnostics,
                    error_count=error_count,
                    warning_count=warning_count,
                )
            else:
                return CompileResult(
                    success=False,
                    stdout=stdout_str,
                    stderr=stderr_str,
                    errors=diagnostics,
                    error_count=error_count,
                    warning_count=warning_count,
                )
        except Exception as e:
            return CompileResult(success=False, stderr=str(e))

    async def run(
        self,
        binary_id: str,
        stdin: str = "",
        timeout: float = 5.0,
        args: list[str] | None = None,
    ) -> RunResult:
        """Run a previously compiled binary by opaque build id.

        The id is resolved against the private build directory and rejected
        if it escapes it, so arbitrary system executables cannot be launched.
        """
        try:
            binary = self._resolve_binary(binary_id)
        except ValueError as e:
            return RunResult(stdout="", stderr=str(e), exit_code=-1)

        if not binary.is_file():
            return RunResult(stdout="", stderr=f"Binary not found: {binary_id}", exit_code=-1)

        cmd = [str(binary)] + (args or [])

        try:
            proc = await asyncio.create_subprocess_exec(
                *cmd,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.PIPE,
                stdin=asyncio.subprocess.PIPE,
            )
            stdout, stderr = await asyncio.wait_for(
                proc.communicate(input=stdin.encode("utf-8") if stdin else None),
                timeout=timeout,
            )
            return RunResult(
                stdout=stdout.decode("utf-8", errors="replace"),
                stderr=stderr.decode("utf-8", errors="replace"),
                exit_code=proc.returncode or 0,
            )
        except asyncio.TimeoutError:
            proc.kill()
            return RunResult(stdout="", stderr=f"Timeout after {timeout}s", exit_code=-1)
        except Exception as e:
            return RunResult(stdout="", stderr=str(e), exit_code=-1)

    async def compile_and_run(
        self,
        source: str,
        filename: str = "exercise.c",
        stdin: str = "",
        timeout: float = 5.0,
    ) -> dict:
        """Compile and run in one call."""
        compile_result = await self.compile(source, filename)
        if not compile_result.success:
            return {
                "success": False,
                "compile_errors": compile_result.stderr,
                "stdout": "",
                "stderr": compile_result.stderr,
                "exit_code": -1,
                "errors": compile_result.errors,
                "error_count": compile_result.error_count,
                "warning_count": compile_result.warning_count,
            }

        run_result = await self.run(compile_result.binary_id, stdin, timeout)
        return {
            "success": run_result.exit_code == 0,
            "compile_errors": "",
            "stdout": run_result.stdout,
            "stderr": run_result.stderr,
            "exit_code": run_result.exit_code,
            "errors": [],
            "error_count": 0,
            "warning_count": 0,
        }
