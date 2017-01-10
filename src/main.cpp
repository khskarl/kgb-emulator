#include <iostream>
#include <stdint.h>
#include <chrono>
#include <thread>

#include "context/context.hpp"
#include "gameboy.hpp"
#include "disassembler.hpp"
#include "rom.hpp"
#include "timer.hpp"
#include "debug.hpp"

int main(int argc, char const *argv[]) {
	if (argc < 2) {
		std::cout << "<*Error*> You didn't supply a ROM path as argument\n" <<
		             "Usage example: ./build/gbemu.out roms/tetris.gb\n";
	}

	const char* filepath = argv[1];

	Rom rom(filepath);
	if (rom.isLoaded == false) {
		std::cout << "<*Error*> Failed to load: " << filepath << " \n";
		return 1;
	}

	if (rom.isLoaded) {
		std::cout << "[ROM DATA]" << '\n' <<
		             "Name: " << rom.GetName() << '\n' <<
		             "Size: " << rom.GetSize() << " B\n";
		// std::cout << DisassembleRom(rom) << '\n';
	}

	Rom bios("roms/bios.gb");

	GameBoy gameBoy;
	gameBoy.Initialize();
	gameBoy.LoadBios(bios);
	gameBoy.LoadRom(rom);

	Context::SetupContext(3);
	Context::SetTitle("MyGameBoy");
	Context::SetDisplayBuffer(gameBoy.GetDisplayBuffer());

	Timer timer;
	while (Context::IsOpen()) {
		timer.Reset();
		Context::HandleEvents();

		// Toggle halt state if halt button was pressed
		if (Context::ShouldHalt())
			gameBoy.ToggleHalt();

		if (gameBoy.GetHalt() && Context::ShouldStep() == true)
			gameBoy.StepInstruction();
		else if (gameBoy.GetHalt() == false && Context::ShouldStep() == false)
			gameBoy.StepEmulation();

		Context::SetDebugText(Debug::GetGameboyText(gameBoy));
		Context::RenderDisplay();

		int elapsedTime = timer.GetTimeInMiliseconds();

		if (16 > elapsedTime)
			std::this_thread::sleep_for(std::chrono::milliseconds(16 - elapsedTime));

		elapsedTime = timer.GetTimeInMiliseconds();
		Context::SetTitle("MyGameBoy | " + std::to_string(elapsedTime) + " ms");
	}

	Context::DestroyContext();

	return 0;
}
