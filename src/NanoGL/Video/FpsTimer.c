#if defined(_WIN32)
#include <windows.h>
#define msleep(x) Sleep(x)
#elif defined(__APPLE__)
#include <unistd.h>
#define msleep(x) usleep((x)*(1000))
#endif
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdint.h>
#include "FpsTimer.h"

static struct FpsTimerWork {
	uint32_t	fps;						/* fps(default: 60) */
	uint32_t	wait_time_ms_fp;			/* wait time [ms] (16bit fixed point number) */
	uint32_t	prev_draw_time_ms;			/* prev draw time [ms] */
	uint32_t	prev_draw_fraction_time_ms_fp;	/* fraction of prev draw time [ms] (16bit fixed point number) */
	uint32_t	elapsed_time_ms;			/* elapsed time [ms] */
	uint32_t	recent_draw_time_ms[32];	/* recent draw time [ms] */
	uint32_t	recent_elapsed_time_ms[32];	/* recent elapsed time [ms] */
	uint32_t	draw_count;
	uint32_t	wait_count;
} FpsTimerWork;

static void     FpsTimer_Initialize(void);
static void     FpsTimer_Finalize(void);
static void     FpsTimer_SetFPS(uint32_t fps);
static uint32_t FpsTimer_GetFPS(void);
static float    FpsTimer_GetRealFPS(void);
static uint32_t FpsTimer_GetCPUPower(void);
static void     FpsTimer_ResetTimer(void);
static void     FpsTimer_WaitFrame(void);
static void     FpsTimer_ResetElapseTimeCounter(void);
static uint32_t FpsTimer_GetElapseTimeCounter(void);

static void FpsTimer_Initialize(void) {
	memset(&FpsTimerWork, 0, sizeof(FpsTimerWork));
	FpsTimer_SetFPS(60);
}

static void FpsTimer_Finalize(void) {
	/* nothing to do */
}

static void FpsTimer_SetFPS(uint32_t fps) {
	FpsTimer_ResetTimer();
	FpsTimer_ResetElapseTimeCounter();

	FpsTimerWork.wait_count = 0;
	FpsTimerWork.draw_count = 0;

	FpsTimerWork.fps = fps;
	if (fps == 0) {	// non-wait mode
		return;
	}

	FpsTimerWork.wait_time_ms_fp = (1000 * 0x10000) / FpsTimerWork.fps;
	FpsTimerWork.prev_draw_fraction_time_ms_fp = 0;
}

static uint32_t FpsTimer_GetFPS(void) {
	return FpsTimerWork.fps;
}

static float FpsTimer_GetRealFPS(void) {
	if (FpsTimerWork.draw_count < 31) {
		return 0;
	}
	uint32_t diff = 
		  FpsTimerWork.recent_draw_time_ms[(FpsTimerWork.draw_count - 1) & 31]
		- FpsTimerWork.recent_draw_time_ms[FpsTimerWork.draw_count & 31];
	if (diff != 0) {
		return 1000.0f * 31.0f / diff;
	}
	else {
		return 0.0f;
	}
}

static uint32_t FpsTimer_GetCPUPower(void) {
	if (FpsTimerWork.draw_count < 31) {
		return 0;
	}
	uint32_t sum = 0;
	for (int i = 0; i<32; i++) {
		sum += FpsTimerWork.recent_elapsed_time_ms[i];
	}
	return 100 - (sum*FpsTimerWork.fps / 160);
}

static void FpsTimer_ResetTimer(void) {
	FpsTimerWork.prev_draw_time_ms = (uint32_t)(glfwGetTime()*1000.0); 
}

static void FpsTimer_WaitFrame(void) {
	uint32_t current_time_ms = (uint32_t)(glfwGetTime()*1000.0);

	FpsTimerWork.recent_draw_time_ms[FpsTimerWork.draw_count & 31] = current_time_ms;

	++FpsTimerWork.draw_count;
	if (FpsTimerWork.draw_count == 64) {
		FpsTimerWork.draw_count = 32;
	}

	++FpsTimerWork.wait_count; // ‚±‚¢‚Â‚ðfps‘ª’è‚ÉŽg‚¤‚±‚Æ‚ªo—ˆ‚éB

	if (FpsTimerWork.fps == 0) {
		FpsTimerWork.recent_elapsed_time_ms[FpsTimerWork.draw_count & 31] = 0;
		return; // Non-wait mode
	}


	FpsTimerWork.prev_draw_fraction_time_ms_fp = (FpsTimerWork.prev_draw_fraction_time_ms_fp & 0xffff) + FpsTimerWork.wait_time_ms_fp;

	uint32_t elapsed_time_ms = (current_time_ms - FpsTimerWork.prev_draw_time_ms);
	uint32_t next_draw_time_ms = FpsTimerWork.prev_draw_fraction_time_ms_fp >> 16;

	if (elapsed_time_ms >=  next_draw_time_ms) {
		// time over!!
		if ((elapsed_time_ms -  next_draw_time_ms) < (FpsTimerWork.wait_time_ms_fp >> 17)) {
			current_time_ms -= (elapsed_time_ms -  next_draw_time_ms);
		}
		else {
			current_time_ms -= (FpsTimerWork.wait_time_ms_fp >> 17);
		}
		FpsTimerWork.prev_draw_time_ms = current_time_ms;
		FpsTimerWork.recent_elapsed_time_ms[FpsTimerWork.draw_count & 31] = 0;
		return;	// non wait
	}

	FpsTimerWork.elapsed_time_ms += elapsed_time_ms;
	FpsTimerWork.recent_elapsed_time_ms[FpsTimerWork.draw_count & 31] = elapsed_time_ms;

	msleep( next_draw_time_ms - elapsed_time_ms);

	FpsTimerWork.prev_draw_time_ms +=  next_draw_time_ms;
}

static void FpsTimer_ResetElapseTimeCounter(void) {
	FpsTimerWork.elapsed_time_ms = 0;
}

static uint32_t FpsTimer_GetElapseTimeCounter(void) {
	return FpsTimerWork.elapsed_time_ms;
}

const struct __tagFpsTimerAPI FpsTimer = {
	FpsTimer_Initialize,
	FpsTimer_Finalize,
	FpsTimer_SetFPS,
	FpsTimer_GetFPS,
	FpsTimer_GetRealFPS,
	FpsTimer_GetCPUPower,
	FpsTimer_ResetTimer,
	FpsTimer_WaitFrame,
	FpsTimer_ResetElapseTimeCounter,
	FpsTimer_GetElapseTimeCounter,
};
