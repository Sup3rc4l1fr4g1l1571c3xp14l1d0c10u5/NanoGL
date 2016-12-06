#include <NanoGL/NanoGL.h>

#if defined(__APPLE__) || defined(__linux__)
#define __declspec(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) 
const struct __tagVideoAPI *GetVideoVTable(void) {
	return &Video;
}

__declspec(dllexport) 
const struct __tagMouseAPI *GetMouseVTable(void) {
	return &Mouse;
}

__declspec(dllexport) 
const struct __tagJoystickAPI *GetJoystickVTable(void) {
	return &Joystick;
}

__declspec(dllexport) 
const struct __tagKeyboardAPI *GetKeyboardVTable(void) {
	return &Keyboard;
}

__declspec(dllexport) 
const struct __tagSoundAPI *GetSoundVTable(void) {
	return &Sound;
}

__declspec(dllexport) 
const struct __tagFpsTimerAPI *GetFpsTimerVTable(void) {
	return &FpsTimer;
}

__declspec(dllexport) 
const struct __tagSystemAPI *GetSystemVTable(void) {
	return &System;
}

__declspec(dllexport) 
const struct __tagDialogAPI *GetDialogVTable(void) {
	return &Dialog;
}

void start(void) {
	/* this is dummy */
}

#ifdef __cplusplus
}
#endif
