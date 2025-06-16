#include "Crypto/Protection.hpp"

#include "Exception.hpp"
#include "Processes/ScopedLocalHeapAllocation.hpp"

#include <Windows.h>

Buffer Crypto::unprotect_data(const Buffer& data)
{
	if (data.empty())
	{
		return {};
	}

	Buffer working_data = data;

	DATA_BLOB input_blob;
	input_blob.pbData = working_data.data();
	input_blob.cbData = static_cast<DWORD>(working_data.size());

	DATA_BLOB output_blob = {};

	static constexpr DWORD NO_UI = CRYPTPROTECT_UI_FORBIDDEN;

	const BOOL result = CryptUnprotectData(
		&input_blob,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		NO_UI,
		&output_blob
	);

	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_CRYPT_UNPROTECT_DATA);
	}

	const ScopedLocalHeapAllocation freer(output_blob.pbData);

	Buffer unprotected_data = {output_blob.pbData, output_blob.pbData + output_blob.cbData};

	return unprotected_data;
}
