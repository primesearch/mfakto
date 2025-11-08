#ifndef MY_FNMATCH_H
#define MY_FNMATCH_H
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#include <fnmatch.h>
#define HAS_FNMATCH
#else
#ifdef __cplusplus
extern "C" {
#endif

/* Define our own glob-matcher.
 * NTDLL has a RtlIsNameInExpression(), but it incurs some requirements on
 * SDK version (not on OS version) due to import lib availability.
 * We choose to use an GPL-compatible fnmatch implementation, which also
 * provides more features such as character ranges from POSIX glob format. */
#define FNM_CASEFOLD   1
#define FNM_IGNORECASE FNM_CASEFOLD
#define FNM_NOMATCH    -1
int fnmatch(const char *pattern, const char *string, int flags);
#endif

/* fnmatch wrapper for use as boolean. 
 * Remember: fnmatch() matches the whole string, so add "*" before and after
 * as needed! */
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
