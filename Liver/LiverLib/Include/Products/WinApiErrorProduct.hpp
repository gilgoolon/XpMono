#pragma once
#include "ErrorProduct.hpp"

class WinApiErrorProduct final : public ErrorProduct
{
public:
	explicit WinApiErrorProduct(const ICommand::Ptr& command, ErrorCode error_code, uint32_t win_api_error);
	~WinApiErrorProduct() override = default;
	WinApiErrorProduct(const WinApiErrorProduct&) = delete;
	WinApiErrorProduct& operator=(const WinApiErrorProduct&) = delete;
	WinApiErrorProduct(WinApiErrorProduct&&) = delete;
	WinApiErrorProduct& operator=(WinApiErrorProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	uint32_t m_win_api_error;
};
