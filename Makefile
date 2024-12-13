# Compiler and flags
CC = gcc
CFLAGS = -Wall -I$(SRC_DIR)

# Directories
GEN_DIR = gen
BUILD_DIR = build
SRC_DIR = src

# Source files (including all .c files in the src directory)
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Lexical and parser specific files
LEX_FILE = $(SRC_DIR)/lexical.l
PARSER_FILE = $(SRC_DIR)/parser.y

# Output files
LEX_OUTPUT = $(GEN_DIR)/lex.yy.c
PARSER_TAB_C = $(GEN_DIR)/parser.tab.c
PARSER_TAB_H = $(GEN_DIR)/parser.tab.h

# Final output
OUTPUT = $(BUILD_DIR)/parser

# Phony targets (to ensure always run)
.PHONY: all lexical parser clean run

# Targets
all: $(OUTPUT)

$(OUTPUT): $(PARSER_TAB_C) $(LEX_OUTPUT) $(SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	# Shows the command being run
	@echo $(CC) $(CFLAGS) -o $@ $(PARSER_TAB_C) $(LEX_OUTPUT) $(SRC_FILES) -lfl
	$(CC) $(CFLAGS) -o $@ $(PARSER_TAB_C) $(LEX_OUTPUT) $(SRC_FILES) -lfl

$(GEN_DIR)/lex.yy.c: $(LEX_FILE)
	@mkdir -p $(GEN_DIR)
	flex -o $(GEN_DIR)/lex.yy.c $(LEX_FILE)

$(GEN_DIR)/parser.tab.c $(GEN_DIR)/parser.tab.h: $(PARSER_FILE)
	@mkdir -p $(GEN_DIR)
	bison -d -o $(GEN_DIR)/parser.tab.c $(PARSER_FILE)

lexical: $(LEX_OUTPUT)

parser: $(PARSER_TAB_C) $(PARSER_TAB_H)

# Run target that builds and runs the program with a given file
run: $(OUTPUT)
	$(BUILD_DIR)/parser examples/code1.olc intermediate.ir

# Clean target
clean:
	rm -rf $(GEN_DIR) $(BUILD_DIR)/parser
