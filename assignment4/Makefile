CC=gcc

TARGET=main

all: $(TARGET)

$(TARGET): main.c
	$(CC) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

