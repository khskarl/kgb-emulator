#include <iostream>
#include <stdint.h>
#include <chrono>
#include <thread>

#include <imgui/imgui.h>

#include "context/context.hpp"
#include "gameboy.hpp"
#include "disassembler.hpp"
#include "rom.hpp"
#include "timer.hpp"
#include "debug.hpp"

const int defaultCycles = 69905;
float speed = 1.0f;


void run_emulator(const std::string& filepath) {
	Rom rom(filepath);

	std::cout <<
		"[ROM DATA]" << '\n' <<
		"Name:      " << rom.GetName() << '\n' <<
		"Size:      " << rom.GetSize() << " B\n" <<
		"#RomBanks: " << (int) rom.GetNumRomBanks() << '\n' <<
		"Type:      " << rom.GetType() << '\n';
	// std::cout << DisassembleRom(rom) << '\n';

	GameBoy gameBoy;
	gameBoy.Initialize();
	gameBoy.LoadRom(rom);

	Context::SetupContext(4);
	Context::SetTitle("KGB Emulator");
	Context::SetDisplayBuffer(gameBoy.GetDisplayBuffer());
	Context::SetJoypadBuffer(gameBoy.GetJoypadBuffer());

	Timer timer;
	while (Context::IsOpen()) {
		timer.Reset();

		Context::HandleEvents();

		// Toggle halt state if halt button was pressed
		if (Context::ShouldHalt())
			gameBoy.ToggleHalt();

		speed = Context::GetSpeedInput();
		int cyclesThisUpdate = defaultCycles * speed;
		if (gameBoy.GetHalt() && Context::ShouldStep() == true)
			gameBoy.StepInstruction();
		else if (gameBoy.GetHalt() == false && Context::ShouldStep() == false)
			gameBoy.StepEmulation(cyclesThisUpdate);

		Context::SetDebugText(Debug::GetGameboyText(gameBoy));

		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button");
		ImGui::End();

		Context::RenderDisplay();

		int elapsedTime = timer.GetTimeInMiliseconds();
		if (16 > elapsedTime)
			std::this_thread::sleep_for(std::chrono::milliseconds(16 - elapsedTime));
		elapsedTime = timer.GetTimeInMiliseconds();

		Context::SetTitle("KGB Emulator | " +
		                  std::to_string(cyclesThisUpdate) + " Hz | " +
		                  std::to_string(elapsedTime) + " ms");
	}

	Context::DestroyContext();
}




int main(int argc, char const *argv[]) {

	if (argc < 2) {
		std::cerr << "<*Error*> You didn't supply a ROM path as argument\n" <<
		             "Usage example: ./build/gameboy.out roms/tetris.gb\n";
		return EXIT_FAILURE;
	}


	try {
		run_emulator(argv[1]);
		return EXIT_SUCCESS;
	} catch (std::exception& err) {
		std::cerr << "<*Error*> Fatal Exception: " << err.what() << '\n';
	} catch (...) {
		std::cerr << "<*Error*> Unknown Exception.\n";
	}

	// return failure in case of exception
	return EXIT_FAILURE;
}
