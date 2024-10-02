CC = gcc
CFLAGS = -g -O3
LIBS = -I $(HEADER_DIR) -I/usr/include/libxml2 -lm -lxml2 -lgsl -lgslcblas

TARGET = stacs
SRC_DIR = src
HEADER_DIR = include
BUILD_DIR = build
INCLUDE_DIR = /usr/include
LIB_DIR = /usr/lib

SRC = $(wildcard $(SRC_DIR)/*.c)
HEAD = $(wildcard $(HEADER_DIR)/*.h)

include: $(HEADER_DIR)/$(TARGET).h $(BUILD_DIR)/lib$(TARGET).so
	sudo install -m 0755 $(BUILD_DIR)/lib$(TARGET).so $(LIB_DIR)
	sudo mkdir -p $(INCLUDE_DIR)/$(TARGET)
	sudo install -m 0644 $(HEAD) $(INCLUDE_DIR)/$(TARGET)
	make clean
	@echo "\nThe STACS shared library is now ready for use"

$(BUILD_DIR)/lib$(TARGET).so: $(SRC) $(HEAD)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $(SRC) $(LIBS)

clean:
	rm -rf $(BUILD_DIR)

uninstall:
	sudo rm -rf $(INCLUDE_DIR)/$(TARGET)
	sudo rm -f $(LIB_DIR)/lib$(TARGET).so

.PHONY: include clean uninstall