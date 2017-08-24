#include <iostream>
#include <stdint.h>
#include <chrono>
#include <thread>

#include <imgui/imgui.h>

#include "context/context.hpp"
#include "gameboy.hpp"
#include "disassembler/disassembler.hpp"
#include "rom.hpp"
#include "timer.hpp"
#include "debug.hpp"

const int defaultCyclesPerFrame = 69905;
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

	GameBoy gameBoy;
	gameBoy.Initialize();
	gameBoy.LoadRom(rom);

	Disassembler disassembler(&gameBoy);

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
			gameBoy.ToggleHaltState();

		speed = Context::GetSpeedInput();
		const int cyclesThisFrame = defaultCyclesPerFrame * speed;
		if (gameBoy.GetHaltState() && Context::ShouldStep() == true)
			gameBoy.StepInstruction();
		else if (gameBoy.GetHaltState() == false && Context::ShouldStep() == false)
			gameBoy.StepEmulation(cyclesThisFrame);

		// -------------- //
		// Begin Gui code //
		// -------------- //
		static bool show_rom_info      = false;
		static bool show_cpu_window    = true;
		static bool show_joypad_status = false;
		static bool show_memory_viewer = false;
		static bool show_disassembler  = true;

		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("Debug")){
			ImGui::MenuItem("CPU",          "c", &show_cpu_window);
			ImGui::MenuItem("Disassembler", "d", &show_disassembler);
			ImGui::MenuItem("Joypad",       "h", &show_joypad_status);
			ImGui::MenuItem("Memory",       "m", &show_memory_viewer);
			ImGui::MenuItem("Rom Info",     "r", &show_rom_info);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		if (show_rom_info)
			RomInfoWindow(&show_rom_info, rom);
		if (show_cpu_window)
			Debug::ShowCpuWindow(&show_cpu_window, &gameBoy);
		if (show_joypad_status)
			Debug::ShowJoypadWindow(&show_joypad_status, gameBoy.GetJoypadBuffer());
		if (show_memory_viewer)
			Debug::ShowMemoryWindow(&show_memory_viewer, gameBoy.GetMMU());
		if (show_disassembler)
			disassembler.ShowDisassemblerWindow(&show_disassembler);

		// ImGui::ShowTestWindow();
		// - End -------- //

		Context::RenderDisplay();

		int elapsedTime = timer.GetTimeInMiliseconds();
		if (16 > elapsedTime)
			std::this_thread::sleep_for(std::chrono::milliseconds(16 - elapsedTime));
		elapsedTime = timer.GetTimeInMiliseconds();

		Context::SetTitle("KGB Emulator | " +
		                  std::to_string(cyclesThisFrame) + " Hz | " +
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
