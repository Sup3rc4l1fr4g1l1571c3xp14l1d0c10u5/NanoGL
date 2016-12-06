#include "./Dialog.h"
#include "../String/CharCodeHelper.h"
#include <wchar.h>
#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib,"Comdlg32.lib")

static char *fixup_filter(const char *filter) {
	if (filter == NULL) {
		return NULL;
	}
	size_t len = strlen(filter) + 2U;
	char *fixuped_filter = (char *)calloc(len,sizeof(char));
	if (fixuped_filter == NULL)
	{
		return NULL;
	}
	strcpy_s(fixuped_filter, len, filter);
	for (char *p = fixuped_filter; *p != '\0'; p++) {
		switch (*p) {
			case '|' : *p = '\0'; break;
			case '\n' : *p = '\0'; break;
			default: break;
		}
	}
	return fixuped_filter;
}

// ^<見出し> '|' <拡張子パターン> ( ';' <拡張子パターン> )* '\n'
string_t OpenFileDialog(const char *title, const char *filter) {
	OPENFILENAMEA ofn;
	char szPath[MAX_PATH];
	char szFile[MAX_PATH];

	memset(&ofn, 0, sizeof(ofn));
	memset(szPath, 0, sizeof(szPath));
	memset(szFile, 0, sizeof(szFile));

	GetCurrentDirectoryA(MAX_PATH, szPath);

	char *fixuped_filter = fixup_filter(filter);
	
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrInitialDir = szPath;	// 初期フォルダを
	ofn.lpstrFile = szFile;			// 選択ファイルを入れるバッファ
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = fixuped_filter;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST;
	
	if (GetOpenFileNameA(&ofn)) {
		free(fixuped_filter);
		return String.Create(szFile);
	} else {
		free(fixuped_filter);
		return String.Create(NULL);
	}
}

string_t OpenFileDialogUTF8(const char *title, const char *filter) {
	string_t ret = String.Create(NULL);
	if (is_contains_not_ascii(title, NULL) || is_contains_not_ascii(filter, NULL)) {
		char *sjis_title = utf82sjis(title, NULL);
		char *sjis_filtere = utf82sjis(filter, NULL);
		ret = OpenFileDialog(sjis_title, sjis_filtere);
		free(sjis_title);
		free(sjis_filtere);
	} else {
		ret = OpenFileDialog(title, filter);
	}
	if (ret.c_str == NULL) {
		return ret;
	} else {
		utf8_t *utf8 = sjis2utf8(ret.c_str, NULL);
		String.Free(ret);
		return String.Create(utf8);
	}
}

string_t SaveFileDialog(const char *title, const char *filter) {
	OPENFILENAMEA ofn;
	char szPath[MAX_PATH];
	char szFile[MAX_PATH];

	memset(&ofn, 0, sizeof(ofn));
	memset(szPath, 0, sizeof(szPath));
	memset(szFile, 0, sizeof(szFile));

	GetCurrentDirectoryA(MAX_PATH, szPath);

	char *fixuped_filter = fixup_filter(filter);

	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = fixuped_filter;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameA(&ofn)) {
		free(fixuped_filter);
		return String.Create(szFile);
	} else {
		free(fixuped_filter);
		return String.Create(NULL);
	}
}

string_t SaveFileDialogUTF8(const char *title, const char *filter) {
	string_t ret = String.Create(NULL);
	if (is_contains_not_ascii(title, NULL) || is_contains_not_ascii(filter, NULL)) {
		char *sjis_title = utf82sjis(title, NULL);
		char *sjis_filtere = utf82sjis(filter, NULL);
		ret = SaveFileDialog(sjis_title, sjis_filtere);
		free(sjis_title);
		free(sjis_filtere);
	} else {
		ret = SaveFileDialog(title, filter);
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
