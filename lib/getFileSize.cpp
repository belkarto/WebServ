#include "webserv.hpp"

std::streamsize	getFileSize(std::ifstream *file)
{
	std::streamsize	file_size;

	file->seekg(0, std::ios::end);
	file_size = file->tellg();
	file->seekg(0, std::ios::beg);
	return file_size;
}

std::streamsize	getFileSize(int fd)
{
    char			buffer[1024];
    std::streamsize	totalBytes;
    std::streamsize	bytesRead;

	off_t originalOffset = lseek(fd, 0, SEEK_CUR);
	totalBytes = 0;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
	{
        totalBytes += bytesRead;
	}
	std::cout << originalOffset << std::endl;
	std::cout << lseek(fd, originalOffset, SEEK_SET) << std::endl;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
        write(STDOUT_FILENO, buffer, bytesRead);
    return totalBytes;
}
