/*=============================================================================
	Add "font on" session	(c)TORO
=============================================================================*/
#define STRICT
#include <windows.h>
#include <shlobj.h>
#include "TOROWIN.H"
#include "fonton.H"
#pragma hdrstop

#define FONT_DIR_NAME L"font"
BOOL regist = TRUE; // -r 登録/解除の動作
BOOL silent = FALSE; // -s エラー表示を出さない

const WCHAR title[] = L"fonton";
const WCHAR RegFont[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

typedef struct {
	WCHAR *error_fontdir;
	WCHAR *error_exist;
	WCHAR *error_regist;
	WCHAR *error_release;
} MSGTABLE;

MSGTABLE msgE = {
	FONT_DIR_NAME L" open error.\n"
	L"Put the font files in the " FONT_DIR_NAME L" folder.",
	L"%s is already registered by system.\n",
	L"%s failed to registered.\n",
	L"%s failed to remove.\n"
};

MSGTABLE msgJ = {
	FONT_DIR_NAME L" フォルダを開けませんでした\n"
	  FONT_DIR_NAME L" フォルダにフォントファイルを入れてください",
	L"%s はシステムに登録済みです。\n",
	L"%s は登録できません。\n",
	L"%s は解除できません。\n"
};

#define message_size 0x1000
#define IsMsgSpace() ((messageptr - message) < (message_size - (MAX_PATH + 64)))
TCHAR *message, *messageptr; // 結果保存用
MSGTABLE *msgs;

void DoFont(const WCHAR *basepath, WIN32_FIND_DATA *ff, BOOL regist)
{
	HKEY HK;
	BOOL exist = FALSE;
	WCHAR path[MAX_PATH];

	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegFont, 0, KEY_READ, &HK) == ERROR_SUCCESS ){
		DWORD index = 0;

		for ( ;; ){
			WCHAR key[MAX_PATH], value[MAX_PATH], *existfont;
			DWORD keylen, valuesize;

			keylen = TSIZEOFW(key);
			valuesize = sizeof(value);
			if ( RegEnumValue(HK, index, key, &keylen, NULL, NULL, (BYTE*)value, &valuesize) != ERROR_SUCCESS){
				break;
			}
			value[valuesize / sizeof(WCHAR)] = '\0';
			existfont = strchrW(value, '\\');
			existfont = (existfont != NULL) ? existfont + 1 : value;
			if ( stricmpW(existfont, ff->cFileName) != 0 ){
				index++;
				continue;
			}
			exist = TRUE;
			if ( regist && IsMsgSpace() ){
				messageptr += wsprintfW(messageptr, msgs->error_exist, ff->cFileName);
			}
			break;
		}
		RegCloseKey(HK);
	}

	if ( exist ) return; // 登録済みのため処理しない

	strcpyW(path, basepath);
	strcatW(path, ff->cFileName);
	if ( regist ){
		if ( (AddFontResource(path) == 0) && IsMsgSpace() ){
			messageptr += wsprintfW(messageptr, msgs->error_regist, ff->cFileName);
		}
	}else{
		if ( (RemoveFontResource(path) == 0) && IsMsgSpace() ){
			messageptr += wsprintfW(messageptr, msgs->error_release, ff->cFileName);
		}
	}
}

#pragma argsused
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int cmdShow)
{
	WCHAR messagebuf[0x1000], basepath[MAX_PATH], path[MAX_PATH], *cmd, *ptr;
	HANDLE hFile;
	WIN32_FIND_DATA ff;
	int result = EXIT_SUCCESS;

	message = messageptr = messagebuf;
	msgs = (LOWORD(GetUserDefaultLCID()) == LCID_JAPANESE) ? &msgJ : &msgE;

	cmd = GetCommandLine();
	ptr = strrchrW(cmd, '\\');
	if ( ptr == NULL ) ptr = cmd;
	if ( strstrW(ptr, L"-r") != NULL ) regist = FALSE;
	if ( strstrW(ptr, L"-s") != NULL ) silent = TRUE;

	GetModuleFileName(hInstance, basepath, TSIZEOFW(path));
	ptr = wcsrchr(basepath, '\\');
	if ( ptr != NULL ) *ptr = '\0';
	wcscat(basepath, L"\\" FONT_DIR_NAME L"\\");

	wcscpy(path, basepath);
	wcscat(path, L"*");
	hFile = FindFirstFile(path, &ff);
	if ( hFile == INVALID_HANDLE_VALUE ){
		result = GetLastError();
		messageptr += wsprintfW(messageptr, msgs->error_fontdir);
	}else{
		do {
			if ( !IsRelativeDir(ff.cFileName) &&
				 !(ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
				DoFont(basepath, &ff, regist);
			}
		}while( IsTrue(FindNextFile(hFile, &ff)) );
		FindClose(hFile);
	}
	if ( !silent && (message != messageptr) ){
		MessageBoxW(GetForegroundWindow(), message, title, MB_OK);
	}
	return result;
}
