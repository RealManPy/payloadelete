CC = gcc
CFLAGS = -Wall -O2
TARGET = payload

all: $(TARGET)

$(TARGET): payload.c
	$(CC) $(CFLAGS) -o $(TARGET) payload.c

clean:
	rm -f $(TARGET)

