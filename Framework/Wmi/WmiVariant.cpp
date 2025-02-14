#include "Wmi/WmiVariant.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

WmiVariant::WmiVariant():
	m_variant{}
{
	VariantInit(&m_variant);
}

WmiVariant::~WmiVariant()
{
	try
	{
		if (FAILED(VariantClear(&m_variant)))
		{
			TRACE(L"failed to clear wmi variant");
		}
	}
	catch (...)
	{
		TRACE(L"failed to clear wmi variant");
	}
}

VARIANT* WmiVariant::get()
{
	return &m_variant;
}

VARTYPE WmiVariant::type() const
{
	return m_variant.vt;
}

std::wstring WmiVariant::wstring() const
{
	return m_variant.bstrVal ? m_variant.bstrVal : L"";
}

bool WmiVariant::boolean() const
{
	return m_variant.boolVal != VARIANT_FALSE;
}

uint64_t WmiVariant::uint64() const
{
	return m_variant.ullVal;
}

uint32_t WmiVariant::uint32() const
{
	return m_variant.ulVal;
}

uint16_t WmiVariant::uint16() const
{
	return m_variant.uiVal;
}

uint8_t WmiVariant::uint8() const
{
	return m_variant.bVal;
}

int64_t WmiVariant::int64() const
{
	return m_variant.llVal;
}

int32_t WmiVariant::int32() const
{
	return m_variant.lVal;
}

int16_t WmiVariant::int16() const
{
	return m_variant.iVal;
}

int8_t WmiVariant::int8() const
{
	return m_variant.cVal;
}

Time::Datetime WmiVariant::datetime() const
{
	const std::wstring date_as_string = wstring();
	static constexpr uint32_t VARIANT_TIME_AS_STRING_LENGTH = 20;
	if (date_as_string.size() < VARIANT_TIME_AS_STRING_LENGTH)
	{
		throw WinApiException(ErrorCode::FAILED_WMI_VARIANT_TIME_CONVERSION);
	}
	SYSTEMTIME result{};
	result.wYear = static_cast<WORD>(std::stoul(date_as_string.substr(0, 4)));
	result.wMonth = static_cast<WORD>(std::stoul(date_as_string.substr(4, 2)));
	result.wDay = static_cast<WORD>(std::stoul(date_as_string.substr(6, 2)));
	result.wHour = static_cast<WORD>(std::stoul(date_as_string.substr(8, 2)));
	result.wMinute = static_cast<WORD>(std::stoul(date_as_string.substr(10, 2)));
	result.wSecond = static_cast<WORD>(std::stoul(date_as_string.substr(12, 2)));
	result.wMilliseconds = static_cast<WORD>(std::stoul(date_as_string.substr(15, 3)));
	return Time::to_datetime(result);
}
