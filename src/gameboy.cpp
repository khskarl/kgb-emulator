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
	mmu.WriteRom(rom.GetData(), rom.GetRomSize());
	for (uint8_t i = 0; i < rom.GetNumRomBanks(); i++) {
		mmu.WriteRomBank(rom.GetData(), i);
	}
}

void GameBoy::UpdateJoypadMemory(uint8_t* const joypadBuffer) {
	uint8_t* joypadRegister = mmu.GetMemoryRef(JOYPAD);
	uint8_t  prevJoypadRegister = *joypadRegister;

	uint8_t isDirectionsSelected = !(*joypadRegister & 0b010000);
	uint8_t isButtonsSelected    = !(*joypadRegister & 0b100000);

	*joypadRegister |= 0x0F;

	if (isDirectionsSelected) {
		for (size_t i = 0; i < 4; i++) {
			*joypadRegister &= (~(joypadBuffer[i] << i));
		}
	}

	if (isButtonsSelected) {
		for (size_t i = 0; i < 4; i++) {
			*joypadRegister &= (~(joypadBuffer[i + 4] << i));
		}
	}

	if (*joypadRegister < prevJoypadRegister) {
		// cpu.RequestInterrupt(4);
	}
}

void GameBoy::StepEmulation () {
	cpu.isHalted = isHalted;

	const uint32_t cyclesThisUpdate = m_clockSpeed / 60;

	size_t cyclesDone = 0;
	while (cyclesDone < cyclesThisUpdate) {
		// By calling this inside this loop, we ensure it's getting the selected pa-
		//rt of the game pad.

		this->StepInstruction();

		cyclesDone += cpu.clockCycles;

		if (cpu.isHalted) {
			isHalted = true;
			break;
		}
	}

}

void GameBoy::StepInstruction () {
	UpdateJoypadMemory(joypad);
	cpu.EmulateCycle();
	this->StepTimers(cpu.clockCycles);
	ppu.StepUpdate(cpu.clockCycles);
	cpu.ProcessInterrupts();

}

void GameBoy::StepTimers (const uint32_t cycles) {
	uint8_t* div = mmu.GetMemoryRef(DIV);
	uint8_t increment = cycles / 16;
	if ((*div) + increment > 255) {
		(*div) = 0;
	} else {
		(*div) += increment;
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
	// Frequencies as defined by the pandocs but divided by the clockspeed (4194304)
	//                                Dec:     0   1   2    3
	//                                Bin:    00  01  10   11
	static const uint32_t frequencies[] = { 1024, 16, 64, 256 };

	this->timerCounter = frequencies[m_clockSpeed / mmu.ReadClockFrequency()];
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

bool GameBoy::GetHaltState () {
	return isHalted;
}

void GameBoy::SetHaltState (const bool state) {
	this->isHalted = state;
	cpu.isHalted = state;
}

void GameBoy::ToggleHaltState () {
	this->isHalted ^= 1;
}

void GameBoy::SetClock(uint32_t new_clock) {
	m_clockSpeed = new_clock;
}
