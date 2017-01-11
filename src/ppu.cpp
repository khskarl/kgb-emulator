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
	// FeedPatternToBackground ();
}

void PPU::StepUpdate (uint16_t cycles) {
	if (IsDisplayEnabled() == false)
		return;

	scanlineCounter -= cycles;

	if (scanlineCounter > 0)
		return;

	uint8_t currLine = NextScanline();

	ResetScanlineCounter();

	// if (currLine == 144)
	// 	RequestInterrupt

	if (currLine > 153)
		ResetScanline();
	else if (currLine < 144) {
		DrawScanline(currLine);
		// RenderBackgroundBuffer ();
		// RenderBackgroundToDisplay ();
	}
	else {
		// We are in VBLANK dude :D
	}
}

uint8_t* PPU::GetDisplayBuffer () {
	return &displayBuffer[0];
}

uint8_t ComputeBitColorID (uint16_t tileLine, uint8_t bitPosition) {
	return ((tileLine & 0x8000) >> (8 + bitPosition - 1)) |
	       ((tileLine & 0x80  ) >> (    bitPosition - 1));
	// return ((tileLine & 0x80  ) >> (    bitPosition - 1)) |
  //        ((tileLine & 0x8000) >> (8 + bitPosition - 1));
}

void PPU::DrawScanline (uint8_t line) {
	for (size_t jPixel = 0; jPixel < 160; jPixel += 1) {
		displayBuffer[line * 160 + jPixel] = 3;
	}
}

void PPU::RenderBackgroundBuffer () {
	// uint8_t* bgTileTable = mmu->GetMemoryRef(0x8800);
	// uint8_t scrollX = *mmu->GetMemoryRef(0xFF43);
	// uint8_t scrollY = *mmu->GetMemoryRef(0xFF42);
	uint16_t bgTilesAddress = GetBackgroundTilesAddress();
	uint16_t tilesAddress = GetTilesAddress();
//reinterpret_cast<int8_t&>(value);
	for (std::size_t iTile = 0; iTile < 32; iTile++) {
		for (std::size_t jTile = 0; jTile < 32; jTile++) {
			uint16_t tileIDAddress = bgTilesAddress + iTile * 32 + jTile;
			int16_t tileID = mmu->ReadByte(tileIDAddress);

			if (tilesAddress == 0x8800)
				tileID -= 128;

			for (std::size_t iPixel = 0; iPixel < 8; iPixel++) {
				uint16_t line = mmu->ReadWord(tilesAddress + tileID * 16 + iPixel);

				uint8_t i = iTile * 8 + iPixel;

				for (std::size_t jPixel = 0; jPixel < 8; jPixel++) {
					uint8_t j = jTile * 8 + jPixel;
					backgroundBuffer[i * 256 + j] = ComputeBitColorID(line, jPixel);

				}
			}
		}
	}
}

void PPU::RenderBackgroundToDisplay () {
	uint8_t scrollY = mmu->ReadByte(SCROLLY);
	uint8_t scrollX = mmu->ReadByte(SCROLLX);

	for (size_t i = 0; i < 144; i++) {
		uint8_t iScrolled = i + scrollY;
		if (iScrolled < 0)
			iScrolled += 256;
		else if (iScrolled > 255)
			iScrolled = iScrolled % 256;

		for (size_t j = 0; j < 160; j++) {
			uint8_t jScrolled = j + scrollX;

			if (jScrolled < 0)
				jScrolled += 256;
			else if (jScrolled > 255)
				jScrolled = jScrolled % 256;

			displayBuffer[i * 160 + j] = backgroundBuffer[iScrolled * 256 + jScrolled];
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
	// for (size_t i = 0; i < 256; i += 1) {
	// 	for (size_t j = 0; j < 256; j += 1) {
	// 		int luminosity = ((i * j) % 64) / 16;
	// 		backgroundBuffer[i * 256 + j] = luminosity;
	// 	}
	// }

	for (size_t i = 0; i < 256; i += 1) {
		for (size_t j = 0; j < 256; j += 1) {
			int luminosity = ((j + i) % 64) / 16;
			backgroundBuffer[i * 256 + j] = luminosity;
		}
	}
}

uint16_t PPU::GetTilesAddress () {
	uint8_t lcdControl = mmu->ReadByte(LCDCTRL);
	if ((lcdControl & 0x16) == 0x16) {
		return 0x8000;
	} else {
		return 0x8800;
	}
}

uint16_t PPU::GetBackgroundTilesAddress () {
	uint8_t lcdControl = mmu->ReadByte(LCDCTRL);
	if ((lcdControl & 0x8) == 0x8) {
		return 0x9C00;
	} else {
		return 0x9800;
	}
}

uint8_t PPU::NextScanline () {
	uint8_t* pScanline = mmu->GetMemoryRef(CURLINE);
	(*pScanline) += 1;
	return *pScanline;
}

void PPU::ResetScanline () {
	mmu->WriteByte(CURLINE, 0);
}

void PPU::ResetScanlineCounter () {
	scanlineCounter = 456;
}

bool PPU::IsDisplayEnabled () {
	return (mmu->ReadByte(LCDCTRL) & 0x80) == 0x80;
}
