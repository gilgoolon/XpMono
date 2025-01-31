#include "ApricotException.hpp"

ApricotException::ApricotException(const ErrorCode code, const ApricotCode apricot_code) :
	Exception(code),
	m_apricot_code(apricot_code)
{
}

ApricotCode ApricotException::apricot_code() const
{
	return m_apricot_code;
}
