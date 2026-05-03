//main.c
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#include "chip8.h"
#include "chip8_sdl.h"
#include "debug.h"

#define SCALE 12
#define CPU_HZ 700.0
#define TIMER_HZ 60.0
#define DEBUG_STEP_MODE 1

int main(int argc, char *argv[]){
    setvbuf(stdout, NULL, _IONBF, 0);

    if(argc < 2){
        fprintf(stderr, "Expected at least 2 args. %d provided. Expected ROM filepath.", argc);
        return 1;
    }

    Chip8 chip8;
    chip8_reset(&chip8);
    char *filename = argv[1];
    load_rom(filename, &chip8);

    //_____SDL Initialization_____
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("CHIP-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DISP_WIDTH * SCALE, DISP_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "sdl_setup: SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "sdl_setup: SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        DISP_WIDTH,
        DISP_HEIGHT
    );
    if (!texture) {
        fprintf(stderr, "sdl_setup: SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //______ Timing Set up _____
    uint64_t now_timer = 0;
    uint64_t last_timer = SDL_GetPerformanceCounter();
    uint64_t perf_freq = SDL_GetPerformanceFrequency();

    double cpu_accum = 0.0;
    double timer_accum = 0.0;

    const double cpu_step = 1.0 / CPU_HZ;
    const double timer_step = 1.0 / TIMER_HZ;

#if DEBUG_STEP_MODE
    //_____Debugging_______
    bool debug_paused = true;
    bool debug_step_once = false;

    printf("\nDebug mode enabled.\n");
    printf("SPACE = step one instruction\n");
    printf("P     = pause/unpause\n");
    printf("M     = dump memory around PC\n");
    printf("I     = dump memory around I\n");
#endif

    //______ Main Loop ________
    uint32_t pixels[DISP_WIDTH * DISP_HEIGHT];
    bool running = true;

    while (running){
        //timing
        now_timer = SDL_GetPerformanceCounter();
        double delta_time = (double)(now_timer - last_timer) / (double)perf_freq;
        last_timer = now_timer;

        //stop large delta
        if (delta_time > 0.1) delta_time = 0.1;

        //SDL Events Processing
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
#if DEBUG_STEP_MODE
                if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_SPACE:
                            debug_step_once = true;
                            break;

                        case SDL_SCANCODE_P:
                            debug_paused = !debug_paused;
                            cpu_accum = 0.0;
                            timer_accum = 0.0;
                            printf("\nDebug paused = %s\n", debug_paused ? "true" : "false");
                            break;

                        case SDL_SCANCODE_M:
                            debug_dump_memory(&chip8, chip8.pc, 64);
                            break;

                        case SDL_SCANCODE_I:
                            debug_dump_memory(&chip8, chip8.I, 64);
                            break;

                        default:
                            break;
                    }
                }
#endif

                int k = sdl_scancode_to_chip8(event.key.keysym.scancode);
                if (k != -1) {
                    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                        chip8.keypad[k] = 1;
                    } else if (event.type == SDL_KEYUP) {
                        chip8.keypad[k] = 0;
                    }
                }
            }
        }

        //CPU cycle
#if DEBUG_STEP_MODE
        if (debug_paused) {
            if (debug_step_once) {
                debug_step_instruction(&chip8);
                debug_step_once = false;
            }
        } else {
            cpu_accum += delta_time;

            while (cpu_accum >= cpu_step){
                chip8_step(&chip8);
                cpu_accum -= cpu_step;
            }

            timer_accum += delta_time;

            while (timer_accum >= timer_step){
                chip8_timer_tick(&chip8);
                timer_accum -= timer_step;
            }
        }
#else
        cpu_accum += delta_time;

        while (cpu_accum >= cpu_step){
            chip8_step(&chip8);
            cpu_accum -= cpu_step;
        }

        timer_accum += delta_time;

        while (timer_accum >= timer_step){
            chip8_timer_tick(&chip8);
            timer_accum -= timer_step;
        }
#endif

        //Update display window if draw flag changed
        if (chip8.draw_flag){
            chip8_disp_to_pixels(&chip8, pixels);
            SDL_UpdateTexture(texture, NULL, pixels, DISP_WIDTH * (int)sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            chip8.draw_flag = false;
        }
    }

    //Clean up sdl objects
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}