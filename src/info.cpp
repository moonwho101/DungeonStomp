#define DIRECTINPUT_VERSION 0x0700
//-----------------------------------------------------------------------------
// File: info.cpp
//
// Desc: Code for handling output of program information to the user
//
// Copyright (c) 2001, Mark Longo, All rights reserved
//-----------------------------------------------------------------------------
#include "prag.h"
#include "resource.h"
#include <ddraw.h>
#include "d3dframe.h"
#include "world.hpp"
#include "D3DApp.h"
#include "DungeonStomp.hpp"
#include "d3dtextr.h"

extern CMyD3DApplication *pCMyApp;
extern D3DAppTextureFormat ThisTextureFormat;
extern char D3Ddevicename[256];

void CMyD3DApplication::DisplayCredits(HWND hwnd) {
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	char buffer[255];
	HBRUSH holdbrush_color;

	hdc = GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex));
	Rectangle(hdc, 10, 58, 512, 430);

	SetBkMode(hdc, TRANSPARENT);

	strcpy_s(buffer, "Credits for Dungeon Stomp:");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy += 40;

	strcpy_s(buffer, "Mark Longo");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "C++ Programmer");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "Aptisense");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "http://www.aptisense.com");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "Email us at:");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "mlongo@aptisense.com");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd, hdc);
}

void CMyD3DApplication::DisplayControls(HWND hwnd) {
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	char buffer[255];
	HBRUSH holdbrush_color;

	hdc = GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex));
	Rectangle(hdc, 10, 58, 512, 430);

	SetBkMode(hdc, TRANSPARENT);

	strcpy_s(buffer, "Controls for Dungeon Stomp:");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "W");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Forward");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "S");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Backward");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "A");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Rotate left");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "D");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Rotate right");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "Spacebar");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Open Doors/Treasure");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "E");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Jump");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "Right Ctrl or Left Mouse");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Attack/Use Spell");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "/");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Talk /Console");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "Insert / Delete or Q / Z");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Change weapon");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "Page up / down");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Head up / down");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "L");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Listen");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "P");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Toggle Panel");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "V");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Change View");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, ",.");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Strife");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "0-9");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Weapon Slots");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "RTYU");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy_s(buffer, "Spell Slots");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd, hdc);
}

void CMyD3DApplication::DisplayRRStats(HWND hwnd) {
	HDC hdc;
	int mem;
	int sx1 = 15, sx2 = 320, sy = 70;
	int nIndex;
	char buffer[255];
	char buf2[255];
	LPTSTR buffer2;
	LPDIRECTDRAW7 lpDD7;
	DDSCAPS2 ddsCaps2;
	DWORD total_memory_for_textures;
	DWORD free_memory_for_textures;
	DWORD total_video_memory;
	DWORD free_video_memory;
	HRESULT hr;
	MEMORYSTATUS memStatus;
	HBRUSH holdbrush_color;

	hdc = GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;

	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex));
	Rectangle(hdc, 10, 58, 512, 430);

	SetBkMode(hdc, TRANSPARENT);
	memStatus.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memStatus);

	mem = (int)memStatus.dwMemoryLoad;
	_itoa_s(mem, buffer, _countof(buffer), 10);
	buffer2 = lstrcat(buffer, " %");
	strcpy_s(buf2, "System memory in use :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer2, strlen(buffer2));
	sy += 20;

	mem = (int)memStatus.dwTotalPhys;
	mem = mem / 1024;
	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcat_s(buffer, " KB");
	strcpy_s(buf2, "Total system memory :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	hr = GetFramework()->GetDirectDraw()->QueryInterface(IID_IDirectDraw7, (void **)&lpDD7);
	if (FAILED(hr))
		return;

	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_TEXTURE;
	hr = GetFramework()->GetDirectDraw()->GetAvailableVidMem(&ddsCaps2, &total_memory_for_textures, &free_memory_for_textures);
	if (FAILED(hr))
		return;

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY;
	hr = GetFramework()->GetDirectDraw()->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory);
	if (FAILED(hr))
		return;

	mem = total_video_memory;
	mem = mem / 1024;
	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcat_s(buffer, " KB");
	strcpy_s(buf2, "Total video memory :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	mem = free_video_memory;
	mem = mem / 1024;
	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcat_s(buffer, " KB");
	strcpy_s(buf2, "Free video memory :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	mem = free_memory_for_textures;
	mem = mem / 1024;
	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcat_s(buffer, " KB");
	strcpy_s(buf2, "Free video & system memory for textures :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	mem = total_memory_for_textures;
	mem = mem / 1024;
	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcat_s(buffer, " KB");
	strcpy_s(buf2, "Total video & system memory for textures :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	TextOut(hdc, sx1, sy, "D3D Device Name :", 17);
	TextOut(hdc, sx2, sy, D3Ddevicename, strlen(D3Ddevicename));
	sy += 20;

	TextOut(hdc, sx1, sy, "Texture Mode    :", 17);
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 40;

	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcpy_s(buf2, "Number of triangles in scene :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcpy_s(buf2, "Number of verts in scene  :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	_itoa_s(mem, buffer, _countof(buffer), 10);
	strcpy_s(buf2, "Number of DP commands in scene  :");
	TextOut(hdc, sx1, sy, buf2, strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy += 20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd, hdc);
}

void CMyD3DApplication::DisplayLegalInfo(HWND hwnd) {
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	char buffer[255];
	HBRUSH holdbrush_color;

	hdc = GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex));
	Rectangle(hdc, 10, 58, 512, 430);

	SetBkMode(hdc, TRANSPARENT);

	strcpy_s(buffer, "Legal info for Dungeon Stomp:");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy += 40;

	strcpy_s(buffer, "Programmed by Mark Longo ");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy += 20;

	strcpy_s(buffer, "Copyright (C) 2001-2021, Aptisense");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy += 20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd, hdc);
}

BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG:
		SetTimer(hwnd, 1, 100, NULL);
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_TIMER:
		KillTimer(hwnd, 1);
		pCMyApp->DisplayCredits(hwnd);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			EndDialog(hwnd, TRUE);
			break;

		case IDC_BUTTON1:
			pCMyApp->DisplayCredits(hwnd);
			break;

		case IDC_BUTTON2:
			pCMyApp->DisplayRRStats(hwnd);
			break;

		case IDC_BUTTON3:
			pCMyApp->DisplayControls(hwnd);
			break;

		case IDC_BUTTON4:
			pCMyApp->DisplayLegalInfo(hwnd);
			break;
			break;
		}
		break;
	}

	return FALSE;
}
