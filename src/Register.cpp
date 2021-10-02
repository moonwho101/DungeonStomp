#define DIRECTINPUT_VERSION 0x0700

#include "prag.h"
#include "resource.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <dinput.h>
#include <dplobby.h>
#include <dplay.h>
#include "D3DApp.h"
#include <dsound.h>
#include "D3DUtil.h"
#include "world.hpp"
#include "3Dengine.hpp"
#include "DungeonStomp.hpp"
#include "D3DTextr.h"
#include "ImportMD2.hpp"
#include "Import3DS.hpp"
#include "LoadWorld.hpp"
#include "DpMessages.hpp"

#include <dsound.h> // include dsound, dmusic
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>

#include "Common.h"
#include "Coll.h"

extern float closesoundid[100];
extern float FastDistance(float fx, float fy, float fz);
extern CMyD3DApplication* pCMyApp;
extern D3DTLVERTEX m_DisplayMessage[10000];
extern int m_DisplayMessageFont[10000];
extern int g_ob_vert_count;
extern int countmessage;
extern int countboundingbox;
extern D3DVECTOR gvelocity;
extern D3DVERTEX boundingbox[2000];
extern CLoadWorld* pCWorld;

extern HWND main_window_handle;

extern int perspectiveview;
extern int doorcounter;
BOOL CALLBACK DlgListServers(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void RegCrypt(char* user, char* key, char* result);

BOOL CALLBACK DlgRegister(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int result = 0;
	static HWND hWndCtl;
	char buffer[60];

	// This handles the messages from the Create/Connect dialog box
	switch (msg)
	{

	case WM_INITDIALOG:

		result = SetDlgItemText(hDlg, IDC_EDIT1, pCMyApp->ds_reg->name);
		result = SetDlgItemText(hDlg, IDC_EDIT2, pCMyApp->ds_reg->key);

		if (strcmp(pCMyApp->ds_reg->registered, "1") == 0)
		{
			result = SetDlgItemText(hDlg, IDC_EDIT3, "REGISTERED");
		}
		else
		{
			result = SetDlgItemText(hDlg, IDC_EDIT3, "PLEASE REGISTER");
		}
		pCMyApp->IsShareware();

		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_OK:

			result = GetDlgItemText(hDlg, IDC_EDIT1, &buffer[0], 50);
			strcpy_s(pCMyApp->ds_reg->name, buffer);

			result = GetDlgItemText(hDlg, IDC_EDIT2, &buffer[0], 50);
			strcpy_s(pCMyApp->ds_reg->key, buffer);
			pCMyApp->SaveDSReg();
			EndDialog(hDlg, -1);
			return (TRUE);
			break;
		case IDC_CANCEL:

			EndDialog(hDlg, -1);
			return (TRUE);
			break;
		}
		break;
	}
	return (FALSE);
}

BOOL CALLBACK DlgPlayOnline(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int result = 0;
	static HWND hWndCtl;

	switch (msg)
	{
	case WM_INITDIALOG:

		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_BUTTON1:
			EndDialog(hDlg, -1);
			pCMyApp->StartMPGame();

			break;

		case IDC_BUTTON5:
			EndDialog(hDlg, -1);
			DialogBox(NULL, (LPCTSTR)IDD_SERVERLIST, main_window_handle,
				(DLGPROC)DlgListServers);
			pCMyApp->forcemainserver = 1;
			pCMyApp->StartMPGame();
			break;

		case IDC_OK:

			EndDialog(hDlg, -1);
			return (TRUE);
			break;
		case IDC_CANCEL:

			EndDialog(hDlg, -1);
			return (TRUE);
			break;
		}
		break;
	}
	return (FALSE);
}

BOOL CALLBACK DlgListServers(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int result = 0;
	static HWND hWndCtl;

	HWND hwndObjects;

	int i = 0;
	int iIndex;

	// This handles the messages from the Create/Connect dialog box
	switch (msg)
	{
	case WM_INITDIALOG:
		hwndObjects = GetDlgItem(hDlg, IDC_LISTSERVERS);
		SendMessage(hwndObjects, LB_RESETCONTENT, 0, 0);

		pCMyApp->GetServer();
		for (i = 0; i < pCMyApp->dsservercount; i++)
		{
			SendMessage(hwndObjects, LB_ADDSTRING, 0, (LPARAM)&pCMyApp->ds_serverinfo[i].name);
		}

		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_OK:
			hwndObjects = GetDlgItem(hDlg, IDC_LISTSERVERS);

			iIndex = SendMessage(hwndObjects, LB_GETCURSEL, 0, 0);

			strcpy_s(pCMyApp->serveripaddress, pCMyApp->ds_serverinfo[iIndex].ipaddress);
			pCMyApp->forcemainserver = 1;
			EndDialog(hDlg, -1);
			return (TRUE);
			break;
		case IDC_CANCEL:
			EndDialog(hDlg, -1);
			return (TRUE);
			break;
		}
		break;
	}
	return (FALSE);
}

