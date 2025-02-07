#include "Wmi/WmiVariant.hpp"

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

std::wstring WmiVariant::value() const
{
	return m_variant.bstrVal ? m_variant.bstrVal : L"";
}
