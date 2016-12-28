#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "disassembler.hpp"
/*
	Reference:
	http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
*/

uint16_t pc;

template<uint8_t ... Args>
std::string Format() {
	std::string str = "Lol";
	return str;
}

/* Shitty string wizardry just for the sake of wizardry */
/* TODO: Rewizard this with variadic template */

std::string Format (const char* mnemonic, const char* args, uint8_t op) {
	pc += 1;

	char format[64];
	sprintf(format, "%%02x        %%-5s %s", args);
	char buffer[128];
	sprintf(buffer, format, op, mnemonic);
	return std::string(buffer);
}

std::string Format (const char* mnemonic, const char* args, uint8_t op, uint8_t x) {
	pc += 2;

	char format[64];
	sprintf(format, "%%02x %%02x     %%-5s %s", args);
	char buffer[128];
	sprintf(buffer, format, op, x, mnemonic, x);
	return std::string(buffer);
}

std::string Format (const char* mnemonic, const char* args, uint8_t op, uint8_t x, uint8_t y) {
	pc += 3;

	char format[64];
	sprintf(format, "%%02x %%02x %%02x  %%-5s %s", args);
	char buffer[128];
	sprintf(buffer, format, op, x, y, mnemonic, x, y);
	return std::string(buffer);
}

std::string DisassembleRom(Rom rom) {
	uint8_t* code = rom.GetData();

	char buffer[64];
	pc = 0x0;
	std::string disassembledRom = "";
	while (pc < rom.GetSize()) {
		sprintf(buffer, "%04x ", pc);
		std::string address(buffer);

		std::string line = address + DisassembleOpcode(code + pc);
		disassembledRom += line + '\n';
	}

	return disassembledRom;
}

std::string DisassembleCB(uint8_t* code);

