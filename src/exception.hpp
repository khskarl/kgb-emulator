#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <exception>
#include <string>

class Exception : public std::exception {
private:
	std::string m_what;

public:
	Exception(std::string msg);
	const char* what() const noexcept override;
	
};


Exception::Exception(std::string msg) :
	m_what(std::move(msg))
{

}


const char* Exception::what() const noexcept
{
	return m_what.c_str();
}

#endif