bool CMyD3DApplication::WriteDSReg()
{

	char junk[25];
	HKEY hkey;
	DWORD dwDisposition;
	long hresult;

	strcpy_s(junk, "test");

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Calamity\\DungeonStomp"), 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
	{
		RegCloseKey(hkey);
		ReadDSReg();
	}
	else
	{
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Calamity\\DungeonStomp"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
		{
			strcpy_s(ds_reg->name, "DEMO");
			hresult = RegSetValueEx(hkey, "Name", 0, REG_SZ, (PBYTE)LPCTSTR(ds_reg->name), strlen(ds_reg->name));

			strcpy_s(ds_reg->key, "DEMOKEY");
			hresult = RegSetValueEx(hkey, "Key", 0, REG_SZ, (PBYTE)LPCTSTR(ds_reg->key), strlen(ds_reg->key));

			RegCloseKey(hkey);
		}
	}
	return 1;
}
bool CMyD3DApplication::SaveDSReg()
{

	char junk[25];
	HKEY hkey;
	DWORD dwDisposition;
	long hresult;
	// DWORD dwType, dwSize;
	strcpy_s(junk, "test");

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Calamity\\DungeonStomp"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
	{
		hresult = RegSetValueEx(hkey, "Name", 0, REG_SZ, (PBYTE)LPCTSTR(ds_reg->name), strlen(ds_reg->name));
		hresult = RegSetValueEx(hkey, "Key", 0, REG_SZ, (PBYTE)LPCTSTR(ds_reg->key), strlen(ds_reg->key));

		RegCloseKey(hkey);
		IsShareware();
	}

	return 1;
}

bool CMyD3DApplication::IsShareware()
{

	char* p;
	char keyresult[100];
	// This handles the messages from the Create/Connect dialog box

	p = &keyresult[0];
	RegCrypt(pCMyApp->ds_reg->name, "DungeonStomp", p);

	if (strstr(pCMyApp->ds_reg->name, "www.ttdown.com") != NULL)
	{
		strcpy_s(ds_reg->registered, "0");
		return FALSE;
	}

	if (strcmp(ds_reg->key, p) == 0)
	{

		strcpy_s(ds_reg->registered, "1");
		return TRUE;
	}
	else
	{
		strcpy_s(ds_reg->registered, "0");
	}

	return FALSE;
}

bool CMyD3DApplication::ReadDSReg()
{

	char junk[4255];
	HKEY hkey;

	DWORD dwType;
	DWORD dwSize;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Calamity\\DungeonStomp"), 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
	{

		dwSize = sizeof(junk);
		dwType = REG_SZ;
		RegQueryValueEx(hkey, "Name", NULL, &dwType, (BYTE*)&junk, &dwSize);

		strcpy_s(ds_reg->name, junk);

		dwSize = sizeof(junk);
		dwType = REG_SZ;
		RegQueryValueEx(hkey, "key", NULL, &dwType, (BYTE*)&junk, &dwSize);

		strcpy_s(ds_reg->key, junk);

		IsShareware();
	}

	return 1;
}

void RegCrypt(char* user, char* key, char* result)
{
	char encstring[1024], string1[1024], string2[1024];
	char encstring2[1024];
	int encstringlen;
	char resultstring[17];
	long int i, c, d;

	strcpy_s(encstring2, "QRSWXYZabcdef023NOPghjkmnopq456789ATUVBCDEFGHJKMrstuvwxyz");

	encstringlen = strlen(encstring);

	strcpy_s(resultstring, "               ");
	d = 0;
	strcpy_s(string1, user);
	strcpy_s(string2, key);
	for (i = 0; i < (int)strlen(string1); i++)
		d = d + string1[i];
	for (i = 0; i < (int)strlen(string2); i++)
		d = d + string2[i];
	for (i = 0; i < 16; i++)
	{
		c = encstring[i] + d;
		if (i < (int)strlen(string1))
			c = c + string1[i];
		if (i < (int)strlen(string2))
			c = c + string2[i];
		resultstring[i] = encstring[(c % encstringlen)];
		d = c;
	}
	resultstring[16] = 0;
	strcpy_s(result, 100, resultstring);
}
