#include "disassembler.hpp"

#include <gameboy.hpp>
#include "decoding.hpp"

#include <imgui/imgui.h>

#include <cstdint>
#include <vector>
#include <string>

Disassembler::Disassembler (GameBoy* const _gameboy) {
	p_gameboy = _gameboy;
	// m_disassembly = DisassembleRom();

}

Disassembler::~Disassembler () {

}

std::vector<std::string> Disassembler::Disassemble () {
	const uint8_t* memory = p_gameboy->mmu.GetMemoryRef(0);

	char buffer[64];
	uint16_t pc = 0x0;

	std::vector<std::string> disassembly = {};
	while (pc < 0x00FF) {
		sprintf(buffer, "%04x ", pc);
		std::string address(buffer);

		std::string line = address + DisassembleOpcode(memory + pc);
		disassembly.push_back(line);
	}

	return disassembly;
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

void Disassembler::ShowDisassemblerWindow (bool* p_open) {
	ImGui::SetNextWindowPos(ImVec2(350, 50), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(275, 275), ImGuiSetCond_FirstUseEver);

	ImGui::Begin("Disassembler", p_open);
	ImGui::PushItemWidth(-1);
	static int current_index = 0;
	{
			// if (i > 0) ImGui::SameLine();
			// ImGui::PushID(current_index);
			// ListBox("", &current_index, m_disassembly);
			// ImGui::PopID();
	}
	ImGui::PopItemWidth();
	ImGui::End();
}
