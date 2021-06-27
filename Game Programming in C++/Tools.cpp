#include "Tools.h"
#include "Error.h"

/*
	
	・stringからファイル名や拡張子を取り出す関数を用意している。

*/


std::string Tools::GetFileExtension(const std::string& filename) {

	std::string extension;
	std::size_t posnum = filename.rfind('.');

	if (posnum != std::string::npos) {

		extension = filename.substr(posnum + 1, filename.size() - posnum);

	}
	else
		error::Tool::ExtensionUnExist(filename);

	return extension;

}

std::string Tools::DetachFileExtension(std::string& filename_tobedetached) {

	std::string extension;
	std::size_t posnum = filename_tobedetached.rfind('.');

	if (posnum != std::string::npos) {

		extension = filename_tobedetached.substr(posnum + 1, filename_tobedetached.size() - posnum);
		filename_tobedetached.erase(posnum);

	}
	else
		error::Tool::ExtensionUnExist(filename_tobedetached);

	return extension;

}

std::string Tools::GetFileNameOnly(const std::string& filename) {

	std::size_t posnum = filename.rfind('/');	
	if (posnum != std::string::npos)
		return filename.substr(posnum + 1, filename.size() - posnum - 1);

	posnum = filename.rfind('\\');
	if (posnum != std::string::npos)
		return filename.substr(posnum + 1, filename.size() - posnum - 1);

	return filename;

}

std::string Tools::DetachEnd(std::string& filename) {

	std::string end;
	std::size_t posnum = filename.rfind('.');

	if (posnum != std::string::npos) {

		end = filename.substr(posnum + 1, filename.size() - posnum);
		filename.erase(posnum);

	}
	else {
		end = filename;
		filename = std::string("");		
	}

	return end;

}