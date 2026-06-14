CC = gcc
CFLAGS = -Wall -g

all: rv_encoder

rv_encoder: rv_encoder.c
	$(CC) $(CFLAGS) $< -o $@

test: rv_encoder
	./rv_encoder

clean:
	rm -f rv_encoder

.PHONY: all test clean
