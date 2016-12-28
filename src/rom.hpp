#ifndef ROM_HPP
#define ROM_HPP

#include <memory>
#include <string>
#include <stdint.h>

class Rom {
private:
	/* Rom data */
	std::shared_ptr<uint8_t> data = nullptr;
	size_t size = 0;

	/* Rom header info */
	std::string name;

	bool LoadRomFromFilepath (std::string path);
public:
	bool isLoaded = false;

	Rom (std::string path);
	~Rom ();

	uint8_t*    GetData();
	size_t      GetSize();
	std::string GetName();
};

#endif
