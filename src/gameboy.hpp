#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP

#include "cpu.hpp"
#include "mmu.hpp"
#include "ppu.hpp"
#include "rom.hpp"

class Disassembler;

class GameBoy {
friend class Disassembler;

private:
	CPU cpu;
	MMU mmu;
	PPU ppu;

	// const uint32_t clockSpeed = 4194304; // Hz or Cycles per second
	// uint32_t clockFrequency = 4096;

	uint32_t timerCounter = 1024; // aka TIMA

	bool isHalted = true;

	uint8_t joypad[8] = {0, 0, 0, 0,
	                     0, 0, 0, 0};

public:
	GameBoy ();
	~GameBoy ();

	void Initialize ();
	void LoadRom  (Rom rom);

	void UpdateJoypadMemory(uint8_t* const joypadBuffer);

	void StepEmulation (const uint32_t cyclesThisUpdate = 69905);
	void StepInstruction ();
	void StepTimers (const uint32_t cycles);

	void ResetClockFrequency ();
	bool IsClockEnabled ();

	uint8_t* GetJoypadBuffer ();
	uint8_t* GetDisplayBuffer ();
	CPU* GetCPU ();
	MMU* GetMMU ();

	bool GetHaltState ();
	void SetHaltState (const bool state);
	void ToggleHaltState ();
};

#endif
