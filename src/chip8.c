#include "chip8.h"
#include "chip8_opcodes.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


static const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80, //F
};


void chip8_reset(Chip8 * chip8){
    /*
    Reset chip to 0 state and set PC address to 0x200
    */
    memset(chip8, 0x0, sizeof(*chip8));
    chip8->pc = 0x200;
    memcpy(chip8->memory[FONT_ADDRESS], &fontset, sizeof(fontset));
}

void load_rom(char * filename, Chip8 *chip8){

    //Open file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("load_rom: fopen");
        return;
    }

    //Fine len of file
    if (fseek(fp, 0, SEEK_END) != 0){
        perror("load_rom: fseek(end)");
        fclose(fp);
        return;
    }

    long ROM_len_long = ftell(fp);
    if (ROM_len_long < 0) {
    perror("load_rom: ftell");
    fclose(fp);
    return;
    }

    if(fseek(fp, 0, SEEK_SET) != 0){
        perror("load_rom: fseek(set)");
        fclose(fp);
        return;
    }

    size_t ROM_len = (size_t)ROM_len_long;

    //ROM + 0x200 <= memsize
    if (ROM_len > MEM_SIZE - 0x200){
        fprintf(stderr, "load_rom: ROM_len + 0x200 > MEM_SIZE. ROM too large");
        fclose(fp);
        return;
    }

    // Read binary ROM file into memory 0x200
    size_t len_read = fread(&chip8->memory[0x200], 1, ROM_len, fp);
    if (len_read != ROM_len){
        fprintf(stderr, "load_rom: Lenght of ROM file does not match length read into memory");
        fclose(fp);
        return;
    }
    
    fclose(fp);
    return;
}

void chip8_step (Chip8 *chip8){
    // Fetch, decode, execute 1 opp code
    uint16_t opcode = chip8_fetch_opcode(chip8);

    uint16_t nnn = opcode &0x0FFF; //low 12 bits address
    uint8_t n = opcode & 0x000F; //nibble low 4 bits
    uint8_t x = (opcode & 0x0F00) >> 8; //low 4 bits of high byte
    uint8_t y = (opcode & 0x00F0) >> 4; //upper 4 bits of low byte
    uint8_t kk = opcode & 0x00FF; //lower 8 bits

    switch (opcode & 0xF000)
    //0x0000 family of instructions
    {
    case (0x0000):
        switch (opcode)
        {
        case (0x00E0):
            //Clear screen
            op_00E0 (chip8);
            break;
        case (0x00EE):
            //RET
            //Return from subroutine
            op_00EE(chip8);
            break;
        default:
            printf("Unknown opcode: 0x%04X at PC: 0x%03X\n", opcode, (unsigned)(chip8->pc - 2));
            assert(0 && "Unknown opcode");
            break;
        }
        break;

    case (0x1000):
        //jump to nnn
        op_1nnn(chip8, nnn);
        break;
    
    case (0x6000):
        // set Vx = kk
        op_6xkk(chip8, x, kk);
        break;
    
    case (0x7000):
        //ADD
        op_7xnn(chip8, x, kk);
        break;

    case (0xA000):
        // set index register I
        op_Annn(chip8, nnn);
        break;

    case (0xD000):
        //Display
        op_Dxyn(chip8, x, y, n);
        break;

    default:
        printf("Unknown opcode: 0x%04X at PC: 0x%03X\n", opcode, (unsigned)(chip8->pc - 2));
        assert(0 && "Unknown opcode");
        break;
    }
}


 uint16_t chip8_fetch_opcode (Chip8 *chip8){
    /*
    Fetch next chip8 opcode from memory

    Reads 2 consecutive bytes starting at PC address
    Combines them into 16 bit opcode
    Advances PC by 2

    @param chip8 pointer

    @return 16 bit opcode
    */
    assert(chip8->pc <= MEM_SIZE - 2);

    uint16_t instruction;
    uint16_t high_byte = chip8->memory[chip8->pc];
    uint16_t low_byte = chip8->memory[chip8->pc + 1];
    //left shift high byte and stitch together
    high_byte = high_byte << 8;
    instruction = high_byte | low_byte;

    chip8->pc += 2;

    return instruction;
}


void timer_tick(Chip8 *chip8){
    /*
    Decrements delay_timer if > 0
    Decrements sound timer if > 0
    */

    if (chip8->delay_timer > 0){
        chip8->delay_timer --;
    }

    if (chip8->sound_timer > 0){
        chip8->sound_timer --;
    }
}