#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <assert.h>
#include <string>
#include <cstdint>
#include <vector>

class GameBoy;
class MMU;
class Debug {
public:
	static void ShowCpuWindow (bool* p_open, GameBoy* const gameboy);
	static void ShowJoypadWindow (bool* p_open, uint8_t joypad_buffer[8]);
	static void ShowMemoryWindow (bool* p_open, MMU* const mmu);
	};
#endif
