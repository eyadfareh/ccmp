CC = gcc
CFLAGS = -g
SOURCE_DIR = src/
SRC_FILES = main.c lexer.c parser.c ast.c code_generator.c
SRC_FILES_FULL = $(addprefix $(SOURCE_DIR), $(SRC_FILES))
HEADER_FILES = lexer.h parser.h ast.h code_generator.h
HEADER_FILES_FULL = $(addprefix $(SOURCE_DIR), $(HEADER_FILES))


TARGET = ccmp

all: $(TARGET)

$(TARGET): $(SRC_FILES_FULL) $(HEADER_FILES_FULL)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES_FULL)



$(SOURCE_DIR)ast.c: ast_generator.py
	python3 ./ast_generator.py source > $@

$(SOURCE_DIR)ast.h: ast_generator.py
	python3 ./ast_generator.py header > $@

clean:
	rm -f $(TARGET) $(SOURCE_DIR)ast.c $(SOURCE_DIR)ast.h
