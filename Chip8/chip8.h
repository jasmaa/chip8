#pragma once
#include <stack>
using namespace std;

class chip8 {

public:
	unsigned char memory[4096] = {
		0x61,
		0x05,
		0x62,
		0x02,
		0x81,
		0x21,
	};
	unsigned short opcode;				// opcode
	unsigned short pc;					// program counter
	unsigned char V[16];				// general registers and flag register
	unsigned short I;					// address register
	stack <unsigned char> call_stack;	// call stack

	unsigned char gfx[64 * 32];			// graphics array

	unsigned char delay_timer;			// delay timer
	unsigned char sound_timer;			// sound timer
	unsigned char key[16];				// hex keypad

	void init();
	void emulateCycle();
};