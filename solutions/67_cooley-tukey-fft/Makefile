CC      = gcc
CFLAGS  = -Wall -std=c99
LDFLAGS = -lm
TARGET  = fft
SRC     = fft.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt && echo "All tests passed."

clean:
	rm -f $(TARGET)

.PHONY: all test clean
