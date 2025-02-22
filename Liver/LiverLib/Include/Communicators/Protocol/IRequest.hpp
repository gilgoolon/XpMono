#pragma once
#include "Interfaces/ISerializable.hpp"

#include <memory>

class IRequest : public ISerializable
{
public:
	enum class Type : uint32_t
	{
		KEEP_ALIVE = 0,
	};

	using Ptr = std::unique_ptr<IRequest>;

	explicit IRequest() = default;
	~IRequest() override = default;
	IRequest(const IRequest&) = delete;
	IRequest& operator=(const IRequest&) = delete;
	IRequest(IRequest&&) = delete;
	IRequest& operator=(IRequest&&) = delete;
};
