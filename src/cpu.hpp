#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include "mmu.hpp"
#include "rom.hpp"

/* This structure allows an intuitive way to access lo/hi bytes from a 16bit */
/* Example
	reg16_t HL = 0xFFAA;
	uint8_t H = HL.hi;
	uint8_t L = HL.lo;
*/

union reg16_t {
	struct {
		uint8_t lo;
		uint8_t hi;
	};
	uint16_t word;

	reg16_t& operator=(const reg16_t& rhs) {
		word = rhs.word;
		return *this;
	}

	reg16_t& operator=(const uint16_t& rhs) {
		word = rhs;
		return *this;
	}

	reg16_t& operator-=(const uint16_t& rhs) {
		word -= rhs;
		return *this;
	}

	reg16_t& operator+=(const uint16_t& rhs) {
		word += rhs;
		return *this;
	}

	operator uint16_t() const { return word; }
};

class CPU {
friend class Debug;
private:
	MMU* mmu;

public:
	/* Registers */
	reg16_t AF; // F is unused
	reg16_t BC;
	reg16_t DE;
	reg16_t HL;

	uint16_t SP = 0xfffe;
	uint16_t PC;

	bool areInterruptsEnabled = false;
	bool isHalted = false;

	uint8_t clockCycles;

	CPU ();
	~CPU ();

	void Initialize (MMU* _mmu, bool doBootrom);
	void EmulateCycle ();
	void ExecuteInstruction (uint8_t opcode);

	/* Interrupt related functions */
	void RequestInterrupt (uint8_t id);
	void ProcessInterrupts ();
	void DoInterrupt (uint8_t id);

	/* Flag helpers */
	bool GetZ () const;
	bool GetN () const;
	bool GetH () const;
	bool GetC () const;
private:
	/* Flag helpers */
	void SetZ (bool value);
	void SetN (bool value);
	void SetH (bool value);
	void SetC (bool value);

	/* Instruction helpers */
	uint8_t  ReadByte ();
	uint16_t ReadWord ();

	void     PushWord (uint16_t value);
	uint16_t PopWord ();

	void EnableInterrupts ();

	/* Common Instructions */
	void Call (uint16_t address); // CALL
	void RotateLeft       (uint8_t& value); // RL
	void RotateLeftCarry  (uint8_t& value); // RLC
	void RotateRight      (uint8_t& value); // RR
	void RotateRightA     ();               // RRA
	void RotateRightCarry (uint8_t& value); // RRC
	void ShiftLeft   (uint8_t& value); // SLA
	void ShiftRight  (uint8_t& value); //
	void ShiftRightA (uint8_t& value); // SRA
	void ShiftRightL (uint8_t& value); // SRL
	void Decrement (uint8_t&  value); // DEC
	void Decrement (uint16_t& value); // DEC
	void Decrement (reg16_t& value); // DEC
	void Increment (uint8_t&  value); // INC
	void Increment (uint16_t& value); // INC
	void Increment (reg16_t& value); // INC
	void AddCarryA (uint8_t value); // ADC
	void AddA      (uint8_t value); // ADD
	void Add       (uint8_t& target, uint8_t value); // ADD
	void Add       (uint16_t& target, uint16_t value); // ADD
	void SubtractCarryA (uint8_t value); // SBC
	void SubtractA      (uint8_t value); // SUB A
	void Subtract       (uint8_t& target, uint8_t value); // SUB
	void CompareA  (uint8_t value); // CP
	void OrA       (uint8_t value); // OR
	void Swap (uint8_t& value); // SWAP

	/* Common CB Instructions */
	void SetBit   (uint8_t& target, uint8_t bit);
	void ResetBit (uint8_t& target, uint8_t bit);
	void SetBitOnAddress   (uint16_t address, uint8_t bit);
	void ResetBitOnAddress (uint16_t address, uint8_t bit);
	void Bit(uint8_t bit, uint8_t value);

	/* Instruction tables */
	void InitializeOpcodeTable ();
	void (CPU::*opcodes  [256]) ();
	void (CPU::*opcodesCB[256]) ();

