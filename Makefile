# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -I$(SRC_DIR)

# Directories
GEN_DIR = gen
BUILD_DIR = build
SRC_DIR = src

# Source files (including all .cpp files in the src directory)
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Lexical and parser specific files
LEX_FILE = $(SRC_DIR)/lexical.l
PARSER_FILE = $(SRC_DIR)/parser.y

# Output files
LEX_OUTPUT = $(GEN_DIR)/lex.yy.cpp
PARSER_TAB_CPP = $(GEN_DIR)/parser.tab.cpp
PARSER_TAB_HPP = $(GEN_DIR)/parser.tab.hpp

# Final output
OUTPUT = $(BUILD_DIR)/parser

# Input files
INPUT := examples/code1.olc

# Intermediate Representation output
IR_OUTPUT := ir_output/code1.json

# Echo colors
COLOR_GREEN=\033[0;32m
COLOR_RED=\033[0;31m
COLOR_BLUE=\033[0;34m
END_COLOR=\033[0m

# Phony targets (to ensure always run)
.PHONY: all lexical parser clean run

# Targets
all: $(OUTPUT)

$(OUTPUT): $(PARSER_TAB_CPP) $(LEX_OUTPUT) $(SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	# Shows the command being run
	@echo $(CXX) $(CXXFLAGS) -o $@ $(PARSER_TAB_CPP) $(LEX_OUTPUT) $(SRC_FILES) -lfl
	$(CXX) $(CXXFLAGS) -o $@ $(PARSER_TAB_CPP) $(LEX_OUTPUT) $(SRC_FILES) -lfl

$(GEN_DIR)/lex.yy.cpp: $(LEX_FILE)
	@mkdir -p $(GEN_DIR)
	flex -o $(GEN_DIR)/lex.yy.cpp $(LEX_FILE)

$(GEN_DIR)/parser.tab.cpp $(PARSER_TAB_HPP): $(PARSER_FILE)
	@mkdir -p $(GEN_DIR)
	bison -d -o $(PARSER_TAB_CPP) $(PARSER_FILE)

lexical: $(LEX_OUTPUT)

parser: $(PARSER_TAB_CPP) $(PARSER_TAB_HPP)

# Run target that builds and runs the program with a given file
run: $(OUTPUT)
	$(BUILD_DIR)/parser ${INPUT} ${IR_OUTPUT}
	@echo "$(COLOR_GREEN)Output written to ${IR_OUTPUT}$(END_COLOR)"

# Clean target
clean:
	rm -rf $(GEN_DIR) $(BUILD_DIR)
	# @echo "$(COLOR_GREEN)Removed gen and build directories$(END_COLOR)"
