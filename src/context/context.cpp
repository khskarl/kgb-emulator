#include <cstdlib>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include "context.hpp"
#include "../debug.hpp"

static sf::Font font;
static sf::Text debugText;

static sf::RenderWindow window;
static sf::Texture displayTexture;
static sf::Sprite displaySprite; // SFML needs a sprite to render a texture
static uint8_t* displayBuffer = nullptr;

static float speedInput = 1.0f;

// Placeholder
uint8_t  pixels[160 * 144 * 4];
uint8_t* joypad = nullptr;

// Placeholder
bool prevShouldHalt = false;
bool prevShouldStep = false;

bool Context::SetupContext (const int scale = 1) {
	font.loadFromFile("../resources/fonts/OpenSans-Bold.ttf");
	debugText.setFont(font);
	debugText.setColor(sf::Color::Magenta);
	debugText.setCharacterSize(15);
	debugText.setString("Debug :D");

	window.create(sf::VideoMode(160 * scale, 144 * scale), "Hi, I'm a secret message :D");

	displayTexture.create(160, 144);
	displayTexture.setSmooth(false);
	displaySprite.setTexture(displayTexture);

	sf::Vector2u textureSize = displayTexture.getSize();
	sf::Vector2u windowSize = window.getSize();

	float scaleX = (float) windowSize.x / textureSize.x;
	float scaleY = (float) windowSize.y / textureSize.y;

	displaySprite.setScale(scaleX, scaleY);

	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);
	return true;
}

void Context::DestroyContext () {
	if (window.isOpen())
		window.close();
}

void Context::HandleEvents () {
	sf::Event event;
	sf::Clock deltaClock;
	while (window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
			window.close();
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				window.close();
			}
			else if (event.key.code == sf::Keyboard::Num0) {
				speedInput = 1.0f;
			}
			else if (event.key.code == sf::Keyboard::Num9) {
				speedInput = 0.5f;
			}
			else if (event.key.code == sf::Keyboard::Num8) {
				speedInput = 0.25f;
			}
			else if (event.key.code == sf::Keyboard::Num7) {
				speedInput = 0.1f;
			}
			else if (event.key.code == sf::Keyboard::Num6) {
				speedInput = 0.05f;
			}
			else if (event.key.code == sf::Keyboard::Num5) {
				speedInput = 0.01f;
			}
		}
	}

	ImGui::SFML::Update(window, deltaClock.restart());

	joypad[0] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
	joypad[1] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	joypad[2] = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	joypad[3] = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	joypad[4] = sf::Keyboard::isKeyPressed(sf::Keyboard::H);
	joypad[5] = sf::Keyboard::isKeyPressed(sf::Keyboard::G);
	joypad[6] = sf::Keyboard::isKeyPressed(sf::Keyboard::B);
	joypad[7] = sf::Keyboard::isKeyPressed(sf::Keyboard::N);
}

void Context::RenderDisplay () {
	window.clear();

	CopyDisplayBuffer(displayBuffer);
	displayTexture.update(pixels);
	window.draw(displaySprite);

	ImGui::SFML::Render(window);
	window.display();
}

void Context::SetJoypadBuffer (uint8_t* const buffer) {
	assert(buffer != nullptr);
	joypad = buffer;
}

// TODO: Use opengl texture binding and GL_R8UI color format to use the VRAM di-
//rectly as a pixel buffer.
// http://fr.sfml-dev.org/forums/index.php?topic=17847.0
// http://www.sfml-dev.org/documentation/2.4.1/classsf_1_1Texture.php
void Context::SetDisplayBuffer (uint8_t* const buffer) {
	assert(buffer != nullptr);
	displayBuffer = buffer;
}

// HACK: Temporary solution
// Currently, Instead of accessing the VRAM memory directly, we convert it's contents to a
//color format that SFML can understand.
void Context::CopyDisplayBuffer (uint8_t* const buffer) {
	for (size_t i = 0; i < 160 * 144 * 4; i += 4) {
		const sf::Vector3<uint8_t> shades[] = {
			sf::Vector3<uint8_t>(155, 188, 15),
			sf::Vector3<uint8_t>(139, 172, 15),
			sf::Vector3<uint8_t>( 48,  98, 48),
			sf::Vector3<uint8_t>( 15,  56, 15),
		};
		pixels[i]     = shades[buffer[i / 4]].x;
		pixels[i + 1] = shades[buffer[i / 4]].y;
		pixels[i + 2] = shades[buffer[i / 4]].z;
		pixels[i + 3] = 255;
	}
}

void Context::SetTitle (std::string title) {
	window.setTitle(title);
}

bool Context::IsOpen () {
	return window.isOpen();
}

bool Context::ShouldHalt () {
	bool curr = sf::Keyboard::isKeyPressed(sf::Keyboard::P);
	bool prev = prevShouldHalt;
	prevShouldHalt = curr;
	return curr ^ prev && curr;
}

bool Context::ShouldStep () {
	bool curr = sf::Keyboard::isKeyPressed(sf::Keyboard::O);
	bool prev = prevShouldStep;
	prevShouldStep = curr;
	return curr ^ prev && curr;
}

void Context::SetDebugText (std::string text) {
	debugText.setString(text);
}

float Context::GetSpeedInput () {
	return speedInput;
}
