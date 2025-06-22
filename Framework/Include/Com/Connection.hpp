#pragma once
#include "Result.hpp"
#include "Runtime.hpp"
#include "Com/Releaser.hpp"

#include <string>
#include <vector>

namespace Com
{
class Connection final
{
public:
	static constexpr auto WMI_ROOT_NAMESPACE = L"ROOT\\CimV2";
	explicit Connection(const std::wstring& namespace_path = WMI_ROOT_NAMESPACE);
	~Connection() = default;
	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;
	Connection(Connection&&) = delete;
	Connection& operator=(Connection&&) = delete;

	[[nodiscard]] std::vector<std::unique_ptr<Result>> query(const std::wstring& query) const;

private:
	[[nodiscard]] IWbemLocator* create_locator();
	[[nodiscard]] IWbemServices* connect_server(IWbemLocator* locator, const std::wstring& namespace_path);

	Runtime m_runtime;
	Releaser m_locator;
	Releaser m_services;
};
}
