#include <cstring> // std::memcpy
#include <iostream>

#include "gameboy.hpp"
#include "mmu.hpp"
#include "cpu.hpp"

#include "debug.hpp"

MMU::MMU () {}
MMU::~MMU () {}

void MMU::Initialize (GameBoy* const argGameboy) {
	gameboy = argGameboy;

	WriteByte(LCDCTRL, 0x91);

	const auto memzero = [] (auto& array) {
		std::memset(array, 0, sizeof(array) / sizeof(array[0]));
	};

	memzero(bios);
	memzero(rom);
	memzero(eram);
	memzero(wram);
	memzero(zram);
	memzero(vram);
	memzero(oam);
	memzero(io);

	GetMemoryRef(JOYPAD)[0]  = 0xFF;
	GetMemoryRef(JOYPAD)[1]  = 0xFF;
	*GetMemoryRef(LCDCTRL)   = 0x91;
}

uint8_t MMU::ReadByte (uint16_t address) {
	return *GetMemoryRef(address);
}

uint16_t MMU::ReadWord (uint16_t address) {
	return ReadByte(address) | (ReadByte(address + 1) << 8);
}

uint8_t MMU::ReadClockFrequency () {
	return ReadByte(TMC) & 0x3;
}


void MMU::WriteByte (uint16_t address, uint8_t value) {
	if (0xFB00 == address) {
		std::cout << std::hex << (int) value << "\n";
		gameboy->SetHalt(true);
	}

	if (0x4000 <= address && address <= 0x7FFF) {
		HandleRomBankSwitch(address);
		return;
	}
	// Reset scanline if we try to write to it
	else if (address == CURLINE) {
		io[address & 0x7F] = 0;
		return;
	}
	else if (address == DIV) {
		io[address & 0x7F] = 0;
		return;
	}
	else if (address == JOYPAD) {
		io[address & 0x7F] = value | (io[address & 0x7F] & 0xCF);
		return;
	}
	else if (address == DMA) {
		StartDmaTransfer(value * 0x100);
		// std::cout << "Starting DMA at " << std::hex << value * 0x100 << "\n";
		// gameboy->SetHalt(true);
		return;
	}

	*GetMemoryRef(address) = value;
}

void MMU::WriteWord (uint16_t address, uint16_t value) {
	if (0xFB00 == address) {
		std::cout << std::hex << (int) value << "\n";
		gameboy->SetHalt(true);
	}
	if (0x4000 <= address && address <= 0x7FFF) {
		HandleRomBankSwitch(address);
		return;
	}
	// Reset current scanline if we try to write to it
	else if (address == CURLINE) {
		io[address & 0x7F] = 0;
		return;
	}
	else if (address == DIV) {
		io[address & 0x7F] = 0;
		return;
	}

	uint8_t* memoryRef = GetMemoryRef(address);
	memoryRef[0] =  value & 0x00FF;
	memoryRef[1] = (value & 0xFF00) >> 8;
}

void MMU::WriteBios (const uint8_t* buffer) {
	assert(buffer != nullptr);
	std::memcpy(bios, buffer, 0x100);
}

// TODO: Refactor it to "WriteRom"
void MMU::WriteBufferToRom (const uint8_t* buffer, size_t bufferSize) {
	assert(buffer != nullptr);
	assert(bufferSize > 0);
	std::memcpy(rom, buffer, 0x4000);
	std::memcpy(romBanks[0], buffer + 0x4000, 0x4000);
}

uint8_t* MMU::GetRomRef (uint16_t address) {
	return &rom[address];
}

uint8_t* MMU::GetIORef (uint16_t address) {
	return &io[address & 0x7F];
}

uint8_t* MMU::GetMemoryRef (uint16_t address) {

	switch (address & 0xF000) {
		/* BIOS / ROM0 */
		case 0x0000:
		if (isInBios && address <= 0xFF)
			return &bios[address];
		else
			return &rom[address];
		break;

		/* ROM0 */
		case 0x1000:
		case 0x2000:
		case 0x3000:
		return &rom[address];
		break;

		/* ROM1 (unbanked) (16k) */
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
		return &romBanks[0][address & 0x3FFF];
		break;

		/* VRAM (8k) */
		case 0x8000:
		case 0x9000:
		return &vram[address & 0x1FFF];

		/* External RAM (8k) */
		case 0xA000:
		case 0xB000:
		return &eram[address & 0x1FFF];

		/* Working RAM (8k) */
		case 0xC000:
		case 0xD000:
		return &wram[address & 0x1FFF];

		/* Working RAM shadow */
		case 0xE000:
		return &wram[address & 0x1FFF];

		/* Working RAM shadow, I/O, Zero-page RAM */
		case 0xF000:
		{
			uint16_t lo = address & 0x0F00;
			if (lo <= 0xD00) {
				return &wram[address & 0x1FFF];
			}
			else if (lo == 0xE00) {
				if (address < 0xFEA0)
					return &oam[address & 0xFF];
				else
					return &unused[address & 0x7F];
			}
			else if (lo == 0xF00) {
				if (address >= 0xFF80)
					return &zram[address & 0x7F];
				else
					return &io[address & 0x7F];
			}
		}
		default:
			assert("ReadByte is trying to access an invalid address" && 0);
		return 0;
		break;

	}
}

void MMU::StartDmaTransfer (uint16_t startingAddress) {
	for (size_t i = 0; i < 160; i++) {
		*GetMemoryRef(0xFE00 + i) = *GetMemoryRef(startingAddress + i);
	}
}

void MMU::HandleRomBankSwitch(uint16_t address) {
	assert("<HandleRomBankSwitch> Unimplemented bank switch!" && 0);
}
