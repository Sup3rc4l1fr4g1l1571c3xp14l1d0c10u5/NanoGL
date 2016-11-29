#pragma once 

#include "../config.h"
#include <stdarg.h>
#include <stddef.h>

/**
 * ������f�[�^�^
 * const char*�ɃL���X�g�����C�����str�`�n�֐��ł��g����
 */
typedef struct __tagString {
	char *c_str;
} string_t;

/**
 * utf8�𖾎��I�Ɏ��������Ƃ��̌^
 */
typedef char utf8_t;

/**
 * @brief ������
 */
extern const struct __tagStringAPI {
	/**
	 * @brief ������ str ����string_t�^�̕�����𐶐�����
	 * @param str �������̕�����(NULL�̏ꍇ�͋󕶎���𐶐�)
	 */
	string_t(*Create)(const char *str);

	/**
	* @return string_t�^�̕������j������
	*/
	void(*Free)(string_t str);

	/**
	* @brief printf �Ɠ���������string_t�^�̕����������Ă����
	* @return �쐬����������̃|�C���^�B�s�v�ɂȂ�����String.Free()�Ŕj�����邱�ƁB
	*/
	string_t(*FormatText)(const char *format, ...);

	/**
	* @brief printf �Ɠ���������string_t�^�̕����������Ă����
	* @return �쐬����������̃|�C���^�B�s�v�ɂȂ�����String.Free()�Ŕj�����邱�ƁB
	*/
	string_t(*FormatTextV)(const char *format, va_list args);

	/**
	* @brief str1��str2��A���������ʂ�Ԃ��B
	* @param str1 �A����̕�����
	* @param str2 �A�����镶����B
	* @return �A�����ʂ̕�����B�s�v�ɂȂ�����String.Free()�Ŕj�����邱�ƁB
	*/
	string_t(*Join)(const char *str1, const char *str2);

	/**
	* @brief Join�Ɠ��l�̓�����s����str1���X�V����B
	* @param str1 �A����̕�����̎Q��
	* @param str2 �A�����镶����B
	* @return str1
	*/
	string_t*(*JoinDirect)(string_t *str1, const char *str2);

	/**
	* @brief target���̕���������from��to�ɒu��������B
	* @param target �Ώۂ̕�����B
	* @param from �u���Ώۂ̕���������
	* @param to   �u����̕���������
	* @return �u�����ʂ̕�����B�s�v�ɂȂ�����String.Free()�Ŕj�����邱�ƁB
	*/
	string_t (*Replace)(const char *target, const char *from, const char *to);

	/**
	* @brief Replace�Ɠ��l�̓�����s����target���X�V����B
	* @param target �����Ώۂ̕�����̎Q��
	* @param from �u���Ώۂ̕���������
	* @param to   �u����̕���������
	* @return target
	*/
	string_t* (*ReplaceDirect)(string_t *target, const char *from, const char *to);
	
	struct __tagStringLocaleAPI
	{
		size_t(*Count)(const char *str, const char *end);
		const char *(*Inc)(const char *str, size_t n);
	} Utf8, Sjis;

} String;
