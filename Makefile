# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Directories
GEN_DIR = gen
BUILD_DIR = build

# Source files
LEX_FILE = src/lexical.l
PARSER_FILE = src/parser.y

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

$(OUTPUT): $(PARSER_TAB_C) $(LEX_OUTPUT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(PARSER_TAB_C) $(LEX_OUTPUT) -lfl

$(GEN_DIR)/lex.yy.c: $(LEX_FILE)
	# @rm -f $(GEN_DIR)  # Remove if it's a file, not a directory
	@mkdir -p $(GEN_DIR)
	flex -o $(GEN_DIR)/lex.yy.c $(LEX_FILE)

$(GEN_DIR)/parser.tab.c $(GEN_DIR)/parser.tab.h: $(PARSER_FILE)
	@rm -f $(GEN_DIR)  # Remove if it's a file, not a directory
	@mkdir -p $(GEN_DIR)
	bison -d -o $(GEN_DIR)/parser.tab.c $(PARSER_FILE)

lexical: $(LEX_OUTPUT)

parser: $(PARSER_TAB_C) $(PARSER_TAB_H)

# Run target that builds and runs the program with a given file
run: $(OUTPUT)
	$(BUILD_DIR)/parser < $(PATH)

# Clean target
clean:
	rm -rf $(GEN_DIR) $(BUILD_DIR)/parser
