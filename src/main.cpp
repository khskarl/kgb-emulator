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

void RomInfoWindow (bool* p_open, Rom& rom) {
	ImGui::SetNextWindowPos(ImVec2(50, 400), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Rom Info", p_open);
		ImGui::Text("Name:      %s", rom.GetName().c_str());
		ImGui::Text("Size:      %i", rom.GetSize());
		ImGui::Text("Type:      %s", rom.GetType().c_str());
		ImGui::Text("#RomBanks: %i", rom.GetNumRomBanks());
	ImGui::End();
}

void run_emulator(const std::string& filepath) {
	Rom rom(filepath);


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

		// -------------- //
		// Begin Gui code //
		// -------------- //
		static bool show_rom_info = true;
		static bool show_cpu_window = true;
		static bool show_joypad_status = true;
		static bool show_memory_viewer = true;
		static bool show_disassembler = true;
		if (show_rom_info)
			RomInfoWindow(&show_rom_info, rom);
		if (show_cpu_window)
			Debug::ShowCpuWindow(&show_cpu_window, &gameBoy);
		if (show_joypad_status)
			Debug::ShowJoypadWindow(&show_joypad_status, gameBoy.GetJoypadBuffer());
		if (show_memory_viewer)
			Debug::ShowMemoryWindow(&show_memory_viewer, gameBoy.GetMMU());
		if (show_disassembler)
			Debug::ShowDisassemblerWindow(&show_disassembler);

		// ImGui::ShowTestWindow();
		// - End -------- //

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
