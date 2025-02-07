#pragma once
#include "WmiReleaser.hpp"
#include "WmiResult.hpp"
#include "WmiRuntime.hpp"

#include <string>
#include <vector>

class WmiConnection final
{
public:
	static constexpr std::wstring_view WMI_ROOT_NAMESPACE = L"ROOT\\CimV2";
	explicit WmiConnection(const std::wstring& namespace_path = std::wstring{WMI_ROOT_NAMESPACE});
	~WmiConnection() = default;
	WmiConnection(const WmiConnection&) = delete;
	WmiConnection& operator=(const WmiConnection&) = delete;
	WmiConnection(WmiConnection&&) = delete;
	WmiConnection& operator=(WmiConnection&&) = delete;

	[[nodiscard]] std::vector<std::unique_ptr<WmiResult>> query(const std::wstring& query) const;

private:
	[[nodiscard]] IWbemLocator* create_locator();
	[[nodiscard]] IWbemServices* connect_server(IWbemLocator* locator, const std::wstring& namespace_path);

	WmiRuntime m_runtime;
	WmiReleaser m_locator;
	WmiReleaser m_services;
};
