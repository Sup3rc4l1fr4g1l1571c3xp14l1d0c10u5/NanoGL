#include "Sound.h"
#include "../Debug/Debug.h"
#include "../String/String.h"

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#include "../String/CharCodeHelper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SAMPLING_FORMAT ma_format_f32
#define CHANNEL_COUNT 2
#define SAMPLING_RATE 48000
#define MAX_CHANNEL_COUNT 256

typedef enum {
    ST_EMPTY,
    ST_PLAY,
    ST_STOP,
} play_status_t;

typedef struct {
    ma_decoder      decoder;
    play_status_t   status;
    ma_bool32       is_loop;
} decoder_context_t;

typedef struct {
    ma_device_config    deviceConfig;
    ma_device           device;
    ma_uint32           decoder_count;
    ma_decoder_config   decoder_config;
    decoder_context_t   decoder_ctxs[1];
} context_t;

static context_t* ctx;


ma_uint32 read_and_mix_pcm_frames_f32(decoder_context_t* ctx, float* pOutputF32, ma_uint32 frameCount) {
    float temp[4096];
    const ma_uint32 tempCapInFrames = ma_countof(temp) / CHANNEL_COUNT;
    ma_uint32 totalFramesRead = 0;

    while (totalFramesRead < frameCount) {
        ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
        ma_uint32 framesToReadThisIteration = tempCapInFrames;
        if (framesToReadThisIteration > totalFramesRemaining) {
            framesToReadThisIteration = totalFramesRemaining;
        }

        /* デコード処理 */
        ma_uint64 framesReadThisIteration;
        ma_result result = ma_decoder_read_pcm_frames(&ctx->decoder, temp, framesToReadThisIteration, &framesReadThisIteration);
        if (result != MA_SUCCESS || framesReadThisIteration == 0) {
            break;
        }

        /* ミキシング処理（単に加算してるだけ） */
        for (ma_uint64 iSample = 0; iSample < framesReadThisIteration * CHANNEL_COUNT; ++iSample) {
            pOutputF32[totalFramesRead * CHANNEL_COUNT + iSample] += temp[iSample];
        }

        totalFramesRead += (ma_uint32)framesReadThisIteration;

        if (framesReadThisIteration < (ma_uint32)framesToReadThisIteration) {
            // 末尾まで再生
            if (ctx->is_loop) {
                // ループ再生の場合は、先頭までシークして再度ループ
                result = ma_decoder_seek_to_pcm_frame(&ctx->decoder, 0);
                if (result != MA_SUCCESS) {
                    // シーク失敗
                    break;
                }
            } else {
                // それ以外の場合は、打ち切り
                break;
            }
        }
    }

    return totalFramesRead;
}


 // 再生デバイスの出力を埋めるコールバック
static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* pOutputF32 = (float*)pOutput;

    MA_ASSERT(pDevice != NULL);
    MA_ASSERT(pDevice->playback.format == SAMPLING_FORMAT);
    MA_ASSERT(pDevice->pUserData != NULL);
    context_t* ctx = (context_t*)pDevice->pUserData;

    for (ma_uint32 iDecoder = 0; iDecoder < ctx->decoder_count; ++iDecoder) {
        if (ctx->decoder_ctxs[iDecoder].status == ST_PLAY) {
            ma_uint32 framesRead = read_and_mix_pcm_frames_f32(&ctx->decoder_ctxs[iDecoder], pOutputF32, frameCount);
            if (framesRead < frameCount) {
                ctx->decoder_ctxs[iDecoder].status = ST_STOP;
            }
        }
    }

    (void)pInput;
}
 
 static bool dispose_channel(ma_uint32 channel_no) {
	if (ctx->decoder_ctxs[channel_no].status != ST_EMPTY) {
		ma_decoder_uninit(&ctx->decoder_ctxs[channel_no].decoder);
		ctx->decoder_ctxs[channel_no].status = ST_EMPTY;
		return true;
	}
	return false;
}

static void Sound_Finalize(void) {
	if (ctx != NULL) {
	    ma_device_uninit(&ctx->device);

		for (ma_uint32 iDecoder = 0; iDecoder <ctx->decoder_count; ++iDecoder) {
			dispose_channel(iDecoder);
		}
		free(ctx);
		ctx = NULL;
	}
}

