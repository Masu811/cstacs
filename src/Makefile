CC = gcc
CFLAGS = -g -O3 $(shell xml2-config --cflags)
LIBS = $(shell xml2-config --libs) -lpng -lm

SRCS = importer.c measurement.c
TARGET = measurement

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

