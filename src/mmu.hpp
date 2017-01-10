#ifndef MMU_HPP
#define MMU_HPP

#include <stdint.h>

#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA  0xFF06
#define TMC  0xFF07

class CPU;
class MMU {
private:

	uint8_t bios[256];
	uint8_t rom[32768];
	uint8_t eram[32768];
	uint8_t wram[32768];
	uint8_t zram[32768];


	uint8_t vram[32768];
	uint8_t oam[32768];


	uint8_t io[128];

public:
	bool isInBios = true;

	MMU ();
	~MMU ();

	void Initialize();

	/* General memory access functions */
	uint8_t  ReadByte  (uint16_t address);
	uint16_t ReadWord  (uint16_t address);
	void     WriteByte (uint16_t address, uint8_t  value);
	void     WriteWord (uint16_t address, uint16_t value);

	/* Specific memory access functions */
	uint8_t  ReadClockFrequency ();

	void WriteBios (uint8_t* buffer);
	void WriteBufferToRom (uint8_t* buffer, uint16_t bufferSize);

	uint8_t* GetRomRef (uint16_t address);
	uint8_t* GetMemoryRef (uint16_t address);
};

#endif

// References
// http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory
// http://gameboy.mongenel.com/dmg/asmmemmap.html
