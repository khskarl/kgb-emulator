#include <cstring> // std::memcpy
#include <iostream>

#include "gameboy.hpp"
#include "mmu.hpp"
#include "cpu.hpp"

#include "debug.hpp"

MMU::MMU () {}
MMU::~MMU () {}

void MMU::Initialize (GameBoy* const argGameboy) {
	p_gameboy = argGameboy;

	WriteByte(LCDCTRL, 0x91);

	const auto memzero = [] (auto& array) {
		std::memset(array, 0, sizeof(array) / sizeof(array[0]));
	};

	memzero(m_memory);
	memzero(m_bios);

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
	if (0x4000 <= address && address <= 0x7FFF) {
		HandleRomBankSwitch(address);
		return;
	}
	// Reset scanline if we try to write to it
	else if (address == CURLINE) {
		value = 0;
	}
	else if (address == DIV) {
		value = 0;
	}
	else if (address == JOYPAD) {
		value = value | (m_memory[address] & 0xCF);
	}
	else if (address == DMA) {
		StartDmaTransfer(value * 0x100);
		// std::cout << "Starting DMA at " << std::hex << value * 0x100 << "\n";
		// gameboy->SetHalt(true);
		return;
	}
	else if (address == SB) {
		// std::cout << std::hex << value;
	}

	*GetMemoryRef(address) = value;
}

void MMU::WriteWord (uint16_t address, uint16_t value) {
	if (0x4000 <= address && address <= 0x7FFF) {
		HandleRomBankSwitch(address);
		return;
	}
	// Reset current scanline if we try to write to it
	else if (address == CURLINE) {
		value = 0;
		return;
	}
	else if (address == DIV) {
		value = 0;
		return;
	}

	uint8_t* memoryRef = GetMemoryRef(address);
	memoryRef[0] =  value & 0x00FF;
	memoryRef[1] = (value & 0xFF00) >> 8;
}

void MMU::WriteBios (const uint8_t* buffer) {
	assert(buffer != nullptr);
	std::memcpy(m_bios, buffer, 0x100);
}

// TODO: Refactor it to "WriteRom"
void MMU::WriteBufferToRom (const uint8_t* buffer, size_t bufferSize) {
	assert(buffer != nullptr);
	assert(bufferSize > 0 && bufferSize <= 0x8001);
	std::memcpy(m_memory, buffer, bufferSize);
	// std::memcpy(romBanks[0], buffer + 0x4000, 0x4000);
}

uint8_t* MMU::GetMemoryRef (uint16_t address) {
	/* Handle BIOS / ROM0 access below 0x100 */
	if (m_isInBios && address < 0x100)
		return &m_bios[address];
	else if (address < 0x100)
		return &m_memory[address];
	else if (address >= 0x100)
		return &m_memory[address];
	else
		assert("ReadByte is trying to access an invalid address" && 0);
}

void MMU::StartDmaTransfer (uint16_t startingAddress) {
	for (size_t i = 0; i < 160; i++) {
		*GetMemoryRef(0xFE00 + i) = *GetMemoryRef(startingAddress + i);
	}
}

void MMU::HandleRomBankSwitch(uint16_t address) {
	assert("<HandleRomBankSwitch> Unimplemented bank switch!" && 0);
}

void MMU::DeactivateBios () {
	m_isInBios = false;
}
