#include <iostream>

#include "cpu.hpp"
#include "mmu.hpp"

#include "debug.hpp"
#include "disassembler.hpp"

CPU::CPU () {}
CPU::~CPU () {}

void CPU::Initialize (MMU* _mmu, bool doBootrom) {
	/* Initialize Registers */
	if (doBootrom) {
		AF = 0;
		BC = 0;
		DE = 0;
		HL = 0;
		SP = 0x0;
		PC = 0x0;
		SetZ(0); SetN(0); SetH(0); SetC(0);
	}
	else {
		AF = 0x01B0;
		BC = 0x0013;
		DE = 0x00D8;
		HL = 0x014D;
		SP = 0xFFFE;
		PC = 0x100;
		SetZ(1); SetN(0); SetH(1); SetC(1);
	}

	clockCycles = 0;

	assert("Assigned MMU wasn't initialized" && _mmu != nullptr);
	mmu = _mmu;

	InitializeOpcodeTable();
}

void CPU::EmulateCycle () {
	if (PC == 0x100) {
		mmu->isInBios = false;
		areInterruptsEnabled = true;
	}


	// std::cout << std::hex << (int) mmu->ReadByte(0xFFB1) << "\n";

	// if (0x69af) {
	// 	isHalted = true;
	// 	// Hi! I'm a badly executed memory dump, please don't mind me :D
	// 	std::cout << "0xFF86: " << std::to_string(mmu->ReadByte(0xff86)) << "\n";
	// 	std::cout << "0xFF87: " << std::to_string(mmu->ReadByte(0xff87)) << "\n";
	// 	std::cout << "0xFF85: " << std::to_string(mmu->ReadByte(0xff85)) << "\n";
	// 	std::cout << "0xFF84: " << std::to_string(mmu->ReadByte(0xff84)) << "\n";
	// 	std::cout << "0xFF83: " << std::to_string(mmu->ReadByte(0xff83)) << "\n";
	// 	std::cout << "0xFF82: " << std::to_string(mmu->ReadByte(0xff82)) << "\n";
	// 	std::cout << "0xFF81: " << std::to_string(mmu->ReadByte(0xff81)) << "\n";
	// 	std::cout << "0xFF80: " << std::to_string(mmu->ReadByte(0xff80)) << "\n";
	// }
	//
	// if (PC == 0x528 || PC == 0xC0B0) {
	// 	isHalted = true;
	// }

	uint8_t opcode = mmu->ReadByte(PC);
	std::cout << std::hex << PC << ' ' << DisassembleOpcode(mmu->GetMemoryRef(PC)) << '\n';

	// [MOST IMPORTANT LINE IN THIS WHOLE PROGRAM]
	ExecuteInstruction(opcode);
}

void CPU::ExecuteInstruction(uint8_t opcode) {
	// PC step must ocurr before executing the instruction,
	//so ReadByte() and ReadWord() will read the following bytes
	PC += 1;
	(this->*opcodes[opcode])(); // Wtf C++
}

void CPU::RequestInterrupt (uint8_t id) {
	uint8_t requestRegister = mmu->ReadByte(IF);
	requestRegister |= 0x10 >> (4 - id); // SET bit ID
	mmu->WriteByte(IF, requestRegister);
	// std::cout << "Interrupt requested!\n" ;
	// isHalted = true;
}

void CPU::ProcessInterrupts () {
	if (areInterruptsEnabled == false)
		return;

	const uint8_t requestRegister = mmu->ReadByte(IF);
	if (requestRegister == 0)
		return;

	const uint8_t enabledRegister = mmu->ReadByte(IE);
	for (size_t id = 0; id < 5; id++) {
		const bool isInterruptRequested = requestRegister & (0x10 >> (4 - id));
		const bool isInterruptEnabled   = enabledRegister & (0x10 >> (4 - id));

		if (isInterruptRequested && isInterruptEnabled) {
			DoInterrupt(id);
			// std::cout << "Doing interrupt " << id << "\n";
			// isHalted = true;
		}
	}
}

void CPU::DoInterrupt (uint8_t id) {
	areInterruptsEnabled = false;
	uint8_t requestRegister = mmu->ReadByte(IF);
	// Reset bit id, we use XOR because the bit has to be 1 before this function, so it always resets it
	requestRegister ^= 0x10 >> (4 - id);
	mmu->WriteByte(IF, requestRegister);

	PushWord(PC);
	switch (id) {
		case 0: PC = 0x40; break;
		case 1: PC = 0x48; break;
		case 2: PC = 0x50; break;
		case 4: PC = 0x60; break;
		default: assert("Invalid interrupt ID" && 0); break;
	}

	// std::cout << "Interrupting!" << "\n";
	// isHalted = true;
}

bool CPU::GetZ () const {
	return (AF.lo & 0x80);
}

bool CPU::GetN () const {
	return (AF.lo & 0x40);
}

bool CPU::GetH () const {
	return (AF.lo & 0x20);
}

bool CPU::GetC () const {
	return (AF.lo & 0x10);
}

void CPU::SetZ (bool value) {
	AF.lo = (AF.lo & ~0x80) | 0x80 * value;
}

void CPU::SetN (bool value) {
	AF.lo = (AF.lo & ~0x40) | 0x40 * value;
}

void CPU::SetH (bool value) {
	AF.lo = (AF.lo & ~0x20) | 0x20 * value;
}

void CPU::SetC (bool value) {
	AF.lo = (AF.lo & ~0x10) | 0x10 * value;
}

uint8_t CPU::ReadByte () {
	uint8_t value = mmu->ReadByte(PC);
	PC += 1;
	return value;
}

uint16_t CPU::ReadWord () {
	uint16_t value = mmu->ReadWord(PC);
	PC += 2;
	return value;
}

void CPU::PushWord (uint16_t value) {
	SP -= 2;
	mmu->WriteWord(SP, value);
}

uint16_t CPU::PopWord () {
	SP += 2;
	return mmu->ReadWord(SP - 2);
}

void CPU::Call (uint16_t address) {
	PushWord(PC);
	PC = address;
}

// FIXME: Double check C flag computation
void CPU::RotateLeft (uint8_t& value) {
	uint8_t oldBit7 = (value >> 7);
	value = (value << 1) | GetC();
	SetZ(value == 0);
	SetN(0), SetH(0);
	SetC(oldBit7);
}
void CPU::RotateLeftCarry (uint8_t& value) {
	uint8_t oldBit7 = (value >> 7);
	value = (value << 1);
	SetZ(value == 0);
	SetN(0), SetH(0);
	SetC(oldBit7);
}
void CPU::RotateRight (uint8_t& value) {
	uint8_t oldBit0 = (value & 1);
	value = (value >> 1) | (GetC() << 6);
	SetZ(value == 0);
	SetN(0), SetH(0);
	SetC(oldBit0);
}
void CPU::RotateRightCarry (uint8_t& value) {
	uint8_t oldBit0 = (value & 1);
	value = (value >> 1);
	SetZ(value == 0);
	SetN(0), SetH(0);
	SetC(oldBit0);
}

void CPU::ShiftLeft (uint8_t& value) {
	uint8_t oldBit7 = (value >> 7);
	value = (value << 1);
	SetZ(value == 0);
	SetN(0), SetH(0);
	SetC(oldBit7);
}

void CPU::ShiftRight (uint8_t& value) {
	uint8_t oldBit7 = (value >> 7);
	value = (value >> 1);
	SetZ(value == 0);
	SetN(0), SetH(0);
	SetC(oldBit7);
}

void CPU::Decrement (uint8_t& value) {
	uint8_t oldBit4 = value & 0x8;
	value -= 1;
	SetZ(value == 0);
	SetN(1);
	SetH( (value & 0x8) ^ oldBit4 && oldBit4 != 0x8 );
}

void CPU::Decrement (uint16_t& value) {
	value -= 1;
}

void CPU::Decrement (reg16_t& value) {
	Decrement(value.word);
}

void CPU::Increment (uint8_t& value) {
	uint8_t prev = value;
	value += 1;
	SetZ(value == 0);
	SetN(0);
	SetH((prev & 0xF) == 0xF);
}
// FIXME: Halfcarry in a 16bit value isn't on bit 4
void CPU::Increment (uint16_t& value) {
	uint16_t prev = value;
	value += 1;
	SetZ(value == 0);
	SetN(0);
	SetH((prev & 0xFF) == 0xFF);
}

void CPU::Increment (reg16_t& value) {
	Increment(value.word);
}

void CPU::AddCarryA (uint8_t value) {
	AddA(value + GetC());
}

void CPU::AddA (uint8_t value) {
	Add(AF.hi, value);
}

void CPU::Add (uint8_t& target, uint8_t value) {
	uint8_t oldValue = target;
	target += value;
	SetZ(target == 0);
	SetN(0);
	SetH((oldValue & 0xF) + (value & 0xF) > 0xF);
	SetC(oldValue + value > 0xFF);
}

void CPU::Add (uint16_t& target, uint16_t value) {
	uint16_t oldValue = target;
	target += value;
	SetZ(target == 0);
	SetN(0);
	SetH((oldValue & 0xF) + (value & 0xF) > 0xF);
	SetC(oldValue + value > 0xFF);
}

void CPU::SubtractCarryA (uint8_t value) {
	SubtractA(value + GetC());
}

void CPU::SubtractA (uint8_t value) {
	Subtract(AF.hi, value);
}

void CPU::Subtract (uint8_t& target, uint8_t value) {
	uint8_t oldTarget = target;
	target -= value;
	SetZ(target == 0);
	SetN(1);
	SetH((oldTarget & 0xF) < (target & 0xF));
	SetC(oldTarget < target);
}

void CPU::CompareA (uint8_t value) {
	SetZ(AF.hi == value);
	SetN(1);
	SetH((AF.hi & 0xF) < (value & 0xF));
	SetC(AF.hi < value);
}

void CPU::OrA (uint8_t value) {
	AF.hi |= value;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
}

void CPU::Swap (uint8_t& value) {
	uint8_t hi = value & 0xF0;
	uint8_t lo = value & 0x0F;
	value = (hi >> 4) | (lo << 4);
	SetZ(value == 0);
	SetN(0), SetH(0), SetC(0);
}

void CPU::SetBit (uint8_t& target, uint8_t bit) {
	uint8_t bitMask = 1 << bit;
	target = (target & ~bitMask) | 1 << bit;
}

void CPU::ResetBit (uint8_t& target, uint8_t bit) {
	uint8_t bitMask = 1 << bit;
	target = (target & ~bitMask) | 0 << bit;
}

void CPU::Bit(uint8_t bit, uint8_t value) {
	SetZ((value & (1 << bit)) == 0);
	SetH(1);
	SetN(0);
}

