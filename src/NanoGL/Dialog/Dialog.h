#pragma once 

#include "../String/String.h"

/**
 * @brief 基本的なダイアログ
 */
extern const struct __tagDialogAPI {
	/**
	* @brief 「ファイルを開く」ダイアログを表示。
	* @param title ダイアログボックスのタイトル部分（見出し）
	* @param filter ダイアログボックスの拡張子フィルタ
	* @return ファイルが選ばれた場合はパス名が、キャンセルされた場合などはNULLが格納されたstring_tが返る
	*/
	string_t(*OpenFileDialog)(const char *title, const char *filter);
	/**
	 * @brief OpenFileDialog関数のUTF8版
	 */
	string_t(*OpenFileDialogUTF8)(const utf8_t *title, const utf8_t *filter);
	/**
	* @brief 「ファイルを保存する」ダイアログを表示
	* @param title ダイアログボックスのタイトル部分（見出し）
	* @param filter ダイアログボックスの拡張子フィルタ
	* @return ファイル名が入力された場合はパス名が、キャンセルされた場合などはNULLが格納されたstring_tが返る
	*/
	string_t(*SaveFileDialog)(const char *title, const char *filter);
	/**
	 * @brief SaveFileDialog関数のUTF8版
	 */
	string_t(*SaveFileDialogUTF8)(const utf8_t *title, const utf8_t *filter);
} Dialog;



