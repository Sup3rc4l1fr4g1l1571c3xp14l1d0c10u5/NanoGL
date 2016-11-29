#pragma once 

#include "../String/String.h"

/**
 * @brief ��{�I�ȃ_�C�A���O
 */
extern const struct __tagDialogAPI {
	/**
	* @brief �u�t�@�C�����J���v�_�C�A���O��\���B
	* @param title �_�C�A���O�{�b�N�X�̃^�C�g�������i���o���j
	* @param filter �_�C�A���O�{�b�N�X�̊g���q�t�B���^
	* @return �t�@�C�����I�΂ꂽ�ꍇ�̓p�X�����A�L�����Z�����ꂽ�ꍇ�Ȃǂ�NULL���i�[���ꂽstring_t���Ԃ�
	*/
	string_t(*OpenFileDialog)(const char *title, const char *filter);
	/**
	 * @brief OpenFileDialog�֐���UTF8��
	 */
	string_t(*OpenFileDialogUTF8)(const utf8_t *title, const utf8_t *filter);
	/**
	* @brief �u�t�@�C����ۑ�����v�_�C�A���O��\��
	* @param title �_�C�A���O�{�b�N�X�̃^�C�g�������i���o���j
	* @param filter �_�C�A���O�{�b�N�X�̊g���q�t�B���^
	* @return �t�@�C���������͂��ꂽ�ꍇ�̓p�X�����A�L�����Z�����ꂽ�ꍇ�Ȃǂ�NULL���i�[���ꂽstring_t���Ԃ�
	*/
	string_t(*SaveFileDialog)(const char *title, const char *filter);
	/**
	 * @brief SaveFileDialog�֐���UTF8��
	 */
	string_t(*SaveFileDialogUTF8)(const utf8_t *title, const utf8_t *filter);
} Dialog;



