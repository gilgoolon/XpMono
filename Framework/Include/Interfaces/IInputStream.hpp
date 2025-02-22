#pragma once
#include "Exception.hpp"
#include "Utils/Buffer.hpp"

#include <memory>

class IInputStream
{
public:
	using Ptr = std::shared_ptr<IInputStream>;

	explicit IInputStream() = default;
	virtual ~IInputStream() = default;
	IInputStream(const IInputStream&) = default;
	IInputStream& operator=(const IInputStream&) = default;
	IInputStream(IInputStream&&) = default;
	IInputStream& operator=(IInputStream&&) = default;

	[[nodiscard]] virtual Buffer read(uint32_t size) const = 0;

	template <typename T>
	[[nodiscard]] T read() const
	{
		Buffer data = read(sizeof(T));
		if (data.size() != sizeof(T))
		{
			throw Exception(ErrorCode::NOT_ENOUGH_DATA);
		}
		return *reinterpret_cast<const T*>(data.data());
	}
};
