#include <file_utility.h>
#include <fstream>
#include <iostream>

const std::string LoadFile(std::string path)
{
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}
