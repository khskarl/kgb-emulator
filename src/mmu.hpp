#ifndef MMU_HPP
#define MMU_HPP

#include <stdint.h>
#include <stddef.h>

#include "common_memory_addresses.hpp"

class GameBoy;
class CPU;

class MMU {
private:
	GameBoy* gameboy = nullptr;

	uint8_t bios[0x100];
	uint8_t rom[0x4000];
	uint8_t romBanks[96][0x4000];
	uint8_t vram[0x2000];
	uint8_t eram[0x2000];
	uint8_t wram[0x1000];
	uint8_t zram[0x1000];

	uint8_t oam[0x2000];

	uint8_t io[0x80];

	uint8_t unused[0x60];

	void HandleRomBankSwitch (uint16_t address);

public:
	bool isInBios = true;

	MMU ();
	~MMU ();

	void Initialize(GameBoy* const gameboy);

	/* General memory access functions */
	uint8_t  ReadByte  (uint16_t address);
	uint16_t ReadWord  (uint16_t address);
	void     WriteByte (uint16_t address, uint8_t  value);
	void     WriteWord (uint16_t address, uint16_t value);

	/* Specific memory access functions */
	uint8_t  ReadClockFrequency ();

	void WriteBios (const uint8_t* buffer);
	void WriteBufferToRom (const uint8_t* buffer, size_t bufferSize);

	uint8_t* GetRomRef    (uint16_t address);
	uint8_t* GetIORef     (uint16_t address);
	uint8_t* GetMemoryRef (uint16_t address);
};

#endif

// References
// http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory
// http://gameboy.mongenel.com/dmg/asmmemmap.html
