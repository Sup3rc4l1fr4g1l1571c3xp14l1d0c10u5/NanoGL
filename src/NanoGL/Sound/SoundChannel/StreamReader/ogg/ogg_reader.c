#include "./ogg_reader.h"
#define STB_VORBIS_HEADER_ONLY
#include "./stb_vorbis.c"

static bool ogg_openfile(SoundStream *self, const char *filename)
{
	stb_vorbis_info info;
	self->stream = stb_vorbis_open_filename((char*)filename, NULL, NULL);
	if (!self->stream) return false;
	// Get file info
	info = stb_vorbis_get_info(self->stream);
	self->channels = info.channels;
	self->bit_par_sample = 16;
	self->sample_rate = info.sample_rate;
	//self->totalSamplesLeft = stb_vorbis_stream_length_in_samples(self->stream) * info.channels;
	return true;
}

static void ogg_close(SoundStream* self) {
	stb_vorbis_close(self->stream);
}

static bool ogg_decode(SoundStream *self, void *buffer, size_t szbuffer, size_t *szdecode) {
	size_t  size = 0;
	while (size < szbuffer) {
		int result = stb_vorbis_get_samples_short_interleaved(self->stream, self->channels, (short*)((unsigned char*)buffer + size), (szbuffer - size)/2);
		if (result > 0) {
			size += result * self->channels * sizeof(short);
		}
		else {
			break;
		}
	}
	*szdecode = size;
	return true;
}

static void ogg_seekstart(SoundStream* self) {
	stb_vorbis_seek_start(self->stream);
	//self->totalSamplesLeft = stb_vorbis_stream_length_in_samples(self->stream) * self->channels;
}

const SoundStreamReader OggReader = {
	ogg_openfile,
	ogg_close,
	ogg_decode,
	ogg_seekstart,
};
