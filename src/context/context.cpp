#include "context.hpp"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cstdlib>
#include "../debug.hpp"

static sf::RenderWindow window;
static sf::Texture displayTexture;
static sf::Sprite displaySprite; // SFML needs a sprite to render a texture
static uint8_t* displayBuffer = nullptr;

// Placeholder
uint8_t pixels[160 * 144 * 4];

bool Context::SetupContext (int scale = 1) {
	window.create(sf::VideoMode(160 * scale, 144 * scale), "Hello SFML! :D");

	displayTexture.create(160, 144);
	displayTexture.setSmooth(false);
	displaySprite.setTexture(displayTexture);

	sf::Vector2u textureSize = displayTexture.getSize();
	sf::Vector2u windowSize = window.getSize();

	float scaleX = (float) windowSize.x / textureSize.x;
	float scaleY = (float) windowSize.y / textureSize.y;

	displaySprite.setScale(scaleX, scaleY);

	for (size_t i = 0; i < 160 * 144 * 4; i += 4) {
		int luminosity = rand() % 256;
		pixels[i] = luminosity;
		pixels[i + 1] = luminosity;
		pixels[i + 2] = luminosity;
		pixels[i + 3] = 255;
	}


	return true;
}

void Context::DestroyContext () {
	if (window.isOpen())
		window.close();
}

void Context::HandleEvents () {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				window.close();
			}
		}
	}
}

void Context::RenderDisplay () {
	window.clear();
	displayTexture.update(pixels);
	window.draw(displaySprite);

	window.display();
}

void Context::SetTitle (std::string title) {
	window.setTitle(title);
}

bool Context::IsOpen () {
	return window.isOpen();
}

// TODO: Use opengl texture binding and GL_R8UI color format to use the VRAM di-
//rectly as a pixel buffer.
// http://fr.sfml-dev.org/forums/index.php?topic=17847.0
// http://www.sfml-dev.org/documentation/2.4.1/classsf_1_1Texture.php
void Context::SetDisplayBuffer (uint8_t* buffer) {
	displayBuffer = buffer;
}

// HACK: Temporary solution
// Instead of accessing the VRAM memory directly, we convert it's contents to a
//color format that SFML can understand.
void Context::CopyDisplayBuffer (uint8_t* buffer) {
	for (size_t i = 0; i < 160 * 144 * 4; i += 4) {
		int luminosity = buffer[i / 4];
		pixels[i] = luminosity;
		pixels[i + 1] = luminosity;
		pixels[i + 2] = luminosity;
		pixels[i + 3] = 255;
	}
}
