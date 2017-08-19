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

bool StringVectorGetter(void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		  *out_text = vector.at(idx).c_str();
	return true;
};

bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
{
	if (values.empty()) { return false; }
	return ImGui::ListBox(label, currIndex, StringVectorGetter,
		static_cast<void*>(&values), values.size(), 20);
}

void Debug::ShowDisassemblerWindow (bool* p_open, std::vector<std::string>& disassembly) {
	ImGui::SetNextWindowPos(ImVec2(120, 200), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiSetCond_FirstUseEver);

	ImGui::Begin("Disassembler", p_open);
	ImGui::PushItemWidth(-1);
	ImGui::Text("Lists:");
	static int current_index = 0;
	{
			// if (i > 0) ImGui::SameLine();
			ImGui::PushID(current_index);
			ListBox("", &current_index, disassembly);
			ImGui::PopID();
	}
	ImGui::PopItemWidth();
	ImGui::End();
}


// std::string Debug::GetGameboyText (GameBoy& gameboy) {
// 	const CPU& cpu = *gameboy.GetCPU();
// 	MMU* mmu = gameboy.GetMMU();
//
// 	std::string text = "";
// 	// text += "ScrollY: " + std::to_string(mmu->ReadByte(SCROLLY)) + "\n";
// 	// text += "ScrollX: " + std::to_string(mmu->ReadByte(SCROLLX)) + "\n";
// 	// text += "CurLine: " + std::to_string(mmu->ReadByte(CURLINE)) + "\n";
//
// 	char buffer[8];
//
// 	sprintf(buffer, "%02X", (*mmu->GetMemoryRef(0xFF00)));
// 	text += "FF00:" + std::string(buffer) + "\n";
// 	text += "FF44:" + std::to_string((*mmu->GetMemoryRef(0xFF44)));
//
// 	return text;
// }
