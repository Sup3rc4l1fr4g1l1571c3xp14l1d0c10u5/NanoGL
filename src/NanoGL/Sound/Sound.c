#include "Sound.h"
#include "../Debug/Debug.h"
#include "../String/String.h"

#if defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "./SoundChannel/ChannelManager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
typedef char utf8_t;
static char *utf82sjis(const utf8_t *pUtf8Str, const utf8_t *pEnd) {
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

static bool IsContainsNotAscii(const char *str, const char *end)
{
	while (str != end && *str != '\0')
	{
		if (!isascii(*str++)) { return true; }
	}
	return false;
}

#endif

static ALCdevice* alDevice;
static ALCcontext* alContext;
 
static void Sound_Finalize(void) {
	ChannelManager.Finalize();

	if (alContext != NULL) {
		alcDestroyContext(alContext);
		alContext = NULL;
	}
	if (alDevice != NULL) {
		alcCloseDevice(alDevice);
		alDevice = NULL;
	}
}

static bool Sound_Initialize_(void) {
	alDevice = alcOpenDevice(NULL);
	if (alDevice == NULL) {
		Debug.PrintError("Cannot open sound alDevice.");
		return false;
	}
	alContext = alcCreateContext(alDevice, NULL);
	if (alContext == NULL) {
		Debug.PrintError("Cannot create OpenAL's context.");
		Sound_Finalize();
		return false;
	}

	if (alcMakeContextCurrent(alContext) == AL_FALSE)
	{
		Debug.PrintError("Cannot select context to current.");
		Sound_Finalize();
		return false;
	}

	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListener3f(AL_ORIENTATION, 0, 0, -1);

	ChannelManager.Initialize();

	return true;
}
static bool Sound_Initialize(void) {
	Debug.PushBanner("**Error in Sound.Initialize");
	bool ret = Sound_Initialize_();
	Debug.PopBanner();
	return ret;
}

static bool Sound_Channel_CreateUTF8_(int channelId, const char*path) {
	if (channelId < 0 || 255 < channelId)
	{
		Debug.PrintError("The range of the argument `channelId` must be a 0 to 255. (You gave %d to the argument `channelId`)", channelId);
		return false;
	}
	if (path == NULL)
	{
		Debug.PrintError("The argument `path` is NULL.");
		return false;
	}
	int ret;
#if !defined(_WIN32)
	ret = ChannelManager.CreateFromFile(channelId, path);
#else
	if (IsContainsNotAscii(path,NULL))
	{
		char *sjispath = utf82sjis(path, NULL);
		ret = ChannelManager.CreateFromFile(channelId, sjispath);
		free(sjispath);
	} else
	{
		ret = ChannelManager.CreateFromFile(channelId, path);
	}
#endif
	if (!ret) {
		Debug.PrintError("The channel id=%d load %s failed.", channelId, path);
		return false;
	}
	
	return true;
}
static bool Sound_Channel_CreateUTF8(int channelId, const char*path) {
	Debug.PushBanner("**Error in Sound.ChannelCreateUTF8");
	bool ret = Sound_Channel_CreateUTF8_(channelId, path);
	Debug.PopBanner();
	return ret;
}

static bool Sound_Channel_Create_(int channelId, const char*path) {
	if (channelId < 0 || 255 < channelId)
	{
		Debug.PrintError("The range of the argument `channelId` must be a 0 to 255. (You gave %d to the argument `channelId`)", channelId);
		return false;
	}
	if (path == NULL)
	{
		Debug.PrintError("The argument `path` is NULL.");
		return false;
	}
	int ret = ChannelManager.CreateFromFile(channelId, path);
	if (!ret) {
		Debug.PrintError("The channel id=%d load %s failed.", channelId, path);
		return false;
	}
	
	return true;
}
static bool Sound_Channel_Create(int channelId, const char*path) {
	Debug.PushBanner("**Error in Sound.ChannelCreate");
	bool ret = Sound_Channel_Create_(channelId, path);
	Debug.PopBanner();
	return ret;
}

static bool Sound_Channel_Free_(int channelId) {
	if (channelId < 0 || 255 < channelId)
	{
		Debug.PrintError("The range of the argument `channelId` must be a 0 to 255. (You gave %d to the argument `channelId`)", channelId);
		return false;
	}
	ChannelManager.Close(channelId);
	
	return true;
}
static bool Sound_Channel_Free(int channelId) {
	Debug.PushBanner("**Error in Sound.ChannelFree");
	bool ret = Sound_Channel_Free_(channelId);
	Debug.PopBanner();
	return ret;
}

static bool Sound_Channel_Play_(int channelId, bool loop) {
	if (channelId < 0 || 255 < channelId)
	{
		Debug.PrintError("The range of the argument `channelId` must be a 0 to 255. (You gave %d to the argument `channelId`)", channelId);
		return false;
	}
	if (!ChannelManager.Play(channelId, loop)) {
		Debug.PrintError("The channel id=%d cannot play.", channelId);
		return false;
	}
	return true;
}

static bool Sound_Channel_Play(int channelId, bool loop) {
	Debug.PushBanner("**Error in Sound.ChannelPlay");
	bool ret = Sound_Channel_Play_(channelId, loop);
	Debug.PopBanner();
	return ret;
}

static bool Sound_Channel_Stop_(int channelId) {
	if (channelId < 0 || 255 < channelId)
	{
		Debug.PrintError("The range of the argument `channelId` must be a 0 to 255. (You gave %d to the argument `channelId`)", channelId);
		return false;
	}
	if (!ChannelManager.Stop(channelId)) {
		Debug.PrintError("The channel id=%d cannot stop.", channelId);
		return false;
	}
	return true;
}

static bool Sound_Channel_Stop(int channelId) {
	Debug.PushBanner("**Error in Sound.ChannelStop");
	bool ret = Sound_Channel_Stop_(channelId);
	Debug.PopBanner();
	return ret;
}

const struct __tagSoundAPI Sound = {
	Sound_Initialize,
	Sound_Finalize,
	Sound_Channel_CreateUTF8,
	Sound_Channel_Create,
	Sound_Channel_Free,
	Sound_Channel_Play,
	Sound_Channel_Stop,
};


