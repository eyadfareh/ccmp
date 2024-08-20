CC = gcc
CFLAGS = -g -Wall
SRC_FILES = main.c
TARGET = ccmp

all: $(TARGET)
	
$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $^
