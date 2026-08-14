"""Compile and run routes."""

from fastapi import APIRouter
from pydantic import BaseModel
from clings.api import compiler, state

router = APIRouter()


class CompileRequest(BaseModel):
    source: str
    filename: str = "exercise.c"
    cflags: list[str] | None = None


class RunRequest(BaseModel):
    binary_id: str
    stdin: str = ""
    timeout: float = 5.0
    args: list[str] | None = None


class CompileRunRequest(BaseModel):
    source: str
    filename: str = "exercise.c"
    stdin: str = ""
    timeout: float = 5.0


@router.post("/compile")
async def compile_code(req: CompileRequest):
    """Compile C source code."""
    result = await compiler.compile(req.source, req.filename, req.cflags)
    return {
        "success": result.success,
        "binary_id": result.binary_id,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "errors": result.errors,
        "error_count": result.error_count,
        "warning_count": result.warning_count,
    }


@router.post("/run")
async def run_code(req: RunRequest):
    """Run compiled binary by opaque build id."""
    result = await compiler.run(req.binary_id, req.stdin, req.timeout, req.args)
    return {
        "stdout": result.stdout,
        "stderr": result.stderr,
        "exit_code": result.exit_code,
    }


@router.post("/compile-and-run")
async def compile_and_run(req: CompileRunRequest):
    """Compile and run in one call."""
    result = await compiler.compile_and_run(req.source, req.filename, req.stdin, req.timeout)
    return result


@router.post("/verify/{exercise_name}")
async def verify_exercise(exercise_name: str, req: CompileRunRequest):
    """Compile, run, and verify against expected return code or stdout test cases."""
    from clings.api import config

    exercise = config.get_exercise(exercise_name)
    if not exercise:
        return {"success": False, "error": f"Exercise '{exercise_name}' not found"}

    mode = exercise.get("mode", "return")
    cases = exercise.get("cases", [])

    # stdout mode: run against test cases
    if mode == "stdout" and cases:
        # First compile once
        compile_result = await compiler.compile(req.source, req.filename)
        if not compile_result.success:
            return {
                "success": False,
                "compile_errors": compile_result.stderr,
                "stdout": "",
                "stderr": compile_result.stderr,
                "exit_code": -1,
                "verified": False,
                "case_results": [],
            }

        case_results = []
        all_passed = True

        for i, case in enumerate(cases):
            run_result = await compiler.run(
                compile_result.binary_id,
                stdin=case.get("stdin", ""),
                timeout=req.timeout,
            )
            actual = run_result.stdout
            expected = case.get("stdout", "")
            passed = actual.strip() == expected.strip()
            if not passed:
                all_passed = False
            case_results.append({
                "case": i + 1,
                "stdin": case.get("stdin", ""),
                "expected": expected,
                "actual": actual,
                "passed": passed,
            })

        # Build combined output
        stdout_parts = []
        stderr_parts = []
        for cr in case_results:
            status = "PASS" if cr["passed"] else "FAIL"
            stdout_parts.append(f"Case {cr['case']} [{status}]:")
            if cr["stdin"]:
                stdin_display = cr["stdin"].strip()
                stdout_parts.append(f"  stdin: {stdin_display}")
            stdout_parts.append(f"  expected: {cr['expected'].strip()}")
            stdout_parts.append(f"  actual:   {cr['actual'].strip()}")
            stdout_parts.append("")

        result = {
            "success": all_passed,
            "compile_errors": "",
            "stdout": "\n".join(stdout_parts),
            "stderr": compile_result.stderr if not all_passed else "",
            "exit_code": 0 if all_passed else -1,
            "verified": all_passed,
            "case_results": case_results,
        }

        if all_passed:
            state.mark_passed(exercise_name)
        else:
            state.mark_failed(exercise_name, "test cases failed")

        return result

    # return mode: check exit code
    result = await compiler.compile_and_run(req.source, req.filename, req.stdin, req.timeout)

    if mode == "return" and exercise.get("expected_return") is not None:
        expected = exercise["expected_return"]
        actual = result.get("exit_code", -1)
        result["verified"] = actual == expected
        result["expected"] = expected
    else:
        result["verified"] = result.get("success", False)

    # Update state
    if result.get("verified"):
        state.mark_passed(exercise_name)
    else:
        state.mark_failed(exercise_name, result.get("stderr", ""))

    return result