/* Instructions specific code */
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
	opcodes[0x12] = &CPU::op0x12; opcodes[0x13] = &CPU::op0x13;
	opcodes[0x14] = &CPU::op0x14; opcodes[0x15] = &CPU::op0x15;
	opcodes[0x16] = &CPU::op0x16; opcodes[0x17] = &CPU::op0x17;
	opcodes[0x18] = &CPU::op0x18; opcodes[0x19] = &CPU::op0x19;
	opcodes[0x1A] = &CPU::op0x1A; opcodes[0x1B] = &CPU::op0x1B;
	opcodes[0x1C] = &CPU::op0x1C; opcodes[0x1D] = &CPU::op0x1D;
	opcodes[0x1E] = &CPU::op0x1E; opcodes[0x1F] = &CPU::op0x1F;

	opcodes[0x20] = &CPU::op0x20; opcodes[0x21] = &CPU::op0x21;
	opcodes[0x22] = &CPU::op0x22; opcodes[0x23] = &CPU::op0x23;
	opcodes[0x24] = &CPU::op0x24; opcodes[0x25] = &CPU::op0x25;
	opcodes[0x26] = &CPU::op0x26; opcodes[0x27] = &CPU::op0x27;
	opcodes[0x28] = &CPU::op0x28; opcodes[0x29] = &CPU::op0x29;
	opcodes[0x2A] = &CPU::op0x2A; opcodes[0x2B] = &CPU::op0x2B;
	opcodes[0x2C] = &CPU::op0x2C; opcodes[0x2D] = &CPU::op0x2D;
	opcodes[0x2E] = &CPU::op0x2E; opcodes[0x2F] = &CPU::op0x2F;

	opcodes[0x30] = &CPU::op0x30; opcodes[0x31] = &CPU::op0x31;
	opcodes[0x32] = &CPU::op0x32; opcodes[0x33] = &CPU::op0x33;
	opcodes[0x34] = &CPU::op0x34; opcodes[0x35] = &CPU::op0x35;
	opcodes[0x36] = &CPU::op0x36; opcodes[0x37] = &CPU::op0x37;
	opcodes[0x38] = &CPU::op0x38; opcodes[0x39] = &CPU::op0x39;
	opcodes[0x3A] = &CPU::op0x3A; opcodes[0x3B] = &CPU::op0x3B;
	opcodes[0x3C] = &CPU::op0x3C; opcodes[0x3D] = &CPU::op0x3D;
	opcodes[0x3E] = &CPU::op0x3E; opcodes[0x3F] = &CPU::op0x3F;

	opcodes[0x40] = &CPU::op0x40; opcodes[0x41] = &CPU::op0x41;
	opcodes[0x42] = &CPU::op0x42; opcodes[0x43] = &CPU::op0x43;
	opcodes[0x44] = &CPU::op0x44; opcodes[0x45] = &CPU::op0x45;
	opcodes[0x46] = &CPU::op0x46; opcodes[0x47] = &CPU::op0x47;
	opcodes[0x48] = &CPU::op0x48; opcodes[0x49] = &CPU::op0x49;
	opcodes[0x4A] = &CPU::op0x4A; opcodes[0x4B] = &CPU::op0x4B;
	opcodes[0x4C] = &CPU::op0x4C; opcodes[0x4D] = &CPU::op0x4D;
	opcodes[0x4E] = &CPU::op0x4E; opcodes[0x4F] = &CPU::op0x4F;

	opcodes[0x50] = &CPU::op0x50; opcodes[0x51] = &CPU::op0x51;
	opcodes[0x52] = &CPU::op0x52; opcodes[0x53] = &CPU::op0x53;
	opcodes[0x54] = &CPU::op0x54; opcodes[0x55] = &CPU::op0x55;
	opcodes[0x56] = &CPU::op0x56; opcodes[0x57] = &CPU::op0x57;
	opcodes[0x58] = &CPU::op0x58; opcodes[0x59] = &CPU::op0x59;
	opcodes[0x5A] = &CPU::op0x5A; opcodes[0x5B] = &CPU::op0x5B;
	opcodes[0x5C] = &CPU::op0x5C; opcodes[0x5D] = &CPU::op0x5D;
	opcodes[0x5E] = &CPU::op0x5E; opcodes[0x5F] = &CPU::op0x5F;

	opcodes[0x60] = &CPU::op0x60; opcodes[0x61] = &CPU::op0x61;
	opcodes[0x62] = &CPU::op0x62; opcodes[0x63] = &CPU::op0x63;
	opcodes[0x64] = &CPU::op0x64; opcodes[0x65] = &CPU::op0x65;
	opcodes[0x66] = &CPU::op0x66; opcodes[0x67] = &CPU::op0x67;
	opcodes[0x68] = &CPU::op0x68; opcodes[0x69] = &CPU::op0x69;
	opcodes[0x6A] = &CPU::op0x6A; opcodes[0x6B] = &CPU::op0x6B;
	opcodes[0x6C] = &CPU::op0x6C; opcodes[0x6D] = &CPU::op0x6D;
	opcodes[0x6E] = &CPU::op0x6E; opcodes[0x6F] = &CPU::op0x6F;

	opcodes[0x70] = &CPU::op0x70; opcodes[0x71] = &CPU::op0x71;
	opcodes[0x72] = &CPU::op0x72; opcodes[0x73] = &CPU::op0x73;
	opcodes[0x74] = &CPU::op0x74; opcodes[0x75] = &CPU::op0x75;
	opcodes[0x76] = &CPU::op0x76; opcodes[0x77] = &CPU::op0x77;
	opcodes[0x78] = &CPU::op0x78; opcodes[0x79] = &CPU::op0x79;
	opcodes[0x7A] = &CPU::op0x7A; opcodes[0x7B] = &CPU::op0x7B;
	opcodes[0x7C] = &CPU::op0x7C; opcodes[0x7D] = &CPU::op0x7D;
	opcodes[0x7E] = &CPU::op0x7E; opcodes[0x7F] = &CPU::op0x7F;

	opcodes[0x80] = &CPU::op0x80; opcodes[0x81] = &CPU::op0x81;
	opcodes[0x82] = &CPU::op0x82; opcodes[0x83] = &CPU::op0x83;
	opcodes[0x84] = &CPU::op0x84; opcodes[0x85] = &CPU::op0x85;
	opcodes[0x86] = &CPU::op0x86; opcodes[0x87] = &CPU::op0x87;
	opcodes[0x88] = &CPU::op0x88; opcodes[0x89] = &CPU::op0x89;
	opcodes[0x8A] = &CPU::op0x8A; opcodes[0x8B] = &CPU::op0x8B;
	opcodes[0x8C] = &CPU::op0x8C; opcodes[0x8D] = &CPU::op0x8D;
	opcodes[0x8E] = &CPU::op0x8E; opcodes[0x8F] = &CPU::op0x8F;

	opcodes[0x90] = &CPU::op0x90; opcodes[0x91] = &CPU::op0x91;
	opcodes[0x92] = &CPU::op0x92; opcodes[0x93] = &CPU::op0x93;
	opcodes[0x94] = &CPU::op0x94; opcodes[0x95] = &CPU::op0x95;
	opcodes[0x96] = &CPU::op0x96; opcodes[0x97] = &CPU::op0x97;
	opcodes[0x98] = &CPU::op0x98; opcodes[0x99] = &CPU::op0x99;
	opcodes[0x9A] = &CPU::op0x9A; opcodes[0x9B] = &CPU::op0x9B;
	opcodes[0x9C] = &CPU::op0x9C; opcodes[0x9D] = &CPU::op0x9D;
	opcodes[0x9E] = &CPU::opNull; opcodes[0x9F] = &CPU::op0x9F;

	opcodes[0xA0] = &CPU::op0xA0; opcodes[0xA1] = &CPU::op0xA1;
	opcodes[0xA2] = &CPU::op0xA2; opcodes[0xA3] = &CPU::op0xA3;
	opcodes[0xA4] = &CPU::op0xA4; opcodes[0xA5] = &CPU::op0xA5;
	opcodes[0xA6] = &CPU::op0xA6; opcodes[0xA7] = &CPU::op0xA7;
	opcodes[0xA8] = &CPU::op0xA8; opcodes[0xA9] = &CPU::op0xA9;
	opcodes[0xAA] = &CPU::op0xAA; opcodes[0xAB] = &CPU::op0xAB;
	opcodes[0xAC] = &CPU::op0xAC; opcodes[0xAD] = &CPU::op0xAD;
	opcodes[0xAE] = &CPU::op0xAE; opcodes[0xAF] = &CPU::op0xAF;

	opcodes[0xB0] = &CPU::op0xB0; opcodes[0xB1] = &CPU::op0xB1;
	opcodes[0xB2] = &CPU::op0xB2; opcodes[0xB3] = &CPU::op0xB3;
	opcodes[0xB4] = &CPU::op0xB4; opcodes[0xB5] = &CPU::op0xB5;
	opcodes[0xB6] = &CPU::op0xB6; opcodes[0xB7] = &CPU::op0xB7;
	opcodes[0xB8] = &CPU::op0xB8; opcodes[0xB9] = &CPU::op0xB9;
	opcodes[0xBA] = &CPU::op0xBA; opcodes[0xBB] = &CPU::op0xBB;
	opcodes[0xBC] = &CPU::op0xBC; opcodes[0xBD] = &CPU::op0xBD;
	opcodes[0xBE] = &CPU::op0xBE; opcodes[0xBF] = &CPU::op0xBF;

	opcodes[0xC0] = &CPU::op0xC0; opcodes[0xC1] = &CPU::op0xC1;
	opcodes[0xC2] = &CPU::op0xC2; opcodes[0xC3] = &CPU::op0xC3;
	opcodes[0xC4] = &CPU::op0xC4; opcodes[0xC5] = &CPU::op0xC5;
	opcodes[0xC6] = &CPU::op0xC6; opcodes[0xC7] = &CPU::op0xC7;
	opcodes[0xC8] = &CPU::op0xC8; opcodes[0xC9] = &CPU::op0xC9;
	opcodes[0xCA] = &CPU::op0xCA; opcodes[0xCB] = &CPU::op0xCB;
	opcodes[0xCC] = &CPU::op0xCC; opcodes[0xCD] = &CPU::op0xCD;
	opcodes[0xCE] = &CPU::op0xCE; opcodes[0xCF] = &CPU::op0xCF;

	opcodes[0xD0] = &CPU::op0xD0; opcodes[0xD1] = &CPU::op0xD1;
	opcodes[0xD2] = &CPU::opNull; opcodes[0xD3] = &CPU::opNull;
	opcodes[0xD4] = &CPU::op0xD4; opcodes[0xD5] = &CPU::op0xD5;
	opcodes[0xD6] = &CPU::op0xD6; opcodes[0xD7] = &CPU::op0xD7;
	opcodes[0xD8] = &CPU::op0xD8; opcodes[0xD9] = &CPU::op0xD9;
	opcodes[0xDA] = &CPU::op0xDA; opcodes[0xDB] = &CPU::opNull;
	opcodes[0xDC] = &CPU::op0xDC; opcodes[0xDD] = &CPU::opNull;
	opcodes[0xDE] = &CPU::op0xDE; opcodes[0xDF] = &CPU::op0xDF;

	opcodes[0xE0] = &CPU::op0xE0; opcodes[0xE1] = &CPU::op0xE1;
	opcodes[0xE2] = &CPU::op0xE2; opcodes[0xE3] = &CPU::opNull;
	opcodes[0xE4] = &CPU::opNull; opcodes[0xE5] = &CPU::op0xE5;
	opcodes[0xE6] = &CPU::op0xE6; opcodes[0xE7] = &CPU::op0xE7;
	opcodes[0xE8] = &CPU::opNull; opcodes[0xE9] = &CPU::op0xE9;
	opcodes[0xEA] = &CPU::op0xEA; opcodes[0xEB] = &CPU::opNull;
	opcodes[0xEC] = &CPU::opNull; opcodes[0xED] = &CPU::opNull;
	opcodes[0xEE] = &CPU::op0xEE; opcodes[0xEF] = &CPU::op0xEF;

	opcodes[0xF0] = &CPU::op0xF0; opcodes[0xF1] = &CPU::op0xF1;
	opcodes[0xF2] = &CPU::op0xF2; opcodes[0xF3] = &CPU::op0xF3;
	opcodes[0xF4] = &CPU::opNull; opcodes[0xF5] = &CPU::op0xF5;
	opcodes[0xF6] = &CPU::op0xF6; opcodes[0xF7] = &CPU::op0xF7;
	opcodes[0xF8] = &CPU::opNull; opcodes[0xF9] = &CPU::op0xF9;
	opcodes[0xFA] = &CPU::op0xFA; opcodes[0xFB] = &CPU::op0xFB;
	opcodes[0xFC] = &CPU::opNull; opcodes[0xFD] = &CPU::opNull;
	opcodes[0xFE] = &CPU::op0xFE; opcodes[0xFF] = &CPU::op0xFF;

	/* -------------------------------------- */
	/* Initialize CB prefixed functions table */
	/* -------------------------------------- */
	opcodesCB[0x00] = &CPU::cb0x00; opcodesCB[0x01] = &CPU::cb0x01;
	opcodesCB[0x02] = &CPU::cb0x02; opcodesCB[0x03] = &CPU::cb0x03;
	opcodesCB[0x04] = &CPU::cb0x04; opcodesCB[0x05] = &CPU::cb0x05;
	opcodesCB[0x06] = &CPU::opNull; opcodesCB[0x07] = &CPU::cb0x07;
	opcodesCB[0x08] = &CPU::cb0x08; opcodesCB[0x09] = &CPU::cb0x09;
	opcodesCB[0x0A] = &CPU::cb0x0A; opcodesCB[0x0B] = &CPU::cb0x0B;
	opcodesCB[0x0C] = &CPU::cb0x0C; opcodesCB[0x0D] = &CPU::cb0x0D;
	opcodesCB[0x0E] = &CPU::opNull; opcodesCB[0x0F] = &CPU::cb0x0F;

	opcodesCB[0x10] = &CPU::cb0x10; opcodesCB[0x11] = &CPU::cb0x11;
	opcodesCB[0x12] = &CPU::cb0x12; opcodesCB[0x13] = &CPU::cb0x13;
	opcodesCB[0x14] = &CPU::cb0x14; opcodesCB[0x15] = &CPU::cb0x15;
	opcodesCB[0x16] = &CPU::cb0x16; opcodesCB[0x17] = &CPU::cb0x17;
	opcodesCB[0x18] = &CPU::cb0x18; opcodesCB[0x19] = &CPU::cb0x19;
	opcodesCB[0x1A] = &CPU::cb0x1A; opcodesCB[0x1B] = &CPU::cb0x1B;
	opcodesCB[0x1C] = &CPU::cb0x1C; opcodesCB[0x1D] = &CPU::cb0x1D;
	opcodesCB[0x1E] = &CPU::opNull; opcodesCB[0x1F] = &CPU::cb0x1F;

	opcodesCB[0x20] = &CPU::cb0x20; opcodesCB[0x21] = &CPU::cb0x21;
	opcodesCB[0x22] = &CPU::cb0x22; opcodesCB[0x23] = &CPU::cb0x23;
	opcodesCB[0x24] = &CPU::cb0x24; opcodesCB[0x25] = &CPU::cb0x25;
	opcodesCB[0x26] = &CPU::opNull; opcodesCB[0x27] = &CPU::cb0x27;
	opcodesCB[0x28] = &CPU::cb0x28; opcodesCB[0x29] = &CPU::cb0x29;
	opcodesCB[0x2A] = &CPU::cb0x2A; opcodesCB[0x2B] = &CPU::cb0x2B;
	opcodesCB[0x2C] = &CPU::cb0x2C; opcodesCB[0x2D] = &CPU::cb0x2D;
	opcodesCB[0x2E] = &CPU::opNull; opcodesCB[0x2F] = &CPU::cb0x2F;

	opcodesCB[0x30] = &CPU::cb0x30; opcodesCB[0x31] = &CPU::cb0x31;
	opcodesCB[0x32] = &CPU::cb0x32; opcodesCB[0x33] = &CPU::cb0x33;
	opcodesCB[0x34] = &CPU::cb0x34; opcodesCB[0x35] = &CPU::cb0x35;
	opcodesCB[0x36] = &CPU::opNull; opcodesCB[0x37] = &CPU::cb0x37;
	opcodesCB[0x38] = &CPU::cb0x38; opcodesCB[0x39] = &CPU::cb0x39;
	opcodesCB[0x3A] = &CPU::cb0x3A; opcodesCB[0x3B] = &CPU::cb0x3B;
	opcodesCB[0x3C] = &CPU::cb0x3C; opcodesCB[0x3D] = &CPU::cb0x3D;
	opcodesCB[0x3E] = &CPU::opNull; opcodesCB[0x3F] = &CPU::cb0x3F;

	opcodesCB[0x40] = &CPU::cb0x40; opcodesCB[0x41] = &CPU::cb0x41;
	opcodesCB[0x42] = &CPU::cb0x42; opcodesCB[0x43] = &CPU::cb0x43;
	opcodesCB[0x44] = &CPU::cb0x44; opcodesCB[0x45] = &CPU::cb0x45;
	opcodesCB[0x46] = &CPU::cb0x46; opcodesCB[0x47] = &CPU::cb0x47;
	opcodesCB[0x48] = &CPU::cb0x48; opcodesCB[0x49] = &CPU::opNull;
	opcodesCB[0x4A] = &CPU::opNull; opcodesCB[0x4B] = &CPU::opNull;
	opcodesCB[0x4C] = &CPU::opNull; opcodesCB[0x4D] = &CPU::opNull;
	opcodesCB[0x4E] = &CPU::opNull; opcodesCB[0x4F] = &CPU::cb0x4F;

	opcodesCB[0x50] = &CPU::cb0x50; opcodesCB[0x51] = &CPU::cb0x51;
	opcodesCB[0x52] = &CPU::cb0x52; opcodesCB[0x53] = &CPU::cb0x53;
	opcodesCB[0x54] = &CPU::cb0x54; opcodesCB[0x55] = &CPU::cb0x55;
	opcodesCB[0x56] = &CPU::cb0x56; opcodesCB[0x57] = &CPU::cb0x57;
	opcodesCB[0x58] = &CPU::cb0x58; opcodesCB[0x59] = &CPU::cb0x59;
	opcodesCB[0x5A] = &CPU::cb0x5A; opcodesCB[0x5B] = &CPU::cb0x5B;
	opcodesCB[0x5C] = &CPU::cb0x5C; opcodesCB[0x5D] = &CPU::cb0x5D;
	opcodesCB[0x5E] = &CPU::cb0x5E; opcodesCB[0x5F] = &CPU::cb0x5F;

	opcodesCB[0x60] = &CPU::cb0x60; opcodesCB[0x61] = &CPU::cb0x61;
	opcodesCB[0x62] = &CPU::cb0x62; opcodesCB[0x63] = &CPU::cb0x63;
	opcodesCB[0x64] = &CPU::cb0x64; opcodesCB[0x65] = &CPU::cb0x65;
	opcodesCB[0x66] = &CPU::cb0x66; opcodesCB[0x67] = &CPU::cb0x67;
	opcodesCB[0x68] = &CPU::cb0x68; opcodesCB[0x69] = &CPU::cb0x69;
	opcodesCB[0x6A] = &CPU::cb0x6A; opcodesCB[0x6B] = &CPU::cb0x6B;
	opcodesCB[0x6C] = &CPU::cb0x6C; opcodesCB[0x6D] = &CPU::op0x6D;
	opcodesCB[0x6E] = &CPU::cb0x6E; opcodesCB[0x6F] = &CPU::cb0x6F;

	opcodesCB[0x70] = &CPU::cb0x70; opcodesCB[0x71] = &CPU::cb0x71;
	opcodesCB[0x72] = &CPU::cb0x72; opcodesCB[0x73] = &CPU::cb0x73;
	opcodesCB[0x74] = &CPU::cb0x74; opcodesCB[0x75] = &CPU::cb0x75;
	opcodesCB[0x76] = &CPU::cb0x76; opcodesCB[0x77] = &CPU::cb0x77;
	opcodesCB[0x78] = &CPU::cb0x78; opcodesCB[0x79] = &CPU::cb0x79;
	opcodesCB[0x7A] = &CPU::cb0x7A; opcodesCB[0x7B] = &CPU::cb0x7B;
	opcodesCB[0x7C] = &CPU::cb0x7C; opcodesCB[0x7D] = &CPU::cb0x7D;
	opcodesCB[0x7E] = &CPU::cb0x7E; opcodesCB[0x7F] = &CPU::cb0x7F;

	opcodesCB[0x80] = &CPU::cb0x80; opcodesCB[0x81] = &CPU::cb0x81;
	opcodesCB[0x82] = &CPU::cb0x82; opcodesCB[0x83] = &CPU::cb0x83;
	opcodesCB[0x84] = &CPU::cb0x84; opcodesCB[0x85] = &CPU::cb0x85;
	opcodesCB[0x86] = &CPU::cb0x86; opcodesCB[0x87] = &CPU::cb0x87;
	opcodesCB[0x88] = &CPU::cb0x88; opcodesCB[0x89] = &CPU::opNull;
	opcodesCB[0x8A] = &CPU::opNull; opcodesCB[0x8B] = &CPU::opNull;
	opcodesCB[0x8C] = &CPU::opNull; opcodesCB[0x8D] = &CPU::opNull;
	opcodesCB[0x8E] = &CPU::opNull; opcodesCB[0x8F] = &CPU::opNull;

	opcodesCB[0x90] = &CPU::opNull; opcodesCB[0x91] = &CPU::opNull;
	opcodesCB[0x92] = &CPU::opNull; opcodesCB[0x93] = &CPU::opNull;
	opcodesCB[0x94] = &CPU::opNull; opcodesCB[0x95] = &CPU::opNull;
	opcodesCB[0x96] = &CPU::opNull; opcodesCB[0x97] = &CPU::opNull;
	opcodesCB[0x98] = &CPU::opNull; opcodesCB[0x99] = &CPU::opNull;
	opcodesCB[0x9A] = &CPU::opNull; opcodesCB[0x9B] = &CPU::opNull;
	opcodesCB[0x9C] = &CPU::opNull; opcodesCB[0x9D] = &CPU::opNull;
	opcodesCB[0x9E] = &CPU::cb0x9E; opcodesCB[0x9F] = &CPU::opNull;

	opcodesCB[0xA0] = &CPU::opNull; opcodesCB[0xA1] = &CPU::opNull;
	opcodesCB[0xA2] = &CPU::opNull; opcodesCB[0xA3] = &CPU::opNull;
	opcodesCB[0xA4] = &CPU::opNull; opcodesCB[0xA5] = &CPU::opNull;
	opcodesCB[0xA6] = &CPU::opNull; opcodesCB[0xA7] = &CPU::opNull;
	opcodesCB[0xA8] = &CPU::opNull; opcodesCB[0xA9] = &CPU::opNull;
	opcodesCB[0xAA] = &CPU::opNull; opcodesCB[0xAB] = &CPU::opNull;
	opcodesCB[0xAC] = &CPU::opNull; opcodesCB[0xAD] = &CPU::opNull;
	opcodesCB[0xAE] = &CPU::opNull; opcodesCB[0xAF] = &CPU::opNull;

	opcodesCB[0xB0] = &CPU::opNull; opcodesCB[0xB1] = &CPU::opNull;
	opcodesCB[0xB2] = &CPU::opNull; opcodesCB[0xB3] = &CPU::opNull;
	opcodesCB[0xB4] = &CPU::opNull; opcodesCB[0xB5] = &CPU::opNull;
	opcodesCB[0xB6] = &CPU::opNull; opcodesCB[0xB7] = &CPU::opNull;
	opcodesCB[0xB8] = &CPU::opNull; opcodesCB[0xB9] = &CPU::opNull;
	opcodesCB[0xBA] = &CPU::opNull; opcodesCB[0xBB] = &CPU::opNull;
	opcodesCB[0xBC] = &CPU::opNull; opcodesCB[0xBD] = &CPU::opNull;
	opcodesCB[0xBE] = &CPU::cb0xBE; opcodesCB[0xBF] = &CPU::opNull;

	opcodesCB[0xC0] = &CPU::opNull; opcodesCB[0xC1] = &CPU::opNull;
	opcodesCB[0xC2] = &CPU::opNull; opcodesCB[0xC3] = &CPU::opNull;
	opcodesCB[0xC4] = &CPU::opNull; opcodesCB[0xC5] = &CPU::opNull;
	opcodesCB[0xC6] = &CPU::opNull; opcodesCB[0xC7] = &CPU::opNull;
	opcodesCB[0xC8] = &CPU::opNull; opcodesCB[0xC9] = &CPU::opNull;
	opcodesCB[0xCA] = &CPU::opNull; opcodesCB[0xCB] = &CPU::opNull;
	opcodesCB[0xCC] = &CPU::opNull; opcodesCB[0xCD] = &CPU::opNull;
	opcodesCB[0xCE] = &CPU::opNull; opcodesCB[0xCF] = &CPU::opNull;

	opcodesCB[0xD0] = &CPU::cb0xD0; opcodesCB[0xD1] = &CPU::opNull;
	opcodesCB[0xD2] = &CPU::opNull; opcodesCB[0xD3] = &CPU::opNull;
	opcodesCB[0xD4] = &CPU::opNull; opcodesCB[0xD5] = &CPU::opNull;
	opcodesCB[0xD6] = &CPU::opNull; opcodesCB[0xD7] = &CPU::opNull;
	opcodesCB[0xD8] = &CPU::cb0xD8; opcodesCB[0xD9] = &CPU::opNull;
	opcodesCB[0xDA] = &CPU::opNull; opcodesCB[0xDB] = &CPU::opNull;
	opcodesCB[0xDC] = &CPU::opNull; opcodesCB[0xDD] = &CPU::opNull;
	opcodesCB[0xDE] = &CPU::cb0xDE; opcodesCB[0xDF] = &CPU::opNull;

	opcodesCB[0xE0] = &CPU::opNull; opcodesCB[0xE1] = &CPU::opNull;
	opcodesCB[0xE2] = &CPU::opNull; opcodesCB[0xE3] = &CPU::opNull;
	opcodesCB[0xE4] = &CPU::opNull; opcodesCB[0xE5] = &CPU::opNull;
	opcodesCB[0xE6] = &CPU::opNull; opcodesCB[0xE7] = &CPU::opNull;
	opcodesCB[0xE8] = &CPU::opNull; opcodesCB[0xE9] = &CPU::opNull;
	opcodesCB[0xEA] = &CPU::opNull; opcodesCB[0xEB] = &CPU::opNull;
	opcodesCB[0xEC] = &CPU::opNull; opcodesCB[0xED] = &CPU::opNull;
	opcodesCB[0xEE] = &CPU::opNull; opcodesCB[0xEF] = &CPU::opNull;

	opcodesCB[0xF0] = &CPU::cb0xF0; opcodesCB[0xF1] = &CPU::opNull;
	opcodesCB[0xF2] = &CPU::opNull; opcodesCB[0xF3] = &CPU::opNull;
	opcodesCB[0xF4] = &CPU::opNull; opcodesCB[0xF5] = &CPU::opNull;
	opcodesCB[0xF6] = &CPU::opNull; opcodesCB[0xF7] = &CPU::opNull;
	opcodesCB[0xF8] = &CPU::cb0xF8; opcodesCB[0xF9] = &CPU::opNull;
	opcodesCB[0xFA] = &CPU::opNull; opcodesCB[0xFB] = &CPU::opNull;
	opcodesCB[0xFC] = &CPU::opNull; opcodesCB[0xFD] = &CPU::opNull;
	opcodesCB[0xFE] = &CPU::cb0xFE; opcodesCB[0xFF] = &CPU::cb0xFF;

}


