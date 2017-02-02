#ifndef COMMON_MEMORY_ADDRESSES
#define COMMON_MEMORY_ADDRESSES

/* Cartridge Header */
#define GAME_TITLE      0x134
#define CARTRIDGE_TYPE  0x147
#define ROM_SIZE        0x148

/* Timer related Addresses */
#define DIV  0xFF04
#define TIMA 0xFF05
#define TMA  0xFF06
#define TMC  0xFF07

/* Display related */
#define LCDCTRL 0xFF40
#define LCDSTAT 0xFF41
#define SCROLLY 0xFF42
#define SCROLLX 0xFF43
#define CURLINE 0xFF44
#define CMPLINE 0xFF45

/* Interrupt related */
#define IF 0xFF0F
#define IE 0xFFFF

#endif
