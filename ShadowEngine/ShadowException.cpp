#include "ShadowException.h"

#include <sstream>

ShadowException::ShadowException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* ShadowException::what() const noexcept 
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ShadowException::GetType() const noexcept
{
	return "Shadow Exception";
}

int ShadowException::GetLine() const noexcept
{
	return line;
}

const std::string & ShadowException::GetFile() const noexcept
{
	return file;
}

std::string ShadowException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << file << std::endl
		<< "[Line]" << line;
	return oss.str();
}
