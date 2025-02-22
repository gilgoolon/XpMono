#pragma once
#include "IResponse.hpp"

class SendRandomResponse final : public IResponse
{
public:
	explicit SendRandomResponse(uint32_t value);
	~SendRandomResponse() override = default;
	SendRandomResponse(const SendRandomResponse&) = delete;
	SendRandomResponse& operator=(const SendRandomResponse&) = delete;
	SendRandomResponse(SendRandomResponse&&) = delete;
	SendRandomResponse& operator=(SendRandomResponse&&) = delete;

	[[nodiscard]] Type type() const override;

private:
	uint32_t m_value;
};
