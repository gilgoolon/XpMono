#pragma once
#include "IRequest.hpp"

class KeepAliveRequest final : public IRequest
{
public:
	explicit KeepAliveRequest(uint32_t liver_id);
	~KeepAliveRequest() override = default;
	KeepAliveRequest(const KeepAliveRequest&) = delete;
	KeepAliveRequest& operator=(const KeepAliveRequest&) = delete;
	KeepAliveRequest(KeepAliveRequest&&) = delete;
	KeepAliveRequest& operator=(KeepAliveRequest&&) = delete;

	[[nodiscard]] Buffer data() const override;
};
