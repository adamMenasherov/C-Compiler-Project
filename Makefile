# Makefile for Compiler Project

# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra -DPARSER_ONLY
LDFLAGS =

# Build directory structure
BUILD_DIR = .build
OBJ_DIR = $(BUILD_DIR)/obj

# Output executable name
TARGET = compiler

# Source files
# Semantic-only build for now: stop after symbol-table printing and exclude TACKY/ASM generation.
DATA_STRUCTURE_SOURCES = \
	DataStructures/HashTable/HashTable.c \
	DataStructures/HashTable/Wrappers/SymbolTableWrapper.c \
	DataStructures/Map/map.c \
	DataStructures/Map/Wrappers/CharIntMap.c \
	DataStructures/Map/Wrappers/SemanticIdentifierMap.c \
	DataStructures/Map/Wrappers/DoubleStringMap.c
ENTRYPOINT_SOURCES = EntryPoint/FileHandling.c EntryPoint/main.c
LEXER_SOURCES = $(shell find Lexer -type f -name '*.c' | sort)
PARSER_SOURCES = Parser/Parser.c Parser/generateUtils.c $(shell find Parser/AST Parser/Common Parser/TACKY -type f -name '*.c' | sort)
SEMANTIC_SOURCES = $(shell find Semantic -type f -name '*.c' | sort)
SOURCES = $(DATA_STRUCTURE_SOURCES) $(ENTRYPOINT_SOURCES) $(LEXER_SOURCES) $(PARSER_SOURCES) $(SEMANTIC_SOURCES)



# Object files (derived from sources, placed in OBJ_DIR)
OBJECTS = $(SOURCES:%.c=$(OBJ_DIR)/%.o)

# Include directories
INCLUDES = -I.

# Default target
.PHONY: all
all: $(TARGET)

# Create build directories if they don't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Build the executable
$(TARGET): $(OBJ_DIR) $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "✓ Build complete: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run the compiler with a test file
.PHONY: run
run: $(TARGET)
	./$(TARGET) testFiles/test.c

# Run the regression test suite
.PHONY: test
test: $(TARGET)
	python3 scripts/run_compiler_tests.py

# Run with --lex flag to stop at lexical analysis
.PHONY: lex
lex: $(TARGET)
	./$(TARGET) --lex testFiles/test.c

# Run the debugger
.PHONY: debug
debug: $(TARGET)
	gdb ./$(TARGET)

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned build artifacts"

# Full rebuild
.PHONY: rebuild
rebuild: clean all

# Show help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make              - Build the compiler (default)"
	@echo "  make run          - Build and run with test file"
	@echo "  make test         - Build and run the regression test suite"
	@echo "  make lex          - Build and run with --lex flag"
	@echo "  make debug        - Build and run with GDB debugger"
	@echo "  make clean        - Remove build directory"
	@echo "  make rebuild      - Clean and rebuild"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Output structure:"
	@echo "  compiler        - Compiled executable (in root)"
	@echo "  .build/         - Build directory (hidden)"
	@echo "  .build/obj/     - Object files (organized by source structure)"
