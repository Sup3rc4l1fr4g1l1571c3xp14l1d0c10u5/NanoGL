#pragma once 

#include "../string/string.h"

/**
 * @brief ��{�I�ȃ_�C�A���O
 */
extern const struct __tagDialogAPI {
	/**
	* @brief �u�t�@�C�����J���v�_�C�A���O��\���B
	* @param prompt �_�C�A���O�{�b�N�X�̌��o��
	* @param message �_�C�A���O�{�b�N�X�̃��b�Z�[�W(�����MacOSX��p)
	* @return �t�@�C�����I�΂ꂽ�ꍇ�̓p�X�����A�L�����Z�����ꂽ�ꍇ�Ȃǂ�NULL���i�[���ꂽstring_t���Ԃ�
	*/
	string_t(*OpenFileDialog)(const char *prompt, const char *message);
	/**
	 * @brief OpenFileDialog�֐���UTF8��
	 */
	string_t(*OpenFileDialogUTF8)(const utf8_t *prompt, const utf8_t *message);
	/**
	* @brief �u�t�@�C����ۑ�����v�_�C�A���O��\��
	* @param prompt �_�C�A���O�{�b�N�X�̌��o��
	* @param message �_�C�A���O�{�b�N�X�̃��b�Z�[�W(�����MacOSX��p)
	* @return �t�@�C���������͂��ꂽ�ꍇ�̓p�X�����A�L�����Z�����ꂽ�ꍇ�Ȃǂ�NULL���i�[���ꂽstring_t���Ԃ�
	*/
	string_t(*SaveFileDialog)(const char *prompt, const char *message);
	/**
	 * @brief SaveFileDialog�֐���UTF8��
	 * @param prompt �_�C�A���O�{�b�N�X�̌��o��
	 * @param message �_�C�A���O�{�b�N�X�̃��b�Z�[�W(�����MacOSX��p)
	 */
	string_t(*SaveFileDialogUTF8)(const utf8_t *prompt, const utf8_t *message);
} Dialog;



