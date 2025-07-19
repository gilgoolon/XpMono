#include "CheckStatusHandler.hpp"

#include "Products/TextTypedProduct.hpp"
#include "Registry/RegistryManager.hpp"

void CheckStatusHandler::run()
{
	const RegistryManager::RegistryValue clipboard_status = RegistryManager::get_key_value(
		RegistryPaths::Microsoft::CLIPBOARD,
		L"EnableClipboardHistory",
		RegistryValueFlag::UINT32
	);

	const std::wstring product = std::get<uint32_t>(clipboard_status)
		                             ? L"Clipboard status is enabled"
		                             : L"Clipboard status is disabled";
	append(std::make_unique<TextTypedProduct>(product));

	finished();
}
