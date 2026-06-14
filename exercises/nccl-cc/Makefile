CC = gcc
CFLAGS = -Wall -Wno-unused-function -std=c99 -g

SRCS = main.c tokenize.c parse.c codegen_rv.c codegen_arm.c codegen_a64.c codegen_x86.c type.c preprocess.c debug.c
TARGET = nccl-cc

all: $(TARGET)

$(TARGET): $(SRCS) nccl_cc.h
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Each backend runs the SAME suite through test/run_tests.sh:
#   - test/*.c declare expected exit codes via '// expect: N'
#   - test/invalid/*.c must be rejected by the compiler
# The runner exits non-zero on any failure, so CI turns red for real.
test: $(TARGET)
	@echo "=== RISC-V backend (qemu-riscv32, -nostdlib runtime) ==="
	@sh test/run_tests.sh rv

test-arm: $(TARGET)
	@echo "=== ARM backend (qemu-arm, -nostdlib runtime) ==="
	@sh test/run_tests.sh arm

test-a64: $(TARGET)
	@echo "=== AArch64 backend (qemu-aarch64, -nostdlib runtime) ==="
	@sh test/run_tests.sh a64

test-x86: $(TARGET)
	@echo "=== x86-64 backend (native, libc) ==="
	@sh test/run_tests.sh x86

# Bare metal: same RV32 compiler output, no OS underneath.
# Runs the suite on qemu-system-riscv32 (-machine virt): putchar goes
# to the 16550A UART, the exit code travels through the SiFive test
# finisher device. Full-system emulation boots slower than user-mode
# qemu, hence a separate target on top of test-all.
test-bare: $(TARGET)
	@echo "=== RISC-V BARE METAL (qemu-system-riscv32 virt) ==="
	@sh test/run_tests.sh rv-bare

test-all: test test-arm test-a64 test-x86 test-bare

# Differential fuzzing: random programs in the supported C subset,
# every backend's exit code must match a gcc -fwrapv reference build.
# FUZZ_N seeds starting at FUZZ_SEED; failures land in fuzz/failures/.
FUZZ_N = 50
FUZZ_SEED = 1
fuzz: $(TARGET)
	@sh fuzz/run_fuzz.sh $(FUZZ_N) $(FUZZ_SEED)

clean:
	rm -f $(TARGET) *.o /tmp/nccl_test_* /tmp/nccl_fuzz_*

.PHONY: all test test-x86 test-arm test-a64 test-bare test-all fuzz clean
