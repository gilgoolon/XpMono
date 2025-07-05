#include "DisableHandler.hpp"

#include "Products/TextTypedProduct.hpp"
#include "Registry/RegistryManager.hpp"

void DisableHandler::run()
{
	constexpr DWORD value_to_set = 0;

	RegistryManager::set_key_value(
		RegistryPaths::Microsoft::CLIPBOARD,
		L"EnableClipboardHistory",
		reinterpret_cast<const BYTE*>(&value_to_set),
		RegistryValueType::UINT32
	);

	append(std::make_unique<TextTypedProduct>(L"Disable Clipboard successfully"));
	finished();
}
