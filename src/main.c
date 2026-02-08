#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Expected at least 2 args. %d provided. Expected ROM filepath.", argc);
        return 1;
    }

    Chip8 chip8;
    chip8_reset(&chip8);
    char *filename = argv[1];
    load_rom(filename, &chip8);
}