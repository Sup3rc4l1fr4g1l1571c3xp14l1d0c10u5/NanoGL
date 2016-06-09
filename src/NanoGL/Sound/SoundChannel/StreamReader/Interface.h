#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <math.h>

#ifndef min
#define min(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x,y) (((x) > (y)) ? (x) : (y))
#endif

typedef struct __tagSoundStream {
	void*	stream;
	int		channels;
	int		sample_rate;
	int		bit_par_sample;
} SoundStream;

typedef struct __tagSoundStreamReaderAPI {
	bool	(*openfile)		(SoundStream *self, const char *path);
	void	(*close)		(SoundStream *self);
	bool	(*decode)		(SoundStream *self, void *buffer, size_t szbuffer, size_t *szdecoded);
	void	(*seekstart)	(SoundStream *self);
} SoundStreamReader;
