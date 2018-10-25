#include <iostream>
#include <iomanip>
#include <SDL.h>
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

/*
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
*/


int width = 256;
int height = 128;
int ratio = width / 64;

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;
SDL_Event e;

bool quit = false;

int main(int argc, char* args[]) {

	// init
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Chip 8 Emualtor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

	Uint32 * pixels = new Uint32[width * height];

	cpu.init();

	cout << "START\n---\n";
	cout << hex;

	while (!quit) {

		cpu.emulateCycle();

		// event detection
		SDL_PollEvent(&e);
		switch (e.type) {
		case SDL_QUIT:
			quit = true;
			break;
		}

		// copy pixels
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (cpu.gfx[j / ratio][i / ratio] == 0) {
					pixels[i + width * j] = 0;
				}
				else {
					pixels[i + width * j] = 0xFFFFFF;
				}
			}
		}

		SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));

		// render
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	// Free resources
	delete[] pixels;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
	SDL_Quit();

	return 0;
}
