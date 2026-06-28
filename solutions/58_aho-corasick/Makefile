.PHONY: all clean test

all: aho_corasick

aho_corasick: aho_corasick.c
	$(CC) $(CFLAGS) -o $@ $<

test: aho_corasick
	./aho_corasick | diff - expected_output.txt

clean:
	rm -f aho_corasick
