#include "./Dialog.h"
#include "../String/String.h"

#define _BSD_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/**
 * GNOME: use zenity
 *     zenity --title='<title>' --file-selection [--save] (--file-filter="<caption>|<pattern>")*
 */

string_t OpenFileDialog(const char *title, const char *filter) {
	string_t cmd;
	
	{
		string_t stitle = String.Create(title);
		String.ReplaceDirect(&stitle, "\\", "'\\\\");
		String.ReplaceDirect(&stitle, "'", "'\\''");
		cmd = String.FormatText("zenity --file-selection --title='%s'", stitle.c_str);
		String.Free(stitle);
	}
	
	if (filter != NULL) {
		char *work = strdup(filter);
		char *p = work;
		while (*p != '\0') {
			char *caption = p;
			while (*p != '|') {
				if (*p == '\0') { goto exit_parse; }
				p++;
			}
			*p = '\0';
			p++;
			
			bool last = false;
			while (!last) {
				char *ext = p;
				while (*p != ';' && *p != '\n' && *p != '\0') {
					p++;
				}
				if (*p != ';') {
					last = true;
				}
				*p = '\0';
				p++;

				// 結合
				string_t sext = String.Create(ext);
				String.ReplaceDirect(&sext, "\\", "'\\\\");
				String.ReplaceDirect(&sext, "'", "'\\''");
				string_t sfilter = String.FormatText(" --file-filter='%s'", sext.c_str);
				String.JoinDirect(&cmd, sfilter.c_str);
				String.Free(sfilter);
				String.Free(sext);
			}
		}
	exit_parse:
		free(work);
	}
	
	FILE *fp = popen(cmd.c_str, "r");
	String.Free(cmd);
	if (fp == NULL) {
		return String.Create(NULL);
	}
	
	string_t result = String.Create("");
	char buf[256];
	while (fgets(buf, sizeof(buf) / sizeof(buf[0]), fp) != NULL) {
		String.JoinDirect(&result, buf);
	}
	pclose(fp);
	
	char *last_lf = strrchr(result.c_str,'\n');
	if (last_lf != NULL && last_lf[1] == '\0') {
		last_lf[0] = '\0';
	}
	return result;
}

string_t OpenFileDialogUTF8(const utf8_t *title, const utf8_t *filter) {
    return OpenFileDialog(title, filter);
}

string_t SaveFileDialog(const char *title, const char *filter) {
	string_t cmd;
	
	{
		string_t stitle = String.Create(title);
		String.ReplaceDirect(&stitle, "\\", "'\\\\");
		String.ReplaceDirect(&stitle, "'", "'\\''");
		cmd = String.FormatText("zenity --file-selection --save --title='%s'", stitle.c_str);
		String.Free(stitle);
	}
	
	if (filter != NULL ) {
		char *work = strdup(filter);
		char *p = work;
		while (*p != '\0') {
			char *caption = p;
			while (*p != '|') {
				if (*p == '\0') { goto exit_parse; }
				p++;
			}
			*p = '\0';
			p++;
			
			bool last = false;
			while (!last) {
				char *ext = p;
				while (*p != ';' && *p != '\n' && *p != '\0') {
					p++;
				}
				if (*p != ';') {
					last = true;
				}
				*p = '\0';
				p++;

				string_t sext = String.Create(ext);
				String.ReplaceDirect(&sext, "\\", "'\\\\");
				String.ReplaceDirect(&sext, "'", "'\\''");
				string_t sfilter = String.FormatText(" --file-filter='%s'", sext.c_str);
				String.JoinDirect(&cmd, sfilter.c_str);
				String.Free(sfilter);
				String.Free(sext);
			}
		}
	exit_parse:
		free(work);
	}
	
	FILE *fp = popen(cmd.c_str, "r");
	String.Free(cmd);
	if (fp == NULL) {
		return String.Create(NULL);
	}
	
	string_t result = String.Create("");
	char buf[256];
	while (fgets(buf, sizeof(buf) / sizeof(buf[0]), fp) != NULL) {
		String.JoinDirect(&result, buf);
	}
	pclose(fp);
	
	char *last_lf = strrchr(result.c_str,'\n');
	if (last_lf != NULL && last_lf[1] == '\0') {
		last_lf[0] = '\0';
	}
	
	return result;
}

string_t SaveFileDialogUTF8(const utf8_t *title, const utf8_t *filter) {
    return SaveFileDialog(title,filter);
}

const struct __tagDialogAPI Dialog = {
    OpenFileDialog,
    OpenFileDialogUTF8,
    SaveFileDialog,
    SaveFileDialogUTF8,
};
