﻿#include <iostream>
#include <iomanip>
//#include <SDL.h>
using namespace std;

#include "chip8.h"

chip8 cpu;

void printState() {
	// print state
	cout << "pc after exec: " << cpu.pc << "\n";
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

	cpu.init();
	cout << "START\n---\n";
	cout << hex;
	while (true) {
		cpu.emulateCycle();
		printState();
		cout << "\n";
	}
	cout << "END\n---\n";
	
	return 0;
}

/*
int width = 256;
int height = 128;
int ratio = width / 64;

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Event e;

bool quit = false;

int main(int argc, char* args[]) {

	// init
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	cpu.init();

	cout << "START\n---\n";
	cout << hex;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			cpu.emulateCycle();

			// quit on exit
			if (e.type == SDL_QUIT) {
				quit = true;
			}

			// render
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					if (cpu.gfx[i][j] == 1) {
						SDL_Rect region;
						region.x = j * ratio;
						region.y = i * ratio;
						region.w = ratio;
						region.h = ratio;
						SDL_RenderFillRect(renderer, &region);
					}
				}
			}
			
			//SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
		}
	}

	// Free resources
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
	SDL_Quit();

	return 0;
}
*/