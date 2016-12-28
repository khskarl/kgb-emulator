#include "mmu.hpp"

MMU::MMU (CPU* _cpu) {
	// if (master == nullptr) {
	// 	assert("Invalid CPU passed as constructor argument");
	// }
	this->cpu = _cpu;
}

MMU::~MMU () {

}

uint8_t MMU::ReadByte (uint16_t address) {
	switch (address & 0xF000) {
		/* BIOS / ROM0 */
		case 0x0000:
		if (inBios) {
			if (address < 0x0100)
				return bios[address];
			else if (cpu->PC == 0x100)
				this->inBios = false;
		}
		return rom[address];
		break;

		/* ROM0 */
		case 0x1000:
		case 0x2000:
		case 0x3000:
		return rom[address];
		break;

		/* ROM1 (unbanked) (16k) */
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
		return rom[address];
		break;

		/* VRAM (8k) */
		case 0x8000:
		case 0x9000:
		return vram[address & 0x1FFF];

		/* External RAM (8k) */
		case 0xA000:
		case 0xB000:
		return eram[address & 0x1FFF];

		/* Working RAM (8k) */
		case 0xC000:
		case 0xD000:
		return wram[address & 0x1FFF];

		/* Working RAM shadow */
		case 0xE000:
		return wram[address & 0x1FFF];

		/* Working RAM shadow, I/O, Zero-page RAM */
		case 0xF000:
		{
			uint8_t lo = address & 0x0F00;
			if (0x000 <= lo && lo <= 0xD00) {
				return wram[address & 0x1FFF];
			}
			else if (lo == 0xE00) {
				if (address < 0xFEA0)
					return oam[address & 0xFF];
				else
					return 0;
			}
			else if (lo == 0xF00) {
				if (address >= 0xFF80)
					return zram[address & 0x7F];
			}
			else
				return 0;
		}
		default:
		// assert blablabla
		return 0;
		break;

	}
}

uint16_t MMU::ReadWord (uint16_t address) {
	return ReadByte(address) | (ReadByte(address + 1) << 8);
}

// References
// http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory
