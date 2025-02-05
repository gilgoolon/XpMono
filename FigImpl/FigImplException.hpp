#pragma once
#include "FigApi.hpp"

class FigImplException final
{
public:
	explicit FigImplException(Fig::FigCode code, Fig::FigSpecificCode specific_code = Fig::FIG_SPECIFIC_CODE_RESERVED);
	~FigImplException() = default;
	FigImplException(const FigImplException&) = delete;
	FigImplException& operator=(const FigImplException&) = delete;
	FigImplException(FigImplException&&) = delete;
	FigImplException& operator=(FigImplException&&) = delete;

	[[nodiscard]] Fig::FigCode code() const;
	[[nodiscard]] Fig::FigSpecificCode specific_code() const;

private:
	Fig::FigCode m_code;
	Fig::FigSpecificCode m_specific_code;
};
