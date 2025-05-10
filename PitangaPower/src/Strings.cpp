#include "Strings.hpp"

std::string Strings::as_string(const std::vector<uint8_t> &buffer)
{
    return std::string{reinterpret_cast<const char*>(buffer.data()), reinterpret_cast<const char*>(buffer.data() + buffer.size())};
}

std::vector<uint8_t> Strings::as_buffer(const std::string &string)
{
    return std::vector<uint8_t>{reinterpret_cast<const uint8_t *>(string.data()), reinterpret_cast<const uint8_t *>(string.data() + string.size())};
}

std::vector<std::string> Strings::split(const std::string &str, const char delimiter)
{
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type end;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }
    result.emplace_back(str.substr(start));
    return result;
}
