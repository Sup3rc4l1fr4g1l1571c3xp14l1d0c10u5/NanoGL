#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "../../../Debug/Debug.h"

#if defined(_MSC_VER)
#define strcasecmp _strcmpi
#elif defined(__APPLE__) || defined(__linux__) 
#include <strings.h>
#endif

#include "./StreamChannel.h"
#include "../StreamReader/StreamReader.h"

enum ChannelStatus { stop, playing, paused, stopping, restart };

struct StreamChannel {
	const SoundStreamReader* stream_reader;

	SoundStream stream;
	ALuint ID;
	ALuint buffers[2];
	ALuint source;
	ALuint format;
	bool shouldLoop;

	enum ChannelStatus  state;
};

static struct StreamChannel *StreamChannel_Create(void) {
	struct StreamChannel *self = (struct StreamChannel *)calloc(1,sizeof(struct StreamChannel));
	if (self == NULL) {
		return NULL;
	}
	memset(self, 0, sizeof(struct StreamChannel));
	alGenSources(1, &self->source);
	alGenBuffers(2, self->buffers);
	self->shouldLoop = true;
	self->stream_reader = NULL;
	return self;
}

static void StreamChannel_Dispose(struct StreamChannel* self) {
	if (self == NULL) {
		return;
	}
	alSourceStop(self->source);

	// force stop
	for (;;) {
		ALint queues;
		alGetSourcei(self->source, AL_BUFFERS_QUEUED, &queues);
		if (queues == 0) {
			break;
		}
		ALint processed;
		alGetSourcei(self->source, AL_BUFFERS_PROCESSED, &processed);
		while (processed--) {
			ALuint buffer;
			alSourceUnqueueBuffers(self->source, 1, &buffer);
		}
	}
	
	alDeleteSources(1, &self->source);
	alDeleteBuffers(2, self->buffers);
	if (self->stream_reader != NULL) {
		self->stream_reader->close(&self->stream);
	}
	memset(self, 0, sizeof(struct StreamChannel));
	self->stream_reader = NULL;
	free(self);
}

