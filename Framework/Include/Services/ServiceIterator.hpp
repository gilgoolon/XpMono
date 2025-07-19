#pragma once
#include "UnopenedService.hpp"
#include "Media/MediaFoundation/Attributes.hpp"

#include <vector>

class ServiceIterator final
{
public:
	explicit ServiceIterator();
	~ServiceIterator() = default;
	ServiceIterator(const ServiceIterator&) = delete;
	ServiceIterator& operator=(const ServiceIterator&) = delete;
	ServiceIterator(ServiceIterator&&) = delete;
	ServiceIterator& operator=(ServiceIterator&&) = delete;

	[[nodiscard]] static std::vector<UnopenedService::Ptr> get_all();
	[[nodiscard]] UnopenedService::Ptr next() const;
	[[nodiscard]] bool has_next() const;

private:
	[[nodiscard]] static SC_HANDLE get_service_manager_handle();
	void load_services() const;
	void retrieve_first() const;
	void retrieve_next() const;

	mutable std::vector<ENUM_SERVICE_STATUS_PROCESSW> m_services;
	mutable UnopenedService::Ptr m_next_result;
	mutable size_t m_next_index;
};
