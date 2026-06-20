CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = cmd_math.c
CMD = math
LDLIBS = -lm

.PHONY: all test clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD) $(LDLIBS)

test: $(CMD)
	./$(CMD) "2+3*4"
	./$(CMD) "(1+2)*(3+4)"
	./$(CMD) "sqrt(16)+sin(0)"
	./$(CMD) "-(-5)"
	@echo "PASS"

clean:
	-rm -f $(CMD)
