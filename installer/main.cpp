/*
Copyright (c) 2006-2008 dogbert <dogber1@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "main.h"


void PrintLastError(LPCSTR function)
{
	LPVOID	lpMsgBuf;
	DWORD   errorid = GetLastError();

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorid, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );
	MessageBox(NULL, (LPCSTR)lpMsgBuf, function, MB_ICONEXCLAMATION | MB_OK);
	LocalFree(lpMsgBuf);
}

BOOL deleteCMIKeys()
{
	HKEY key;
	unsigned int i;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), NULL, KEY_SET_VALUE, &key) != ERROR_SUCCESS) {
		PrintLastError("RegOpenKeyEx()");
		return FALSE;
	}
	for (i=0;i<NumberOfCMIKeys;i++) {
		RegDeleteValue(key, CMIKeys[i]);
	}
	RegCloseKey(key);
	return TRUE;
}

BOOL doCMIKeysExist()
{
	HKEY key;
	unsigned int i;
	BOOL result = FALSE;
	DWORD size;
	LONG rqvRes;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), NULL, KEY_READ, &key) != ERROR_SUCCESS) {
		PrintLastError("RegOpenKeyEx()");
		return FALSE;
	}
	for (i=0;i<NumberOfCMIKeys;i++) {
		size = 0;
		rqvRes = RegQueryValueEx(key, CMIKeys[i], NULL, NULL, NULL, &size);
		result |= (rqvRes == ERROR_SUCCESS) || (rqvRes == ERROR_MORE_DATA);
	}
	RegCloseKey(key);

	return result;
}

void writeUninstallerKeys()
{
	TCHAR SysDir[MAX_PATH];
	unsigned int len;
	HKEY key;

	if (GetSystemDirectory(SysDir, ARRAYSIZE(SysDir))==0) {
		PrintLastError("GetSystemDirectory()");
		return;
	}
	len = strlen(SysDir);
	strcat(SysDir, Uninstaller);

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\CMIDriver"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &key, NULL) != ERROR_SUCCESS) {
		PrintLastError("RegCreateKeyEx()");
		return;
	}
	RegSetValueEx(key, "DisplayName", NULL, REG_SZ, (BYTE*)&DisplayName, sizeof(DisplayName));
	RegSetValueEx(key, "URLInfoAbout", NULL, REG_SZ, (BYTE*)&URLInfoAbout, sizeof(URLInfoAbout));
	RegSetValueEx(key, "Publisher", NULL, REG_SZ, (BYTE*)&Publisher, sizeof(Publisher));
	RegSetValueEx(key, "UninstallString", NULL, REG_SZ, (BYTE*)&SysDir, strlen(SysDir));

	SysDir[len] = 0;
	strcat(SysDir, DisplayIcon);
	RegSetValueEx(key, "DisplayIcon", NULL, REG_SZ, (BYTE*)&SysDir, strlen(SysDir));

	RegCloseKey(key);
}


void closeCMIApps()
{
	HWND hWnd;

	if (hWnd = FindWindow(NULL, "PCI 3D Audio Configuration")) {
		// the crap won't eat WM_CLOSE
		ShowWindow(hWnd, SW_NORMAL);
		hWnd = FindWindowEx(hWnd, NULL, NULL, "OK");
		SendMessage(hWnd, BM_CLICK, 0, 0);
	}
	if (hWnd = FindWindow("Hidden Window class", "Hidden PCI Main Window")) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
	if (hWnd = FindWindow(NULL, "Mixer")) {
		if (FindWindowEx(hWnd, NULL, "AfxFrameOrView42s", NULL)) {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
	if (hWnd = FindWindow(NULL, "Aureon 5.1 Fun Mixer")) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
	if (hWnd = FindWindow(NULL, "Aureon 7.1 PCI")) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
	if (hWnd = FindWindow("cmiControlPanel", NULL)) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
}

UINT findWaveDeviceID()
{
	WAVEOUTCAPS  woc;
	UINT         i, numDev;

	numDev = waveOutGetNumDevs();
	for (i=0;i<numDev;i++) {
	    if (!waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS))) {
			if ((CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), NORM_IGNORECASE, woc.szPname, -1, TEXT("C-Media Wave Device"), -1) == CSTR_EQUAL) ||
			    (CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), NORM_IGNORECASE, woc.szPname, -1, TEXT("Speakers (CMI8738/C3DX PCI Audi"), -1) == CSTR_EQUAL) ||
			    (CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), NORM_IGNORECASE, woc.szPname, -1, TEXT("Aureon 5.1 Fun Wave"), -1) == CSTR_EQUAL) ||
			    (CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), NORM_IGNORECASE, woc.szPname, -1, TEXT("Speakers (TerraTec Aureon 5.1 F"), -1) == CSTR_EQUAL)) {
				return i;
			}
    	}
	}
	return 0xFFFFFFFF;
}

// old versions of the official drivers crash on unloading
// workaround: open sound card to force a reboot
void openCMIWave()
{
	WAVEFORMATEX         wfx;
	UINT waveID = findWaveDeviceID();

	wfx.wFormatTag      = WAVE_FORMAT_PCM;
	wfx.wBitsPerSample  = 16;
	wfx.nChannels       = (WORD)2;
	wfx.nSamplesPerSec  = 44100;
	wfx.nAvgBytesPerSec = 44100 * (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.nBlockAlign     = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.cbSize          = 0;

	if (waveID != 0xFFFFFFFF) {
		waveOutOpen(&hWave, waveID, (WAVEFORMATEX*)&(wfx), 0, 0, CALLBACK_NULL);
	}
}

BOOL findInfHook(TCHAR* infContent, DWORD size)
{
	DWORD devIDLen = _tcslen(INF_HOOK);
	DWORD i,j = 0;

	if (size < devIDLen) {
		return FALSE;
	}

	for (i=0;i<size-devIDLen;i++) {
		if (_tcsnicmp(infContent, INF_HOOK, devIDLen) == 0) {
			return TRUE;
		}
		infContent = _tcsinc(infContent);
	}
	return FALSE;
}

BOOL isCMIInf(TCHAR* filename)
{
	DWORD size;
	HANDLE hFile, hFileMap;
	CHAR* hMem;
	BOOL result = FALSE;

	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	size = GetFileSize(hFile, NULL);
	if (size < _tcslen(INF_HOOK)) {
		CloseHandle(hFile);
		return FALSE;
	}
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFileMap == NULL) {
		PrintLastError("CreateFileMapping()");
		CloseHandle(hFile);
		return FALSE;
	}
	hMem = (CHAR*)MapViewOfFile(hFileMap, FILE_MAP_READ, NULL, NULL, 0);
	if (hMem == NULL) {
		PrintLastError("MapViewOfFile()");
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return FALSE;
	}
	result = findInfHook(hMem, size);

	UnmapViewOfFile(hMem);
	CloseHandle(hFileMap);
	CloseHandle(hFile);
	return result;
}

int enumCMIDrivers(CHAR* infList, CHAR* infInfoList)
{
	TCHAR InfFile[MAX_PATH];
	HANDLE hFind, hInf;
	WIN32_FIND_DATA wfd;
	TCHAR infData[MAX_INF_STRING_LENGTH];
	INFCONTEXT Context;
	int result = 0;

	if (GetWindowsDirectory(InfFile, ARRAYSIZE(InfFile))==0) {
		PrintLastError("GetWindowsDirectory()");
		return 0;
	}
	*_tcscat(InfFile, "\\inf\\oem*.inf");

	hFind = FindFirstFile(InfFile, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) {
		// nothing found
		return 0;
	}

    do {
		*_tcsrchr(InfFile, _T('\\')) = _T('\0');
		*_tcscat(InfFile, _T("\\"));
		*_tcscat(InfFile, wfd.cFileName);
		if (isCMIInf(InfFile)) {
			hInf = SetupOpenInfFile(wfd.cFileName, NULL, INF_STYLE_WIN4, 0);
			if (hInf != INVALID_HANDLE_VALUE) {
				*_tcscat(infInfoList, _T("Vendor: "));
				if (SetupFindFirstLine(hInf, INFSTR_SECT_VERSION, INFSTR_KEY_PROVIDER, &Context) && (SetupGetStringField(&Context, 1, infData, ARRAYSIZE(infData), NULL))) {
					if (_tcsnicmp(infData, driverProvider, _tcslen(driverProvider)) == 0) {
						result--; // these drivers can be updated
					}
					*_tcscat(infInfoList, infData);
				} else {
					*_tcscat(infInfoList, _T("unknown"));
				}
				*_tcscat(infInfoList, _T(", Driver Version: "));
				if (SetupFindFirstLine(hInf, INFSTR_SECT_VERSION, INFSTR_DRIVERVERSION_SECTION, &Context) && (SetupGetStringField(&Context, 2, infData, ARRAYSIZE(infData), NULL))) {
					*_tcscat(infInfoList, infData);
				} else {
					*_tcscat(infInfoList, _T("unknown"));
				}
				if (SetupGetStringField(&Context, 1, infData, ARRAYSIZE(infData), NULL)) {
					*_tcscat(infInfoList, _T(" ("));
					*_tcscat(infInfoList, infData);
					*_tcscat(infInfoList, _T(")"));
				}
				SetupCloseInfFile(hInf);
			} else {
				*_tcscat(infInfoList, _T("unknown version"));
			}
			*_tcscat(infInfoList, _T("\r\n"));
			*_tcscat(infList, wfd.cFileName);
			*_tcscat(infList, _T(","));
			result++;
		}

	} while (FindNextFile(hFind, &wfd));

	FindClose(hFind);
	return result;
}

BOOL scanForHardwareChanges()
{
	DEVINST     devInst;
	CONFIGRET   status;

    if (CM_Locate_DevNode(&devInst, NULL, CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) {
		PrintLastError("CM_Locate_DevNode()");
		return FALSE;
	}
    if (CM_Reenumerate_DevNode(devInst, CM_REENUMERATE_NORMAL) != CR_SUCCESS) {
		PrintLastError("CM_Reenumerate_DevNode()");
        return FALSE;
    }

	return TRUE;
}

BOOL updateDriver(HWND hWnd)
{
	TCHAR DriverPath[MAX_PATH];
	unsigned int i;

	scanForHardwareChanges();
	if (GetModuleFileName(NULL, DriverPath, MAX_PATH) == 0) {
		PrintLastError("GetModuleFileName()");
		EnableWindow(GetDlgItem(hWnd, IDB_INSTALL), TRUE);
		return FALSE;
	}
	*_tcsrchr(DriverPath, _T('\\')) = _T('\0');
	*_tcscat(DriverPath, _T("\\CM8738.inf"));

	for (i=0;i<devArraySize;i++) {
		//TODO: delete device - scan for device updates - delete device if cmedia driver is found - repeat until no drivers are available to the OS - install this driver
		if (UpdateDriverForPlugAndPlayDevices(hWnd, devIDs[i], DriverPath, INSTALLFLAG_FORCE, NULL)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL deleteDriver(TCHAR* infName)
{
	TCHAR infPath[MAX_PATH];

	if (GetWindowsDirectory(infPath, ARRAYSIZE(infPath))==0) {
		PrintLastError("GetWindowsDirectory()");
		return 0;
	}
	*_tcscat(infPath, _T("\\inf\\"));
	*_tcscat(infPath, infName);
	DeleteFile(infPath);
	*_tcsrchr(infPath, _T('.')) = _T('\0');
	*_tcscat(infPath, _T(".pnf"));
	DeleteFile(infPath);
	*_tcsrchr(infPath, _T('.')) = _T('\0');
	*_tcscat(infPath, _T(".cat"));
	DeleteFile(infPath);

	return TRUE;
}

// load dynamically because 2k's setupapi.dll doesn't have SetupUninstallOEMInfA()
// infName without path (e.g. 'oem4.inf')
BOOL removeDriver(TCHAR* infName)
{
	HMODULE mSetupAPI;
	SetupUninstallOEMInfProto SetupUninstallOEMInf;

	if (_tcslen(infName) == 0) {
		return FALSE;
	}
	mSetupAPI = LoadLibrary(_T("setupapi.dll"));
	if (!mSetupAPI) {
		return deleteDriver(infName);
	}
	SetupUninstallOEMInf = (SetupUninstallOEMInfProto)GetProcAddress(mSetupAPI,"SetupUninstallOEMInfA");
	if (!SetupUninstallOEMInf) {
		FreeLibrary(mSetupAPI);
		return deleteDriver(infName);
	}
    if (!SetupUninstallOEMInf(infName, SUOI_FORCEDELETE, NULL)) {
		FreeLibrary(mSetupAPI);
		return deleteDriver(infName);
	}
	FreeLibrary(mSetupAPI);
	return TRUE;
}

// infNames - comma seperated list of names
void removeDrivers(TCHAR* InfList)
{
	TCHAR* infName;
	while (_tcsrchr(InfList, _T(','))) {
		*_tcsrchr(InfList, _T(',')) = _T('\0');
		infName = _tcsrchr(InfList, _T(','));
		if (infName) {
			infName = _tcsinc(infName);
		} else {
			infName = InfList;
		}
		removeDriver(infName);
	}
}

BOOL isUserAdmin()
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;

	b = AllocateAndInitializeSid(
	    &NtAuthority, 2,
	    SECURITY_BUILTIN_DOMAIN_RID,
	    DOMAIN_ALIAS_RID_ADMINS,
	    0, 0, 0, 0, 0, 0,
	    &AdministratorsGroup);

	if (b) {
		if (!CheckTokenMembership(NULL, AdministratorsGroup, &b)) {
			b = FALSE;
		}
		FreeSid(AdministratorsGroup);
	}

	return b;
}

BOOL installDriver(HWND hWnd)
{
	TCHAR InfInfo[1023], InfList[511];
	BOOL result;

	EnableWindow(GetDlgItem(hWnd, IDB_INSTALL), FALSE);


	*_tcscpy(InfInfo, _T("The following drivers for C-Media cards have been found in the driver repository of Windows:\r\n\r\n"));
	*_tcscpy(InfList, _T(""));
	if (enumCMIDrivers(InfList, InfInfo) > 0) {
		*_tcscat(InfInfo, _T("\r\nThese drivers will be wiped from the system so as to avoid driver conflicts. Do you want to continue?"));
		if (MessageBox(hWnd, InfInfo, "Information", MB_YESNO | MB_ICONINFORMATION) == IDNO) {
			EnableWindow(GetDlgItem(hWnd, IDB_INSTALL), TRUE);
			return FALSE;
		}
		removeDrivers(InfList);
	}
	closeCMIApps();
	openCMIWave();
	result = updateDriver(hWnd);
	if (!result) {
		PrintLastError("installDriver()");
	}
	if (hWave) {
		waveOutClose(hWave);
		hWave = NULL;
	}
	EnableWindow(GetDlgItem(hWnd, IDB_INSTALL), TRUE);
	return result;
}

BOOL isVista()
{
	OSVERSIONINFO osV;
	ZeroMemory(&osV, sizeof(OSVERSIONINFO));
	osV.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osV);
	return (osV.dwMajorVersion == 6) && (osV.dwMinorVersion == 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {
		case WM_CLOSE:
			DestroyWindow(hWnd);
			return TRUE;
		case WM_DESTROY:
			PostQuitMessage(0);
			return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDB_CLOSE) {
				PostQuitMessage(0);
				return TRUE;
			}
			if (LOWORD(wParam) == IDB_INSTALL) {
				if (installDriver(hWnd)) {
					if (doCMIKeysExist()) {
						if (MessageBox(hWnd, "The driver has been successfully installed! Do you want to remove the remains of the official C-Media driver?", "Driver Installer", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
							deleteCMIKeys();
						}
					} else {
						MessageBox(hWnd, "The driver has been successfully installed!", "Driver Installer", MB_ICONINFORMATION);
					}
					writeUninstallerKeys();
					if (isVista()) {
						WinExec("control mmsys.cpl", SW_NORMAL);
					}
					PostQuitMessage(0);
				}

				return TRUE;
			}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	WNDCLASSEX wce;
	HWND       hWnd;
	MSG        msg;

	if (hWnd = FindWindow("cmiDriverInstaller", NULL)) {
		SetForegroundWindow(hWnd);
		return FALSE;
	}

	if (!isUserAdmin()) {
		MessageBox(NULL, (LPCSTR)"This application requires administrative rights for installing the driver. Please run this application as administrator (Vista: right-click, \"run as administrator\"; XP: login with the administrator account)!", "Error", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	hWave = NULL;
	hInst = hInstance;
	ZeroMemory(&wce, sizeof(WNDCLASSEX));
	wce.cbSize        = sizeof(WNDCLASSEX);
	wce.lpfnWndProc   = DefDlgProc;
	wce.style         = 0;
	wce.cbWndExtra    = DLGWINDOWEXTRA;
	wce.hInstance     = hInstance;
	wce.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wce.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wce.lpszClassName = "cmiDriverInstaller";
	wce.lpszMenuName  = NULL;
	wce.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wce.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	if(!RegisterClassEx(&wce)) {
		PrintLastError("RegisterClassEx()");
		return -1;
	}

	hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)WndProc, NULL);
	if (!hWnd) {
		PrintLastError("CreateDialogParam()");
		return -1;
	}

	while (GetMessage(&msg, (HWND) NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
