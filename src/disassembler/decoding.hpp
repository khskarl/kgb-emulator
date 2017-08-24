#ifndef DECODING_HPP
#define DECODING_HPP
#include <stdint.h>
#include <string>
#include <vector>
#include "rom.hpp"

// struct Instruction {
// 	uint8_t code;
// 	const char mnemonic[5];
// 	const char format[13];
// };
//
// // TODO: This list
// Instruction instructions[] = {
// 	{0x00, "NOP", ""},
// 	{0x01, "LD" , "BC,$%02x%02x"},
// 	// 0x02, 0x03 ...
// };
//
// struct DisassemblyLine {
//
// };

extern std::vector<std::string> DisassembleRom(const Rom& rom);
extern std::string DisassembleOpcode(const uint8_t* code);
extern uint8_t GetLastDisassembleStep ();

#endif
