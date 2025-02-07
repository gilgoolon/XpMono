#include "Wmi/WmiResult.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Utils/Strings.hpp"
#include "Wmi/WmiException.hpp"
#include "Wmi/WmiVariant.hpp"

WmiResult::WmiResult(IWbemClassObject* const object):
	m_object(object)
{
	m_object->AddRef();
}

std::wstring WmiResult::get_formatted_property(const std::wstring& property_name)
{
	static constexpr long RESERVED = 0;
	WmiVariant variant;
	CIMTYPE value_type;
	static constexpr long* DONT_OUT_FLAVOR_ORIGIN = nullptr;
	const HRESULT hresult = static_cast<IWbemClassObject*>(m_object.get())->Get(
		property_name.c_str(),
		RESERVED,
		variant.get(),
		&value_type,
		DONT_OUT_FLAVOR_ORIGIN
	);
	if (FAILED(hresult))
	{
		throw WmiException(ErrorCode::FAILED_WMI_GET_PROPERTY, hresult);
	}
	switch (value_type)
	{
	case VT_BSTR:
		return variant.wstring();
	case VT_BOOL:
		return variant.boolean() ? L"True" : L"False";
	case VT_UI1:
		return std::to_wstring(variant.uint8());
	case VT_UI2:
		return std::to_wstring(variant.uint16());
	case VT_UI4:
		return std::to_wstring(variant.uint32());
	case VT_UI8:
		return std::to_wstring(variant.uint64());
	case VT_I1:
		return std::to_wstring(variant.int8());
	case VT_I2:
		return std::to_wstring(variant.int16());
	case VT_I4:
		return std::to_wstring(variant.int32());
	case VT_I8:
		return std::to_wstring(variant.int64());
	case VT_DATE:
		return Strings::to_wstring(Time::to_string(variant.datetime()));
	default:
		TRACE(L"unsupported variant type")
		throw Exception(ErrorCode::WMI_UNSUPPORTED_VARIANT_TYPE);
	}
}
