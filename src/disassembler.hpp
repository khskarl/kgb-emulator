#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP
#include <string>
#include <stdint.h>
#include "rom.hpp"


std::string DisassembleRom(Rom rom);
std::string DisassembleOpcode(uint8_t* code);

#endif
