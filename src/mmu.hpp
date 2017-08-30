#ifndef MMU_HPP
#define MMU_HPP

#include <stdint.h>
#include <stddef.h>

#include "common_memory_addresses.hpp"

class GameBoy;
class CPU;

class MMU {
private:
	GameBoy* p_gameboy = nullptr;

	uint8_t m_bios[0x100];
	uint8_t m_memory[0xFFFF + 1]; // 64k

	uint8_t m_numRomBanks = 0;
	uint8_t m_currRomBank = 0;
	uint8_t m_romBanks[125][0x4000];

	void HandleRomBankSwitch(uint16_t address, uint16_t value);

public:
	bool m_isInBios = true;

	MMU ();
	~MMU ();

	void Initialize (GameBoy* const gameboy);
	void DeactivateBios ();

	/* General memory access functions */
	uint8_t  ReadByte  (uint16_t address);
	uint16_t ReadWord  (uint16_t address);
	void     WriteByte (uint16_t address, uint8_t  value);
	void     WriteWord (uint16_t address, uint16_t value);
	void     WriteByteRaw (uint16_t address, uint8_t value);
	void     WriteWordRaw (uint16_t address, uint16_t value);

	/* Specific memory access functions */
	uint8_t  ReadClockFrequency ();

	/* Write memory access */
	void WriteBios (const uint8_t* buffer);
	void WriteRom  (const uint8_t* buffer, size_t bufferSize);
	void WriteRomBank (const uint8_t* rom_data, uint8_t num_bank);

	/* Memory References */
	uint8_t* GetMemoryRef (uint16_t address);

	/* Others */
	void StartDmaTransfer (uint16_t startingAddress);
};

#endif

// References
// http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory
// http://gameboy.mongenel.com/dmg/asmmemmap.html
