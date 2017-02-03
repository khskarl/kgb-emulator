#include "debug.hpp"
#include "gameboy.hpp"
#include "mmu.hpp"

std::string Debug::GetGameboyText (GameBoy& gameboy) {
	CPU* cpu = gameboy.GetCPU();
	MMU* mmu = gameboy.GetMMU();

	std::string text = "";
	// text += "ScrollY: " + std::to_string(mmu->ReadByte(SCROLLY)) + "\n";
	// text += "ScrollX: " + std::to_string(mmu->ReadByte(SCROLLX)) + "\n";
	// text += "CurLine: " + std::to_string(mmu->ReadByte(CURLINE)) + "\n";

	char buffer[8];
	sprintf(buffer, "%04X", cpu->AF.word);
	text += "AF: " + std::string(buffer) + "\n";
	sprintf(buffer, "%04X", cpu->BC.word);
	text += "BC: " + std::string(buffer) + "\n";
	sprintf(buffer, "%04X", cpu->DE.word);
	text += "DE: " + std::string(buffer) + "\n";
	sprintf(buffer, "%04X", cpu->HL.word);
	text += "HL: " + std::string(buffer) + "\n";

	sprintf(buffer, "%04X", cpu->SP);
	text += "SP: " + std::string(buffer) + "\n";

	return text;
}
