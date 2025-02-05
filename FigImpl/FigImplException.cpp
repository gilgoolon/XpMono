#include "FigImplException.hpp"

FigImplException::FigImplException(const Fig::FigCode code, const Fig::FigSpecificCode specific_code):
	m_code(code),
	m_specific_code(specific_code)
{
}

Fig::FigCode FigImplException::code() const
{
	return m_code;
}

Fig::FigSpecificCode FigImplException::specific_code() const
{
	return m_specific_code;
}
