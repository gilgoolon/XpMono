#pragma once
#include "IResponse.hpp"
#include "Interfaces/IInputStream.hpp"

namespace ResponseFactory
{
[[nodiscard]] IResponse::Ptr create(IInputStream::Ptr input);
};
