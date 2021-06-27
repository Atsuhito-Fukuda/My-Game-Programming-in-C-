#pragma once
#include <string>

class Tools {
	
public:
	static std::string GetFileExtension(const std::string& filename);
	static std::string DetachFileExtension(std::string& filename_tobedetached);
	static std::string GetFileNameOnly(const std::string& filename);
	static std::string DetachEnd(std::string& filename);	

};