// NOP
void CPU::op0x00 () {
	// isHalted = true;
	clockCycles = 4;
}
// STOP 0
void CPU::op0x10 () {
	clockCycles = 4;
}
// JR NZ,r8
void CPU::op0x20 () {
	uint8_t value = ReadByte();

	if (GetZ() == 0) {
		PC += reinterpret_cast<int8_t&>(value);
		clockCycles = 12;
	}
	else
		clockCycles = 8;
}
// JR NC,r8
void CPU::op0x30 () {
	uint8_t value = ReadByte();

	if (GetC() == 0) {
		PC += reinterpret_cast<int8_t&>(value);
		clockCycles = 12;
	}
	else
		clockCycles = 8;
}

// LD BC,D16
void CPU::op0x01 () {
	BC = ReadWord();
	clockCycles = 12;
}
// LD DE,D16
void CPU::op0x11 () {
	DE = ReadWord();
	clockCycles = 12;
}
// LD HL,D16
void CPU::op0x21 () {
	HL = ReadWord();
	clockCycles = 12;
}
// LD SP,D16
void CPU::op0x31 () {
	SP = ReadWord();
	clockCycles = 12;
}

// LD (BC),A
void CPU::op0x02 () {
	mmu->WriteByte(BC, AF.hi);
	clockCycles = 8;
}
// LD (DE),A
void CPU::op0x12 () {
	mmu->WriteByte(DE, AF.hi);
	clockCycles = 8;
}
// LD (HL+),A
void CPU::op0x22 () {
	mmu->WriteByte(HL, AF.hi);
	HL += 1;
	clockCycles = 8;
}
// LD (HL-),A
void CPU::op0x32 () {
	mmu->WriteByte(HL, AF.hi);
	HL -= 1;
	clockCycles = 8;
}

