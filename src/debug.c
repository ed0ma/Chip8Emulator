#include "debug.h"

#include <stdio.h>

static uint16_t debug_peek_opcode(const Chip8 *chip8){
    uint16_t high_byte = chip8->memory[chip8->pc];
    uint16_t low_byte  = chip8->memory[chip8->pc + 1];

    return (high_byte << 8) | low_byte;
}

void debug_print_state(const Chip8 *chip8){
    printf("\nPC=0x%03X I=0x%03X SP=%u DT=%u ST=%u OPCODE=0x%04X\n",
           chip8->pc,
           chip8->I,
           chip8->sp,
           chip8->delay_timer,
           chip8->sound_timer,
           debug_peek_opcode(chip8));

    for (int i = 0; i < 16; i++){
        printf("V%X=%02X ", i, chip8->V[i]);

        if (i == 7){
            printf("\n");
        }
    }

    printf("\n");
    fflush(stdout);
}

void debug_dump_memory(const Chip8 *chip8, uint16_t start, int count){
    printf("\nMemory dump from 0x%03X:\n", start);

    for (int i = 0; i < count; i++){
        uint16_t addr = start + i;

        if (addr >= MEM_SIZE){
            break;
        }

        if (i % 16 == 0){
            printf("\n0x%03X: ", addr);
        }

        printf("%02X ", chip8->memory[addr]);
    }

    printf("\n");
    fflush(stdout);
}

void debug_step_instruction(Chip8 *chip8){
    printf("\n--- STEP ---\n");

    printf("Before:");
    debug_print_state(chip8);

    chip8_step(chip8);

    printf("After:");
    debug_print_state(chip8);
    
    fflush(stdout);
}