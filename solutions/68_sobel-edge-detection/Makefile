CC      = gcc
CFLAGS  = -Wall -std=c99
TARGET  = sobel
SRC     = sobel.c
LDLIBS  = -lm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