// INC BC
void CPU::op0x03 () {
	BC += 1;
	clockCycles = 8;
}
// INC DE
void CPU::op0x13 () {
	DE += 1;
	clockCycles = 8;
}
// INC HL
void CPU::op0x23 () {
	HL += 1;
	clockCycles = 8;
}
// INC SP
void CPU::op0x33 () {
	SP += 1;
	clockCycles = 8;
}

// INC B
void CPU::op0x04 () {
	Increment(BC.hi);
	clockCycles = 4;
}
// INC D
void CPU::op0x14 () {
	Increment(DE.hi);
	clockCycles = 4;
}
// INC H
void CPU::op0x24 () {
	Increment(HL.hi);
	clockCycles = 4;
}
// INC (HL)
void CPU::op0x34 () {
	uint8_t value = mmu->ReadByte(HL);
	Increment(value);
	mmu->WriteByte(HL, value);
	clockCycles = 4;
}

// DEC B
void CPU::op0x05 () {
	Decrement(BC.hi);
	clockCycles = 4;
}
// DEC D
void CPU::op0x15 () {
	Decrement(DE.hi);
	clockCycles = 4;
}
// DEC H
void CPU::op0x25 () {
	Decrement(HL.hi);
	clockCycles = 4;
}
// DEC (HL)
void CPU::op0x35 () {
	uint8_t value = mmu->ReadByte(HL);
	Decrement(value);
	mmu->WriteByte(HL, value);
	clockCycles = 4;
}

// LD B,d8
void CPU::op0x06 () {
	BC.hi = ReadByte();

	clockCycles = 8;
}
// LD D,d8
void CPU::op0x16 () {
	DE.hi = ReadByte();
	clockCycles = 12;
}
// LD H,d8
void CPU::op0x26 () {
	HL.hi = ReadByte();
	clockCycles = 12;
}
// LD (HL),d8
void CPU::op0x36 () {
	mmu->WriteByte(HL, ReadByte());
	clockCycles = 12;
}

