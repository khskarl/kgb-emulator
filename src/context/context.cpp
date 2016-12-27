#include "context.hpp"

#include <iostream>
#include <SDL.h>

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture *texture = nullptr;

static bool shouldQuit = false;

static uint8_t* keypad;

void Context::SetKeymap (uint8_t* keypadMemory) {
	keypad = keypadMemory;
}

bool Context::SetupContext () {
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	int scale = 10;
	int flags = SDL_WINDOW_RESIZABLE;
	window = SDL_CreateWindow("Chip8 Emulator",
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          64 * scale, 32 * scale,
	                          flags);

	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		DestroyContext();
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		DestroyContext();
		return false;
	}

	texture = SDL_CreateTexture(renderer,
	                            SDL_GetWindowPixelFormat(window),
	                            SDL_TEXTUREACCESS_TARGET,
	                            64, 32);

	if (texture == nullptr) {
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		DestroyContext();
	}

	shouldQuit = false;

	return true;
}

void Context::DestroyContext () {
	if (texture)
		SDL_DestroyTexture(texture);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}

void Context::UpdateKeypad () {
	const uint8_t *keys = SDL_GetKeyboardState(NULL);
	keypad[0x1] = keys[SDL_SCANCODE_1];
	keypad[0x2] = keys[SDL_SCANCODE_2];
	keypad[0x3] = keys[SDL_SCANCODE_3];
	keypad[0xC] = keys[SDL_SCANCODE_4];
	keypad[0x4] = keys[SDL_SCANCODE_Q];
	keypad[0x5] = keys[SDL_SCANCODE_W];
	keypad[0x6] = keys[SDL_SCANCODE_E];
	keypad[0xD] = keys[SDL_SCANCODE_R];
	keypad[0x7] = keys[SDL_SCANCODE_A];
	keypad[0x8] = keys[SDL_SCANCODE_S];
	keypad[0x9] = keys[SDL_SCANCODE_D];
	keypad[0xE] = keys[SDL_SCANCODE_F];
	keypad[0xA] = keys[SDL_SCANCODE_Z];
	keypad[0x0] = keys[SDL_SCANCODE_X];
	keypad[0xB] = keys[SDL_SCANCODE_C];
	keypad[0xF] = keys[SDL_SCANCODE_V];
}

void Context::HandleEvents () {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			shouldQuit = true;
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				shouldQuit = true;
				break;
			}
		}
	}
}

void Context::Draw (uint8_t displayBuffer[64 * 32]) {
	SDL_SetRenderTarget(renderer, texture);

	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255);
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			if (displayBuffer[i * 64 + j]) {
				SDL_RenderDrawPoint(renderer, j, i);
			}
		}
	}

	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);
}

void Context::Sleep (float ms) {
	SDL_Delay(ms);
}

bool Context::ShouldQuit () {
	return shouldQuit;
}
