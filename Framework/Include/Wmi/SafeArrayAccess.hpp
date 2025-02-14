#pragma once
#include "WmiException.hpp"
#include "Synchronization/CriticalSection.hpp"

#include <vector>

class SafeArrayAccess final
{
public:
	explicit SafeArrayAccess(SAFEARRAY* safe_array);
	~SafeArrayAccess();
	SafeArrayAccess(const SafeArrayAccess&) = delete;
	SafeArrayAccess& operator=(const SafeArrayAccess&) = delete;
	SafeArrayAccess(SafeArrayAccess&&) = delete;
	SafeArrayAccess& operator=(SafeArrayAccess&&) = delete;

	template <typename T>
	std::vector<T> value() const
	{
		std::vector<T> result;
		if (m_safearray == nullptr)
		{
			return result;
		}
		static constexpr UINT SINGLE_DIM = 1;
		LONG low_bound = 0;
		LONG up_bound = 0;
		HRESULT hresult = SafeArrayGetLBound(m_safearray, SINGLE_DIM, &low_bound);
		if (FAILED(hresult))
		{
			throw WmiException(ErrorCode::FAILED_SAFE_ARRAY_ACCESS, hresult);
		}
		hresult = SafeArrayGetUBound(m_safearray, SINGLE_DIM, &up_bound);
		if (FAILED(hresult))
		{
			throw WmiException(ErrorCode::FAILED_SAFE_ARRAY_ACCESS, hresult);
		}
		auto* const arr = static_cast<T*>(m_data);
		for (LONG i = low_bound; i < up_bound; ++i)
		{
			result.push_back(arr[i]);
		}
		return result;
	}

private:
	SAFEARRAY* m_safearray;
	void* m_data;
};