std::string DisassembleOpcode(uint8_t* code) {

	switch (code[0]) {
		case 0x00: return Format("NOP",  "",  code[0]); break;
		case 0x10: return Format("STOP",	 "0", code[0]); break;
		case 0x20: return Format("JR", "NZ,$%02x", code[0], code[1]); break;
		case 0x30: return Format("JR", "NC,$%02x", code[0], code[1]); break;

		case 0x01: return Format("LD", "BC,$%02x%02x", code[0], code[1], code[2]); break;
		case 0x11: return Format("LD", "DE,$%02x%02x", code[0], code[1], code[2]); break;
		case 0x21: return Format("LD", "HL,$%02x%02x", code[0], code[1], code[2]); break;
		case 0x31: return Format("LD", "SP,$%02x%02x", code[0], code[1], code[2]); break;

		case 0x02: return Format("LD", "(BC),A",  code[0]); break;
		case 0x12: return Format("LD", "(DE),A",  code[0]); break;
		case 0x22: return Format("LD", "(HL+),A", code[0]); break;
		case 0x32: return Format("LD", "(HL-),A", code[0]); break;

		case 0x03: return Format("INC", "BC", code[0]); break;
		case 0x13: return Format("INC", "DE", code[0]); break;
		case 0x23: return Format("INC", "HL", code[0]); break;
		case 0x33: return Format("INC", "SP", code[0]); break;

		case 0x04: return Format("INC", "B",    code[0]); break;
		case 0x14: return Format("INC", "D",    code[0]); break;
		case 0x24: return Format("INC", "H",    code[0]); break;
		case 0x34: return Format("INC", "(HL)", code[0]); break;

		case 0x05: return Format("DEC", "B",    code[0]); break;
		case 0x15: return Format("DEC", "D",    code[0]); break;
		case 0x25: return Format("DEC", "H",    code[0]); break;
		case 0x35: return Format("DEC", "(HL)", code[0]); break;

		case 0x06: return Format("LD", "B,$%02x",    code[0], code[1]); break;
		case 0x16: return Format("LD", "D,$%02x",    code[0], code[1]); break;
		case 0x26: return Format("LD", "H,$%02x",    code[0], code[1]); break;
		case 0x36: return Format("LD", "(HL),$%02x", code[0], code[1]); break;

		case 0x07: return Format("RLCA", "", code[0]); break;
		case 0x17: return Format("RLA",  "", code[0]); break;
		case 0x27: return Format("DAA",  "", code[0]); break;
		case 0x37: return Format("SCF",  "", code[0]); break;

		case 0x08: return Format("LD", "($%02x%02x),SP", code[0], code[1], code[2]); break;
		case 0x18: return Format("JR", "$%02x",   code[0], code[1]); break;
		case 0x28: return Format("JR", "Z,$%02x", code[0], code[1]); break;
		case 0x38: return Format("JR", "C,$%02x", code[0], code[1]); break;

		case 0x09: return Format("ADD", "HL,BC", code[0]); break;
		case 0x19: return Format("ADD", "HL,DE", code[0]); break;
		case 0x29: return Format("ADD", "HL,HL", code[0]); break;
		case 0x39: return Format("ADD", "HL,SP", code[0]); break;

		case 0x0A: return Format("LD", "A,(BC)",  code[0]); break;
		case 0x1A: return Format("LD", "A,(DE)",  code[0]); break;
		case 0x2A: return Format("LD", "A,(HL+)", code[0]); break;
		case 0x3A: return Format("LD", "A,(HL-)", code[0]); break;

		case 0x0B: return Format("DEC", "BC", code[0]); break;
		case 0x1B: return Format("DEC", "DE", code[0]); break;
		case 0x2B: return Format("DEC", "HL", code[0]); break;
		case 0x3B: return Format("DEC", "SP", code[0]); break;

		case 0x0C: return Format("INC", "C", code[0]); break;
		case 0x1C: return Format("INC", "E", code[0]); break;
		case 0x2C: return Format("INC", "L", code[0]); break;
		case 0x3C: return Format("INC", "A", code[0]); break;

		case 0x0D: return Format("DEC", "C", code[0]); break;
		case 0x1D: return Format("DEC", "E", code[0]); break;
		case 0x2D: return Format("DEC", "L", code[0]); break;
		case 0x3D: return Format("DEC", "A", code[0]); break;

		case 0x0E: return Format("LD", "C,$%02x", code[0], code[1]); break;
		case 0x1E: return Format("LD", "E,$%02x", code[0], code[1]); break;
		case 0x2E: return Format("LD", "L,$%02x", code[0], code[1]); break;
		case 0x3E: return Format("LD", "A,$%02x", code[0], code[1]); break;

		case 0x0F: return Format("RRCA", "", code[0]); break;
		case 0x1F: return Format("RRA",  "", code[0]); break;
		case 0x2F: return Format("CPL",  "", code[0]); break;
		case 0x3F: return Format("CCF",  "", code[0]); break;


		/* Horizontal order */
		case 0x40: return Format("LD", "B,B",    code[0]); break;
		case 0x41: return Format("LD", "B,C",    code[0]); break;
		case 0x42: return Format("LD", "B,D",    code[0]); break;
		case 0x43: return Format("LD", "B,E",    code[0]); break;
		case 0x44: return Format("LD", "B,H",    code[0]); break;
		case 0x45: return Format("LD", "B,L",    code[0]); break;
		case 0x46: return Format("LD", "B,(HL)", code[0]); break;
		case 0x47: return Format("LD", "B,A",    code[0]); break;
		case 0x48: return Format("LD", "C,B",    code[0]); break;
		case 0x49: return Format("LD", "C,C",    code[0]); break;
		case 0x4A: return Format("LD", "C,D",    code[0]); break;
		case 0x4B: return Format("LD", "C,E",    code[0]); break;
		case 0x4C: return Format("LD", "C,H",    code[0]); break;
		case 0x4D: return Format("LD", "C,L",    code[0]); break;
		case 0x4E: return Format("LD", "C,(HL)", code[0]); break;
		case 0x4F: return Format("LD", "C,A",    code[0]); break;

		case 0x50: return Format("LD", "D,B",    code[0]); break;
		case 0x51: return Format("LD", "D,C",    code[0]); break;
		case 0x52: return Format("LD", "D,D",    code[0]); break;
		case 0x53: return Format("LD", "D,E",    code[0]); break;
		case 0x54: return Format("LD", "D,H",    code[0]); break;
		case 0x55: return Format("LD", "D,L",    code[0]); break;
		case 0x56: return Format("LD", "D,(HL)", code[0]); break;
		case 0x57: return Format("LD", "D,A",    code[0]); break;
		case 0x58: return Format("LD", "E,B",    code[0]); break;
		case 0x59: return Format("LD", "E,C",    code[0]); break;
		case 0x5A: return Format("LD", "E,D",    code[0]); break;
		case 0x5B: return Format("LD", "E,E",    code[0]); break;
		case 0x5C: return Format("LD", "E,H",    code[0]); break;
		case 0x5D: return Format("LD", "E,L",    code[0]); break;
		case 0x5E: return Format("LD", "E,(HL)", code[0]); break;
		case 0x5F: return Format("LD", "E,A",    code[0]); break;

		case 0x60: return Format("LD", "H,B",    code[0]); break;
		case 0x61: return Format("LD", "H,C",    code[0]); break;
		case 0x62: return Format("LD", "H,D",    code[0]); break;
		case 0x63: return Format("LD", "H,E",    code[0]); break;
		case 0x64: return Format("LD", "H,H",    code[0]); break;
		case 0x65: return Format("LD", "H,L",    code[0]); break;
		case 0x66: return Format("LD", "H,(HL)", code[0]); break;
		case 0x67: return Format("LD", "H,A",    code[0]); break;
		case 0x68: return Format("LD", "L,B",    code[0]); break;
		case 0x69: return Format("LD", "L,C",    code[0]); break;
		case 0x6A: return Format("LD", "L,D",    code[0]); break;
		case 0x6B: return Format("LD", "L,E",    code[0]); break;
		case 0x6C: return Format("LD", "L,H",    code[0]); break;
		case 0x6D: return Format("LD", "L,L",    code[0]); break;
		case 0x6E: return Format("LD", "L,(HL)", code[0]); break;
		case 0x6F: return Format("LD", "L,A",    code[0]); break;

		case 0x70: return Format("LD", "(HL),B", code[0]); break;
		case 0x71: return Format("LD", "(HL),C", code[0]); break;
		case 0x72: return Format("LD", "(HL),D", code[0]); break;
		case 0x73: return Format("LD", "(HL),E", code[0]); break;
		case 0x74: return Format("LD", "(HL),H", code[0]); break;
		case 0x75: return Format("LD", "(HL),L", code[0]); break;
		case 0x76: return Format("HALT", "",     code[0]); break;
		case 0x77: return Format("LD", "(HL),A", code[0]); break;
		case 0x78: return Format("LD", "A,B",    code[0]); break;
		case 0x79: return Format("LD", "A,C",    code[0]); break;
		case 0x7A: return Format("LD", "A,D",    code[0]); break;
		case 0x7B: return Format("LD", "A,E",    code[0]); break;
		case 0x7C: return Format("LD", "A,H",    code[0]); break;
		case 0x7D: return Format("LD", "A,L",    code[0]); break;
		case 0x7E: return Format("LD", "A,(HL)", code[0]); break;
		case 0x7F: return Format("LD", "A,A",    code[0]); break;

		case 0x80: return Format("ADD", "A,B",    code[0]); break;
		case 0x81: return Format("ADD", "A,C",    code[0]); break;
		case 0x82: return Format("ADD", "A,D",    code[0]); break;
		case 0x83: return Format("ADD", "A,E",    code[0]); break;
		case 0x84: return Format("ADD", "A,H",    code[0]); break;
		case 0x85: return Format("ADD", "A,L",    code[0]); break;
		case 0x86: return Format("ADD", "A,(HL)", code[0]); break;
		case 0x87: return Format("ADD", "A,A",    code[0]); break;
		case 0x88: return Format("ADC", "A,B",    code[0]); break;
		case 0x89: return Format("ADC", "A,C",    code[0]); break;
		case 0x8A: return Format("ADC", "A,D",    code[0]); break;
		case 0x8B: return Format("ADC", "A,E",    code[0]); break;
		case 0x8C: return Format("ADC", "A,H",    code[0]); break;
		case 0x8D: return Format("ADC", "A,L",    code[0]); break;
		case 0x8E: return Format("ADC", "A,(HL)", code[0]); break;
		case 0x8F: return Format("ADC", "A,A",    code[0]); break;

		case 0x90: return Format("SUB", "B",    code[0]); break;
		case 0x91: return Format("SUB", "C",    code[0]); break;
		case 0x92: return Format("SUB", "D",    code[0]); break;
		case 0x93: return Format("SUB", "E",    code[0]); break;
		case 0x94: return Format("SUB", "H",    code[0]); break;
		case 0x95: return Format("SUB", "L",    code[0]); break;
		case 0x96: return Format("SUB", "(HL)", code[0]); break;
		case 0x97: return Format("SUB", "A",    code[0]); break;
		case 0x98: return Format("SBC", "A,B",    code[0]); break;
		case 0x99: return Format("SBC", "A,C",    code[0]); break;
		case 0x9A: return Format("SBC", "A,D",    code[0]); break;
		case 0x9B: return Format("SBC", "A,E",    code[0]); break;
		case 0x9C: return Format("SBC", "A,H",    code[0]); break;
		case 0x9D: return Format("SBC", "A,L",    code[0]); break;
		case 0x9E: return Format("SBC", "A,(HL)", code[0]); break;
		case 0x9F: return Format("SBC", "A,A",    code[0]); break;

		case 0xA0: return Format("AND", "B",    code[0]); break;
		case 0xA1: return Format("AND", "C",    code[0]); break;
		case 0xA2: return Format("AND", "D",    code[0]); break;
		case 0xA3: return Format("AND", "E",    code[0]); break;
		case 0xA4: return Format("AND", "H",    code[0]); break;
		case 0xA5: return Format("AND", "L",    code[0]); break;
		case 0xA6: return Format("AND", "(HL)", code[0]); break;
		case 0xA7: return Format("AND", "A",    code[0]); break;
		case 0xA8: return Format("XOR", "B",    code[0]); break;
		case 0xA9: return Format("XOR", "C",    code[0]); break;
		case 0xAA: return Format("XOR", "D",    code[0]); break;
		case 0xAB: return Format("XOR", "E",    code[0]); break;
		case 0xAC: return Format("XOR", "H",    code[0]); break;
		case 0xAD: return Format("XOR", "L",    code[0]); break;
		case 0xAE: return Format("XOR", "(HL)", code[0]); break;
		case 0xAF: return Format("XOR", "A",    code[0]); break;

		case 0xB0: return Format("OR", "B",    code[0]); break;
		case 0xB1: return Format("OR", "C",    code[0]); break;
		case 0xB2: return Format("OR", "D",    code[0]); break;
		case 0xB3: return Format("OR", "E",    code[0]); break;
		case 0xB4: return Format("OR", "H",    code[0]); break;
		case 0xB5: return Format("OR", "L",    code[0]); break;
		case 0xB6: return Format("OR", "(HL)", code[0]); break;
		case 0xB7: return Format("OR", "A",    code[0]); break;
		case 0xB8: return Format("CP", "B",    code[0]); break;
		case 0xB9: return Format("CP", "C",    code[0]); break;
		case 0xBA: return Format("CP", "D",    code[0]); break;
		case 0xBB: return Format("CP", "E",    code[0]); break;
		case 0xBC: return Format("CP", "H",    code[0]); break;
		case 0xBD: return Format("CP", "L",    code[0]); break;
		case 0xBE: return Format("CP", "(HL)", code[0]); break;
		case 0xBF: return Format("CP", "A",    code[0]); break;


		/* Vertical order */
		case 0xC0: return Format("RET", "NZ", code[0]); break;
		case 0xD0: return Format("RET", "NC", code[0]); break;
		case 0xE0: return Format("LDH", "($%02x),A", code[0], code[1]); break;
		case 0xF0: return Format("LDH", "A,($%02x)", code[0], code[1]); break;

		case 0xC1: return Format("POP", "BC", code[0]); break;
		case 0xD1: return Format("POP", "DE", code[0]); break;
		case 0xE1: return Format("POP", "HL", code[0]); break;
		case 0xF1: return Format("POP", "AF", code[0]); break;

		case 0xC2: return Format("JP", "NZ,$%02x%02x", code[0], code[1], code[2]); break;
		case 0xD2: return Format("JP", "NC,$%02x%02x", code[0], code[1], code[2]); break;
		case 0xE2: return Format("LD", "(C),A", code[0]); break;
		case 0xF2: return Format("LD", "A,(C)", code[0]); break;

		case 0xC3: return Format("JP", "$%02x%02x", code[0], code[1], code[2]); break;
		/*   0xE3: I don't exist */
		/*   0xD3: I don't exist */
		case 0xF3: return Format("DI", "", code[0]); break;

		case 0xC4: return Format("CALL", "NZ,$%02x%02x", code[0], code[1], code[2]); break;
		case 0xD4: return Format("CALL", "NC,$%02x%02x", code[0], code[1], code[2]); break;
		/*   0xE4: I don't exist */
		/*   0xF4: I don't exist */

		case 0xC5: return Format("PUSH", "BC", code[0]); break;
		case 0xD5: return Format("PUSH", "DE", code[0]); break;
		case 0xE5: return Format("PUSH", "HL", code[0]); break;
		case 0xF5: return Format("PUSH", "AF", code[0]); break;

		case 0xC6: return Format("ADD", "A, $%02x", code[0], code[1]); break;
		case 0xD6: return Format("SUB", "$%02x",    code[0], code[1]); break;
		case 0xE6: return Format("AND", "$%02x",    code[0], code[1]); break;
		case 0xF6: return Format("OR",  "$%02x",    code[0], code[1]); break;

		case 0xC7: return Format("RST", "$00", code[0]); break;
		case 0xD7: return Format("RST", "$10", code[0]); break;
		case 0xE7: return Format("RST", "$20", code[0]); break;
		case 0xF7: return Format("RST", "$30", code[0]); break;

		case 0xC8: return Format("RET", "Z", code[0]); break;
		case 0xD8: return Format("RET", "C", code[0]); break;
		case 0xE8: return Format("ADD", "SP,$%02x", code[0], code[1]); break;
		case 0xF8: return Format("LD",  "HL,SP+$%02x", code[0], code[1]); break;

		case 0xC9: return Format("RET",  "",      code[0]); break;
		case 0xD9: return Format("RETI", "",      code[0]); break;
		case 0xE9: return Format("JP",   "(HL)",  code[0]); break;
		case 0xF9: return Format("LD",   "SP,HL", code[0]); break;

		case 0xCA: return Format("JP", "Z,$%02x%02x", code[0], code[1], code[2]); break;
		case 0xDA: return Format("JP", "C,$%02x%02x", code[0], code[1], code[2]); break;
		case 0xEA: return Format("LD", "($%02x%02x),A", code[0], code[1], code[2]); break;
		case 0xFA: return Format("LD", "A,($%02x%02x)", code[0], code[1], code[2]); break;

		case 0xCB: return DisassembleCB(code); break;
		/*   0xDB: I don't exist */
		/*   0xEB: I don't exist */
		case 0xFB: return Format("EI", "", code[0]); break;

		case 0xCC: return Format("CALL", "Z,$%02x%02x", code[0], code[1], code[2]); break;
		case 0xDC: return Format("CALL", "C,$%02x%02x", code[0], code[1], code[2]); break;
		/*   0xEC: I don't exist */
		/*   0xFC: I don't exist */

		case 0xCD: return Format("CALL", "$%02x%02x", code[0], code[1], code[2]); break;
		/*   0xDD: I don't exist */
		/*   0xED: I don't exist */
		/*   0xFD: I don't exist */

		case 0xCE: return Format("ADC", "A,$%02x", code[0], code[1]); break;
		case 0xDE: return Format("SBC", "A,$%02x", code[0], code[1]); break;
		case 0xEE: return Format("XOR", "%02",     code[0], code[1]); break;
		case 0xFE: return Format("CP",  "%02",     code[0], code[1]); break;

		case 0xCF: return Format("RST", "$08", code[0]); break;
		case 0xDF: return Format("RST", "$18", code[0]); break;
		case 0xEF: return Format("RST", "$28", code[0]); break;
		case 0xFF: return Format("RST", "$38", code[0]); break;

		default: return Format("Unknown", "", code[0]); break;
	}
}


