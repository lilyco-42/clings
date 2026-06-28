.PHONY: all clean 

all: aho_corasick

aho_corasick: aho_corasick.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f aho_corasick
