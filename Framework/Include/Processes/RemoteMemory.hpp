#pragma once
#include <cstdint>
#include <memory>
#include <vector>

class RemoteMemory
{
	friend class Process;

	explicit RemoteMemory(std::weak_ptr<Process> process, uint32_t size);

public:
	using Ptr = std::unique_ptr<RemoteMemory>;
	~RemoteMemory();
	RemoteMemory(const RemoteMemory&) = delete;
	RemoteMemory& operator=(const RemoteMemory&) = delete;
	RemoteMemory(RemoteMemory&&) = delete;
	RemoteMemory& operator=(RemoteMemory&&) = delete;

	void write(const std::vector<uint8_t>& data) const;

private:
	std::weak_ptr<Process> m_process;
	void* m_base_address;

	[[nodiscard]] static void* allocate_remote_memory(const Process& process, uint32_t size);

	[[nodiscard]] void* get() const;

	friend class HookedFunction;
};