	/* Instructions */
	void op0x00 (); void op0x01 (); void op0x02 (); void op0x03 ();
	void op0x04 (); void op0x05 (); void op0x06 (); void op0x07 ();
	void op0x08 (); void op0x09 (); void op0x0A (); void op0x0B ();
	void op0x0C (); void op0x0D (); void op0x0E (); void op0x0F ();

	void op0x10 (); void op0x11 (); void op0x12 (); void op0x13 ();
	void op0x14 (); void op0x15 (); void op0x16 (); void op0x17 ();
	void op0x18 (); void op0x19 (); void op0x1A (); void op0x1B ();
	void op0x1C (); void op0x1D (); void op0x1E (); void op0x1F ();

	void op0x20 (); void op0x21 (); void op0x22 (); void op0x23 ();
	void op0x24 (); void op0x25 (); void op0x26 (); void op0x27 ();
	void op0x28 (); void op0x29 (); void op0x2A (); void op0x2B ();
	void op0x2C (); void op0x2D (); void op0x2E (); void op0x2F ();

	void op0x30 (); void op0x31 (); void op0x32 (); void op0x33 ();
	void op0x34 (); void op0x35 (); void op0x36 (); void op0x37 ();
	void op0x38 (); void op0x39 (); void op0x3A (); void op0x3B ();
	void op0x3C (); void op0x3D (); void op0x3E (); void op0x3F ();

	void op0x40 (); void op0x41 (); void op0x42 (); void op0x43 ();
	void op0x44 (); void op0x45 (); void op0x46 (); void op0x47 ();
	void op0x48 (); void op0x49 (); void op0x4A (); void op0x4B ();
	void op0x4C (); void op0x4D (); void op0x4E (); void op0x4F ();

	void op0x50 (); void op0x51 (); void op0x52 (); void op0x53 ();
	void op0x54 (); void op0x55 (); void op0x56 (); void op0x57 ();
	void op0x58 (); void op0x59 (); void op0x5A (); void op0x5B ();
	void op0x5C (); void op0x5D (); void op0x5E (); void op0x5F ();

	void op0x60 (); void op0x61 (); void op0x62 (); void op0x63 ();
	void op0x64 (); void op0x65 (); void op0x66 (); void op0x67 ();
	void op0x68 (); void op0x69 (); void op0x6A (); void op0x6B ();
	void op0x6C (); void op0x6D (); void op0x6E (); void op0x6F ();

	void op0x70 (); void op0x71 (); void op0x72 (); void op0x73 ();
	void op0x74 (); void op0x75 (); void op0x76 (); void op0x77 ();
	void op0x78 (); void op0x79 (); void op0x7A (); void op0x7B ();
	void op0x7C (); void op0x7D (); void op0x7E (); void op0x7F ();

	void op0x80 (); void op0x81 (); void op0x82 (); void op0x83 ();
	void op0x84 (); void op0x85 (); void op0x86 (); void op0x87 ();
	void op0x88 (); void op0x89 (); void op0x8A (); void op0x8B ();
	void op0x8C (); void op0x8D (); void op0x8E (); void op0x8F ();

	void op0x90 (); void op0x91 (); void op0x92 (); void op0x93 ();
	void op0x94 (); void op0x95 (); void op0x96 (); void op0x97 ();
	void op0x98 (); void op0x99 (); void op0x9A (); void op0x9B ();
	void op0x9C (); void op0x9D (); void op0x9E (); void op0x9F ();

	void op0xA0 (); void op0xA1 (); void op0xA2 (); void op0xA3 ();
	void op0xA4 (); void op0xA5 (); void op0xA6 (); void op0xA7 ();
	void op0xA8 (); void op0xA9 (); void op0xAA (); void op0xAB ();
	void op0xAC (); void op0xAD (); void op0xAE (); void op0xAF ();

	void op0xB0 (); void op0xB1 (); void op0xB2 (); void op0xB3 ();
	void op0xB4 (); void op0xB5 (); void op0xB6 (); void op0xB7 ();
	void op0xB8 (); void op0xB9 (); void op0xBA (); void op0xBB ();
	void op0xBC (); void op0xBD (); void op0xBE (); void op0xBF ();

