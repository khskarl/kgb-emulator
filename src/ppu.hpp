#include <cstdint>
#include "mmu.hpp"

class PPU {
private:
	uint8_t displayBuffer[160 * 144];
	uint8_t backgroundBuffer[256 * 256];

	int32_t scanlineCounter = 0;

	MMU* mmu = nullptr;

	void RenderBackgroundToDisplay ();
	void RenderBackgroundBuffer ();

	void FeedRandomToDisplay ();
	void FeedRandomToBackground ();
	void FeedPatternToBackground ();

	uint16_t GetTilesAddress ();
	uint16_t GetBackgroundTilesAddress ();

	uint8_t NextScanline ();
	void ResetScanline ();
	void ResetScanlineCounter ();
public:
	PPU ();
	virtual ~PPU ();

	void Initialize (MMU* _mmu);

	void StepUpdate (uint16_t cycles);

	bool IsDisplayEnabled ();
	bool IsBackgroundEnabled ();

	uint8_t* GetDisplayBuffer ();


};
