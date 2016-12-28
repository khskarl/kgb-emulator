#include "rom.hpp"

#include <fstream>

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

	this->data = std::unique_ptr<uint8_t>(new uint8_t[this->size]);
	file.read((char*) this->data.get(), this->size);

	/* Undo changes in case we failed to read the file */
	if (file.good() == false) {
		this->size = 0;
		return false;
	}

	this->name = std::string(reinterpret_cast<char const*>(this->data.get() + 0x134), 16);

	file.close();
	return true;
}

uint8_t* Rom::GetData () {
	return this->data.get();
}

size_t Rom::GetSize () {
	return this->size;
}

std::string Rom::GetName () {
	return this->name;
}
