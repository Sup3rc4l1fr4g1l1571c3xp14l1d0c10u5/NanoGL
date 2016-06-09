#pragma once 

#include "../config.h"
#include <stdarg.h>

/**
 * 文字列データ型
 * const char*にキャストするとC言語のstr～系関数でも使える
 */
typedef struct __tagString {
	char *c_str;
} string_t;

/**
 * @brief 文字列
 */
extern const struct __tagStringAPI {
	/**
	 * @brief 文字列 str からstring_t型の文字列を生成する
	 * @param str 生成元の文字列(NULLの場合は空文字列を生成)
	 */
	string_t(*Create)(const char *str);

	/**
	* @return string_t型の文字列を破棄する
	*/
	void(*Free)(string_t str);

	/**
	* @brief printf と同じ書式でstring_t型の文字列を作ってくれる
	* @return 作成した文字列のポインタ。不要になったらString.Free()で破棄すること。
	*/
	string_t(*FormatText)(const char *format, ...);

	/**
	* @brief printf と同じ書式でstring_t型の文字列を作ってくれる
	* @return 作成した文字列のポインタ。不要になったらString.Free()で破棄すること。
	*/
	string_t(*FormatTextV)(const char *format, va_list args);

	/**
	* @brief str1にstr2を連結するした結果を返す。
	* @param str1 連結先の文字列。関数中で破棄されるため、この関数の呼出し後では使用できない
	* @param str2 連結する文字列。
	* @return 連結結果の文字列。不要になったらString.Free()で破棄すること。
	*/
	string_t(*Join)(string_t str1, const char *str2);

	struct __tagStringLocaleAPI
	{
		size_t(*Count)(const char *str, const char *end);
		const char *(*Inc)(const char *str, size_t n);
	} Utf8, Sjis;

} String;
