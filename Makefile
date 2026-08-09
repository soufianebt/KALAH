# KALAH Game - Makefile
# Supports building CLI, Graphics (SDL2), and library versions

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = 

# Source directories
SRC_MODEL = src/model
SRC_CORE = src/core
SRC_CLI = src/ui/cli
SRC_GRAPHICS = src/ui/graphics

# Object files
OBJ_MODEL = $(SRC_MODEL)/kalah_model.o
OBJ_CORE = $(SRC_CORE)/kalah_engine.o
OBJ_CLI = $(SRC_CLI)/cli_interface.o
OBJ_CLI_MAIN = $(SRC_CLI)/main.o
OBJ_GRAPHICS = $(SRC_GRAPHICS)/graphics_interface.o
OBJ_GRAPHICS_MAIN = $(SRC_GRAPHICS)/main.o

# Targets
TARGET_CLI = KALAH_CLI.exe
TARGET_GRAPHICS = KALAH_GRAPHICS.exe
TARGET_LIB = libkalah.a

# Phony targets
.PHONY: all cli lib graphics both clean help run run-graphics sdl directories

# Default target
all: directories cli lib

# Build everything including graphics (if SDL2 available)
both: directories cli lib graphics

# Create necessary directories
directories:
	-@if not exist $(SRC_MODEL) md $(SRC_MODEL)
	-@if not exist $(SRC_CORE) md $(SRC_CORE)
	-@if not exist $(SRC_CLI) md $(SRC_CLI)
	-@if not exist $(SRC_GRAPHICS) md $(SRC_GRAPHICS)

# Build CLI version
cli: $(TARGET_CLI)

$(TARGET_CLI): $(OBJ_MODEL) $(OBJ_CORE) $(OBJ_CLI) $(OBJ_CLI_MAIN)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[OK] CLI executable created: $(TARGET_CLI)"

# Build library
lib: $(TARGET_LIB)

$(TARGET_LIB): $(OBJ_MODEL) $(OBJ_CORE)
	ar rcs $@ $^
	@echo "[OK] Library created: $(TARGET_LIB)"

# Build graphics version
graphics: $(TARGET_GRAPHICS)

$(TARGET_GRAPHICS): $(OBJ_MODEL) $(OBJ_CORE) $(OBJ_GRAPHICS) $(OBJ_GRAPHICS_MAIN)
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2
	@echo "[OK] Graphics executable created: $(TARGET_GRAPHICS)"

# Compile model
$(OBJ_MODEL): $(SRC_MODEL)/kalah_model.c include/kalah_model.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile engine
$(OBJ_CORE): $(SRC_CORE)/kalah_engine.c include/kalah_engine.h include/kalah_model.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile CLI interface
$(OBJ_CLI): $(SRC_CLI)/cli_interface.c include/cli_interface.h include/kalah_engine.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile CLI main
$(OBJ_CLI_MAIN): $(SRC_CLI)/main.c include/cli_interface.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile graphics interface
$(OBJ_GRAPHICS): $(SRC_GRAPHICS)/graphics_interface.c include/graphics_interface.h include/kalah_engine.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile graphics main
$(OBJ_GRAPHICS_MAIN): $(SRC_GRAPHICS)/main.c include/graphics_interface.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build with SDL2 (optional)
sdl: graphics
	@echo "[OK] Built SDL2 graphics version"

# Run the CLI game
run: cli
	./$(TARGET_CLI)

# Run the graphics game
run-graphics: graphics
	./$(TARGET_GRAPHICS)

# Clean build artifacts
clean:
	-@del /Q src\model\*.o src\core\*.o src\ui\cli\*.o src\ui\graphics\*.o $(TARGET_CLI) $(TARGET_GRAPHICS) $(TARGET_LIB) >nul 2>&1
	@echo "[OK] Build artifacts cleaned"

# Display help
help:
	@echo KALAH Game Build System
	@echo ========================
	@echo.
	@echo Available targets:
	@echo   make all          - Build CLI and library (default)
	@echo   make cli          - Build CLI executable only
	@echo   make lib          - Build static library only
	@echo   make graphics     - Build SDL2 graphics version
	@echo   make both         - Build CLI and graphics versions
	@echo   make sdl          - Build with SDL2 support
	@echo   make run          - Build and run CLI game
	@echo   make run-graphics - Build and run graphics game
	@echo   make clean        - Remove build artifacts
	@echo   make help         - Show this help message
	@echo.
	@echo Examples:
	@echo   make              # Build CLI and library
	@echo   make run          # Play the CLI game
	@echo   make graphics     # Build graphics version (requires SDL2)
	@echo   make run-graphics # Run the graphics version
	@echo   make clean && make # Clean rebuild
