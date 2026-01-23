CC = gcc
CFLAGS = -O3
LIBS = -lstacs

TARGET = eval
SRC = eval.c
BUILD_DIR = bin

default: eval.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBS)

run: default
	./$(BUILD_DIR)/$(TARGET)
