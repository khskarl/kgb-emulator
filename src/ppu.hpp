#include <cstdint>
#include "mmu.hpp"

class CPU;

class PPU {
private:
	uint8_t displayBuffer[160 * 144];
	uint8_t backgroundBuffer[256 * 256];

	int32_t scanlineCounter = 456; // Lasts 456 Hz (Cycles)

	CPU* cpu = nullptr; // For interrupts
	MMU* mmu = nullptr; // For memory access

	void SetDisplayStatus ();

	void DrawScanline (uint8_t line);

	void FeedRandomToDisplay ();

	uint16_t GetTilesAddress ();
	uint16_t GetBackgroundTilesAddress ();

	uint8_t NextScanline ();
	void ResetScanline ();
	void ResetScanlineCounter ();
public:
	PPU ();
	virtual ~PPU ();

	void Initialize (CPU* _cpu, MMU* _mmu);

	void StepUpdate (uint16_t cycles);

	bool IsDisplayEnabled ();
	bool IsBackgroundEnabled ();

	uint8_t* GetDisplayBuffer ();


};
