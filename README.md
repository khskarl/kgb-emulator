# KGB emulator
_Karll's Game Boy_ is a Game Boy emulator written in C++14 for learning emulation and modern C++. It doesn't do anything interesting yet.

## Dependencies
-   SFML2

## Usage example
A makefile is present in the repository, to build go to the project's root directory using the terminal and type the following command:
```
$ mkdir build && cd build && cmake .. && make && cd ..
```

To run the program, type the name of the executable followed by the rom's location:
```
$ ./build/gameboy-emulator roms/tetris.gb
```

## TODO
-   [x] Port to SFML
-   [x] Tetris Title Screen
-   [x] Input
-   [ ] Pass Blargh's cpu_instr tests
-   [x] Play Tetris
-   [ ] Get Opus5 working
-   [ ] MBC1
-   [ ] Sound
-   [ ] Play Metroid II

## Known bugs
-   It doesn't work for anything else other than Tetris and Dr. Mario.

## References & Game Boy documentation
-   [DuoDreamer's GameBoy Memory map](http://gameboy.mongenel.com/dmg/asmmemmap.html)
-   [RealBoy in-depth blog](https://realboyemulator.wordpress.com/)
-   [Pan Docs GameBoy Documentation](http://bgb.bircd.org/pandocs.htm)
-   [Pastraiser's GameBoy CPU instruction table](http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)
-   [Codeslinger's GameBoy emulation guide](http://www.codeslinger.co.uk/pages/projects/gameboy.html)
-   [Game Boy Boot ROM Disassembly](https://gist.github.com/drhelius/6063288)
