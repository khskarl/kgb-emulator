#ifndef MMU_HPP
#define MMU_HPP

#include <stdint.h>

#define DIV  0xFF04
#define TIMA 0xFF05
#define TMA  0xFF06
#define TMC  0xFF07

#define LCDCTRL 0xFF40
#define LCDSTAT 0xFF41
#define SCROLLY 0xFF42
#define SCROLLX 0xFF43
#define CURLINE 0xFF44
#define CMPLINE 0xFF45

#define IF 0xFF0F
#define IE 0xFFFF

class CPU;
class MMU {
private:
	uint8_t bios[0xFF+1];
	uint8_t rom[0x4000];
	uint8_t vram[0x2000];
	uint8_t eram[0x2000];
	uint8_t wram[0x1000];
	uint8_t zram[0x1000];

	uint8_t oam[0x2000];

	uint8_t io[0x80];

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

	uint8_t* GetRomRef    (uint16_t address);
	uint8_t* GetIORef     (uint16_t address);
	uint8_t* GetMemoryRef (uint16_t address);
};

#endif

// References
// http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory
// http://gameboy.mongenel.com/dmg/asmmemmap.html
