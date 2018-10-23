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
	while (true) {
		cpu.emulateCycle();

		// print state
		cout << "opcode: " << std::hex << cpu.opcode << "\n";
		cout << "V: ";
		for (int i = 0; i < 16; i++) {
			cout << std::hex << unsigned(cpu.V[i]) << " ";
		}
		cout << "\n";
		cout << "pc: " << cpu.pc << "\n";
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