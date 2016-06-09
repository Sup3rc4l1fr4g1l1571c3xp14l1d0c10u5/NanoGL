#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "./mp3_reader.h"
#include <mad.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

struct FileSegmentStream {
	FILE *fp;
	size_t pos;
	size_t head;
	size_t tail;
};

static bool FileSegmentStream_Initialize(struct FileSegmentStream *self, FILE *fp, size_t head, size_t tail)
{
	if (self->fp != NULL)
	{
		return false;
	}
	self->fp = fp;
	self->pos = head;
	self->head = head;
	self->tail = tail;
	fseek(fp, head, SEEK_SET);
	return true;
}
static void FileSegmentStream_Finalize(struct FileSegmentStream *self)
{
	if (self != NULL) {
		if (self->fp != NULL) {
			fclose(self->fp);
			self->fp = NULL;
		}
	}
}

static bool FileSegmentStream_EOF(struct FileSegmentStream *self) {
	assert(self != NULL);
	assert(self->fp != NULL);
	return self->tail == self->pos;
}

static bool FileSegmentStream_Read(struct FileSegmentStream *self, void *buf, size_t readsize, size_t *readedsize) {
	assert(self != NULL);
	assert(self->fp != NULL);
	if (self->tail <= self->pos)
	{
		*readedsize = 0;
		return false;
	}
	size_t request = min(self->tail - self->pos, readsize);
	*readedsize = fread(buf, 1, request, self->fp);
	self->pos += *readedsize;
	return true;
}

static bool FileSegmentStream_Seek(struct FileSegmentStream *self, long offset, int origin) {
	assert(self != NULL);
	assert(self->fp != NULL);
	long target = 0;
	switch (origin)
	{
	case SEEK_SET: {
		target = offset + self->head;
		if (target < self->head)
		{
			target = self->head;
		}
		if (target > self->tail)
		{
			target = self->tail;
		}
		break;
	}
	case SEEK_CUR: {
		target = offset + self->pos;
		if (target < self->head)
		{
			target = self->head;
		}
		if (target > self->tail)
		{
			target = self->tail;
		}
		break;
	}
	case SEEK_END: {
		target = offset + self->tail;
		if (target < self->head)
		{
			target = self->head;
		}
		if (target > self->tail)
		{
			target = self->tail;
		}
		break;
	}
	default:
		return false;
	}
	fseek(self->fp, target, SEEK_SET);
	self->pos = target;
	return true;
}
size_t FileSegmentStream_Position(struct FileSegmentStream *self) {
	assert(self != NULL);
	assert(self->fp != NULL);
	assert(self->pos >= self->head);
	assert(self->pos <= self->tail);
	return self->pos - self->head;
}

typedef struct __tagDecodeWork {
	struct FileSegmentStream fss;
	uint8_t *decoded_datas;
	size_t decoded_data_length;
	size_t decoded_data_capacity;
	struct mad_stream stream;
	struct mad_frame frame;
	struct mad_synth synth;
	//int frame_count;
	bool eof;
	uint8_t *input_buffer;
	int input_length;
	int samplerate;
	int channel;
	int bitswidth;
	//size_t decoded_size;

} DecodeWork;

#define INPUT_BUFFER_LEN (4096*64)

