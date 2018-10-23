#include <iostream>
#include <iomanip>
using namespace std;

#include "chip8.h"

chip8 cpu;

void printState() {
	// print state
	cout << "pc: " << cpu.pc << "\n";
	cout << "opcode: " << cpu.opcode << "\n";
	cout << "V: ";
	for (int i = 0; i < 16; i++) {
		cout << unsigned(cpu.V[i]) << " ";
	}
	cout << "\n";
	cout << "I: " << cpu.I << "\n";
	cout << "---\n";
}

void printScrn() {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			if (cpu.gfx[i][j] == 0) {
				cout << "0";
			}
			else {
				cout << "1";
			}
		}
		cout << "\n";
	}
}

int main() {

	// init
	cpu.init();

	// game loop
	// derp testing
	cout << "START\n---\n";
	cout << hex;
	while (true) {
		cpu.emulateCycle();
		//printState();
		printScrn();
	}
	cout << "END";

	int n;
	cin >> n;

	return 0;
}