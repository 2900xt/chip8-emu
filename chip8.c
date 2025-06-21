#include "chip8.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void chip8_init()
{
    extern unsigned int ROM_OFFSET;
    g_chip8_data.pc = ROM_OFFSET;

    srand(time(NULL));
}

void chip8_print_state()
{
    for(int i = 0; i < 16; i++)
    {
        printf("V%d=0x%x\n", i, g_chip8_data.regs[i]);
    }
    
    printf("PC=0x%x\n", g_chip8_data.pc);
    printf("ID=0x%x\n", g_chip8_data.idx);
    printf("SP=0x%x\n", g_chip8_data.sp);
    printf("OP=0x%x\n", g_chip8_data.opcode);
}

int main(int argc, char **argv)
{
    if(argc <= 1)
    {
        fprintf(stderr, "Usage: chip8 <rom_file_bin>");
        return 1;
    }

    chip8_load_fonts();
    chip8_load_rom(argv[1]);
    chip8_init();

    return 0;
}