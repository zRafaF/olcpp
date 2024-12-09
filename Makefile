# Compiler and tools
FLEX = flex
BISON = bison
CC = gcc

# Directories
SRC_DIR = src
BUILD_DIR = build

# Input files
LEX_FILE = $(SRC_DIR)/lexical.l
YACC_FILE = $(SRC_DIR)/parser.y

# Output files
LEX_C = $(BUILD_DIR)/lex.yy.c
YACC_C = $(BUILD_DIR)/parser.c
YACC_H = $(BUILD_DIR)/parser.h

LEXICAL_OBJ = $(BUILD_DIR)/lexical.o
PARSER_OBJ = $(BUILD_DIR)/parser.o

EXECUTABLE = $(BUILD_DIR)/parser_exec

# Rules
all: $(EXECUTABLE)

# Compile lexical analyzer
lexical: $(LEXICAL_OBJ)

$(LEXICAL_OBJ): $(LEX_FILE)
	mkdir -p $(BUILD_DIR)
	$(FLEX) -o $(LEX_C) $(LEX_FILE)

# Compile parser
parse: $(PARSER_OBJ)

$(PARSER_OBJ): $(YACC_FILE)
	mkdir -p $(BUILD_DIR)
	$(BISON) -d -o $(YACC_C) $(YACC_FILE)

# Link the final executable
$(EXECUTABLE): $(LEXICAL_OBJ) $(PARSER_OBJ)
	$(CC) $(LEXICAL_OBJ) $(PARSER_OBJ) -o $(EXECUTABLE)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
