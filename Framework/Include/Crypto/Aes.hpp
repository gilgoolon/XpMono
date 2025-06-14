#pragma once
#include <vector>

namespace Aes
{
enum class Mode : uint32_t
{
	CBC = 0,
	GCM,
};

void encrypt(std::vector<uint8_t>& data,
             const std::vector<uint8_t>& key,
             const std::vector<uint8_t>& iv = {},
             Mode mode = Mode::CBC);

void decrypt(std::vector<uint8_t>& data,
             const std::vector<uint8_t>& key,
             const std::vector<uint8_t>& iv = {},
             Mode mode = Mode::CBC);
}
