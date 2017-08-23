#include <imgui/imgui.h>

#include "debug.hpp"
#include "gameboy.hpp"
#include "mmu.hpp"

void Debug::ShowCpuWindow (bool* p_open, GameBoy* const gameboy) {
	// char hex_input[5] = "";
	// const int hex_input_flags = ImGuiInputTextFlags_CharsHexadecimal |
	//                             ImGuiInputTextFlags_CharsUppercase;

	CPU* cpu = gameboy->GetCPU();
	ImGui::SetNextWindowPos(ImVec2(50,50), ImGuiSetCond_FirstUseEver);
	// Begin Window
	ImGui::Begin("CPU Status", p_open);

	ImGui::Text("AF: %04X", cpu->AF.word); ImGui::SameLine(82);
	ImGui::Text("BC: %04X", cpu->BC.word);
	ImGui::Text("DE: %04X", cpu->DE.word); ImGui::SameLine(82);
	ImGui::Text("HL: %04X", cpu->HL.word);
	ImGui::Text("PC: %04X", cpu->PC); ImGui::SameLine(82);
	ImGui::Text("SP: %04X", cpu->SP);

	ImGui::Spacing();

	bool Z = cpu->GetZ();
	bool N = cpu->GetN();
	bool H = cpu->GetH();
	bool C = cpu->GetC();
	ImGui::Checkbox("Z", &Z); ImGui::SameLine(0);
	ImGui::Checkbox("N", &N); ImGui::SameLine(0);
	ImGui::Checkbox("H", &H); ImGui::SameLine(0);
	ImGui::Checkbox("C", &C);
	cpu->SetZ(Z);
	cpu->SetN(N);
	cpu->SetH(H);
	cpu->SetC(C);

	ImGui::Separator();
	static bool show_cpu_editor = false;
	ImGui::Button("Edit");
	ImGui::End();
	// End Window
}

void Debug::ShowJoypadWindow (bool* p_open, uint8_t joypad_buffer[8]) {
	bool right  = joypad_buffer[0];
	bool left   = joypad_buffer[1];
	bool up     = joypad_buffer[2];
	bool down   = joypad_buffer[3];
	bool A      = joypad_buffer[4];
	bool B      = joypad_buffer[5];
	bool select = joypad_buffer[6];
	bool start  = joypad_buffer[7];

	ImGui::SetNextWindowPos(ImVec2(400, 50), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Joypad Status", p_open);
	ImGui::Checkbox("Up", &up); ImGui::SameLine(0);
	ImGui::Checkbox("Down", &down); ImGui::SameLine(0);
	ImGui::Checkbox("Left", &left); ImGui::SameLine(0);
	ImGui::Checkbox("Right", &right);
	ImGui::Checkbox("A", &A); ImGui::SameLine(0);
	ImGui::Checkbox("B", &B); ImGui::SameLine(0);
	ImGui::Checkbox("Select", &select); ImGui::SameLine(0);
	ImGui::Checkbox("Start", &start); ImGui::SameLine(0);
	ImGui::End();

}

void Debug::ShowMemoryWindow (bool* p_open, MMU* const mmu) {
	ImGui::Begin("Memory Viewer", p_open);

	ImGui::End();
}