// TODO: Disassemble CB.. instructions
std::string DisassembleCB(uint8_t* code) {
	switch (code[1]) {
		case 0x00: return Format("RLC", "B", code[0]); break;
		case 0x01: return Format("RLC", "C", code[0]); break;
		case 0x02: return Format("RLC", "D", code[0]); break;
		case 0x03: return Format("RLC", "E", code[0]); break;
		case 0x04: return Format("RLC", "H", code[0]); break;
		case 0x05: return Format("RLC", "L", code[0]); break;
		case 0x06: return Format("RLC", "(HL)", code[0]); break;
		case 0x07: return Format("RLC", "A", code[0]); break;
		case 0x08: return Format("RRC", "B", code[0]); break;
		case 0x09: return Format("RRC", "C", code[0]); break;
		case 0x0A: return Format("RRC", "D", code[0]); break;
		case 0x0B: return Format("RRC", "E", code[0]); break;
		case 0x0C: return Format("RRC", "H", code[0]); break;
		case 0x0D: return Format("RRC", "L", code[0]); break;
		case 0x0E: return Format("RRC", "(HL)", code[0]); break;
		case 0x0F: return Format("RRC", "A", code[0]); break;

		case 0x10: return Format("RL", "B", code[0]); break;
		case 0x11: return Format("RL", "C", code[0]); break;
		case 0x12: return Format("RL", "D", code[0]); break;
		case 0x13: return Format("RL", "E", code[0]); break;
		case 0x14: return Format("RL", "H", code[0]); break;
		case 0x15: return Format("RL", "L", code[0]); break;
		case 0x16: return Format("RL", "(HL)", code[0]); break;
		case 0x17: return Format("RL", "A", code[0]); break;
		case 0x18: return Format("RR", "B", code[0]); break;
		case 0x19: return Format("RR", "C", code[0]); break;
		case 0x1A: return Format("RR", "D", code[0]); break;
		case 0x1B: return Format("RR", "E", code[0]); break;
		case 0x1C: return Format("RR", "H", code[0]); break;
		case 0x1D: return Format("RR", "L", code[0]); break;
		case 0x1E: return Format("RR", "(HL)", code[0]); break;
		case 0x1F: return Format("RR", "A", code[0]); break;

		case 0x20: return Format("SLA", "B", code[0]); break;
		case 0x21: return Format("SLA", "C", code[0]); break;
		case 0x22: return Format("SLA", "D", code[0]); break;
		case 0x23: return Format("SLA", "E", code[0]); break;
		case 0x24: return Format("SLA", "H", code[0]); break;
		case 0x25: return Format("SLA", "L", code[0]); break;
		case 0x26: return Format("SLA", "(HL)", code[0]); break;
		case 0x27: return Format("SLA", "A", code[0]); break;
		case 0x28: return Format("SRA", "B", code[0]); break;
		case 0x29: return Format("SRA", "C", code[0]); break;
		case 0x2A: return Format("SRA", "D", code[0]); break;
		case 0x2B: return Format("SRA", "E", code[0]); break;
		case 0x2C: return Format("SRA", "H", code[0]); break;
		case 0x2D: return Format("SRA", "L", code[0]); break;
		case 0x2E: return Format("SRA", "(HL)", code[0]); break;
		case 0x2F: return Format("SRA", "A", code[0]); break;

		case 0x30: return Format("SWAP", "B", code[0]); break;
		case 0x31: return Format("SWAP", "C", code[0]); break;
		case 0x32: return Format("SWAP", "D", code[0]); break;
		case 0x33: return Format("SWAP", "E", code[0]); break;
		case 0x34: return Format("SWAP", "H", code[0]); break;
		case 0x35: return Format("SWAP", "L", code[0]); break;
		case 0x36: return Format("SWAP", "(HL)", code[0]); break;
		case 0x37: return Format("SWAP", "A", code[0]); break;
		case 0x38: return Format("SRL", "B", code[0]); break;
		case 0x39: return Format("SRL", "C", code[0]); break;
		case 0x3A: return Format("SRL", "D", code[0]); break;
		case 0x3B: return Format("SRL", "E", code[0]); break;
		case 0x3C: return Format("SRL", "H", code[0]); break;
		case 0x3D: return Format("SRL", "L", code[0]); break;
		case 0x3E: return Format("SRL", "(HL)", code[0]); break;
		case 0x3F: return Format("SRL", "A", code[0]); break;

		case 0x40: return Format("BIT", "0,B", code[0]); break;
		case 0x41: return Format("BIT", "0,C", code[0]); break;
		case 0x42: return Format("BIT", "0,D", code[0]); break;
		case 0x43: return Format("BIT", "0,E", code[0]); break;
		case 0x44: return Format("BIT", "0,H", code[0]); break;
		case 0x45: return Format("BIT", "0,L", code[0]); break;
		case 0x46: return Format("BIT", "0,(HL)", code[0]); break;
		case 0x47: return Format("BIT", "0,A", code[0]); break;
		case 0x48: return Format("BIT", "1,B", code[0]); break;
		case 0x49: return Format("BIT", "1,C", code[0]); break;
		case 0x4A: return Format("BIT", "1,D", code[0]); break;
		case 0x4B: return Format("BIT", "1,E", code[0]); break;
		case 0x4C: return Format("BIT", "1,H", code[0]); break;
		case 0x4D: return Format("BIT", "1,L", code[0]); break;
		case 0x4E: return Format("BIT", "1,(HL)", code[0]); break;
		case 0x4F: return Format("BIT", "1,A", code[0]); break;

		case 0x50: return Format("BIT", "2,B", code[0]); break;
		case 0x51: return Format("BIT", "2,C", code[0]); break;
		case 0x52: return Format("BIT", "2,D", code[0]); break;
		case 0x53: return Format("BIT", "2,E", code[0]); break;
		case 0x54: return Format("BIT", "2,H", code[0]); break;
		case 0x55: return Format("BIT", "2,L", code[0]); break;
		case 0x56: return Format("BIT", "2,(HL)", code[0]); break;
		case 0x57: return Format("BIT", "2,A", code[0]); break;
		case 0x58: return Format("BIT", "3,B", code[0]); break;
		case 0x59: return Format("BIT", "3,C", code[0]); break;
		case 0x5A: return Format("BIT", "3,D", code[0]); break;
		case 0x5B: return Format("BIT", "3,E", code[0]); break;
		case 0x5C: return Format("BIT", "3,H", code[0]); break;
		case 0x5D: return Format("BIT", "3,L", code[0]); break;
		case 0x5E: return Format("BIT", "3,(HL)", code[0]); break;
		case 0x5F: return Format("BIT", "3,A", code[0]); break;

		case 0x60: return Format("BIT", "4,B", code[0]); break;
		case 0x61: return Format("BIT", "4,C", code[0]); break;
		case 0x62: return Format("BIT", "4,D", code[0]); break;
		case 0x63: return Format("BIT", "4,E", code[0]); break;
		case 0x64: return Format("BIT", "4,H", code[0]); break;
		case 0x65: return Format("BIT", "4,L", code[0]); break;
		case 0x66: return Format("BIT", "4,(HL)", code[0]); break;
		case 0x67: return Format("BIT", "4,A", code[0]); break;
		case 0x68: return Format("BIT", "5,B", code[0]); break;
		case 0x69: return Format("BIT", "5,C", code[0]); break;
		case 0x6A: return Format("BIT", "5,D", code[0]); break;
		case 0x6B: return Format("BIT", "5,E", code[0]); break;
		case 0x6C: return Format("BIT", "5,H", code[0]); break;
		case 0x6D: return Format("BIT", "5,L", code[0]); break;
		case 0x6E: return Format("BIT", "5,(HL)", code[0]); break;
		case 0x6F: return Format("BIT", "5,A", code[0]); break;

		case 0x70: return Format("BIT", "6,B", code[0]); break;
		case 0x71: return Format("BIT", "6,C", code[0]); break;
		case 0x72: return Format("BIT", "6,D", code[0]); break;
		case 0x73: return Format("BIT", "6,E", code[0]); break;
		case 0x74: return Format("BIT", "6,H", code[0]); break;
		case 0x75: return Format("BIT", "6,L", code[0]); break;
		case 0x76: return Format("BIT", "6,(HL)", code[0]); break;
		case 0x77: return Format("BIT", "6,A", code[0]); break;
		case 0x78: return Format("BIT", "7,B", code[0]); break;
		case 0x79: return Format("BIT", "7,C", code[0]); break;
		case 0x7A: return Format("BIT", "7,D", code[0]); break;
		case 0x7B: return Format("BIT", "7,E", code[0]); break;
		case 0x7C: return Format("BIT", "7,H", code[0]); break;
		case 0x7D: return Format("BIT", "7,L", code[0]); break;
		case 0x7E: return Format("BIT", "7,(HL)", code[0]); break;
		case 0x7F: return Format("BIT", "7,A", code[0]); break;

		case 0x80: return Format("RES", "0,B", code[0]); break;
		case 0x81: return Format("RES", "0,C", code[0]); break;
		case 0x82: return Format("RES", "0,D", code[0]); break;
		case 0x83: return Format("RES", "0,E", code[0]); break;
		case 0x84: return Format("RES", "0,H", code[0]); break;
		case 0x85: return Format("RES", "0,L", code[0]); break;
		case 0x86: return Format("RES", "0,(HL)", code[0]); break;
		case 0x87: return Format("RES", "0,A", code[0]); break;
		case 0x88: return Format("RES", "1,B", code[0]); break;
		case 0x89: return Format("RES", "1,C", code[0]); break;
		case 0x8A: return Format("RES", "1,D", code[0]); break;
		case 0x8B: return Format("RES", "1,E", code[0]); break;
		case 0x8C: return Format("RES", "1,H", code[0]); break;
		case 0x8D: return Format("RES", "1,L", code[0]); break;
		case 0x8E: return Format("RES", "1,(HL)", code[0]); break;
		case 0x8F: return Format("RES", "1,A", code[0]); break;

		case 0x90: return Format("RES", "2,B", code[0]); break;
		case 0x91: return Format("RES", "2,C", code[0]); break;
		case 0x92: return Format("RES", "2,D", code[0]); break;
		case 0x93: return Format("RES", "2,E", code[0]); break;
		case 0x94: return Format("RES", "2,H", code[0]); break;
		case 0x95: return Format("RES", "2,L", code[0]); break;
		case 0x96: return Format("RES", "2,(HL)", code[0]); break;
		case 0x97: return Format("RES", "2,A", code[0]); break;
		case 0x98: return Format("RES", "3,B", code[0]); break;
		case 0x99: return Format("RES", "3,C", code[0]); break;
		case 0x9A: return Format("RES", "3,D", code[0]); break;
		case 0x9B: return Format("RES", "3,E", code[0]); break;
		case 0x9C: return Format("RES", "3,H", code[0]); break;
		case 0x9D: return Format("RES", "3,L", code[0]); break;
		case 0x9E: return Format("RES", "3,(HL)", code[0]); break;
		case 0x9F: return Format("RES", "3,A", code[0]); break;

		case 0xA0: return Format("RES", "4,B", code[0]); break;
		case 0xA1: return Format("RES", "4,C", code[0]); break;
		case 0xA2: return Format("RES", "4,D", code[0]); break;
		case 0xA3: return Format("RES", "4,E", code[0]); break;
		case 0xA4: return Format("RES", "4,H", code[0]); break;
		case 0xA5: return Format("RES", "4,L", code[0]); break;
		case 0xA6: return Format("RES", "4,(HL)", code[0]); break;
		case 0xA7: return Format("RES", "4,A", code[0]); break;
		case 0xA8: return Format("RES", "5,B", code[0]); break;
		case 0xA9: return Format("RES", "5,C", code[0]); break;
		case 0xAA: return Format("RES", "5,D", code[0]); break;
		case 0xAB: return Format("RES", "5,E", code[0]); break;
		case 0xAC: return Format("RES", "5,H", code[0]); break;
		case 0xAD: return Format("RES", "5,L", code[0]); break;
		case 0xAE: return Format("RES", "5,(HL)", code[0]); break;
		case 0xAF: return Format("RES", "5,A", code[0]); break;

		case 0xB0: return Format("RES", "6,B", code[0]); break;
		case 0xB1: return Format("RES", "6,C", code[0]); break;
		case 0xB2: return Format("RES", "6,D", code[0]); break;
		case 0xB3: return Format("RES", "6,E", code[0]); break;
		case 0xB4: return Format("RES", "6,H", code[0]); break;
		case 0xB5: return Format("RES", "6,L", code[0]); break;
		case 0xB6: return Format("RES", "6,(HL)", code[0]); break;
		case 0xB7: return Format("RES", "6,A", code[0]); break;
		case 0xB8: return Format("RES", "7,B", code[0]); break;
		case 0xB9: return Format("RES", "7,C", code[0]); break;
		case 0xBA: return Format("RES", "7,D", code[0]); break;
		case 0xBB: return Format("RES", "7,E", code[0]); break;
		case 0xBC: return Format("RES", "7,H", code[0]); break;
		case 0xBD: return Format("RES", "7,L", code[0]); break;
		case 0xBE: return Format("RES", "7,(HL)", code[0]); break;
		case 0xBF: return Format("RES", "7,A", code[0]); break;

		case 0xC0: return Format("SET", "0,B", code[0]); break;
		case 0xC1: return Format("SET", "0,C", code[0]); break;
		case 0xC2: return Format("SET", "0,D", code[0]); break;
		case 0xC3: return Format("SET", "0,E", code[0]); break;
		case 0xC4: return Format("SET", "0,H", code[0]); break;
		case 0xC5: return Format("SET", "0,L", code[0]); break;
		case 0xC6: return Format("SET", "0,(HL)", code[0]); break;
		case 0xC7: return Format("SET", "0,A", code[0]); break;
		case 0xC8: return Format("SET", "1,B", code[0]); break;
		case 0xC9: return Format("SET", "1,C", code[0]); break;
		case 0xCA: return Format("SET", "1,D", code[0]); break;
		case 0xCB: return Format("SET", "1,E", code[0]); break;
		case 0xCC: return Format("SET", "1,H", code[0]); break;
		case 0xCD: return Format("SET", "1,L", code[0]); break;
		case 0xCE: return Format("SET", "1,(HL)", code[0]); break;
		case 0xCF: return Format("SET", "1,A", code[0]); break;

		case 0xD0: return Format("SET", "2,B", code[0]); break;
		case 0xD1: return Format("SET", "2,C", code[0]); break;
		case 0xD2: return Format("SET", "2,D", code[0]); break;
		case 0xD3: return Format("SET", "2,E", code[0]); break;
		case 0xD4: return Format("SET", "2,H", code[0]); break;
		case 0xD5: return Format("SET", "2,L", code[0]); break;
		case 0xD6: return Format("SET", "2,(HL)", code[0]); break;
		case 0xD7: return Format("SET", "2,A", code[0]); break;
		case 0xD8: return Format("SET", "3,B", code[0]); break;
		case 0xD9: return Format("SET", "3,C", code[0]); break;
		case 0xDA: return Format("SET", "3,D", code[0]); break;
		case 0xDB: return Format("SET", "3,E", code[0]); break;
		case 0xDC: return Format("SET", "3,H", code[0]); break;
		case 0xDD: return Format("SET", "3,L", code[0]); break;
		case 0xDE: return Format("SET", "3,(HL)", code[0]); break;
		case 0xDF: return Format("SET", "3,A", code[0]); break;

		case 0xE0: return Format("SET", "4,B", code[0]); break;
		case 0xE1: return Format("SET", "4,C", code[0]); break;
		case 0xE2: return Format("SET", "4,D", code[0]); break;
		case 0xE3: return Format("SET", "4,E", code[0]); break;
		case 0xE4: return Format("SET", "4,H", code[0]); break;
		case 0xE5: return Format("SET", "4,L", code[0]); break;
		case 0xE6: return Format("SET", "4,(HL)", code[0]); break;
		case 0xE7: return Format("SET", "4,A", code[0]); break;
		case 0xE8: return Format("SET", "5,B", code[0]); break;
		case 0xE9: return Format("SET", "5,C", code[0]); break;
		case 0xEA: return Format("SET", "5,D", code[0]); break;
		case 0xEB: return Format("SET", "5,E", code[0]); break;
		case 0xEC: return Format("SET", "5,H", code[0]); break;
		case 0xED: return Format("SET", "5,L", code[0]); break;
		case 0xEE: return Format("SET", "5,(HL)", code[0]); break;
		case 0xEF: return Format("SET", "5,A", code[0]); break;

		case 0xF0: return Format("SET", "6,B", code[0]); break;
		case 0xF1: return Format("SET", "6,C", code[0]); break;
		case 0xF2: return Format("SET", "6,D", code[0]); break;
		case 0xF3: return Format("SET", "6,E", code[0]); break;
		case 0xF4: return Format("SET", "6,H", code[0]); break;
		case 0xF5: return Format("SET", "6,L", code[0]); break;
		case 0xF6: return Format("SET", "6,(HL)", code[0]); break;
		case 0xF7: return Format("SET", "6,A", code[0]); break;
		case 0xF8: return Format("SET", "7,B", code[0]); break;
		case 0xF9: return Format("SET", "7,C", code[0]); break;
		case 0xFA: return Format("SET", "7,D", code[0]); break;
		case 0xFB: return Format("SET", "7,E", code[0]); break;
		case 0xFC: return Format("SET", "7,H", code[0]); break;
		case 0xFD: return Format("SET", "7,L", code[0]); break;
		case 0xFE: return Format("SET", "7,(HL)", code[0]); break;
		case 0xFF: return Format("SET", "7,A", code[0]); break;

		default: return Format("Unknown CB..", "", code[0]);
	}
}

/* References
http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
*/
