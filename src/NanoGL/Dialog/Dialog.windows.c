#include "./Dialog.h"
#include "../String/CharCodeHelper.h"
#include <wchar.h>
#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>

#pragma comment(lib,"Comdlg32.lib")

string_t OpenFileDialog(const char *prompt, const char *message) {
	OPENFILENAMEA ofn;
	char szPath[MAX_PATH];
	char szFile[MAX_PATH];
	(void*)message;

	memset(&ofn, 0, sizeof(ofn));
	memset(szPath, 0, sizeof(szPath));
	memset(szFile, 0, sizeof(szFile));

	GetCurrentDirectoryA(MAX_PATH, szPath);

	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrInitialDir = szPath;	// 初期フォルダを
	ofn.lpstrFile = szFile;			// 選択ファイルを入れるバッファ
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "すべてのファイル(*.*)\0*.*\0";
	ofn.lpstrTitle = prompt;
	ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST;
	
	if (GetOpenFileNameA(&ofn)) {
		return String.Create(szFile);
	} else {
		return String.Create(NULL);
	}
}

string_t OpenFileDialogUTF8(const char *prompt, const char *message) {
	string_t ret = String.Create(NULL);
	if (is_contains_not_ascii(prompt, NULL) || is_contains_not_ascii(message, NULL)) {
		char *sjisprompt = utf82sjis(prompt, NULL);
		char *sjismessage = utf82sjis(message, NULL);
		ret = OpenFileDialog(sjisprompt, sjismessage);
		free(sjisprompt);
		free(sjismessage);
	} else {
		ret = OpenFileDialog(prompt, message);
	}
	if (ret.c_str == NULL) {
		return ret;
	} else {
		utf8_t *utf8 = sjis2utf8(ret.c_str, NULL);
		String.Free(ret);
		return String.Create(utf8);
	}
}

string_t SaveFileDialog(const char *prompt, const char *message) {
	OPENFILENAMEA ofn;
	char szPath[MAX_PATH];
	char szFile[MAX_PATH];
	(void*)message;

	memset(&ofn, 0, sizeof(ofn));
	memset(szPath, 0, sizeof(szPath));
	memset(szFile, 0, sizeof(szFile));

	GetCurrentDirectoryA(MAX_PATH, szPath);

	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrInitialDir = szPath;	// 初期フォルダを
	ofn.lpstrFile = szFile;			// 選択ファイルを入れるバッファ
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "すべてのファイル(*.*)\0*.*\0";
	ofn.lpstrTitle = prompt;
	ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameA(&ofn)) {
		return String.Create(szFile);
	} else {
		return String.Create(NULL);
	}
}

string_t SaveFileDialogUTF8(const utf8_t *prompt, const utf8_t *message) {
	string_t ret = String.Create(NULL);
	if (is_contains_not_ascii(prompt, NULL) || is_contains_not_ascii(message, NULL)) {
		char *sjisprompt = utf82sjis(prompt, NULL);
		char *sjismessage = utf82sjis(message, NULL);
		ret = SaveFileDialog(sjisprompt, sjismessage);
		free(sjisprompt);
		free(sjismessage);
	} else {
		ret = SaveFileDialog(prompt, message);
	}
	if (ret.c_str == NULL) {
		return ret;
	} else {
		utf8_t *utf8 = sjis2utf8(ret.c_str, NULL);
		String.Free(ret);
		return String.Create(utf8);
	}
}

const struct __tagDialogAPI Dialog = {
	OpenFileDialog,
	OpenFileDialogUTF8,
	SaveFileDialog,
	SaveFileDialogUTF8,
};
