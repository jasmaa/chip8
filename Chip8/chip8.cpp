#include "chip8.h"

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

void chip8::init() {
	// init registers and memory

	// read file into memory
	fstream filestr("roms/ibm.ch8", fstream::in | fstream::binary);
	unsigned char n;
	int i = 0;
	while (!filestr.eof()) {
		n = filestr.get();
		memory[i + 512] = n;
		i++;
	}
	filestr.close();

	pc = 0x200;

	// set font for characters starting at 00
	// 0
	memory[0] = 0xF0;
	memory[1] = 0x90;
	memory[2] = 0x90;
	memory[3] = 0x90;
	memory[4] = 0xF0;
	// 1
	memory[5] = 0x20;
	memory[6] = 0x60;
	memory[7] = 0x20;
	memory[8] = 0x20;
	memory[9] = 0x70;
	// 2
	memory[10] = 0xF0;
	memory[11] = 0x10;
	memory[12] = 0xF0;
	memory[13] = 0x80;
	memory[14] = 0xF0;
	// 3
	memory[15] = 0xF0;
	memory[16] = 0x10;
	memory[17] = 0xF0;
	memory[18] = 0x10;
	memory[19] = 0xF0;
	// 4
	memory[20] = 0x90;
	memory[21] = 0x90;
	memory[22] = 0xF0;
	memory[23] = 0x10;
	memory[24] = 0x10;
	// 5
	memory[25] = 0xF0;
	memory[26] = 0x80;
	memory[27] = 0xF0;
	memory[28] = 0x90;
	memory[29] = 0xF0;
	// 6
	memory[30] = 0xF0;
	memory[31] = 0x80;
	memory[32] = 0xF0;
	memory[33] = 0x90;
	memory[34] = 0xF0;
	// 7
	memory[35] = 0xF0;
	memory[36] = 0x10;
	memory[37] = 0x20;
	memory[38] = 0x40;
	memory[39] = 0x40;
	// 8
	memory[40] = 0xF0;
	memory[41] = 0x90;
	memory[42] = 0xF0;
	memory[43] = 0x90;
	memory[44] = 0xF0;
	// 9
	memory[45] = 0xF0;
	memory[46] = 0x90;
	memory[47] = 0xF0;
	memory[48] = 0x10;
	memory[49] = 0xF0;
	// A
	memory[50] = 0xF0;
	memory[51] = 0x90;
	memory[52] = 0xF0;
	memory[53] = 0x90;
	memory[54] = 0x90;
	// B
	memory[55] = 0xE0;
	memory[56] = 0x90;
	memory[57] = 0xE0;
	memory[58] = 0x90;
	memory[59] = 0xE0;
	// C
	memory[60] = 0xF0;
	memory[61] = 0x80;
	memory[62] = 0x80;
	memory[63] = 0x80;
	memory[64] = 0xF0;
	// D
	memory[65] = 0xE0;
	memory[66] = 0x90;
	memory[67] = 0x90;
	memory[68] = 0x90;
	memory[69] = 0xE0;
	// E
	memory[70] = 0xF0;
	memory[71] = 0x80;
	memory[72] = 0xF0;
	memory[73] = 0x80;
	memory[74] = 0xF0;
	// F
	memory[75] = 0xF0;
	memory[76] = 0x80;
	memory[77] = 0xF0;
	memory[78] = 0x80;
	memory[79] = 0x80;

}

void chip8::emulateCycle() {

	can_draw = false;

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

		can_draw = true;

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
		I = V[0xF & (opcode >> 8)] * 5;
	}
	// bcd
	else if (unsigned(opcode & 0xF0FF) == 0xF033) {
		int num = V[0xF & (opcode >> 8)];
		// convert to decimal
		memory[I] = num / 100;
		memory[I + 1] = (num / 10) % 10;
		memory[I + 2] = num % 10;
	}
	// reg dump
	else if (unsigned(opcode & 0xF0FF) == 0xF055) {
		int num = V[0xF & (opcode >> 8)];
		for (int i = 0; i <= num; i++) {
			memory[I + i] = V[i];
		}
	}
	// reg load
	else if (unsigned(opcode & 0xF0FF) == 0xF065) {
		int num = V[0xF & (opcode >> 8)];
		for (int i = 0; i <= num; i++) {
			V[i] = memory[I + i];
		}
	}
	
	// move pc
	pc += 2;
}
