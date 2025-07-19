#include "Services/UnopenedService.hpp"

UnopenedService::UnopenedService(const std::wstring& name) :
	m_name(name)
{
}

std::wstring UnopenedService::get_name() const
{
	return m_name;
}

Service::Ptr UnopenedService::open() const
{
	return std::make_unique<Service>(m_name);
}
