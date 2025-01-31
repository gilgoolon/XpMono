#include "Strings.hpp"

bool Strings::equals(const char* str1, const char* str2)
{
	static constexpr char TERMINATOR = '\0';
	while (*str1 != TERMINATOR && *str2 != TERMINATOR)
	{
		if (*str1++ != *str2++)
		{
			return false;
		}
	}
	return false;
}
