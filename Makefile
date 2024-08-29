CC = gcc
CFLAGS = -g
SRC_FILES = main.c lexer.c parser.c ast.c
HEADER_FILES = lexer.h parser.h ast.h
TARGET = ccmp

all: $(TARGET)

$(TARGET): $(SRC_FILES) $(HEADER_FILES)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES)



ast.c: ast_generator.py
	python3 ./ast_generator.py source > $@

ast.h: ast_generator.py
	python3 ./ast_generator.py header > $@

#ast_generator: ast_generator.c
#	$(CC) $(CFLAGS) -o $@ ast_generator.c

clean:
	rm -f $(TARGET) ast.c ast.h
