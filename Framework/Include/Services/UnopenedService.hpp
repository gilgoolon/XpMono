#pragma once
#include <memory>
#include <string>

#include "Service.hpp"

class UnopenedService final
{
public:
	using Ptr = std::unique_ptr<UnopenedService>;

	explicit UnopenedService(const std::wstring& name);
	~UnopenedService() = default;
	UnopenedService(const UnopenedService&) = delete;
	UnopenedService& operator=(const UnopenedService&) = delete;
	UnopenedService(UnopenedService&&) = delete;
	UnopenedService& operator=(UnopenedService&&) = delete;

	[[nodiscard]] std::wstring get_name() const;

	[[nodiscard]] Service::Ptr open() const;

private:
	std::wstring m_name;
};
