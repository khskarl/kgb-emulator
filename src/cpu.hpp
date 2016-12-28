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
	void (CPU::*opcodes[0xFF]) ();
	void (CPU::*opcodesCB[0xFF]) ();

	void InitializeOpcodeTable ();

public:
	reg16_t AF;
	reg16_t BC;
	reg16_t DE;
	reg16_t HL;

	uint16_t SP;
	uint16_t PC;

	uint8_t clockCycles;

	CPU ();
	virtual ~CPU ();

	void Initialize ();
	void EmulateCycle ();
	void FetchOpcode ();
	void EmulateOpcode ();

private:
	void opNull ();
	void op0x3E ();
};

#endif

/* References
https://wornwinter.wordpress.com/2015/02/14/adventures-in-gameboy-emulation-part-2-the-cpu/
*/
