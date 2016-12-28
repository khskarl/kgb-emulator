#ifndef MMU_HPP
#define MMU_HPP

#include <stdint.h>
#include "cpu.hpp"

class MMU {
private:
	bool inBios = true;

	uint8_t bios[256];
	uint8_t rom[32768];
	uint8_t eram[32768];
	uint8_t wram[32768];
	uint8_t zram[32768];


	uint8_t vram[32768];
	uint8_t oam[32768];

	CPU* cpu = nullptr;
public:
	MMU (CPU* _cpu);
	~MMU ();

	uint8_t  ReadByte (uint16_t address);
	uint16_t ReadWord (uint16_t address);
	void WriteByte (uint16_t address, uint8_t value);
	void WriteWord (uint16_t address, uint8_t value);

};

#endif
