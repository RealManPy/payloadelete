# Makefile
CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -O2
TARGET = payload.exe
SRC = payload.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

