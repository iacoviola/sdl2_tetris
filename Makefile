SRC_DIR = src/
OBJ_DIR = obj/
BUILD_DIR = build/

# Path: src/
# wildcard is used to get all the files with .cpp extension
SRC = $(wildcard $(SRC_DIR)*.cpp)

# Path: obj/
# % for each file in SRC, replace the path with the path to the obj directory
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

# Path: build/
BIN = $(BUILD_DIR)main

# Compiler
CC = g++

# Flags
CFLAGS = -Wall -std=c++17 -g

# Libraries
LIBS = -lSDL2

# Includes
INCLUDE_PATHS =

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	INCLUDE_PATHS += -I/opt/homebrew/include/
endif

# Build
all: $(BIN)

# $@ = target (BIN), $^ = all dependencies (OBJ)
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o $@ $^ $(LIBS)

# $< = first dependency
# % for each cpp file in SRC, create a corresponding .o file in OBJ
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

# Clean
clean:
	rm -f $(OBJ_DIR)*.o $(BIN)