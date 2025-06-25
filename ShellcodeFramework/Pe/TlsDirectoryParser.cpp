#include "Pe/TlsDirectoryParser.hpp"

#include "Trace.hpp"
#include "Pe/Pe.hpp"

bool Pe::parse_tls_directory(const void* module, TlsDirectory& tls_directory)
{
	const IMAGE_DATA_DIRECTORY* optional_tls_directory = get_data_directory(module, IMAGE_DIRECTORY_ENTRY_TLS);
	if (optional_tls_directory == nullptr)
	{
		TRACE(L"failed to parse PE headers");
		return false;
	}

	const auto raw_tls_directory = reinterpret_cast<const IMAGE_TLS_DIRECTORY*>(static_cast<const uint8_t*>(module) +
		optional_tls_directory->VirtualAddress);

	TlsDirectory parsed_tls_directory{};

	parsed_tls_directory.data = View{
		.address = reinterpret_cast<
			void*>(raw_tls_directory->StartAddressOfRawData),
		.size = raw_tls_directory->EndAddressOfRawData - raw_tls_directory->StartAddressOfRawData + 1
	};
	parsed_tls_directory.index_address = reinterpret_cast<uint32_t*>(raw_tls_directory->AddressOfIndex);
	parsed_tls_directory.callbacks = reinterpret_cast<TlsCallback*>(raw_tls_directory->AddressOfCallBacks);
	parsed_tls_directory.zeros_padding = raw_tls_directory->SizeOfZeroFill;

	tls_directory = parsed_tls_directory;

	return true;
}
