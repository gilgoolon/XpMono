#pragma once
#include "IInputStream.hpp"
#include "IOutputStream.hpp"

class IIOStream : public IInputStream, public IOutputStream
{
public:
	using Ptr = std::shared_ptr<IIOStream>;

	explicit IIOStream() = default;
	~IIOStream() override = default;
	IIOStream(const IIOStream&) = default;
	IIOStream& operator=(const IIOStream&) = default;
	IIOStream(IIOStream&&) = default;
	IIOStream& operator=(IIOStream&&) = default;
};
