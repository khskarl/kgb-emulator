#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

#include "cpu.hpp"
#include "mmu.hpp"
#include "ppu.hpp"
#include "rom.hpp"

class GameBoy {
private:
	CPU cpu;
	MMU mmu;
	PPU ppu;

	// const uint32_t clockSpeed = 4194304; // Hz or Cycles per second
	// uint32_t clockFrequency = 4096;

	uint32_t timerCounter = 1024; // aka TIMA

public:
	GameBoy ();
	~GameBoy ();

	void Initialize ();

	void LoadBios (Rom bios);
	void LoadRom  (Rom rom);

	void StepEmulation (const int cyclesThisUpdate = 69905);
	void StepInstruction ();
	void StepTimers (uint16_t cycles);

	void ResetClockFrequency ();
	bool IsClockEnabled ();

	uint8_t* GetDisplayBuffer ();
};

#endif
