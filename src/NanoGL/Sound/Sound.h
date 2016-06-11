#ifndef _NanoGL_SOUND_H
#define _NanoGL_SOUND_H

#include "../config.h"
#include <stdbool.h>

extern const struct __tagSoundAPI {
	/**
	* @brief SoundAPI������������
	* @retval true  �������ɐ���
	* @retval false �������Ɏ��s
	* @note ���C�u�����̓����ŗ��p����Ă��邽�ߒʏ�͗��p�҂��Ăяo���K�v�͖����B
	*/
	bool(*Initialize)(void);

	/**
	 * @brief SoundAPI�̏I���������s��
	 * @note ���C�u�����̓����ŗ��p����Ă��邽�ߒʏ�͗��p�҂��Ăяo���K�v�͖����B
	 */
	void(*Finalize)(void);

	/**
	 * @brief ChannelCreate��UTF8��
	 */
	bool (*ChannelCreateUTF8)(int channelId, const char*path);

	/**
	 * @brief ���y�t�@�C��(wav��mp3)�� path ����ǂݎ�� �`�����l�� channelId �ԂɊ��蓖�Ă�B
	 * @param channelId ������̃`�����l���ԍ�
	 * @param path �ǂݍ��މ��y�t�@�C���̃p�X
	 * @retval true  ����
	 * @retval false ���s
	 */
	bool (*ChannelCreate)(int channelId, const char*path);

	/**
	 * @brief �`�����l�� channelId �ԂɊ��蓖�ĂĂ��鉹�y�f�[�^���J������
	 * @param channelId �J������`�����l���ԍ�
	 * @retval true  ����
	 * @retval false ���s
	 */
	bool (*ChannelFree)(int channelid);

	/**
	 * @brief �`�����l�� channelId �ԂɊ��蓖�ĂĂ��鉹�y�f�[�^���Đ�����
	 * @param channelId �Đ�����`�����l���ԍ�
	 * @param loop true:���[�v�Đ����� false:���[�v�Đ����Ȃ�
	 * @retval true  ����
	 * @retval false ���s
	 */
	bool (*ChannelPlay)(int channelid, bool loop);

	/**
	 * @brief �`�����l�� channelId �ԂɊ��蓖�ĂĂ��鉹�y�f�[�^���Đ����Ȃ��~����
	 * @param channelId ��~����`�����l���ԍ�
	 * @retval true  ����
	 * @retval false ���s
	 */
	bool (*ChannelStop)(int channelid);
} Sound;

#endif

