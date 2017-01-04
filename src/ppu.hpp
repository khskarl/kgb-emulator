#include <cstdint>
#include "mmu.hpp"

class PPU {
private:
	uint8_t displayBuffer[160 * 144];
	uint8_t backgroundBuffer[256 * 256];

	MMU* mmu = nullptr;

	void RenderBackgroundToDisplay ();
	void RenderBackgroundBuffer ();


	void FeedRandomToDisplay ();
	void FeedRandomToBackground ();
public:
	PPU ();
	virtual ~PPU ();

	void Initialize (MMU* _mmu);

	void StepUpdate ();

	uint8_t* GetDisplayBuffer ();


};
