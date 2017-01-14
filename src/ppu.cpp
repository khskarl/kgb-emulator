#include "ppu.hpp"

#include "cpu.hpp"

#include <algorithm>
#include <iostream>
#include "debug.hpp"

PPU::PPU () {}
PPU::~PPU () {}

void PPU::Initialize(CPU* _cpu, MMU* _mmu) {
	assert("CPU reference invalid" && _cpu != nullptr);
	assert("MMU reference invalid" && _mmu != nullptr);
	cpu = _cpu;
	mmu = _mmu;

	// FeedRandomToBackground ();
	// FeedPatternToBackground ();
}

void PPU::StepUpdate (uint16_t cycles) {
	// SetLCDStatus()

	if (IsDisplayEnabled() == false)
		return;

	scanlineCounter -= cycles;

	if (scanlineCounter > 0)
		return;

	uint8_t currLine = NextScanline();

	ResetScanlineCounter();

	if (currLine == 144)
		cpu->RequestInterrupt(0);

	if (currLine > 153)
		ResetScanline();
	else if (currLine < 144) {
		DrawScanline(currLine);
	}
}

uint8_t* PPU::GetDisplayBuffer () {
	return &displayBuffer[0];
}

uint8_t ComputeBitColorID (uint16_t line, uint8_t numBit) {
	return (line & (0x8000 >> numBit)) >> (14 - numBit) |
	       (line & (0x80   >> numBit)) >> (7  - numBit);
}

void PPU::DrawScanline (uint8_t line) {
	uint8_t scrollY = mmu->ReadByte(SCROLLY);
	uint8_t scrollX = mmu->ReadByte(SCROLLX);
	uint16_t bgTilesMapAddress = GetBackgroundTilesAddress();
	uint16_t tilesAddress = GetTilesAddress();

	uint8_t iScrolled = scrollY + line;
	uint8_t iTile = (iScrolled) / 8;
	// uint8_t iTilePixel = iTile * 32;

	for (size_t jPixel = 0; jPixel < 160; jPixel += 1) {
		uint8_t jScrolled = scrollX + jPixel;
		uint8_t jTile = (jScrolled) / 8;

		// FIXME: Getting wrong tiles
		uint16_t tileIdAddress = bgTilesMapAddress + iTile * 32 + jTile;
		uint8_t untreatedByte = mmu->ReadByte(tileIdAddress);




		int16_t tileID;
		if (tilesAddress == 0x8000)
			tileID = untreatedByte;
		else
			tileID = reinterpret_cast<int8_t&>(untreatedByte);

		// std::cout << "i: " << std::hex << (int) iTile << '\n';
		// std::cout << "j: " << std::hex << (int) jTile << '\n';
		// std::cout << "tileIdAddress: " << std::hex << (int) tileIdAddress << '\n';
		// std::cout << "tileId:        " << std::hex << (int) tileID << '\n';
		// assert(tileIdAddress != 0x9910);
		// if (tileIdAddress == 0x9910) {
		// 	while (1) {}
		// }

		uint16_t tileLocation = tilesAddress;
		if (tilesAddress == 0x8000)
			tileLocation += tileID * 16;
		else
			tileLocation += (tileID + 128) * 16;
		// if (tileIdLocation == 0x990F)
		// 	tileLocation = 0x80C0;
		uint16_t tile = mmu->ReadWord(tileLocation + (iScrolled % 8) * 2);
		// std::cout << "[" << std::hex << tileLocation << "] " << tileID << " : " << tile  << "\n";


		if (tileID != 0)
			displayBuffer[line * 160 + jPixel] = 3;

		displayBuffer[line * 160 + jPixel] = ComputeBitColorID(tile, jScrolled % 8);
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

uint16_t PPU::GetTilesAddress () { // FIXME: Double check, may be inverted
	uint8_t lcdControl = mmu->ReadByte(LCDCTRL);
	if ((lcdControl & 0x16) == 0x16) {
		return 0x8800;
	} else {
		return 0x8000;
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
	uint8_t* pScanline = mmu->GetIORef(CURLINE);
	(*pScanline) += 1;
	return *pScanline;
}

void PPU::ResetScanline () {
	(*mmu->GetIORef(CURLINE)) = 0;
}

void PPU::ResetScanlineCounter () {
	scanlineCounter = 456;
}

bool PPU::IsDisplayEnabled () {
	return (mmu->ReadByte(LCDCTRL) & 0x80) == 0x80;
}
