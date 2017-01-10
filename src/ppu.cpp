#include "ppu.hpp"

#include <algorithm>
#include <iostream>
#include "debug.hpp"

PPU::PPU () {}
PPU::~PPU () {}

void PPU::Initialize(MMU* _mmu) {
	assert("MMU is nullptr" && _mmu != nullptr);
	mmu = _mmu;
	// FeedRandomToBackground ();
	FeedPatternToBackground ();
}

void PPU::StepUpdate () {
	RenderBackgroundBuffer ();
	RenderBackgroundToDisplay ();
}

uint8_t* PPU::GetDisplayBuffer () {
	return &displayBuffer[0];
}

uint8_t ComputeBitColorID (uint16_t tileLine, uint8_t bitPosition) {
	return ((tileLine & 0x8000) >> (8 + bitPosition - 1)) |
	       ((tileLine & 0x80  ) >> (    bitPosition - 1));
}


void PPU::RenderBackgroundBuffer () {
	// uint8_t* bgTileTable = mmu->GetMemoryRef(0x8800);
	// uint8_t scrollX = *mmu->GetMemoryRef(0xFF43);
	// uint8_t scrollY = *mmu->GetMemoryRef(0xFF42);

	for (std::size_t iTile = 0; iTile < 32; iTile++) {
		for (std::size_t jTile = 0; jTile < 32; jTile++) {
			uint16_t tileIDAddress = 0x9800 + iTile * 32 + jTile;
			uint8_t tileID = mmu->ReadByte(tileIDAddress);


			for (std::size_t iPixel = 0; iPixel < 8; iPixel++) {
				uint16_t line = mmu->ReadWord(0x8000 + tileID + iPixel);
				uint8_t i = iTile + iPixel;
				for (std::size_t jPixel = 0; jPixel < 8; jPixel++) {
					uint8_t j = jTile + jPixel;

					backgroundBuffer[i * 32 + j] = ComputeBitColorID(line, jPixel);
				}
			}

		}
	}
}

void PPU::RenderBackgroundToDisplay () {
	uint8_t scrollX = mmu->ReadByte(0xFF43);
	uint8_t scrollY = mmu->ReadByte(0xFF42);

	for (size_t i = 0; i < 144; i++) {
		uint8_t iScrolled = i + scrollY;
		if (iScrolled < 0)
			iScrolled += 144;
		else if (iScrolled > 143)
			iScrolled = iScrolled % 144;

		for (size_t j = 0; j < 160; j++) {
			uint8_t jScrolled = j + scrollX;

			if (jScrolled < 0)
				jScrolled += 160;
			else if (jScrolled > 159)
				jScrolled = jScrolled % 160;

			displayBuffer[i * 160 + j] = backgroundBuffer[iScrolled * 160 + jScrolled];
		}
	}
}

void PPU::FeedRandomToDisplay () {
	for (size_t i = 0; i < 160 * 144; i += 1) {
		int luminosity = rand() % 4;
		displayBuffer[i] = luminosity;
	}
}

void PPU::FeedRandomToBackground () {
	for (size_t i = 0; i < 256 * 256; i += 1) {
		int luminosity = rand() % 4;
		backgroundBuffer[i] = luminosity;
	}
}

void PPU::FeedPatternToBackground () {
	for (size_t i = 0; i < 256; i += 1) {
		for (size_t j = 0; j < 256; j += 1) {
			int luminosity = ((i * j) % 64) / 16;
			backgroundBuffer[i * 256 + j] = luminosity;
		}
	}
}
