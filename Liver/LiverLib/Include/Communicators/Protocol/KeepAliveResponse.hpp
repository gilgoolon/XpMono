#pragma once
#include "IResponse.hpp"

class KeepAliveResponse final : public IResponse
{
public:
	explicit KeepAliveResponse() = default;
	~KeepAliveResponse() override = default;
	KeepAliveResponse(const KeepAliveResponse&) = delete;
	KeepAliveResponse& operator=(const KeepAliveResponse&) = delete;
	KeepAliveResponse(KeepAliveResponse&&) = delete;
	KeepAliveResponse& operator=(KeepAliveResponse&&) = delete;

	[[nodiscard]] Type type() const override
	{
		return Type::KEEP_ALIVE;
	}
};
