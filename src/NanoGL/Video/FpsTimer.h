#ifndef _NANOGL_VIDEO_FPSTIMER_H
#define _NANOGL_VIDEO_FPSTIMER_H

#include <stdbool.h>

extern const struct __tagFpsTimerAPI {
	void(*Initialize)(void);
	void(*Finalize)(void);
	void(*SetFPS)(unsigned int fps);
	unsigned int(*GetFPS)(void);
	float(*GetRealFPS)(void);
	unsigned int(*GetCPUPower)(void);
	void(*ResetTimer)(void);
	void(*WaitFrame)(void);
	void(*ResetElapseTimeCounter)(void);
	unsigned int(*GetElapseTimeCounter)(void);
} FpsTimer;

#endif
