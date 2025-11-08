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
* Define our own glob matcher.
* 
* NTDLL's RtlIsNameInExpression() function does this, but it requires specific
* Windows SDK (as opposed to Windows) versions due to the availability of
* certain libraries.
* 
* We choose an GPL-compatible fnmatch implementation that also provides more
* features, such as character ranges from POSIX globs.
*/
#define FNM_CASEFOLD   1
#define FNM_IGNORECASE FNM_CASEFOLD
#define FNM_NOMATCH    -1
int fnmatch(const char *pattern, const char *string, int flags);
#endif

/*
* fnmatch wrapper for use as a Boolean.
* 
* Remember: add "*" before and after the input string as needed because
* fnmatch() matches the whole string against the provided pattern.
*/
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
