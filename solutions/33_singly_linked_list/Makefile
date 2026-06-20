CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_33e clean

test_33e: 33e_array_ops
	@# 验证数组内容（insert 0,1,2,3 位置）
	./33e_array_ops | grep -q "\['a', 'b', 'c', 'd'\]"
	@# 验证长度计算
	./33e_array_ops | grep -q "length: 4"
	@# 验证输出恰好 2 行
	test $$(./33e_array_ops | wc -l) -eq 2
	@echo "PASS: 33e_array_ops"

33e_array_ops: 33e_array_ops.c list.c list.h
	$(CC) $(CFLAGS) 33e_array_ops.c list.c -o 33e_array_ops

clean:
	-rm -f 33e_array_ops
