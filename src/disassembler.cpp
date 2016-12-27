#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "disassembler.hpp"

template<uint8_t ... Args>
std::string Format() {
	std::string str = "Lol";
	return str;
}

/* Shitty string wizardry just for the sake of wizardry */
/* TODO: Rewizard this with variadic template */

std::string Format (const char* mnemonic, const char* args, uint8_t op) {
	char format[64];
	sprintf(format, "%%02x             %%-10s %s", args);
	char buffer[128];
	sprintf(buffer, format, op, mnemonic);
	return std::string(buffer);
}

std::string Format (const char* mnemonic, const char* args, uint8_t op, uint8_t x) {
	char format[64];
	sprintf(format, "%%02x %%02x       %%-10s %s", args);
	char buffer[128];
	sprintf(buffer, format, op, x, mnemonic, x);
	return std::string(buffer);
}

std::string Format (const char* mnemonic, const char* args, uint8_t op, uint8_t x, uint8_t y) {
	char format[64];
	sprintf(format, "%%02x %%02x %%02x %%-10s %s", args);
	char buffer[128];
	sprintf(buffer, format, op, x, y, mnemonic, x, y);
	return std::string(buffer);
}

std::string DisassembleRom(Rom rom) {
	uint8_t* code = rom.GetData();
	uint16_t pc = 0x0;

	char buffer[64];
	sprintf(buffer, "%04x ", pc);
	std::string address(buffer);

	std::string line = address + DisassembleOpcode(code + pc);
	return line;
}

std::string DisassembleOpcode(uint8_t* code) {
	std::string opcode = Format("Unknown", "%02x%02x", code[0], code[1], code[2]);

	return opcode;
}
