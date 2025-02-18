# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -Wall -I$(SRC_DIR)
CXXFLAGS = -Wall -I$(SRC_DIR) -std=c++20

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
GENERATOR_OUTPUT = $(BUILD_DIR)/generator

# Input files
INPUT := examples/code1.olc

# Intermediate Representation output
IR_OUTPUT := ir_output/code1.json

# Assembly output
ASM_OUTPUT := ir_output/code1.rap

# Echo colors
COLOR_GREEN=\033[0;32m
COLOR_RED=\033[0;31m
COLOR_BLUE=\033[0;34m
END_COLOR=\033[0m


# Phony targets (to ensure always run)
.PHONY: all lexical parser clean run

# Targets
all: $(OUTPUT) $(GENERATOR_OUTPUT)

$(OUTPUT): $(PARSER_TAB_C) $(LEX_OUTPUT) $(SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	# Shows the command being run
	@echo $(CC) $(CFLAGS) -o $@ $(PARSER_TAB_C) $(LEX_OUTPUT) $(SRC_FILES) -lfl
	$(CC) $(CFLAGS) -o $@ $(PARSER_TAB_C) $(LEX_OUTPUT) $(SRC_FILES) -lfl

$(GENERATOR_OUTPUT): $(SRC_DIR)/semantic/main.cpp
	@mkdir -p $(BUILD_DIR)
	@echo $(CXX) $(CXXFLAGS) -o $@ $<
	$(CXX) $(CXXFLAGS) -o $@ $<

$(GEN_DIR)/lex.yy.c: $(LEX_FILE)
	@mkdir -p $(GEN_DIR)
	flex -o $(GEN_DIR)/lex.yy.c $(LEX_FILE)

$(GEN_DIR)/parser.tab.c $(GEN_DIR)/parser.tab.h: $(PARSER_FILE)
	@mkdir -p $(GEN_DIR)
	bison -d -o $(GEN_DIR)/parser.tab.c $(PARSER_FILE)

lexical: $(LEX_OUTPUT)

parser: $(PARSER_TAB_C) $(PARSER_TAB_H)

# Run target that builds and runs the program with a given file
run: $(OUTPUT) $(GENERATOR_OUTPUT)
	$(BUILD_DIR)/parser ${INPUT} ${IR_OUTPUT}
	@echo "$(COLOR_GREEN)Output written to ${IR_OUTPUT}$(END_COLOR)"
	$(GENERATOR_OUTPUT) ${IR_OUTPUT} ${ASM_OUTPUT}
	@echo "$(COLOR_GREEN)Assembly code generated at ${ASM_OUTPUT}$(END_COLOR)"

# Clean target
clean:
	rm -rf $(GEN_DIR) $(BUILD_DIR)
	# @echo "$(COLOR_GREEN)Removed gen and build directories$(END_COLOR)"