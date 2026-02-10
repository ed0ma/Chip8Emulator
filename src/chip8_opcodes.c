#include "chip8.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>


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


void op_2nnn(Chip8 *chip8, uint16_t nnn){
    /*
    2nnn: Call addr
    Call subroutine at nnn

    sp = sp + 1
    stack[sp] = pc
    pc = nnn
    */
    chip8->sp ++;
    chip8->stack[chip8->sp] = chip8->pc;
    chip8->pc = nnn;
}


void op_3xkk(Chip8 *chip8, uint8_t x, uint8_t kk){
    /*
    3xkk: SE Vx, byte
    Skip next instruction if Vx = kk
    If equal PC = PC + 2
    */
    if (chip8->V[x] == kk){
        chip8->pc += 2;
    }
}


void op_4xkk(Chip8 *chip8, uint8_t x, uint8_t kk){
    /*
    4xkk: SNE Vx, byte
    Skip next instruction if Vx != kk
    If not equal PC = PC + 2
    */
    if (chip8->V[x] != kk){
        chip8->pc += 2;
    }
}


void op_5xy0(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    5xkk: SE Vx, Vy
    Skip next instruction if Vx = Vy
    If equal PC = PC + 2
    */
    if (chip8->V[x] == chip8->V[y]){
        chip8->pc += 2;
    }
}


void op_6xkk(Chip8 *chip8, uint8_t x, uint8_t kk){
    /*
    6xkk: Load Vx
    Set Vx = kk
    */
    chip8->V[x] = kk;
}


void op_7xkk(Chip8 *chip8, uint8_t x, uint8_t kk){
    /*
    7xkk: Add kk to Vx
    Set Vx = Vx + kk
    */
    chip8->V[x] += kk;
}


void op_8xy0(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    8xy0: Ld Vx, Vy
    Store the value of register Vy in register Vx
    */
    chip8->V[x] = chip8->V[y];
}


void op_8xy1(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    8xy1: OR Vx, Vy
    Set Vx = Vx OR Vy
    OR is bitwise
    */
    chip8->V[x] = chip8->V[y] | chip8->V[x];
}


void op_8xy2(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    8xy2: AND Vx, Vy
    Set Vx = Vx AND Vy
    AND is bitwise
    */
    chip8->V[x] = chip8->V[y] & chip8->V[x];
}


void op_8xy3(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    8xy3: XOR Vx, Vy
    Set Vx = Vx XOR Vy
    XOR is bitwise
    */
    chip8->V[x] = chip8->V[y] ^ chip8->V[x];
}


void op_8xy4(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    8xy4: ADD Vx, Vy
    Set Vx = Vx + Vy, VF = carry
    If result > 255 (8 bits) VF = 1, low 8 bits are stored to Vx.
    If result <= 255 VF = 0
    */
    uint16_t sum = (uint16_t)chip8->V[x] + (uint16_t)chip8->V[y];
    chip8->V[0xF] = (sum > 0xFF);
    chip8->V[x] = (uint8_t)(sum & 0xFF);
}


void op_8xy5(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    8xy5: SUB Vx, Vy
    Set Vx = Vx - Vy, VF = Not Borrow
    If Vx > Vy, VF = 1
    else VF = 0
    */
    if(chip8->V[x] >= chip8->V[y]){
        chip8->V[0xF] = 0x1;
    }else{
        chip8->V[0xF] = 0x0;
    }
    chip8->V[x] = chip8->V[x] - chip8->V[y];
}


void op_8xy6(Chip8 *chip8, uint8_t x){
    /*
    8xy6: SHR Vx,
    Set Vx = Vx SHR 1
    If LSB of Vx = 1, VF = 1
    */
    if((chip8->V[x] & 0x0001) == 1){
        chip8->V[0xF] = 1; 
    }
    chip8->V[x] = chip8->V[x] >> 1;
}


void op_8xy7(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    8xy7: SUBN Vx, Vy
    Set Vx = Vy - Vx
    IF Vy > Vx, VF = 1
    else VF = 0
    */
    if(chip8->V[x] < chip8->V[y]){
        chip8->V[0xF] = 0x1;
    }else{
        chip8->V[0xF] = 0x0;
    }
    chip8->V[x] = chip8->V[y] - chip8->V[x];
}


void op_8xyE(Chip8 *chip8, uint8_t x){
    /*
    8xyE: SHL Vx
    Set Vx = Vx SHL 1
    If MSB of Vx = 1, VF = 1
    */
    if((chip8->V[x] & 0x1000) == 1){
        chip8->V[0xF] = 1; 
    }
    chip8->V[x] = chip8->V[x] << 1;
}


