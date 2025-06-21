#include "chip8.h"

// CLS (clear the display)
void chip8_op_0E00()
{
    memset(g_chip8_data.vid, 0, sizeof(g_chip8_data.vid));
}

// 00EE - RET
// Return from a subroutine
void chip8_op_00EE()
{
    if (g_chip8_data.sp <= 0)
    {
        fprintf(stderr, "Aborting!\nInvalid SP during RET\n");
        chip8_print_state();
        exit(-1);
    }

    g_chip8_data.pc = g_chip8_data.stk[--g_chip8_data.sp];
}

// 1nnn - JP addr
// Jump to location nnn
void chip8_op_1nnn()
{
    g_chip8_data.pc = g_chip8_data.opcode & 0x0FFF;
}

//  2nnn - CALL addr
// Call subroutine at nnn.
void chip8_op_2nnn()
{
    if (g_chip8_data.sp >= 15)
    {
        fprintf(stderr, "Aborting!\nStack Overflow\n");
        chip8_print_state();
        exit(-1);
    }

    g_chip8_data.stk[g_chip8_data.sp++] = g_chip8_data.pc;
    g_chip8_data.pc = g_chip8_data.opcode & 0x0FFF;
}

// 3xkk - SE Vx, byte
// Skip next instruction if Vx == kk
void chip8_op_3xkk()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint16_t kk = (g_chip8_data.opcode & 0x00FF);
    if (g_chip8_data.regs[x] == kk)
    {
        g_chip8_data.pc += 2;
    }
}

// 4xkk - SNE Vx, byte
// Skip next instruction if Vx != kk
void chip8_op_4xkk()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint16_t kk = (g_chip8_data.opcode & 0x00FF);
    if (g_chip8_data.regs[x] != kk)
    {
        g_chip8_data.pc += 2;
    }
}

// 5xy0 - SE Vx, Vy
// Skip next instruction if Vx == Vy
void chip8_op_5xy0()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    if (g_chip8_data.regs[x] == g_chip8_data.regs[y])
    {
        g_chip8_data.pc += 2;
    }
}

// 6xkk - LD Vx, byte
// Set Vx = kk
void chip8_op_6xkk()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t kk = (g_chip8_data.opcode & 0x00FF);
    g_chip8_data.regs[x] = kk;
}

// 7xkk - ADD Vx, byte
// Set Vx = Vx + kk
void chip8_op_7xkk()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t kk = (g_chip8_data.opcode & 0x00FF);
    g_chip8_data.regs[x] += kk;
}

// 8xy0 - LD Vx, Vy
// Set Vx = Vy.
void chip8_op_8xy0()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    g_chip8_data.regs[x] = g_chip8_data.regs[y];
}

// 8xy1 - OR Vx, Vy
// Set Vx = Vx | Vy.
void chip8_op_8xy1()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    g_chip8_data.regs[x] |= g_chip8_data.regs[y];
}

// 8xy2 - AND Vx, Vy
// Set Vx = Vx & Vy.
void chip8_op_8xy2()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    g_chip8_data.regs[x] &= g_chip8_data.regs[y];
}

// 8xy3 - XOR Vx, Vy
// Set Vx = Vx ^ Vy.
void chip8_op_8xy3()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    g_chip8_data.regs[x] ^= g_chip8_data.regs[y];
}

// 8xy4 - ADD Vx, Vy
// Set Vx = Vx + Vy, set VF = carry
void chip8_op_8xy4()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    uint16_t ans = g_chip8_data.regs[x] + g_chip8_data.regs[y];

    g_chip8_data.regs[0xF] = ans > 0xFF;
    g_chip8_data.regs[x] = ans & 0xFF;
}

// 8xy5 - SUB Vx, Vy
// Set Vx = Vx - Vy, set VF = not borrow
void chip8_op_8xy5()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    uint16_t ans = g_chip8_data.regs[x] - g_chip8_data.regs[y];

    g_chip8_data.regs[0xF] = g_chip8_data.regs[x] > g_chip8_data.regs[y];

    g_chip8_data.regs[x] = ans & 0xFF;
}

