#include "chip8.h"
#include <stdio.h>

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
    if (ans > 0xFF)
    {
        g_chip8_data.regs[0xF] = 1;
    }

    g_chip8_data.regs[x] = ans & 0xFF;
}

// 8xy5 - SUB Vx, Vy
// Set Vx = Vx - Vy, set VF = not borrow
void chip8_op_8xy5()
{
    uint8_t x = (g_chip8_data.opcode & 0x0F00) >> 8;
    uint8_t y = (g_chip8_data.opcode & 0x00F0) >> 4;
    uint16_t ans = g_chip8_data.regs[x] - g_chip8_data.regs[y];
    if (g_chip8_data.regs[x] > g_chip8_data.regs[y])
    {
        g_chip8_data.regs[0xF] = 1;
    }

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
    if (g_chip8_data.regs[y] > g_chip8_data.regs[x])
    {
        g_chip8_data.regs[0xF] = 1;
    }

    g_chip8_data.regs[x] = ans & 0xFF;
}

// 8xy8 - SHL Vx {, Vy}
// Set Vx = Vx << 1, Set VF = 1 if MSB is on
void chip8_op_8xy8()
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