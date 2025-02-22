#pragma once
#include "Utils/Buffer.hpp"

#include <memory>

class IInputStream
{
public:
	using Ptr = std::shared_ptr<IInputStream>;

	explicit IInputStream() = default;
	virtual ~IInputStream() = default;
	IInputStream(const IInputStream&) = delete;
	IInputStream& operator=(const IInputStream&) = delete;
	IInputStream(IInputStream&&) = delete;
	IInputStream& operator=(IInputStream&&) = delete;

	[[nodiscard]] virtual Buffer read(uint32_t size) = 0;
};
