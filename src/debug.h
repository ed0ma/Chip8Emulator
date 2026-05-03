#ifndef CHIP8_DEBUG_H
#define CHIP8_DEBUG_H

#include <stdint.h>
#include "chip8.h"

void debug_print_state(const Chip8 *chip8);
void debug_dump_memory(const Chip8 *chip8, uint16_t start, int count);
void debug_step_instruction(Chip8 *chip8);

#endif