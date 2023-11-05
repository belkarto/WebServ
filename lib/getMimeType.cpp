#include "webserv.hpp"

std::string	getMimeType(const std::string& fileExtension) 
{
	std::map<std::string, std::string> mimeTypes = {
	    {".jpg", "image/jpeg"},
	    {".jpeg", "image/jpeg"},
	    {".html", "text/html"},
	};	
	std::map<std::string, std::string>::iterator it = mimeTypes.find(fileExtension);
	if (it != mimeTypes.end()) 
	    return it->second;
	return "application/octet-stream";
}