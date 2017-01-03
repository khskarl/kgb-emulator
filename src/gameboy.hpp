#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

#include "cpu.hpp"
#include "mmu.hpp"
#include "rom.hpp"

class GameBoy {
private:
	CPU cpu;
	MMU mmu;

	uint32_t clockSpeed = 4194304; // Hz aka Cycles per second

public:
	GameBoy ();
	~GameBoy ();

	void Initialize ();

	void LoadBios (Rom bios);
	void LoadRom  (Rom rom);

	void StepUpdate ();
	void StepCycle  ();
};

#endif
