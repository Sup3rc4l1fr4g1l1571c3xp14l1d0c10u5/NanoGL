#include "./null_reader.h"
#include <string.h>

static bool null_openfile(SoundStream *self, const char *filename)
{
	(void)filename;
	self->stream = NULL;
	self->bit_par_sample = 8;
	self->channels = 1;
	self->sample_rate = 22050;
	//self->totalSamplesLeft = 22050 * 1024;
	return true;
}

static void null_close(SoundStream* self) {
	(void)self;
}

static bool null_decode(SoundStream *self, void *buffer, size_t szbuffer, size_t *szdecode) {
	(void)self;
	memset(buffer, 0, szbuffer);
	*szdecode = szbuffer;
	return true;
}

static void null_seekstart(SoundStream* self) {
	(void)self;
}

const SoundStreamReader NullReader = {
	null_openfile,
	null_close,
	null_decode,
	null_seekstart,
};
