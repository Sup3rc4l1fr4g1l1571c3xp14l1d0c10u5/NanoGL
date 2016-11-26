#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "../Video/Video.h"
#include "../Sound/Sound.h"
#include "../Debug/Debug.h"
#include "./System.h"

#if defined(_WIN32)
#include <conio.h>
#include <Windows.h>
#endif

extern void start(void);

static bool SystemIsRunning = false;

static void __NullHandler(void) {}

#if defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#pragma comment(linker, "/alternatename:___InitializeHandler=___DefaultInitializeHandler")
#pragma comment(linker, "/alternatename:___FinalizeHandler=___DefaultFinalizeHandler")
extern void(* const __InitializeHandler)(void);
extern void(* const __FinalizeHandler)(void);
#elif defined(__llvm__)
#define NORETURN __attribute__((noreturn))
#pragma weak __InitializeHandler = __DefaultInitializeHandler
#pragma weak __FinalizeHandler = __DefaultFinalizeHandler
//extern void(* const __InitializeHandler)(void);
//extern void(* const __FinalizeHandler)(void);
#elif defined(__GNUC__)
#define NORETURN __declspec(noreturn)
extern void(* const __InitializeHandler)(void) __attribute__((weak, alias ("__DefaultInitializeHandler")));
extern void(* const __FinalizeHandler)(void) __attribute__((weak, alias("__DefaultFinalizeHandler")));
#endif
void(*__DefaultInitializeHandler)(void) = __NullHandler;
void(*__DefaultFinalizeHandler)(void) = __NullHandler;

static QuitHandler _UserQuitHandler = NULL;
NORETURN
static void System_QuitHandlerExit(int dummy);
static void (*_QuitHandler)(int) = System_QuitHandlerExit;

static void System_SetQuitHandler(QuitHandler handler) {
	_UserQuitHandler = handler;
}

static void System_Quit(void)
{
	if (_UserQuitHandler != NULL) {
		_UserQuitHandler();
	}
	_QuitHandler(EXIT_SUCCESS);
}

static int _CommandLineArgC;
static const char **_CommandLineArgV;

static jmp_buf jmp;

NORETURN
static void System_QuitHandler(int dummy) {
	(void)dummy;
	longjmp(jmp, 1);
}

NORETURN
static void System_QuitHandlerExit(int dummy) {
	exit(dummy);
}

static void System_Run(int argc, const char *argv[]) {
	if (SystemIsRunning == true)
	{
		return;
	}
	SystemIsRunning = true;
	if (Video.Initialize() == false)
	{
		goto EXIT1;
	}

	if (Sound.Initialize() == false) {
		goto EXIT2;
	}

	if (setjmp(jmp) == 0) {
		_QuitHandler = System_QuitHandler;
		_CommandLineArgC = argc;
		_CommandLineArgV = argv;
		__InitializeHandler();
		start();
	}
	__FinalizeHandler();

	Sound.Finalize();
EXIT2:
	Video.Finalize();
EXIT1:
	if (Debug.IsConsoleShown()) {
#if defined(_WIN32)
		Debug.PrintError("[Press any key to close debug console.]");
		while (!_kbhit()) {
			Sleep(100);
		}
		_getch();
#endif
		Debug.DisableConsole();
	}
	SystemIsRunning = false;
}

static int System_GetCommandLineArgCount(void)
{
	return _CommandLineArgC;
}

static const char *System_GetCommandLineArgValue(int index)
{
	if (index < 0 || index >= _CommandLineArgC)
	{
		return NULL;
	}
	else {
		return _CommandLineArgV[index];
	}
}


const struct __tagSystemAPI System = {
	System_Run,
	System_Quit,
	System_GetCommandLineArgCount,
	System_GetCommandLineArgValue,
	System_SetQuitHandler,
};