// RLCA
void CPU::op0x07 () {
	RotateLeftCarry(AF.hi);
	clockCycles = 4;
}
// RLA
// FIXME: Maybe RLA isn't the same as RL A, check Flags on pastraiser
void CPU::op0x17 () {
	RotateLeft(AF.hi);
	clockCycles = 4;
}
// DAA // FIXME: Maybe wrong
void CPU::op0x27 () {
	uint8_t oldA = AF.hi;
	uint8_t upperNibble = (AF.hi & 0xF0) >> 4;
	uint8_t lowerNibble = (AF.hi & 0x0F);
	if (lowerNibble > 9 || GetH()) {
		AF.hi += 0x6;
	}
	if (upperNibble > 9 || GetC()) {
		AF.hi += 0x60;
	}
	SetZ(AF.hi == 0);
	SetH(0);
	SetC(oldA > AF.hi);
	clockCycles = 4;
}
// SCF
void CPU::op0x37 () {
	SetN(0); SetH(0); SetC(1);
	clockCycles = 4;
}

// LD (a16),SP
void CPU::op0x08 () {
	mmu->WriteByte(ReadWord(), SP);
	clockCycles = 20;
}
// JR r8
void CPU::op0x18 () {
	uint8_t value = ReadByte();

	PC += reinterpret_cast<int8_t&>(value);
	clockCycles = 12;
}
// JR Z,r8
void CPU::op0x28 () {
	uint8_t value = ReadByte();

	if (GetZ() == 1) {
		PC += reinterpret_cast<int8_t&>(value);
		clockCycles = 12;
	}
	else
		clockCycles = 8;
}
// JR C,r8
void CPU::op0x38 () {
	uint8_t value = ReadByte();

	if (GetC() == 1) {
		PC += reinterpret_cast<int8_t&>(value);
		clockCycles = 12;
	}
	else
		clockCycles = 8;
}

// ADD HL,BC
void CPU::op0x09 () {
	Add(HL.word, BC);
	clockCycles = 8;
}
// ADD HL,DE
void CPU::op0x19 () {
	Add(HL.word, DE);
	clockCycles = 8;
}
// ADD HL,HL
void CPU::op0x29 () {
	Add(HL.word, HL);
	clockCycles = 8;
}
// ADD HL,SP
void CPU::op0x39 () {
	Add(HL.word, SP);
	clockCycles = 8;
}

// LD A,(BC)
void CPU::op0x0A () {
	AF.hi = mmu->ReadByte(BC);
	clockCycles = 8;
}
// LD A,(DE)
void CPU::op0x1A () {
	AF.hi = mmu->ReadByte(DE);
	clockCycles = 8;
}
// LD A,(HL+)
void CPU::op0x2A () {
	AF.hi = mmu->ReadByte(HL);
	HL += 1;
	clockCycles = 8;
}
// LD A,(HL-)
void CPU::op0x3A () {
	AF.hi = mmu->ReadByte(HL);
	HL -= 1;
	clockCycles = 8;
}

// DEC BC
void CPU::op0x0B () {
	Decrement(BC);
	clockCycles = 8;
}
// DEC DE
void CPU::op0x1B () {
	Decrement(DE);
	clockCycles = 8;
}
// DEC HL
void CPU::op0x2B () {
	Decrement(HL);
	clockCycles = 8;
}
// DEC SP
void CPU::op0x3B () {
	Decrement(SP);
	clockCycles = 8;
}

// INC C
void CPU::op0x0C () {
	Increment(BC.lo);
	clockCycles = 4;
}
// INC E
void CPU::op0x1C () {
	Increment(DE.lo);
	clockCycles = 4;
}
// INC L
void CPU::op0x2C () {
	Increment(HL.lo);
	clockCycles = 4;
}
// INC A
void CPU::op0x3C () {
	Increment(AF.hi);
	clockCycles = 4;
}

// DEC C
void CPU::op0x0D () {
	Decrement(BC.lo);
	clockCycles = 4;
}
// DEC E
void CPU::op0x1D () {
	Decrement(DE.lo);
	clockCycles = 4;
}
// DEC L
void CPU::op0x2D () {
	Decrement(HL.lo);
	clockCycles = 4;
}
// DEC A
void CPU::op0x3D () {
	Decrement(AF.hi);
	clockCycles = 4;
}

// LD C,d8
void CPU::op0x0E () {
	BC.lo = ReadByte();
	clockCycles = 8;
}
// LD E,d8
void CPU::op0x1E () {
	DE.lo = ReadByte();
	clockCycles = 8;
}
// LD L,d8
void CPU::op0x2E () {
	HL.lo = ReadByte();
	clockCycles = 8;
}
// LD A,d8
void CPU::op0x3E () {
	AF.hi = ReadByte();
	clockCycles = 8;
}

// RRCA
void CPU::op0x0F () {
	RotateRightCarry(AF.hi);
	clockCycles = 4;
}
// RRA
void CPU::op0x1F () {
	RotateRight(AF.hi);
}
// CPL
void CPU::op0x2F () {
	AF.hi ^= 0xFF;
	SetN(1), SetH(1);
	clockCycles = 4;
}
// CCF
void CPU::op0x3F () {
	SetN(0); SetH(0); SetC(!GetC());
	clockCycles = 4;
}

/* 4. instructions */
// LD B,B
void CPU::op0x40 () {
	BC.hi = BC.hi;
	clockCycles = 4;
}
// LD B,C
void CPU::op0x41 () {
	BC.hi = BC.lo;
	clockCycles = 4;
}
// LD B,D
void CPU::op0x42 () {
	BC.hi = DE.hi;
	clockCycles = 4;
}
// LD B,E
void CPU::op0x43 () {
	BC.hi = DE.lo;
	clockCycles = 4;
}
// LD B,H
void CPU::op0x44 () {
	BC.hi = HL.hi;
	clockCycles = 4;
}
// LD B,L
void CPU::op0x45 () {
	BC.hi = HL.lo;
	clockCycles = 4;
}
// LD B,(HL)
void CPU::op0x46 () {
	BC.hi = mmu->ReadByte(HL);
	clockCycles = 8;
}
// LD B,A
void CPU::op0x47 () {
	BC.hi = AF.hi;
	clockCycles = 4;
}
// LD C,B
void CPU::op0x48 () {
	BC.lo = BC.hi;
	clockCycles = 4;
}
// LD C,C
void CPU::op0x49 () {
	BC.lo = BC.lo;
	clockCycles = 4;
}
// LD C,D
void CPU::op0x4A () {
	BC.lo = DE.hi;
	clockCycles = 4;
}
// LD C,E
void CPU::op0x4B () {
	BC.lo = DE.lo;
	clockCycles = 4;
}
// LD C,H
void CPU::op0x4C () {
	BC.lo = HL.hi;
	clockCycles = 4;
}
// LD C,L
void CPU::op0x4D () {
	BC.lo = HL.lo;
	clockCycles = 4;
}
// LD C,(HL)
void CPU::op0x4E () {
	BC.lo = mmu->ReadByte(HL);
	clockCycles = 4;
}
// LD C,A
void CPU::op0x4F () {
	BC.lo = AF.hi;
	clockCycles = 4;
}

/* 5. instructions */
// LD D,B
void CPU::op0x50 () {
	DE.hi = BC.hi;
	clockCycles = 4;
}
// LD D,C
void CPU::op0x51 () {
	DE.hi = BC.lo;
	clockCycles = 4;
}
// LD D,D
void CPU::op0x52 () {
	DE.hi = DE.hi;
	clockCycles = 4;
}
// LD D,E
void CPU::op0x53 () {
	DE.hi = DE.lo;
	clockCycles = 4;
}
// LD D,H
void CPU::op0x54 () {
	DE.hi = HL.hi;
	clockCycles = 4;
}
// LD D,L
void CPU::op0x55 () {
	DE.hi = HL.lo;
	clockCycles = 4;
}
// LD D,(HL)
void CPU::op0x56 () {
	DE.hi = mmu->ReadByte(HL);
	clockCycles = 4;
}
// LD D,A
void CPU::op0x57 () {
	DE.hi = AF.hi;
	clockCycles = 4;
}
// LD E,B
void CPU::op0x58 () {
	DE.lo = BC.hi;
	clockCycles = 4;
}
// LD E,C
void CPU::op0x59 () {
	DE.lo = BC.lo;
	clockCycles = 4;
}
// LD E,D
void CPU::op0x5A () {
	DE.lo = DE.hi;
	clockCycles = 4;
}
// LD E,E
void CPU::op0x5B () {
	DE.lo = DE.lo;
	clockCycles = 4;
}
// LD E,H
void CPU::op0x5C () {
	DE.lo = HL.hi;
	clockCycles = 4;
}
// LD E,L
void CPU::op0x5D () {
	DE.lo = HL.lo;
	clockCycles = 4;
}
// LD E,(HL)
void CPU::op0x5E () {
	DE.lo = mmu->ReadByte(HL);
	clockCycles = 4;
}
// LD E,A
void CPU::op0x5F () {
	DE.lo = AF.hi;
	clockCycles = 4;
}

/* 6. instructions */
// LD H,B
void CPU::op0x60 () {
	HL.hi = BC.hi;
	clockCycles = 4;
}
// LD H,C
void CPU::op0x61 () {
	HL.hi = BC.lo;
	clockCycles = 4;
}
// LD H,D
void CPU::op0x62 () {
	HL.hi = DE.hi;
	clockCycles = 4;
}
// LD H,E
void CPU::op0x63 () {
	HL.hi = DE.lo;
	clockCycles = 4;
}
// LD H,H
void CPU::op0x64 () {
	HL.hi = HL.hi;
	clockCycles = 4;
}
// LD H,L
void CPU::op0x65 () {
	HL.hi = HL.lo;
	clockCycles = 4;
}
// LD H,(HL)
void CPU::op0x66 () {
	HL.hi = mmu->ReadByte(HL);
	clockCycles = 4;
}
// LD H,A
void CPU::op0x67 () {
	HL.hi = AF.hi;
	clockCycles = 4;
}
// LD L,B
void CPU::op0x68 () {
	HL.lo = BC.hi;
	clockCycles = 4;
}
// LD L,C
void CPU::op0x69 () {
	HL.lo = BC.lo;
	clockCycles = 4;
}
// LD L,D
void CPU::op0x6A () {
	HL.lo = DE.hi;
	clockCycles = 4;
}
// LD L,E
void CPU::op0x6B () {
	HL.lo = DE.lo;
	clockCycles = 4;
}
// LD L,H
void CPU::op0x6C () {
	HL.lo = HL.hi;
	clockCycles = 4;
}
// LD L,L
void CPU::op0x6D () {
	HL.lo = HL.lo;
	clockCycles = 4;
}
// LD L,(HL)
void CPU::op0x6E () {
	HL.lo = mmu->ReadByte(HL);
	clockCycles = 4;
}
// LD L,A
void CPU::op0x6F () {
	HL.lo = AF.hi;
	clockCycles = 4;
}

