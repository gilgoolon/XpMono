#pragma once
#include "Utils/Buffer.hpp"

#include <memory>

class IInputStream
{
public:
	using Ptr = std::shared_ptr<IInputStream>;

	explicit IInputStream() = default;
	virtual ~IInputStream() = default;

	[[nodiscard]] virtual Buffer read(uint32_t size) const = 0;
};
