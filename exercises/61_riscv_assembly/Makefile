CC = riscv64-linux-gnu-gcc
CFLAGS = -march=rv32im -mabi=ilp32 -nostdlib -static
QEMU = qemu-riscv32

all: return42 sum100 fact

return42: return42.s
	$(CC) $(CFLAGS) $< -o $@

sum100: sum100.s
	$(CC) $(CFLAGS) $< -o $@

fact: fact.s
	$(CC) $(CFLAGS) $< -o $@

test: all
	@echo "=== return42 ==="
	@$(QEMU) ./return42; echo "exit: $$?"
	@echo "=== sum100 ==="
	@$(QEMU) ./sum100; echo "exit: $$?"
	@echo "=== fact ==="
	@$(QEMU) ./fact; echo "exit: $$?"

clean:
	rm -f return42 sum100 fact

.PHONY: all test clean
