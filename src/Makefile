CC = gcc
CFLAGS = -g $(shell xml2-config --cflags)
LIBS = $(shell xml2-config --libs)

SRCS = importer.c
TARGET = importer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

