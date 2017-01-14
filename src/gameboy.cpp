#include "gameboy.hpp"

GameBoy::GameBoy () {}
GameBoy::~GameBoy () {}

void GameBoy::Initialize () {
	cpu.Initialize(&mmu);
	mmu.Initialize();
	ppu.Initialize(&cpu, &mmu);

	timerCounter = 1024;
}

void GameBoy::LoadBios (Rom bios) {
	mmu.WriteBios(bios.GetData());
}

void GameBoy::LoadRom (Rom rom) {
	mmu.WriteBufferToRom(rom.GetData(), rom.GetSize());
}

void GameBoy::StepEmulation (const uint32_t cyclesThisUpdate) {
	size_t cylesDone = 0;
	while (cylesDone < cyclesThisUpdate) {
		this->StepInstruction();

		uint16_t numCycles = cpu.clockCycles;
		this->StepTimers(numCycles);
		this->StepGraphics(numCycles);
		ppu.StepUpdate(numCycles);
		cpu.ProcessInterrupts();

		cylesDone += numCycles;
	}

}

void GameBoy::StepInstruction () {
	cpu.EmulateCycle();
}

void GameBoy::StepTimers (uint32_t cycles) {

	if (mmu.ReadByte(DIV) + cycles >= 255) {
		mmu.WriteByte(DIV, 0);

	} else {
		mmu.WriteByte(DIV, cycles);
	}

	if (IsClockEnabled() == false)
		return;

	timerCounter -= cycles;

	// enough cpu clock cycles have happened to update the timer
	if (timerCounter <= 0) {
		ResetClockFrequency();

		// timer about to overflow
		if (mmu.ReadByte(TIMA) == 255) {
			mmu.WriteByte(TIMA, mmu.ReadByte(TMA));
			// a timer interrupt should ocurr
		}
		else {
			mmu.WriteByte(TIMA, mmu.ReadByte(TIMA) + 1);
		}
	}
}

void GameBoy::StepGraphics (uint32_t cycles) {

}

void GameBoy::ResetClockFrequency () {
	// Frequencies as define by the pandocs but divided by the clockspeed (4194304)
	//                                Dec:     0   1   2    3
	//                                Bin:    00  01  10   11
	static const uint32_t frequencies[] = { 1024, 16, 64, 256 };

	this->timerCounter = frequencies[mmu.ReadClockFrequency()];
}

bool GameBoy::IsClockEnabled () {
	return (mmu.ReadByte(TMC) & 0x4) == 0x4 ? true : false;
}


uint8_t* GameBoy::GetDisplayBuffer () {
	return ppu.GetDisplayBuffer();
}

MMU* GameBoy::GetMMU () {
	return &mmu;
}

bool GameBoy::GetHalt () {
	return isHalted;
}

void GameBoy::SetHalt (bool state) {
	this->isHalted = state;
}

void GameBoy::ToggleHalt () {
	this->isHalted ^= 1;
}
