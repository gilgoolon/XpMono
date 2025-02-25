#pragma once
#include "IResponse.hpp"
#include "Interfaces/IInputStream.hpp"

namespace ResponseFactory
{
[[nodiscard]] IResponse::Ptr create(const IInputStream::Ptr& input);
}
