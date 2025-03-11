#pragma once
#include "FigApi.hpp"
#include "IProduct.hpp"

class FigProduct final : public IProduct
{
public:
	explicit FigProduct(const ICommand::Ptr& command,
	                    Fig::FigId fig_id,
	                    Fig::OperationId operation_id,
	                    Buffer data);
	~FigProduct() override = default;
	FigProduct(const FigProduct&) = delete;
	FigProduct& operator=(const FigProduct&) = delete;
	FigProduct(FigProduct&&) = delete;
	FigProduct& operator=(FigProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	Fig::FigId m_fig_id;
	Fig::OperationId m_operation_id;
	Buffer m_data;
};
