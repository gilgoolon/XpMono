#include "Com/ComVariant.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

ComVariant::ComVariant():
	m_variant{}
{
	VariantInit(&m_variant);
}

ComVariant::~ComVariant()
{
	try
	{
		if (FAILED(VariantClear(&m_variant)))
		{
			TRACE(L"failed to clear com variant");
		}
	}
	catch (...)
	{
		TRACE(L"failed to clear com variant");
	}
}

VARIANT* ComVariant::get()
{
	return &m_variant;
}

VARTYPE ComVariant::type() const
{
	return m_variant.vt;
}

bool ComVariant::has_value() const
{
	return m_variant.vt != VT_NULL;
}

std::wstring ComVariant::wstring() const
{
	return m_variant.bstrVal ? m_variant.bstrVal : L"";
}

bool ComVariant::boolean() const
{
	return m_variant.boolVal != VARIANT_FALSE;
}

uint64_t ComVariant::uint64() const
{
	return m_variant.ullVal;
}

uint32_t ComVariant::uint32() const
{
	return m_variant.ulVal;
}

uint16_t ComVariant::uint16() const
{
	return m_variant.uiVal;
}

uint8_t ComVariant::uint8() const
{
	return m_variant.bVal;
}

int64_t ComVariant::int64() const
{
	return m_variant.llVal;
}

int32_t ComVariant::int32() const
{
	return m_variant.lVal;
}

int16_t ComVariant::int16() const
{
	return m_variant.iVal;
}

int8_t ComVariant::int8() const
{
	return m_variant.cVal;
}

Time::Datetime ComVariant::datetime() const
{
	const std::wstring date_as_string = wstring();
	static constexpr uint32_t VARIANT_TIME_AS_STRING_LENGTH = 20;
	if (date_as_string.size() < VARIANT_TIME_AS_STRING_LENGTH)
	{
		throw WinApiException(ErrorCode::FAILED_COM_VARIANT_TIME_CONVERSION);
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
