all:
	g++ chip8.c -o chip8-emu -lSDL2

test: all
	./chip8-emu 10 1 test_opcode.ch8
