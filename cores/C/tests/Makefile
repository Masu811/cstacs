CC = gcc
CFLAGS = -O3 -Wall -Wextra -Wpedantic
LIBS = -I $(HEADER_DIR) -I/usr/include/libxml2 -lm -lxml2 -lgsl -lgslcblas

TARGET = test
SRC_DIR = ../src
HEADER_DIR = ../include
BUILD_DIR = bin
INCLUDE_DIR = /usr/include
LIB_DIR = /usr/lib
TEST_DIR = .

SRC = $(wildcard $(SRC_DIR)/*.c)
HEAD = $(wildcard $(HEADER_DIR)/*.h)
TEST = $(wildcard $(TEST_DIR)/*.c)

default: $(SRC) $(HEAD) $(TEST)
	mkdir -p $(BUILD_DIR)
	$(CC) -O0 -fsanitize=address,undefined -o $(BUILD_DIR)/$(TARGET) \
	    $(TEST) $(LIBS) -lcriterion

test: default
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
