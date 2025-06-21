#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <SDL2/SDL.h>

struct chip8_data
{
    // V0..VF registers
    uint8_t regs[16];

    // memory address register
    uint16_t idx;

    // program counter
    uint16_t pc;

    // 16-word long stack
    uint16_t stk[16];

    // stack pointer
    uint8_t sp;

    // delay timer
    uint8_t delTime;

    // sound timer
    uint8_t sfxTime;

    // keypad state
    uint8_t keys[16];

    // current instruction
    uint32_t opcode;

    // 0x000 to 0xFFF memory (4096 bytes)
    uint8_t mem[4096];

    // display memory
    uint32_t vid[64 * 32];
} g_chip8_data;

// debug functions
void chip8_print_state();

// memory functions
void chip8_load_rom(const char *filename);
void chip8_load_fonts();

// instruction functions
void chip8_decode_execute();

// SDL functions
const int VIDEO_WIDTH = 64;
const int VIDEO_HEIGHT = 32;
const unsigned int FONTSET_SZ = 80;
const unsigned int FONT_OFFSET = 80;
void platform_init(const char *title, int window_width, int window_height, int texture_width, int texture_height);
void platform_update(void *buffer, int pitch);
int process_input(uint8_t *keys);
#endif