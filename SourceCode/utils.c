/*-------------------------------------------------------------------------------
**	Utilities
**-----------------------------------------------------------------------------*/

#include "definitions.h"

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER) || defined(_WINDOWS_)

#define IS_WINDOWS
#include <Windows.h>

#else

#define NON_WINDOWS
#include <unistd.h>
#include <sys/time.h>

#endif

unsigned int get_time(void)
{
#if defined(IS_WINDOWS)
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return (unsigned int)((((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

//END