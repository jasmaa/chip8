#pragma once
#include <stack>
using namespace std;

class chip8 {

public:

	bool can_draw;

	unsigned char memory[4096];
	unsigned short opcode;				// opcode
	unsigned short pc;					// program counter
	unsigned char V[16];				// general registers and flag register
	unsigned short I;					// address register
	stack <unsigned short> call_stack;	// call stack

	unsigned char gfx[32][64];			// graphics array

	unsigned char delay_timer;			// delay timer
	unsigned char sound_timer;			// sound timer
	unsigned char key[16];				// hex keypad

	void init();
	void emulateCycle();
};