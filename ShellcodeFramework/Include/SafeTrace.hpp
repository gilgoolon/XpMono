#pragma once

namespace SafeTrace
{
void debug(const wchar_t* message);
};

#ifdef _DEBUG
#define TRACE(str) \
	do { \
	SafeTrace::debug(L"[" __PROJECTW__ L"] " __FILEW__ L":" __FUNCTIONW__ L" - " str); \
	} while (0)
#else
#define TRACE(str) 
#endif
