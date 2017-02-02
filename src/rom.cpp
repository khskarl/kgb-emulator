#include "rom.hpp"

#include <fstream>
#include <iostream>

#include "common_memory_addresses.hpp"

Rom::Rom (std::string path) {
	this->isLoaded = LoadRomFromFilepath(path);
}

Rom::~Rom () {}

bool Rom::LoadRomFromFilepath (std::string path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (file.is_open() == false)
		return false;

	this->size = file.tellg();
	file.seekg(0);

	this->data = new uint8_t[this->size];
	file.read((char*) this->data, this->size);

	/* Undo changes in case we failed to read the file */
	if (file.good() == false) {
		this->size = 0;
		return false;
	}

	this->name = std::string(reinterpret_cast<char const*>(this->data + 0x134), 16);

	file.close();
	return true;
}

uint8_t* const Rom::GetData () const {
	return this->data;
}

const size_t Rom::GetSize () const {
	return this->size;
}

const std::string Rom::GetName () const {
	return this->name;
}

const std::string Rom::GetCatridgeType () const {
	std::string type = "";
	uint8_t id = data[CARTRIDGE_TYPE];
	switch (id) {
		case 0x00:
		return "ROM ONLY";
		break;
		case 0x01:
		return "MBC1";
		break;
		case 0x02:
		return "MBC1+RAM";
		break;
		case 0x03:
		return "MBC1+RAM+BATTERY";
		break;
		case 0x05:
		return "MBC2";
		break;
		case 0x06:
		return "MBC2+BATTERY";
		break;
		case 0x08:
		return "ROM+RAM";
		break;
		case 0x09:
		return "ROM+RAM+BATTERY";
		break;
		case 0x0B:
		return "MMM01";
		break;
		case 0x0C:
		return "MMM01+RAM";
		break;
		case 0x0D:
		return "MMM01+RAM+BATTERY";
		break;
		case 0x0F:
		return "MBC3+TIMER+BATTERY";
		break;
		case 0x10:
		return "MBC3+TIMER+RAM+BATTERY";
		break;
		case 0x11:
		return "MBC3";
		break;
		case 0x12:
		return "MBC3+RAM";
		break;
		case 0x13:
		return "MBC3+RAM+BATTERY";
		break;
		default:
		return "UNKNOWN";
		break;
	}

}

const uint8_t Rom::GetNumRomBanks() const {
	return (this->size / 65536) * 4;
}
