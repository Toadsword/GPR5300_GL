#include <file_utility.h>
#include <fstream>
#include <iostream>
#include <ostream>


const std::string LoadFile(std::string path)
{
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}

std::string GetFilenameExtension(std::string path)
{
	std::string extension = "";
	const auto folderLastIndex = path.find_last_of('/');
	std::string filename = path.substr(folderLastIndex + 1, path.size());
	const auto filenameExtensionIndex = filename.find_last_of('.');
	if (filenameExtensionIndex > path.size())
	{
		std::cerr << "[Error] Path: " << path << " has not a correct extension";
		return extension;
	}
	extension = filename.substr(filenameExtensionIndex);
	return extension;
}

std::string GetFilenameFromPath(std::string path)
{
	std::string extension = "";
#ifdef WIN32
	const auto folderLastIndex = path.find_last_of('\\');
#else
	const auto folderLastIndex = path.find_last_of('/');
#endif

	std::string filename = path.substr(folderLastIndex + 1, path.size());
	
	return filename;
}

const BinaryFile LoadBinaryFile(std::string path)
{
    std::ifstream shaderFile;
    shaderFile.open(path, std::ios::binary | std::ios::ate);
    if (shaderFile.is_open())
    {
        size_t size = shaderFile.tellg();
        shaderFile.seekg(0, std::ios::beg);
        char* bin = new char[size];
        shaderFile.read(bin, size);
        return {bin, size};
    }
    else
    {
        std::cerr << "Could not open \"" << path << "\"" << std::endl;
        return {nullptr,0};
    }
}
