#pragma once
#include "FigApi.hpp"
#include "IProduct.hpp"

class FigOperationErrorProduct final : public IProduct
{
public:
	explicit FigOperationErrorProduct(const ICommand::Ptr& command,
	                                  Fig::FigId fig_id,
	                                  Fig::OperationId operation_id,
	                                  Fig::FigSpecificCode code);
	~FigOperationErrorProduct() override = default;
	FigOperationErrorProduct(const FigOperationErrorProduct&) = delete;
	FigOperationErrorProduct& operator=(const FigOperationErrorProduct&) = delete;
	FigOperationErrorProduct(FigOperationErrorProduct&&) = delete;
	FigOperationErrorProduct& operator=(FigOperationErrorProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	Fig::FigId m_fig_id;
	Fig::OperationId m_operation_id;
	Fig::FigSpecificCode m_code;
};
