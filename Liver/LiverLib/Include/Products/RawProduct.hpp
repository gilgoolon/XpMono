#pragma once
#include "IProduct.hpp"

class RawProduct final : public IProduct
{
public:
	explicit RawProduct(const ICommand::Ptr& command, Buffer data);
	~RawProduct() override = default;
	RawProduct(const RawProduct&) = delete;
	RawProduct& operator=(const RawProduct&) = delete;
	RawProduct(RawProduct&&) = delete;
	RawProduct& operator=(RawProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	Buffer m_data;
};
