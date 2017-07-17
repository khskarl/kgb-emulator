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

	uint8_t isDirectionsSelected = ~(*joypadRegister & 0b010000);
	uint8_t isButtonsSelected    = ~(*joypadRegister & 0b100000);

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

	if (*joypadRegister != prevJoypadRegister) {
		// cpu.RequestInterrupt(4);
	}
}

void GameBoy::StepEmulation (const uint32_t cyclesThisUpdate) {
	cpu.isHalted = isHalted;

	// uint8_t joypadRegister = *mmu.GetMemoryRef(JOYPAD);
	// std::string p0 = joypadRegister & 0b0001 ? "1" : "0";
	// std::string p1 = joypadRegister & 0b0010 ? "1" : "0";
	// std::string p2 = joypadRegister & 0b0100 ? "1" : "0";
	// std::string p3 = joypadRegister & 0b1000 ? "1" : "0";
	// std::string directions = joypadRegister & 0b10000 ? "1" : "0";
	// std::string buttons = joypadRegister & 0b100000 ? "1" : "0";
	//
	// std::cout << "Curr FF00: " << std::hex << (int) joypadRegister << "\n";
	// std::cout << "Buttons:    " + buttons + "\n";
	// std::cout << "Directions: " + directions + "\n";
	// std::cout << "| P3: " + p3 + " | P2: " + p2 + " | P1: " + p1 + " | P0: " + p0 + "\n";

	size_t cylesDone = 0;
	while (cylesDone < cyclesThisUpdate) {
		// By calling this inside this loop, we ensure it's getting the selected pa-
		//rt of the game pad.
		UpdateJoypadMemory(joypad);

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
