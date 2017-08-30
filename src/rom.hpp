#ifndef ROM_HPP
#define ROM_HPP

#include <stdint.h>
#include <vector>
#include <string>

class Rom {
private:
	/* Rom data */
	std::vector<uint8_t> m_data;

	/* Rom header info */
	std::string m_name;
	std::string m_type;
	size_t m_romSize = 0;
	size_t m_ramSize = 0;
	size_t m_numRomBanks = 0;
	size_t m_numRamBanks = 0;
	uint8_t m_mbc = 0;

	bool LoadRomFromFilepath (std::string path);

public:
	Rom (std::string path);
	~Rom ();

	const uint8_t*     GetData() const;
	size_t             GetRomSize() const;
	size_t             GetRamSize() const;
	const std::string& GetName() const;
	const std::string& GetType() const;
	uint8_t            GetNumRomBanks () const;
	uint8_t            GetNumRamBanks () const;
};


inline const uint8_t* Rom::GetData () const {
	return m_data.data();
}

inline size_t Rom::GetRomSize () const {
	return m_romSize;
}

inline size_t Rom::GetRamSize () const {
	return m_ramSize;
}

inline const std::string& Rom::GetName () const {
	return m_name;
}

inline const std::string& Rom::GetType () const {
	return m_type;
}

inline uint8_t Rom::GetNumRomBanks() const {
	return m_numRomBanks;
}

inline uint8_t Rom::GetNumRamBanks() const {
	return m_numRamBanks;
}


#endif
