#pragma once
#include "ErrorCode.hpp"
#include "IProduct.hpp"

class ErrorProduct final : public IProduct
{
public:
	explicit ErrorProduct(const ICommand::Ptr& command, ErrorCode code);
	~ErrorProduct() override = default;
	ErrorProduct(const ErrorProduct&) = delete;
	ErrorProduct& operator=(const ErrorProduct&) = delete;
	ErrorProduct(ErrorProduct&&) = delete;
	ErrorProduct& operator=(ErrorProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	ErrorCode m_code;
};
