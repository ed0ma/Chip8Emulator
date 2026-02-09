#include "chip8.h"
#include <stdbool.h>

void op_00E0 (Chip8 *chip8){
    /*
    00E0: Clear screen
    Set all display pixels = 0
    */
    memset(chip8->display, 0x0, sizeof(chip8->display));
    chip8->draw_flag = true;
    return;
}


void op_00EE(Chip8 *chip8){
    /*
    00EE: RET
    Return from subroutine
    Set program counter to address at top of stack
    Subtract 1 from stack pointer
    */
    assert(chip8->sp > 0);
    chip8->sp --;
    chip8->pc = chip8->stack[chip8->sp];
}


void op_1nnn(Chip8 *chip8, uint16_t nnn){
    /*
    1nnn: JUMP
    Set PC value to nnn
    */
    chip8->pc = nnn;
}


void op_6xkk(Chip8 *chip8, uint8_t x, uint8_t kk){
    /*
    6xkk: Load Vx
    Set Vx = kk
    */
    chip8->V[x] = kk;
}


void op_7xnn(Chip8 *chip8, uint8_t x, uint8_t kk){
    /*
    7xnn: Add kk to Vx
    Set Vx = Vx + kk
    */
    chip8->V[x] += kk;
}


void op_Annn(Chip8 *chip8, uint16_t nnn){
    /*
    Annn: Set index
    Set register I (index register) to nnn
    */
   chip8->I = nnn;
}


void op_Dxyn(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t n){
    /*
    Dxyn: DRW Vx, Vy, n
    Draw n-byte sprite from memory[I] at (Vx, Vy)
    Sprite is XORed onto display; VF = 1 if any pixel is erased (collision), else 0
    Start coords wrap (Vx % 64, Vy % 32); drawing clips at right/bottom edges
    */
    chip8->V[0xF] = 0;
    uint8_t y_cord = chip8->V[y] % 32;

    for (int row = 0; row < n; row++){
        uint8_t x_cord = chip8->V[x] % 64; //reset x cord
        if (y_cord > 31){
            //bottom edge of screen
            break;
        }
        uint8_t sprite_data = chip8->memory[chip8->I + row];
        for (int pixel = 0; pixel < 8; pixel ++){
            if(x_cord > 63){
                //right edge of screen
                break;
            }
            uint8_t sprite_bit = (sprite_data >> (7 - pixel)) & 1; //Extract bits of the sprite from MSB to LSB
            if (sprite_bit) {
                // flip a 1 -> 0, that's a collision
                if (chip8->display[x_cord][y_cord]) {
                    chip8->V[0xF] = 1;
                }

                // XOR toggles the pixel
                chip8->display[x_cord][y_cord] ^= 1;
            }
            x_cord ++;
        }
        y_cord ++;
    }
}