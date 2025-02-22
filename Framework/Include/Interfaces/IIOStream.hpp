#pragma once
#include "IInputStream.hpp"
#include "IOutputStream.hpp"

class IIOStream : public IInputStream, public IOutputStream
{
public:
	using Ptr = std::shared_ptr<IIOStream>;

	explicit IIOStream() = default;
	~IIOStream() override = default;
};