	void op0xC0 (); void op0xC1 (); void op0xC2 (); void op0xC3 ();
	void op0xC4 (); void op0xC5 (); void op0xC6 (); void op0xC7 ();
	void op0xC8 (); void op0xC9 (); void op0xCA (); void op0xCB ();
	void op0xCC (); void op0xCD (); void op0xCE (); void op0xCF ();

	void op0xD0 (); void op0xD1 (); void op0xD2 (); void op0xD3 ();
	void op0xD4 (); void op0xD5 (); void op0xD6 (); void op0xD7 ();
	void op0xD8 (); void op0xD9 (); void op0xDA (); void op0xDB ();
	void op0xDC (); void op0xDD (); void op0xDE (); void op0xDF ();

	void op0xE0 (); void op0xE1 (); void op0xE2 (); void op0xE3 ();
	void op0xE4 (); void op0xE5 (); void op0xE6 (); void op0xE7 ();
	void op0xE8 (); void op0xE9 (); void op0xEA (); void op0xEB ();
	void op0xEC (); void op0xED (); void op0xEE (); void op0xEF ();

	void op0xF0 (); void op0xF1 (); void op0xF2 (); void op0xF3 ();
	void op0xF4 (); void op0xF5 (); void op0xF6 (); void op0xF7 ();
	void op0xF8 (); void op0xF9 (); void op0xFA (); void op0xFB ();
	void op0xFC (); void op0xFD (); void op0xFE (); void op0xFF ();

	/* Instructions with 0xCB.. prefix */
	void cb0x00 (); void cb0x01 (); void cb0x02 (); void cb0x03 ();
	void cb0x04 (); void cb0x05 (); void cb0x06 (); void cb0x07 ();
	void cb0x08 (); void cb0x09 (); void cb0x0A (); void cb0x0B ();
	void cb0x0C (); void cb0x0D (); void cb0x0E (); void cb0x0F ();

	void cb0x10 (); void cb0x11 (); void cb0x12 (); void cb0x13 ();
	void cb0x14 (); void cb0x15 (); void cb0x16 (); void cb0x17 ();
	void cb0x18 (); void cb0x19 (); void cb0x1A (); void cb0x1B ();
	void cb0x1C (); void cb0x1D (); void cb0x1E (); void cb0x1F ();

	void cb0x20 (); void cb0x21 (); void cb0x22 (); void cb0x23 ();
	void cb0x24 (); void cb0x25 (); void cb0x26 (); void cb0x27 ();
	void cb0x28 (); void cb0x29 (); void cb0x2A (); void cb0x2B ();
	void cb0x2C (); void cb0x2D (); void cb0x2E (); void cb0x2F ();

	void cb0x30 (); void cb0x31 (); void cb0x32 (); void cb0x33 ();
	void cb0x34 (); void cb0x35 (); void cb0x36 (); void cb0x37 ();
	void cb0x38 (); void cb0x39 (); void cb0x3A (); void cb0x3B ();
	void cb0x3C (); void cb0x3D (); void cb0x3E (); void cb0x3F ();

	void cb0x40 (); void cb0x41 (); void cb0x42 (); void cb0x43 ();
	void cb0x44 (); void cb0x45 (); void cb0x46 (); void cb0x47 ();
	void cb0x48 (); void cb0x49 (); void cb0x4A (); void cb0x4B ();
	void cb0x4C (); void cb0x4D (); void cb0x4E (); void cb0x4F ();

	void cb0x50 (); void cb0x51 (); void cb0x52 (); void cb0x53 ();
	void cb0x54 (); void cb0x55 (); void cb0x56 (); void cb0x57 ();
	void cb0x58 (); void cb0x59 (); void cb0x5A (); void cb0x5B ();
	void cb0x5C (); void cb0x5D (); void cb0x5E (); void cb0x5F ();

