#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "wav_reader.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FCC(c1,c2,c3,c4)	(((uint32_t)(c4)<<24)+((uint32_t)(c3)<<16)+((uint16_t)(c2)<<8)+(uint8_t)(c1))	/* FourCC */

struct RiffHeader {
	uint32_t riff;
	int32_t  size;
	uint32_t type;
};

struct FormatChunk {
	uint32_t id;
	int32_t size;
	int16_t format;
	uint16_t channels;
	uint32_t samplerate;
	uint32_t bytepersec;
	uint16_t blockalign;
	uint16_t bitswidth;
};

struct DataChunk {
	uint32_t id;
	uint32_t size;
};

struct wav
{
	FILE *fp;
	size_t length;
	int nchannels;
	int samplerate;
	int bitswidth;
	int startpos;
};

static bool wav_openfile(SoundStream *self, const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL)
	{
		goto failed;
	}
	struct RiffHeader header;
	if (fread(&header, 1, sizeof(struct RiffHeader), file) != sizeof(struct RiffHeader))
	{
		goto failed;
	}
	if (header.riff != FCC('R', 'I', 'F', 'F') || header.type != FCC('W', 'A', 'V', 'E')) {
		goto failed;
	}
	struct FormatChunk format_chunk;

	if (fread(&format_chunk, 1, sizeof(struct FormatChunk), file) != sizeof(struct FormatChunk))
	{
		goto failed;
	}
	if (format_chunk.id != FCC('f', 'm', 't', ' ')) {
		goto failed;
	}

	struct DataChunk data_chunk;

	if (fread(&data_chunk, 1, sizeof(struct DataChunk), file) != sizeof(struct DataChunk))
	{
		goto failed;
	}
	if (data_chunk.id != FCC('d', 'a', 't', 'a')) {
		goto failed;
	}
	
	struct wav *w = (struct wav *)calloc(1, sizeof(struct wav));
	if (w == NULL)
	{
		goto failed;
	}
	w->length = data_chunk.size;
	w->nchannels = format_chunk.channels;
	w->samplerate = format_chunk.samplerate;
	w->bitswidth = format_chunk.bitswidth;
	w->startpos = ftell(file);
	w->fp = file;

	self->stream = w;
	self->channels = w->nchannels;
	self->sample_rate = w->samplerate;
	self->bit_par_sample = w->bitswidth;
	//self->totalSamplesLeft = w->length;
	return true;

failed:
	if (file != NULL) { fclose(file); }
	return false;

}

static void wav_close(SoundStream* self) {
	struct wav *info = (struct wav *)self->stream;
	if (info != NULL) {
		if (info->fp != NULL) { fclose(info->fp); }
		free(info);
		self->stream = NULL;
	}
}

static bool wav_decode(SoundStream *self, void *buffer, size_t szbuffer, size_t *szdecode) {
	struct wav *info = self->stream;
	size_t  size = 0;
	while (size < szbuffer) {
		size_t lpos = ftell(info->fp) - info->startpos;
		size_t readsize = min(info->length - lpos, (szbuffer - size));
		int result = fread((unsigned char*)buffer + size, 1, readsize, info->fp);
		if (result > 0) {
			size += result;
		}
		else {
			break;
		}
	}
	*szdecode = size;
	return true;
}

static void wav_seekstart(SoundStream* self) {
	struct wav *info = (struct wav*)(self->stream);
	fseek(info->fp, info->startpos, SEEK_SET);
	//self->totalSamplesLeft = info->length;
}

const SoundStreamReader WavReader = {
	wav_openfile,
	wav_close,
	wav_decode,
	wav_seekstart,
};
