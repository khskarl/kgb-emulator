#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP
#include <stdint.h>
#include <string>
#include "rom.hpp"


extern std::string DisassembleRom(const Rom& rom);
extern std::string DisassembleOpcode(const uint8_t* code);

#endif
