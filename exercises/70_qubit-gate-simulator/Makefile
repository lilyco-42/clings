# Makefile for 70_qubit-gate-simulator
#
# TODO: Fill in all fields marked with ??? and implement the build rules.
#       完成后删除本 TODO 行。

CC      = ???
CFLAGS  = ???
LDFLAGS = ???
TARGET  = ???
SRC     = ???

all: $(TARGET)

$(TARGET): $(SRC) complex.h
	???

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt && echo "All tests passed."

clean:
	rm -f $(TARGET)

.PHONY: all test clean
