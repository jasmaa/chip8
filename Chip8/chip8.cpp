#include "chip8.h"

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

void chip8::init() {
	// init registers and memory

	// read file into memory
	fstream filestr("roms/Space Invaders [David Winter] (alt).ch8", fstream::in | fstream::binary);
	unsigned char n;
	int i = 0;
	while (!filestr.eof()) {
		n = filestr.get();
		memory[i + 512] = n;
		i++;
	}
	filestr.close();

	// TEST
	I = 0;
	pc = 0x200;
}

void chip8::emulateCycle() {
	// fetch opcode
	opcode = unsigned(memory[pc] << 8 | memory[pc + 1]);

	// decode and execute opcode
	if (opcode == 0x00E0) {
		// clear screen
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 64; j++) {
				gfx[i][j] = 0;
			}
		}
	}
	// return from subroutine
	else if (opcode == 0x00EE) {
		pc = call_stack.top();
		call_stack.pop();
	}
	// rca
	else if (unsigned(opcode & 0xF000) == 0x0000) {
		// call rca
	}
	// jump to NNN
	else if (unsigned(opcode & 0xF000) == 0x1000) {
		pc = 0x0FFF & opcode;
		pc -= 2;
	}
	// subroutine to NNN
	else if (unsigned(opcode & 0xF000) == 0x2000) {
		call_stack.push(pc);
		pc = 0x0FFF & opcode;
		pc -= 2;
	}
	// skip on VX == NN
	else if (unsigned(opcode & 0xF000) == 0x3000) {
		if (V[0xF & (opcode >> 12)] == 0xFF & opcode) {
			pc += 2;
		}
	}
	// skip on VX != NN
	else if (unsigned(opcode & 0xF000) == 0x4000) {
		if (V[0xF & (opcode >> 12)] != 0xFF & opcode) {
			pc += 2;
		}
	}
	// skip on VX == VY
	else if (unsigned(opcode & 0xF000) == 0x5000) {
		if (V[0xF & (opcode >> 4)] == V[0xF & (opcode >> 8)]) {
			pc += 2;
		}
	}
	// assign VX = NN
	else if (unsigned(opcode & 0xF000) == 0x6000) {
		V[0xF & (opcode >> 8)] = 0xFF & opcode;
	}
	// do VX += NN
	else if (unsigned(opcode & 0xF000) == 0x7000) {
		V[0xF & (opcode >> 8)] += 0xFF & opcode;
	}
	// set VX = VY
	else if (unsigned(opcode & 0xF00F) == 0x8000) {
		V[0xF & (opcode >> 8)] = V[0xF & (opcode >> 4)];
	}
	// set VX = VX | VY
	else if (unsigned(opcode & 0xF00F) == 0x8001) {
		V[0xF & (opcode >> 8)] |= V[0xF & (opcode >> 4)];
	}
	// set VX = VX & VY
	else if (unsigned(opcode & 0xF00F) == 0x8002) {
		V[0xF & (opcode >> 8)] &= V[0xF & (opcode >> 4)];
	}
	// set VX = VX ^ VY
	else if (unsigned(opcode & 0xF00F) == 0x8003) {
		V[0xF & (opcode >> 8)] ^= V[0xF & (opcode >> 4)];
	}
	// set VX += VY
	else if (unsigned(opcode & 0xF00F) == 0x8004) {
		unsigned short s = V[0xF & (opcode >> 8)] + V[0xF & (opcode >> 4)];
		// set carry
		if (s >> 8 != 0) {
			V[0xF] = 1;
		}
		else {
			V[0xF] = 0;
		}
		V[0xF & (opcode >> 8)] = 0xFF & s;
	}
	// set VX -= VY
	else if (unsigned(opcode & 0xF00F) == 0x8005) {
		unsigned short s = V[0xF & (opcode >> 8)] - V[0xF & (opcode >> 4)];
		// set carry
		if (s >> 8 != 0) {
			V[0xF] = 0;
		}
		else {
			V[0xF] = 1;
		}
		V[0xF & (opcode >> 8)] = 0xFF & s;
	}
	// V >>= 1
	else if (unsigned(opcode & 0xF00F) == 0x8006) {
		V[0xF] = V[0xF & (opcode >> 8)] & 0x1;	// get lsb
		V[0xF & (opcode >> 8)] >>= 1;
	}
	// set VX -= VY
	else if (unsigned(opcode & 0xF00F) == 0x8007) {
		unsigned short s = V[0xF & (opcode >> 4)] - V[0xF & (opcode >> 8)];
		// set carry
		if (s >> 8 != 0) {
			V[0xF] = 0;
		}
		else {
			V[0xF] = 1;
		}
		V[0xF & (opcode >> 8)] = 0xFF & s;
	}
	// V <<= 1
	else if (unsigned(opcode & 0xF00F) == 0x8006) {
		V[0xF] = V[0xF & (opcode >> 8)] >> 7;
		V[0xF & (opcode >> 8)] <<= 1;
	}
	// skip on VX == VY
	else if (unsigned(opcode & 0xF00F) == 0x9000) {
		if (V[0xF & (opcode >> 4)] != V[0xF & (opcode >> 8)]) {
			pc += 2;
		}
	}
	// set I = NNN
	else if (unsigned(opcode & 0xF000) == 0xA000) {
		I = 0x0FFF & opcode;
	}
	// jump to NNN + V0
	else if (unsigned(opcode & 0xF000) == 0xB000) {
		pc = 0x0FFF & opcode + V[0];
		pc -= 2;
	}
	// set VX to rand & NN
	else if (unsigned(opcode & 0xF000) == 0xC000) {
		V[0xF & (opcode >> 8)] = (rand() % 256) & 0x00FF & opcode;
	}
	// draw
	else if (unsigned(opcode & 0xF000) == 0xD000) {
		int row = V[0xF & (opcode >> 4)];
		int col = V[0xF & (opcode >> 8)];
		int height = 0xF & opcode;
		int mem = I;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < 8; j++) {
				// set VF on set to unset
				if (gfx[row + i][col + j] == 1 && (memory[mem] >> (7-j)) & 0x1 == 0) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}

				gfx[row + i][col + j] ^= (memory[mem] >> (7 - j)) & 0x1;
			}
			mem++;
		}
	}
	// skip if key is in VX
	else if (unsigned(opcode & 0xF0FF) == 0xE09E) {
		if (key[V[0xF & (opcode >> 8)]] == 1) {
			pc += 2;
		}
	}
	// skip if key is not in VX
	else if (unsigned(opcode & 0xF0FF) == 0xF00A) {
		if (key[V[0xF & (opcode >> 8)]] == 0) {
			pc += 2;
		}
	}
	// set delay timer to VX
	else if (unsigned(opcode & 0xF0FF) == 0xF015) {
		delay_timer = V[0xF & (opcode >> 8)];
	}
	// set sound timer to VX
	else if (unsigned(opcode & 0xF0FF) == 0xF018) {
		sound_timer = V[0xF & (opcode >> 8)];
	}
	// I += VX
	else if (unsigned(opcode & 0xF0FF) == 0xF01E) {
		I += V[0xF & (opcode >> 8)];
	}
	// I = sprite address in VX
	else if (unsigned(opcode & 0xF0FF) == 0xF029) {
		// do sprite stuff here
	}
	
	// move pc
	pc += 2;
}
