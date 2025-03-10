#pragma once
#include "FigApi.hpp"
#include "IProduct.hpp"

class FigErrorProduct final : public IProduct
{
public:
	explicit FigErrorProduct(const ICommand::Ptr& command, Fig::FigId fig_id, Fig::FigSpecificCode code);
	~FigErrorProduct() override = default;
	FigErrorProduct(const FigErrorProduct&) = delete;
	FigErrorProduct& operator=(const FigErrorProduct&) = delete;
	FigErrorProduct(FigErrorProduct&&) = delete;
	FigErrorProduct& operator=(FigErrorProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	Fig::FigId m_fig_id;
	Fig::FigSpecificCode m_code;
};
