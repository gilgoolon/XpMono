#pragma once
#include "Exception.hpp"

class ComException final : public Exception
{
public:
	explicit ComException(ErrorCode error_code, HRESULT hresult);
	~ComException() override = default;
	ComException(const ComException&) = delete;
	ComException& operator=(const ComException&) = delete;
	ComException(ComException&&) = delete;
	ComException& operator=(ComException&&) = delete;

	[[nodiscard]] HRESULT hresult() const;
	[[nodiscard]] std::string message() const;

private:
	HRESULT m_hresult;
};
