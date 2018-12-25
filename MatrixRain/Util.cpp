#include "Util.h"

std::string mr::Util::ToString(const wchar_t * w)
{
	std::string result;

	while (*w != L'\0')
	{
		result += (char)*(w++);
	}

	return result;
}
