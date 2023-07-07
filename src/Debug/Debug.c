#include "Debug.h"

#if defined(_WIN32)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static bool console_created = false;
typedef struct tagBanner {
	struct tagBanner *prev;
	char str[1];
} Banner;

static Banner *banner = NULL;

static bool EnableConsole(void) {
	if (console_created) {
		return true;
	}
#if defined(_WIN32)
	if (AllocConsole() == FALSE)
	{
		return false;
	}
	if (_fileno(stdin) < 0) {
		(void)freopen("CONIN$", "r", stdin);
	}
	if (_fileno(stdout) < 0) {
		(void)freopen("CONOUT$", "w", stdout);
	}
	if (_fileno(stderr) < 0) {
		(void)freopen("CONOUT$", "w", stderr);
	}
#endif
	console_created = true;
	return true;
}

static void DebugPushBanner(const char *format, ...)
{
	if (format == NULL) {
		return;
	} else {
		va_list args;
		va_start(args, format);
#if defined(_MSC_VER)
		int len = _vscprintf(format, args);
#elif defined (__GNUC__)
		int len = vsnprintf(NULL, 0, format, args);
#else
#error Cannot calc buffer size
#endif
		va_end(args);
		size_t newBufLen = len + 1;
		Banner *newBanner = calloc(newBufLen + sizeof(Banner), sizeof(char));
		if (newBanner != NULL) {
			newBanner->prev = banner;
			va_start(args, format);
#if defined(_MSC_VER)
			vsprintf_s(newBanner->str, newBufLen, format, args);
#elif defined (__GNUC__)
			vsnprintf(newBanner->str, newBufLen, format, args);
#else
#error Cannot calc buffer size
#endif
			va_end(args);
			banner = newBanner;
		}
		va_end(args);

	}
}
static void DebugPopBanner(void)
{
	if (banner == NULL)
	{
		return;
	}
	Banner *newBanner = banner->prev;
	free(banner);
	banner = newBanner;
}


static void DebugPrintError(const char *format, ...)
{
	if (EnableConsole()) {
		if (banner != NULL) {
			fprintf(stderr, "%s: ", banner->str);
		}
		va_list args;
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
		fputs("\n", stderr);
	}
}

static void DisableConsole(void)
{
	if (console_created)
	{
#if defined(_WIN32)
		FreeConsole();
#endif
		console_created = false;
	}
}

static bool IsConsoleShown(void) {
#if defined(_WIN32)
	return console_created && (_fileno(stdout) >= 0 || _fileno(stderr) >= 0 || _fileno(stdin) >= 0);
#else
	return console_created;
#endif
}

const struct __tagDebugAPI Debug = {
	EnableConsole,
	DisableConsole,
	DebugPushBanner,
	DebugPopBanner,
	DebugPrintError,
	IsConsoleShown,
};
