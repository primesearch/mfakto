#ifndef MY_FNMATCH_H
#define MY_FNMATCH_H
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#pragma warn "using system fnmatch"
#include <fnmatch.h>
#define HAS_FNMATCH
#else
#ifdef __cplusplus
extern "C"
{
#endif
/* Define our own. It would be easier to just use ntdll RtlIsNameInExpression() on Windows, but ntdll.lib is Windows 10+ SDK only.
 * There are many license-compatible fnmatch implementations available anyways, so. */
#define FNM_CASEFOLD   1
#define FNM_IGNORECASE FNM_CASEFOLD
#define FNM_NOMATCH    -1
int fnmatch(const char *pattern, const char *string, int flags);
#endif

inline static int patmatch(const char *pattern, const char *string, int flags)
{
    return fnmatch(pattern, string, flags) == 0;
}
#ifdef __cplusplus
}
#endif
#endif