#ifndef _NanoGL_SYSTEM_H
#define _NanoGL_SYSTEM_H

#if defined(_MSC_VER)
// __attribute__((weak)) / #pragma comment(linker, "/alternatename:_pWeakValue=_pDefaultWeakValue") 

#define NanoGL_Initialize() \
static void Initialize(void); \
extern void(* const __InitializeHandler)(void) = Initialize;\
static void Initialize(void) 

#define NanoGL_Finalize() \
static void Finalize(void); \
extern void(* const __FinalizeHandler)(void) = Finalize;\
static void Finalize(void) 

#elif defined(__llvm__)

#define NanoGL_Initialize() \
void __attribute__((weak)) __InitializeHandler(void)
    
#define NanoGL_Finalize() \
void __attribute__((weak)) __FinalizeHandler(void)

#elif defined(__GNUC__)
#define NanoGL_Initialize() \
static void Initialize(void); \
void(* const __InitializeHandler)(void) = Initialize;\
static void Initialize(void) 

#define NanoGL_Finalize() \
static void Finalize(void); \
void(* const __FinalizeHandler)(void) = Finalize;\
static void Finalize(void) 

#else
#error "Initializer/Finalize is not supported"
#endif

typedef void (*QuitHandler)(void);

extern const struct __tagSystemAPI {
	void(*Run)(int argc, const char *argv[]);
	void (*Quit)(void);
	int(*GetCommandLineArgCount)(void);
	const char *(*GetCommandLineArgValue)(int index);
	void (*SetQuitHandler)(QuitHandler handler);
} System;

#endif
