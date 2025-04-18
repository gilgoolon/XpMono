#include "File.hpp"

#include <fstream>

File::File(std::filesystem::path path):
    _path(std::move(path))
{
}

std::string File::read_text()
{
    std::ifstream file(_path);
    std::string str;
    std::string file_contents;
    while (std::getline(file, str))
    {
        file_contents += str;
        file_contents.push_back('\n');
    }
    return file_contents;
}
