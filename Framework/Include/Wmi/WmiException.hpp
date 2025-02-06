#pragma once
#include "Exception.hpp"

class WmiException final : public Exception
{
public:
	explicit WmiException(ErrorCode error_code, HRESULT hresult);
	~WmiException() override = default;
	WmiException(const WmiException&) = delete;
	WmiException& operator=(const WmiException&) = delete;
	WmiException(WmiException&&) = delete;
	WmiException& operator=(WmiException&&) = delete;

	[[nodiscard]] HRESULT hresult() const;
	[[nodiscard]] std::string message() const;

private:
	HRESULT m_hresult;
};
