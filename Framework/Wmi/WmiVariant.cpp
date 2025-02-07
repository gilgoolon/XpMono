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

std::wstring WmiVariant::wstring() const
{
	return m_variant.bstrVal ? m_variant.bstrVal : L"";
}

bool WmiVariant::boolean() const
{
	return m_variant.boolVal != VARIANT_FALSE;
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
	SYSTEMTIME system_time{};
	if (VariantTimeToSystemTime(m_variant.date, &system_time) != TRUE)
	{
		throw WinApiException(ErrorCode::FAILED_WMI_VARIANT_TIME_CONVERSION);
	}
	return Time::to_datetime(system_time);
}
