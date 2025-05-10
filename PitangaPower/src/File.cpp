#include "File.hpp"

#include <filesystem/fat.h>
#include <pico/stdlib.h>

File::File(std::filesystem::path path):
    m_fd(create_file(std::move(path)))
{
}

File::~File()
{
    try
    {
        close(m_fd);
    }
    catch(...)
    {
    }
    
}

int File::create_file(std::filesystem::path path)
{
    const int result = open(path.string().c_str(), O_RDWR | O_CREAT);
    static constexpr int OPEN_FAILED = -1;
    if (result == OPEN_FAILED)
    {
        throw std::runtime_error("failed to open file");
    }
    return result;
}

void File::write(const std::vector<uint8_t> &data)
{
    const int result = ::write(m_fd, reinterpret_cast<const char*>(data.data()), data.size());
    static constexpr int WRITE_FAILED = -1;
    if (result == WRITE_FAILED)
    {
        throw std::runtime_error("failed to write file");
    }
}

std::vector<uint8_t> File::read_all() const
{
    std::vector<uint8_t> buffer;
    static constexpr size_t CHUNK_SIZE = 1024;
    size_t old_size = 0;

    do {
        old_size = buffer.size();
        buffer.resize(old_size + CHUNK_SIZE);

        const int result = ::read(m_fd, reinterpret_cast<char *>(buffer.data()) + old_size, CHUNK_SIZE);

        static constexpr int READ_FAILED = -1;
        if (result == READ_FAILED)
        {
            throw std::runtime_error("failed to read file");
        }

        buffer.resize(old_size + result);
    } while(old_size != buffer.size());

    return buffer;
}
