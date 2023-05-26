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
LIBS = -lSDL2 -lSDL2_ttf

# Includes
INCLUDE_PATHS =

# Library paths
LIBRARY_PATHS =

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	INCLUDE_PATHS += -I/opt/homebrew/include/
endif

ifeq ($(OS),Windows_NT)
	#CFLAGS += -Wl,-subsystem,windows
	INCLUDE_PATHS += -IC:\SDL2\SDL2-2.26.5\x86_64-w64-mingw32\include -IC:\SDL2\SDL2_ttf-2.20.2\x86_64-w64-mingw32\include
	LIBRARY_PATHS += -LC:\SDL2\SDL2-2.26.5\x86_64-w64-mingw32\lib -LC:\SDL2\SDL2_ttf-2.20.2\x86_64-w64-mingw32\lib
	LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
endif

# Build
all: $(BIN)

# $@ = target (BIN), $^ = all dependencies (OBJ)
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) -o $@ $^ $(LIBS)

# $< = first dependency
# % for each cpp file in SRC, create a corresponding .o file in OBJ
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(wildcard $(SRC_DIR)%.hpp)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

# Clean
clean:
	rm -f $(OBJ_DIR)*.o $(BIN)