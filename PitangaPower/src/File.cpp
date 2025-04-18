#include "File.hpp"

#include <fstream>

File::File(std::filesystem::path path):
    _path(std::move(path))
{
}

std::vector<uint8_t> File::read_all()
{
    std::ifstream inputFile(_path, std::ios_base::binary);

    inputFile.seekg(0, std::ios_base::end);
    auto length = inputFile.tellg();
    inputFile.seekg(0, std::ios_base::beg);

    // Make a buffer of the exact size of the file and read the data into it.
    std::vector<uint8_t> buffer(length);
    inputFile.read(reinterpret_cast<char *>(buffer.data()), length);

    inputFile.close();
    return buffer;
}