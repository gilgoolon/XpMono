#pragma once
#include "Utils/Buffer.hpp"

#include <memory>

class IOutputStream
{
public:
	using Ptr = std::shared_ptr<IOutputStream>;

	explicit IOutputStream() = default;
	virtual ~IOutputStream() = default;

	virtual void write(const Buffer& data) const = 0;
};
