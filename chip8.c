#include "chip8.h"

void chip8_init()
{
    extern const unsigned int ROM_OFFSET;
    g_chip8_data.pc = ROM_OFFSET;

    srand(time(NULL));
}

void chip8_print_state()
{
    for (int i = 0; i < 16; i++)
    {
        printf("V%d=0x%x\n", i, g_chip8_data.regs[i]);
    }

    printf("PC=0x%x\n", g_chip8_data.pc);
    printf("ID=0x%x\n", g_chip8_data.idx);
    printf("SP=0x%x\n", g_chip8_data.sp);
    printf("OP=0x%x\n", g_chip8_data.opcode);
}

void chip8_cycle()
{
    g_chip8_data.opcode = (g_chip8_data.mem[g_chip8_data.pc] << 8) | g_chip8_data.mem[g_chip8_data.pc + 1];
    g_chip8_data.pc += 2;

    chip8_decode_execute();

    if (g_chip8_data.delTime > 0)
    {
        g_chip8_data.delTime--;
    }

    if (g_chip8_data.sfxTime > 0)
    {
        g_chip8_data.sfxTime--;
    }
}

long long time_millis()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: chip8-emu <video_scale> <delay_ms> <rom_file_bin>\n");
        return 1;
    }

    int video_scale = atoi(argv[1]);
    int cycle_delay_ms = atoi(argv[2]);
    const char *rom_filename = argv[3];

    platform_init("CHIP-8 Emulator", VIDEO_WIDTH * video_scale, VIDEO_HEIGHT * video_scale, VIDEO_WIDTH, VIDEO_HEIGHT);

    chip8_load_fonts();
    chip8_load_rom(rom_filename);
    chip8_init();

    int video_pitch = sizeof(g_chip8_data.vid[0]) * VIDEO_WIDTH;
    long long last_cycle_time = time_millis();
    int quit = 0;

    while (!quit)
    {
        quit = process_input(g_chip8_data.keys);

        long long cur_time = time_millis();
        long long delta = cur_time - last_cycle_time;
        if (delta > cycle_delay_ms)
        {
            last_cycle_time = cur_time;
            chip8_cycle();
            platform_update(g_chip8_data.vid, video_pitch);
        }
    }

    return 0;
}

#include "instructions.c"
#include "mem.c"
#include "video.c"