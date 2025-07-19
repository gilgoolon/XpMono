#include "EnableHandler.hpp"

#include "Products/TextTypedProduct.hpp"
#include "Registry/RegistryManager.hpp"

void EnableHandler::run()
{
	constexpr DWORD value_to_set = 1;

	RegistryManager::set_key_value(
		RegistryPaths::Microsoft::CLIPBOARD,
		L"EnableClipboardHistory",
		reinterpret_cast<const BYTE*>(&value_to_set),
		RegistryValueType::UINT32
	);

	append(std::make_unique<TextTypedProduct>(L"Enable Clipboard successfully"));
	finished();
}
