#include "FigException.hpp"

FigException::FigException(const ErrorCode code,
                           const Fig::FigCode fig_code,
                           const Fig::FigSpecificCode fig_specific_code):
	Exception(code),
	m_fig_code(fig_code),
	m_fig_specific_code(fig_specific_code)
{
}

Fig::FigCode FigException::fig_code() const
{
	return m_fig_code;
}

Fig::FigSpecificCode FigException::fig_specific_code() const
{
	return m_fig_specific_code;
}
