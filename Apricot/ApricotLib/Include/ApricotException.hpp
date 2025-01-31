#pragma once
#include "ApricotCode.hpp"
#include "Exception.hpp"

class ApricotException final : public Exception
{
public:
	explicit ApricotException(ErrorCode code, ApricotCode apricot_code);
	~ApricotException() override = default;
	ApricotException(const ApricotException&) = delete;
	ApricotException& operator=(const ApricotException&) = delete;
	ApricotException(ApricotException&&) = delete;
	ApricotException& operator=(ApricotException&&) = delete;

	[[nodiscard]] ApricotCode apricot_code() const;

private:
	ApricotCode m_apricot_code;
};
