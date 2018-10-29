#include <iostream>
#include <iomanip>
#include <SDL.h>
using namespace std;

#include "chip8.h"

chip8 cpu;

int width = 1028;
int height = 512;
int ratio = width / 64;

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;
SDL_Event e;

bool quit = false;

int main(int argc, char* args[]) {

	// init
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

	Uint32 * pixels = new Uint32[width * height];

	cpu.init();


	while (!quit) {
		cpu.emulateCycle();

		// key detection
		SDL_PollEvent(&e);
		switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;

			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
					case SDLK_1:
						cpu.key[0x1] = 0;
						break;
					case SDLK_2:
						cpu.key[0x2] = 0;
						break;
					case SDLK_3:
						cpu.key[0x3] = 0;
						break;
					case SDLK_4:
						cpu.key[0xC] = 0;
						break;
					case SDLK_q:
						cpu.key[0x4] = 0;
						break;
					case SDLK_w:
						cpu.key[0x5] = 0;
						break;
					case SDLK_e:
						cpu.key[0x6] = 0;
						break;
					case SDLK_r:
						cpu.key[0xD] = 0;
						break;
					case SDLK_a:
						cpu.key[0x7] = 0;
						break;
					case SDLK_s:
						cpu.key[0x8] = 0;
						break;
					case SDLK_d:
						cpu.key[0x9] = 0;
						break;
					case SDLK_f:
						cpu.key[0xE] = 0;
						break;
					case SDLK_z:
						cpu.key[0xA] = 0;
						break;
					case SDLK_x:
						cpu.key[0x0] = 0;
						break;
					case SDLK_c:
						cpu.key[0xB] = 0;
						break;
					case SDLK_v:
						cpu.key[0xF] = 0;
						break;
				}
				break;

			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
					case SDLK_1:
						cpu.key[0x1] = 1;
						break;
					case SDLK_2:
						cpu.key[0x2] = 1;
						break;
					case SDLK_3:
						cpu.key[0x3] = 1;
						break;
					case SDLK_4:
						cpu.key[0xC] = 1;
						break;
					case SDLK_q:
						cpu.key[0x4] = 1;
						break;
					case SDLK_w:
						cpu.key[0x5] = 1;
						break;
					case SDLK_e:
						cpu.key[0x6] = 1;
						break;
					case SDLK_r:
						cpu.key[0xD] = 1;
						break;
					case SDLK_a:
						cpu.key[0x7] = 1;
						break;
					case SDLK_s:
						cpu.key[0x8] = 1;
						break;
					case SDLK_d:
						cpu.key[0x9] = 1;
						break;
					case SDLK_f:
						cpu.key[0xE] = 1;
						break;
					case SDLK_z:
						cpu.key[0xA] = 1;
						break;
					case SDLK_x:
						cpu.key[0x0] = 1;
						break;
					case SDLK_c:
						cpu.key[0xB] = 1;
						break;
					case SDLK_v:
						cpu.key[0xF] = 1;
						break;
				}
				break;
		}

		if (cpu.hasDraw) {
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
