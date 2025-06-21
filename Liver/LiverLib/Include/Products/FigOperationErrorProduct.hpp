#pragma once
#include "FigApi.hpp"
#include "IProduct.hpp"

class FigOperationErrorProduct final : public IProduct
{
public:
	explicit FigOperationErrorProduct(const ICommand::Ptr& command,
	                                  Fig::FigId fig_id,
	                                  Fig::OperationId operation_id,
	                                  Fig::OperationType operation_type,
	                                  Fig::FigCode fig_code,
	                                  Fig::FigSpecificCode specific_code);
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
	Fig::OperationType m_operation_type;
	Fig::FigCode m_fig_code;
	Fig::FigSpecificCode m_specific_code;
};