static bool Sound_Initialize_(void) {
    ctx = (context_t*)calloc(1, sizeof(context_t) + sizeof(decoder_context_t) * MAX_CHANNEL_COUNT);
    if (ctx == NULL) {
        return MA_FALSE;
    }
    ctx->decoder_count = MAX_CHANNEL_COUNT;

    // 共通のデコーダー設定を生成
    ctx->decoder_config = ma_decoder_config_init(SAMPLING_FORMAT, CHANNEL_COUNT, SAMPLING_RATE);

    // デバイス設定を生成
    ctx->deviceConfig = ma_device_config_init(ma_device_type_playback);
    ctx->deviceConfig.playback.format = SAMPLING_FORMAT;
    ctx->deviceConfig.playback.channels = CHANNEL_COUNT;
    ctx->deviceConfig.sampleRate = SAMPLING_RATE;
    ctx->deviceConfig.dataCallback = data_callback;
    ctx->deviceConfig.pUserData = ctx;

    // デバイス生成
    if (ma_device_init(NULL, &ctx->deviceConfig, &ctx->device) != MA_SUCCESS) {
        free(ctx);
        ctx = NULL;
        return MA_FALSE;
    }

    if (ma_device_start(&ctx->device) != MA_SUCCESS) {
        free(ctx);
        ctx = NULL;
        return MA_FALSE;
    }

    return MA_TRUE;
}

static bool Sound_Initialize(void) {
	Debug.PushBanner("**Error in Sound.Initialize");
	bool ret = Sound_Initialize_();
	Debug.PopBanner();
	return ret;
}

static bool load_file_to_channel(ma_uint32 channe_no, const char* filepath) {
	dispose_channel(channe_no);

    ma_result result = ma_decoder_init_file(filepath, &ctx->decoder_config, &(ctx->decoder_ctxs[channe_no].decoder));
    if (result != MA_SUCCESS) {
        // ロード失敗
        ctx->decoder_ctxs[channe_no].status = ST_EMPTY;
        return false;
    }

    ctx->decoder_ctxs[channe_no].is_loop = MA_FALSE;
    ctx->decoder_ctxs[channe_no].status = ST_STOP;

    return true;
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
	ret = load_file_to_channel(channelId, path);
#else
	if (is_contains_not_ascii(path,NULL))
	{
		char *sjispath = utf82sjis(path, NULL);
		ret = load_file_to_channel(channelId, sjispath);
		free(sjispath);
	} else
	{
		ret = load_file_to_channel(channelId, path);
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
	int ret = load_file_to_channel(channelId, path);
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
	dispose_channel(channelId);
	
	return true;
}

static bool Sound_Channel_Free(int channelId) {
	Debug.PushBanner("**Error in Sound.ChannelFree");
	bool ret = Sound_Channel_Free_(channelId);
	Debug.PopBanner();
	return ret;
}

static bool Sound_Channel_Play_(int channe_no, bool loop) {
	if (channe_no < 0 || 255 < channe_no)
	{
		Debug.PrintError("The range of the argument `channe_no` must be a 0 to 255. (You gave %d to the argument `channe_no`)", channe_no);
		return false;
	}
    if (ctx->decoder_ctxs[channe_no].status != ST_EMPTY) {
		// 先頭までシーク
		ctx->decoder_ctxs[channe_no].status = ST_STOP;
		ma_result result = ma_decoder_seek_to_pcm_frame(&ctx->decoder_ctxs[channe_no].decoder, 0);
		if (result != MA_SUCCESS) {
			// シーク失敗
			Debug.PrintError("The channel id=%d cannot play.", channe_no);
			return false;
		}
        ctx->decoder_ctxs[channe_no].status = ST_PLAY;
		ctx->decoder_ctxs[channe_no].is_loop = loop;
		return true;
    } else {
		Debug.PrintError("The channel id=%d cannot play.", channe_no);
		return false;
    }
}

static bool Sound_Channel_Play(int channelId, bool loop) {
	Debug.PushBanner("**Error in Sound.ChannelPlay");
	bool ret = Sound_Channel_Play_(channelId, loop);
	Debug.PopBanner();
	return ret;
}

static bool Sound_Channel_Stop_(int channe_no) {
	if (channe_no < 0 || 255 < channe_no)
	{
		Debug.PrintError("The range of the argument `channe_no` must be a 0 to 255. (You gave %d to the argument `channe_no`)", channe_no);
		return false;
	}
	ctx->decoder_ctxs[channe_no].status = ST_STOP;
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


