CC = gcc
CFLAGS = -Wall -Wextra
TARGET = xxds
SOURCE = xxds.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

clean:
	rm -f $(TARGET)

.PHONY: clean install

