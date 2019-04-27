#pragma once

#include <string>

struct BinaryFile
{
    char* bin;
    size_t size;
};

const std::string LoadFile(std::string path);
const BinaryFile LoadBinaryFile(std::string path);
std::string GetFilenameExtension(std::string path);
std::string GetFilenameFromPath(std::string path);