// 8xy6 - SHR Vx {, Vy}
// Set Vx = Vx >> 1, Set VF = Vx & 1
void chip8_op_8xy6()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    g_chip8_data.regs[0xF] = g_chip8_data.regs[x] & 1;
    g_chip8_data.regs[x] >>= 1;
}

// 8xy7 - SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = not borrow
void chip8_op_8xy7()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    uint16_t ans = g_chip8_data.regs[y] - g_chip8_data.regs[x];

    g_chip8_data.regs[0xF] = (g_chip8_data.regs[y] > g_chip8_data.regs[x]);

    g_chip8_data.regs[x] = ans & 0xFF;
}

// 8xyE - SHL Vx {, Vy}
// Set Vx = Vx << 1, Set VF = 1 if MSB is on
void chip8_op_8xyE()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    g_chip8_data.regs[0xF] = (g_chip8_data.regs[x] & 0x80) >> 7;
    g_chip8_data.regs[x] <<= 1;
}

// 9xy0 - SNE Vx, Vy
// Skip next instruction if Vx != Vy.
void chip8_op_9xy0()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    if (g_chip8_data.regs[x] != g_chip8_data.regs[y])
    {
        g_chip8_data.pc += 2;
    }
}

// Annn - LD I, addr
// Set I = nnn.
void chip8_op_Annn()
{
    g_chip8_data.idx = g_chip8_data.opcode & 0x0FFF;
}

// Bnnn - JP V0, addr
// Jump to location nnn + V0.
void chip8_op_Bnnn()
{
    g_chip8_data.pc += g_chip8_data.opcode & 0x0FFF;
}

// Cxkk - RND Vx, byte
// Set Vx = random byte AND kk.
void chip8_op_Cxkk()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t kk = (g_chip8_data.opcode & 0x00FF);
    g_chip8_data.regs[x] = (rand() % 256) & kk;
}

void chip8_op_Dxyn()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    uint8_t height = g_chip8_data.opcode & 0x000F;

    uint8_t x_pos = g_chip8_data.regs[x] % VIDEO_WIDTH;
    uint8_t y_pos = g_chip8_data.regs[y] % VIDEO_HEIGHT;
    g_chip8_data.regs[0xF] = 0;

    for (uint8_t row = 0; row < height; ++row)
    {
        uint8_t sprite_byte = g_chip8_data.mem[g_chip8_data.idx + row];
        for (uint8_t col = 0; col < 8; col++)
        {
            uint8_t sprite_pixel = sprite_byte & (0x80 >> col);
            uint32_t *screen_pixel = &g_chip8_data.vid[(y_pos + row) * VIDEO_WIDTH + (x_pos + col)];

            if (sprite_pixel)
            {
                if (*screen_pixel)
                    g_chip8_data.regs[0xF] = 1;

                *screen_pixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void chip8_op_Ex9E()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    if (g_chip8_data.keys[g_chip8_data.regs[x]])
    {
        g_chip8_data.pc += 2;
    }
}

void chip8_op_ExA1()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    if (!g_chip8_data.keys[g_chip8_data.regs[x]])
    {
        g_chip8_data.pc += 2;
    }
}

void chip8_op_Fx07()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    g_chip8_data.regs[x] = g_chip8_data.delTime;
}

void chip8_op_Fx0A()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;

    for (uint8_t key = 0; key < 16; key++)
    {
        if (g_chip8_data.keys[key])
        {
            g_chip8_data.regs[x] = key;
            return;
        }
    }

    g_chip8_data.pc -= 2;
}

void chip8_op_Fx15()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    g_chip8_data.delTime = g_chip8_data.regs[x];
}

void chip8_op_Fx18()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    g_chip8_data.sfxTime = g_chip8_data.regs[x];
}

void chip8_op_Fx1E()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    g_chip8_data.idx += g_chip8_data.regs[x];
}

void chip8_op_Fx29()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t digit = g_chip8_data.regs[x];
    g_chip8_data.idx = FONT_OFFSET + 5 * digit;
}

