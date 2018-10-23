#include <iostream>
#include <iomanip>
using namespace std;

#include "chip8.h"

chip8 cpu;

int main() {

	// init
	cpu.init();

	// game loop
	// derp testing
	cout << "START\n---\n";
	cout << hex;
	while (true) {
		cpu.emulateCycle();

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

		if (cpu.opcode == 0) {
			break;
		}
	}
	cout << "END";

	int n;
	cin >> n;

	return 0;
}