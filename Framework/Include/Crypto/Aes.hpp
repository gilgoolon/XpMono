#pragma once
#include "Utils/Buffer.hpp"

#include <vector>

namespace Aes
{
enum class Mode : uint32_t
{
	CBC = 0,
	GCM,
};

Buffer encrypt(const Buffer& data,
               const Buffer& key,
               const Buffer& iv = {},
               Mode mode = Mode::CBC);

Buffer decrypt(const Buffer& data,
               const Buffer& key,
               const Buffer& iv = {},
               Mode mode = Mode::CBC);
}
