#ifndef COMMON_MEMORY_ADDRESSES
#define COMMON_MEMORY_ADDRESSES

/* Cartridge Header */
#define GAME_TITLE      0x0134
#define CARTRIDGE_TYPE  0x0147
#define ROM_SIZE        0x0148

/* Object Attribute Memory (or sprite attribute table) */
#define OAM 0xFE00

/* Joypad */
#define JOYPAD 0xFF00

/* Timers Addresses */
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

/* Others */
#define DMA  0xFF46
#define BGP  0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49

/* Interrupt related */
#define IF 0xFF0F
#define IE 0xFFFF

#endif