void chip8_op_Fx33()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t val = g_chip8_data.regs[x];

    for (int i = 2; i >= 0; i--)
    {
        g_chip8_data.mem[g_chip8_data.idx + i] = val % 10;
        val /= 10;
    }
}

void chip8_op_Fx55()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++)
    {
        g_chip8_data.mem[g_chip8_data.idx + i] = g_chip8_data.regs[i];
    }
}

void chip8_op_Fx65()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++)
    {
        g_chip8_data.regs[i] = g_chip8_data.mem[g_chip8_data.idx + i];
    }
}

void chip8_decode_execute()
{
    switch (g_chip8_data.opcode & 0xF000)
    {
    case 0x0000:
        switch (g_chip8_data.opcode & 0x00FF)
        {
        case 0x00E0:
            chip8_op_0E00();
            break;
        case 0x00EE:
            chip8_op_00EE();
            break;
        default:
            fprintf(stderr, "Aborting!\nInvalid opcode: 0x%04X\n", g_chip8_data.opcode);
            chip8_print_state();
            exit(-1);
        }
        break;
    case 0x1000:
        chip8_op_1nnn();
        break;
    case 0x2000:
        chip8_op_2nnn();
        break;
    case 0x3000:
        chip8_op_3xkk();
        break;
    case 0x4000:
        chip8_op_4xkk();
        break;
    case 0x5000:
        chip8_op_5xy0();
        break;
    case 0x6000:
        chip8_op_6xkk();
        break;
    case 0x7000:
        chip8_op_7xkk();
        break;
    case 0x8000:
        switch (g_chip8_data.opcode & 0x000F)
        {
        case 0x0000:
            chip8_op_8xy0();
            break;
        case 0x0001:
            chip8_op_8xy1();
            break;
        case 0x0002:
            chip8_op_8xy2();
            break;
        case 0x0003:
            chip8_op_8xy3();
            break;
        case 0x0004:
            chip8_op_8xy4();
            break;
        case 0x0005:
            chip8_op_8xy5();
            break;
        case 0x0006:
            chip8_op_8xy6();
            break;
        case 0x0007:
            chip8_op_8xy7();
            break;
        case 0x000E:
            chip8_op_8xyE();
            break;
        default:
            fprintf(stderr, "Aborting!\nInvalid opcode: 0x%04X\n", g_chip8_data.opcode);
            chip8_print_state();
            exit(-1);
        }
        break;
    case 0x9000:
        chip8_op_9xy0();
        break;
    case 0xA000:
        chip8_op_Annn();
        break;
    case 0xB000:
        chip8_op_Bnnn();
        break;
    case 0xC000:
        chip8_op_Cxkk();
        break;
    case 0xD000:
        chip8_op_Dxyn();
        break;
    case 0xE000:
        switch (g_chip8_data.opcode & 0x00FF)
        {
        case 0x009E:
            chip8_op_Ex9E();
            break;
        case 0x00A1:
            chip8_op_ExA1();
            break;
        default:
            fprintf(stderr, "Aborting!\nInvalid opcode: 0x%04X\n", g_chip8_data.opcode);
            chip8_print_state();
            exit(-1);
        }
        break;
    case 0xF000:
        switch (g_chip8_data.opcode & 0x00FF)
        {
        case 0x0007:
            chip8_op_Fx07();
            break;
        case 0x000A:
            chip8_op_Fx0A();
            break;
        case 0x0015:
            chip8_op_Fx15();
            break;
        case 0x0018:
            chip8_op_Fx18();
            break;
        case 0x001E:
            chip8_op_Fx1E();
            break;
        case 0x0029:
            chip8_op_Fx29();
            break;
        case 0x0033:
            chip8_op_Fx33();
            break;
        case 0x0065:
            chip8_op_Fx65();
            break;
        case 0x0055:
            chip8_op_Fx55();
            break;
        default:
            fprintf(stderr, "Aborting!\nInvalid opcode: 0x%04X\n", g_chip8_data.opcode);
            chip8_print_state();
            exit(-1);
        }
        break;
    default:
        fprintf(stderr, "Aborting!\nInvalid opcode: 0x%04X\n", g_chip8_data.opcode);
        chip8_print_state();
        exit(-1);
    }
}