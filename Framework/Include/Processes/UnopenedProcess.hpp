#pragma once
#include <memory>

#include "Process.hpp"

class UnopenedProcess final
{
public:
	using Ptr = std::unique_ptr<UnopenedProcess>;

	explicit UnopenedProcess(uint32_t pid);
	~UnopenedProcess() = default;
	UnopenedProcess(const UnopenedProcess&) = delete;
	UnopenedProcess& operator=(const UnopenedProcess&) = delete;
	UnopenedProcess(UnopenedProcess&&) = delete;
	UnopenedProcess& operator=(UnopenedProcess&&) = delete;

	[[nodiscard]] uint32_t get_pid() const;

	[[nodiscard]] Process::Ptr open() const;

	[[nodiscard]] bool is_ignored() const;

private:
	uint32_t m_pid;
};
