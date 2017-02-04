#include "cpu.hpp"
#include "mmu.hpp"

#include "iostream"
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

	if (PC == 0x286) {
		isHalted = true;
	}

	uint8_t opcode = mmu->ReadByte(PC);

	std::cout << std::hex << PC << ' ' << DisassembleOpcode(mmu->GetMemoryRef(PC)) << '\n';
	// std::cout << std::hex << opcode << '\n';
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

	uint8_t requestRegister = mmu->ReadByte(IF);
	if (requestRegister == 0)
		return;


	uint8_t enabledRegister = mmu->ReadByte(IE);
	for (size_t id = 0; id < 5; id++) {
		bool isInterruptRequested = requestRegister & (0x10 >> (4 - id));
		bool isInterruptEnabled   = enabledRegister & (0x10 >> (4 - id));

		if (isInterruptRequested && isInterruptEnabled) {
			DoInterrupt(id);
			std::cout << "Doing interrupt " << id << "\n";
			isHalted = true;
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

bool CPU::GetZ () {
	return (AF.lo & 0x80);
}

bool CPU::GetN () {
	return (AF.lo & 0x40);
}

bool CPU::GetH () {
	return (AF.lo & 0x20);
}

bool CPU::GetC () {
	return (AF.lo & 0x10);
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
// FIXME: Double check C flag computation
void CPU::RotateLeft (uint8_t& value) {
	uint8_t oldBit7 = (value >> 7);
	value = (value << 1) | GetC();
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
	uint8_t oldBit4 = value & 0x8;
	value -= 1;
	SetZ(value == 0);
	SetN(1);
	SetH( (value & 0x8) ^ oldBit4 && oldBit4 != 0x8 );
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

void CPU::AddA (uint8_t value) {
	uint8_t oldA = AF.hi;
	AF.hi += value;
	SetZ(AF.hi == 0);
	SetN(0);
	SetH((oldA & 0xF) + (value & 0xF) > 0xF);
	SetC(oldA + value > 0xFF);
}

void CPU::Add (uint16_t& x, uint16_t value) {
	uint8_t oldX = x;
	x += value;
	SetZ(x == 0);
	SetN(0);
	SetH((oldX & 0xF) + (value & 0xF) > 0xF);
	SetC(oldX + value > 0xFF);
}

void CPU::SubtractA (uint8_t value) {
	uint8_t oldA = AF.hi;
	AF.hi -= value;
	SetZ(AF.hi == 0);
	SetN(1);
	SetH((oldA & 0xF) < (value & 0xF));
	SetC(oldA < value);
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
	opcodes[0x1E] = &CPU::op0x1E; opcodes[0x1F] = &CPU::opNull;

	opcodes[0x20] = &CPU::op0x20; opcodes[0x21] = &CPU::op0x21;
	opcodes[0x22] = &CPU::op0x22; opcodes[0x23] = &CPU::op0x23;
	opcodes[0x24] = &CPU::op0x24; opcodes[0x25] = &CPU::op0x25;
	opcodes[0x26] = &CPU::op0x26; opcodes[0x27] = &CPU::opNull;
	opcodes[0x28] = &CPU::op0x28; opcodes[0x29] = &CPU::op0x29;
	opcodes[0x2A] = &CPU::op0x2A; opcodes[0x2B] = &CPU::op0x2B;
	opcodes[0x2C] = &CPU::op0x2C; opcodes[0x2D] = &CPU::op0x2D;
	opcodes[0x2E] = &CPU::op0x2E; opcodes[0x2F] = &CPU::op0x2F;

	opcodes[0x30] = &CPU::op0x30; opcodes[0x31] = &CPU::op0x31;
	opcodes[0x32] = &CPU::op0x32; opcodes[0x33] = &CPU::op0x33;
	opcodes[0x34] = &CPU::op0x34; opcodes[0x35] = &CPU::op0x35;
	opcodes[0x36] = &CPU::op0x36; opcodes[0x37] = &CPU::opNull;
	opcodes[0x38] = &CPU::op0x38; opcodes[0x39] = &CPU::op0x39;
	opcodes[0x3A] = &CPU::op0x3A; opcodes[0x3B] = &CPU::op0x3B;
	opcodes[0x3C] = &CPU::op0x3C; opcodes[0x3D] = &CPU::op0x3D;
	opcodes[0x3E] = &CPU::op0x3E; opcodes[0x3F] = &CPU::opNull;

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
	opcodes[0x76] = &CPU::opNull; opcodes[0x77] = &CPU::op0x77;
	opcodes[0x78] = &CPU::op0x78; opcodes[0x79] = &CPU::op0x79;
	opcodes[0x7A] = &CPU::op0x7A; opcodes[0x7B] = &CPU::op0x7B;
	opcodes[0x7C] = &CPU::op0x7C; opcodes[0x7D] = &CPU::op0x7D;
	opcodes[0x7E] = &CPU::op0x7E; opcodes[0x7F] = &CPU::op0x7F;

	opcodes[0x80] = &CPU::op0x80; opcodes[0x81] = &CPU::op0x81;
	opcodes[0x82] = &CPU::op0x82; opcodes[0x83] = &CPU::op0x83;
	opcodes[0x84] = &CPU::op0x84; opcodes[0x85] = &CPU::op0x85;
	opcodes[0x86] = &CPU::op0x86; opcodes[0x87] = &CPU::op0x87;
	opcodes[0x88] = &CPU::opNull; opcodes[0x89] = &CPU::opNull;
	opcodes[0x8A] = &CPU::opNull; opcodes[0x8B] = &CPU::opNull;
	opcodes[0x8C] = &CPU::opNull; opcodes[0x8D] = &CPU::opNull;
	opcodes[0x8E] = &CPU::opNull; opcodes[0x8F] = &CPU::opNull;

	opcodes[0x90] = &CPU::op0x90; opcodes[0x91] = &CPU::op0x91;
	opcodes[0x92] = &CPU::op0x92; opcodes[0x93] = &CPU::op0x93;
	opcodes[0x94] = &CPU::op0x94; opcodes[0x95] = &CPU::op0x95;
	opcodes[0x96] = &CPU::op0x96; opcodes[0x97] = &CPU::op0x97;
	opcodes[0x98] = &CPU::opNull; opcodes[0x99] = &CPU::opNull;
	opcodes[0x9A] = &CPU::opNull; opcodes[0x9B] = &CPU::opNull;
	opcodes[0x9C] = &CPU::opNull; opcodes[0x9D] = &CPU::opNull;
	opcodes[0x9E] = &CPU::opNull; opcodes[0x9F] = &CPU::opNull;

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
	opcodes[0xC4] = &CPU::opNull; opcodes[0xC5] = &CPU::op0xC5;
	opcodes[0xC6] = &CPU::opNull; opcodes[0xC7] = &CPU::op0xC7;
	opcodes[0xC8] = &CPU::op0xC8; opcodes[0xC9] = &CPU::op0xC9;
	opcodes[0xCA] = &CPU::op0xCA; opcodes[0xCB] = &CPU::op0xCB;
	opcodes[0xCC] = &CPU::opNull; opcodes[0xCD] = &CPU::op0xCD;
	opcodes[0xCE] = &CPU::opNull; opcodes[0xCF] = &CPU::op0xCF;

	opcodes[0xD0] = &CPU::opNull; opcodes[0xD1] = &CPU::op0xD1;
	opcodes[0xD2] = &CPU::opNull; opcodes[0xD3] = &CPU::opNull;
	opcodes[0xD4] = &CPU::opNull; opcodes[0xD5] = &CPU::op0xD5;
	opcodes[0xD6] = &CPU::op0xD6; opcodes[0xD7] = &CPU::op0xD7;
	opcodes[0xD8] = &CPU::opNull; opcodes[0xD9] = &CPU::op0xD9;
	opcodes[0xDA] = &CPU::op0xDA; opcodes[0xDB] = &CPU::opNull;
	opcodes[0xDC] = &CPU::opNull; opcodes[0xDD] = &CPU::opNull;
	opcodes[0xDE] = &CPU::opNull; opcodes[0xDF] = &CPU::op0xDF;

	opcodes[0xE0] = &CPU::op0xE0; opcodes[0xE1] = &CPU::op0xE1;
	opcodes[0xE2] = &CPU::op0xE2; opcodes[0xE3] = &CPU::opNull;
	opcodes[0xE4] = &CPU::opNull; opcodes[0xE5] = &CPU::op0xE5;
	opcodes[0xE6] = &CPU::op0xE6; opcodes[0xE7] = &CPU::op0xE7;
	opcodes[0xE8] = &CPU::opNull; opcodes[0xE9] = &CPU::op0xE9;
	opcodes[0xEA] = &CPU::op0xEA; opcodes[0xEB] = &CPU::opNull;
	opcodes[0xEC] = &CPU::opNull; opcodes[0xED] = &CPU::opNull;
	opcodes[0xEE] = &CPU::opNull; opcodes[0xEF] = &CPU::op0xEF;

	opcodes[0xF0] = &CPU::op0xF0; opcodes[0xF1] = &CPU::op0xF1;
	opcodes[0xF2] = &CPU::op0xF2; opcodes[0xF3] = &CPU::op0xF3;
	opcodes[0xF4] = &CPU::opNull; opcodes[0xF5] = &CPU::op0xF5;
	opcodes[0xF6] = &CPU::opNull; opcodes[0xF7] = &CPU::op0xF7;
	opcodes[0xF8] = &CPU::opNull; opcodes[0xF9] = &CPU::op0xF9;
	opcodes[0xFA] = &CPU::op0xFA; opcodes[0xFB] = &CPU::op0xFB;
	opcodes[0xFC] = &CPU::opNull; opcodes[0xFD] = &CPU::opNull;
	opcodes[0xFE] = &CPU::op0xFE; opcodes[0xFF] = &CPU::op0xFF;

	/* -------------------------------------- */
	/* Initialize CB prefixed functions table */
	/* -------------------------------------- */
	opcodesCB[0x10] = &CPU::cb0x10; opcodesCB[0x11] = &CPU::cb0x11;
	opcodesCB[0x12] = &CPU::cb0x12; opcodesCB[0x13] = &CPU::cb0x13;
	opcodesCB[0x14] = &CPU::cb0x14; opcodesCB[0x15] = &CPU::cb0x15;
	opcodesCB[0x16] = &CPU::cb0x16; opcodesCB[0x17] = &CPU::opNull;
	opcodesCB[0x18] = &CPU::opNull; opcodesCB[0x19] = &CPU::opNull;
	opcodesCB[0x1A] = &CPU::opNull; opcodesCB[0x1B] = &CPU::opNull;
	opcodesCB[0x1C] = &CPU::opNull; opcodesCB[0x1D] = &CPU::opNull;
	opcodesCB[0x1E] = &CPU::opNull; opcodesCB[0x1F] = &CPU::opNull;

	opcodesCB[0x30] = &CPU::opNull; opcodesCB[0x31] = &CPU::opNull;
	opcodesCB[0x32] = &CPU::opNull; opcodesCB[0x33] = &CPU::opNull;
	opcodesCB[0x34] = &CPU::opNull; opcodesCB[0x35] = &CPU::opNull;
	opcodesCB[0x36] = &CPU::opNull; opcodesCB[0x37] = &CPU::cb0x37;
	opcodesCB[0x38] = &CPU::opNull; opcodesCB[0x39] = &CPU::opNull;
	opcodesCB[0x3A] = &CPU::opNull; opcodesCB[0x3B] = &CPU::opNull;
	opcodesCB[0x3C] = &CPU::opNull; opcodesCB[0x3D] = &CPU::opNull;
	opcodesCB[0x3E] = &CPU::opNull; opcodesCB[0x3F] = &CPU::opNull;

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
		PC += value;
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
	opNull();
	clockCycles = 4;
}
// RLA
// FIXME: Maybe RLA isn't the same as RL A, check Flags on pastraiser
void CPU::op0x17 () {
	RotateLeft(AF.hi);
	clockCycles = 4;
}
// DAA
// SCF

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
	opNull();
	clockCycles = 4;
}
// RRA
// CPL
void CPU::op0x2F () {
	AF.hi ^= 0xFF;
	SetN(1), SetH(1);
	clockCycles = 4;
}
// CCF

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
void CPU::op0x49 () { // Copying C to C? Is this Right?
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
	DE.hi = HL.hi;
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
	AF.lo = HL.lo;
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

	clockCycles = 4;
}
// LD A,A
void CPU::op0x7F () {
	AF.hi = AF.hi;

	clockCycles = 4;
}

/* 8. instructions */
// ADD A,B
void CPU::op0x80() {
	AddA(BC.hi);

	clockCycles = 4;
}
// ADD A,C
void CPU::op0x81() {
	AddA(BC.lo);

	clockCycles = 4;
}
// ADD A,D
void CPU::op0x82() {
	AddA(DE.hi);

	clockCycles = 4;
}
// ADD A,E
void CPU::op0x83() {
	AddA(DE.lo);

	clockCycles = 4;
}
// ADD A,H
void CPU::op0x84() {
	AddA(HL.hi);

	clockCycles = 4;
}
// ADD A,L
void CPU::op0x85() {
	AddA(HL.lo);

	clockCycles = 4;
}
// ADD A,(HL)
void CPU::op0x86() {
	AddA(mmu->ReadByte(HL));

	clockCycles = 8;
}
// ADD A,A
void CPU::op0x87() {
	AddA(AF.hi);

	clockCycles = 4;
}
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
void CPU::op0x90() {
	SubtractA(BC.hi);

	clockCycles = 4;
}
// SUB C
void CPU::op0x91() {
	SubtractA(BC.lo);

	clockCycles = 4;
}
// SUB D
void CPU::op0x92() {
	SubtractA(DE.hi);

	clockCycles = 4;
}
// SUB E
void CPU::op0x93() {
	SubtractA(DE.lo);

	clockCycles = 4;
}
// SUB H
void CPU::op0x94() {
	SubtractA(HL.hi);

	clockCycles = 4;
}
// SUB L
void CPU::op0x95() {
	SubtractA(HL.lo);

	clockCycles = 4;
}
// SUB (HL)
void CPU::op0x96() {
	SubtractA(mmu->ReadByte(HL));

	clockCycles = 8;
}
// SUB A
void CPU::op0x97() {
	SubtractA(AF.hi);

	clockCycles = 4;
}
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
// LDH (a8),A
void CPU::op0xE0 () {
	mmu->WriteByte(ReadByte() + 0xFF00, AF.hi);
	clockCycles = 12;
}
// LDH A,(a8)
void CPU::op0xF0 () {
	AF.hi = mmu->ReadByte(ReadByte() + 0xFF00);
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
// CALL NC,a16
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
// PUSH AF // FIXME: There is NO value in F because of our flags hack
void CPU::op0xF5 () {
	PushWord(AF);
	clockCycles += 16;
}

// ADD A,d8
// SUB d8
void CPU::op0xD6 () {
	SubtractA(ReadByte());
	clockCycles += 8;
}
// AND d8
void CPU::op0xE6 () {
	AF.hi &= ReadByte();
	SetZ(AF.hi == 0);
	SetN(0), SetH(0), SetC(0);

	clockCycles += 8;
}
// OR d8

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
// CALL C,a16
// EC..: I don't exist
// FC..: I don't exist
// CALL a16
void CPU::op0xCD () {
	PushWord(PC + 2);
	PC = ReadWord();
	clockCycles = 24;
}
// DD..: I don't exist
// ED..: I don't exist
// FD..: I don't exist

// ADC A,d8
// SBC A,d8
// XOR d8
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
void CPU::cb0x37 () {
	Swap(AF.hi);
}
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
	SetZ((DE.hi & 0x80) == 0);
	SetN(0), SetH(1);
}
// BIT 7,E
void CPU::cb0x7B () {
	SetZ((DE.lo & 0x80) == 0);
	SetN(0), SetH(1);
}
// BIT 7,H
void CPU::cb0x7C () {
	SetZ((HL.hi & 0x80) == 0);
	SetN(0), SetH(1);
}
// BIT 7,L
void CPU::cb0x7D () {
	SetZ((HL.lo & 0x80) == 0);
	SetN(0), SetH(1);
}
// BIT 7,(HL)
void CPU::cb0x7E () {
	SetZ((HL.lo & 0x80) == 0);
	SetN(0), SetH(1);
	clockCycles = 16;
}
// BIT 7,A
void CPU::cb0x7F () {
	SetZ((AF.hi & 0x80) == 0);
	SetN(0), SetH(1);
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
	isHalted = true;
	std::cout << "Instruction not implemented\n";
	// assert("Not implemented" && 0);
}
