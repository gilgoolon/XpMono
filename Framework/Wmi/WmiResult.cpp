#include "Wmi/WmiResult.hpp"

#include "Exception.hpp"
#include "Wmi/WmiException.hpp"
#include "Wmi/WmiVariant.hpp"

WmiResult::WmiResult(IWbemClassObject* const object):
	m_object(object)
{
	m_object->AddRef();
}

std::wstring WmiResult::get_property(const std::wstring& property_name)
{
	static constexpr long RESERVED = 0;
	WmiVariant variant;
	static constexpr CIMTYPE* DONT_OUT_CIM_TYPE = nullptr;
	static constexpr long* DONT_OUT_FLAVOR_ORIGIN = nullptr;
	const HRESULT hresult = static_cast<IWbemClassObject*>(m_object.get())->Get(
		property_name.c_str(),
		RESERVED,
		variant.get(),
		DONT_OUT_CIM_TYPE,
		DONT_OUT_FLAVOR_ORIGIN
	);
	if (FAILED(hresult))
	{
		throw WmiException(ErrorCode::FAILED_WMI_GET_PROPERTY, hresult);
	}
	return variant.value();
}
