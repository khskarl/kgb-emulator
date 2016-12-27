#ifndef ROM_HPP
#define ROM_HPP

#include <memory>
#include <string>
#include <stdint.h>

class Rom {
private:
	std::shared_ptr<uint8_t> data = nullptr;
	size_t size = 0;

	bool LoadRomFromFilepath (std::string path);
public:
	bool isLoaded = false;

	Rom (std::string path);
	virtual ~Rom ();

	size_t GetSize();
	uint8_t* GetData();
};

#endif
