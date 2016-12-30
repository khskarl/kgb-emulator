#include "cpu.hpp"
#include "mmu.hpp"

#include "iostream"
#include "debug.hpp"
#include "disassembler.hpp"

CPU::CPU () {
}

CPU::~CPU () {}

void CPU::Initialize () {
	AF = 0;
	BC = 0;
	DE = 0;
	HL = 0;

	SP = 0x0;
	PC = 0x0;

	Z = 0, N = 0, H = 0, C = 0;
	clockCycles = 0;

	mmu.Initialize(this);
	InitializeOpcodeTable();
}

void CPU::LoadRom (const Rom& rom) {
	mmu.WriteBufferToRom(rom.GetData(), rom.GetSize());
}

void CPU::EmulateCycle () {
	uint8_t opcode = mmu.ReadByte(PC);

	std::cout << std::hex << PC << ' ' << DisassembleOpcode(mmu.GetRomRef(PC)) << '\n';
	// std::cout << std::hex << opcode << '\n';
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

	opcodes[0x10] = &CPU::opNull; opcodes[0x11] = &CPU::op0x11;
	opcodes[0x12] = &CPU::op0x12; opcodes[0x13] = &CPU::opNull;
	opcodes[0x14] = &CPU::opNull; opcodes[0x15] = &CPU::opNull;
	opcodes[0x16] = &CPU::opNull; opcodes[0x17] = &CPU::opNull;
	opcodes[0x18] = &CPU::opNull; opcodes[0x19] = &CPU::opNull;
	opcodes[0x1A] = &CPU::opNull; opcodes[0x1B] = &CPU::opNull;
	opcodes[0x1C] = &CPU::opNull; opcodes[0x1D] = &CPU::opNull;
	opcodes[0x1E] = &CPU::opNull; opcodes[0x1F] = &CPU::opNull;

	opcodes[0x20] = &CPU::opNull; opcodes[0x21] = &CPU::op0x21;
	opcodes[0x22] = &CPU::op0x22; opcodes[0x23] = &CPU::opNull;
	opcodes[0x24] = &CPU::opNull; opcodes[0x25] = &CPU::opNull;
	opcodes[0x26] = &CPU::opNull; opcodes[0x27] = &CPU::opNull;
	opcodes[0x28] = &CPU::opNull; opcodes[0x29] = &CPU::opNull;
	opcodes[0x2A] = &CPU::opNull; opcodes[0x2B] = &CPU::opNull;
	opcodes[0x2C] = &CPU::opNull; opcodes[0x2D] = &CPU::opNull;
	opcodes[0x2E] = &CPU::opNull; opcodes[0x2F] = &CPU::opNull;

	opcodes[0x30] = &CPU::opNull; opcodes[0x31] = &CPU::op0x31;
	opcodes[0x32] = &CPU::op0x32; opcodes[0x33] = &CPU::opNull;
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
	opcodes[0xA8] = &CPU::op0xA8; opcodes[0xA9] = &CPU::op0xA9;
	opcodes[0xAA] = &CPU::op0xAA; opcodes[0xAB] = &CPU::op0xAB;
	opcodes[0xAC] = &CPU::op0xAC; opcodes[0xAD] = &CPU::op0xAD;
	opcodes[0xAE] = &CPU::op0xAE; opcodes[0xAF] = &CPU::op0xAF;

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
	opcodes[0xCA] = &CPU::opNull; opcodes[0xCB] = &CPU::op0xCB;
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


	/* Initialize CB prefixed functions table */
	opcodesCB[0x7C] = &CPU::cb0x7C;
	opcodesCB[0x70] = &CPU::opNull; opcodesCB[0x71] = &CPU::opNull;
	opcodesCB[0x72] = &CPU::opNull; opcodesCB[0x73] = &CPU::opNull;
	opcodesCB[0x74] = &CPU::opNull; opcodesCB[0x75] = &CPU::opNull;
	opcodesCB[0x76] = &CPU::opNull; opcodesCB[0x77] = &CPU::opNull;
	opcodesCB[0x78] = &CPU::opNull; opcodesCB[0x79] = &CPU::opNull;
	opcodesCB[0x7A] = &CPU::cb0x7A; opcodesCB[0x7B] = &CPU::cb0x7B;
	opcodesCB[0x7C] = &CPU::cb0x7C; opcodesCB[0x7D] = &CPU::cb0x7D;
	opcodesCB[0x7E] = &CPU::cb0x7E; opcodesCB[0x7F] = &CPU::cb0x7F;

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
// JR NZ,r8
// JR NC,r8

// LD BC,D16
void CPU::op0x01 () {
	BC = mmu.ReadWord(PC + 1);

	PC += 3;
	clockCycles = 12;
}
// LD DE,D16
void CPU::op0x11 () {
	DE = mmu.ReadWord(PC + 1);

	PC += 3;
	clockCycles = 12;
}
// LD HL,D16
void CPU::op0x21 () {
	HL = mmu.ReadWord(PC + 1);

	PC += 3;
	clockCycles = 12;
}
// LD SP,D16
void CPU::op0x31 () {
	SP = mmu.ReadWord(PC + 1);

	PC += 3;
	clockCycles = 12;
}



// LD (BC),A
void CPU::op0x02 () {
	mmu.WriteByte(BC, AF.hi);

	PC += 1;
	clockCycles = 8;
}
// LD (DE),A
void CPU::op0x12 () {
	mmu.WriteByte(DE, AF.hi);

	PC += 1;
	clockCycles = 8;
}
// LD (HL+),A
void CPU::op0x22 () {
	mmu.WriteByte(HL, AF.hi);
	HL += 1;

	PC += 1;
	clockCycles = 8;
}
// LD (HL-),A
void CPU::op0x32 () {
	mmu.WriteByte(HL, AF.hi);
	HL -= 1;

	PC += 1;
	clockCycles = 8;
}

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
	BC.hi = mmu.ReadByte(PC + 1);
	PC += 2;
	clockCycles = 8;
}
// LD E,d8
void CPU::op0x1E () {
	DE.lo = mmu.ReadByte(PC + 1);
	PC += 2;
	clockCycles = 8;
}
// LD L,d8
void CPU::op0x2E () {
	HL.lo = mmu.ReadByte(PC + 1);
	PC += 2;
	clockCycles = 8;
}
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
void CPU::op0xA8() {
	AF.hi ^= BC.hi;
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 4;
}
// XOR C
void CPU::op0xA9() {
	AF.hi ^= BC.lo;
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 4;
}
// XOR D
void CPU::op0xAA() {
	AF.hi ^= DE.hi;
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 4;
}
// XOR E
void CPU::op0xAB() {
	AF.hi ^= DE.lo;
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 4;
}
// XOR H
void CPU::op0xAC() {
	AF.hi ^= HL.hi;
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 4;
}
// XOR L
void CPU::op0xAD() {
	AF.hi ^= HL.lo;
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 4;
}
// XOR (HL)
void CPU::op0xAE() {
	AF.hi ^= mmu.ReadByte(HL);
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 8;
}
// XOR A
void CPU::op0xAF() {
	AF.hi ^= AF.hi;
	Z = (AF.hi == 0);

	N = H = C = 0;
	PC += 1;
	clockCycles += 4;
}

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
void CPU::op0xCB () {
	uint8_t secondByte = mmu.ReadByte(PC + 1);

	(this->*opcodesCB[secondByte])();
	PC += 2;
	clockCycles = 8;
	/* In very few instructions, 16 cycles are needed instead,
	which are added when the instruction is called.
	e.g. 0xCB06, 0xCB16, 0xCB26...
	*/
}
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
void CPU::op0xFF () {
	SP -= 2;
	mmu.WriteWord(SP, PC);

	PC = 0x38;
	clockCycles = 8;
}
// CB0. instructions
// RLC B
// RLC C
// RLC D
// RLC E
// RLC H
// RLC L
// RLC (HL)
// RLC A
// RRC B
// RRC C
// RRC D
// RRC E
// RRC H
// RRC L
// RRC (HL)
// RRC A
// CB1. instructions
// RL B
// RL C
// RL D
// RL E
// RL H
// RL L
// RL (HL)
// RL A
// RR B
// RR C
// RR D
// RR E
// RR H
// RR L
// RR (HL)
// RR A
// CB2. instructions
// SLA B
// SLA C
// SLA D
// SLA E
// SLA H
// SLA L
// SLA (HL)
// SLA A
// SRA B
// SRA C
// SRA D
// SRA E
// SRA H
// SRA L
// SRA (HL)
// SRA A
// CB3. instructions
// SWAP B
// SWAP C
// SWAP D
// SWAP E
// SWAP H
// SWAP L
// SWAP (HL)
// SWAP A
// SRL B
// SRL C
// SRL D
// SRL E
// SRL H
// SRL L
// SRL (HL)
// SRL A
// CB4.
// BIT 0,B
// BIT 0,C
// BIT 0,D
// BIT 0,E
// BIT 0,H
// BIT 0,L
// BIT 0,(HL)
// BIT 0,A
// BIT 1,B
// BIT 1,C
// BIT 1,D
// BIT 1,E
// BIT 1,H
// BIT 1,L
// BIT 1,(HL)
// BIT 1,A
// CB5. instructions
// BIT 2,B
// BIT 2,C
// BIT 2,D
// BIT 2,E
// BIT 2,H
// BIT 2,L
// BIT 2,(HL)
// BIT 2,A
// BIT 3,B
// BIT 3,C
// BIT 3,D
// BIT 3,E
// BIT 3,H
// BIT 3,L
// BIT 3,(HL)
// BIT 3,A
// CB6. instructions
// BIT 4,B
// BIT 4,C
// BIT 4,D
// BIT 4,E
// BIT 4,H
// BIT 4,L
// BIT 4,(HL)
// BIT 4,A
// BIT 5,B
// BIT 5,C
// BIT 5,D
// BIT 5,E
// BIT 5,H
// BIT 5,L
// BIT 5,(HL)
// BIT 5,A
// CB7. instructions
// BIT 6,B
// BIT 6,C
// BIT 6,D
// BIT 6,E
// BIT 6,H
// BIT 6,L
// BIT 6,(HL)
// BIT 6,A
// BIT 7,B
// BIT 7,C
// BIT 7,D
void CPU::cb0x7A () {
	Z = (DE.hi & 0x80) == 0;
	N = 0, H = 1;
}
// BIT 7,E
void CPU::cb0x7B () {
	Z = (DE.lo & 0x80) == 0;
	N = 0, H = 1;
}
// BIT 7,H
void CPU::cb0x7C () {
	Z = (HL.hi & 0x80) == 0;
	N = 0, H = 1;
}
// BIT 7,L
void CPU::cb0x7D () {
	Z = (HL.lo & 0x80) == 0;
	N = 0, H = 1;
}
// BIT 7,(HL)
void CPU::cb0x7E () {
	Z = (HL.lo & 0x80) == 0;
	N = 0, H = 1;
	clockCycles = 16;
}
// BIT 7,A
void CPU::cb0x7F () {
	Z = (AF.hi & 0x80) == 0;
	N = 0, H = 1;
}
// CB8. instructions
// RES 0,B
// RES 0,C
// RES 0,D
// RES 0,E
// RES 0,H
// RES 0,L
// RES 0,(HL)
// RES 0,A
// RES 1,B
// RES 1,C
// RES 1,D
// RES 1,E
// RES 1,H
// RES 1,L
// RES 1,(HL)
// RES 1,A
// CB9. instructions
// RES 2,B
// RES 2,C
// RES 2,D
// RES 2,E
// RES 2,H
// RES 2,L
// RES 2,(HL)
// RES 2,A
// RES 3,B
// RES 3,C
// RES 3,D
// RES 3,E
// RES 3,H
// RES 3,L
// RES 3,(HL)
// RES 3,A
// CBA. instructions
// RES 4,B
// RES 4,C
// RES 4,D
// RES 4,E
// RES 4,H
// RES 4,L
// RES 4,(HL)
// RES 4,A
// RES 5,B
// RES 5,C
// RES 5,D
// RES 5,E
// RES 5,H
// RES 5,L
// RES 5,(HL)
// RES 5,A
// CBB. instructions
// RES 6,B
// RES 6,C
// RES 6,D
// RES 6,E
// RES 6,H
// RES 6,L
// RES 6,(HL)
// RES 6,A
// RES 7,B
// RES 7,C
// RES 7,D
// RES 7,E
// RES 7,H
// RES 7,L
// RES 7,(HL)
// RES 7,A
// CBC. instructions
// SET 0,B
// SET 0,C
// SET 0,D
// SET 0,E
// SET 0,H
// SET 0,L
// SET 0,(HL)
// SET 0,A
// SET 1,B
// SET 1,C
// SET 1,D
// SET 1,E
// SET 1,H
// SET 1,L
// SET 1,(HL)
// SET 1,A
// CBD. instructions
// SET 2,B
// SET 2,C
// SET 2,D
// SET 2,E
// SET 2,H
// SET 2,L
// SET 2,(HL)
// SET 2,A
// SET 3,B
// SET 3,C
// SET 3,D
// SET 3,E
// SET 3,H
// SET 3,L
// SET 3,(HL)
// SET 3,A
// CBE. instructions
// SET 4,B
// SET 4,C
// SET 4,D
// SET 4,E
// SET 4,H
// SET 4,L
// SET 4,(HL)
// SET 4,A
// SET 5,B
// SET 5,C
// SET 5,D
// SET 5,E
// SET 5,H
// SET 5,L
// SET 5,(HL)
// SET 5,A
// CBF. instructions
// SET 6,B
// SET 6,C
// SET 6,D
// SET 6,E
// SET 6,H
// SET 6,L
// SET 6,(HL)
// SET 6,A
// SET 7,B
// SET 7,C
// SET 7,D
// SET 7,E
// SET 7,H
// SET 7,L
// SET 7,(HL)
// SET 7,A



/* Not implemented instructions call this function */
void CPU::opNull () {
	assert("Not implemented");
}
