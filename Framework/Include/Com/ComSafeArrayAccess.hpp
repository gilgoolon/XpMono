#pragma once
#include "ComException.hpp"
#include "Synchronization/CriticalSection.hpp"

#include <vector>

class ComSafeArrayAccess final
{
public:
	explicit ComSafeArrayAccess(SAFEARRAY* safe_array);
	~ComSafeArrayAccess();
	ComSafeArrayAccess(const ComSafeArrayAccess&) = delete;
	ComSafeArrayAccess& operator=(const ComSafeArrayAccess&) = delete;
	ComSafeArrayAccess(ComSafeArrayAccess&&) = delete;
	ComSafeArrayAccess& operator=(ComSafeArrayAccess&&) = delete;

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
			throw ComException(ErrorCode::FAILED_COM_SAFE_ARRAY_ACCESS, hresult);
		}
		hresult = SafeArrayGetUBound(m_safearray, SINGLE_DIM, &up_bound);
		if (FAILED(hresult))
		{
			throw ComException(ErrorCode::FAILED_COM_SAFE_ARRAY_ACCESS, hresult);
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
