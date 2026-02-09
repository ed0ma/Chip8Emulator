//main.c

#include "chip8.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Expected at least 2 args. %d provided. Expected ROM filepath.", argc);
        return 1;
    }

    Chip8 chip8;
    chip8_reset(&chip8);
    char *filename = argv[1];
    load_rom(filename, &chip8);

    //Main loop
    uint8_t last_timer_ms = 0;
    uint8_t now_timer_ms = 0;
    while (1){

    }
}