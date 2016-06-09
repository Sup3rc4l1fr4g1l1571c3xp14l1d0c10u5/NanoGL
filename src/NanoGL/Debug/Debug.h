#ifndef _NanoGL_DEBUG_H
#define _NanoGL_DEBUG_H

#include "../config.h"
#include <stdbool.h>

extern const struct __tagDebugAPI {

	bool (*EnableConsole)(void);
	void (*DisableConsole)(void);
	void (*PushBanner)(const char *format, ...);
	void (*PopBanner)(void);
	/**
	* @brief デバッグ用のメッセージを表示する
	* @param 書式は printf と同じ
	*/
	void (*PrintError)(const char *format, ...);
	bool (*IsConsoleShown)(void);
} Debug;

#endif
