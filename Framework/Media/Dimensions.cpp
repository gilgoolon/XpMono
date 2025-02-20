#include "Media/Dimensions.hpp"

#include "Exception.hpp"

Dimensions::Dimensions Dimensions::get_dimensions()
{
	const int left_bound = GetSystemMetrics(SM_XVIRTUALSCREEN);
	const int top_bound = GetSystemMetrics(SM_YVIRTUALSCREEN);
	const int right_bound = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	const int bottom_bound = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	return {left_bound, top_bound, right_bound, bottom_bound};
}
