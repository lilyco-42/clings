CC      = gcc
CFLAGS  = -Wall -std=c99
LDFLAGS = -lm
TARGET  = fft
SRC     = fft.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
