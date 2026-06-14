#!/bin/sh
# run_fuzz.sh - differential fuzzing driver for nccl-cc
#
# usage:  sh fuzz/run_fuzz.sh [iterations] [start-seed]   (from nccl-cc/)
#         make fuzz [FUZZ_N=100] [FUZZ_SEED=1]
#
# For every seed:
#   1. fuzz/gen_prog.c emits a random program in the supported C subset
#      (UB designed out - see the generator header for the rules)
#   2. the host gcc builds the REFERENCE binary with -fwrapv so signed
#      overflow wraps exactly like the machine ops our backends emit
#   3. every available nccl-cc backend compiles and runs the same
#      program; exit codes must agree with the reference
#
# A mismatch saves the failing program to fuzz/failures/seed_<N>.c and
# the suite exits non-zero. Backends whose toolchain is missing are
# skipped with a notice (CI images have all of them).
set -u

N="${1:-50}"
START="${2:-1}"
FUZZDIR="$(dirname "$0")"
CC_BIN="./nccl-cc"
GEN="/tmp/nccl_fuzz_gen_$$"
SRC="/tmp/nccl_fuzz_$$.c"
REF="/tmp/nccl_fuzz_ref_$$"
OUT="/tmp/nccl_fuzz_out_$$"

# shared backend definitions (TARGET_FLAG / RUN / assemble)
. "$FUZZDIR/../test/backends.sh"

BACKENDS="rv arm a64 x86"

if [ ! -x "$CC_BIN" ]; then
    echo "error: $CC_BIN not found - run 'make' first" >&2
    exit 2
fi
if ! cc -O1 -o "$GEN" "$FUZZDIR/gen_prog.c"; then
    echo "error: cannot build the program generator" >&2
    exit 2
fi

cleanup() { rm -f "$GEN" "$SRC" "$REF" "$OUT".*; }
trap cleanup EXIT

# which backends can this machine actually run?
ACTIVE=""
for be in $BACKENDS; do
    setup_backend "$be"
    if backend_tool_ok; then
        ACTIVE="$ACTIVE $be"
    else
        echo "note: skipping backend '$be' (toolchain not installed)"
    fi
done
if [ -z "$ACTIVE" ]; then
    echo "error: no usable backend toolchains" >&2
    exit 2
fi

echo "fuzzing seeds $START..$((START + N - 1)) across:$ACTIVE"
fails=0
i="$START"
while [ "$i" -lt "$((START + N))" ]; do
    "$GEN" "$i" > "$SRC"

    # reference: host gcc with wrap-around signed arithmetic
    if ! gcc -fwrapv -w -o "$REF" "$SRC" 2>/dev/null; then
        echo "seed $i: REFERENCE gcc rejected the program (generator bug)"
        fails=$((fails + 1))
        i=$((i + 1))
        continue
    fi
    timeout 10 "$REF" >/dev/null 2>&1
    ref=$?

    for be in $ACTIVE; do
        setup_backend "$be"
        if ! timeout 10 $CC_BIN $TARGET_FLAG "$SRC" >"$OUT.s" 2>/dev/null; then
            echo "seed $i [$be]: nccl-cc failed to compile"
            fails=$((fails + 1))
            mkdir -p "$FUZZDIR/failures"
            cp "$SRC" "$FUZZDIR/failures/seed_$i.c"
            continue
        fi
        if ! assemble "$OUT.s" "$OUT.bin" 2>/dev/null; then
            echo "seed $i [$be]: assemble/link failed"
            fails=$((fails + 1))
            mkdir -p "$FUZZDIR/failures"
            cp "$SRC" "$FUZZDIR/failures/seed_$i.c"
            continue
        fi
        timeout 20 $RUN "$OUT.bin" </dev/null >/dev/null 2>&1
        got=$?
        if [ "$got" != "$ref" ]; then
            echo "seed $i [$be]: MISMATCH got $got, reference $ref"
            fails=$((fails + 1))
            mkdir -p "$FUZZDIR/failures"
            cp "$SRC" "$FUZZDIR/failures/seed_$i.c"
        fi
    done
    i=$((i + 1))
done

echo ""
if [ "$fails" -eq 0 ]; then
    echo "fuzz: $N seeds x$ACTIVE backends, all match the gcc reference"
else
    echo "fuzz: $fails mismatch(es) - failing programs saved to fuzz/failures/"
fi
[ "$fails" -eq 0 ]
