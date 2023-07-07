#ifndef _NANOGL_STRING_H
#define _NANOGL_STRING_H

#include "../Config.h"
#include <stdarg.h>
#include <stddef.h>

/**
 * 文字列データ型
 * const char*にキャストするとC言語のstr～系関数でも使える
 */
typedef struct __tagString {
	char *c_str;
} string_t;

/**
 * utf8を明示的に示したいときの型
 */
typedef char utf8_t;

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
	* @brief str1にstr2を連結した結果を返す。
	* @param str1 連結先の文字列
	* @param str2 連結する文字列。
	* @return 連結結果の文字列。不要になったらString.Free()で破棄すること。
	*/
	string_t(*Join)(const char *str1, const char *str2);

	/**
	* @brief Joinと同様の動作を行うがstr1を更新する。
	* @param str1 連結先の文字列の参照
	* @param str2 連結する文字列。
	* @return str1
	*/
	string_t*(*JoinDirect)(string_t *str1, const char *str2);

	/**
	* @brief target中の部分文字列fromをtoに置き換える。
	* @param target 対象の文字列。
	* @param from 置換対象の部分文字列
	* @param to   置換後の部分文字列
	* @return 置換結果の文字列。不要になったらString.Free()で破棄すること。
	*/
	string_t (*Replace)(const char *target, const char *from, const char *to);

	/**
	* @brief Replaceと同様の動作を行うがtargetを更新する。
	* @param target 処理対象の文字列の参照
	* @param from 置換対象の部分文字列
	* @param to   置換後の部分文字列
	* @return target
	*/
	string_t* (*ReplaceDirect)(string_t *target, const char *from, const char *to);
	
	struct __tagStringLocaleAPI
	{
		size_t(*Count)(const char *str, const char *end);
		const char *(*Inc)(const char *str, size_t n);
	} Utf8, Sjis;

} String;

#endif
