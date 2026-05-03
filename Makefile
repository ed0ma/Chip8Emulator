CC = gcc

TARGET = chip8.exe

SRC = src/main.c \
      src/chip8.c \
      src/chip8_opcodes.c \
      src/chip8_sdl.c \
      src/debug.c

CFLAGS = -Wall -Wextra -g
SDL_FLAGS = $(shell pkg-config --cflags --libs sdl2)

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(SDL_FLAGS)

run: all
	./$(TARGET) "roms/test_opcode.ch8"

ibm: all
	./$(TARGET) "roms/IBM Logo.ch8"

clean:
	rm -f $(TARGET)