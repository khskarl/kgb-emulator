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

	uint32_t clockSpeed = 4194304; // Hz aka Cycles per second
	uint32_t frequency = 4096;

public:
	GameBoy ();
	~GameBoy ();

	void Initialize ();

	void LoadBios (Rom bios);
	void LoadRom  (Rom rom);

	void StepUpdate ();
	void StepCycle  ();

	uint8_t* GetDisplayBuffer ();
	PPU ppu;
};

#endif
