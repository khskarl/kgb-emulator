#ifndef CPU_HPP
#define CPU_HPP

#include <stdint.h>

/* This structure allows a simple way to access lo/hi bytes from a 16bit */
/* Example
	reg16_t HL = 0xFFAA;
	uint8_t H = HL.hi;
	uint8_t L = HL.lo;
*/

typedef union {
	struct {
		uint8_t lo;
		uint8_t hi;
	};
	uint16_t word;
} reg16_t;

typedef struct {
	uint8_t size;
	uint8_t cycles;
} Clock;


class CPU {
private:
	void (CPU::*opcodes  [0xFF]) ();
	void (CPU::*opcodesCB[0xFF]) ();

	void InitializeOpcodeTable ();

public:
	/* Registers */
	reg16_t AF;
	reg16_t BC;
	reg16_t DE;
	reg16_t HL;

	uint16_t SP;
	uint16_t PC;

	/* Flags */
	bool Z, N, H, C;

	uint8_t clockCycles;

	CPU ();
	~CPU ();

	void Initialize ();
	void EmulateCycle ();
	void FetchOpcode ();
	void EmulateOpcode ();

private:
	/* Instructions */
	void op0x00 (); void op0x01 (); void op0x02 (); void op0x03 ();
	void op0x04 (); void op0x05 (); void op0x06 (); void op0x07 ();
	void op0x08 (); void op0x09 (); void op0x0A (); void op0x0B ();
	void op0x0C (); void op0x0D (); void op0x0E (); void op0x0F ();

	void op0x10 (); void op0x11 (); void op0x12 (); void op0x13 ();
	void op0x14 (); void op0x15 (); void op0x16 (); void op0x17 ();
	void op0x18 (); void op0x19 (); void op0x1A (); void op0x1B ();
	void op0x1C (); void op0x1D (); void op0x1E (); void op0x1F ();

	void op0x3E ();

	void opNull ();
};

#endif

// References
// https://wornwinter.wordpress.com/2015/02/14/adventures-in-gameboy-emulation-part-2-the-cpu/
