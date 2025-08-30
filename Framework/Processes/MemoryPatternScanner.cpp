#include "Processes/MemoryPatternScanner.hpp"
#include "Exception.hpp"

MemoryPatternScanner::MemoryPatternScanner(const Process& process, const std::vector<MemoryFilter>& filters) :
	m_process(process),
	m_filters(filters),
	m_memory_iterator(process)
{
	retrieve_first();
}

std::vector<void*> MemoryPatternScanner::get_all() const
{
	std::vector<void*> results;
	while (has_next())
	{
		results.push_back(next());
	}
	return results;
}

void* MemoryPatternScanner::next() const
{
	if (!has_next())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	void* result = m_next_result;
	m_next_result = nullptr;
	return std::move(result);
}

bool MemoryPatternScanner::has_next() const
{
	if (m_next_result != nullptr)
	{
		return true;
	}
	retrieve_next();
	return m_next_result != nullptr;
}

void MemoryPatternScanner::retrieve_first() const
{
	retrieve_next();
	if (m_next_result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_MEMORY_PATTERN_SCANNER_NO_MATCH);
	}
}

void MemoryPatternScanner::retrieve_next() const
{
	for (const void* memory_address = m_memory_iterator.next(); memory_address != nullptr; memory_address =
	     m_memory_iterator.next())
	{
		bool is_matching = true;

		try
		{
			for (const auto& filter : m_filters)
			{
				if (!filter(m_process, reinterpret_cast<uintptr_t>(memory_address)))
				{
					is_matching = false;
					break;
				}
			}
		}
		catch (...)
		{
			continue;
		}

		if (is_matching)
		{
			m_next_result = const_cast<void*>(memory_address);
			return;
		}
	}

	m_next_result = nullptr;
}
