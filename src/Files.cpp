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
#include "D3DFile.h"
#include <dsound.h> // include dsound, dmusic
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>

#include "Common.h"
#include "Coll.h"
extern HWND main_window_handle;
extern CMyD3DApplication *pCMyApp;
extern CLoadWorld *pCWorld;
//-----------------------------------------------------------------------------
// Name: LoadFile()
// Desc: Uses Windows' OpenFileName dialog to get the name of an X file to
//       load, then proceeds to load that file.
//-----------------------------------------------------------------------------
void CMyD3DApplication::LoadFile() {
	//	CD3DFile* m_pFileObject;
	static TCHAR strInitialDir[512] = "";
	static TCHAR strFileName[512];
	TCHAR strCurrentName[512] = "*.map";
	int filenamelen = 0;
	int a = 0;
	char cname[255];
	char junk[255];
	int truelevel = 0;

	strcpy_s(strInitialDir, "..\\levels");
	OPENFILENAME ofn = { sizeof(OPENFILENAME), main_window_handle, NULL,
		                 "Map Files (*.map)\0*.map\0\0",
		                 NULL, 0, 1, strCurrentName, 512, strFileName, 512,
		                 strInitialDir, "Open Map File", OFN_FILEMUSTEXIST, 0, 1,
		                 ".MAP", 0, NULL, NULL };

	// Run the OpenFileName dialog.
	if (FALSE == GetOpenFileName(&ofn))
		return;

	// Store the initial directory for next time
	strcpy_s(strInitialDir, strCurrentName);
	strstr(strInitialDir, strFileName)[0] = '\0';
	strcpy_s(current_levelname, strCurrentName);

	filenamelen = strlen(strFileName);
	strcpy_s(levelname, strFileName);
	levelname[filenamelen - 4] = '\0';
	junk[0] = (levelname[strlen(levelname) - 1]);
	junk[1] = '\0';
	truelevel = atoi(junk);
	current_level = truelevel;
	pCMyApp->num_players2 = 0;
	pCMyApp->itemlistcount = 0;
	pCMyApp->num_monsters = 0;

	pCMyApp->ClearObjectList();
	pCMyApp->ResetSound();

	pCWorld->LoadSoundFiles(m_hWnd, "sounds.dat");

	if (!pCWorld->LoadWorldMap(m_hWnd, pCMyApp->current_levelname)) {
		// PrintMessage(m_hWnd, "LoadWorldMap failed", NULL, LOGFILE_ONLY);
		// return FALSE;
	}
	pCMyApp->ResetPlayer();
	strcpy_s(cname, pCMyApp->current_levelname);
	a = strlen(cname);

	cname[a - 3] = 'c';
	cname[a - 2] = 'm';
	cname[a - 1] = 'p';

	if (!pCWorld->InitPreCompiledWorldMap(m_hWnd, cname)) {
		// PrintMessage(m_hWnd, "InitPreCompiledWorldMap failed", NULL, LOGFILE_ONLY);
		// return FALSE;
	}

	cname[a - 3] = 'm';
	cname[a - 2] = 'o';
	cname[a - 1] = 'd';

	pCWorld->LoadMod(m_hWnd, cname);
	sprintf_s(pCMyApp->gActionMessage, "Loading %s", pCMyApp->current_levelname);
	pCMyApp->UpdateScrollList(0, 245, 255);

	openingscreen = 0;
	return;
}

void CMyD3DApplication::LoadFileSpot() {

	static TCHAR strInitialDir2[512] = "";
	static TCHAR strFileName2[512];
	TCHAR strCurrentName2[512] = "*.spt";
	int filenamelen = 0;
	int a = 0;

	strcpy_s(strInitialDir2, "..\\savegame");
	OPENFILENAME ofn = { sizeof(OPENFILENAME), main_window_handle, NULL,
		                 "Spot Files (*.spt)\0*.spt\0\0",
		                 NULL, 0, 1, strCurrentName2, 512, strFileName2, 512,
		                 strInitialDir2, "Open Spot File", OFN_FILEMUSTEXIST, 0, 1,
		                 ".MAP", 0, NULL, NULL };

	// Run the OpenFileName dialog.
	if (FALSE == GetOpenFileName(&ofn))
		return;

	// Store the initial directory for next time
	strcpy_s(strInitialDir2, strCurrentName2);
	strstr(strInitialDir2, strFileName2)[0] = '\0';

	filenamelen = strlen(strFileName2);

	load_game(strFileName2);

	sprintf_s(gActionMessage, "Loading %s", strFileName2);
	UpdateScrollList(0, 245, 255);

	// Return successful
	openingscreen = 0;
	return;
}

void CMyD3DApplication::SaveFileSpot() {

	static TCHAR strInitialDir2[512] = "";
	static TCHAR strFileName2[512];
	TCHAR strCurrentName2[512] = "spot.spt";
	int filenamelen = 0;

	strcpy_s(strInitialDir2, "..\\savegame");
	OPENFILENAME ofn = { sizeof(OPENFILENAME), main_window_handle, NULL,
		                 "Spot Files (*.spt)\0*.spt\0\0",
		                 NULL, 0, 1, strCurrentName2, 512, strFileName2, 512,
		                 strInitialDir2, "Open Spot File", OFN_FILEMUSTEXIST, 0, 1,
		                 ".MAP", 0, NULL, NULL };

	// Run the OpenFileName dialog.
	if (FALSE == GetSaveFileName(&ofn))
		return;

	// Store the initial directory for next time
	strcpy_s(strInitialDir2, strCurrentName2);
	strstr(strInitialDir2, strFileName2)[0] = '\0';

	filenamelen = strlen(strFileName2);

	save_game(strFileName2);

	sprintf_s(gActionMessage, "Saving %s", strFileName2);
	UpdateScrollList(0, 245, 255);

	return;
}
