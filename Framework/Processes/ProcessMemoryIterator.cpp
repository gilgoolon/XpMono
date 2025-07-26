#include "Processes/ProcessMemoryIterator.hpp"

#include "Exception.hpp"

ProcessMemoryIterator::ProcessMemoryIterator(const Process& process, const RegionFilter& filter) :
	m_process(process),
	m_filter(filter)
{
	retrieve_first();
}

void* ProcessMemoryIterator::next() const
{
	if (!has_next())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	void* result = m_next_address;
	m_next_address = nullptr;
	return result;
}

bool ProcessMemoryIterator::has_next() const
{
	if (m_next_address != nullptr)
	{
		return true;
	}
	retrieve_next();
	return m_next_address != nullptr;
}

void ProcessMemoryIterator::retrieve_first() const
{
	LPVOID constexpr first_region_address = nullptr;
	m_current_region_info = m_process.get_region_info(first_region_address);

	while (m_current_region_info.State != m_filter.state ||
		m_current_region_info.Type != m_filter.type ||
		m_current_region_info.Protect != m_filter.protect)
	{
		next_region();
		if (m_current_region_info.RegionSize == 0)
		{
			throw WinApiException(ErrorCode::FAILED_PROCESS_MEMORY_ITERATOR_CREATE);
		}
	}

	m_next_address = m_current_region_info.BaseAddress;
}

void ProcessMemoryIterator::retrieve_next() const
{
	if (m_next_address < reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_current_region_info.BaseAddress) +
		m_current_region_info.RegionSize))
	{
		m_next_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_next_address) + 1);
		return;
	}

	next_region();
	if (m_current_region_info.RegionSize == 0)
	{
		m_next_address = nullptr;
	}
}

void ProcessMemoryIterator::next_region() const
{
	auto next_region_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_current_region_info.BaseAddress) +
		m_current_region_info.RegionSize);
	m_current_region_info = m_process.get_region_info(next_region_address);

	do
	{
		next_region_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_current_region_info.BaseAddress) +
			m_current_region_info.RegionSize);

		try
		{
			m_current_region_info = m_process.get_region_info(next_region_address);
		}
		catch (const Exception& e)
		{
			if (e.code() == ErrorCode::OUT_OF_BOUNDS)
			{
				m_current_region_info.RegionSize = 0;
				return;
			}
			throw;
		}
	}
	while (
		m_current_region_info.State != m_filter.state ||
		m_current_region_info.Type != m_filter.type ||
		m_current_region_info.Protect != m_filter.protect
	);
}
