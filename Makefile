# Compiler for default (Unix-like) platform
CC = gcc

# Compiler for Windows (cross-compiling)
WIN_CC = x86_64-w64-mingw32-gcc

# Compiler flags
CFLAGS = -Wall -g

# Static linking flag
STATIC = -static

# Library and include paths for ncurses (Unix)
NCURSES_LIB = /usr/lib/x86_64-linux-gnu
NCURSES_INCLUDE = /usr/include
NCURSES_STATIC_LIB = $(NCURSES_LIB)/libncurses.a

# Library and include paths for PDCurses (Windows)
PDCURSES_LIB = /home/edwin/Downloads/PDCurses-3.9/wincon
PDCURSES_INCLUDE = /usr/x86_64-w64-mingw32/include
PDCURSES_STATIC_LIB = $(PDCURSES_LIB)/pdcurses.a

# Target executable names
TARGET = cred_manager
WIN_TARGET = cred_manager.exe

# Find all .c files in the current directory
SOURCES = $(wildcard *.c)

# Generate object file names from source files
OBJECTS = $(SOURCES:.c=.o)
WIN_OBJECTS = $(SOURCES:.c=.win.o)

# Find all .h files in the current directory
HEADERS = $(wildcard *.h)

# Default target (Unix-like)
all: $(TARGET)

# Windows target
windows: $(WIN_TARGET)

# Link object files to create Unix executable (statically with ncurses)
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -lncurses

# Link object files to create Windows executable (statically with PDCurses)
$(WIN_TARGET): $(WIN_OBJECTS) $(PDCURSES_STATIC_LIB)
	$(WIN_CC) $(STATIC) $(WIN_OBJECTS) -o $(WIN_TARGET) -L$(PDCURSES_LIB) $(PDCURSES_STATIC_LIB)

# Compile source files to object files (Unix with ncurses)
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile source files to object files (Windows with PDCurses)
%.win.o: %.c $(HEADERS)
	$(WIN_CC) $(CFLAGS) -c $< -o $@ -I$(PDCURSES_INCLUDE)

# Clean up
clean:
	rm -f $(OBJECTS) $(WIN_OBJECTS) $(TARGET) $(WIN_TARGET)

# Phony targets
.PHONY: all windows clean