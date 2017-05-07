#include "debug.hpp"
#include "gameboy.hpp"
#include "mmu.hpp"

std::string Debug::GetGameboyText (GameBoy& gameboy) {
	const CPU& cpu = *gameboy.GetCPU();
	// const MMU& mmu = gameboy.GetMMU(); not used yet

	std::string text = "";
	// text += "ScrollY: " + std::to_string(mmu->ReadByte(SCROLLY)) + "\n";
	// text += "ScrollX: " + std::to_string(mmu->ReadByte(SCROLLX)) + "\n";
	// text += "CurLine: " + std::to_string(mmu->ReadByte(CURLINE)) + "\n";

	char buffer[8];
	sprintf(buffer, "%04X", cpu.AF.word);
	text += "AF: " + std::string(buffer) + "\n";
	sprintf(buffer, "%04X", cpu.BC.word);
	text += "BC: " + std::string(buffer) + "\n";
	sprintf(buffer, "%04X", cpu.DE.word);
	text += "DE: " + std::string(buffer) + "\n";
	sprintf(buffer, "%04X", cpu.HL.word);
	text += "HL: " + std::string(buffer) + "\n";
	sprintf(buffer, "%04X", cpu.SP);
	text += "SP: " + std::string(buffer) + "\n";
	text += "\n";
	text += "Z: " + std::string(cpu.GetZ() ? "1" : "0") + "\n";
	text += "N: " + std::string(cpu.GetN() ? "1" : "0") + "\n";
	text += "H: " + std::string(cpu.GetH() ? "1" : "0") + "\n";
	text += "C: " + std::string(cpu.GetC() ? "1" : "0") + "\n";

	return text;
}
