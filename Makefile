CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
LDFLAGS = -pthread

TARGET  = main
SRC     = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
