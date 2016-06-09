#pragma once 

#include "../config.h"
#include <stdarg.h>

/**
 * ������f�[�^�^
 * const char*�ɃL���X�g�����C�����str�`�n�֐��ł��g����
 */
typedef struct __tagString {
	char *c_str;
} string_t;

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
	* @brief str1��str2��A�����邵�����ʂ�Ԃ��B
	* @param str1 �A����̕�����B�֐����Ŕj������邽�߁A���̊֐��̌ďo����ł͎g�p�ł��Ȃ�
	* @param str2 �A�����镶����B
	* @return �A�����ʂ̕�����B�s�v�ɂȂ�����String.Free()�Ŕj�����邱�ƁB
	*/
	string_t(*Join)(string_t str1, const char *str2);

	struct __tagStringLocaleAPI
	{
		size_t(*Count)(const char *str, const char *end);
		const char *(*Inc)(const char *str, size_t n);
	} Utf8, Sjis;

} String;
