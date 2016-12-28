#include "cpu.hpp"

void CPU::Initialize() {
	this->InitializeOpcodeTable();
}

void CPU::InitializeOpcodeTable() {
	opcodes[0x00] = &CPU::opNull; opcodes[0x01] = &CPU::opNull;
	opcodes[0x02] = &CPU::opNull; opcodes[0x03] = &CPU::opNull;
	opcodes[0x04] = &CPU::opNull; opcodes[0x05] = &CPU::opNull;
	opcodes[0x06] = &CPU::opNull; opcodes[0x07] = &CPU::opNull;
	opcodes[0x08] = &CPU::opNull; opcodes[0x09] = &CPU::opNull;
	opcodes[0x0A] = &CPU::opNull; opcodes[0x0B] = &CPU::opNull;
	opcodes[0x0C] = &CPU::opNull; opcodes[0x0D] = &CPU::opNull;
	opcodes[0x0E] = &CPU::opNull; opcodes[0x0F] = &CPU::opNull;

	opcodes[0x10] = &CPU::opNull; opcodes[0x11] = &CPU::opNull;
	opcodes[0x12] = &CPU::opNull; opcodes[0x13] = &CPU::opNull;
	opcodes[0x14] = &CPU::opNull; opcodes[0x15] = &CPU::opNull;
	opcodes[0x16] = &CPU::opNull; opcodes[0x17] = &CPU::opNull;
	opcodes[0x18] = &CPU::opNull; opcodes[0x19] = &CPU::opNull;
	opcodes[0x1A] = &CPU::opNull; opcodes[0x1B] = &CPU::opNull;
	opcodes[0x1C] = &CPU::opNull; opcodes[0x1D] = &CPU::opNull;
	opcodes[0x1E] = &CPU::opNull; opcodes[0x1F] = &CPU::opNull;

	opcodes[0x20] = &CPU::opNull; opcodes[0x21] = &CPU::opNull;
	opcodes[0x22] = &CPU::opNull; opcodes[0x23] = &CPU::opNull;
	opcodes[0x24] = &CPU::opNull; opcodes[0x25] = &CPU::opNull;
	opcodes[0x26] = &CPU::opNull; opcodes[0x27] = &CPU::opNull;
	opcodes[0x28] = &CPU::opNull; opcodes[0x29] = &CPU::opNull;
	opcodes[0x2A] = &CPU::opNull; opcodes[0x2B] = &CPU::opNull;
	opcodes[0x2C] = &CPU::opNull; opcodes[0x2D] = &CPU::opNull;
	opcodes[0x2E] = &CPU::opNull; opcodes[0x2F] = &CPU::opNull;

	opcodes[0x30] = &CPU::opNull; opcodes[0x31] = &CPU::opNull;
	opcodes[0x32] = &CPU::opNull; opcodes[0x33] = &CPU::opNull;
	opcodes[0x34] = &CPU::opNull; opcodes[0x35] = &CPU::opNull;
	opcodes[0x36] = &CPU::opNull; opcodes[0x37] = &CPU::opNull;
	opcodes[0x38] = &CPU::opNull; opcodes[0x39] = &CPU::opNull;
	opcodes[0x3A] = &CPU::opNull; opcodes[0x3B] = &CPU::opNull;
	opcodes[0x3C] = &CPU::opNull; opcodes[0x3D] = &CPU::opNull;
	opcodes[0x3E] = &CPU::op0x3E; opcodes[0x3F] = &CPU::opNull;

	opcodes[0x40] = &CPU::opNull; opcodes[0x41] = &CPU::opNull;
	opcodes[0x42] = &CPU::opNull; opcodes[0x43] = &CPU::opNull;
	opcodes[0x44] = &CPU::opNull; opcodes[0x45] = &CPU::opNull;
	opcodes[0x46] = &CPU::opNull; opcodes[0x47] = &CPU::opNull;
	opcodes[0x48] = &CPU::opNull; opcodes[0x49] = &CPU::opNull;
	opcodes[0x4A] = &CPU::opNull; opcodes[0x4B] = &CPU::opNull;
	opcodes[0x4C] = &CPU::opNull; opcodes[0x4D] = &CPU::opNull;
	opcodes[0x4E] = &CPU::opNull; opcodes[0x4F] = &CPU::opNull;

	opcodes[0x50] = &CPU::opNull; opcodes[0x51] = &CPU::opNull;
	opcodes[0x52] = &CPU::opNull; opcodes[0x53] = &CPU::opNull;
	opcodes[0x54] = &CPU::opNull; opcodes[0x55] = &CPU::opNull;
	opcodes[0x56] = &CPU::opNull; opcodes[0x57] = &CPU::opNull;
	opcodes[0x58] = &CPU::opNull; opcodes[0x59] = &CPU::opNull;
	opcodes[0x5A] = &CPU::opNull; opcodes[0x5B] = &CPU::opNull;
	opcodes[0x5C] = &CPU::opNull; opcodes[0x5D] = &CPU::opNull;
	opcodes[0x5E] = &CPU::opNull; opcodes[0x5F] = &CPU::opNull;

	opcodes[0x60] = &CPU::opNull; opcodes[0x61] = &CPU::opNull;
	opcodes[0x62] = &CPU::opNull; opcodes[0x63] = &CPU::opNull;
	opcodes[0x64] = &CPU::opNull; opcodes[0x65] = &CPU::opNull;
	opcodes[0x66] = &CPU::opNull; opcodes[0x67] = &CPU::opNull;
	opcodes[0x68] = &CPU::opNull; opcodes[0x69] = &CPU::opNull;
	opcodes[0x6A] = &CPU::opNull; opcodes[0x6B] = &CPU::opNull;
	opcodes[0x6C] = &CPU::opNull; opcodes[0x6D] = &CPU::opNull;
	opcodes[0x6E] = &CPU::opNull; opcodes[0x6F] = &CPU::opNull;

	opcodes[0x70] = &CPU::opNull; opcodes[0x71] = &CPU::opNull;
	opcodes[0x72] = &CPU::opNull; opcodes[0x73] = &CPU::opNull;
	opcodes[0x74] = &CPU::opNull; opcodes[0x75] = &CPU::opNull;
	opcodes[0x76] = &CPU::opNull; opcodes[0x77] = &CPU::opNull;
	opcodes[0x78] = &CPU::opNull; opcodes[0x79] = &CPU::opNull;
	opcodes[0x7A] = &CPU::opNull; opcodes[0x7B] = &CPU::opNull;
	opcodes[0x7C] = &CPU::opNull; opcodes[0x7D] = &CPU::opNull;
	opcodes[0x7E] = &CPU::opNull; opcodes[0x7F] = &CPU::opNull;


	opcodes[0x80] = &CPU::opNull; opcodes[0x81] = &CPU::opNull;
	opcodes[0x82] = &CPU::opNull; opcodes[0x83] = &CPU::opNull;
	opcodes[0x84] = &CPU::opNull; opcodes[0x85] = &CPU::opNull;
	opcodes[0x86] = &CPU::opNull; opcodes[0x87] = &CPU::opNull;
	opcodes[0x88] = &CPU::opNull; opcodes[0x89] = &CPU::opNull;
	opcodes[0x8A] = &CPU::opNull; opcodes[0x8B] = &CPU::opNull;
	opcodes[0x8C] = &CPU::opNull; opcodes[0x8D] = &CPU::opNull;
	opcodes[0x8E] = &CPU::opNull; opcodes[0x8F] = &CPU::opNull;

	opcodes[0x90] = &CPU::opNull; opcodes[0x91] = &CPU::opNull;
	opcodes[0x92] = &CPU::opNull; opcodes[0x93] = &CPU::opNull;
	opcodes[0x94] = &CPU::opNull; opcodes[0x95] = &CPU::opNull;
	opcodes[0x96] = &CPU::opNull; opcodes[0x97] = &CPU::opNull;
	opcodes[0x98] = &CPU::opNull; opcodes[0x99] = &CPU::opNull;
	opcodes[0x9A] = &CPU::opNull; opcodes[0x9B] = &CPU::opNull;
	opcodes[0x9C] = &CPU::opNull; opcodes[0x9D] = &CPU::opNull;
	opcodes[0x9E] = &CPU::opNull; opcodes[0x9F] = &CPU::opNull;

	opcodes[0xA0] = &CPU::opNull; opcodes[0xA1] = &CPU::opNull;
	opcodes[0xA2] = &CPU::opNull; opcodes[0xA3] = &CPU::opNull;
	opcodes[0xA4] = &CPU::opNull; opcodes[0xA5] = &CPU::opNull;
	opcodes[0xA6] = &CPU::opNull; opcodes[0xA7] = &CPU::opNull;
	opcodes[0xA8] = &CPU::opNull; opcodes[0xA9] = &CPU::opNull;
	opcodes[0xAA] = &CPU::opNull; opcodes[0xAB] = &CPU::opNull;
	opcodes[0xAC] = &CPU::opNull; opcodes[0xAD] = &CPU::opNull;
	opcodes[0xAE] = &CPU::opNull; opcodes[0xAF] = &CPU::opNull;

	opcodes[0xB0] = &CPU::opNull; opcodes[0xB1] = &CPU::opNull;
	opcodes[0xB2] = &CPU::opNull; opcodes[0xB3] = &CPU::opNull;
	opcodes[0xB4] = &CPU::opNull; opcodes[0xB5] = &CPU::opNull;
	opcodes[0xB6] = &CPU::opNull; opcodes[0xB7] = &CPU::opNull;
	opcodes[0xB8] = &CPU::opNull; opcodes[0xB9] = &CPU::opNull;
	opcodes[0xBA] = &CPU::opNull; opcodes[0xBB] = &CPU::opNull;
	opcodes[0xBC] = &CPU::opNull; opcodes[0xBD] = &CPU::opNull;
	opcodes[0xBE] = &CPU::opNull; opcodes[0xBF] = &CPU::opNull;

	opcodes[0xC0] = &CPU::opNull; opcodes[0xC1] = &CPU::opNull;
	opcodes[0xC2] = &CPU::opNull; opcodes[0xC3] = &CPU::opNull;
	opcodes[0xC4] = &CPU::opNull; opcodes[0xC5] = &CPU::opNull;
	opcodes[0xC6] = &CPU::opNull; opcodes[0xC7] = &CPU::opNull;
	opcodes[0xC8] = &CPU::opNull; opcodes[0xC9] = &CPU::opNull;
	opcodes[0xCA] = &CPU::opNull; opcodes[0xCB] = &CPU::opNull;
	opcodes[0xCC] = &CPU::opNull; opcodes[0xCD] = &CPU::opNull;
	opcodes[0xCE] = &CPU::opNull; opcodes[0xCF] = &CPU::opNull;

	opcodes[0xD0] = &CPU::opNull; opcodes[0xD1] = &CPU::opNull;
	opcodes[0xD2] = &CPU::opNull; opcodes[0xD3] = &CPU::opNull;
	opcodes[0xD4] = &CPU::opNull; opcodes[0xD5] = &CPU::opNull;
	opcodes[0xD6] = &CPU::opNull; opcodes[0xD7] = &CPU::opNull;
	opcodes[0xD8] = &CPU::opNull; opcodes[0xD9] = &CPU::opNull;
	opcodes[0xDA] = &CPU::opNull; opcodes[0xDB] = &CPU::opNull;
	opcodes[0xDC] = &CPU::opNull; opcodes[0xDD] = &CPU::opNull;
	opcodes[0xDE] = &CPU::opNull; opcodes[0xDF] = &CPU::opNull;

	opcodes[0xE0] = &CPU::opNull; opcodes[0xE1] = &CPU::opNull;
	opcodes[0xE2] = &CPU::opNull; opcodes[0xE3] = &CPU::opNull;
	opcodes[0xE4] = &CPU::opNull; opcodes[0xE5] = &CPU::opNull;
	opcodes[0xE6] = &CPU::opNull; opcodes[0xE7] = &CPU::opNull;
	opcodes[0xE8] = &CPU::opNull; opcodes[0xE9] = &CPU::opNull;
	opcodes[0xEA] = &CPU::opNull; opcodes[0xEB] = &CPU::opNull;
	opcodes[0xEC] = &CPU::opNull; opcodes[0xED] = &CPU::opNull;
	opcodes[0xEE] = &CPU::opNull; opcodes[0xEF] = &CPU::opNull;

	opcodes[0xF0] = &CPU::opNull; opcodes[0xF1] = &CPU::opNull;
	opcodes[0xF2] = &CPU::opNull; opcodes[0xF3] = &CPU::opNull;
	opcodes[0xF4] = &CPU::opNull; opcodes[0xF5] = &CPU::opNull;
	opcodes[0xF6] = &CPU::opNull; opcodes[0xF7] = &CPU::opNull;
	opcodes[0xF8] = &CPU::opNull; opcodes[0xF9] = &CPU::opNull;
	opcodes[0xFA] = &CPU::opNull; opcodes[0xFB] = &CPU::opNull;
	opcodes[0xFC] = &CPU::opNull; opcodes[0xFD] = &CPU::opNull;
	opcodes[0xFE] = &CPU::opNull; opcodes[0xFF] = &CPU::opNull;
}

void CPU::opNull () {
	// printf("Not implemented\n");
}

/* LD A,d8 */
void CPU::op0x3E() {
	// AF.hi = MMU->ReadByte(PC + 1);
	clockCycles = 8;
	PC += 2;
}
