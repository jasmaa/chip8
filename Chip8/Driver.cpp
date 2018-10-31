#include <iostream>
#include <iomanip>
#include <SDL.h>
using namespace std;

#include "chip8.h"

#define BUZZ_PATH "buzz.wav"
static Uint8 * audio_pos;
static Uint32 audio_len;

Chip8 cpu;

int width = 512;
int height = width / 2;
int ratio = width / 128;

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;
SDL_Event e;

bool quit = false;

void buzz_callback(void *userdata, Uint8 *stream, int len) {

	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);

	audio_pos += len;
	audio_len -= len;
}

int main(int argc, char* args[]) {

	if (argc != 2) {
		return 0;
	}

	// init
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		return 1;
	}
	window = SDL_CreateWindow("Chip 8 Emulator - v0.1.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

	Uint32 * pixels = new Uint32[width * height];

	// sound init
	static Uint32 wav_length;
	static Uint8 * wav_buffer;
	static SDL_AudioSpec wav_spec;
	if (SDL_LoadWAV(BUZZ_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL) {
		cout << "Cannot load file";
		return 1;
	}
	// set call back
	wav_spec.callback = buzz_callback;
	wav_spec.userdata = NULL;
	audio_pos = wav_buffer;
	audio_len = wav_length;
	// open audio
	if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
		cout << "Could not open audio";
		return 1;
	}

	// load rom and init cpu
	cpu.init(args[1]);

	// game loop
	while (!quit) {

		// key detection
		cpu.key_pressed = false;
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
				cpu.key_pressed = true;
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

		cpu.emulateCycle();

		if (cpu.has_buzz) {
			SDL_PauseAudio(0);
		}
		else {
			// reset
			SDL_PauseAudio(1);
			audio_pos = wav_buffer;
			audio_len = wav_length;
		}

		// render if draw called
		if (cpu.has_draw) {

			ratio = cpu.extended_enabled ? width / 128 : width / 64;

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
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);
	SDL_Quit();

	return 0;
}
