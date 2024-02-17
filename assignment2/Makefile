CC=gcc

TARGET=schedule

all: $(TARGET)

$(TARGET): schedule.c
	$(CC) schedule.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

