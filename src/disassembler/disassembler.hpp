#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include <vector>
#include <string>

class GameBoy;

class Disassembler {
private:
	GameBoy* p_gameboy = nullptr;
	std::vector<std::string> m_disassembly;

public:
	Disassembler (GameBoy* const _gameboy);
	~Disassembler ();

	std::vector<std::string> Disassemble ();
	// GUI
	void ShowDisassemblerWindow (bool* p_open);
};

#endif
