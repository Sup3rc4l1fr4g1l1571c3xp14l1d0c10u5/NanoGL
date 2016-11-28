#if defined(_WIN32)
#elif defined(__APPLE__) || defined(__linux__)
#define _strdup(x) strdup(x)
#define strcat_s(_Destination,_SizeInBytes,_Source) strcat(_Destination,_Source)
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "String.h"

static string_t String_Create(const char *str)
{
	string_t s;
	if (str != NULL)
	{
		s.c_str = _strdup(str);
	}
	else
	{
		s.c_str = NULL;
	}
	return s;
}

static void String_Free(string_t str)
{
	if (str.c_str != NULL)
	{
		free((void*)str.c_str);
	}
}

static string_t String_FormatTextV(const char *format, va_list args)
{
	va_list copied;
	va_copy(copied, args);
#if defined(_MSC_VER)
	int len = _vscprintf(format, copied);
#elif defined (__GNUC__)
	int len = vsnprintf(NULL, 0, format, copied);
#else
#error Cannot calc buffer size
#endif
	va_end(copied);
	size_t newBufLen = len + 1;
	char* newBuf = calloc(newBufLen, sizeof(char));
	if (newBuf != NULL) {
		va_copy(copied, args);
#if defined(_MSC_VER)
		vsprintf_s(newBuf, newBufLen, format, copied);
#elif defined (__GNUC__)
		vsnprintf(newBuf, newBufLen, format, copied);
#else
#error Cannot calc buffer size
#endif
		va_end(copied);
	}
	string_t s;
	s.c_str = newBuf;
	return s;
}
static string_t String_FormatText(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	string_t s = String_FormatTextV(format, args);
	va_end(args);
	return s;
}

string_t String_Join(string_t str1, const char *str2) {
	size_t len1 = strlen(str1.c_str);
	size_t len2 = strlen(str2);
	size_t sz = len1 + len2 + 1;
	char *newBuf = realloc(str1.c_str, sz);
	if (newBuf == NULL)
	{
		return str1;
	}
	strcat_s(newBuf, sz, str2);
	string_t s;
	s.c_str = newBuf;
	return s;
}

// utf8str～endの間でUTF8とみなして文字数（notバイト数）を数える
static size_t String_UTF8_Count(const char *utf8str, const char *end)
{
	size_t wc = 0;

	while (*utf8str != '\0' && (utf8str < end || end == NULL)) {
		unsigned char ch = *(unsigned char*)utf8str;
		if ((ch & 0x80) == 0x00) {
			// 1byte
			utf8str += 1;
			wc++;
		}
		else if ((ch & 0xC0) == 0xC0) {
			// 2byte
			utf8str += 2;
			wc++;
		}
		else if ((ch & 0xE0) == 0xE0) {
			// 3byte
			utf8str += 3;
			wc++;
		}
		else {
			// bad!
			utf8str++;
		}
	}
	return wc;
}

// utf8strをUTF8文字列とみなして n文字目のバイト位置を算出する
static const char * String_UTF8_Inc(const char *utf8str, size_t n)
{
	while (n > 0) {
		unsigned char ch = *(unsigned char*)utf8str;
		if ((ch & 0x80) == 0x00) {
			// 1byte
			utf8str += 1;
			n--;
		}
		else if ((ch & 0xC0) == 0xC0) {
			// 2byte
			utf8str += 2;
			n--;
		}
		else if ((ch & 0xE0) == 0xE0) {
			// 3byte
			utf8str += 3;
			n--;
		}
		else {
			// bad!
			utf8str++;
		}
	}
	return utf8str;
}

// sjisstr～endの間でSJISとみなして文字数（notバイト数）を数える
static size_t String_SJIS_Count(const char *sjisstr, const char *end)
{
	size_t wc = 0;

	while (*sjisstr != '\0' && (sjisstr < end || end == NULL)) {
		unsigned char ch = *(unsigned char*)sjisstr;
		if ((ch ^ 0x20U) - 0xA1U < 94U / 2U)
		{
			// SJIS Lead Byte
			sjisstr += 2;
			wc++;
		}
		else
		{
			sjisstr += 1;
			wc++;
		}
	}
	return wc;
}

// sjisstr を SJIS文字列とみなして n文字目のバイト位置を算出する
static const char *String_SJIS_Inc(const char *sjisstr, size_t n)
{
	// http://www.st.rim.or.jp/~phinloda/cqa/cqa15.html#Q4 
	while (n > 0)
	{
		unsigned int ch = (unsigned int)*(unsigned char*)sjisstr;
		if ((ch ^ 0x20U) - 0xA1U < 94U / 2U)
		{
			// SJIS Lead Byte
			sjisstr += 2;
		}
		else
		{
			sjisstr += 1;
		}
		n--;
	}
	return sjisstr;
}

const struct __tagStringAPI String = {
	String_Create,
	String_Free,
	String_FormatText,
	String_FormatTextV,
	String_Join,
	{
		String_UTF8_Count,
		String_UTF8_Inc,
	},
	{
		String_SJIS_Count,
		String_SJIS_Inc,
	}
};
