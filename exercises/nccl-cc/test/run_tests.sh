#!/bin/sh
# run_tests.sh - nccl-cc test runner
#
# usage:  sh test/run_tests.sh <rv|rv-bare|arm|a64|x86>   (run from nccl-cc/)
#
# Valid tests (test/*.c) declare their expected exit code in a header:
#     // expect: N
# The runner compiles each test with nccl-cc, assembles/links it with
# the target toolchain, executes it (under qemu for cross targets) and
# compares the actual exit code against the expectation.
#
# Invalid tests (test/invalid/*.c) must make the COMPILER exit non-zero.
#
# Every compile is capped at 10s and every run at 20s (timeout exit
# code 124), so a hung compiler or a runaway binary fails the suite
# instead of wedging CI.
#
# Exit status: 0 when every test passes, 1 otherwise (CI relies on it).
set -u

BACKEND="${1:-rv}"
CC_BIN="./nccl-cc"
TESTDIR="$(dirname "$0")"
OUT="/tmp/nccl_test_$$"

# backend definitions (TARGET_FLAG / RUN / assemble) are shared with
# the differential fuzzer - see test/backends.sh
. "$TESTDIR/backends.sh"
if ! setup_backend "$BACKEND"; then
    echo "usage: $0 <rv|rv-bare|arm|a64|x86>" >&2
    exit 2
fi
if ! backend_tool_ok; then
    echo "error: toolchain for backend '$BACKEND' not installed ($BACKEND_TOOLS)" >&2
    exit 2
fi

if [ ! -x "$CC_BIN" ]; then
    echo "error: $CC_BIN not found - run 'make' first" >&2
    exit 2
fi

pass=0
fail=0

# ---- valid tests: exit code must match the '// expect: N' header ----
for src in "$TESTDIR"/*.c; do
    name=$(basename "$src" .c)
    expect=$(sed -n 's@^// expect: \([0-9][0-9]*\).*@\1@p' "$src" | head -1)
    if [ -z "$expect" ]; then
        echo "SKIP  $name (no '// expect:' header)"
        continue
    fi
    if ! timeout 10 $CC_BIN $TARGET_FLAG "$src" >"$OUT.s" 2>"$OUT.err"; then
        echo "FAIL  $name (compile error)"
        sed 's/^/      /' "$OUT.err" | head -3
        fail=$((fail + 1))
        continue
    fi
    if ! assemble "$OUT.s" "$OUT.bin" 2>"$OUT.err"; then
        echo "FAIL  $name (assemble/link error)"
        sed 's/^/      /' "$OUT.err" | head -3
        fail=$((fail + 1))
        continue
    fi
    timeout 20 $RUN "$OUT.bin" </dev/null >/dev/null 2>&1
    got=$?
    if [ "$got" = "$expect" ]; then
        printf 'ok    %-18s => %s\n' "$name" "$got"
        pass=$((pass + 1))
    else
        printf 'FAIL  %-18s => got %s, expect %s\n' "$name" "$got" "$expect"
        fail=$((fail + 1))
    fi
done

# ---- invalid tests: the compiler itself must reject them ----
# A timeout (rc=124) is reported as a HANG, not as a rejection: an
# infinite parser loop must never be mistaken for a diagnostic.
for src in "$TESTDIR"/invalid/*.c; do
    [ -e "$src" ] || continue
    name="invalid/$(basename "$src" .c)"
    timeout 10 $CC_BIN $TARGET_FLAG "$src" >"$OUT.s" 2>/dev/null
    rc=$?
    if [ "$rc" = 0 ]; then
        printf 'FAIL  %-18s (compiler accepted an invalid program)\n' "$name"
        fail=$((fail + 1))
    elif [ "$rc" = 124 ]; then
        printf 'FAIL  %-18s (compiler HUNG - killed by timeout)\n' "$name"
        fail=$((fail + 1))
    else
        printf 'ok    %-18s (rejected, exit %s)\n' "$name" "$rc"
        pass=$((pass + 1))
    fi
done

rm -f "$OUT".*

echo ""
echo "[$BACKEND] $pass passed, $fail failed"
[ "$fail" -eq 0 ]
