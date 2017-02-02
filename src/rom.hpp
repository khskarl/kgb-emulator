#ifndef ROM_HPP
#define ROM_HPP

#include <memory>
#include <string>
#include <stdint.h>

class Rom {
private:
	/* Rom data */
	uint8_t* data = nullptr;
	size_t size = 0;

	/* Rom header info */
	std::string name;
	uint8_t numRomBanks = 0;

	bool LoadRomFromFilepath (std::string path);
public:
	bool isLoaded = false;

	Rom (std::string path);
	~Rom ();

	uint8_t* const    GetData() const;
	const size_t      GetSize() const;
	const std::string GetName() const;
	const std::string GetCatridgeType() const;
};

#endif