void op_9xy0(Chip8 *chip8, uint8_t x, uint8_t y){
    /*
    9xyo: SNE Vx, Vy
    Skip next instruction if Vx != Vy
    If not equal PC = PC + 2
    */
    if (chip8->V[x] != chip8->V[y]){
        chip8->pc += 2;
    }
}


void op_Annn(Chip8 *chip8, uint16_t nnn){
    /*
    Annn: Set index
    Set register I (index register) to nnn
    */
    chip8->I = nnn;
}


void op_Bnnn(Chip8 *chip8, uint16_t nnn){
    /*
    Bnnn: Jp V0, addr
    PC = nnn + V0
    */
    chip8->pc = nnn + chip8->V[0];
}


void op_Cxkk(Chip8 *chip8, uint8_t x, uint8_t kk){
    /*
    Cxkk: RND Vx, byte
    Set Vx = random byte AND kk
    Generate a random number from 0-255 and logical AND it with kk. Store in Vx
    */
    uint8_t r = (uint8_t)(rand() & 0xFF);
    chip8->V[x] = r & kk;
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
                if (chip8->display[y_cord][x_cord]) {
                    chip8->V[0xF] = 1;
                }

                // XOR toggles the pixel
                chip8->display[y_cord][x_cord] ^= 1;
            }
            x_cord ++;
        }
        y_cord ++;
    }
}


void op_Ex9E(Chip8 *chip8, uint8_t x){
    /*
    Ex9E: SKP Vx
    Skip next instruction if key with the value of Vx is pressed
    If the same key is pressed, PC = PC + 2
    */
    uint8_t key = chip8->V[x] & 0x0F;
    if (chip8->keypad[key]) {
        chip8->pc += 2;
    }
}


void op_ExA1(Chip8 *chip8, uint8_t x){
    /*
    ExA1: SKNP Vx
    Skipp next instruction if key with the value fo Vx is not pressed
    If key is in up position, PC = PC + 2
    */
    uint8_t key = chip8->V[x] & 0x0F;
    if (!chip8->keypad[key]) {
        chip8->pc += 2;
    }
}


void op_Fx07(Chip8 *chip8, uint8_t x){
    /*
    Fx07: LD VX, DT
    Set Vx = display timer value
    */
    chip8->V[x] = chip8->delay_timer;
}


void op_Fx0A(Chip8 *chip8, uint8_t x){
    /*
    Fx0A: LD Vx, k
    Wait for a key press, store the value of the key in Vx
    */
    for (uint8_t k = 0; k <16; k++){
        if (chip8->keypad[k]){
            chip8->V[x] = k;
            return;
        }
    }

    //no key pressed. Re execute opcode next cycle
    chip8->pc -= 2;
}


void op_Fx15(Chip8 *chip8, uint8_t x){
    /*
    Fx15: LD DT, Vx
    Set Display timer = Vx
    */
    chip8->delay_timer = chip8->V[x];
}


void op_Fx18(Chip8 *chip8, uint8_t x){
    /*
    Fx18: LD ST, Vx
    Set ST equal to Vx
    */
    chip8->sound_timer = chip8->V[x];
}


void op_Fx1E(Chip8 *chip8, uint8_t x){
    /*
    Fx1E: ADD I, Vx
    Set I = I + Vx
    */
    chip8->I += chip8->V[x];
}


void op_Fx29(Chip8 *chip8, uint8_t x){
    /*
    Fx29: LD F, Vx
    Set I = location of sprite for digit Vx (0-F).
    Each sprite is 5 bytes.
    */
    uint8_t digit = chip8->V[x] & 0x0F;
    chip8->I = FONT_ADDRESS + (digit * 5);
}

void op_Fx33(Chip8 *chip8, uint8_t x){
    /*
    Fx33: LD B, Vx
    Take decimal value of Vx nad:
        -I = hundreds
        -I + 1 = tens
        -I + 2 = ones
    */
    uint8_t v = chip8->V[x];

    chip8->memory[chip8->I]     = v / 100;
    chip8->memory[chip8->I + 1] = (v / 10) % 10;
    chip8->memory[chip8->I + 2] = v % 10;
}


void op_Fx55(Chip8 *chip8, uint8_t x){
    /*
    Fx55: Ld [i], Vx
    Store registers V0-Vx in memory starting at I
    */
    for(uint8_t i = 0; i <= x; i++){
        chip8->memory[chip8->I + i] = chip8->V[i];
    }
}


void op_Fx65(Chip8 *chip8, uint8_t x){
    /*
    Fx65: LD Vx, [I]
    Read starting at memory location I into registers V0-Vx 
    */
    for(uint8_t i = 0; i <= x; i++){
        chip8->V[i] = chip8->memory[chip8->I + i];
    }
}