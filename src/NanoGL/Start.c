#include "NanoGL.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#pragma comment(lib,"shell32.lib")
#include <Avrt.h>
#pragma comment(lib,"Avrt.lib")
#endif
#include <stdlib.h>

#if defined(_WIN32)

static HANDLE mmcssHandle = 0;

static void MMCSS_Initialize(void) {
	if (timeBeginPeriod(1U) != TIMERR_NOERROR) {
		Debug.PrintError("Failed to setup minimum resolution for periodic timers.");
	} else {
		DWORD  mmcssTaskIndex = 0UL;
		mmcssHandle = AvSetMmThreadCharacteristics("Pro Audio", &mmcssTaskIndex);
		if (mmcssHandle == 0) {
			Debug.PrintError("Failed to set MMCSS task priority.");
		}
	}
}

static void MMCSS_Finalize(void) {
	if (mmcssHandle != 0) {
		AvRevertMmThreadCharacteristics(mmcssHandle);
		mmcssHandle = 0;
		timeEndPeriod(1);
	}
}

#endif

#ifdef _WIN32

static wchar_t *mb2wide(const char *src) {
	int len = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
	if (len == 0) {
		return NULL;
	}

	wchar_t *dst = (wchar_t*) calloc(len + 1, sizeof(wchar_t));
	if (dst == NULL) {
		return NULL;
	}

	int dstlen = MultiByteToWideChar(CP_ACP, 0, src, -1, dst, len + 1);
	if (dstlen == 0) {
		free(dst);
		return NULL;
	}
	dst[dstlen] = L'\0';

	return dst;
}

static char *wide2mb(wchar_t *src)
{
	int len = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	if (len == 0) {
		return NULL;
	}

	char *dst = (char*) calloc(len + 1, sizeof(char));
	if (dst == NULL) {
		return NULL;
	}

	int dstlen = WideCharToMultiByte(CP_ACP, 0, src, -1, dst, len + 1, NULL, NULL);
	if (dstlen == 0) {
		free(dst);
		return NULL;
	}
	dst[dstlen] = '\0';

	return dst;
}

static bool GetCommandLineArgs(int *argc, char ***argv) {

	wchar_t *wargv = mb2wide(GetCommandLine());
	if (wargv == NULL) {
		return false;
	}
	
	wchar_t **wargvs = CommandLineToArgvW(wargv, argc);
	
	free(wargv);
	
	if (wargvs == NULL) {
		return false;
	}
	
	char** argvs = (char**) calloc(*argc, sizeof(char*));
	if (argvs == NULL) {
		goto failed;
	}

	for (int i = 0; i < *argc; ++i) {
		argvs[i] = wide2mb(wargvs[i]);
		if (argvs[i] == NULL) {
			goto failed2;
		}
	}
	LocalFree(wargvs);
	
	*argv = argvs;
	
	return true;

failed2:
	for (int i = 0; i < *argc; ++i) {
		if (argvs[i] != NULL) {
			free(argvs[i]);
			argvs[i] = NULL;
		}
	}
failed:
	LocalFree(wargvs);
	return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;

	int argc;
	char **argv;
	
	if (GetCommandLineArgs(&argc, &argv) == false) {
		return EXIT_FAILURE;
	}
	
#else
int main(int argc, char *argv[]) {
#endif

#ifdef _WIN32
	MMCSS_Initialize();
#endif
	System.Run(argc, (const char **)argv);

#ifdef _WIN32
	MMCSS_Finalize();
#endif

	return EXIT_SUCCESS;
}

