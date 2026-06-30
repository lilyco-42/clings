# Makefile for 49_dining-philosophers-sync
#
# 多线程程序工程化要点：编译期和链接期都要告诉工具链"这是线程程序"。
#   - 编译期 -pthread：定义 _REENTRANT，让某些 libc 函数走线程安全版本
#   - 链接期 -pthread：链接 pthread 库（现代 glibc 已合入 libc，但仍需此标志）
#
# TODO: Fill in all fields marked with ??? and implement the build rules.
#       完成后删除本 TODO 行。

CC      = ???
CFLAGS  = ???
TARGET  = ???
SRC     = ???

all: $(TARGET)

$(TARGET): $(SRC)
	???

test: $(TARGET)
	???

clean:
	???

.PHONY: all test clean
