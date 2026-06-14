# backends.sh - shared backend definitions for the test runner and the
# differential fuzzer. POSIX sh, meant to be sourced from nccl-cc/:
#
#     . test/backends.sh
#     setup_backend rv || exit 2
#
# setup_backend defines three things for the chosen backend:
#     TARGET_FLAG   nccl-cc command-line flag selecting the backend
#     RUN           command prefix that executes a produced binary
#     assemble()    assembles+links $1 (asm) into $2 (binary)
#
# backend_tool_ok() reports whether the toolchain/emulator for the
# CURRENT backend is installed (lets callers skip, not fail, on
# machines without every cross toolchain).

setup_backend() {
    case "$1" in
    rv)
        TARGET_FLAG=""
        RUN="qemu-riscv32"
        BACKEND_TOOLS="riscv64-linux-gnu-gcc qemu-riscv32"
        assemble() {
            riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static \
                "$1" runtime_rv_io.s runtime_rv.s -o "$2"
        }
        ;;
    rv-bare)
        # Same compiler output, but linked for NO operating system:
        # platform layer = UART putchar + test-finisher exit, fixed
        # load address 0x80000000 (see bare_rv.ld). qemu-system
        # propagates the finisher status as its own exit code, so
        # '// expect: N' headers work unchanged.
        TARGET_FLAG=""
        RUN="qemu-system-riscv32 -machine virt -nographic -bios none -kernel"
        BACKEND_TOOLS="riscv64-linux-gnu-gcc qemu-system-riscv32"
        assemble() {
            riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static \
                -Wl,--build-id=none -T bare_rv.ld \
                "$1" runtime_rv_io.s runtime_rv_bare.s -o "$2"
        }
        ;;
    arm)
        TARGET_FLAG="-target=arm"
        RUN="qemu-arm"
        BACKEND_TOOLS="arm-linux-gnueabihf-gcc qemu-arm"
        assemble() {
            arm-linux-gnueabihf-gcc -marm -nostdlib -static \
                "$1" runtime_arm.s -o "$2"
        }
        ;;
    a64)
        TARGET_FLAG="-target=a64"
        RUN="qemu-aarch64"
        BACKEND_TOOLS="aarch64-linux-gnu-gcc qemu-aarch64"
        assemble() {
            aarch64-linux-gnu-gcc -nostdlib -static \
                "$1" runtime_a64.s -o "$2"
        }
        ;;
    x86)
        TARGET_FLAG="-target=x86"
        RUN=""
        BACKEND_TOOLS="cc"
        assemble() {
            cc "$1" -o "$2"   # native: link against libc (real printf)
        }
        ;;
    *)
        return 1
        ;;
    esac
    return 0
}

# True when every tool of the current backend is on PATH.
backend_tool_ok() {
    for t in $BACKEND_TOOLS; do
        command -v "$t" >/dev/null 2>&1 || return 1
    done
    return 0
}
