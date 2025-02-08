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

std::optional<std::wstring> WmiResult::get_formatted_property(const std::wstring& property_name)
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
		if (hresult == WBEM_E_NOT_FOUND)
		{
			TRACE(L"property not found: ", property_name)
			return {};
		}
		throw WmiException(ErrorCode::FAILED_WMI_GET_PROPERTY, hresult);
	}
	const bool is_array = (value_type & CIM_FLAG_ARRAY) > 0;
	if (is_array)
	{
		value_type &= ~CIM_FLAG_ARRAY;
	}
	switch (value_type)
	{
	case CIM_STRING:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<BSTR>());
		}
		return variant.wstring();
	case CIM_BOOLEAN:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<BOOLEAN>());
		}
		return Strings::to_wstring<BOOLEAN>(variant.boolean());
	case CIM_UINT8:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<uint8_t>());
		}
		return std::to_wstring(variant.uint8());
	case CIM_UINT16:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<uint16_t>());
		}
		return std::to_wstring(variant.uint16());
	case CIM_UINT32:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<uint32_t>());
		}
		return std::to_wstring(variant.uint32());
	case CIM_UINT64:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<uint64_t>());
		}
		return std::to_wstring(variant.uint64());
	case CIM_SINT8:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<int8_t>());
		}
		return std::to_wstring(variant.int8());
	case CIM_SINT16:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<int16_t>());
		}
		return std::to_wstring(variant.int16());
	case CIM_SINT32:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<int32_t>());
		}
		return std::to_wstring(variant.int32());
	case CIM_SINT64:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<int64_t>());
		}
		return std::to_wstring(variant.int64());
	case CIM_DATETIME:
		if (is_array)
		{
			return Strings::to_wstring(variant.array<DATE>());
		}
		return Strings::to_wstring(Time::to_string(variant.datetime()));
	default:
		TRACE(L"unsupported variant type: ", value_type)
		return {};
	}
}
