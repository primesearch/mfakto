#ifndef MY_FNMATCH_H
#define MY_FNMATCH_H
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#include <fnmatch.h>
#define HAS_FNMATCH
#else
#ifdef __cplusplus
extern "C" {
#endif

/*
Define our own. It would be easier to just use NTDLL's RtlIsNameInExpression()
on Windows, but ntdll.lib is only available in the Native API for Windows 10
and above.

However, there are many GPL-compatible fnmatch implementations available.
*/
#define FNM_CASEFOLD   1
#define FNM_IGNORECASE FNM_CASEFOLD
#define FNM_NOMATCH    -1
int fnmatch(const char *pattern, const char *string, int flags);
#endif

inline static int patmatch(const char *pattern, const char *string, int flags)
{
    return fnmatch(pattern, string, flags) == 0;
}

#ifndef HAS_FNMATCH
#ifdef __cplusplus
}
#endif
#endif

#endif
