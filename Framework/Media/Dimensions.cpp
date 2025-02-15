#include "Media/Dimensions.hpp"

#include "Exception.hpp"
#include "Synchronization/CriticalSection.hpp"

static int get_system_metrics(const int metric)
{
	const int result = GetSystemMetrics(metric);
	static constexpr int GSM_FAILED = 0;
	if (result == GSM_FAILED)
	{
		throw WinApiException(ErrorCode::FAILED_GET_DIMENSIONS);
	}
	return result;
}

Dimensions::Dimensions Dimensions::get_dimensions()
{
	const int left_bound = get_system_metrics(SM_XVIRTUALSCREEN);
	const int top_bound = get_system_metrics(SM_YVIRTUALSCREEN);
	const int right_bound = get_system_metrics(SM_CXVIRTUALSCREEN);
	const int bottom_bound = get_system_metrics(SM_CYVIRTUALSCREEN);
	return {left_bound, top_bound, right_bound, bottom_bound};
}
