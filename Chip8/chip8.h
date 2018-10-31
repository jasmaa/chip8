#pragma once
#include <stack>
using namespace std;

class Chip8 {

public:
	unsigned char memory[4096];
	unsigned short opcode;				// opcode
	unsigned short pc;					// program counter
	unsigned char V[16];				// general registers and flag register
	unsigned short I;					// address register
	stack <unsigned short> call_stack;	// call stack

	unsigned char delay_timer;			// delay timer
	unsigned char sound_timer;			// sound timer

	unsigned char key[16];				// hex keypad
	unsigned char gfx[32][64];			// graphics array
	bool has_draw;
	bool key_pressed;
	bool has_buzz;

	void init(string fname);
	void emulateCycle();
};