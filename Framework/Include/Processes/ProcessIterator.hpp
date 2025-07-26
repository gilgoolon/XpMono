#pragma once
#include "ScopedHandle.hpp"
#include "UnopenedProcess.hpp"

class ProcessIterator final
{
public:
	explicit ProcessIterator();
	~ProcessIterator() = default;
	ProcessIterator(const ProcessIterator&) = delete;
	ProcessIterator& operator=(const ProcessIterator&) = delete;
	ProcessIterator(ProcessIterator&&) = delete;
	ProcessIterator& operator=(ProcessIterator&&) = delete;

	[[nodiscard]] static std::vector<UnopenedProcess::Ptr> get_all();

private:
	[[nodiscard]] static HANDLE create_processes_snapshot();
	void retrieve_first() const;
	void retrieve_next() const;

public:
	[[nodiscard]] UnopenedProcess::Ptr next() const;
	[[nodiscard]] bool has_next() const;

private:
	ScopedHandle m_snapshot_handle;
	mutable UnopenedProcess::Ptr m_next_result;
};
