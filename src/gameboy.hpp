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

	uint32_t clockSpeed = 4194304; // Hz aka Cycles per second
	uint32_t frequency = 4096;

public:
	GameBoy ();
	~GameBoy ();

	void Initialize ();

	void LoadBios (Rom bios);
	void LoadRom  (Rom rom);

	void StepEmulation ();
	void StepInstruction  ();

	uint8_t* GetDisplayBuffer ();
};

#endif
