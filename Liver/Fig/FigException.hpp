#pragma once
#include "Exception.hpp"

#include <FigApi.hpp>

class FigException final : public Exception
{
public:
	explicit FigException(ErrorCode code,
	                      Fig::FigCode fig_code,
	                      Fig::FigSpecificCode fig_specific_code = Fig::FIG_SPECIFIC_CODE_RESERVED);
	~FigException() override = default;
	FigException(const FigException&) = delete;
	FigException& operator=(const FigException&) = delete;
	FigException(FigException&&) = delete;
	FigException& operator=(FigException&&) = delete;

	[[nodiscard]] Fig::FigCode fig_code() const;
	[[nodiscard]] Fig::FigSpecificCode fig_specific_code() const;

private:
	Fig::FigCode m_fig_code;
	Fig::FigSpecificCode m_fig_specific_code;
};
