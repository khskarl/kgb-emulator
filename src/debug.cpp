#include "debug.hpp"
#include "gameboy.hpp"
#include "mmu.hpp"

std::string Debug::GetGameboyText (GameBoy& gameboy) {
	CPU* cpu = gameboy.GetCPU();
	MMU* mmu = gameboy.GetMMU();

	std::string text = "";
	text += "ScrollY: " + std::to_string(mmu->ReadByte(SCROLLY)) + "\n";
	text += "ScrollX: " + std::to_string(mmu->ReadByte(SCROLLX)) + "\n";
	text += "CurLine: " + std::to_string(mmu->ReadByte(CURLINE)) + "\n";

	text += "A: " + std::to_string(cpu->AF.hi) + "\n";
	text += "B: " + std::to_string(cpu->BC.hi) + "\n";
	text += "C: " + std::to_string(cpu->BC.lo) + "\n";
	text += "D: " + std::to_string(cpu->DE.hi) + "\n";
	text += "E: " + std::to_string(cpu->DE.lo) + "\n";
	text += "H: " + std::to_string(cpu->HL.hi) + "\n";
	text += "L: " + std::to_string(cpu->HL.lo) + "\n";

	return text;
}
