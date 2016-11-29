#include "./ChannelManager.h"
#include "./StreamChannel/StreamChannel.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#if defined(_WIN32)
#include <process.h>
#include <Windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <pthread.h>
#include <unistd.h>
#define msleep(x) usleep((x)*(1000))
#endif

#define MAX_CHANNEL 256

typedef struct __tagChannelManagerInstance
{
	bool initialized;
	struct StreamChannel *channels[MAX_CHANNEL];
#if defined(_WIN32)
	uintptr_t threadHandle;
	HANDLE hEventProgressThreadIsTerminateRequest;
	HANDLE hEventProgressThreadIsTerminated;
#elif defined(__APPLE__) || defined(__linux__)
	pthread_t thread;
	bool flagProgressThreadIsTerminateRequest;
#endif

} ChannelManagerInstance;

static ChannelManagerInstance manager;
#if defined(_WIN32)
static CRITICAL_SECTION cs;
#define LOCK()   do { EnterCriticalSection( &cs ); } while(0)
#define UNLOCK() do { LeaveCriticalSection( &cs ); } while(0)
#elif defined(__APPLE__) || defined(__linux__)
static pthread_mutex_t mutex;
#define LOCK()   do { pthread_mutex_lock( &mutex ); } while(0)
#define UNLOCK() do { pthread_mutex_unlock( &mutex ); } while(0)
#endif

static void ChannelManager_Progress(void);


#if defined(_WIN32)
static unsigned int __stdcall ProgressThreadProc(void *arg)
{
	(void)arg;
	for (;;)
	{
		if (WaitForSingleObject(manager.hEventProgressThreadIsTerminateRequest, 10) == WAIT_OBJECT_0)
		{
			break;
		}
		
		LOCK();
		ChannelManager_Progress();
		UNLOCK();
	}

	SetEvent(manager.hEventProgressThreadIsTerminated);
	_endthreadex(0);
	return 0;
}
#elif defined(__APPLE__) || defined(__linux__)
static void *ProgressThreadProc(void *arg)
{
	(void)arg;
	for (;;)
	{
		usleep(10*1000);
		LOCK();
		if (manager.flagProgressThreadIsTerminateRequest)
		{
			break;
		}
		
		ChannelManager_Progress();
		UNLOCK();
	}

	UNLOCK();
	pthread_exit(NULL);
	return NULL;
}
#endif

static bool ChannelManager_Initialize(void)
{
	if (manager.initialized)
	{
		return false;
	}
#if defined(_WIN32)
	InitializeCriticalSection(&cs);
#elif defined(__APPLE__) || defined(__linux__)
	pthread_mutex_init( &mutex, NULL);
#endif

	LOCK();
	
	memset(&manager, 0, sizeof(manager));
	manager.initialized = true;
#if defined(_WIN32)
	manager.hEventProgressThreadIsTerminated = CreateEvent(NULL, true, false, NULL);
	manager.hEventProgressThreadIsTerminateRequest = CreateEvent(NULL, true, false, NULL);
	manager.threadHandle = _beginthreadex(NULL, 0, ProgressThreadProc, NULL, 0, NULL);
#elif defined(__APPLE__) || defined(__linux__)
	manager.flagProgressThreadIsTerminateRequest = false;
	pthread_create(&manager.thread , NULL , ProgressThreadProc , NULL);
#endif

	UNLOCK();

	return true;
}

static void ChannelManager_Finalize(void)
{
	if (!manager.initialized)
	{
		return;
	}
#if defined(_WIN32)
	SetEvent(manager.hEventProgressThreadIsTerminateRequest);
	WaitForSingleObject(manager.hEventProgressThreadIsTerminated, INFINITE);
#elif defined(__APPLE__) || defined(__linux__)
	LOCK();
	manager.flagProgressThreadIsTerminateRequest = true;
	UNLOCK();
	pthread_join(manager.thread, NULL);
#endif

	for (int i = 0; i<MAX_CHANNEL; i++)
	{
		if (manager.channels[i] != NULL)
		{
			StreamChannel.Dispose(manager.channels[i]);
			manager.channels[i] = NULL;
		}
	}
#if defined(_WIN32)
	DeleteCriticalSection(&cs);
#elif defined(__APPLE__) || defined(__linux__)
	pthread_mutex_destroy( &mutex );
#endif
	memset(&manager, 0, sizeof(manager));
	manager.initialized = false;
}

static bool ChannelManager_CreateFromFile(int channel, const char *filename)
{
	bool ret = false;
	if (!manager.initialized || channel < 0 || MAX_CHANNEL <= channel)
	{
		return false;
	}

	LOCK();
	
	if (manager.channels[channel] != NULL)
	{
		goto end;
	}
	manager.channels[channel] = StreamChannel.Create();
	if (StreamChannel.Open(manager.channels[channel], filename) == false)
	{
		StreamChannel.Dispose(manager.channels[channel]);
		manager.channels[channel] = NULL;
		goto end;
	}
	ret = true;
end:

	UNLOCK();

	return ret;
}

static bool ChannelManager_Close(int channel)
{
	bool ret = false;
	if (!manager.initialized || channel < 0 || MAX_CHANNEL <= channel)
	{
		return false;
	}

	LOCK();

	if (manager.channels[channel] != NULL) {
		manager.channels[channel] = StreamChannel.Create();
		StreamChannel.Stop(manager.channels[channel]);
		StreamChannel.Dispose(manager.channels[channel]);
		manager.channels[channel] = NULL;
		ret = true;
	}

	UNLOCK();

	return ret;
}

static bool ChannelManager_Play(int channel, bool loop)
{
	bool ret = false;
	if (!manager.initialized || channel < 0 || MAX_CHANNEL <= channel)
	{
		return false;
	}

	LOCK();

	if (manager.channels[channel] != NULL) {
		ret = StreamChannel.Play(manager.channels[channel], loop);
	}

	UNLOCK();

	return ret;
}

static bool ChannelManager_IsPlay(int channel) {
	bool ret = false;
	if (!manager.initialized || channel < 0 || MAX_CHANNEL <= channel) {
		return false;
	}

	LOCK();

	if (manager.channels[channel] != NULL) {
		ret = StreamChannel.IsPlay(manager.channels[channel]);
	}

	UNLOCK();

	return ret;
}

static bool ChannelManager_Stop(int channel)
{
	bool ret = false;
	if (!manager.initialized || channel < 0 || MAX_CHANNEL <= channel)
	{
		return false;
	}

	LOCK();

	if (manager.channels[channel] != NULL) {
		ret = StreamChannel.Stop(manager.channels[channel]);
	}

	UNLOCK();

	return ret;
}

static void ChannelManager_Progress(void)
{
	for (int i=0; i<MAX_CHANNEL; i++) {
		if (manager.channels[i] != NULL) {
			StreamChannel.Progress(manager.channels[i]);
		}
	}
}



const struct __tagChannelManagerAPI ChannelManager = {
	ChannelManager_Initialize,
	ChannelManager_Finalize,
	ChannelManager_CreateFromFile,
	ChannelManager_Close,
	ChannelManager_Play,
	ChannelManager_IsPlay,
	ChannelManager_Stop,
	ChannelManager_Progress,
};