static bool mp3_openfile(SoundStream *self, const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL)
	{
		return false;;
	}
	int file_head = 0;
	int file_tail = 0;
	int filesize = 0;
	int samplerate = 0;
	int channel = 0;

	{
		uint8_t *workbuffer = (uint8_t*)calloc(1, INPUT_BUFFER_LEN);
		if (workbuffer == NULL)
		{
			return false;
		}
		struct mad_stream stream;
		struct mad_header header;

		fseek(file, 0, SEEK_END);
		filesize = ftell(file);

		// ID3V1?
		fseek(file, -128, SEEK_END);
		size_t readed = fread(workbuffer, 1, 128, file);
		if (strncmp((const char*)workbuffer, "TAG", 3) == 0) {
			file_tail = filesize-128;
		} else
		{
			file_tail = filesize;
		}

		// ID3V2?
		fseek(file, 0, SEEK_SET);
		readed = fread(workbuffer, 1, INPUT_BUFFER_LEN, file);
		if (strncmp((const char*)workbuffer, "ID3", 3) == 0)
		{
			size_t offset =
				10 +
				(((workbuffer[6] & 0x7F) << (7 * 3)) |
				((workbuffer[7] & 0x7F) << (7 * 2)) |
					((workbuffer[8] & 0x7F) << (7 * 1)) |
					((workbuffer[9] & 0x7F) << (7 * 0)));
			file_head = offset;
		} else
		{
			file_head = 0;
		}

		fseek(file, file_head, SEEK_SET);
		readed = fread(workbuffer, 1, INPUT_BUFFER_LEN, file);

		if (workbuffer[0] != 0xFF)
		{
			// あれー？MP3フレームがないぞ
			free(workbuffer);
			fclose(file);
			return false;
		}
		switch (workbuffer[1] & 0xFE)
		{
			case 0xFA: /* MPEG ADTS, layer III, v1 */
			case 0xFC: /* MPEG ADTS, layer II, v1 */
			case 0xFE: /* MPEG ADTS, layer I, v1 */
			case 0xF2: /* MPEG ADTS, layer III, v2 */
			case 0xF4: /* MPEG ADTS, layer II, v2 */
			case 0xF6: /* MPEG ADTS, layer I, v2 */
			case 0xE2: /* MPEG ADTS, layer III, v2.5 */
				break;	// ok
			default:
				// 知らないフォーマットだ
				free(workbuffer);
				fclose(file);
				return false;
		}

		// 本当はきちんと全フレームスキャンして色々しなければならないが
		// 手を抜く

		mad_stream_init(&stream);
		mad_header_init(&header);
		mad_stream_buffer(&stream, workbuffer, readed);
		int ret = mad_header_decode(&header, &stream);
		if (ret != 0) {
			free(workbuffer);
			fclose(file);
			mad_stream_finish(&stream);
			mad_header_finish(&header);
			return false;
		}
		samplerate = header.samplerate;
		channel = MAD_NCHANNELS(&header);
		fseek(file, file_head, SEEK_SET);
		mad_stream_finish(&stream);
		mad_header_finish(&header);
		free(workbuffer);
	}

	DecodeWork *work = calloc(1, sizeof(DecodeWork));
	if (work == NULL)
	{
		return false;
	}
	if (FileSegmentStream_Initialize(&work->fss, file, file_head, file_tail) == false)
	{
		free(work);
		return false;
	}

	mad_stream_init(&work->stream);
	mad_frame_init(&work->frame);
	mad_synth_init(&work->synth);
	work->eof = false;
	// デコードデータのバッファ
	work->decoded_datas = (unsigned char *)calloc(1, INPUT_BUFFER_LEN);	// AL側のバッファサイズの二倍以上にしておくといいよ
	work->decoded_data_length = 0;
	work->decoded_data_capacity = INPUT_BUFFER_LEN;
	// 入力データのバッファ
	work->input_buffer = (unsigned char *)calloc(1, INPUT_BUFFER_LEN);
	work->input_length = 0;

	// サンプリングレートやチャンネル数（MPEG仕様上１フレームごとに変えることができるがそういうものには対応しない）
	work->samplerate = samplerate;
	work->channel = channel;
	work->bitswidth = 16;


	self->stream = work;
	self->channels = work->channel;
	self->sample_rate = work->samplerate;
	self->bit_par_sample = work->bitswidth;
	return true;
}

static void mp3_close(SoundStream* self) {
	DecodeWork *work = (DecodeWork *)self->stream;
	if (work != NULL) {
		FileSegmentStream_Finalize(&work->fss);
		mad_stream_finish(&work->stream);
		mad_frame_finish(&work->frame);
		mad_synth_finish(&work->synth);
		free(work->decoded_datas);
		free(work->input_buffer);
		free(work);
		self->stream = NULL;
	}
}

static signed int scale(mad_fixed_t sample)
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}