static bool StreamChannel_FillStream(struct StreamChannel* self, ALuint buffer) {
	size_t readed = 0;
	uint8_t *pcm = (uint8_t*)calloc(1, BUFFER_SIZE);
	if (pcm == NULL) {
		return false;
	}
	for (;;) {
		size_t size = 0;
		if (self->stream_reader->decode(&self->stream, (uint8_t*)pcm + readed, BUFFER_SIZE - readed, &size) == false) {
			// error
			return false;
		}
		if (size == 0) {
			// End of Stream
			if (self->shouldLoop) {
				self->stream_reader->seekstart(&self->stream);
			} else {
				break;
			}
		}
		readed += size;
		if (self->shouldLoop) {
			if (readed == BUFFER_SIZE)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (readed > 0) {
		alBufferData(buffer, self->format, pcm, readed, self->stream.sample_rate);
	}
	free(pcm);
	return readed > 0;
}

static const struct __tagReaderMapTable {
	const char *ext;
	const SoundStreamReader *reader;
} ReaderMapTable[] = {
	{ ".wav", &WavReader },
	{ ".ogg", &OggReader },
	{ ".mp3", &Mp3Reader },
	{ NULL, NULL },
};

static bool StreamChannel_Open(struct StreamChannel* self, const char* filename) {
	if (self->stream_reader != NULL)
	{
		return false;
	}
	const char *ext = strrchr(filename, '.');
	self->stream_reader = &NullReader;
	for (const struct __tagReaderMapTable *p = ReaderMapTable; p->ext != NULL; p++)
	{
		if (strcasecmp(p->ext, ext) == 0)
		{
			self->stream_reader = p->reader;
			break;
		}
	}
	if (self->stream_reader->openfile(&self->stream, filename) == false)
	{
		return false;
	}
	ALuint format = 0;
	if (self->stream.channels == 2) {
		if (self->stream.bit_par_sample == 8) {
			format = AL_FORMAT_STEREO8;
		}
		else if (self->stream.bit_par_sample == 16) {
			format = AL_FORMAT_STEREO16;
		}
	}
	else {
		if (self->stream.bit_par_sample == 8) {
			format = AL_FORMAT_MONO8;
		}
		else if (self->stream.bit_par_sample == 16) {
			format = AL_FORMAT_MONO16;
		}
	}
	if (format == 0)
	{
		self->stream_reader->close(&self->stream);
		self->stream_reader = NULL;
		return false;
	}
	self->format = format;
	self->state = stop;

	return true;
}

static bool StreamChannel_Stop(struct StreamChannel* self)
{
	if (self == NULL)
	{
		return false;
	}
	switch (self->state) {
		case stop: {
			return false;
		}
		case paused:
		case playing: {
			alSourceStop(self->source);
			self->state = stopping;
			return true;
		}
		case restart: {
			self->state = stopping;
			return true;
		}
		case stopping: {
			return true;
		}
		default: {
			return false;
		}
	}
}

static bool StreamChannel_Play(struct StreamChannel* self, bool loop)
{
	if (self == NULL)
	{
		return false;
	}
	switch (self->state) {
	case stop: {
		self->shouldLoop = loop;
		self->state = playing;
		self->stream_reader->seekstart(&self->stream);
		if (!StreamChannel_FillStream(self, self->buffers[0])) {
			// 一つ目のバッファには絶対にデータが必要
			Debug.PrintError("Buffer is empty");
			return false;
		}
		if (!StreamChannel_FillStream(self, self->buffers[1]))
		{
			// 効果音などの１バッファで十分なものの場合は二つめのバッファを使わない
			alSourceQueueBuffers(self->source, 1, self->buffers);
		}
		else
		{
			alSourceQueueBuffers(self->source, 2, self->buffers);
		}
		alSourcePlay(self->source);
		return true;
	}
	case paused: {
		alSourcePlay(self->source);
		self->state = playing;
		return true;
	}
	case playing: {
		StreamChannel_Stop(self);
		self->state = restart;
		return true;
	}
	case stopping: {
		self->state = restart;
		return true;
	}
	case restart: {
		// nothing 
		return true;
	}
	default:
		return false;
	}
	return true;
}

static bool StreamChannel_IsPlay(struct StreamChannel* self)
{
	if (self == NULL)
	{
		return false;
	}
	return self->state == playing;
}


static bool StreamChannel_Pause(struct StreamChannel* self)
{
	if (self == NULL || self->state != playing)
	{
		return false;
	}
	alSourcePause(self->source);
	self->state  = paused;
	return true;
}

static bool StreamChannel_Progress(struct StreamChannel* self) {
	switch (self->state) {
		
		case stop:
			return false;

		case playing:
			{
				// Stop playing if source is empty.
				ALint queues = 0;
				alGetSourcei(self->source, AL_BUFFERS_QUEUED, &queues);
				if (queues == 0)
				{
					alSourceStop(self->source);
					self->state = stop;
					return false;
				}
			}
		
			{
				// if queue is processed, rotate buffer and fill buffer.
				ALint processed = 0;
				alGetSourcei(self->source, AL_BUFFERS_PROCESSED, &processed);

				while (processed--) {
					ALuint buffer = 0;

					alSourceUnqueueBuffers(self->source, 1, &buffer);

					if (!StreamChannel_FillStream(self, buffer)) {
						// Stream is end and not loop
						return true;
					}
					alSourceQueueBuffers(self->source, 1, &buffer);
				}
			}
		
			return true;

		case restart:
		case stopping: {
			ALint queues;
			alGetSourcei(self->source, AL_BUFFERS_QUEUED, &queues);
			if (queues == 0) {
				if (self->state == restart) {
					self->state = stop;
					return StreamChannel_Play(self, self->shouldLoop);
				}
				self->state = stop;
				
				return false;
			}

			ALint processed = 0;
			alGetSourcei(self->source, AL_BUFFERS_PROCESSED, &processed);
			while (processed--) {
				ALuint buffer = 0;
				alSourceUnqueueBuffers(self->source, 1, &buffer);
			}
			return true;
		}
		default:
			return false;
	}
}

const struct __tagStreamChannelAPI StreamChannel = {
	StreamChannel_Create,
	StreamChannel_Dispose,
	StreamChannel_Open,
	StreamChannel_Stop,
	StreamChannel_Play,
	StreamChannel_IsPlay,
	StreamChannel_Pause,
	StreamChannel_Progress,
};
