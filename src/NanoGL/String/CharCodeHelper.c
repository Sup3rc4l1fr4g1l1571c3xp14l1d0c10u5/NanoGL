#if defined(_WIN32)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <windows.h>
#include <stdbool.h>
#include "./CharCodeHelper.h"

char *utf82sjis(const utf8_t *pUtf8Str, const utf8_t *pEnd) {
	if (pUtf8Str == NULL) {
		return NULL;
	}
	// Convert UTF8 to WideChar(UTF-16)
	int nUtf8ByteCount = pEnd - pUtf8Str;
	int nUtf16WordCount = MultiByteToWideChar(CP_UTF8, 0, pUtf8Str, nUtf8ByteCount, NULL, 0);
	wchar_t *pUtf16Str = calloc(nUtf16WordCount + 1, sizeof(wchar_t));
	if (pUtf16Str == NULL) { return NULL; }
	if (MultiByteToWideChar(CP_UTF8, 0, pUtf8Str, nUtf8ByteCount, pUtf16Str, nUtf16WordCount) != nUtf16WordCount) {
		free(pUtf16Str);
		return NULL;
	}

	// Convert WideChar(UTF-16) to UShiftJIS(CP923)
	int nConvBytes = WideCharToMultiByte(CP_ACP, 0, pUtf16Str, -1, NULL, 0, NULL, NULL);
	char *pSjisStr = calloc(nConvBytes + 1, sizeof(char));
	if (pSjisStr == NULL) {
		free(pUtf16Str);
		return false;
	}
	if (WideCharToMultiByte(CP_ACP, 0, pUtf16Str, -1, pSjisStr, nConvBytes, NULL, NULL) != nConvBytes) {
		free(pUtf16Str);
		free(pSjisStr);
		return false;
	}

	free(pUtf16Str);

	return pSjisStr;
}

utf8_t *sjis2utf8(const char *pSjisStr, const char *pEnd) {
	if (pSjisStr == NULL) {
		return NULL;
	}
	// Convert ShiftJIS(CP923) to WideChar(UTF-16)
	int nConvBytes = pEnd == NULL ? -1 : (pEnd - pSjisStr);
	int nUtf16WordCount = MultiByteToWideChar(CP_ACP, 0, pSjisStr, nConvBytes, NULL, 0);
	wchar_t *pUtf16Str = calloc(nUtf16WordCount + 1, sizeof(wchar_t));
	if (pUtf16Str == NULL) { return NULL; }
	if (MultiByteToWideChar(CP_ACP, 0, pSjisStr, nConvBytes, pUtf16Str, nUtf16WordCount) != nUtf16WordCount) {
		free(pUtf16Str);
		return NULL;
	}

	// Convert WideChar(UTF-16) to ShiftJIS(CP923)
	int nUtf8WordCount = WideCharToMultiByte(CP_UTF8, 0, pUtf16Str, -1, NULL, 0, NULL, NULL);
	utf8_t *pUtf8Str = calloc(nUtf8WordCount + 1, sizeof(utf8_t));
	if (pUtf8Str == NULL) {
		free(pUtf16Str);
		return false;
	}
	if (WideCharToMultiByte(CP_UTF8, 0, pUtf16Str, -1, pUtf8Str, nUtf8WordCount, NULL, NULL) != nUtf8WordCount) {
		free(pUtf16Str);
		free(pUtf8Str);
		return false;
	}

	free(pUtf16Str);

	return pUtf8Str;
}

bool is_contains_not_ascii(const char *str, const char *end) {
	if (str == NULL) {
		return false;
	}
	while (str != end && *str != '\0') {
		if (!isascii(*str++)) { return true; }
	}
	return false;
}

#endif
