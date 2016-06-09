#include "Sound.h"
#include "../Debug/Debug.h"

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
	if (!ChannelManager.CreateFromFile(channelId, path)) {
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
	Sound_Channel_Create,
	Sound_Channel_Free,
	Sound_Channel_Play,
	Sound_Channel_Stop,
};


