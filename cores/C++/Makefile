CXX = g++
CXXFLAGS = -g -O3 -std=c++20
LIBS = -lgsl -lgslcblas

TARGET = stacs
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = /usr/include
LIB_DIR = /usr/lib

SRC = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.c)
HEAD = $(wildcard $(SRC_DIR)/*.hpp) $(wildcard $(SRC_DIR)/*.h)

include: $(SRC_DIR)/$(TARGET).hpp $(BUILD_DIR)/lib$(TARGET).so
	sudo install -m 0755 $(BUILD_DIR)/lib$(TARGET).so $(LIB_DIR)
	sudo mkdir -p $(INCLUDE_DIR)/$(TARGET)
	sudo install -m 0644 $(HEAD) $(INCLUDE_DIR)/$(TARGET)
	make clean
	sudo ldconfig
	@echo ""
	@echo "The STACS shared library is now ready for use"

$(BUILD_DIR)/lib$(TARGET).so: $(SRC) $(HEAD)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $@ $(SRC) $(LIBS)

clean:
	rm -rf $(BUILD_DIR)

uninstall:
	sudo rm -rf $(INCLUDE_DIR)/$(TARGET)
	sudo rm -f $(LIB_DIR)/lib$(TARGET).so
	sudo ldconfig

.PHONY: include clean uninstall
