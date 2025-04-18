#include "File.hpp"

#include <filesystem/fat.h>
#include <fstream>

File::File(std::filesystem::path path):
    m_path(std::move(path))
{
}

void File::write(const std::vector<uint8_t> &data)
{
    std::ofstream file(m_path);

    file.write(reinterpret_cast<const char *>(data.data()), data.size());
}

std::string File::read_text() const
{
    std::ifstream file(m_path);
    std::string str;
    std::string file_contents;
    while (std::getline(file, str))
    {
        file_contents += str;
        file_contents.push_back('\n');
    }
    return file_contents;
}
