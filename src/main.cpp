#include <iostream>
#include <stdint.h>

#include "context/context.hpp"
#include "disassembler.hpp"
#include "rom.hpp"

int main(int argc, char const *argv[]) {
	if (argc < 2) {
		std::cout << "<*Error*> You didn't supply a ROM path as argument\n" <<
		             "Usage example: ./build/gbemu.out roms/tetris.gb\n";
	}

	const char* filepath = argv[1];

	Rom rom(filepath);
	if (rom.isLoaded == false) {
		std::cout << "<*Error*> Failed to load: " << filepath << " \n";
		return 1;
	}

	if (rom.isLoaded) {
		std::cout << "[ROM DATA]" << '\n' <<
		             "Name: " << rom.GetName() << '\n' <<
		             "Size: " << rom.GetSize() << " B\n";

		std::cout << DisassembleRom(rom) << '\n';
	}

	return 0;
}
