#pragma once
#include <vector>

namespace Aes
{
void encrypt(std::vector<uint8_t>& data,
             const std::vector<uint8_t>& key,
             const std::vector<uint8_t>& iv = {});

void decrypt(std::vector<uint8_t>& data,
             const std::vector<uint8_t>& key);
}
