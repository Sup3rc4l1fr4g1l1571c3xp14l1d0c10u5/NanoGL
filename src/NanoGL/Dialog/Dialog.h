#pragma once 

#include "../string/string.h"

/**
 * @brief 基本的なダイアログ
 */
extern const struct __tagDialogAPI {
	/**
	* @brief 「ファイルを開く」ダイアログを表示。
	* @param prompt ダイアログボックスの見出し
	* @param message ダイアログボックスのメッセージ(現状はMacOSX専用)
	* @return ファイルが選ばれた場合はパス名が、キャンセルされた場合などはNULLが格納されたstring_tが返る
	*/
	string_t(*OpenFileDialog)(const char *prompt, const char *message);
	/**
	 * @brief OpenFileDialog関数のUTF8版
	 */
	string_t(*OpenFileDialogUTF8)(const utf8_t *prompt, const utf8_t *message);
	/**
	* @brief 「ファイルを保存する」ダイアログを表示
	* @param prompt ダイアログボックスの見出し
	* @param message ダイアログボックスのメッセージ(現状はMacOSX専用)
	* @return ファイル名が入力された場合はパス名が、キャンセルされた場合などはNULLが格納されたstring_tが返る
	*/
	string_t(*SaveFileDialog)(const char *prompt, const char *message);
	/**
	 * @brief SaveFileDialog関数のUTF8版
	 * @param prompt ダイアログボックスの見出し
	 * @param message ダイアログボックスのメッセージ(現状はMacOSX専用)
	 */
	string_t(*SaveFileDialogUTF8)(const utf8_t *prompt, const utf8_t *message);
} Dialog;