/* 7. instructions */
// LD (HL),B
void CPU::op0x70 () {
	mmu->WriteByte(HL, BC.hi);
	clockCycles = 8;
}
// LD (HL),C
void CPU::op0x71 () {
	mmu->WriteByte(HL, BC.lo);
	clockCycles = 8;
}
// LD (HL),D
void CPU::op0x72 () {
	mmu->WriteByte(HL, DE.hi);
	clockCycles = 8;
}
// LD (HL),E
void CPU::op0x73 () {
	mmu->WriteByte(HL, DE.lo);
	clockCycles = 8;
}
// LD (HL),H
void CPU::op0x74 () {
	mmu->WriteByte(HL, HL.hi);
	clockCycles = 8;
}
// LD (HL),L
void CPU::op0x75 () {
	mmu->WriteByte(HL, HL.lo);
	clockCycles = 8;
}
// HALT
void CPU::op0x76 () {
	clockCycles = 4;
}
// LD (HL),A
void CPU::op0x77 () {
	mmu->WriteByte(HL, AF.hi);
	clockCycles = 8;
}
// LD A,B
void CPU::op0x78 () {
	AF.hi = BC.hi;
	clockCycles = 4;
}
// LD A,C
void CPU::op0x79 () {
	AF.hi = BC.lo;
	clockCycles = 4;
}
// LD A,D
void CPU::op0x7A () {
	AF.hi = DE.hi;
	clockCycles = 4;
}
// LD A,E
void CPU::op0x7B () {
	AF.hi = DE.lo;
	clockCycles = 4;
}
// LD A,H
void CPU::op0x7C () {
	AF.hi = HL.hi;
	clockCycles = 4;
}
// LD A,L
void CPU::op0x7D () {
	AF.hi = HL.lo;
	clockCycles = 4;
}
// LD A,(HL)
void CPU::op0x7E () {
	AF.hi = mmu->ReadByte(HL);
	clockCycles = 8;
}
// LD A,A
void CPU::op0x7F () {
	AF.hi = AF.hi;
	clockCycles = 4;
}

/* 8. instructions */
// ADD A,B
void CPU::op0x80 () {
	AddA(BC.hi);
	clockCycles = 4;
}
// ADD A,C
void CPU::op0x81 () {
	AddA(BC.lo);
	clockCycles = 4;
}
// ADD A,D
void CPU::op0x82 () {
	AddA(DE.hi);
	clockCycles = 4;
}
// ADD A,E
void CPU::op0x83 () {
	AddA(DE.lo);
	clockCycles = 4;
}
// ADD A,H
void CPU::op0x84 () {
	AddA(HL.hi);
	clockCycles = 4;
}
// ADD A,L
void CPU::op0x85 () {
	AddA(HL.lo);
	clockCycles = 4;
}
// ADD A,(HL)
void CPU::op0x86 () {
	AddA(mmu->ReadByte(HL));
	clockCycles = 8;
}
// ADD A,A
void CPU::op0x87 () {
	AddA(AF.hi);
	clockCycles = 4;
}
// ADC A,B
void CPU::op0x88 () {
	AddCarryA(BC.hi);
	clockCycles = 4;
}
// ADC A,C
void CPU::op0x89 () {
	AddCarryA(BC.lo);
	clockCycles = 4;
}
// ADC A,D
void CPU::op0x8A () {
	AddCarryA(DE.hi);
	clockCycles = 4;
}
// ADC A,E
void CPU::op0x8B () {
	AddCarryA(DE.lo);
	clockCycles = 4;
}
// ADC A,H
void CPU::op0x8C () {
	AddCarryA(HL.hi);
	clockCycles = 4;
}
// ADC A,L
void CPU::op0x8D () {
	AddCarryA(HL.lo);
	clockCycles = 4;
}
// ADC A,(HL)
void CPU::op0x8E () {
	uint8_t value = mmu->ReadByte(HL);
	AddCarryA(value);
	mmu->WriteByte(HL, value);
	clockCycles = 8;
}
// ADC A,A
void CPU::op0x8F () {
	AddCarryA(AF.hi);
	clockCycles = 4;
}

/* 9. instructions */
// SUB B
void CPU::op0x90 () {
	SubtractA(BC.hi);
	clockCycles = 4;
}
// SUB C
void CPU::op0x91 () {
	SubtractA(BC.lo);
	clockCycles = 4;
}
// SUB D
void CPU::op0x92 () {
	SubtractA(DE.hi);
	clockCycles = 4;
}
// SUB E
void CPU::op0x93 () {
	SubtractA(DE.lo);
	clockCycles = 4;
}
// SUB H
void CPU::op0x94 () {
	SubtractA(HL.hi);
	clockCycles = 4;
}
// SUB L
void CPU::op0x95 () {
	SubtractA(HL.lo);
	clockCycles = 4;
}
// SUB (HL)
void CPU::op0x96 () {
	SubtractA(mmu->ReadByte(HL));
	clockCycles = 8;
}
// SUB A
void CPU::op0x97 () {
	SubtractA(AF.hi);
	clockCycles = 4;
}
// SBC A,B
void CPU::op0x98 () {
	SubtractCarryA(AF.hi);
	clockCycles = 4;
}
// SBC A,C
void CPU::op0x99 () {
	SubtractCarryA(BC.lo);
	clockCycles = 4;
}
// SBC A,D
void CPU::op0x9A () {
	SubtractCarryA(DE.hi);
	clockCycles = 4;
}
// SBC A,E
void CPU::op0x9B () {
	SubtractCarryA(DE.lo);
	clockCycles = 4;
}
// SBC A,H
void CPU::op0x9C () {
	SubtractCarryA(HL.hi);
	clockCycles = 4;
}
// SBC A,L
void CPU::op0x9D () {
	SubtractCarryA(HL.lo);
	clockCycles = 4;
}
// SBC A,(HL)
// SBC A,A
void CPU::op0x9F () {
	SubtractCarryA(AF.hi);
	clockCycles = 4;
}

/* A. instructions */
// AND B
void CPU::op0xA0() {
	AF.hi &= BC.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 4;
}
// AND C
void CPU::op0xA1() {
	AF.hi &= BC.lo;
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 4;
}
// AND D
void CPU::op0xA2() {
	AF.hi &= DE.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 4;
}
// AND E
void CPU::op0xA3() {
	AF.hi &= DE.lo;
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 4;
}
// AND H
void CPU::op0xA4() {
	AF.hi &= HL.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 4;
}
// AND L
void CPU::op0xA5() {
	AF.hi &= HL.lo;
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 4;
}
// AND (HL)
void CPU::op0xA6() {
	AF.hi &= mmu->ReadByte(HL);
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 8;
}
// AND A
void CPU::op0xA7() {
	AF.hi &= AF.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);
	clockCycles = 4;
}
// XOR B
void CPU::op0xA8() {
	AF.hi ^= BC.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 4;
}
// XOR C
void CPU::op0xA9() {
	AF.hi ^= BC.lo;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 4;
}
// XOR D
void CPU::op0xAA() {
	AF.hi ^= DE.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 4;
}
// XOR E
void CPU::op0xAB() {
	AF.hi ^= DE.lo;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 4;
}
// XOR H
void CPU::op0xAC() {
	AF.hi ^= HL.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 4;
}
// XOR L
void CPU::op0xAD() {
	AF.hi ^= HL.lo;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 4;
}
// XOR (HL)
void CPU::op0xAE() {
	AF.hi ^= mmu->ReadByte(HL);
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 8;
}
// XOR A
void CPU::op0xAF() {
	AF.hi ^= AF.hi;
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);
	clockCycles = 4;
}

/* B. instructions */
// OR B
void CPU::op0xB0 () {
	OrA(BC.hi);
	clockCycles = 4;
}
// OR C
void CPU::op0xB1 () {
	OrA(BC.lo);
	clockCycles = 4;
}
// OR D
void CPU::op0xB2 () {
	OrA(DE.hi);
	clockCycles = 4;
}
// OR E
void CPU::op0xB3 () {
	OrA(DE.lo);
	clockCycles = 4;
}
// OR H
void CPU::op0xB4 () {
	OrA(HL.hi);
	clockCycles = 4;
}
// OR L
void CPU::op0xB5 () {
	OrA(HL.lo);
	clockCycles = 4;
}
// OR (HL)
void CPU::op0xB6 () {
	OrA(mmu->ReadByte(HL));
	clockCycles = 4;
}
// OR A
void CPU::op0xB7 () {
	OrA(AF.hi);
	clockCycles = 4;
}
// CP B
void CPU::op0xB8() {
	CompareA(BC.hi);
	clockCycles = 4;
}
// CP C
void CPU::op0xB9() {
	CompareA(BC.lo);
	clockCycles = 4;
}
// CP D
void CPU::op0xBA() {
	CompareA(DE.hi);
	clockCycles = 4;
}
// CP E
void CPU::op0xBB() {
	CompareA(DE.lo);
	clockCycles = 4;
}
// CP H
void CPU::op0xBC() {
	CompareA(HL.hi);
	clockCycles = 4;
}
// CP L
void CPU::op0xBD() {
	CompareA(HL.lo);
	clockCycles = 4;
}
// CP (HL)
void CPU::op0xBE() {
	CompareA(mmu->ReadByte(HL));
	clockCycles = 4;
}
// CP A
void CPU::op0xBF() {
	CompareA(AF.hi);
	clockCycles = 4;
}
/*  */
// RET NZ
void CPU::op0xC0 () {
	if (GetZ() == 0)
		PC = PopWord();
	clockCycles = 8;
}
// RET NC
void CPU::op0xD0 () {
	if (GetC() == 0)
		PC = PopWord();
	clockCycles = 8;
}
// LDH (a8),A
void CPU::op0xE0 () {
	mmu->WriteByte(0xFF00 + ReadByte(), AF.hi);
	clockCycles = 12;
}
// LDH A,(a8)
void CPU::op0xF0 () {
	AF.hi = mmu->ReadByte(0xFF00 + ReadByte());;
	clockCycles = 12;
}

// POP BC
void CPU::op0xC1 () {
	BC = PopWord();
	clockCycles = 12;
}
// POP DE
void CPU::op0xD1 () {
	DE = PopWord();
	clockCycles = 12;
}
// POP HL
void CPU::op0xE1 () {
	HL = PopWord();
	clockCycles = 12;
}
// POP AF
void CPU::op0xF1 () {
	AF = PopWord();
	clockCycles = 12;
}

