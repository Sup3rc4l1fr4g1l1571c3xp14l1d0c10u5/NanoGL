#ifndef _NANOGL_CHAR_CODE_HELPER_H
#define _NANOGL_CHAR_CODE_HELPER_H

#if defined(_WIN32)

#include <stdbool.h>
#include "./String.h"

extern char		*utf82sjis(const utf8_t *pUtf8Str, const utf8_t *pEnd);
extern utf8_t	*sjis2utf8(const char *pUtf8Str, const char *pEnd);

extern bool		is_contains_not_ascii(const char *str, const char *end);

#endif

#endif
