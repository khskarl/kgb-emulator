#include "rom.hpp"

#include <fstream>
#include <iostream>
#include <utility>
#include <cassert>

#include "common_memory_addresses.hpp"
#include "exception.hpp"


static std::string get_cartridge_type_string(uint8_t id);


Rom::Rom (const std::string path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	const size_t size = file.tellg();
	file.seekg(0);

	m_data.resize(size + 1);

	file.read((char*) m_data.data(), size);

	if (file.good() == false)
		throw Exception("failed to read file \'" + path + '\'');

	m_name = std::string(reinterpret_cast<char const*>(&m_data[0x134]), 16);
	uint8_t type = m_data[CARTRIDGE_TYPE];
	switch (type) {
		case 0x00:
		m_mbc = 0; // NONE
		break;
		case 0x01:
		case 0x02:
		case 0x03:
		m_mbc = 1; // MBC1
		break;
		default:
		assert("Cartidge type unsupported!" && 0);
		break;
	}
	m_type = get_cartridge_type_string(type);
}

Rom::~Rom () { }

std::string get_cartridge_type_string(const uint8_t id)
{
	switch (id) {
		case 0x00: return "ROM ONLY";
		case 0x01: return "MBC1";
		case 0x02: return "MBC1+RAM";
		case 0x03: return "MBC1+RAM+BATTERY";
		case 0x05: return "MBC2";
		case 0x06: return "MBC2+BATTERY";
		case 0x08: return "ROM+RAM";
		case 0x09: return "ROM+RAM+BATTERY";
		case 0x0B: return "MMM01";
		case 0x0C: return "MMM01+RAM";
		case 0x0D: return "MMM01+RAM+BATTERY";
		case 0x0F: return "MBC3+TIMER+BATTERY";
		case 0x10: return "MBC3+TIMER+RAM+BATTERY";
		case 0x11: return "MBC3";
		case 0x12: return "MBC3+RAM";
		case 0x13: return "MBC3+RAM+BATTERY";
		default: return "UNKNOWN";
	}
}
