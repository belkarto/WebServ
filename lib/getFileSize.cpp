#include "webserv.hpp"

std::streamsize	getFileSize(std::ifstream *file)
{
	std::streamsize	file_size;

	file->seekg(0, std::ios::end);
	file_size = file->tellg();
	file->seekg(0, std::ios::beg);
	return file_size;
}