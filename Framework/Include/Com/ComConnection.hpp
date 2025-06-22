#pragma once
#include "ComReleaser.hpp"
#include "ComResult.hpp"
#include "ComRuntime.hpp"

#include <string>
#include <vector>

class ComConnection final
{
public:
	static constexpr auto WMI_ROOT_NAMESPACE = L"ROOT\\CimV2";
	explicit ComConnection(const std::wstring& namespace_path = WMI_ROOT_NAMESPACE);
	~ComConnection() = default;
	ComConnection(const ComConnection&) = delete;
	ComConnection& operator=(const ComConnection&) = delete;
	ComConnection(ComConnection&&) = delete;
	ComConnection& operator=(ComConnection&&) = delete;

	[[nodiscard]] std::vector<std::unique_ptr<ComResult>> query(const std::wstring& query) const;

private:
	[[nodiscard]] IWbemLocator* create_locator();
	[[nodiscard]] IWbemServices* connect_server(IWbemLocator* locator, const std::wstring& namespace_path);

	ComRuntime m_runtime;
	ComReleaser m_locator;
	ComReleaser m_services;
};
