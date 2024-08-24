CC = gcc
CFLAGS = -g -Wall
SRC_FILES = main.c lexer.c parser.c
HEADER_FILES = lexer.h parser.h
TARGET = ccmp

all: $(TARGET)

$(TARGET): $(SRC_FILES) $(HEADER_FILES)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES)
