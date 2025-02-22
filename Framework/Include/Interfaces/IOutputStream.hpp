#pragma once
#include "Utils/Buffer.hpp"

#include <memory>

class IOutputStream
{
public:
	using Ptr = std::shared_ptr<IOutputStream>;

	explicit IOutputStream() = default;
	virtual ~IOutputStream() = default;
	IOutputStream(const IOutputStream&) = delete;
	IOutputStream& operator=(const IOutputStream&) = delete;
	IOutputStream(IOutputStream&&) = delete;
	IOutputStream& operator=(IOutputStream&&) = delete;

	virtual void write(const Buffer& data) = 0;
};
