#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 4096
#define DISP_WIDTH 64
#define DISP_HEIGHT 32
#define FONT_ADDRESS 0x0

typedef struct Chip8 {
// Add in chip8 struct
    uint8_t memory[MEM_SIZE]; //RAM
    uint8_t V[16]; //16 8 bit regs
    uint8_t delay_timer; //8 bit delay timer
    uint8_t sound_timer; //8 bit sound timer
    uint16_t pc; //program counter
    uint8_t sp; //stack pointer
    uint16_t I; // 16 bit index reg
    uint16_t stack[16]; //stack
    uint16_t keypad[16]; //16 bit keypad state
    uint8_t display[DISP_HEIGHT][DISP_WIDTH]; //display buffer
    bool draw_flag; //flag to see if image needs to be drawn
} Chip8;

void chip8_reset(Chip8 * chip8);
void load_rom(char * filename, Chip8 *chip8);
void timer_tick(Chip8 *chip8);
