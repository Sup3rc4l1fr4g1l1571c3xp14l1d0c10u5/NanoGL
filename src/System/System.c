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

#if defined(__llvm__)
void __attribute__((weak)) start(void) {
	puts("empty!");
}
#elif defined(__GNUC__)
void __attribute__((weak)) start(void) {
	puts("empty!");
}
#else
extern void start(void);
#endif

static bool SystemIsRunning = false;

static void __NullHandler(void) {}

#if defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#if defined(_M_IX86)
#pragma comment(linker, "/alternatename:___InitializeHandler=___DefaultInitializeHandler")
#pragma comment(linker, "/alternatename:___FinalizeHandler=___DefaultFinalizeHandler")
#elif defined(_M_IA64) || defined(_M_AMD64)
#pragma comment(linker, "/alternatename:__InitializeHandler=__DefaultInitializeHandler")
#pragma comment(linker, "/alternatename:__FinalizeHandler=__DefaultFinalizeHandler")
#endif
extern void(* const __InitializeHandler)(void);
extern void(* const __FinalizeHandler)(void);
void(*__DefaultInitializeHandler)(void) = __NullHandler;
void(*__DefaultFinalizeHandler)(void) = __NullHandler;
#elif defined(__llvm__)
#define NORETURN __attribute__((noreturn))
void __attribute__((weak)) __InitializeHandler(void) {}
void __attribute__((weak)) __FinalizeHandler(void) {}
#elif defined(__GNUC__)
#if defined(__MINGW32__)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN __attribute__((noreturn))
#endif
extern void(* const __InitializeHandler)(void) __attribute__((weak, alias ("__DefaultInitializeHandler")));
extern void(* const __FinalizeHandler)(void) __attribute__((weak, alias("__DefaultFinalizeHandler")));
void(*__DefaultInitializeHandler)(void) = __NullHandler;
void(*__DefaultFinalizeHandler)(void) = __NullHandler;
#endif

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
		(void)_getch();
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
