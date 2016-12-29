#include "cpu.hpp"
#include "mmu.hpp"

#include "iostream"
#include "debug.hpp"
#include "disassembler.hpp"

CPU::CPU () {
}

CPU::~CPU () {}

void CPU::Initialize () {
	AF.word = 0;
	BC.word = 0;
	DE.word = 0;
	HL.word = 0;

	SP = 0x0;
	PC = 0x0;

	Z = 0, N = 0, H = 0, C = 0;
	clockCycles = 0;

	mmu.Initialize(this);
	this->InitializeOpcodeTable();
}

void CPU::LoadRom (const Rom& rom) {
	mmu.WriteBufferToRom(rom.GetData(), rom.GetSize());
}

void CPU::EmulateCycle () {
	uint8_t opcode = mmu.ReadByte(PC);

	std::cout << std::hex << PC << ' ' << DisassembleOpcode(mmu.GetRomRef(PC)) << '\n';
	(this->*opcodes[opcode])(); // Wtf C++
}


void CPU::InitializeOpcodeTable () {
	opcodes[0x00] = &CPU::op0x00; opcodes[0x01] = &CPU::op0x01;
	opcodes[0x02] = &CPU::op0x02; opcodes[0x03] = &CPU::op0x03;
	opcodes[0x04] = &CPU::op0x04; opcodes[0x05] = &CPU::op0x05;
	opcodes[0x06] = &CPU::op0x06; opcodes[0x07] = &CPU::op0x07;
	opcodes[0x08] = &CPU::op0x08; opcodes[0x09] = &CPU::op0x09;
	opcodes[0x0A] = &CPU::op0x0A; opcodes[0x0B] = &CPU::op0x0B;
	opcodes[0x0C] = &CPU::op0x0C; opcodes[0x0D] = &CPU::op0x0D;
	opcodes[0x0E] = &CPU::op0x0E; opcodes[0x0F] = &CPU::op0x0F;

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

	opcodes[0x30] = &CPU::opNull; opcodes[0x31] = &CPU::op0x31;
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


// NOP
void CPU::op0x00 () {
	PC += 1;
	clockCycles = 4;
}
// STOP 0
void CPU::op0x10 () {
	PC += 2;
	clockCycles = 4;
}
// JR NC,r8
// JR NC,r8

// LD BC,D16
void CPU::op0x01 () {
	opNull();
	PC += 3;
	clockCycles = 12;
}
// LD DE,D16
// LD HL,D16
// LD SP,D16
void CPU::op0x31 () {
	SP = mmu.ReadWord(PC + 1);

	PC += 3;
	clockCycles = 12;
}



// LD (BC),A
void CPU::op0x02 () {
	opNull();
	PC += 1;
	clockCycles = 8;
}
// LD (DE),A
// LD (HL+),A
// LD (HL-),A

// INC BC
void CPU::op0x03 () {
	opNull();
	PC += 1;
	clockCycles = 8;
}
// INC DE
// INC HL
// INC SP

// INC B
void CPU::op0x04 () {
	opNull();
	PC += 1;
	clockCycles = 4;
}
// INC D
// INC H
// INC (HL)

// DEC B
void CPU::op0x05 () {
	opNull();
	PC += 1;
	clockCycles = 4;
}
// DEC D
// DEC H
// DEC (HL)

// LD B,d8
void CPU::op0x06 () {
	opNull();
	PC += 2;
	clockCycles = 8;
}
// LD D,d8
// LD H,d8
// LD (HL),d8

// RLCA
void CPU::op0x07 () {
	opNull();
	PC += 1;
	clockCycles = 4;
}
// RLA
// DAA
// SCF

// LD (a16),SP
void CPU::op0x08 () {
	opNull();
	PC += 3;
	clockCycles = 20;
}
// JR r8
// JR Z,r8
// JR C,r8

// ADD HL,BC
void CPU::op0x09 () {
	opNull();
	PC += 1;
	clockCycles = 8;
}
// ADD HL,DE
// ADD HL,HL
// ADD HL,SP

// LD A,(BC)
void CPU::op0x0A () {
	opNull();
	PC += 1;
	clockCycles = 8;
}
// LD A,(DE)
// LD A,(HL+)
// LD A,(HL-)

// DEC BC
void CPU::op0x0B () {
	opNull();
	PC += 1;
	clockCycles = 8;
}
// DEC DE
// DEC HL
// DEC SP

// INC C
void CPU::op0x0C () {
	opNull();
	PC += 1;
	clockCycles = 4;
}
// INC E
// INC L
// INC A

// DEC C
void CPU::op0x0D () {
	opNull();
	PC += 1;
	clockCycles = 4;
}
// DEC E
// DEC L
// DEC A

// LD C,d8
void CPU::op0x0E () {
	opNull();
	PC += 2;
	clockCycles = 8;
}
// LD E,d8
// LD L,d8
// LD A,d8
void CPU::op0x3E () {
	AF.hi = mmu.ReadByte(PC + 1);
	PC += 2;
	clockCycles = 8;
}

// RRCA
void CPU::op0x0F () {
	opNull();
	PC += 1;
	clockCycles = 4;
}
// RRA
// CPL
// CCF

/* 4. instructions */
// LD B,B
// LD B,C
// LD B,D
// LD B,E
// LD B,H
// LD B,L
// LD B,(HL)
// LD B,A
// LD C,B
// LD C,C
// LD C,D
// LD C,E
// LD C,H
// LD C,L
// LD C,(HL)
// LD C,A

/* 5. instructions */
// LD D,B
// LD D,C
// LD D,D
// LD D,E
// LD D,H
// LD D,L
// LD D,(HL)
// LD D,A
// LD E,B
// LD E,C
// LD E,D
// LD E,E
// LD E,H
// LD E,L
// LD E,(HL)
// LD E,A

/* 6. instructions */
// LD H,B
// LD H,C
// LD H,D
// LD H,E
// LD H,H
// LD H,L
// LD H,(HL)
// LD H,A
// LD L,B
// LD L,C
// LD L,D
// LD L,E
// LD L,H
// LD L,L
// LD L,(HL)
// LD L,A

/* 7. instructions */
// LD (HL),B
// LD (HL),C
// LD (HL),D
// LD (HL),E
// LD (HL),H
// LD (HL),L
// HALT
// LD (HL),A
// LD A,B
// LD A,C
// LD A,D
// LD A,E
// LD A,H
// LD A,L
// LD A,(HL)
// LD A,A

/* 8. instructions */
// ADD A,B
// ADD A,C
// ADD A,D
// ADD A,E
// ADD A,H
// ADD A,L
// ADD A,(HL)
// ADD A,A
// ADC A,B
// ADC A,C
// ADC A,D
// ADC A,E
// ADC A,H
// ADC A,L
// ADC A,(HL)
// ADC A,A

/* 9. instructions */
// SUB B
// SUB C
// SUB D
// SUB E
// SUB H
// SUB L
// SUB (HL)
// SUB A
// SBC A,B
// SBC A,C
// SBC A,D
// SBC A,E
// SBC A,H
// SBC A,L
// SBC A,(HL)
// SBC A,A

/* A. instructions */
// AND B
// AND C
// AND D
// AND E
// AND H
// AND L
// AND (HL)
// AND A
// XOR B
// XOR C
// XOR D
// XOR E
// XOR H
// XOR L
// XOR (HL)
// XOR A

/* B. instructions */
// OR B
// OR C
// OR D
// OR E
// OR H
// OR L
// OR (HL)
// OR A
// CP B
// CP C
// CP D
// CP E
// CP H
// CP L
// CP (HL)
// CP A


/*  */
// RET NZ
// RET NC
// LDH (a8),A
// LDH A,(a8)

// POP BC
// POP DE
// POP HL
// POP AF

// JP NZ,a16
// JP NC,a16
// LD (C),A
// LD A,(C)

// JP a16
// D3..: I don't exist
// E3..: I don't exist
// DI

// CALL NZ,a16
// CALL NC,a16
// E4..: I don't exist
// F4..: I don't exist

// PUSH BC
// PUSH DE
// PUSH HL
// PUSH AF

// ADD A,d8
// SUB d8
// AND d8
// OR d8

// RST 00H
// RST 10H
// RST 20H
// RST 30H

// RET Z
// RET C
// ADD SP,r8
// LD HL,SP+r8

// RET
// RETI
// JP (HL)
// LD SP,HL

// JP Z,a16
// JP C,a16
// LD (a16),A
// LD A,(a16)

// PREFIX CB
// DB..: I don't exist
// EB..: I don't exist
// EI

// CALL Z,a16
// CALL C,a16
// EC..: I don't exist
// FC..: I don't exist


// CALL a16
// DD..: I don't exist
// ED..: I don't exist
// FD..: I don't exist

// ADC A,d8
// SBC A,d8
// XOR A,d8
// CP  A,d8

// RST 08H
// RST 18H
// RST 28H
// RST 38H



/* Not implemented instructions call this function */
void CPU::opNull () {
	assert("Not implemented");
}
