#include "gameboy.hpp"

GameBoy::GameBoy () {}
GameBoy::~GameBoy () {}

void GameBoy::Initialize () {
	cpu.Initialize(&mmu);
	mmu.Initialize();
}

void GameBoy::LoadRom (Rom rom) {
	mmu.WriteBufferToRom(rom.GetData(), rom.GetSize());
}

void GameBoy::StepUpdate () {
	size_t cyclesThisUpdate = 0;
	const int MAXCYCLES = 4194304 / 60;
	while (cyclesThisUpdate < MAXCYCLES)
	{
		StepCycle();
		cyclesThisUpdate += cpu.clockCycles;
	}
}

void GameBoy::StepCycle () {
	cpu.EmulateCycle();
}