	void cb0x60 (); void cb0x61 (); void cb0x62 (); void cb0x63 ();
	void cb0x64 (); void cb0x65 (); void cb0x66 (); void cb0x67 ();
	void cb0x68 (); void cb0x69 (); void cb0x6A (); void cb0x6B ();
	void cb0x6C (); void cb0x6D (); void cb0x6E (); void cb0x6F ();

	void cb0x70 (); void cb0x71 (); void cb0x72 (); void cb0x73 ();
	void cb0x74 (); void cb0x75 (); void cb0x76 (); void cb0x77 ();
	void cb0x78 (); void cb0x79 (); void cb0x7A (); void cb0x7B ();
	void cb0x7C (); void cb0x7D (); void cb0x7E (); void cb0x7F ();

	void cb0x80 (); void cb0x81 (); void cb0x82 (); void cb0x83 ();
	void cb0x84 (); void cb0x85 (); void cb0x86 (); void cb0x87 ();
	void cb0x88 (); void cb0x89 (); void cb0x8A (); void cb0x8B ();
	void cb0x8C (); void cb0x8D (); void cb0x8E (); void cb0x8F ();

	void cb0x90 (); void cb0x91 (); void cb0x92 (); void cb0x93 ();
	void cb0x94 (); void cb0x95 (); void cb0x96 (); void cb0x97 ();
	void cb0x98 (); void cb0x99 (); void cb0x9A (); void cb0x9B ();
	void cb0x9C (); void cb0x9D (); void cb0x9E (); void cb0x9F ();

	void cb0xA0 (); void cb0xA1 (); void cb0xA2 (); void cb0xA3 ();
	void cb0xA4 (); void cb0xA5 (); void cb0xA6 (); void cb0xA7 ();
	void cb0xA8 (); void cb0xA9 (); void cb0xAA (); void cb0xAB ();
	void cb0xAC (); void cb0xAD (); void cb0xAE (); void cb0xAF ();

	void cb0xB0 (); void cb0xB1 (); void cb0xB2 (); void cb0xB3 ();
	void cb0xB4 (); void cb0xB5 (); void cb0xB6 (); void cb0xB7 ();
	void cb0xB8 (); void cb0xB9 (); void cb0xBA (); void cb0xBB ();
	void cb0xBC (); void cb0xBD (); void cb0xBE (); void cb0xBF ();

	void cb0xC0 (); void cb0xC1 (); void cb0xC2 (); void cb0xC3 ();
	void cb0xC4 (); void cb0xC5 (); void cb0xC6 (); void cb0xC7 ();
	void cb0xC8 (); void cb0xC9 (); void cb0xCA (); void cb0xCB ();
	void cb0xCC (); void cb0xCD (); void cb0xCE (); void cb0xCF ();

	void cb0xD0 (); void cb0xD1 (); void cb0xD2 (); void cb0xD3 ();
	void cb0xD4 (); void cb0xD5 (); void cb0xD6 (); void cb0xD7 ();
	void cb0xD8 (); void cb0xD9 (); void cb0xDA (); void cb0xDB ();
	void cb0xDC (); void cb0xDD (); void cb0xDE (); void cb0xDF ();

	void cb0xE0 (); void cb0xE1 (); void cb0xE2 (); void cb0xE3 ();
	void cb0xE4 (); void cb0xE5 (); void cb0xE6 (); void cb0xE7 ();
	void cb0xE8 (); void cb0xE9 (); void cb0xEA (); void cb0xEB ();
	void cb0xEC (); void cb0xED (); void cb0xEE (); void cb0xEF ();

	void cb0xF0 (); void cb0xF1 (); void cb0xF2 (); void cb0xF3 ();
	void cb0xF4 (); void cb0xF5 (); void cb0xF6 (); void cb0xF7 ();
	void cb0xF8 (); void cb0xF9 (); void cb0xFA (); void cb0xFB ();
	void cb0xFC (); void cb0xFD (); void cb0xFE (); void cb0xFF ();

	void opNull ();
};

#endif

// References
// https://wornwinter.wordpress.com/2015/02/14/adventures-in-gameboy-emulation-part-2-the-cpu/
