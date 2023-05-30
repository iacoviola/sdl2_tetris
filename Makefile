SRC_DIR = src/
BUILD_DIR = build/
DEBUG_DIR = $(BUILD_DIR)debug/
RELEASE_DIR = $(BUILD_DIR)release/

DBG_OBJ_DIR = $(DEBUG_DIR)obj/
REL_OBJ_DIR = $(RELEASE_DIR)obj/

# Path: src/
# wildcard is used to get all the files with .cpp extension
SRC = $(wildcard $(SRC_DIR)*.cpp)

# Path: obj/
# % for each file in SRC, replace the path with the path to the obj directory
OBJS = $(SRC:$(SRC_DIR)%.cpp=%.o)
DBG_OBJ = $(addprefix $(DBG_OBJ_DIR), $(OBJS))
REL_OBJ = $(addprefix $(REL_OBJ_DIR), $(OBJS))

# Path: build/
DBG_BIN = $(DEBUG_DIR)main
REL_BIN = $(RELEASE_DIR)main

# Compiler
CC = g++

# Flags
CFLAGS = -Wall -std=c++17
DBG_FLAGS = -g3 -DDEBUG
REL_FLAGS = -O3 -DNDEBUG

# Libraries
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer

# Includes
INCLUDE_PATHS =

# Library paths
LIBRARY_PATHS =

# Resource paths
RESOURCES_PATH =

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	INCLUDE_PATHS += -I/opt/homebrew/include/
endif

ifeq ($(OS),Windows_NT)
	CFLAGS += -Wl,-subsystem,windows
	INCLUDE_PATHS += -IC:\SDL2\SDL2-2.26.5\x86_64-w64-mingw32\include -IC:\SDL2\SDL2_ttf-2.20.2\x86_64-w64-mingw32\include -IC:\SDL2\SDL2_image-2.6.3\x86_64-w64-mingw32\include -IC:\SDL2\SDL2_mixer-2.6.3\x86_64-w64-mingw32\include
	LIBRARY_PATHS += -LC:\SDL2\SDL2-2.26.5\x86_64-w64-mingw32\lib -LC:\SDL2\SDL2_ttf-2.20.2\x86_64-w64-mingw32\lib -LC:\SDL2\SDL2_image-2.6.3\x86_64-w64-mingw32\lib -LC:\SDL2\SDL2_mixer-2.6.3\x86_64-w64-mingw32\lib
	LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
	RESOURCES_PATH = res/icon/icon.res
endif

# Build
all: prepare debug

# Debug
debug: prepare $(DBG_BIN)

# $@ = target (BIN), $^ = all dependencies (OBJ)
$(DBG_BIN): $(DBG_OBJ)
	$(CC) $(CFLAGS) $(DBG_FLAGS) $(LIBRARY_PATHS) -o $@ $^ $(RESOURCES_PATH) $(LIBS)

# $< = first dependency
# % for each cpp file in SRC, create a corresponding .o file in OBJ
$(DBG_OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) $(DBG_FLAGS) $(INCLUDE_PATHS) -c -o $@ $<

# Release
release: prepare $(REL_BIN)

$(REL_BIN): $(REL_OBJ)
	$(CC) $(CFLAGS) $(REL_FLAGS) $(LIBRARY_PATHS) -o $@ $^ $(RESOURCES_PATH) $(LIBS)

$(REL_OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) $(REL_FLAGS) $(INCLUDE_PATHS) -c -o $@ $<

prepare:
ifeq ($(OS),Windows_NT)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(DEBUG_DIR) mkdir $(DEBUG_DIR)
	@if not exist $(RELEASE_DIR) mkdir $(RELEASE_DIR)
	@if not exist $(DBG_OBJ_DIR) mkdir $(DBG_OBJ_DIR)
	@if not exist $(REL_OBJ_DIR) mkdir $(REL_OBJ_DIR)
else
	@mkdir -p $(BUILD_DIR) $(DEBUG_DIR) $(RELEASE_DIR) $(DBG_OBJ_DIR) $(REL_OBJ_DIR)
endif

# Clean
clean:
ifeq ($(OS),Windows_NT)
	@if exist $(DBG_OBJ_DIR) del $(subst /,\, $(DBG_OBJ_DIR)*.o)
	@if exist $(REL_OBJ_DIR) del $(subst /,\, $(REL_OBJ_DIR)*.o)
	@if exist $(DBG_BIN) del $(subst /,\, $(DBG_BIN))
	@if exist $(REL_BIN) del $(subst /,\, $(REL_BIN))
else
	@rm -f $(DBG_OBJ_DIR)*.o
	@rm -f $(REL_OBJ_DIR)*.o
	@rm -f $(DBG_BIN)
	@rm -f $(REL_BIN)
endif