// JP NZ,a16
void CPU::op0xC2 () {
	uint16_t value = ReadWord();
	if (GetZ() == 0)
		PC = value;
	clockCycles = 12;
}
// JP NC,a16
// LD (C),A
void CPU::op0xE2 () {
	mmu->WriteByte(BC.lo + 0xFF00, AF.hi);
	clockCycles = 8;
}
// LD A,(C)
void CPU::op0xF2 () {
	AF.hi = mmu->ReadByte(BC.lo + 0xFF00);
	clockCycles = 8;
}

// JP a16
void CPU::op0xC3 () {
	PC = ReadWord();
	// uint16_t lol = ((PC & 0xFF00) >> 8) | ((PC & 0x00FF) << 8);
	// std::cout << std::hex << lol << '\n';
	clockCycles = 12;
}
// D3..: I don't exist
// E3..: I don't exist
// DI
void CPU::op0xF3 () {
	areInterruptsEnabled = false;
	clockCycles = 4;
}

// CALL NZ,a16
void CPU::op0xC4 () {
	uint16_t address = ReadWord();
	if (GetZ() == false) {
		Call(address);
		clockCycles = 24;
	} else {
		clockCycles = 12;
	}
}
// CALL NC,a16
void CPU::op0xD4 () {
	uint16_t address = ReadWord();
	if (GetC() == false) {
		Call(address);
		clockCycles = 24;
	} else {
		clockCycles = 12;
	}
}
// E4..: I don't exist
// F4..: I don't exist

// PUSH BC
void CPU::op0xC5 () {
	PushWord(BC);
	clockCycles += 16;
}
// PUSH DE
void CPU::op0xD5 () {
	PushWord(DE);
	clockCycles += 16;
}
// PUSH HL
void CPU::op0xE5 () {
	PushWord(HL);
	clockCycles += 16;
}
// PUSH AF
void CPU::op0xF5 () {
	PushWord(AF);
	clockCycles += 16;
}

// ADD A,d8
void CPU::op0xC6 () {
	AddA(ReadByte());
	clockCycles += 8;
}

// SUB d8
void CPU::op0xD6 () {
	SubtractA(ReadByte());
	clockCycles += 8;
}
// AND d8
void CPU::op0xE6 () {
	AF.hi &= ReadByte();
	SetZ(AF.hi == 0);
	SetN(0), SetH(1), SetC(0);

	clockCycles += 8;
}
// OR d8
void CPU::op0xF6 () {
	OrA(ReadByte());
	clockCycles += 8;
}

// RST 00H
void CPU::op0xC7 () {
	PushWord(PC);
	PC = 0x00;
	clockCycles = 32;
}
// RST 10H
void CPU::op0xD7 () {
	PushWord(PC);
	PC = 0x10;
	clockCycles = 32;
}
// RST 20H
void CPU::op0xE7 () {
	PushWord(PC);
	PC = 0x20;
	clockCycles = 32;
}
// RST 30H
void CPU::op0xF7 () {
	PushWord(PC);
	PC = 0x30;
	clockCycles = 32;
}

// RET Z
void CPU::op0xC8 () {
	if (GetZ() != 0)
		PC = PopWord();
	clockCycles = 8;
}
// RET C
void CPU::op0xD8 () {
	if (GetC() != 0)
		PC = PopWord();
	clockCycles = 8;
}
// ADD SP,r8
// LD HL,SP+r8

// RET
void CPU::op0xC9 () {
	PC = PopWord();
	clockCycles = 8;
}
// RETI
void CPU::op0xD9 () {
	PC = PopWord();
	areInterruptsEnabled = true;
	clockCycles = 8;
}
// JP (HL)
void CPU::op0xE9 () {
	PC = HL;
	clockCycles = 4;
}
// LD SP,HL
void CPU::op0xF9 () {
	SP = HL;
	clockCycles = 8;
}

// JP Z,a16
void CPU::op0xCA () {
	uint16_t address = ReadWord();
	if (GetZ() != 0)
		PC = address;
	clockCycles = 16;
}
// JP C,a16
void CPU::op0xDA () {
	uint16_t address = ReadWord();
	if (GetC() != 0)
		PC = address;
	clockCycles = 16;
}
// LD (a16),A
void CPU::op0xEA () {
	mmu->WriteByte(ReadWord(), AF.hi);

	clockCycles = 16;
}
// LD A,(a16)
void CPU::op0xFA () {
	AF.hi = mmu->ReadByte(ReadWord());

	clockCycles = 16;
}

// PREFIX CB
void CPU::op0xCB () {
	uint8_t secondByte = ReadByte();
	(this->*opcodesCB[secondByte])();
	clockCycles = 8;
	/* In very few instructions, 16 cycles are needed instead,
	which are added when the instruction is called.
	e.g. 0xCB06, 0xCB16, 0xCB26...
	*/
}
// DB..: I don't exist
// EB..: I don't exist
// EI
void CPU::op0xFB () {
	areInterruptsEnabled = true;
	clockCycles = 4;
}
// CALL Z,a16
void CPU::op0xCC () {
	uint16_t address = ReadWord();
	if (GetZ() == true) {
		Call(address);
		clockCycles = 24;
	} else {
		clockCycles = 12;
	}
}
// CALL C,a16
void CPU::op0xDC () {
	uint16_t address = ReadWord();
	if (GetC() == true) {
		Call(address);
		clockCycles = 24;
	} else {
		clockCycles = 12;
	}
}
// EC..: I don't exist
// FC..: I don't exist
// CALL a16
void CPU::op0xCD () {
	uint16_t address = ReadWord();
	Call(address);
	clockCycles = 24;
}
// DD..: I don't exist
// ED..: I don't exist
// FD..: I don't exist

// ADC A,d8
void CPU::op0xCE () {
	AddCarryA(ReadByte());
	clockCycles = 8;
}
// SBC A,d8
void CPU::op0xDE () {
	SubtractCarryA(ReadByte());
	clockCycles = 8;
}
// XOR d8
void CPU::op0xEE () {
	AF.hi ^= ReadByte();
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);

	clockCycles = 8;
}
// CP d8
void CPU::op0xFE () {
	CompareA(ReadByte());

	clockCycles = 8;
}

