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
	for (auto& bank : m_romBanks) { memzero(bank); }

	GetMemoryRef(JOYPAD)[0]  = 0xFF;
	GetMemoryRef(JOYPAD)[1]  = 0xFF;
	GetMemoryRef(LCDCTRL)[0] = 0x91;
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
	if (IF == address) {
		// std::cout << "WRITING TO IF :D \n";
		// std::cout << std::hex << (int) value <<  " \n";
	}
	if (0x0000 <= address && address <= 0x7FFF) {
		HandleRomBankSwitch(address, value);
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

	// if (address == 0xffe7 || address == 0xff9e) {
	// 	std::cout << std::hex << address << " : " << (int)*GetMemoryRef(address) << "\n";
	// 	std::cout << std::hex << address << "n: " << (int)value << "\n";
	// 	p_gameboy->SetHalt(true);
	// }

	*GetMemoryRef(address) = value;
}

void MMU::WriteWord (uint16_t address, uint16_t value) {
	if (0x0000 <= address && address <= 0x7FFF) {
		HandleRomBankSwitch(address, value);
		return;
	}
	// Reset current scanline if we try to write to it
	else if (address == CURLINE) {
		value = 0;
	}
	else if (address == DIV) {
		value = 0;
	}

	uint8_t* memoryRef = GetMemoryRef(address);
	memoryRef[0] =  value & 0x00FF;
	memoryRef[1] = (value & 0xFF00) >> 8;
}

void MMU::WriteBios (const uint8_t* buffer) {
	assert(buffer != nullptr);
	std::memcpy(m_bios, buffer, 0x100);
}

void MMU::WriteRom (const uint8_t* buffer) {
	assert(buffer != nullptr);
	std::memcpy(m_memory, buffer, 0x8000);
}

void MMU::WriteRomBank (const uint8_t* rom_data, uint8_t num_bank) {
	assert(rom_data != nullptr);
	uint16_t offset = 0x4000 * num_bank + 32 * 1024;
	std::memcpy(m_romBanks[num_bank], rom_data + offset, 0x4000);
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

void MMU::HandleRomBankSwitch(uint16_t address, uint16_t value) {
	if (0x2000 <= address && address <= 0x3FFF) {
		uint8_t bank = value & 0b00011111;
		if (bank != 0)
			bank -= 1;

		SwitchRomBank(bank);
	}
}

void MMU::SwitchRomBank (uint8_t bank_id) {
	std::memcpy(&m_memory[0x4000], m_romBanks[bank_id], 0x4000);
}

void MMU::DeactivateBios () {
	m_isInBios = false;
}
