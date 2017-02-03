#include "mmu.hpp"
#include "cpu.hpp"

#include <iostream>
#include <cstring> // std::memcpy
#include "debug.hpp"

MMU::MMU () {}
MMU::~MMU () {}

void MMU::Initialize () {
	WriteByte(LCDCTRL, 0x91);

	for (uint8_t &v : bios) {	v = 0; }
	for (uint8_t &v : rom)  { v = 0; }
	for (uint8_t &v : eram) {	v = 0; }
	for (uint8_t &v : wram) {	v = 0; }
	for (uint8_t &v : zram) {	v = 0; }
	for (uint8_t &v : vram) {	v = 0; }
	for (uint8_t &v : oam)  { v = 0; }
	for (uint8_t &v : io)   {	v = 0; }
}

uint8_t MMU::ReadByte (uint16_t address) {
	assert(address >= 0x0000 && address <= 0xFFFF);

	return *GetMemoryRef(address);
}

uint16_t MMU::ReadWord (uint16_t address) {
	return ReadByte(address) | (ReadByte(address + 1) << 8);
}

uint8_t MMU::ReadClockFrequency () {
	return ReadByte(TMC) & 0x3;
}


void MMU::WriteByte (uint16_t address, uint8_t value) {
	assert(address >= 0x0000 && address <= 0xFFFF);

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

	*GetMemoryRef(address) = value;
}

void MMU::WriteWord (uint16_t address, uint16_t value) {
	uint8_t* memory = GetMemoryRef(address);

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

	memory[0] =  value & 0x00FF;
	memory[1] = (value & 0xFF00) >> 8;
}

void MMU::WriteBios (uint8_t* buffer) {
	assert(buffer != nullptr);
	std::memcpy(bios, buffer, 256);
}

// TODO: Rename it to "WriteRom"
void MMU::WriteBufferToRom (uint8_t* buffer, uint32_t bufferSize) {
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
	assert(address >= 0x0000 && address <= 0xFFFF);
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
			if (0x000 <= lo && lo <= 0xD00) {
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

void MMU::HandleRomBankSwitch(uint16_t address) {
	assert("Unimplemented bank switch!" && 0);
}
