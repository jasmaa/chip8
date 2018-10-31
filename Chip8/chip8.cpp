#include <thread>
#include <chrono>
#include <fstream>
#include <iomanip>
using namespace std;

#include "chip8.h"

void Chip8::init(string fname) {

	// read fonts
	fstream filestr_font("fontchip8", fstream::in | fstream::binary);
	unsigned char n;
	int i = 0;
	while (!filestr_font.eof()) {
		n = filestr_font.get();
		memory[i] = n;
		i++;
	}
	filestr_font.close();

	fstream filestr_font48("fontsuperchip48", fstream::in | fstream::binary);
	while (!filestr_font48.eof()) {
		n = filestr_font48.get();
		memory[i] = n;
		i++;
	}
	filestr_font48.close();

	// read file into memory
	fstream filestr(fname, fstream::in | fstream::binary);
	i = 0;
	while (!filestr.eof()) {
		n = filestr.get();
		memory[i + 512] = n;
		i++;
	}
	filestr.close();
	
	pc = 0x200;

}

void Chip8::emulateCycle() {

	has_draw = false;

	// fetch opcode
	opcode = unsigned(memory[pc] << 8 | memory[pc + 1]);

	// decode and execute opcode

	// scroll screen down
	if (unsigned(opcode & 0xFFF0) == 0x00C0) {
		int n = unsigned(opcode & 0xF);
		for (int i = 0; i < n; i++) {
			// move screen down
			for (int j = 63; j > 0; j--) {
				for (int k = 0; k < 128; k++) {
					gfx[j][k] = gfx[j - 1][k];
				}
			}
			for (int k = 0; k < 128; k++) {
				gfx[0][k] = 0;
			}
		}
	}
	// clear screen
	else if (opcode == 0x00E0) {
		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 128; j++) {
				gfx[i][j] = 0;
			}
		}
	}
	// return from subroutine
	else if (opcode == 0x00EE) {
		pc = call_stack.top();
		call_stack.pop();
	}
	// scroll screen right 4
	else if (opcode == 0x00FB) {
		for (int i = 0; i < 4; i++) {
			// move screen right
			for (int j = 127; j > 0; j--) {
				for (int k = 0; k < 64; k++) {
					gfx[k][j] = gfx[k][j - 1];
				}
			}
			for (int j = 0; j < 64; j++) {
				gfx[j][0] = 0;
			}
		}
	}
	// scroll screen left 4
	else if (opcode == 0x00FC) {
		for (int i = 0; i < 4; i++) {
			// move screen right
			for (int j = 0; j < 127; j++) {
				for (int k = 0; k < 64; k++) {
					gfx[k][j] = gfx[k][j + 1];
				}
			}
			for (int j = 0; j < 64; j++) {
				gfx[j][127] = 0;
			}
		}
	}
	// disable extended
	else if (opcode == 0x00FE) {
		extended_enabled = false;
	}
	// enable extended
	else if (opcode == 0x00FF) {
		extended_enabled = true;
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
		if (V[0xF & (opcode >> 8)] == unsigned(0xFF & opcode)) {
			pc += 2;
		}
	}
	// skip on VX != NN
	else if (unsigned(opcode & 0xF000) == 0x4000) {
		if (V[0xF & (opcode >> 8)] != unsigned(0xFF & opcode)) {
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
	// set VX = VY - VX
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
	else if (unsigned(opcode & 0xF00F) == 0x800E) {
		V[0xF] = (V[0xF & (opcode >> 8)] >> 7) & 0x1;
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
		V[0xF & (opcode >> 8)] = (rand() % 0xFF) & opcode;
	}
	// draw
	else if (unsigned(opcode & 0xF000) == 0xD000) {

		has_draw = true;
		V[0xF] = 0;

		int scrn_width = extended_enabled ? 128 : 64;
		int scrn_height = extended_enabled ? 64 : 32;

		int row = V[0xF & (opcode >> 4)];
		int col = V[0xF & (opcode >> 8)];
		int height = 0xF & opcode;
		int width = 8;
		int mem = I;

		// extended drawing
		if (height == 0) {
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 16; j++) {

					short data = unsigned((memory[mem] << 8) | memory[mem + 1]);

					// set VF on set to unset
					if (unsigned(gfx[(row + i) % scrn_height][(col + j) % scrn_width]) == 0x1 && unsigned((data >> (15 - j)) & 0x1) == 0x1) {
						V[0xF] = 1;
					}
					gfx[(row + i) % scrn_height][(col + j) % scrn_width] ^= (data >> (15 - j)) & 0x1;

				}
				mem += 2;
			}
		}
		// normal drawing
		else {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < 8; j++) {

					// set VF on set to unset
					if (unsigned(gfx[(row + i) % scrn_height][(col + j) % scrn_width]) == 0x1 && unsigned((memory[mem] >> (7 - j)) & 0x1) == 0x1) {
						V[0xF] = 1;
					}
					gfx[(row + i) % scrn_height][(col + j) % scrn_width] ^= (memory[mem] >> (7 - j)) & 0x1;

				}
				mem++;
			}
		}
	}
	// skip if key is in VX
	else if (unsigned(opcode & 0xF0FF) == 0xE09E) {
		if (key[V[0xF & (opcode >> 8)]] == 1) {
			pc += 2;
		}
	}
	// skip if key is not in VX
	else if (unsigned(opcode & 0xF0FF) == 0xE0A1) {
		if (key[V[0xF & (opcode >> 8)]] == 0) {
			pc += 2;
		}
	}
	// set VX = delay timer
	else if (unsigned(opcode & 0xF0FF) == 0xF007) {
		V[0xF & (opcode >> 8)] = delay_timer;
	}
	// wait key and store in VX
	else if (unsigned(opcode & 0xF0FF) == 0xF00A) {

		if (!key_pressed) {
			pc -= 2;
		}
		else {
			for (int i = 0; i < 0xF; i++) {
				if (key[i] == 0x1) {
					V[0xF & (opcode >> 8)] = i;
				}
			}
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
	// I to font for superchip
	else if (unsigned(opcode & 0xF0FF) == 0xF030) {
		I = V[0xF & (opcode >> 8)] * 10 + 0x51;
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
		int num = 0xF & (opcode >> 8);
		for (int i = 0; i <= num; i++) {
			memory[(I + i) % 4096] = V[i];
		}
	}
	// reg load
	else if (unsigned(opcode & 0xF0FF) == 0xF065) {
		int num = 0xF & (opcode >> 8);
		for (int i = 0; i <= num; i++) {
			V[i] = memory[(I + i) % 4096];
		}
	}
	
	// move pc
	pc += 2;

	// move timers
	if (delay_timer > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(3));
		delay_timer--;
	}
	if (sound_timer > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		sound_timer--;
		has_buzz = true;
	}
	else {
		has_buzz = false;
	}

	// default wait time
	std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}
