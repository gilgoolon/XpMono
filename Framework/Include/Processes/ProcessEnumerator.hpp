#pragma once
#include "ScopedHandle.hpp"
#include "UnopenedProcess.hpp"

class ProcessEnumerator final
{
public:
	explicit ProcessEnumerator();
	~ProcessEnumerator() = default;
	ProcessEnumerator(const ProcessEnumerator&) = delete;
	ProcessEnumerator& operator=(const ProcessEnumerator&) = delete;
	ProcessEnumerator(ProcessEnumerator&&) = delete;
	ProcessEnumerator& operator=(ProcessEnumerator&&) = delete;

	[[nodiscard]] static std::vector<UnopenedProcess::Ptr> get_all();

private:
	[[nodiscard]] static HANDLE create_processes_snapshot();
	void retrieve_first() const;
	void retrieve_next() const;
	[[nodiscard]] bool is_skippable_process(const UnopenedProcess& process) const;

public:
	[[nodiscard]] UnopenedProcess::Ptr next();
	[[nodiscard]] bool has_next() const;

private:
	ScopedHandle m_snapshot_handle;
	mutable UnopenedProcess::Ptr m_next_result;
};
