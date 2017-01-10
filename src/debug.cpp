#include "debug.hpp"
#include "gameboy.hpp"
#include "mmu.hpp"

std::string Debug::GetGameboyText (GameBoy& gameboy) {
	MMU* mmu = gameboy.GetMMU();

	std::string text = "";
	text += "ScrollY: " + std::to_string(mmu->ReadByte(SCROLLY)) + "\n";
	text += "ScrollX: " + std::to_string(mmu->ReadByte(SCROLLX)) + "\n";

	return text;
}