// RST 08H
void CPU::op0xCF () {
	PushWord(PC);
	PC = 0x08;
	clockCycles = 32;
}
// RST 18H
void CPU::op0xDF () {
	PushWord(PC);
	PC = 0x18;
	clockCycles = 32;
}
// RST 28H
void CPU::op0xEF () {
	PushWord(PC);
	PC = 0x28;
	clockCycles = 32;
}
// RST 38H
void CPU::op0xFF () {
	PushWord(PC);
	PC = 0x38;
	clockCycles = 32;
}
// CB0. instructions
// RLC B
void CPU::cb0x00 () {
	RotateLeftCarry(BC.hi);
}
// RLC C
void CPU::cb0x01 () {
	RotateLeftCarry(BC.lo);
}
// RLC D
void CPU::cb0x02 () {
	RotateLeftCarry(DE.hi);
}
// RLC E
void CPU::cb0x03 () {
	RotateLeftCarry(DE.lo);
}
// RLC H
void CPU::cb0x04 () {
	RotateLeftCarry(HL.hi);
}
// RLC L
void CPU::cb0x05 () {
	RotateLeftCarry(HL.lo);
}
// RLC (HL)
// RLC A
void CPU::cb0x07 () {
	RotateLeftCarry(AF.hi);
}
// RRC B
void CPU::cb0x08 () {
	RotateRightCarry(BC.hi);
}
// RRC C
void CPU::cb0x09 () {
	RotateRightCarry(BC.lo);
}
// RRC D
void CPU::cb0x0A () {
	RotateRightCarry(DE.hi);
}
// RRC E
void CPU::cb0x0B () {
	RotateRightCarry(DE.lo);
}
// RRC H
void CPU::cb0x0C () {
	RotateRightCarry(HL.hi);
}
// RRC L
void CPU::cb0x0D () {
	RotateRightCarry(HL.lo);
}
// RRC (HL)
// RRC A
void CPU::cb0x0F () {
	RotateRightCarry(AF.hi);
}
// CB1. instructions
// RL B
void CPU::cb0x10() {
	RotateLeft(BC.hi);
}
// RL C
void CPU::cb0x11() {
	RotateLeft(BC.lo);
}
// RL D
void CPU::cb0x12() {
	RotateLeft(DE.hi);
}
// RL E
void CPU::cb0x13() {
	RotateLeft(DE.lo);
}
// RL H
void CPU::cb0x14() {
	RotateLeft(HL.hi);
}
// RL L
void CPU::cb0x15() {
	RotateLeft(HL.lo);
}
// RL (HL)
void CPU::cb0x16() {
	uint8_t value = mmu->ReadByte(HL);
	RotateLeft(value);
	mmu->WriteByte(HL, value);
}
// RL A
void CPU::cb0x17 () {
	RotateLeft(AF.hi);
}
// RR B
void CPU::cb0x18 () {
	RotateRight(BC.hi);
}
// RR C
void CPU::cb0x19 () {
	RotateRight(BC.lo);
}
// RR D
void CPU::cb0x1A () {
	RotateRight(DE.hi);
}
// RR E
void CPU::cb0x1B () {
	RotateRight(DE.lo);
}
// RR H
void CPU::cb0x1C () {
	RotateRight(HL.hi);
}
// RR L
void CPU::cb0x1D () {
	RotateRight(HL.lo);
}
// RR (HL)
// RR A
void CPU::cb0x1F () {
	RotateRight(AF.hi);
}
// CB2. instructions
// SLA B
void CPU::cb0x20 () {
	ShiftLeft(BC.hi);
}
// SLA C
void CPU::cb0x21 () {
	ShiftLeft(BC.lo);
}
// SLA D
void CPU::cb0x22 () {
	ShiftLeft(DE.hi);
}
// SLA E
void CPU::cb0x23 () {
	ShiftLeft(DE.lo);
}
// SLA H
void CPU::cb0x24 () {
	ShiftLeft(HL.hi);
}
// SLA L
void CPU::cb0x25 () {
	ShiftLeft(HL.lo);
}
// SLA (HL)
// SLA A
void CPU::cb0x27 () {
	ShiftLeft(AF.hi);
}
// SRA B
void CPU::cb0x28 () {
	ShiftLeft(BC.hi);
}
// SRA C
void CPU::cb0x29 () {
	ShiftLeft(BC.lo);
}
// SRA D
void CPU::cb0x2A () {
	ShiftLeft(DE.hi);
}
// SRA E
void CPU::cb0x2B () {
	ShiftLeft(DE.lo);
}
// SRA H
void CPU::cb0x2C () {
	ShiftLeft(HL.hi);
}
// SRA L
void CPU::cb0x2D () {
	ShiftLeft(HL.lo);
}
// SRA (HL)
// SRA A
void CPU::cb0x2F () {
	ShiftLeft(AF.hi);
}
// CB3. instructions
// SWAP B
void CPU::cb0x30 () {
	Swap(BC.hi);
}
// SWAP C
void CPU::cb0x31 () {
	Swap(BC.lo);
}
// SWAP D
void CPU::cb0x32 () {
	Swap(DE.hi);
}
// SWAP E
void CPU::cb0x33 () {
	Swap(DE.lo);
}
// SWAP H
void CPU::cb0x34 () {
	Swap(HL.hi);
}
// SWAP L
void CPU::cb0x35 () {
	Swap(HL.lo);
}
// SWAP (HL)
// SWAP A
void CPU::cb0x37 () {
	Swap(AF.hi);
}
// SRL B
void CPU::cb0x38 () {
	ShiftRight(BC.hi);
}
// SRL C
void CPU::cb0x39 () {
	ShiftRight(BC.lo);
}
// SRL D
void CPU::cb0x3A () {
	ShiftRight(DE.hi);
}
// SRL E
void CPU::cb0x3B () {
	ShiftRight(DE.lo);
}
// SRL H
void CPU::cb0x3C () {
	ShiftRight(HL.hi);
}
// SRL L
void CPU::cb0x3D () {
	ShiftRight(HL.lo);
}
// SRL (HL)
// SRL A
void CPU::cb0x3F () {
	ShiftRight(AF.hi);
}
// CB4.
// BIT 0,B
void CPU::cb0x40 () {
	Bit(0, BC.hi);
}
// BIT 0,C
void CPU::cb0x41 () {
	Bit(0, BC.lo);
}
// BIT 0,D
void CPU::cb0x42 () {
	Bit(0, DE.hi);
}
// BIT 0,E
void CPU::cb0x43 () {
	Bit(0, DE.lo);
}
// BIT 0,H
void CPU::cb0x44 () {
	Bit(0, HL.hi);
}
// BIT 0,L
void CPU::cb0x45 () {
	Bit(0, HL.lo);
}
// BIT 0,(HL)
void CPU::cb0x46 () {
	Bit(0, mmu->ReadByte(HL));
	clockCycles = 16;
}
// BIT 0,A
void CPU::cb0x47 () {
	Bit(0, AF.hi);
}
// BIT 1,B
void CPU::cb0x48 () {
	Bit(1, BC.hi);
}
// BIT 1,C
// BIT 1,D
// BIT 1,E
// BIT 1,H
// BIT 1,L
// BIT 1,(HL)
// BIT 1,A
void CPU::cb0x4F () {
	Bit(1, AF.hi);
}
// CB5. instructions
// BIT 2,B
void CPU::cb0x50 () {
	Bit(2, BC.hi);
}
// BIT 2,C
void CPU::cb0x51 () {
	Bit(2, BC.lo);
}
// BIT 2,D
void CPU::cb0x52 () {
	Bit(2, DE.hi);
}
// BIT 2,E
void CPU::cb0x53 () {
	Bit(2, DE.lo);
}
// BIT 2,H
void CPU::cb0x54 () {
	Bit(2, HL.hi);
}
// BIT 2,L
void CPU::cb0x55 () {
	Bit(2, HL.lo);
}
// BIT 2,(HL)
void CPU::cb0x56 () {
	Bit(2, mmu->ReadByte(HL));
}
// BIT 2,A
void CPU::cb0x57 () {
	Bit(2, AF.hi);
}
// BIT 3,B
void CPU::cb0x58 () {
	Bit(3, BC.hi);
}
// BIT 3,C
void CPU::cb0x59 () {
	Bit(3, BC.lo);
}
// BIT 3,D
void CPU::cb0x5A () {
	Bit(3, DE.hi);
}
// BIT 3,E
void CPU::cb0x5B () {
	Bit(3, DE.lo);
}
// BIT 3,H
void CPU::cb0x5C () {
	Bit(3, HL.hi);
}
// BIT 3,L
void CPU::cb0x5D () {
	Bit(3, HL.lo);
}
// BIT 3,(HL)
void CPU::cb0x5E () {
	Bit(3, mmu->ReadByte(HL));
}
// BIT 3,A
void CPU::cb0x5F () {
	Bit(3, AF.hi);
}
// CB6. instructions
// BIT 4,B
void CPU::cb0x60 () {
	Bit(4, BC.hi);
}
// BIT 4,C
void CPU::cb0x61 () {
	Bit(4, BC.lo);
}
// BIT 4,D
void CPU::cb0x62 () {
	Bit(4, DE.hi);
}
// BIT 4,E
void CPU::cb0x63 () {
	Bit(4, DE.lo);
}
// BIT 4,H
void CPU::cb0x64 () {
	Bit(4, HL.hi);
}
// BIT 4,L
void CPU::cb0x65 () {
	Bit(4, HL.lo);
}
// BIT 4,(HL)
void CPU::cb0x66 () {
	Bit(4, mmu->ReadByte(HL));
}
// BIT 4,A
void CPU::cb0x67 () {
	Bit(4, AF.hi);
}
// BIT 5,B
void CPU::cb0x68 () {
	Bit(5, BC.hi);
}
// BIT 5,C
void CPU::cb0x69 () {
	Bit(5, BC.lo);
}
// BIT 5,D
void CPU::cb0x6A () {
	Bit(5, DE.hi);
}
// BIT 5,E
void CPU::cb0x6B () {
	Bit(5, DE.lo);
}
// BIT 5,H
void CPU::cb0x6C () {
	Bit(5, HL.hi);
}
// BIT 5,L
void CPU::cb0x6D () {
	Bit(5, HL.lo);
}
// BIT 5,(HL)
void CPU::cb0x6E () {
	Bit(5, mmu->ReadByte(HL));
}
// BIT 5,A
void CPU::cb0x6F () {
	Bit(5, AF.hi);
}
// CB7. instructions
// BIT 6,B
void CPU::cb0x70 () {
	Bit(6, BC.hi);
}
// BIT 6,C
void CPU::cb0x71 () {
	Bit(6, BC.lo);
}
// BIT 6,D
void CPU::cb0x72 () {
	Bit(6, DE.hi);
}
// BIT 6,E
void CPU::cb0x73 () {
	Bit(6, DE.lo);
}
// BIT 6,H
void CPU::cb0x74 () {
	Bit(6, HL.hi);
}
// BIT 6,L
void CPU::cb0x75 () {
	Bit(6, HL.lo);
}
// BIT 6,(HL)
void CPU::cb0x76 () {
	Bit(6, mmu->ReadByte(HL));
}
// BIT 6,A
void CPU::cb0x77 () {
	Bit(6, AF.hi);
}
// BIT 7,B
void CPU::cb0x78 () {
	Bit(7, BC.hi);
}
// BIT 7,C
void CPU::cb0x79 () {
	Bit(7, BC.lo);
}
// BIT 7,D
void CPU::cb0x7A () {
	Bit(7, DE.hi);
}
// BIT 7,E
void CPU::cb0x7B () {
	Bit(7, DE.lo);
}
// BIT 7,H
void CPU::cb0x7C () {
	Bit(7, HL.hi);
}
// BIT 7,L
void CPU::cb0x7D () {
	Bit(7, HL.lo);
}
// BIT 7,(HL)
void CPU::cb0x7E () {
	Bit(7, mmu->ReadByte(HL));
	clockCycles = 16;
}
// BIT 7,A
void CPU::cb0x7F () {
	Bit(7, AF.hi);
}
// CB8. instructions
// RES 0,B
void CPU::cb0x80 () {
	ResetBit(BC.hi, 0);
}
// RES 0,C
void CPU::cb0x81 () {
	ResetBit(BC.lo, 0);
}
// RES 0,D
void CPU::cb0x82 () {
	ResetBit(DE.hi, 0);
}
// RES 0,E
void CPU::cb0x83 () {
	ResetBit(DE.lo, 0);
}
// RES 0,H
void CPU::cb0x84 () {
	ResetBit(HL.hi, 0);
}
// RES 0,L
void CPU::cb0x85 () {
	ResetBit(HL.lo, 0);
}
// RES 0,(HL)
void CPU::cb0x86 () {
	uint8_t target = mmu->ReadByte(HL);
	ResetBit(target, 0);
	mmu->WriteByte(HL, target);
	clockCycles = 16;
}
// RES 0,A
void CPU::cb0x87 () {
	ResetBit(AF.hi, 0);
}
// RES 1,B
void CPU::cb0x88 () {
	ResetBit(BC.hi, 1);
}
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
void CPU::cb0x9E () {
	uint8_t target = mmu->ReadByte(HL);
	ResetBit(target, 3);
	mmu->WriteByte(HL, target);
	clockCycles = 16;
}
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
void CPU::cb0xBE () {
	uint8_t target = mmu->ReadByte(HL);
	ResetBit(target, 7);
	mmu->WriteByte(HL, target);
	clockCycles = 16;
}
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
void CPU::cb0xD0 () {
	SetBit(BC.hi, 2);
}
// SET 2,C
// SET 2,D
// SET 2,E
// SET 2,H
// SET 2,L
// SET 2,(HL)
// SET 2,A
// SET 3,B
void CPU::cb0xD8 () {
	SetBit(BC.hi, 3);
}
// SET 3,C
// SET 3,D
// SET 3,E
// SET 3,H
// SET 3,L
// SET 3,(HL)
void CPU::cb0xDE () {
	uint8_t target = mmu->ReadByte(HL);
	SetBit(target, 3);
	mmu->WriteByte(HL, target);
	clockCycles = 16;
}
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
void CPU::cb0xF0 () {
	SetBit(BC.hi, 6);
}
// SET 6,C
// SET 6,D
// SET 6,E
// SET 6,H
// SET 6,L
// SET 6,(HL)
// SET 6,A
// SET 7,B
void CPU::cb0xF8 () {
	SetBit(BC.hi, 7);
}
// SET 7,C
// SET 7,D
// SET 7,E
// SET 7,H
// SET 7,L
// SET 7,(HL)
void CPU::cb0xFE () {
	uint8_t target = mmu->ReadByte(HL);
	SetBit(target, 7);
	mmu->WriteByte(HL, target);
	clockCycles = 16;
}
// SET 7,A
void CPU::cb0xFF () {
	SetBit(AF.hi, 7);
}

/* Not implemented instructions call this function */
void CPU::opNull () {
	isHalted = true;
	std::cout << "Halting! Instruction not implemented\n";
	// assert("Not implemented" && 0);
}
