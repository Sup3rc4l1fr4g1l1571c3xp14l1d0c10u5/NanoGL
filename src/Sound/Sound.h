#ifndef _NanoGL_SOUND_H
#define _NanoGL_SOUND_H

#include "../Config.h"
#include <stdbool.h>

extern const struct __tagSoundAPI {
	/**
	* @brief SoundAPIを初期化する
	* @retval true  初期化に成功
	* @retval false 初期化に失敗
	* @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	*/
	bool(*Initialize)(void);

	/**
	 * @brief SoundAPIの終了処理を行う
	 * @note ライブラリの内部で利用されているため通常は利用者が呼び出す必要は無い。
	 */
	void(*Finalize)(void);

	/**
	 * @brief ChannelCreateのUTF8版
	 */
	bool (*ChannelCreateUTF8)(int channelId, const char*path);

	/**
	 * @brief 音楽ファイル(wavかmp3)を path から読み取り チャンネル channelId 番に割り当てる。
	 * @param channelId 割当先のチャンネル番号
	 * @param path 読み込む音楽ファイルのパス
	 * @retval true  成功
	 * @retval false 失敗
	 */
	bool (*ChannelCreate)(int channelId, const char*path);

	/**
	 * @brief チャンネル channelId 番に割り当てている音楽データを開放する
	 * @param channelId 開放するチャンネル番号
	 * @retval true  成功
	 * @retval false 失敗
	 */
	bool (*ChannelFree)(int channelid);

	/**
	 * @brief チャンネル channelId 番に割り当てている音楽データを再生する
	 * @param channelId 再生するチャンネル番号
	 * @param loop true:ループ再生する false:ループ再生しない
	 * @retval true  成功
	 * @retval false 失敗
	 */
	bool (*ChannelPlay)(int channelid, bool loop);

	/**
	 * @brief チャンネル channelId 番に割り当てている音楽データが再生中なら停止する
	 * @param channelId 停止するチャンネル番号
	 * @retval true  成功
	 * @retval false 失敗
	 */
	bool (*ChannelStop)(int channelid);
} Sound;

#endif

