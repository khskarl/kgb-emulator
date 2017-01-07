#include "gameboy.hpp"

GameBoy::GameBoy () {}
GameBoy::~GameBoy () {}

void GameBoy::Initialize () {
	cpu.Initialize(&mmu);
	mmu.Initialize();
	ppu.Initialize(&mmu);
}

void GameBoy::LoadBios (Rom bios) {
	mmu.WriteBios(bios.GetData());
}

void GameBoy::LoadRom (Rom rom) {
	mmu.WriteBufferToRom(rom.GetData(), rom.GetSize());
}

void GameBoy::StepEmulation () {
	size_t cyclesThisUpdate = 0;
	const int maxCycles = clockSpeed / frequency;
	while (cyclesThisUpdate < maxCycles)
	{
		StepInstruction();
		cyclesThisUpdate += cpu.clockCycles;
	}

	ppu.StepUpdate();
}

void GameBoy::StepInstruction () {
	cpu.EmulateCycle();
}

uint8_t* GameBoy::GetDisplayBuffer () {
	return ppu.GetDisplayBuffer();
}
