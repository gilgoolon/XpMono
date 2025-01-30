#pragma once
#include <memory>
#include <Windows.h>

class CriticalSection final
{
public:
	explicit CriticalSection();
	~CriticalSection();
	CriticalSection(const CriticalSection&) = delete;
	CriticalSection& operator=(const CriticalSection&) = delete;
	CriticalSection(CriticalSection&&) = delete;
	CriticalSection& operator=(CriticalSection&&) = delete;

private:
	[[nodiscard]] static CRITICAL_SECTION create_critical_section();

	struct Unlocker final
	{
		void operator()(CRITICAL_SECTION* section) const;
	};

public:
	using Acquired = std::unique_ptr<CRITICAL_SECTION, Unlocker>;
	[[nodiscard]] Acquired acquire();

private:
	CRITICAL_SECTION m_section;
};
