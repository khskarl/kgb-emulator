#include <iostream>
#include "bios.hpp"
#include "gameboy.hpp"

GameBoy::GameBoy () {}
GameBoy::~GameBoy () {}

void GameBoy::Initialize () {
	cpu.Initialize(&mmu, false);
	mmu.Initialize(this);
	ppu.Initialize(&cpu, &mmu);
	mmu.WriteBios(kGameboyDMGBios);
	timerCounter = 1024;
}

void GameBoy::LoadRom (Rom rom) {
	mmu.WriteBufferToRom(rom.GetData(), rom.GetSize());
}

void GameBoy::UpdateJoypadMemory(uint8_t* const joypadBuffer) {
	uint8_t* joypadRegister = mmu.GetMemoryRef(JOYPAD);
	uint8_t  prevJoypadRegister = *joypadRegister;
	// *joypadRegister |= 0x0F;

	size_t offset = 0;
	for (size_t i = 0; i < 4; i++) {
		// *joypadRegister &= ((~joypadBuffer[i + offset]) << i);
	}

	if (*joypadRegister != prevJoypadRegister) {
		cpu.RequestInterrupt(4);
	}
}

void GameBoy::StepEmulation (const uint32_t cyclesThisUpdate) {
	cpu.isHalted = isHalted;
	UpdateJoypadMemory(joypad);

	size_t cylesDone = 0;
	while (cylesDone < cyclesThisUpdate) {
		this->StepInstruction();

		this->StepTimers(cpu.clockCycles);
		ppu.StepUpdate(cpu.clockCycles);
		cpu.ProcessInterrupts();

		cylesDone += cpu.clockCycles;

		if (cpu.isHalted) {
			isHalted = true;
			break;
		}
	}

}

void GameBoy::StepInstruction () {
	cpu.EmulateCycle();
}

void GameBoy::StepTimers (const uint32_t cycles) {
	uint8_t* div = mmu.GetMemoryRef(DIV);
	if ((*div) + cycles >= 255) {
		(*div) = 0;
	} else {
		(*div) = cycles;
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
			cpu.RequestInterrupt(2);
		}
		else {
			mmu.WriteByte(TIMA, mmu.ReadByte(TIMA) + 1);
		}
	}
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

uint8_t* GameBoy::GetJoypadBuffer() {
	return joypad;
}

uint8_t* GameBoy::GetDisplayBuffer () {
	return ppu.GetDisplayBuffer();
}

CPU* GameBoy::GetCPU () {
	return &cpu;
}

MMU* GameBoy::GetMMU () {
	return &mmu;
}

bool GameBoy::GetHalt () {
	return isHalted;
}

void GameBoy::SetHalt (const bool state) {
	this->isHalted = state;
	cpu.isHalted = state;
}

void GameBoy::ToggleHalt () {
	this->isHalted ^= 1;
}