static bool mp3_decode(SoundStream *self, void *buffer, size_t szbuffer, size_t *szdecode) {
	DecodeWork *work = (DecodeWork *)self->stream;

	size_t outputed = 0;

	for (;;)
	{

		if (work->decoded_data_length >= szbuffer - outputed)
		{
			size_t copysize = szbuffer - outputed;
			memmove(((uint8_t*)buffer) + outputed, work->decoded_datas, copysize);
			memmove(work->decoded_datas, work->decoded_datas + copysize, work->decoded_data_length - copysize);
			work->decoded_data_length -= copysize;
			*szdecode = szbuffer;
			return true;
		}
		else if (work->decoded_data_length > 0)
		{
			memmove(((uint8_t*)buffer) + outputed, work->decoded_datas, work->decoded_data_length);
			outputed += work->decoded_data_length;
			work->decoded_data_length = 0;
		}
		assert(work->decoded_data_length == 0);

		if (work->stream.buffer == NULL || work->stream.error == MAD_ERROR_BUFLEN)
		{
			// 初回ロード時もしくはバッファ不足エラー時は新しくデータを読み込む

			if (work->stream.next_frame != NULL) {
				// 未デコードデータを入力バッファの先頭に移動させる
				if (work->input_buffer == work->stream.next_frame)
				{
					// デコード不可能＝EOFとみていいのでは？
					work->eof = true;
					*szdecode = outputed;
					return true;
				}
				size_t sz = work->input_length - (work->stream.next_frame - work->input_buffer);
				memmove(work->input_buffer, work->stream.next_frame, sz);
				work->input_length = sz;
			}

			size_t len;
			FileSegmentStream_Read(&work->fss, work->input_buffer + work->input_length, INPUT_BUFFER_LEN - work->input_length, &len);
			work->input_length += len;

			// バッファにデータを読み取る
			mad_stream_buffer(&work->stream, work->input_buffer, work->input_length);

			// バッファ不足エラーをリセット
			work->stream.error = MAD_ERROR_NONE;
		}

		// フレームを一つデコード
		int ret = mad_frame_decode(&work->frame, &work->stream);

		if (ret != 0)
		{
			if (work->stream.error == MAD_ERROR_BUFLEN)
			{
				continue;
			}
			else {
				// エラー発生
				*szdecode = 0;
				return false;
			}
		}

		// PCMに変換
		mad_synth_frame(&work->synth, &work->frame);
		size_t length = work->synth.pcm.length;
		mad_fixed_t *left = work->synth.pcm.samples[0];
		mad_fixed_t *right = work->synth.pcm.samples[1];

		// デコード後のデータを一旦溢れバッファに展開してループ頭に戻る
		if (MAD_NCHANNELS(&work->frame.header) == 2)
		{
			if (work->decoded_data_length + length * 4 >= work->decoded_data_capacity)
			{
				size_t newsize = work->decoded_data_length + (length * 4) * 2;
				void *newbuf = realloc(work->decoded_datas, newsize);
				if (newbuf == NULL)
				{
					*szdecode = 0;
					return false;
				}
				work->decoded_datas = newbuf;
				work->decoded_data_capacity = newsize;
			}

			uint8_t *data = work->decoded_datas + work->decoded_data_length;
			for (size_t i = 0; i < length; i++)
			{
				int16_t sample0 = (int16_t)scale(*left++);
				int16_t sample1 = (int16_t)scale(*right++);

				data[0] = (uint8_t)(sample0 >> 0);
				data[1] = (uint8_t)(sample0 >> 8);
				data[2] = (uint8_t)(sample1 >> 0);
				data[3] = (uint8_t)(sample1 >> 8);
				data += 4;
			}
			work->decoded_data_length += length * 4;
		}
		else if (MAD_NCHANNELS(&work->frame.header) == 1)
		{
			if (work->decoded_data_length + length * 2 >= work->decoded_data_capacity)
			{
				size_t newsize = work->decoded_data_length + (length * 2) * 2;
				void *newbuf = realloc(work->decoded_datas, newsize);
				if (newbuf == NULL)
				{
					*szdecode = 0;
					return false;
				}
				work->decoded_datas = newbuf;
				work->decoded_data_capacity = newsize;
			}

			uint8_t *data = (uint8_t *)work->decoded_datas + work->decoded_data_length;
			for (size_t i = 0; i < length; i++)
			{
				int16_t sample0 = (int16_t)scale(*left++);

				data[0] = (uint8_t)(sample0 >> 0);
				data[1] = (uint8_t)(sample0 >> 8);
				data += 2;
			}
			work->decoded_data_length += length * 2;
		}
		else
		{
			*szdecode = 0;
			return false;
		}
	}
}

static void mp3_seekstart(SoundStream* self) {
	DecodeWork *work = (DecodeWork *)self->stream;
	FileSegmentStream_Seek(&work->fss, 0, SEEK_SET);

	mad_stream_finish(&work->stream);
	mad_frame_finish(&work->frame);
	mad_synth_finish(&work->synth);

	mad_stream_init(&work->stream);
	mad_frame_init(&work->frame);
	mad_synth_init(&work->synth);
	work->decoded_data_length = 0;
	work->input_length = 0;
}

const SoundStreamReader Mp3Reader = {
	mp3_openfile,
	mp3_close,
	mp3_decode,
	mp3_seekstart,
};

