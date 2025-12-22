//-----------------------------------------------------------------------------
// File: DungeonStomp.cpp
// Copyright (c) 2001, Mark Longo, All rights reserved
//-----------------------------------------------------------------------------

#define DIRECTINPUT_VERSION 0x0700
#define STRICT
#define D3D_OVERLOADS

#include "prag.h"
#include "resource.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <dinput.h>
#include <dplobby.h>
#include <dplay.h>
#include "D3DApp.h"
#include <dsound.h>
#include "D3DUtil.h"
#include "D3DEnum.h"
#include "world.hpp"
#include "3Dengine.hpp"
#include "DungeonStomp.hpp"
#include "D3DTextr.h"
#include "ImportMD2.hpp"
#include "Import3DS.hpp"
#include "LoadWorld.hpp"
#include "DpMessages.hpp"
#include "dplay2.hpp"
#include <tchar.h>
#include "math3d.h"
#include "vectormath.h"
#include <winsock.h>
#include <string.h>
#include <dsound.h> // include dsound, dmusic
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include "col_local.h"
#include "icetypes.h"
#include "IceRevisitedRadix.h"
#include "t3dlib3.h"

#define SND_FIRE_AR15 0
#define SND_FIRE_MP5 1
#define DELETEARRAY(x)  \
	{                   \
		if (x != null)  \
			delete[] x; \
		x = null;       \
	}
#define SAFE_DELETE_ARRAY(p) \
	{                        \
		if (p) {             \
			delete[] (p);    \
			(p) = NULL;      \
		}                    \
	}
#define MAX_SOUND_BUFFERS 30
#define MAX_PLAYER_NAME 14
#define MAX_SESSION_NAME 256
#define MAX_NUM_OBJECTS_PER_CELL 650
#define DPLAY_SAMPLE_KEY TEXT("Software\\Microsoft\\DirectX DirectPlay Samples")
#define EPSILON 0.05f //  0.005f - so the monsters don't fall through the floor we set this larger 0.05f

// Missle
D3DVECTOR MissleSave;
D3DVECTOR calculatemisslelength(D3DVECTOR velocity);

// Door
int listendoor = 0;
int listenfailed = 0.0f;
int doorcounter = 0;

struct doors {
	int doornum;
	float angle;
	int swing;
	int key;
	int open;
	float saveangle;
	int type;
	int listen;
	int y;
	float up;

} DOORS, door[200];

// Camera
float rotate_camera = 0;
float cameraheight = 50.0f;

// Collision
CollisionPacket colPack2;
CollisionPacket collisionPackage;
int collisionRecursionDepth = 0;
int lastcollide = 0;
int collisioncode = 1;
int objectcollide = 0;
int foundcollisiontrue = 0;
int currentmonstercollisionid = -1;
float collisiondist = 4190.0f;
BOOL foundcollision = 0;

extern void loadcollisionmap(D3DVECTOR eyeball, D3DVECTOR v, D3DVECTOR spheresize);
extern void checkTriangle(CollisionPacket *colPackage, VECTOR p1, VECTOR p2, VECTOR p3);

struct TCollisionPacket {
	// data about player movement
	D3DVECTOR velocity;
	D3DVECTOR sourcePoint;
	// radius of ellipsoid.
	D3DVECTOR eRadius;
	// data for collision response
	BOOL foundCollision;
	double nearestDistance;                    // nearest distance to hit
	D3DVECTOR nearestIntersectionPoint;        // on sphere
	D3DVECTOR nearestPolygonIntersectionPoint; // on polygon
};

// Move
int direction = 0;
int directionlast = 0;
int savelastmove = 0;
int savelaststrifemove = 0;
float playerspeedmax = 340.0f;
float playerspeedlevel = 340.0f;
float movespeed = 0.0f;
float movespeedold = 0.0f;
float movespeedsave = 0.0f;
float movetime = 0.0f;
float moveaccel = 600.0f;
int playermove = 0;
int playermovestrife = 0;

// Merchant
char merchantdisplay[255];
int merchantcurrent = 0;
int merchantfound = 0;

// Mouse
int mousefilter = 1;

// Directx
RadixSort RS;
udword *Sorted;
D3DVERTEX temp_v[MAX_NUM_VERTICES]; // debug
int zbufferenable = 1;
int filtertype = 1;
int src_dist[MAX_NUM_VERTICES];
int drawthistri = 1;
int lockframerate = 0;
int polysorton = 1;
int tempvcounter = 0;
int mdmg = 0;
int numberofsquares = 11;
int ap_cnt = 0;
float fDot2last = 0;
float fDot2 = 0;
float viewportheight;
float viewportwidth;
float totaldist = 0.0f;
float *InputValues;
float *InputValues2;
char fpsbuffer[256];

// Jump
int jumpstart = 0;
int nojumpallow = 0;
int jumpvdir = 0;
float cleanjumpspeed = 0;
float lastjumptime = 0;

// Saving Throw
int showsavingthrow = 0;
int showlisten = 0;
int savefailed = 0.0f;

// Items
int objectscroll = 0;
int g_ob_vert_count = 0;
float objectheight = 0;
float objecttrueheight = 0;
float objectx = 0;
float objectz = 0;
float objecty = 0;

// Sounds
extern int totalsounds;
extern struct msoundlist slist[500];
extern int slistcount;

struct msoundlist {
	int id;
	int yell;
	int attack;
	int die;
	int weapon;

	int hd;
	int ac;
	int hp;
	int thaco;
	float speed;
	char name[80];
	char damage[80];
	int playing;
};

// Run
extern int runflag;

// Lights
int ambientlighton = 0;
int lighttype = 0;

// Material
int lastmaterial = 0;

// Gun
D3DVECTOR GunTrue;
D3DVECTOR GunTruesave;
int weapondrop = 0;
int lastgun = 0;
int animategunslow = 0;
int animationdirgun = 0;
int firepause = 0;
int num_op_guns = 1;
int num_cars;
float weapon_flash_r;
float weapon_flash_g;
bool cycleweaponbuttonpressed = FALSE;
FLOAT fLastGunFireTime;

// Sound Music
int musicon = 1;
float closesoundid[100];

// DialogBox
D3DVERTEX bubble[600];
D3DTLVERTEX m_DisplayMessage[10000];

int dialogpause = 0;
int dialognum = 0;
int dialogtextcount = 0;
int scrolllistnum = 6;
int slistcounter = 0;
int sliststart = 0;
int textcounter = 0;
int m_DisplayMessageFont[10000];
int countdisplay = 0;
int countmessage = 0;
char gfinaltext[2048];
char screentext[2048];

struct gametext {
	int textnum;
	int type;
	char text[2048];
	int shown;
} GAMETEXT, gtext[200];

struct gametext dialogtext[200];

struct scrolllisting {

	int num;
	float angle;
	char text[2048];
	int r;
	int g;
	int b;

} SCROLLLISTING, scrolllist1[50];

// BoundingBox
int excludebox = 0;
D3DVERTEX boundingbox[2000];
int countboundingbox = 0;

// Timer
LONGLONG fTimeKeyscroll = 0;
int showscreentimer = 0;
FLOAT fTimeKeysave = 0;
LONGLONG ScreenTime = 0;
LONGLONG ScreenTimeLast = 0;
LONGLONG gametimer = 0;
LONGLONG gametimerlast = 0;
int maingameloop = 0;

LONGLONG elapsegametimer = 0;
LONGLONG elapsegametimerlast = 0;
FLOAT elapsegametimersave = 0;

LONGLONG gametimerpost = 0;
LONGLONG gametimerlastpost = 0;
int maingamelooppost = 0;

LONGLONG gametimer2 = 0;
LONGLONG gametimerlast2 = 0;
int maingameloop2 = 0;

LONGLONG gametimerdoor = 0;
LONGLONG gametimerlastdoor = 0;
int maingameloopdoor = 0;

LONGLONG gametimerplayer = 0;
LONGLONG gametimerlastplayer = 0;
int maingameloopplayer = 0;

// Monster
int monsterenable = 1;
int monstercount = 0;
int monstermoveon = 1;
int showmonstermenu = 1;
int monstercull[1000];
int monstertype[1000];
int monsterangle[1000];
int monsterobject[1000];
float monsterheight[1000];
float monstertrueheight[1000];
float monsterx[1000];
float monsterz[1000];
float monsterdist = 150.0f;
extern HRESULT ProcessDirectPlayMessages(HWND hDlg);

// Gravity
D3DVECTOR gravityvector;
D3DVECTOR gravityvectorold;
D3DVECTOR gravityvectorconst;
D3DVECTOR gvel;
D3DVECTOR gvelocity;
float gravitytime = 0;

// View Move Camera
D3DMATRIX matView;
D3DVECTOR EyeTrue;
D3DVECTOR LookTrue;
D3DVERTEX showview[10];
D3DVECTOR savevelocity;
D3DVECTOR saveoldvelocity;

D3DTLVERTEX crosshair[10];
BOOL bSkipThisCell;
bool changeviewbuttonpressed = FALSE;
int perspectiveview = 1;
float maxgvel = 30.0f;
float calcy = 0;
float saveangy = 0;
float planex = 0, planey = 0, planez = 0;
float planex2 = 0, planey2 = 0, planez2 = 0;
float planex3 = 0, planey3 = 0, planez3 = 0;
float culldist = 1450.0f;
float rotateprop;
float normalscale = 4;
double nearestDistance = -99;
char itoabuffer[256];

extern D3DVECTOR finalv;

struct camera_float {
	float x;
	float y;
	float z;
	float dir;
} CAMERAFLOAT, cameraf;

// Flare

int flarestart = 0;
int flaretype = 0;
int flarenum = 0;
int flarecounter = 0;
float flarefadeout = 0;
FLOAT FlareTime = 0;
FLOAT FlareTimeLast = 0;

// Player
D3DVECTOR saveplocation;
D3DVECTOR modellocation;
int currentscreen = 0;
int startposcounter = 0;
int countplayers;
float playerspeed = 180.0f;
float currentspeed = 0;
float model_y = 30;
float model_x = 0;
float model_z = 0;
float lastmodely = 0;

struct startposition {
	float x;
	float y;
	float z;
	float angle;

} STARTPOSTION, startpos[200];

// Debug Mode
int betamode;
int showpanel = 1;
int menuflares = 1;
int showdebug = 0;
int showdebugfps = 0;
int numnormals;
int normalon = 1;

extern HMENU gmenuhandle;
extern HMENU gmenuhandle2;
D3DLVERTEX normal_line[20];

extern BOOL CALLBACK DlgRegister(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK DlgListServers(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK DlgPlayOnline(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
char debug_buffer[1024][3];

// Dice
int numdice = 2;

struct diceroll {

	char name[40];
	char monster[50];
	char prefix[40];
	int roll;
	int rollnum;
	int sides;
	int rollmax;

	D3DTLVERTEX dicebox[4];
} DICEROLL, dice[50];

// Texture
D3DTLVERTEX m_BackgroundMesh[2000];
int showtexture = 1;
int vpolysrc[MAX_NUM_VERTICES];

float tuad = 0.625f;
float tvad = 0.625f;

struct poly_sort {
	float dist;
	int vert_index;
	int srcstart;
	int srcfstart;
	int vertsperpoly;
	int facesperpoly;
	int texture;

} POLY_SORT, psort[MAX_NUM_VERTICES];

void SortPoly(int num_poly);

// DirectPlay
BYTE diks[256];

LPDIRECTINPUT7 g_pDI;                        // The DInput object
LPDIRECTINPUT7 g_Keyboard_pDI;               // The DInput object
LPDIRECTINPUTDEVICE2 g_pdidDevice2;          // The DIDevice2 interface
LPDIRECTINPUTDEVICE2 g_Keyboard_pdidDevice2; // The DIDevice2 interface
GUID g_guidJoystick;                         // The GUID for the joystick

BOOL g_bUseKeyboard = TRUE;
BOOL g_bUseMouse = FALSE;
BOOL g_bUseJoystick = FALSE;
BOOL bMouseLookOn = TRUE;
BOOL bMouseLookup_is_mouse_forward = TRUE;
BOOL bIsFlashlightOn = FALSE;

int vol_x[MAX_NUM_QUADS];
int vol_z[MAX_NUM_QUADS];
int alpha_poly_index[MAX_NUM_QUADS];
int alpha_vert_index[MAX_NUM_QUADS];
int alpha_face_index[MAX_NUM_QUADS];
int alpha_face_src[MAX_NUM_QUADS];

#define EXITCODE_FORWARD 1 // Dialog success, so go forward
#define EXITCODE_BACKUP 2  // Dialog canceled, show previous dialog
#define EXITCODE_QUIT 3    // Dialog quit, close app
#define EXITCODE_ERROR 4   // Dialog error, terminate app

//-----------------------------------------------------------------------------

#define GAMEMSG_WAVE 1

extern BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM);
extern HRESULT DPConnect_CheckForLobbyLaunch(BOOL *pbLaunchedByLobby);
extern int DPConnect_StartDirectPlayConnect(HINSTANCE hInst, BOOL bBackTrack);
extern HANDLE g_hDPMessageEvent;
extern LPDIRECTSOUNDBUFFER g_pDSBuffer[MAX_SOUND_BUFFERS];

HWND main_window_handle;
D3DVERTEX *g_pObstacleVertices = NULL;
WORD *g_pObstacleIndices = NULL;
DWORD g_dwNumObstacleVertices;
DWORD g_dwNumObstacleIndices;

D3DVERTEX *g_pObstacleVertices2 = NULL;
WORD *g_pObstacleIndices2 = NULL;
DWORD g_dwNumObstacleVertices2;
DWORD g_dwNumObstacleIndices2;
int firemissle = 0;
D3DVECTOR realEye;

D3DVECTOR eRadius;
HRESULT CreateSphere(D3DVERTEX **ppVertices, DWORD *pdwNumVertices,
                     WORD **ppIndices, DWORD *pdwNumIndices,
                     FLOAT fRadius, DWORD dwNumRings);

HRESULT CreateSphere2(D3DVERTEX **ppVertices, DWORD *pdwNumVertices,
                      WORD **ppIndices, DWORD *pdwNumIndices,
                      FLOAT fRadius, DWORD dwNumRings);

float fixangle(float angle, float adjust);
FLOAT g_fCurrentTime;
extern LPDIRECTPLAY4A g_pDP;
extern LPDPLCONNECTION g_pDPLConnection;
extern LPDIRECTPLAYLOBBY3 g_pDPLobby;
extern GUID g_AppGUID;
extern HANDLE g_hDPMessageEvent;
extern HKEY g_hDPlaySampleRegKey;
extern TCHAR g_strPreferredProvider[MAX_SESSION_NAME];
extern TCHAR g_strSessionName[MAX_SESSION_NAME];
extern TCHAR g_strLocalPlayerName[MAX_PLAYER_NAME];
extern DPID g_LocalPlayerDPID;
extern BOOL g_bHostPlayer;
extern TCHAR g_strAppName[256];
extern BOOL g_bUseProtocol;
extern VOID *g_pvDPMsgBuffer;
extern DWORD g_dwDPMsgBufferSize;
extern DWORD g_dwNumberOfActivePlayers;

extern LPDIRECTINPUTDEVICE2 g_Keyboard_pdidDevice2;
extern LPDIRECTINPUT7 g_Keyboard_pDI;
extern CMD2 *pCMD2;
extern C3DS *pC3DS;
extern char rr_multiplay_chat_string[1024];
extern DPLAYINFO DPInfo;

BOOL logfile_start_flag = TRUE;

SETUPINFO setupinfo;
HDC hdc;

float spheresize = 25.0f;
float sphereheight = 50.0f;

void SWAP(int &x, int &y, int &temp);
float FastDistance(float fx, float fy, float fz);
void PrintMemAllocated(int mem, char *message);
VOID DisplayError(CHAR *strMessage);

int PrintMsgX = 10;
int PrintMsgY = 10;
int total_allocated_memory_count = 0;

CONTROLS Controls;

CMyD3DApplication *pCMyApp;
extern CLoadWorld *pCWorld;

//-----------------------------------------------------------------------------
// Name: AboutProc()
// Desc: Minimal message proc function for the about box
//-----------------------------------------------------------------------------
BOOL CALLBACK AboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM) {
	if (WM_COMMAND == uMsg)
		if (IDOK == LOWORD(wParam) || IDCANCEL == LOWORD(wParam))
			EndDialog(hWnd, TRUE);

	return (WM_INITDIALOG == uMsg) ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication() {
	m_strWindowTitle = TEXT("Dungeon Stomp");
	m_bAppUseZBuffer = TRUE;
	// m_fnConfirmDevice = NULL;
	m_fnConfirmDevice = ConfirmDevice;
	m_dltType = D3DLIGHT_SPOT;
	k = (float)0.017453292;

	m_vUpVec = D3DVECTOR(0, 1, 0);
	m_tableFog = FALSE;

	rendering_first_frame = TRUE;
	RRAppActive = FALSE;
	firing_gun_flag = FALSE;
	delay_key_press_flag = FALSE;
	multiplay_flag = FALSE;
	display_scores = FALSE;
	bInWalkMode = TRUE;
	bInTalkMode = FALSE;
	num_packets_sent = 0;
	num_packets_received = 0;
	m_bShowStats = FALSE;
	m_bWindowed = TRUE;
	m_bToggleFullscreen = FALSE;
	m_bUseViewPoints = FALSE;

	m_toggleFog = FALSE;
	m_num_sounds = 0;
	num_players = 0;
	num_players2 = 0;
	num_your_guns = 0;
	num_cars = 0;
	num_debug_models = 0;
	current_frame = 0;
	current_sequence = 0;
	current_frame = 0;
	current_sequence = 0;

	car_gear = 1;
	editor_insert_mode = 0;
	object_rot_angle = 0;
	toggle_delay = 0;

	tx_rate_ps = 90;
	rx_rate_ps = 90;

	current_screen_status = FULLSCREEN_ONLY; // default, in case rrsetup.ini

	num_packets_received_ps = 0;
	num_packets_sent_ps = 0;

	pi = 3.141592654f;
	piover2 = (pi / 2.0f);

	Z_NEAR = D3DVAL(6.0);
	Z_FAR = D3DVAL(4000.0);

	Z_FAR = D3DVAL(54000.0);

	float angle = 50.0f;
	FOV = angle * D3DVAL((pi / 180.0f)); // Field Of View set to 90 degrees

	// Move the camera position around
	D3DVECTOR vFrom(700.0f, 22.0f, 700.0f);
	D3DVECTOR vAt(850.0f, 25.0f, 480.0f);
	D3DVECTOR vUp(0.0f, 1.0f, 0.0f);
	SetViewParams(&vFrom, &vAt, &vUp, 0.1f);

	IsRenderingOk = FALSE;
	bEnableAlphaTransparency = TRUE;

	bEnableLighting = TRUE;
	ShadeMode = D3DSHADE_GOURAUD;

	// 2400 //9400
	CreateSphere(&g_pObstacleVertices, &g_dwNumObstacleVertices,
	             &g_pObstacleIndices, &g_dwNumObstacleIndices, (float)9400.0f, 36);

	CreateSphere2(&g_pObstacleVertices2, &g_dwNumObstacleVertices2,
	              &g_pObstacleIndices2, &g_dwNumObstacleIndices2, (float)spheresize, 10);

	pCMyApp = this;
}

//-----------------------------------------------------------------------------
// Name: OutputText()
// Desc: Draws text on the window.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::OutputText(DWORD x, DWORD y, TCHAR *str) {
	HDC hDC;

	// Get a DC for the surface. Then, write out the buffer
	if (m_pddsRenderTarget) {
		if (SUCCEEDED(m_pddsRenderTarget->GetDC(&hDC))) {
			SetTextColor(hDC, RGB(255, 255, 0));
			SetBkMode(hDC, TRANSPARENT);
			ExtTextOut(hDC, x, y, 0, NULL, str, lstrlen(str), NULL);
			m_pddsRenderTarget->ReleaseDC(hDC);
		}
	}

	// Do the same for the left surface (in case of stereoscopic viewing).
	if (m_pddsRenderTargetLeft) {
		if (SUCCEEDED(m_pddsRenderTargetLeft->GetDC(&hDC))) {
			// Use a different color to help distinguish left eye view
			SetTextColor(hDC, RGB(255, 0, 255));
			SetBkMode(hDC, TRANSPARENT);
			ExtTextOut(hDC, x, y, 0, NULL, str, lstrlen(str), NULL);
			m_pddsRenderTargetLeft->ReleaseDC(hDC);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: ShowStats()
// Desc: Shows frame rate and dimensions of the rendering device.
//-----------------------------------------------------------------------------

VOID CMyD3DApplication::ShowStats() {
	static FLOAT fFPS = 0.0f;
	static LONGLONG fLastTimeFPS = 0;
	static LONGLONG fLastTimeFPSLast = 0;
	static DWORD dwFrames = 0L;

	// Keep track of the time lapse and frame count
	// FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds
	FLOAT fTime;
	fLastTimeFPS = DSTimer();
	fTime = (float)((fLastTimeFPS - fLastTimeFPSLast) * (float)time_factor);

	++dwFrames;

	// Update the frame rate once per second
	if ((fTime) > 1.0f) {
		fFPS = dwFrames / fTime;
		fLastTimeFPSLast = fLastTimeFPS;
		dwFrames = 0L;
		FLOAT fTime2 = (timeGetTime()) * 0.001f;
		num_packets_received_ps = num_packets_received;
		num_packets_sent_ps = num_packets_sent;
		num_packets_received = 0;
		num_packets_sent = 0;
	}

	if (displaychat == 0)
		return;

	// Setup the text buffer to write out dimensions
	CHAR buffer[80];
	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	m_pddsRenderTarget->GetSurfaceDesc(&ddsd);

	sprintf_s(buffer, "%3.02f FPS (%dx%dx%d)", fFPS, ddsd.dwWidth,
	          ddsd.dwHeight, ddsd.ddpfPixelFormat.dwRGBBitCount);

	sprintf_s(fpsbuffer, "%3.02f FPS %dx%dx%d", fFPS, ddsd.dwWidth,
	          ddsd.dwHeight, ddsd.ddpfPixelFormat.dwRGBBitCount);

	strcpy_s(debug_buffer[0], buffer);

	sprintf_s(buffer, "Health   %d", player_list[trueplayernum].health);
	sprintf_s(buffer, "xp %d lvl %d y %f", player_list[trueplayernum].xp, player_list[trueplayernum].hd, player_list[trueplayernum].y);
	sprintf_s(player_list[trueplayernum].chatstr, "%d", player_list[trueplayernum].health);
}

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one,
//       create a device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE pInst,
                                    VOID *pvContext) {
	memcpy(pvContext, &pInst->guidInstance, sizeof(GUID));

	return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: InputDeviceSelectProc()
// Desc: Dialog procedure for selecting an input device.
//-----------------------------------------------------------------------------
BOOL CALLBACK InputDeviceSelectProc(HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) {
	HWND hwndKeyboardButton = GetDlgItem(hWnd, IDC_KEYBOARD);
	HWND hwndMouseButton = GetDlgItem(hWnd, IDC_MOUSE);
	HWND hwndJoystickButton = GetDlgItem(hWnd, IDC_JOYSTICK);

	HWND hwndMusicCheck = GetDlgItem(hWnd, IDC_CHECK1);

	if (WM_INITDIALOG == msg) {

		if (musicon == 1)
			SendMessage(hwndMusicCheck, BM_SETCHECK, 1, 0L);
		else
			SendMessage(hwndMusicCheck, BM_SETCHECK, 0, 0L);

		SendMessage(hwndKeyboardButton, BM_SETCHECK, 0, 0L);
		SendMessage(hwndMouseButton, BM_SETCHECK, 0, 0L);
		SendMessage(hwndJoystickButton, BM_SETCHECK, 0, 0L);
		if (pCMyApp->currentinputtype == 1)
			SendMessage(hwndKeyboardButton, BM_SETCHECK, 1, 0L);
		else if (pCMyApp->currentinputtype == 2)
			SendMessage(hwndMouseButton, BM_SETCHECK, 1, 0L);
		else if (pCMyApp->currentinputtype == 3)
			SendMessage(hwndJoystickButton, BM_SETCHECK, 1, 0L);

		EnableWindow(hwndJoystickButton, (g_guidJoystick != GUID_NULL));

		return TRUE;
	}

	if (WM_COMMAND == msg && IDOK == LOWORD(wParam)) {
		// Destroy the old device
		//   pCMyApp->DestroyInputDevice();

		// Check the dialog controls to see which device type to create
		int g_bUseKeyboard1 = SendMessage(hwndKeyboardButton, BM_GETCHECK, 0, 0L);
		int g_bUseMouse1 = SendMessage(hwndMouseButton, BM_GETCHECK, 0, 0L);
		int g_bUseJoystick1 = SendMessage(hwndJoystickButton, BM_GETCHECK, 0, 0L);

		int g_bUseMusic = SendMessage(hwndMusicCheck, BM_GETCHECK, 0, 0L);

		if (g_bUseMusic)
			musicon = 1;
		else
			musicon = 0;

		if (g_bUseKeyboard1) {
			pCMyApp->currentinputtype = 1;
		}

		if (g_bUseMouse1) {
			pCMyApp->currentinputtype = 2;
		}

		if (g_bUseJoystick1) {
			pCMyApp->CreateInputDevice(GetParent(hWnd), g_Keyboard_pDI,
			                           g_Keyboard_pdidDevice2,
			                           GUID_SysKeyboard, &c_dfDIKeyboard,
			                           DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
			g_Keyboard_pdidDevice2->Acquire();

			pCMyApp->CreateInputDevice(GetParent(hWnd), g_pDI,
			                           g_pdidDevice2,
			                           g_guidJoystick, &c_dfDIJoystick,
			                           DISCL_EXCLUSIVE | DISCL_FOREGROUND);
			g_pdidDevice2->Acquire();

			// Set the range of the joystick axes tp [-1000,+1000]
			DIPROPRANGE diprg;
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYOFFSET;
			diprg.lMin = -10;
			diprg.lMax = +10;

			diprg.diph.dwObj = DIJOFS_X; // Set the x-axis range
			g_pdidDevice2->SetProperty(DIPROP_RANGE, &diprg.diph);

			diprg.diph.dwObj = DIJOFS_Y; // Set the y-axis range
			g_pdidDevice2->SetProperty(DIPROP_RANGE, &diprg.diph);

			// Set the dead zone for the joystick axes (because many joysticks
			// aren't perfectly calibrated to be zero when centered).
			DIPROPDWORD dipdw;
			dipdw.diph.dwSize = sizeof(DIPROPDWORD);
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipdw.diph.dwHow = DIPH_DEVICE;
			dipdw.dwData = 1000; // Here, 1000 = 10%

			dipdw.diph.dwObj = DIJOFS_X; // Set the x-axis deadzone
			g_pdidDevice2->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

			dipdw.diph.dwObj = DIJOFS_Y; // Set the y-axis deadzone
			g_pdidDevice2->SetProperty(DIPROP_RANGE, &dipdw.diph);

			pCMyApp->currentinputtype = 3;
		}

		pCMyApp->SaveDSini();
		pCMyApp->switchcontrols = 1;

		EndDialog(hWnd, IDOK);
		return TRUE;
	}

	if (WM_COMMAND == msg && IDCANCEL == LOWORD(wParam)) {
		EndDialog(hWnd, IDCANCEL);
		return TRUE;
	}

	return FALSE;
}

BOOL CALLBACK CaptionProc(HWND hWnd, UINT msg, WPARAM wParam,
                          LPARAM lParam) {

	if (WM_COMMAND == msg && IDCANCEL == LOWORD(wParam)) {
		EndDialog(hWnd, IDCANCEL);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: DisplayError()
// Desc: Displays an error message.
//-----------------------------------------------------------------------------
VOID DisplayError(CHAR *strMessage) {
	MessageBox(NULL, strMessage, "DInput Sample Game", MB_OK);
}

//-----------------------------------------------------------------------------
// Name: CreateDInput()
// Desc: Initialize the DirectInput objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateDInput(HWND hWnd) {
#define DIDEVTYPE_JOYSTICK 4
	extern HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);
	// Create the main DirectInput object

	PrintMessage(NULL, "CD3DApplication::CreateDInput()", NULL, LOGFILE_ONLY);

	HRESULT h = DirectInputCreateEx((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), DIRECTINPUT_VERSION, IID_IDirectInput7, (LPVOID *)&g_pDI, NULL);

	if (FAILED(h)) {
		DisplayError("DirectInputCreate() failed.");
		return E_FAIL;
	}

	// Check to see if a joystick is present. If so, the enumeration callback
	// will save the joystick's GUID, so we can create it later.
	ZeroMemory(&g_guidJoystick, sizeof(GUID));

	g_pDI->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoysticksCallback,
	                   &g_guidJoystick, DIEDFL_ATTACHEDONLY);

	// keyboard

	if (FAILED(DirectInputCreateEx((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	                               DIRECTINPUT_VERSION, IID_IDirectInput7, (LPVOID *)&g_Keyboard_pDI, NULL))) {
		DisplayError("DirectInputCreate() failed.");
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DestroyDInput()
// Desc: Terminate our usage of DirectInput
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::DestroyDInput() {
	// Destroy the DInput object
	if (g_pDI)
		g_pDI->Release();
	g_pDI = NULL;
}

//-----------------------------------------------------------------------------
// Name: CreateInputDevice()
// Desc: Create a DirectInput device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateInputDevice(HWND hWnd,
                                             LPDIRECTINPUT7 pDI,
                                             LPDIRECTINPUTDEVICE2 pDIdDevice,
                                             GUID guidDevice,
                                             const DIDATAFORMAT *pdidDataFormat, DWORD dwFlags) {

	PrintMessage(NULL, "CD3DApplication::CreateInputDevice()", NULL, LOGFILE_ONLY);

	// Obtain an interface to the input device
	if (FAILED(pDI->CreateDeviceEx(guidDevice, IID_IDirectInputDevice2,
	                               (VOID **)&pDIdDevice, NULL))) {
		PrintMessage(NULL, "CD3DApplication::CreateInputDevice() - CreateDeviceEx FAILED", NULL, LOGFILE_ONLY);
		DisplayError("CreateDeviceEx() failed");
		return E_FAIL;
	} else
		PrintMessage(NULL, "CD3DApplication::CreateInputDevice() - CreateDeviceEx ok", NULL, LOGFILE_ONLY);

	// Set the device data format. Note: a data format specifies which
	// controls on a device we are interested in, and how they should be
	// reported.
	if (FAILED(pDIdDevice->SetDataFormat(pdidDataFormat))) {
		DisplayError("SetDataFormat() failed");
		return E_FAIL;
	}

	// Set the cooperativity level to let DirectInput know how this device
	// should interact with the system and with other DirectInput applications.
	if (FAILED(pDIdDevice->SetCooperativeLevel(hWnd, dwFlags))) {
		DisplayError("SetCooperativeLevel() failed");
		return E_FAIL;
	}

	if (guidDevice == GUID_SysKeyboard)
		g_Keyboard_pdidDevice2 = pDIdDevice;
	else
		g_pdidDevice2 = pDIdDevice;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DestroyInputDevice()
// Desc: Release the DirectInput device
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::DestroyInputDevice() {
	// Unacquire and release the device's interfaces
	if (g_pdidDevice2) {
		g_pdidDevice2->Unacquire();
		g_pdidDevice2->Release();
		g_pdidDevice2 = NULL;
	}

	// keyboard

	if (g_Keyboard_pdidDevice2) {
		g_Keyboard_pdidDevice2->Unacquire();
		g_Keyboard_pdidDevice2->Release();
		g_Keyboard_pdidDevice2 = NULL;
	}
}

/*
HRESULT CreateSphere( D3DVERTEX** ppVertices, DWORD* pdwNumVertices,
                      WORD** ppIndices, DWORD* pdwNumIndices,
                      FLOAT fRadius, DWORD dwNumRings )
{
    // Allocate memory for the vertices and indices
    DWORD      dwNumVertices = (dwNumRings*(2*dwNumRings+1)+2);
    DWORD      dwNumIndices  = 6*(dwNumRings*2)*((dwNumRings-1)+1);
    D3DVERTEX* pVertices     = new D3DVERTEX[dwNumVertices];
    WORD*      pIndices      = new WORD[dwNumIndices];

    (*ppVertices) = pVertices;
    (*ppIndices)  = pIndices;

    // Counters
    WORD x, y, vtx = 0, index = 0;

    // Angle deltas for constructing the sphere's vertices
    FLOAT fDAng   = g_PI / dwNumRings;
    FLOAT fDAngY0 = fDAng;

    // Make the middle of the sphere
    for( y=0; y<dwNumRings; y++ )
    {
        FLOAT y0 = (FLOAT)cos(fDAngY0);
        FLOAT r0 = (FLOAT)sin(fDAngY0);
        FLOAT tv = (1.0f - y0)/2;

        for( x=0; x<(dwNumRings*2)+1; x++ )
        {
            FLOAT fDAngX0 = x*fDAng;

            D3DVECTOR v( r0*(FLOAT)sin(fDAngX0), y0, (r0*(FLOAT)cos(fDAngX0)) );
            FLOAT tu = 1.0f - x/(dwNumRings*2.0f);

            *pVertices++ = D3DVERTEX( fRadius*v, v, tu, tv );
            vtx ++;
        }
        fDAngY0 += fDAng;
    }

    for( y=0; y<dwNumRings-1; y++ )
    {
        for( x=0; x<(dwNumRings*2); x++ )
        {
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+0) );
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) );
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) );
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) );
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) );
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+1) );
            index += 6;
        }
    }
    // Make top and bottom
    D3DVECTOR vy( 0, 1, 0 );
    WORD wNorthVtx = vtx;
    *pVertices++ = D3DVERTEX( fRadius*vy, vy, 0.5f, 0.0f );
    vtx++;
    WORD wSouthVtx = vtx;
    *pVertices++ = D3DVERTEX( -fRadius*vy,-vy, 0.5f, 1.0f );
    vtx++;

    for( x=0; x<(dwNumRings*2); x++ )
    {
        WORD p1 = wSouthVtx;
        WORD p2 = (WORD)( (y)*(dwNumRings*2+1) + (x+1) );
        WORD p3 = (WORD)( (y)*(dwNumRings*2+1) + (x+0) );

        *pIndices++ = p1;
        *pIndices++ = p3;
        *pIndices++ = p2;
        index += 3;
    }

    for( x=0; x<(dwNumRings*2); x++ )
    {
        WORD p1 = wNorthVtx;
        WORD p2 = (WORD)( (0)*(dwNumRings*2+1) + (x+1) );
        WORD p3 = (WORD)( (0)*(dwNumRings*2+1) + (x+0) );

        *pIndices++ = p1;
        *pIndices++ = p3;
        *pIndices++ = p2;
        index += 3;
    }

    (*pdwNumVertices) = vtx;
    (*pdwNumIndices)  = index;


    return S_OK;
}
*/

VOID CMyD3DApplication::UpdateControls() {
	int i;
	int look_up = 0;
	int look_down = 0;

	// Note we want to always keyboard, either alone, or with another input
	// device such as a mouse or joystick etc

	// Read from input devices
	if ((g_Keyboard_pdidDevice2) || (g_pdidDevice2)) {
		HRESULT hr;
		//  BYTE         diks[256]; // DInput keyboard state buffer
		DIMOUSESTATE dims; // DInput mouse state structure
		DIJOYSTATE dijs;   // DInput joystick state structure

		// read the current keyboard state
		if (g_Keyboard_pdidDevice2) {
			g_Keyboard_pdidDevice2->Acquire();
			hr = g_Keyboard_pdidDevice2->GetDeviceState(sizeof(diks), &diks);
		}

		// read the current mouse and keyboard state
		if (g_bUseMouse) {
			g_Keyboard_pdidDevice2->Acquire();
			hr = g_Keyboard_pdidDevice2->GetDeviceState(sizeof(diks), &diks);

			g_pdidDevice2->Acquire();
			hr = g_pdidDevice2->GetDeviceState(sizeof(DIMOUSESTATE), &dims);
		}

		// read the current joystick state
		if (g_bUseJoystick) {
			// Poll the device before reading the current state. This is required
			// for some devices (joysticks) but has no effect for others (keyboard
			// and mice). Note: this uses a DIDevice2 interface for the device.

			g_Keyboard_pdidDevice2->Acquire();
			hr = g_Keyboard_pdidDevice2->GetDeviceState(sizeof(diks), &diks);

			g_pdidDevice2->Poll();

			g_pdidDevice2->Acquire();
			hr = g_pdidDevice2->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
		}

		// Check whether the input stream has been interrupted. If so,
		// re-acquire and try again.
		if (hr == DIERR_INPUTLOST) {
			PrintMessage(NULL, "DIERR_INPUTLOST", NULL, LOGFILE_ONLY);

			hr = g_pdidDevice2->Acquire();
			if (FAILED(hr)) {
				PrintMessage(NULL, "Acquire Input device FAILED", NULL, LOGFILE_ONLY);
				return; // S_OK;
			}
		}

		// Read Keyboard input only
		if (g_Keyboard_pdidDevice2) {
			Controls.bLeft = diks[DIK_A] && 0x80;
			Controls.bRight = diks[DIK_D] && 0x80;
			Controls.bForward = diks[DIK_W] && 0x80;
			Controls.bBackward = diks[DIK_S] && 0x80;
			// Controls.bUp = diks[DIK_NUMPADPLUS] && 0x80;
			// Controls.bDown = diks[DIK_NUMPADMINUS] && 0x80;
			Controls.bHeadUp = diks[DIK_PGUP] && 0x80;
			Controls.bHeadDown = diks[DIK_PGDN] && 0x80;
			Controls.bStepLeft = diks[DIK_COMMA] && 0x80;
			Controls.bStepRight = diks[DIK_PERIOD] && 0x80;
			Controls.bPrevWeap = diks[DIK_INSERT] && 0x80;
			Controls.bNextWeap = diks[DIK_DELETE] && 0x80;
			Controls.bInTalkMode = diks[DIK_SLASH] && 0x80;
			Controls.opendoor = diks[DIK_SPACE] && 0x80;
			// Check both left and right control keys

			Controls.bFire2 = diks[DIK_END] && 0x80;

			Controls.bCameraleft = diks[DIK_LSHIFT] && 0x80;
			Controls.bCameraright = diks[DIK_RSHIFT] && 0x80;
			Controls.missle = diks[DIK_E] && 0x80;
			Controls.bFire = diks[DIK_RCONTROL] && 0x80;
			Controls.spell = diks[DIK_RCONTROL] && 0x80;
		}

		// Read mouse input
		if (g_bUseMouse) {
			Controls.bLeft = 0;
			Controls.bRight = 0;
			Controls.bHeadUp = 0;
			Controls.bHeadDown = 0;

			int threshhold = 5;

			if (dims.lX < 0)
				Controls.bLeft = dims.lX;
			if (dims.lX > 0)
				Controls.bRight = dims.lX;

			if (dims.lY < 0)
				Controls.bHeadUp = dims.lY;
			if (dims.lY > 0)
				Controls.bHeadDown = dims.lY;

			Controls.bFire = dims.rgbButtons[0] && 0x80;
			Controls.opendoor = diks[DIK_SPACE] && 0x80;
			Controls.bForward = dims.rgbButtons[1] && 0x80;

			if (Controls.bForward == 0)
				Controls.bForward = diks[DIK_W] && 0x80;

			Controls.bStepLeft = diks[DIK_A] && 0x80;
			Controls.bStepRight = diks[DIK_D] && 0x80;

			Controls.bBackward = diks[DIK_S] && 0x80;
			Controls.spell = dims.rgbButtons[0] && 0x80;

			Controls.missle = diks[DIK_E] && 0x80;
		}

		// Read joystick input
		if (g_bUseJoystick) {

			/*
			Control PAD
			if (dijs.rgdwPOV[0] == -1)
			{
			}
			else
			{

			    if (dijs.rgdwPOV[0] >= 0 && dijs.rgdwPOV[0] <= 4500 ||
			        dijs.rgdwPOV[0] >= 31500)
			    {
			        //up
			        Controls.bForward = 1;
			    }
			    if (dijs.rgdwPOV[0] >= 4500 && dijs.rgdwPOV[0] <= 13500)
			    {
			        //right
			        Controls.bRight = 1;
			    }
			    if (dijs.rgdwPOV[0] >= 13500 && dijs.rgdwPOV[0] <= 22500)
			    {
			        //down
			        Controls.bBackward = 1;
			    }

			    if (dijs.rgdwPOV[0] >= 27000 && dijs.rgdwPOV[0] <= 36000)
			    {
			        //left
			        Controls.bLeft = -1;
			    }
			}

			*/

			// Control PAD
			if (dijs.rgdwPOV[0] == -1) {
				cycleweaponbuttonpressed = FALSE;
			} else if (dijs.rgdwPOV[0] >= 4500 && dijs.rgdwPOV[0] <= 13500 && cycleweaponbuttonpressed == FALSE) {
				// right
				CycleNextWeapon();
				cycleweaponbuttonpressed = TRUE;
			} else if (dijs.rgdwPOV[0] >= 27000 && dijs.rgdwPOV[0] <= 36000 && cycleweaponbuttonpressed == FALSE) {
				// left
				CyclePreviousWeapon();
				cycleweaponbuttonpressed = TRUE;
			}

			// Right controller - look around

			if (dijs.lRx > 32767.0) {
				// right
				Controls.bRight = 1;
			}

			if (dijs.lRx < 32767.0) {
				// left
				Controls.bLeft = -1;
			}

			if (dijs.lRy < 32767) {
				// up
				Controls.bHeadUp = -1.0f;
			}

			if (dijs.lRy > 32767) {
				// down
				Controls.bHeadDown = 1.0f;
			}

			// Left Controller - Move
			if (dijs.lX >= 6.0) {
				// right
				Controls.bStepRight = 1;
			}

			if (dijs.lX <= -6.0) {
				// left
				Controls.bStepLeft = -1;
			}

			if (dijs.lY < 0.0) {
				Controls.bForward = 1;
			}
			if (dijs.lY > 0.0) {
				Controls.bBackward = 1;
			}

			// sprintf_s(gActionMessage, "%ld %ld %ld", dijs.lRx,dijs.lRy,dijs.lRz); //right controller
			// sprintf_s(gActionMessage, "%ld %ld %ld", dijs.lX,dijs.lY,dijs.lZ); //left controller
			// sprintf_s(gActionMessage, "%ld", dijs.rgdwPOV[0]);  //Control PAD
			// UpdateScrollList(0, 245, 255);

			// Button xbox  0=A 1=B 2=X 3=Y
			Controls.bFire = dijs.rgbButtons[2] && 0x80;    // Fire
			Controls.missle = dijs.rgbButtons[0] && 0x80;   // Jump
			Controls.opendoor = dijs.rgbButtons[3] && 0x80; // Door
			Controls.spell = dijs.rgbButtons[2] && 0x80;    // Spell

			Controls.bPrevWeap = diks[DIK_INSERT] && 0x80;
			Controls.bNextWeap = diks[DIK_DELETE] && 0x80;

			Controls.dialogpausejoystick = dijs.rgbButtons[3] && 0x80;
			Controls.changeviews = dijs.rgbButtons[1] && 0x80;
		}

		for (i = 0; i < 256; i++) {
			if ((diks[i] && 0x80) == FALSE)
				DelayKey2[i] = FALSE;
		}

		if (RRAppActive == TRUE && dialogpause == 0) {
			MovePlayer(&Controls);
		}

		if (Controls.dialogpausejoystick && dialogpause) {
			gtext[dialognum].shown = 1;
			dialogpause = 0;
		}

		if (!Controls.changeviews) {
			changeviewbuttonpressed = FALSE;
		}

		if (Controls.changeviews && changeviewbuttonpressed == FALSE) {
			SwitchView();
			changeviewbuttonpressed = TRUE;
		}
	}
}

VOID CMyD3DApplication::UpdateTalk() {
	int i;
	int look_up = 0;
	int look_down = 0;

	// Note we want to always keyboard, either alone, or with another input
	// device such as a mouse or joystick etc

	// Read from input devices
	if ((g_Keyboard_pdidDevice2) || (g_pdidDevice2)) {
		HRESULT hr;
		//  BYTE         diks[256]; // DInput keyboard state buffer
		//      DIMOUSESTATE dims;      // DInput mouse state structure
		//    DIJOYSTATE   dijs;      // DInput joystick state structure

		// read the current keyboard state
		if (g_Keyboard_pdidDevice2) {
			g_Keyboard_pdidDevice2->Acquire();
			hr = g_Keyboard_pdidDevice2->GetDeviceState(sizeof(diks), &diks);
		}

		if (hr == DIERR_INPUTLOST) {
			PrintMessage(NULL, "DIERR_INPUTLOST", NULL, LOGFILE_ONLY);

			hr = g_pdidDevice2->Acquire();
			if (FAILED(hr)) {
				PrintMessage(NULL, "Acquire Input device FAILED", NULL, LOGFILE_ONLY);
				return; // S_OK;
			}
		}

		for (i = 0; i < 256; i++) {
			if ((diks[i] && 0x80) == FALSE)
				DelayKey2[i] = FALSE;
		}

		if ((diks[DIK_SLASH] && 0x80) && (DelayKey2[DIK_SLASH] == FALSE)) {
			bInTalkMode = !bInTalkMode;
			ResetChatString();
			DelayKey2[DIK_SLASH] = TRUE;
		}

		HandleTalkMode(diks);
	}
}

//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::Cleanup3DEnvironment() {
	SetbActive(FALSE);
	SetbReady(FALSE);

	if (GetFramework()) {
		DeleteDeviceObjects();
		DeleteFramework();

		FinalCleanup();
	}

	D3DEnum_FreeResources();
}

LRESULT CMyD3DApplication::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam) {
	int MyPlayerNum;

	float fStart;
	float fEnd;
	float fDensity = 1.0;
	BOOL fUseRange = TRUE;

	HMENU hMenu;
	int type = 0;
	m_hWnd = hWnd;

	hMenu = GetMenu(hWnd);

	switch (uMsg) {

	case WM_CHAR:

		OnCharDown(wParam);

		break;

	case WM_KEYDOWN:

		OnKeyDown(wParam);
		break;

	case WM_ACTIVATEAPP:
		RRAppActive = (BOOL)wParam;
		break;

	case WM_CREATE:
		SetTimer(hWnd, ANIMATION_TIMER, 90, NULL);
		SetTimer(hWnd, GUN_TIMER, 90, NULL);
		break;

	case WM_TIMER:

		if (wParam == ANIMATION_TIMER) {
			if (GetFramework() && GetbActive() && GetbReady() && m_bWindowed) {

				// if (IsRenderingOk == TRUE && dialogpause == 0)
				//	AnimateCharacters();
			}
		}
		if (wParam == GUN_TIMER) {
			// if (GetFramework() && GetbActive() && GetbReady() && m_bWindowed)
			//{
			// }
		}

		if (wParam == RESPAWN_TIMER) {
			m_vEyePt.x = 700;
			m_vEyePt.y = 50;
			m_vEyePt.z = 700;

			MyHealth = 100;

			// MyPlayerNum = GetPlayerNumber(MyRRnetID);

			MyPlayerNum = 1;

			player_list[MyPlayerNum].x = m_vEyePt.x;
			player_list[MyPlayerNum].y = m_vEyePt.y;
			player_list[MyPlayerNum].z = m_vEyePt.z;
			player_list[MyPlayerNum].rot_angle = 0;
			player_list[MyPlayerNum].bIsPlayerAlive = TRUE;
			player_list[MyPlayerNum].bStopAnimating = FALSE;
			player_list[MyPlayerNum].current_weapon = 0;
			player_list[MyPlayerNum].current_car = 0;
			player_list[MyPlayerNum].current_frame = 0;
			player_list[MyPlayerNum].current_sequence = 0;
			player_list[MyPlayerNum].bIsPlayerInWalkMode = TRUE;
			player_list[MyPlayerNum].health = 100;

			KillTimer(hWnd, RESPAWN_TIMER);
		}
		break;

	case WM_INITMENUPOPUP:
		CheckMenuItem(hMenu, ID_TEXTURES_ALPHATRANSPARENCY, bEnableAlphaTransparency ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);

		if (ShadeMode == D3DSHADE_FLAT) {
			CheckMenuItem(hMenu, MENU_FLAT, MF_CHECKED);
			CheckMenuItem(hMenu, MENU_GOURAUD, MF_UNCHECKED);
		}
		if (ShadeMode == D3DSHADE_GOURAUD) {
			CheckMenuItem(hMenu, MENU_FLAT, MF_UNCHECKED);
			CheckMenuItem(hMenu, MENU_GOURAUD, MF_CHECKED);
		}

		if (mousefilter == 1) {
			CheckMenuItem(hMenu, ID_RENDER_MOUSEFILTER, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_MOUSEFILTER, MF_UNCHECKED);
		}

		if (highperftimer == 1) {
			CheckMenuItem(hMenu, ID_RENDER_PERFTIMER, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_PERFTIMER, MF_UNCHECKED);
		}

		if (collisioncode == 0) {
			CheckMenuItem(hMenu, ID_RENDER_COLLISION, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_COLLISION, MF_UNCHECKED);
		}

		if (musicon == 1) {
			CheckMenuItem(hMenu, ID_LIGHTING_MUSIC, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_LIGHTING_MUSIC, MF_UNCHECKED);
		}

		if (antialias == 1) {
			CheckMenuItem(hMenu, ID_TEXTURES_ANTIALIAS, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_TEXTURES_ANTIALIAS, MF_UNCHECKED);
		}

		if (filtertype == 0) {
			CheckMenuItem(hMenu, ID_TEXTURES_POINTFILTER, MF_CHECKED);
			CheckMenuItem(hMenu, ID_TEXTURES_LINEARFILTER, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_TEXTURES_ANISOTROPICFILTER, MF_UNCHECKED);
		}
		if (filtertype == 1) {
			CheckMenuItem(hMenu, ID_TEXTURES_POINTFILTER, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_TEXTURES_LINEARFILTER, MF_CHECKED);
			CheckMenuItem(hMenu, ID_TEXTURES_ANISOTROPICFILTER, MF_UNCHECKED);
		}
		if (filtertype == 2) {
			CheckMenuItem(hMenu, ID_TEXTURES_POINTFILTER, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_TEXTURES_LINEARFILTER, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_TEXTURES_ANISOTROPICFILTER, MF_CHECKED);
		}

		if (showpanel == 1) {
			CheckMenuItem(hMenu, ID_RENDER_PANEL, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_PANEL, MF_UNCHECKED);
		}

		if (menuflares == 1) {
			CheckMenuItem(hMenu, ID_LIGHTING_FLARES, MF_CHECKED);
		} else if (menuflares == 0) {
			CheckMenuItem(hMenu, ID_LIGHTING_FLARES, MF_UNCHECKED);
		}

		//

		if (crosshairenabled == 1) {
			CheckMenuItem(hMenu, ID_RENDER_CROSSHAIR, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_CROSSHAIR, MF_UNCHECKED);
		}

		if (showmonstermenu == 1) {
			CheckMenuItem(hMenu, ID_MONSTERS_SHOWBODIES, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_MONSTERS_SHOWBODIES, MF_UNCHECKED);
		}

		if (polysorton == 1) {
			CheckMenuItem(hMenu, ID_RENDER_SORTPOLY, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_SORTPOLY, MF_UNCHECKED);
		}

		if (ambientlighton == 1) {
			CheckMenuItem(hMenu, ID_LIGHTING_AMBIENTLIGHT, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_LIGHTING_AMBIENTLIGHT, MF_UNCHECKED);
		}

		if (monstermoveon == 1) {
			CheckMenuItem(hMenu, ID_MONSTERS_MOVE, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_MONSTERS_MOVE, MF_UNCHECKED);
		}

		if (lockframerate == 1) {
			CheckMenuItem(hMenu, ID_LOCK_FRAMERATE, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_LOCK_FRAMERATE, MF_UNCHECKED);
		}

		if (wireframe == 1) {
			CheckMenuItem(hMenu, ID_RENDER_WIREFRAME, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_WIREFRAME, MF_UNCHECKED);
		}

		if (solid == 1) {
			CheckMenuItem(hMenu, ID_RENDER_SOLID, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_SOLID, MF_UNCHECKED);
		}

		if (zbufferenable == 1) {
			CheckMenuItem(hMenu, ID_RENDER_ZBUFFER, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_ZBUFFER, MF_UNCHECKED);
		}

		if (monsterenable == 1) {
			CheckMenuItem(hMenu, ID_MONSTERS_ENABLED, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_MONSTERS_ENABLED, MF_UNCHECKED);
		}

		if (point == 1) {
			CheckMenuItem(hMenu, ID_RENDER_POINT, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_POINT, MF_UNCHECKED);
		}

		if (displaycap == 1) {
			CheckMenuItem(hMenu, ID_ACTION_SHOWCAPTIONS, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_ACTION_SHOWCAPTIONS, MF_UNCHECKED);
		}

		if (normalon == 1) {
			CheckMenuItem(hMenu, ID_RENDER_SHOWNORMALS, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_SHOWNORMALS, MF_UNCHECKED);
		}

		if (specularon == 1) {
			CheckMenuItem(hMenu, ID_LIGHTING_SPECULAR, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_LIGHTING_SPECULAR, MF_UNCHECKED);
		}

		if (displaychat == 1) {
			CheckMenuItem(hMenu, ID_ACTION_SHOWCHAT, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_ACTION_SHOWCHAT, MF_UNCHECKED);
		}
		if (showdebug == 1) {
			CheckMenuItem(hMenu, ID_RENDER_DEBUG, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_DEBUG, MF_UNCHECKED);
		}

		if (showdebugfps == 1) {
			CheckMenuItem(hMenu, ID_RENDER_DEBUGFPS, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_DEBUGFPS, MF_UNCHECKED);
		}

		if (showtexture == 1) {
			CheckMenuItem(hMenu, ID_TEXTURES_SHOWTEXTURES, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_TEXTURES_SHOWTEXTURES, MF_UNCHECKED);
		}

		if (drawsphere == 1) {
			CheckMenuItem(hMenu, ID_RENDER_ELLIPSE, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_RENDER_ELLIPSE, MF_UNCHECKED);
		}

		if (display_scores == TRUE) {
			CheckMenuItem(hMenu, ID_ACTION_WHO, MF_CHECKED);
		} else {
			CheckMenuItem(hMenu, ID_ACTION_WHO, MF_UNCHECKED);
		}

		if (perspectiveview == 1) {
			CheckMenuItem(hMenu, MENU_FIRSTPERSON, MF_CHECKED);
			CheckMenuItem(hMenu, MENU_THIRDPERSON, MF_UNCHECKED);
		} else {
			CheckMenuItem(hMenu, MENU_THIRDPERSON, MF_CHECKED);
			CheckMenuItem(hMenu, MENU_FIRSTPERSON, MF_UNCHECKED);
		}

		if (lighttype == 1) {
			CheckMenuItem(hMenu, MENU_FLASHLIGHT, MF_CHECKED);
			CheckMenuItem(hMenu, MENU_LIGHTBULB, MF_UNCHECKED);
		}
		if (lighttype == 2) {
			CheckMenuItem(hMenu, MENU_FLASHLIGHT, MF_UNCHECKED);
			CheckMenuItem(hMenu, MENU_LIGHTBULB, MF_CHECKED);
		}

		if (lighttype == 0) {

			CheckMenuItem(hMenu, MENU_LIGHTBULB, MF_UNCHECKED);
			CheckMenuItem(hMenu, MENU_FLASHLIGHT, MF_UNCHECKED);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case MENU_ABOUT:
			Pause(TRUE);
			DialogBox(hInstApp, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)AppAbout);
			Pause(FALSE);
			break;

		case ID_MULTIPLAYER_DISCONNECT:
			PrintMessage(NULL, "MsgProc - ID_MULTIPLAYER_DISCONNECT", NULL, LOGFILE_ONLY);

			break;

		case ID_MULTIPLAYER_PLAYONLINENOW:
			/*
			if(multiplay_flag == TRUE)
			    break;
			forcemainserver=1;
			// See if we were launched from a lobby server
			hr = DPConnect_CheckForLobbyLaunch( &bLaunchedByLobby );
			if( FAILED(hr) )
			{
			    if( hr == DPERR_USERCANCEL )
			        return S_OK;
			    return hr;
			}

			if( !bLaunchedByLobby )
			{
			    // If not, the first step is to prompt the user about the network
			    // connection and which session they would like to join or
			    // if they want to create a new one.
			    nExitCode = DPConnect_StartDirectPlayConnect( hInstApp, FALSE );

			    // See the above EXITCODE #defines for what nExitCode could be
			    if( nExitCode == EXITCODE_QUIT )
			    {
			        // The user canceled the multiplayer connect.
			        // The sample will now quit.
			        return E_ABORT;
			    }

			    if( nExitCode == EXITCODE_ERROR || g_pDP == NULL )
			    {
			        MessageBox( NULL, "Multiplayer connect failed. ",
			                      "You may need to reboot",
			                      MB_OK | MB_ICONERROR );

			        return E_FAIL;
			    }
			}
			*/
			DialogBox(NULL, (LPCTSTR)IDD_PLAYONLINE, main_window_handle,
			          (DLGPROC)DlgPlayOnline);

			break;

		case ID_MULTIPLAYER_CONNECT:

			break;
			//
			// Possible modes
			// --------------
			// Vertex Fog (fog type)
			//		Linear (fog mode)
			//			(Can set to Range Based)
			//			Start
			//			End
			//
			// Pixel (Table) Fog (fog type)
			//		Fog Mode - Linear
			//				Start
			//				End
			//		Fog Mode - Exponential
			//				Density (0.0 - 1.0)
			//

		case ID_FILE_REGISTER:

			DialogBox(NULL, (LPCTSTR)IDD_REG, main_window_handle,
			          (DLGPROC)DlgRegister);

			break;

		case ID_MULTIPLAYER_LISTDSSERVERS:
			DialogBox(NULL, (LPCTSTR)IDD_SERVERLIST, main_window_handle,
			          (DLGPROC)DlgListServers);

			break;

		case MENU_FIRSTPERSON:
			look_up_ang = 0.0f;
			m_vEyePt = m_vLookatPt;
			perspectiveview = 1;
			break;

		case MENU_THIRDPERSON:
			look_up_ang = 35.0f;
			m_vLookatPt = m_vEyePt;
			perspectiveview = 0;
			break;

		case ID_ACTION_PAIN:
			SetPlayerAnimationSequence(trueplayernum, 5);
			break;

		case ID_ACTION_JUMP:
			SetPlayerAnimationSequence(trueplayernum, 6);
			break;

		case ID_ACTION_FLIP:
			SetPlayerAnimationSequence(trueplayernum, 7);
			break;

		case ID_ACTION_SALUTE:
			SetPlayerAnimationSequence(trueplayernum, 8);
			break;

		case ID_ACTION_TAUNT:
			SetPlayerAnimationSequence(trueplayernum, 9);
			break;

		case ID_ACTION_WAVE:
			SetPlayerAnimationSequence(trueplayernum, 10);
			break;

		case ID_ACTION_POINT:
			SetPlayerAnimationSequence(trueplayernum, 11);
			break;

		case ID_ACTION_DEATH:
			// SetPlayerAnimationSequence(trueplayernum, 12);
			break;

		case ID_ACTION_MODEL:

			saveangy = angy;
			saveplocation = m_vLookatPt;
			saveplocation = modellocation;
			m_vEyePt.x = 700;
			m_vEyePt.y = 160;
			m_vEyePt.z = 700;

			m_vLookatPt.x = 700;
			m_vLookatPt.y = 160;
			m_vLookatPt.z = 700;
			perspectiveview = 0;

			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x00f0f0f0);

			openingscreen = 4;

			break;

		case ID_TEXTURES_ANTIALIAS:
			if (antialias == 0) {
				antialias = 1;
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, 2);
			} else {
				antialias = 0;
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, 0);
			}

			break;

		case ID_LIGHTING_FLARES:
			if (menuflares == 0) {
				D3DMATERIAL7 mtrl;
				menuflares = 1;
				D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);

				mtrl.dcvSpecular.r = 0.3f;
				mtrl.dcvSpecular.g = 0.3f;
				mtrl.dcvSpecular.b = 0.3f;
				mtrl.dcvSpecular.a = 0.0f;

				mtrl.power = 40.0f;
				m_pd3dDevice->SetMaterial(&mtrl);
				//		lastmaterial=texture_number;
			} else {
				D3DMATERIAL7 mtrl;
				menuflares = 0;
				D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);

				mtrl.dcvSpecular.r = 0.3f;
				mtrl.dcvSpecular.g = 0.3f;
				mtrl.dcvSpecular.b = 0.3f;
				mtrl.dcvSpecular.a = 0.0f;

				mtrl.power = 40.0f;
				m_pd3dDevice->SetMaterial(&mtrl);
			}
			break;

		case ID_LIGHTING_SPECULAR:
			if (specularon == 0) {
				specularon = 1;
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);
			} else {
				specularon = 0;
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
			}
			break;

		case ID_ACTION_SHOWCAPTIONS:
			if (displaycap == 0) {
				displaycap = 1;
			} else {
				displaycap = 0;
			}
			break;

		case ID_MONSTERS_SHOWBODIES:
			if (showmonstermenu == 0) {
				showmonstermenu = 1;
			} else {
				showmonstermenu = 0;
			}
			break;

		case ID_ACTION_SHOWCHAT:
			if (displaychat == 0) {
				displaychat = 1;
			} else {
				displaychat = 0;
			}
			break;

		case ID_RENDER_PERFTIMER:

			if (highperftimer == 0) {

				highperftimer = 1;
				initDSTimer();
			} else {

				LONGLONG count = (LONGLONG)timeGetTime();

				// clear timer selection flag

				perf_flag = FALSE;

				// set timer scaling factor
				time_factor = (float)0.001;

				gametimerlast = count;
				gametimerlastpost = count;
				gametimerlast2 = count;

				elapsegametimer = count;
				elapsegametimersave = 0;
				elapsegametimerlast = count;

				gametimerlast = count;
				gametimerlastpost = count;
				gametimerlast2 = count;
				gametimerlastdoor = count;

				highperftimer = 0;
			}
			break;

		case ID_RENDER_MOUSEFILTER:

			if (mousefilter == 0) {

				mousefilter = 1;
			} else {

				mousefilter = 0;
			}
			break;

		case ID_RENDER_CROSSHAIR:

			if (crosshairenabled == 0) {

				crosshairenabled = 1;
			} else {

				crosshairenabled = 0;
			}
			break;

		case ID_MONSTERS_ENABLED:

			if (monsterenable == 0) {

				monsterenable = 1;
			} else {
				monsterenable = 0;
			}
			break;

		case ID_RENDER_PANEL:

			if (showpanel == 0) {

				showpanel = 1;
			} else {
				showpanel = 0;
			}
			break;

		case ID_TEXTURES_POINTFILTER:
			// D3DTFG_POINT        = 1
			// D3DTFG_LINEAR       = 2
			// D3DTFG_FLATCUBIC    = 3
			// D3DTFG_GAUSSIANCUBIC= 4
			// D3DTFG_ANISOTROPIC  = 5

			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);

			filtertype = 0;
			break;
		case ID_TEXTURES_LINEARFILTER:
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

			filtertype = 1;
			break;
		case ID_TEXTURES_ANISOTROPICFILTER:
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_ANISOTROPIC);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_ANISOTROPIC);

			filtertype = 2;
			break;

		case ID_LOCK_FRAMERATE:

			if (lockframerate == 0) {

				lockframerate = 1;
			} else {
				lockframerate = 0;
			}
			break;
		case ID_RENDER_DEBUGFPS:

			if (showdebugfps == 0) {
				showdebugfps = 1;
			} else {
				showdebugfps = 0;
			}
			break;

		case ID_RENDER_DEBUG:

			if (showdebug == 0) {
				showdebug = 1;
			} else {
				showdebug = 0;
			}
			break;
		case ID_LIGHTING_MUSIC:
			if (musicon == 0) {
				musicon = 1;
			} else {
				musicon = 0;
			}
			break;
		case ID_RENDER_ZBUFFER:

			if (zbufferenable == 0) {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
				zbufferenable = 1;
			} else {
				zbufferenable = 0;
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
			}
			break;

		case ID_RENDER_SORTPOLY:
			if (polysorton == 0) {
				polysorton = 1;
			} else {
				polysorton = 0;
			}
			break;

		case ID_RENDER_SOLID:
			if (solid == 1) {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
				solid = 0;
			} else {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
				solid = 1;
				wireframe = 0;
				point = 0;
			}
			break;
		case ID_RENDER_COLLISION:
			if (collisioncode == 0)
				collisioncode = 1;
			else
				collisioncode = 0;

			break;

		case ID_RENDER_WIREFRAME:
			if (wireframe == 1) {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
				wireframe = 0;
			} else {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
				solid = 0;
				wireframe = 1;
				point = 0;
			}
			break;

		case ID_RENDER_POINT:
			if (point == 1) {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
				point = 0;
			} else {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_POINT);
				solid = 0;
				wireframe = 0;
				point = 1;
			}
			break;

		case ID_LIGHTING_AMBIENTLIGHT:
			if (ambientlighton == 1) {
				ambientlighton = 0;
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, NULL);
			} else {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x00f0f0f0);
				ambientlighton = 1;
			}
			break;

		case ID_MONSTERS_MOVE:
			if (monstermoveon == 1) {
				monstermoveon = 0;
			} else {
				monstermoveon = 1;
			}
			break;

		case ID_ACTION_WHO:
			if (display_scores == FALSE) {
				display_scores = TRUE;
			} else {
				display_scores = FALSE;
			}
			break;

		case ID_RENDER_ELLIPSE:
			if (drawsphere) {
				drawsphere = 0;
			} else {
				drawsphere = 1;
			}
			break;

		case ID_RENDER_SHOWNORMALS:
			if (normalon == 0) {
				normalon = 1;
			} else {
				normalon = 0;
			}
			break;

		case ID_TEXTURES_SHOWTEXTURES:
			if (showtexture == 0) {
				showtexture = 1;
			} else {
				showtexture = 0;
			}
			break;

		case MENU_FOG_OFF:
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
			m_toggleFog = FALSE;
			m_tableFog = FALSE;
			break;

		case MENU_FLASHLIGHT:

			if (lighttype == 0) {
				lighttype = 1;
				bIsFlashlightOn = TRUE;
			} else {
				lighttype = 0;
				bIsFlashlightOn = FALSE;
			}

			break;
		case MENU_LIGHTBULB:

			if (lighttype == 0) {
				bIsFlashlightOn = TRUE;
				lighttype = 2;
			} else {
				bIsFlashlightOn = FALSE;
				lighttype = 0;
			}
			break;

		case MENU_VERTEXFOG:
			// Set fog renderstates
			fStart = 40.0;
			fEnd = 500.0;
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
			m_pd3dDevice->SetRenderState(
			    D3DRENDERSTATE_FOGCOLOR,
			    RGB_MAKE(0, 0, 80)); // Highest 8 bits aren't used.

			// Set fog parameters.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_LINEAR);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fStart));
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD *)(&fEnd));
			m_tableFog = FALSE;
			break;

		case MENU_VERTEXFOG_RANGE:
			// Set fog renderstates
			fStart = 40.0;
			fEnd = 500.0;
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
			m_pd3dDevice->SetRenderState(
			    D3DRENDERSTATE_FOGCOLOR,
			    RGB_MAKE(0, 0, 80)); // Highest 8 bits aren't used.

			// Set fog parameters.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_LINEAR);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fStart));
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD *)(&fEnd));

			// Enable range-based fog if desired (only supported for vertex fog).
			// For this example, it is assumed that fUseRange is set to a nonzero value
			// only if the driver exposes the D3DPRASTERCAPS_FOGRANGE capability.
			//
			// Note: this is slightly more performance intensive
			//       than non-range-based fog.
			m_pd3dDevice->SetRenderState(
			    D3DRENDERSTATE_RANGEFOGENABLE,
			    TRUE);
			m_tableFog = FALSE;
			break;

		case MENU_TABLEFOGLIN: // Pixel fog - linear
			m_tableFog = TRUE;
			fStart = (float)200.0;
			fEnd = 800.0;

			// Enable fog blending.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);

			// Set the fog color.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0, 0, 10));

			// Set fog parameters.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_NONE);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fStart));
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD *)(&fEnd));
			break;

		case MENU_TABLEFOGEXP: // Pixel fog - exponential
			m_tableFog = TRUE;
			fDensity = 0.005f; // for exponential modes

			// Enable fog blending.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);

			// Set the fog color.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0, 0, 80));

			// Set fog parameters.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_NONE);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGDENSITY, *(DWORD *)(&fDensity));
			break;

		case MENU_TABLEFOGEXP2: // Pixel fog - exponential
			m_tableFog = TRUE;
			fDensity = 0.005f; // for exponential modes

			// Enable fog blending.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);

			// Set the fog color.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0, 0, 80));

			// Set fog parameters.
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_NONE);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP2);
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGDENSITY, *(DWORD *)(&fDensity));
			break;

		case IDM_SELECTINPUTDEVICE:
			DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			          MAKEINTRESOURCE(IDD_SELECTINPUTDEVICE), hWnd,
			          (DLGPROC)InputDeviceSelectProc);
			break;

		case ID_ACTION_CAPTION:
			DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			          MAKEINTRESOURCE(IDD_CAPTION), hWnd,
			          (DLGPROC)CaptionProc);
			break;

			// Shading selection
		case MENU_FLAT_SHADING:
			CheckMenuItem(hMenu, MENU_FLAT, MF_CHECKED);
			CheckMenuItem(hMenu, MENU_GOURAUD, MF_UNCHECKED);
			ShadeMode = D3DSHADE_FLAT;
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE,
			                             D3DSHADE_FLAT);
			break;

		case MENU_GOURAUD_SHADING:
			CheckMenuItem(hMenu, MENU_FLAT, MF_UNCHECKED);
			CheckMenuItem(hMenu, MENU_GOURAUD, MF_CHECKED);
			ShadeMode = D3DSHADE_GOURAUD;
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE,
			                             D3DSHADE_GOURAUD);
			break;

		case MENU_LIGHT_SOURCES:
			bEnableLighting = !bEnableLighting;
			CheckMenuItem(hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);
			break;

		case ID_TEXTURES_ALPHATRANSPARENCY:
			bEnableAlphaTransparency = !bEnableAlphaTransparency;
			CheckMenuItem(hMenu, ID_TEXTURES_ALPHATRANSPARENCY, bEnableAlphaTransparency ? MF_CHECKED : MF_UNCHECKED);
			break;

		case IDM_EXIT:

			if (openingscreen == 0) {
				Pause(TRUE);
				UINT resultclick = MessageBox(main_window_handle, "Quit Game?", "Quit Game", MB_ICONQUESTION | MB_YESNO);
				Pause(FALSE);
				if (resultclick == IDYES) {
					PrintMsgY = 10;

					perspectiveview = 0;
					look_up_ang = 0.0f;

					ResetMainPlayer();
					openingscreen = 1;
					saveplocation = m_vLookatPt;
					m_vEyePt.x = 700;
					m_vEyePt.y = 160;
					m_vEyePt.z = 700;

					m_vLookatPt.x = 700;
					m_vLookatPt.y = 160;
					m_vLookatPt.z = 700;
					perspectiveview = 0;
					//	FadeOut();
				} else {
				}
			} else if (openingscreen == 1) {
				ExitGame();
			}
			break;

		default:
			return CD3DApplication::MsgProc(hWnd, uMsg, wParam,
			                                lParam);
		}
		break;

	case WM_GETMINMAXINFO:
		((MINMAXINFO *)lParam)->ptMinTrackSize.x = 100;
		((MINMAXINFO *)lParam)->ptMinTrackSize.y = 100;
		break;

	case WM_CLOSE:

		DSPostQuit();
		PrintMessage(NULL, "MsgProc - WM_CLOSE", NULL, LOGFILE_ONLY);

		// stop all sounds
		DSound_Stop_All_Sounds();

		// delete all sounds
		DSound_Delete_All_Sounds();

		// shutdown directsound
		DSound_Shutdown();

		// delete all the music
		DMusic_Delete_All_MIDI();

		// shutdown directmusic
		DMusic_Shutdown();

		Cleanup3DEnvironment();

		D3DTextr_InvalidateAllTextures();

		DestroyWindow(hWnd);
		PostQuitMessage(0);
		return 0;

	default:
		return CD3DApplication::MsgProc(hWnd, uMsg, wParam,
		                                lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void CMyD3DApplication::AddPlayerLightSource(int player_num, float x, float y, float z) {
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	D3DLIGHT7 light;

	if (bEnableLighting == FALSE)
		return;

	ZeroMemory(&light, sizeof(D3DLIGHT7));

	light.dcvDiffuse.r = 0.0f;
	light.dcvDiffuse.g = 0.0f;
	light.dcvDiffuse.b = 0.0f;

	weapon_flash_r -= 0.05f;
	weapon_flash_g = weapon_flash_r / 2.0f;

	light.dcvAmbient.r = weapon_flash_r; // 0.4f;
	light.dcvAmbient.g = weapon_flash_g; // 0.2f;
	light.dcvAmbient.b = 0.0f;

	if (weapon_flash_r < 0) {
		weapon_flash_r = 0.4f;
		weapon_flash_r = 0.2f;
		player_list[player_num].bIsFiring = FALSE;
	}
	light.dcvSpecular.r = 1.0f;
	light.dcvSpecular.g = 1.0f;
	light.dcvSpecular.b = 1.0f;

	light.dvRange = 200.0f;

	pos_x = x;
	pos_y = -100;
	pos_z = z;

	dir_x = 0;
	dir_y = 100;
	dir_z = 0;

	light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);
	light.dvDirection = D3DVECTOR(dir_x, dir_y, dir_z);
	light.dvFalloff = 100.0f;

	// angle dvPhi   must be within the range 0 to pi	 radians
	// angle dvTheta must be within the range 0 to dvPhi radians

	light.dvTheta = 1.4f; // spotlight's inner cone
	light.dvPhi = 2.1f;   // spotlight's outer cone
	light.dvAttenuation0 = 1.0f;
	light.dltType = D3DLIGHT_SPOT;

	// Set the light

	m_pd3dDevice->SetLight(num_light_sources, &light);
	m_pd3dDevice->LightEnable((DWORD)num_light_sources, TRUE);
	num_light_sources++;
}

extern float gametimerAnimation;
const int MAX_VERTS_PER_POLY = 256;
const int MAX_TRIANGLE_LIST_VERTS = MAX_VERTS_PER_POLY * 3;

struct SimpleVertex {
	D3DVECTOR position;
	float tu;
	float tv;
};

SimpleVertex originalVerts[MAX_VERTS_PER_POLY];
SimpleVertex triangleListVerts[MAX_TRIANGLE_LIST_VERTS];
D3DVECTOR triangleListNormals[MAX_TRIANGLE_LIST_VERTS];


void CMyD3DApplication::PlayerToD3DVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag, int nextFrame) {

	float qdist = 0;

	int i, j;
	int num_verts_per_poly;
	int num_poly;
	int i_count;
	short v_index;
	float rx, ry, rz;
	float tx, ty;

	vert_ptr tp;
	DWORD r, g, b;
	D3DPRIMITIVETYPE p_command;
	const int srcVertStart = cnt;

	if (angle >= 360)
		angle = angle - 360;
	if (angle < 0)
		angle += 360;

	x_off = 0;
	y_off = 0;
	z_off = 0;

	if (pmdata[pmodel_id].use_indexed_primitive == TRUE) {
		PlayerToD3DIndexedVertList(pmodel_id, 0, angle, texture_alias, tex_flag);
		return;
	}

	cosine = cos_table[angle];
	sine = sin_table[angle];

	if (curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	i_count = 0;

	num_poly = pmdata[pmodel_id].num_polys_per_frame;

	for (i = 0; i < num_poly; i++) {
		p_command = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_vert[i];

		psort[number_of_polys_per_frame].srcstart = cnt;

		int originalVertexCount = 0;

		for (j = 0; j < num_verts_per_poly; j++) {
			v_index = pmdata[pmodel_id].f[i_count];

			tp = &pmdata[pmodel_id].w[curr_frame][v_index];

			float x, y, z;
			if (nextFrame != -1) {
				const vert_ptr tpNextFrame = &pmdata[pmodel_id].w[nextFrame][v_index];
				const float t = (gametimerAnimation > 0.0f && gametimerAnimation < 1.0f) ? gametimerAnimation : 0.0f;

				if (t > 0.0f) {
					x = tp->x + t * (tpNextFrame->x - tp->x);
					z = tp->y + t * (tpNextFrame->y - tp->y);
					y = tp->z + t * (tpNextFrame->z - tp->z);
				} else {
					x = tp->x;
					z = tp->y;
					y = tp->z;
				}
			} else {
				x = tp->x;
				z = tp->y;
				y = tp->z;
			}

			if (weapondrop == 1) {
				y -= 40.0f;
			}

			if (weapondrop == 0) {
				rx = wx + (x * cosine - z * sine);
				ry = wy + y;
				rz = wz + (x * sine + z * cosine);
			} else {
				rx = wx + x * sinf(fDot2 * k) * sinf(angle * k);
				ry = wy + y * cosf(fDot2 * k);
				rz = wz + (z)*sinf(fDot2 * k) * cosf(angle * k);
			}

			if (weapondrop == 1) {
				float newx = x;
				float newy = y;
				float newz = z;

				rx = (newy * sinf(fDot2 * k) + newx * cosf(fDot2 * k));
				ry = (newy * cosf(fDot2 * k) - newx * sinf(fDot2 * k));
				rz = newz;

				newx = rx;
				newy = ry;
				newz = rz;

				rx = (newx * cosine - newz * sine);
				ry = newy;
				rz = (newx * sine + newz * cosine);

				newx = rx;
				newy = ry;
				newz = rz;
				rx = newx;
				ry = (newy * cosf(0.0f * k) - newz * sinf(0.0f * k));
				rz = (newy * sinf(0.0f * k) + newz * cosf(0.0f * k));
			} else {
				rx = (x * cosine - z * sine);
				ry = y;
				rz = (x * sine + z * cosine);
			}

			rx = rx + wx;
			ry = ry + wy;

			rz = rz + wz;

			tx = pmdata[pmodel_id].t[i_count].x * pmdata[pmodel_id].skx;
			ty = pmdata[pmodel_id].t[i_count].y * pmdata[pmodel_id].sky;

			r = 0;
			g = 0;
			b = 0;

			originalVerts[originalVertexCount].position = D3DVECTOR(D3DVAL(rx), D3DVAL(ry), D3DVAL(rz));
			originalVerts[originalVertexCount].tu = D3DVAL(tx);
			originalVerts[originalVertexCount].tv = D3DVAL(ty);
			originalVertexCount++;
			i_count++;
		}

		int triangleVertexCount = 0;

		if (p_command == D3DPT_TRIANGLELIST) {
			for (int v = 0; v < originalVertexCount; v++) {
				triangleListVerts[triangleVertexCount++] = originalVerts[v];
			}
		} else if (p_command == D3DPT_TRIANGLEFAN) {
			for (int v = 1; v < originalVertexCount - 1; v++) {
				triangleListVerts[triangleVertexCount++] = originalVerts[0];
				triangleListVerts[triangleVertexCount++] = originalVerts[v];
				triangleListVerts[triangleVertexCount++] = originalVerts[v + 1];
			}
		} else if (p_command == D3DPT_TRIANGLESTRIP) {
			for (int v = 0; v < originalVertexCount - 2; v++) {
				if ((v & 1) == 0) {
					triangleListVerts[triangleVertexCount++] = originalVerts[v];
					triangleListVerts[triangleVertexCount++] = originalVerts[v + 1];
					triangleListVerts[triangleVertexCount++] = originalVerts[v + 2];
				} else {
					triangleListVerts[triangleVertexCount++] = originalVerts[v + 1];
					triangleListVerts[triangleVertexCount++] = originalVerts[v];
					triangleListVerts[triangleVertexCount++] = originalVerts[v + 2];
				}
			}
		} else {
			for (int v = 0; v < originalVertexCount; v++) {
				triangleListVerts[triangleVertexCount++] = originalVerts[v];
			}
		}

		for (int v = 0; (v + 2) < triangleVertexCount; v += 3) {
			const D3DVECTOR &a = triangleListVerts[v].position;
			const D3DVECTOR &b = triangleListVerts[v + 1].position;
			const D3DVECTOR &c = triangleListVerts[v + 2].position;
			D3DVECTOR edge1 = b - a;
			D3DVECTOR edge2 = c - a;
			D3DVECTOR normal = CrossProduct(edge1, edge2);
			normal = Normalize(normal);
			/*		if (Magnitude(normal) < 0.1f) {
			            normal = D3DVECTOR(0.0f, 0.0f, 0.0f);
			        }*/
			triangleListNormals[v] = normal;
			triangleListNormals[v + 1] = normal;
			triangleListNormals[v + 2] = normal;
		}

		for (int v = 0; v < triangleVertexCount; v++) {
			src_v[cnt].x = triangleListVerts[v].position.x;
			src_v[cnt].y = triangleListVerts[v].position.y;
			src_v[cnt].z = triangleListVerts[v].position.z;
			src_v[cnt].tu = triangleListVerts[v].tu;
			src_v[cnt].tv = triangleListVerts[v].tv;
			src_v[cnt].nx = triangleListNormals[v].x;
			src_v[cnt].ny = triangleListNormals[v].y;
			src_v[cnt].nz = triangleListNormals[v].z;
			src_collide[cnt] = 1;
			cnt++;
		}

		float centroidx = 0.0f;
		float centroidy = 0.0f;
		float centroidz = 0.0f;
		if (triangleVertexCount >= 3) {
			centroidx = (triangleListVerts[0].position.x + triangleListVerts[1].position.x + triangleListVerts[2].position.x) * 0.3333333333333f;
			centroidy = (triangleListVerts[0].position.y + triangleListVerts[1].position.y + triangleListVerts[2].position.y) * 0.3333333333333f;
			centroidz = (triangleListVerts[0].position.z + triangleListVerts[1].position.z + triangleListVerts[2].position.z) * 0.3333333333333f;
		}

		qdist = FastDistance(
		    m_vEyePt.x - centroidx,
		    m_vEyePt.y - centroidy,
		    m_vEyePt.z - centroidz);

		psort[number_of_polys_per_frame].vert_index = number_of_polys_per_frame;
		psort[number_of_polys_per_frame].dist = qdist;
		psort[number_of_polys_per_frame].texture = texture_alias;
		psort[number_of_polys_per_frame].vertsperpoly = triangleVertexCount;

		verts_per_poly[number_of_polys_per_frame] = triangleVertexCount;
		dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLELIST;

		if (triangleVertexCount >= 3)
			num_triangles_in_scene += triangleVertexCount / 3;

		num_verts_in_scene += triangleVertexCount;
		num_dp_commands_in_scene++;

		if (tex_flag == USE_PLAYERS_SKIN)
			texture_list_buffer[number_of_polys_per_frame] = texture_alias;
		else
			texture_list_buffer[number_of_polys_per_frame] = pmdata[pmodel_id].texture_list[i];

		texture_list_buffer[number_of_polys_per_frame] = texture_alias;

		number_of_polys_per_frame++;

	} // end for vert_cnt

	const int newVertCount = cnt - srcVertStart;
	if (newVertCount > 0) {
		std::vector<D3DVECTOR> smoothedNormals(newVertCount);
		const float normalShareEpsilon = 0.0005f;
		for (int i = 0; i < newVertCount; ++i) {
			const int vertIndex = srcVertStart + i;
			float nx = src_v[vertIndex].nx;
			float ny = src_v[vertIndex].ny;
			float nz = src_v[vertIndex].nz;
			const float vx = src_v[vertIndex].x;
			const float vy = src_v[vertIndex].y;
			const float vz = src_v[vertIndex].z;
			for (int j = 0; j < newVertCount; ++j) {
				if (i == j)
					continue;
				const int otherIndex = srcVertStart + j;
				if (fabsf(vx - src_v[otherIndex].x) < normalShareEpsilon &&
				    fabsf(vy - src_v[otherIndex].y) < normalShareEpsilon &&
				    fabsf(vz - src_v[otherIndex].z) < normalShareEpsilon) {
					nx += src_v[otherIndex].nx;
					ny += src_v[otherIndex].ny;
					nz += src_v[otherIndex].nz;
				}
			}
			float lenSq = nx * nx + ny * ny + nz * nz;
			if (lenSq > 0.000001f) {
				float invLen = 1.0f / sqrtf(lenSq);
				nx *= invLen;
				ny *= invLen;
				nz *= invLen;
			} else {
				nx = 0.0f;
				ny = 0.0f;
				nz = 0.0f;
			}
			smoothedNormals[i] = D3DVECTOR(nx, ny, nz);
		}
		for (int i = 0; i < newVertCount; ++i) {
			const int vertIndex = srcVertStart + i;
			src_v[vertIndex].nx = smoothedNormals[i].x;
			src_v[vertIndex].ny = smoothedNormals[i].y;
			src_v[vertIndex].nz = smoothedNormals[i].z;
		}
	}

	return;
}


void CMyD3DApplication::PlayerToD3DIndexedVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag) {

	float qdist = 0;
	int i, j;
	int num_verts_per_poly;
	int num_faces_per_poly;
	int num_poly;
	int poly_command;
	int i_count, face_i_count;
	float x, y, z;
	float rx, ry, rz;
	float tx, ty;
	int count_v = 0;
	FILE *fp;
	float mx[6000];
	float my[6000];
	float mz[6000];

	D3DVECTOR v1, v2, vw1, vw2, vw3, vw4, vDiff2, vDiff3, vw5;
	const int srcVertStart = cnt;

	x_off = 0;
	y_off = 0;
	z_off = 0;

	if (curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	curr_frame = 0;
	cosine = cos_table[angle];
	sine = sin_table[angle];

	i_count = 0;
	face_i_count = 0;

	if (rendering_first_frame == TRUE) {
		if (fopen_s(&fp, "ds.log", "a") != 0) {
		}
	}

	// process and transfer the model data from the pmdata structure
	// to the array of D3DVERTEX structures, src_v

	num_poly = pmdata[pmodel_id].num_polys_per_frame;

	psort[number_of_polys_per_frame].srcstart = cnt;

	for (i = 0; i < num_poly; i++) {
		poly_command = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_verts_per_object[i];
		num_faces_per_poly = pmdata[pmodel_id].num_faces_per_object[i];
		count_v = 0;

		psort[number_of_polys_per_frame].srcstart = cnt;

		for (j = 0; j < num_verts_per_poly; j++) {

			x = pmdata[pmodel_id].w[curr_frame][i_count].x + x_off;
			z = pmdata[pmodel_id].w[curr_frame][i_count].y + y_off;
			y = pmdata[pmodel_id].w[curr_frame][i_count].z + z_off;

			rx = wx + (x * cosine - z * sine);
			ry = wy + y;
			rz = wz + (x * sine + z * cosine);

			if (fDot2 != 0.0f) {
				float newx, newy, newz;

				newx = x;
				newy = y;
				newz = z;

				rx = (newy * sinf(fDot2 * k) + newx * cosf(fDot2 * k));
				ry = (newy * cosf(fDot2 * k) - newx * sinf(fDot2 * k));
				rz = newz;

				newx = rx;
				newy = ry;
				newz = rz;

				rx = (newx * cosine - newz * sine);
				ry = newy;
				rz = (newx * sine + newz * cosine);

				newx = rx;
				newy = ry;
				newz = rz;
				rx = newx;
				ry = (newy * cosf(0.0f * k) - newz * sinf(0.0f * k));
				rz = (newy * sinf(0.0f * k) + newz * cosf(0.0f * k));

				D3DMATRIX matW;
				D3DMATRIX matPoint;
				D3DMATRIX matRotateX;
				D3DMATRIX matRotateY;
				D3DMATRIX matRotateZ;
			} else {

				rx = (x * cosine - z * sine);
				ry = y;
				rz = (x * sine + z * cosine);
			}

			rx = rx + wx;
			ry = ry + wy;
			rz = rz + wz;

			tx = pmdata[pmodel_id].t[i_count].x * pmdata[pmodel_id].skx;
			ty = pmdata[pmodel_id].t[i_count].y * pmdata[pmodel_id].sky;
			ty = 1.0f - ty;

			src_v[cnt].x = D3DVAL(rx);
			src_v[cnt].y = D3DVAL(ry);
			src_v[cnt].z = D3DVAL(rz);
			src_v[cnt].tu = D3DVAL(tx);
			src_v[cnt].tv = D3DVAL(ty);

			src_collide[cnt] = 1;

			if (rendering_first_frame == TRUE) {
				fprintf(fp, "%f %f %f, ",
				        src_v[cnt].x,
				        src_v[cnt].y,
				        src_v[cnt].z);
			}
			mx[j] = rx;
			my[j] = ry;
			mz[j] = rz;

			src_v[cnt].nx = 1.0f;
			src_v[cnt].ny = 1.0f;
			src_v[cnt].nz = 0.3f;

			cnt++;
			i_count++;

		} // end for j
		psort[number_of_polys_per_frame].srcfstart = cnt_f;
		//			face_i_count = 0;
		for (j = 0; j < num_faces_per_poly * 3; j++) {
			src_f[cnt_f] = pmdata[pmodel_id].f[face_i_count];

			// if (rendering_first_frame == TRUE)
			//{
			// fprintf( fp, "%d ", src_f[cnt_f] );
			// }

			cnt_f++;
			face_i_count++;
		}

		float centroidx = (mx[0] + mx[1] + mx[2]) * 0.3333333333333f;
		float centroidy = (my[0] + my[1] + my[2]) * 0.3333333333333f;
		float centroidz = (mz[0] + mz[1] + mz[2]) * 0.3333333333333f;

		qdist = FastDistance(
		    m_vEyePt.x - centroidx,
		    m_vEyePt.y - centroidy,
		    m_vEyePt.z - centroidz);

		psort[number_of_polys_per_frame].vert_index = number_of_polys_per_frame;
		psort[number_of_polys_per_frame].texture = texture_alias;

		psort[number_of_polys_per_frame].dist = qdist;
		psort[number_of_polys_per_frame].vertsperpoly = num_verts_per_poly;
		psort[number_of_polys_per_frame].facesperpoly = num_faces_per_poly;

		verts_per_poly[number_of_polys_per_frame] = num_verts_per_poly;
		faces_per_poly[number_of_polys_per_frame] = num_faces_per_poly;

		dp_command_index_mode[number_of_polys_per_frame] = USE_INDEXED_DP;
		dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLELIST;

		num_triangles_in_scene += num_faces_per_poly;
		num_verts_in_scene += num_verts_per_poly;
		num_dp_commands_in_scene++;

		if (tex_flag == USE_PLAYERS_SKIN)
			texture_list_buffer[number_of_polys_per_frame] = texture_alias;
		else
			texture_list_buffer[number_of_polys_per_frame] = pmdata[pmodel_id].texture_list[i];

		number_of_polys_per_frame++;

	} // end for vert_cnt

	/*const int newVertCount = cnt - srcVertStart;
	if (newVertCount > 0) {
		std::vector<D3DVECTOR> smoothedNormals(newVertCount);
		const float normalShareEpsilon = 0.0005f;
		for (int i = 0; i < newVertCount; ++i) {
			const int vertIndex = srcVertStart + i;
			float nx = src_v[vertIndex].nx;
			float ny = src_v[vertIndex].ny;
			float nz = src_v[vertIndex].nz;
			const float vx = src_v[vertIndex].x;
			const float vy = src_v[vertIndex].y;
			const float vz = src_v[vertIndex].z;
			for (int j = 0; j < newVertCount; ++j) {
				if (i == j)
					continue;
				const int otherIndex = srcVertStart + j;
				if (fabsf(vx - src_v[otherIndex].x) < normalShareEpsilon &&
				    fabsf(vy - src_v[otherIndex].y) < normalShareEpsilon &&
				    fabsf(vz - src_v[otherIndex].z) < normalShareEpsilon) {
					nx += src_v[otherIndex].nx;
					ny += src_v[otherIndex].ny;
					nz += src_v[otherIndex].nz;
				}
			}
			float lenSq = nx * nx + ny * ny + nz * nz;
			if (lenSq > 0.000001f) {
				float invLen = 1.0f / sqrtf(lenSq);
				nx *= invLen;
				ny *= invLen;
				nz *= invLen;
			} else {
				nx = 0.0f;
				ny = 0.0f;
				nz = 0.0f;
			}
			smoothedNormals[i] = D3DVECTOR(nx, ny, nz);
		}
		for (int i = 0; i < newVertCount; ++i) {
			const int vertIndex = srcVertStart + i;
			src_v[vertIndex].nx = smoothedNormals[i].x;
			src_v[vertIndex].ny = smoothedNormals[i].y;
			src_v[vertIndex].nz = smoothedNormals[i].z;
		}
	}*/

	if (rendering_first_frame == TRUE) {
		fprintf(fp, " \n\n");
		fclose(fp);
	}

	return;
}

void CMyD3DApplication::ObjectToD3DVertList(int ob_type, int angle, int oblist_index) {

	int ob_vert_count = 0;
	int poly;
	float qdist = 0;
	int num_vert;
	int vert_cnt;
	int w, i;
	float x, y, z;
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	D3DPRIMITIVETYPE poly_command;
	BOOL poly_command_error = TRUE;
	D3DVECTOR v1, v2, vw1, vw2, vw3, vw4, vDiff2, vDiff3, vw5;
	float workx, worky, workz;
	int countnorm = 0;

	float zaveragedist;
	int zaveragedistcount = 0;

	x_off = 0;
	y_off = 0;
	z_off = 0;

	if (cnt > MAX_NUM_QUADS) {
		sprintf_s(gActionMessage, "Exceeded CNT...");
		UpdateScrollList(0, 245, 255);
	}

	if ((oblist[oblist_index].light_source->command != 0) &&
	    (oblist_overlite_flags[oblist_index] == FALSE) &&
	    (bEnableLighting == FALSE))

	{
		// light off do nothing
		return;
	}

	if ((oblist[oblist_index].light_source->command != 0) &&
	    (oblist_overlite_flags[oblist_index] == FALSE) &&
	    (bEnableLighting))

	{
		oblist_overlite_flags[oblist_index] = TRUE;

		// Set up the light structure
		D3DLIGHT7 light;
		ZeroMemory(&light, sizeof(D3DLIGHT7));

		light.dcvDiffuse.r = .5f;
		light.dcvDiffuse.g = .5f;
		light.dcvDiffuse.b = .5f;

		light.dcvAmbient.r = 0.3f;
		light.dcvAmbient.g = 0.4f;
		light.dcvAmbient.b = 0.6f;

		light.dcvSpecular.r = 0.2f;
		light.dcvSpecular.g = 0.2f;
		light.dcvSpecular.b = 0.2f;

		light.dvRange = 600.0f;
		light.dcvAmbient.r = oblist[oblist_index].light_source->rcolour;
		light.dcvAmbient.g = oblist[oblist_index].light_source->gcolour;
		light.dcvAmbient.b = oblist[oblist_index].light_source->bcolour;

		pos_x = oblist[oblist_index].light_source->position_x;
		pos_y = oblist[oblist_index].light_source->position_y;
		pos_z = oblist[oblist_index].light_source->position_z;

		dir_x = oblist[oblist_index].light_source->direction_x;
		dir_y = oblist[oblist_index].light_source->direction_y;
		dir_z = oblist[oblist_index].light_source->direction_z;

		switch (oblist[oblist_index].light_source->command) {

		case POINT_LIGHT_SOURCE:
			light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);
			light.dvAttenuation0 = 1.0f;
			light.dltType = D3DLIGHT_POINT;

			break;

		case DIRECTIONAL_LIGHT_SOURCE:
			light.dvDirection = D3DVECTOR(dir_x, dir_y, dir_z);
			light.dltType = D3DLIGHT_DIRECTIONAL;

			break;

		case SPOT_LIGHT_SOURCE:

			light.dltType = D3DLIGHT_SPOT;
			light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);
			light.dvDirection = D3DVECTOR(dir_x, dir_y, dir_z);
			light.dvFalloff = 400.0f;
			light.dvRange = (float)600.0f;

			light.dvTheta = 2.0f; // spotlight's inner cone
			light.dvPhi = 2.5f;   // spotlight's outer cone

			light.dvTheta = 1.4f; // spotlight's inner cone
			light.dvPhi = 2.1f;   // spotlight's outer cone

			light.dvAttenuation0 = 2.0f;

			light.dcvAmbient.r = 0.5f;
			light.dcvAmbient.g = 0.5f;
			light.dcvAmbient.b = 0.5f;

			light.dcvDiffuse.r = oblist[oblist_index].light_source->rcolour;
			light.dcvDiffuse.g = oblist[oblist_index].light_source->gcolour;
			light.dcvDiffuse.b = oblist[oblist_index].light_source->bcolour;
			break;

		case 900:
			light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);
			light.dltType = D3DLIGHT_POINT;

			light.dvAttenuation0 = 1.0f;
			light.dvRange = 150.f;
			light.dcvAmbient.r = 0.5f;
			light.dcvAmbient.g = 0.5f;
			light.dcvAmbient.b = 0.5f;

			light.dcvDiffuse.r = 1.0f;
			light.dcvDiffuse.g = 1.0f;
			light.dcvDiffuse.b = 1.0f;

			// light.dcvDiffuse.r = 1.0f;
			// light.dcvDiffuse.g = 0.2f;
			// light.dcvDiffuse.b = 0.3f;

			/*
			if (maingameloop)
			{

			    int sizeofarea = 50.0f;

			    int skip = random_num(2);
			    if (skip == 1)
			    {

			        int flip = random_num(6);
			        if (flip == 1)
			        {
			            if (oblist[oblist_index].light_source->flickerrangedir == 0)
			                oblist[oblist_index].light_source->flickerrangedir = 1;
			            else
			                oblist[oblist_index].light_source->flickerrangedir = 0;
			        }

			        if (oblist[oblist_index].light_source->flickerrangedir == 0)
			        {

			            oblist[oblist_index].light_source->flickerrange += (4.1f + (float)random_num(4));

			            if (oblist[oblist_index].light_source->flickerrange > sizeofarea)
			            {
			                oblist[oblist_index].light_source->flickerrange = sizeofarea;
			                oblist[oblist_index].light_source->flickerrangedir = 1;
			            }
			        }
			        else
			        {

			            oblist[oblist_index].light_source->flickerrange -= (4.1f + (float)random_num(4));

			            if (oblist[oblist_index].light_source->flickerrange < 0.0f)
			            {
			                oblist[oblist_index].light_source->flickerrange = 0.0f;
			                oblist[oblist_index].light_source->flickerrangedir = 0;
			            }
			        }
			    }
			    //				}
			    skip = random_num(2);
			    if (skip == 1)
			    {

			        oblist[oblist_index].light_source->flickeratt += 0.1f;

			        if (oblist[oblist_index].light_source->flickeratt > 0.4f)
			        {
			            oblist[oblist_index].light_source->flickeratt = 0.0f;
			        }
			    }
			}

			*/

			break;
		}

		// Set the light
		m_pd3dDevice->SetLight(num_light_sources, &light);
		m_pd3dDevice->LightEnable((DWORD)num_light_sources, TRUE);
		num_light_sources++;

		return;
	}

	if (bSkipThisCell == TRUE)
		return;

	cosine = cos_table[angle];
	sine = sin_table[angle];

	ob_vert_count = 0;
	poly = num_polys_per_object[ob_type];

	for (w = 0; w < poly; w++) {
		num_vert = obdata[ob_type].num_vert[w];

		mx[0] = 0.0f;
		mx[1] = 0.0f;
		mx[2] = 0.0f;
		mx[3] = 0.0f;
		my[0] = 0.0f;
		my[1] = 0.0f;
		my[2] = 0.0f;
		my[3] = 0.0f;
		mz[0] = 0.0f;
		mz[1] = 0.0f;
		mz[2] = 0.0f;
		mz[3] = 0.0f;

		zaveragedist = 0.0f;

		if (strstr(oblist[oblist_index].name, "!") != NULL) {
			psort[number_of_polys_per_frame].texture = oblist[oblist_index].monstertexture;
			ctext = oblist[oblist_index].monstertexture;
		} else {
			psort[number_of_polys_per_frame].texture = obdata[ob_type].tex[w];
			ctext = obdata[ob_type].tex[w];
		}
		texture_list_buffer[number_of_polys_per_frame] = ctext;

		int cresult;

		cresult = CycleBitMap(ctext);

		if (cresult != -1) {
			oblist[oblist_index].monstertexture = cresult;

			D3DVECTOR collidenow;
			D3DVECTOR normroadold;
			D3DVECTOR work1, work2, vDiff;
			normroadold.x = 50;
			normroadold.y = 0;
			normroadold.z = 0;

			work1.x = m_vEyePt.x;
			work1.y = m_vEyePt.y;
			work1.z = m_vEyePt.z;

			work2.x = wx;
			work2.y = wy;
			work2.z = wz;

			vDiff = work1 - work2;
			vDiff = Normalize(vDiff);

			vw1.x = work1.x;
			vw1.y = work1.y;
			vw1.z = work1.z;

			vw2.x = work2.x;
			vw2.y = work2.y;
			vw2.z = work2.z;

			vDiff = vw1 - vw2;

			D3DVECTOR final, final2;
			D3DVECTOR m, n;

			final = Normalize(vDiff);
			final2 = Normalize(normroadold);

			float fDot = dot(final, final2);
			float convangle;
			convangle = (float)acos(fDot) / k;

			fDot = convangle;

			if (vw2.z < vw1.z) {
			} else {
				fDot = 180.0f + (180.0f - fDot);
			}

			cosine = cos_table[(int)fDot];
			sine = sin_table[(int)fDot];
		}

		for (vert_cnt = 0; vert_cnt < num_vert; vert_cnt++) {

			x = obdata[ob_type].v[ob_vert_count].x;
			y = obdata[ob_type].v[ob_vert_count].y;
			z = obdata[ob_type].v[ob_vert_count].z;

			FLOAT tu = ((fTimeKeyscroll * (float)time_factor) / 10.0f) - (FLOAT)floor((fTimeKeyscroll * (float)time_factor) / 10.0f);

			if (num_vert == 4 && objectscroll == 1) {

				if (vert_cnt == 1) {
					tx[vert_cnt] = tu;
					tx[vert_cnt - 1] = tx[vert_cnt];
				}

				if (vert_cnt == 3) {
					tx[vert_cnt] = tu - 1.0f;
					tx[vert_cnt - 1] = tx[vert_cnt];
				}

				ty[vert_cnt] = obdata[ob_type].t[ob_vert_count].y;

			} else {
				tx[vert_cnt] = obdata[ob_type].t[ob_vert_count].x;
				ty[vert_cnt] = obdata[ob_type].t[ob_vert_count].y;
			}

			mx[vert_cnt] = wx + (x * cosine - z * sine);
			my[vert_cnt] = wy + y;
			mz[vert_cnt] = wz + (x * sine + z * cosine);

			zaveragedist = zaveragedist + mz[vert_cnt];
			zaveragedistcount++;

			ob_vert_count++;
			g_ob_vert_count++;

		} // end for vert_cnt

		float centroidx = (mx[0] + mx[1] + mx[2]) * 0.3333333333333f;
		float centroidy = (my[0] + my[1] + my[2]) * 0.3333333333333f;
		;
		float centroidz = (mz[0] + mz[1] + mz[2]) * 0.3333333333333f;

		zaveragedist = zaveragedist / zaveragedistcount;

		verts_per_poly[number_of_polys_per_frame] = num_vert;

		psort[number_of_polys_per_frame].vertsperpoly = num_vert;
		psort[number_of_polys_per_frame].srcstart = cnt;

		poly_command = obdata[ob_type].poly_cmd[w];

		if (obdata[ob_type].use_texmap[w] == FALSE) {
			for (i = 0; i < verts_per_poly[number_of_polys_per_frame]; i++) {

				src_v[cnt].x = D3DVAL(mx[i]);
				src_v[cnt].y = D3DVAL(my[i]);
				src_v[cnt].z = D3DVAL(mz[i]);
				src_v[cnt].tu = D3DVAL(tx[i]);
				src_v[cnt].tv = D3DVAL(ty[i]);

				if (drawthistri == 1)
					src_on[cnt] = 1;
				else
					src_on[cnt] = 0;

				if (objectcollide == 1)
					src_collide[cnt] = 1;
				else
					src_collide[cnt] = 0;

				if (i == 0) {

					vw1.x = D3DVAL(mx[i]);
					vw1.y = D3DVAL(my[i]);
					vw1.z = D3DVAL(mz[i]);

					vw2.x = D3DVAL(mx[i + 1]);
					vw2.y = D3DVAL(my[i + 1]);
					vw2.z = D3DVAL(mz[i + 1]);

					vw3.x = D3DVAL(mx[i + 2]);
					vw3.y = D3DVAL(my[i + 2]);
					vw3.z = D3DVAL(mz[i + 2]);

					// calculate the NORMAL for the road using the CrossProduct <-important!

					D3DVECTOR vDiff = vw1 - vw2;
					vDiff2 = vw3 - vw2;

					D3DVECTOR vCross = CrossProduct(vDiff, vDiff2);

					D3DVECTOR final = Normalize(vCross);

					workx = -final.x;
					worky = -final.y;
					workz = -final.z;
				}

				src_v[cnt].nx = workx;
				src_v[cnt].ny = worky;
				src_v[cnt].nz = workz;

				cnt++;
			}
		} else {
			for (i = 0; i < verts_per_poly[number_of_polys_per_frame]; i++) {
				src_v[cnt].x = D3DVAL(mx[i]);
				src_v[cnt].y = D3DVAL(my[i]);
				src_v[cnt].z = D3DVAL(mz[i]);
				src_v[cnt].tu = D3DVAL(TexMap[ctext].tu[i]);
				src_v[cnt].tv = D3DVAL(TexMap[ctext].tv[i]);
				if (drawthistri == 1)
					src_on[cnt] = 1;
				else
					src_on[cnt] = 0;

				if (objectcollide == 1)
					src_collide[cnt] = 1;
				else
					src_collide[cnt] = 0;

				if (i == 0) {
					vw1.x = D3DVAL(mx[i]);
					vw1.y = D3DVAL(my[i]);
					vw1.z = D3DVAL(mz[i]);

					vw2.x = D3DVAL(mx[i + 1]);
					vw2.y = D3DVAL(my[i + 1]);
					vw2.z = D3DVAL(mz[i + 1]);

					vw3.x = D3DVAL(mx[i + 2]);
					vw3.y = D3DVAL(my[i + 2]);
					vw3.z = D3DVAL(mz[i + 2]);

					// calculate the NORMAL for the road using the CrossProduct <-important!

					D3DVECTOR vDiff = vw1 - vw2;
					vDiff2 = vw3 - vw2;

					D3DVECTOR vCross = CrossProduct(vDiff, vDiff2);

					D3DVECTOR final = Normalize(vCross);

					workx = -final.x;
					worky = -final.y;
					workz = -final.z;
				}
				src_v[cnt].nx = workx;
				src_v[cnt].ny = worky;
				src_v[cnt].nz = workz;

				cnt++;
			}
		}

		qdist = FastDistance(
		    m_vEyePt.x - centroidx,
		    m_vEyePt.y - centroidy,
		    m_vEyePt.z - centroidz);

		psort[number_of_polys_per_frame].vert_index = number_of_polys_per_frame;
		psort[number_of_polys_per_frame].dist = qdist;
		dp_commands[number_of_polys_per_frame] = poly_command;
		dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		number_of_polys_per_frame++;

		if ((poly_command == D3DPT_TRIANGLESTRIP) || (poly_command == D3DPT_TRIANGLEFAN))
			num_triangles_in_scene += (num_vert - 2);

		if (poly_command == D3DPT_TRIANGLELIST)
			num_triangles_in_scene += (num_vert / 3);

		num_verts_in_scene += num_vert;
		num_dp_commands_in_scene++;

		if ((poly_command < 0) || (poly_command > 6))
			PrintMessage(NULL, "CMyD3DApplication::ObjectToD3DVertList -  ERROR UNRECOGNISED COMMAND", NULL, LOGFILE_ONLY);

	} // end for w

	return;
}

int maingameloop3 = 0;
float gametimerAnimation = 0;

//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render3DEnvironment() {
	HRESULT hr;

	static LONGLONG fTimeClock = 0;
	static LONGLONG fTimeClockLast = 0;

	static LONGLONG startthreesecondtimer = 0;
	static LONGLONG startthreesecondtimerlast = 0;

	FLOAT fTime = 0;

	int scrollwords = 0;

	UpdateTalk();

	fTimeClock = DSTimer();

	if (openingscreen == 0)
		SetCursor(NULL);
	if (currentscreen != openingscreen || switchcontrols == 1) {
		switchcontrols = 0;
		if (openingscreen == 0) {
			if (ambientlighton == 0) {

				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, NULL);
			} else {
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x00f0f0f0);
			}
			if (currentinputtype == 1) {

				g_bUseKeyboard = TRUE;
				g_bUseMouse = FALSE;
				g_bUseJoystick = FALSE;
			}

			else if (currentinputtype == 2) {

				g_bUseKeyboard = FALSE;
				g_bUseMouse = TRUE;
				g_bUseJoystick = FALSE;
			} else if (currentinputtype == 3) {

				g_bUseKeyboard = FALSE;
				g_bUseMouse = FALSE;
				g_bUseJoystick = TRUE;
			}
			Pause(TRUE);
			SelectInputDevice();
			Pause(FALSE);
		} else {

			g_bUseKeyboard = TRUE;
			g_bUseMouse = FALSE;
			g_bUseJoystick = FALSE;
			Pause(TRUE);
			SelectInputDevice();
			Pause(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}
	currentscreen = openingscreen;

	// Check the cooperative level before rendering
	if (FAILED(hr = m_pDD->TestCooperativeLevel())) {
		switch (hr) {
		case DDERR_EXCLUSIVEMODEALREADYSET:
		case DDERR_NOEXCLUSIVEMODE:
			// Do nothing because some other app has exclusive mode
			return S_OK;

		case DDERR_WRONGMODE:
			// The display mode changed on us. Resize accordingly
			if (m_pDeviceInfo->bWindowed)
				return Change3DEnvironment();
			break;
		}
		return hr;
	}

	static LARGE_INTEGER frequency = { 0 };
	static LARGE_INTEGER lastTime = { 0 };
	static float elapsedTime = 0.0f;
	float kAnimationSpeed = 7.0f;

	// Initialize frequency and lastTime on first call
	if (frequency.QuadPart == 0) {
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTime);
	}

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	// Calculate elapsed time in milliseconds
	elapsedTime = (float)((currentTime.QuadPart - lastTime.QuadPart) * 1000.0 / frequency.QuadPart);

	// To find the current t we divide the elapsed time by the ratio of 1 second / our anim speed.
	// Since we aren't using 1 second as our t = 1, we need to divide the speed by 1000
	// milliseconds to get our new ratio, which is a 5th of a second.
	float t = elapsedTime / (1000.0f / kAnimationSpeed);
	gametimerAnimation = t;

	// If our elapsed time goes over a 5th of a second, we start over and go to the next key frame
	if (elapsedTime >= (1000.0f / kAnimationSpeed)) {
		// Animation Cycle
		maingameloop3 = 1;
		QueryPerformanceCounter(&lastTime);
	} else {
		maingameloop3 = 0;
	}

	if (maingameloop3) {
		AnimateCharacters();
	}

	if (gametimerdoor)
		fTimeKeyscroll = DSTimer();

	gametimer = DSTimer();

	if ((gametimer - gametimerlast) * time_factor >= 40.0f / 1000) {
		maingameloop = 1;
		gametimerlast = DSTimer();
	} else {

		maingameloop = 0;
	}

	if (RRAppActive && player_list[trueplayernum].bIsPlayerAlive == TRUE)
		UpdateControls();

	// HandleTalkMode(diks);
	UpdateTalk();

	// only update fire in mainloop
	if (player_list[trueplayernum].firespeed > 0 && maingameloop) {
		player_list[trueplayernum].firespeed--;

		if (player_list[trueplayernum].firespeed <= 0)
			player_list[trueplayernum].firespeed = 0;
	}

	for (int i = 0; i < num_monsters; i++) {

		// only update fire in mainloop
		if (monster_list[i].bIsPlayerValid == TRUE && monster_list[i].firespeed > 0 && maingameloop) {
			monster_list[i].firespeed--;
			if (monster_list[trueplayernum].firespeed <= 0)
				monster_list[trueplayernum].firespeed = 0;
		}
	}

	if (flarestart == 1 && maingameloop2)
		CycleFlare();

	gametimer2 = DSTimer();

	if ((gametimer2 - gametimerlast2) * time_factor >= 60.0f / 1000.0f) {
		maingameloop2 = 1;
		gametimerlast2 = DSTimer();
	} else {

		maingameloop2 = 0;
	}

	gametimerpost = DSTimer();

	if ((gametimerpost - gametimerlastpost) * time_factor >= 60000.0f / 1000.0f) {
		maingamelooppost = 1;
		gametimerlastpost = DSTimer();
	} else {

		maingamelooppost = 0;
	}

	gametimerdoor = DSTimer();

	if ((gametimerdoor - gametimerlastdoor) * time_factor >= 30.0f / 1000.0f) {
		maingameloopdoor = 1;
		gametimerlastdoor = DSTimer();
	} else {

		maingameloopdoor = 0;
	}

	gametimerplayer = DSTimer();

	if ((gametimerplayer - gametimerlastplayer) * time_factor >= 90.0f / 1000.0f) {
		maingameloopplayer = 1;

		gametimerlastplayer = DSTimer();
	} else {
		maingameloopplayer = 0;
	}

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);

	MonsterControl();

	SetDiceTexture();

	// FrameMove (animate) the scene
	turnoffscreentext = 1;

	if (maingameloop) {
		rotateprop += .005f;
		if (rotateprop > 3.14f * 2.0f)
			rotateprop = 0;
	}

	ScanMod();
	ActivateSwitch();

	elapsegametimer = DSTimer();

	fTime = (elapsegametimer - elapsegametimerlast) * (float)time_factor;

	elapsegametimersave = fTime;

	if (debugtimer == 1) {
		sprintf_s(gActionMessage, "egt %I64d  egt %I64d egtl %I64d ftime %10.10f tfact %10.10f", elapsegametimer, elapsegametimerlast,
		          (elapsegametimer - elapsegametimerlast), fTime, (float)time_factor);

		UpdateScrollList(0, 245, 255);
	}

	elapsegametimerlast = DSTimer();

	if (GetbFrameMoving() || GetSingleStep()) {
		if (openingscreen > 0) {
			if (FAILED(hr = FrameMoveOpeningScreen(fTime)))
				return hr;
		} else if (perspectiveview == 0) {
			if (FAILED(hr = FrameMove2(fTime)))
				return hr;
		} else {
			if (FAILED(hr = FrameMove(fTime)))
				return hr;
		}
		//		}
		SetSingleStep(FALSE);
	}

	// If the display is in a stereo mode, render the scene from the left eye
	// first, then the right eye.
	if (m_bAppUseStereo && m_pDeviceInfo->bStereo && !m_pDeviceInfo->bWindowed) {
		// Render the scene from the left eye
		m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &m_matLeftView);
		if (FAILED(hr = m_pd3dDevice->SetRenderTarget(m_pddsRenderTargetLeft, 0)))
			return hr;

		if (openingscreen > 0) {

			if (FAILED(hr = RenderOpeningScreen()))
				return hr;
		} else {

			if (FAILED(hr = Render()))
				return hr;
		}

		// Render the scene from the right eye
		m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &m_matRightView);
		if (FAILED(hr = m_pd3dDevice->SetRenderTarget(m_pddsRenderTarget, 0)))
			return hr;

		if (openingscreen > 0) {

			if (FAILED(hr = RenderOpeningScreen()))
				return hr;
		} else {

			if (FAILED(hr = Render()))
				return hr;
		}
	} else {
		// Set center viewing matrix if app is stereo-enabled
		if (m_bAppUseStereo)
			m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &m_matView);

		// Render the scene as normal
		if (openingscreen > 0) {

			if (FAILED(hr = RenderOpeningScreen()))
				return hr;
		} else {

			if (FAILED(hr = Render()))
				return hr;
		}
	}

	// Show the frame rate, etc.
	// if( m_bShowStats )
	// ShowStats();

	if (maingameloop)
		ScanMod();
	if (maingameloop)
		CheckMidiMusic();

	m_pd3dDevice->GetViewport(&vp);
	countmessage = 0;
	char junk[255];

	int gunmodel = 0;
	for (int a = 0; a < num_your_guns; a++) {

		if (your_gun[a].model_id == player_list[trueplayernum].gunid) {

			gunmodel = a;
		}
	}

	int attackbonus = your_gun[gunmodel].sattack;
	int damagebonus = your_gun[gunmodel].sdamage;

	static FLOAT fFPS = 0.0f;

	if (showdebugfps || showdebug) {

		// static FLOAT fFPS = 0.0f;
		static FLOAT fLastTime = 0.0f;
		static DWORD dwFrames = 0L;

		// Keep track of the time lapse and frame count
		FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds
		++dwFrames;

		// Update the frame rate once per second
		if (fTime - fLastTime > 1.0f) {
			fFPS = dwFrames / (fTime - fLastTime);
			fLastTime = fTime;
			dwFrames = 0L;
		}
	}

	if (showdebugfps) {
		sprintf_s(junk, "%-7.02f", fFPS);
		display_message(0.0f, 140.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);
	}

	if (showdebug) {

		sprintf_s(junk, "Fps: %-7.02f Quad: %d Box %d Players %d", fFPS, number_of_polys_per_frame, countboundingbox / 24, num_players);
		display_message(0.0f, 160.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "items: %d monsters: %d 3ds: %d", itemlistcount, num_monsters, num_players2);
		display_message(0.0f, 170.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "Vert: %d tempv: %d total: %d", num_verts_in_scene, tempvcounter,
		          num_verts_in_scene + tempvcounter);
		display_message(0.0f, 180.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "cnt_f %d ap_cnt %d lights %d",
		          cnt_f,
		          ap_cnt, num_light_sources);
		display_message(0.0f, 190.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "DP:  %d ItemInScene: %d time: %2.1f gravity %2.1f lj: %2.1f",
		          num_dp_commands_in_scene, monstercount, (float)elapsegametimer, gravitytime, lastjumptime);
		display_message(0.0f, 200.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "x: %2.1f y: %2.1f z: %2.1f look %2.1f a %2.1f st %2.4f s %2.5f",
		          player_list[trueplayernum].x,
		          player_list[trueplayernum].y,
		          player_list[trueplayernum].z, look_up_ang, angy,
		          elapsegametimersave, currentspeed);
		display_message(0.0f, 210.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "sounds: %d movespeed %2.5f movetime %2.4f gamma: %2.4f", totalsounds, movespeed, movetime, gammasetting);
		display_message(0.0f, 220.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);
	}

	int flag = 1;
	float scrollmessage1 = 60;
	int count = 0;
	int scount = 0;
	char junk2[2048];

	if (debugscreensize == 0)
		scrolllistnum = 6;
	else
		scrolllistnum = 15;

	scount = sliststart;
	scrollmessage1 = 14.0f * (scrolllistnum + 2);

	while (flag) {
		sprintf_s(junk2, "%s", scrolllist1[scount].text);
		display_message(0.0f, scrollmessage1, junk2, vp, scrolllist1[scount].r, scrolllist1[scount].g, scrolllist1[scount].b, 12.5, 16, 0);
		scrollmessage1 -= 14.0f;

		count++;
		scount--;

		if (scount < 0)
			scount = scrolllistnum - 1;

		if (count >= scrolllistnum)
			flag = 0;
	}

	display_box(0.0f, (FLOAT)vp.dwHeight - 210.0f, "", vp, 255, 255, 255, 300.0f, (FLOAT)vp.dwHeight, 1);

	sprintf_s(junk, "Dungeon Stomp 1.80");
	display_message(5.0f, (FLOAT)vp.dwHeight - 150.0f - 14.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

	sprintf_s(junk, "AREA: ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 10.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%s", gfinaltext);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 10.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	statusbardisplay((float)player_list[trueplayernum].hp, (float)player_list[trueplayernum].hp, 1);

	sprintf_s(junk, "HP  : ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 24.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

	sprintf_s(junk, "%s %d/%d", statusbar, player_list[trueplayernum].health, player_list[trueplayernum].hp);
	display_message(0.0f + 65.0f, (FLOAT)vp.dwHeight - 150.0f + 24.0f, junk, vp, 255, 255, 255, 12.5, 16, 0);

	statusbardisplay((float)player_list[trueplayernum].health, (float)player_list[trueplayernum].hp, 0);
	sprintf_s(junk, "%s", statusbar);
	display_message(0.0f + 65.0f, (FLOAT)vp.dwHeight - 150.0f + 24.0f, junk, vp, 255, 255, 255, 12.5, 16, 0);

	sprintf_s(junk, "WPN : ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 38.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

	char junk3[255];
	if (strstr(your_gun[current_gun].gunname, "SCROLL-MAGICMISSLE") != NULL) {
		strcpy_s(junk3, "MAGIC MISSLE");
		sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
	} else if (strstr(your_gun[current_gun].gunname, "SCROLL-FIREBALL") != NULL) {
		strcpy_s(junk3, "FIREBALL");
		sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
	} else if (strstr(your_gun[current_gun].gunname, "SCROLL-LIGHTNING") != NULL) {
		strcpy_s(junk3, "LIGHTNING");
		sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
	} else if (strstr(your_gun[current_gun].gunname, "SCROLL-HEALING") != NULL) {
		strcpy_s(junk3, "HEALING");
		sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
	}

	else {
		sprintf_s(junk, "%s", your_gun[current_gun].gunname);
	}
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 38.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	sprintf_s(junk, "DMG :");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 52.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%dD%d", player_list[trueplayernum].damage1, player_list[trueplayernum].damage2);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 52.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	sprintf_s(junk, "BNS : ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 66.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "+%d/%+d", attackbonus, damagebonus);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 66.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	int nextlevelxp = LevelUpXPNeeded(player_list[trueplayernum].xp) + 1;

	sprintf_s(junk, "XP  : ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 80.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%d", player_list[trueplayernum].xp);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 80.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	sprintf_s(junk, "LVL : ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 94.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%d (%d)", player_list[trueplayernum].hd, nextlevelxp);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 94.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	sprintf_s(junk, "ARMR: ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 108.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%d", player_list[trueplayernum].ac);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 108.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	sprintf_s(junk, "THAC: ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 122.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%d", player_list[trueplayernum].thaco);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 122.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	sprintf_s(junk, "GOLD: ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 136.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%d", player_list[trueplayernum].gold);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 136.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	sprintf_s(junk, "KEYS: ");
	display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 150.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	sprintf_s(junk, "%d", player_list[trueplayernum].keys);
	display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 150.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

	if (bInTalkMode) {
		sprintf_s(junk, "%s`", rr_multiplay_chat_string);
		display_message((FLOAT)vp.dwWidth / 4, (FLOAT)vp.dwHeight - 150.0f + 136.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);
	}

	// Show the frame on the primary surface.
	if (FAILED(hr = (GetFramework())->ShowFrame())) {
		if (DDERR_SURFACELOST != hr)
			return hr;

		(GetFramework())->RestoreSurfaces();
		RestoreSurfaces();
	}

	if (lockframerate == 1) {
		while (1) {
			if ((float)((DSTimer() - fTimeClock) * (float)time_factor) > 0.05f) {
				break;
			}
		}
	}

	startthreesecondtimer = DSTimer(); // Get current time in seconds

	if ((float)((startthreesecondtimer - startthreesecondtimerlast) * (float)time_factor) > 1.0f) {
		startthreesecondtimerlast = startthreesecondtimer;
		if (closesoundid[2] < 100.0f) {
			ApplyPlayerDamage(trueplayernum, 5);
			PlayWavSound(SoundID("pain1"), 100);
			SetPlayerAnimationSequence(trueplayernum, 4);
		}
	}

	UpdateTalk();
	if (player_list[trueplayernum].y < -3000.00) {

		m_vEyePt.x = 780;
		m_vEyePt.y = 160;
		m_vEyePt.z = 780;

		m_vLookatPt.x = 780;
		m_vLookatPt.y = 160;
		m_vLookatPt.z = 780;

		sprintf_s(gActionMessage, "You fell into a bottomless pit...");
		UpdateScrollList(0, 245, 255);
	}
	RegenrateHP();

	return S_OK;
}

void CMyD3DApplication::InitRRvariables() {
	int i;
	float fangle;
	float dx, dy;
	RECT szClient;

	GetClientRect(m_hWnd, &szClient);

	dx = (float)(szClient.right - szClient.left);
	dy = (float)(szClient.bottom - szClient.top);

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);
	ASPECT_RATIO = ((FLOAT)vp.dwHeight) / vp.dwWidth;

	PrintMessage(NULL, "CMyD3DApplication::InitRRvariables - starting", NULL, LOGFILE_ONLY);
	gammafadeout = 0;
	fadeoutcount = 0;

	crosshairenabled = 1;
	currentscreen = -1;
	gammasetting = 1.0f;
	buttonselect = -1;
	buttonselectlast = -1;
	gravityvectorconst.x = 0.0f;
	gravityvectorconst.y = -200.0f; // 9.8/s
	gravityvectorconst.z = 0.0f;
	gravityvector = gravityvectorconst;
	gravityvector.y = -50.0f;

	walk_mode_enabled = TRUE;
	current_gun = 0;
	gravitydropcount = 0;
	car_speed = (float)0;
	angy = 0;
	look_up_ang = 0;
	openingscreen = 1;
	displaycap = 1;
	displaychat = 1;
	numnormals = 0;
	highperftimer = 1;
	itemlistcount = 0;
	hitmonster = 1;
	criticalhiton = 0;
	spellhiton = 0;
	antialias = 0;
	debug_me2("START", "", 0, 0);

	GetCurrentDirectory(255, currentdir);

	oblist = new OBJECTLIST[MAX_OBJECTLIST];
	obdata = new OBJECTDATA[MAX_OBJECTDATA];

	src_v = new D3DVERTEX[MAX_NUM_VERTICES];
	item_list = new PLAYER[MAX_NUM_ITEMS];
	model_list = new MODELLIST[MAX_NUM_PLAYERS];
	player_list = new PLAYER[MAX_NUM_PLAYERS];
	player_list2 = new PLAYER[MAX_NUM_3DS];
	monster_list = new PLAYER[MAX_NUM_MONSTERS];
	levelmodify = new LEVELMOD[50];
	switchmodify = new SWITCHMOD[50];

	merchantlist = new MERCHANT[50];
	ds_reg = new DSREGISTRY[1];
	ds_serverinfo = new DSSERVERINFO[100];
	car_list = new PLAYER[2];
	debug = new PLAYER[2];

	your_gun = new GUNLIST[MAX_NUM_GUNS];

	ds_online = new DSONLINE[2];

	sound_list = new SOUNDLIST[250];

	ds_inilist = new DSINI[2];

	midi_list = new SOUNDLIST[50];

	your_missle = new GUNLIST[MAX_MISSLE];

	other_players_guns = new GUNLIST[1];
	pmdata = new PLAYERMODELDATA[MAX_NUM_PMMODELS];

	poly_clip_flags = new int[MAX_NUM_QUADS];
	poly_clip = new int[MAX_NUM_QUADS];
	texture_list_buffer = new int[MAX_NUM_QUADS];

	dp_command_index_mode = new BOOL[MAX_NUM_QUADS];
	dp_commands = new D3DPRIMITIVETYPE[MAX_NUM_QUADS];

	verts_per_poly = new int[MAX_NUM_QUADS];
	faces_per_poly = new int[MAX_NUM_QUADS];
	src_f = new int[MAX_NUM_FACE_INDICES];

	oblist_overdraw_flags = new BOOL[MAX_NUM_QUADS];
	oblist_overlite_flags = new BOOL[MAX_NUM_QUADS];
	num_vert_per_object = new int[500];

	cell_length = new int *[200];

	strcpy_s(ds_reg->key, "");
	strcpy_s(ds_reg->registered, "0");

	strcpy_s(ds_reg->name, "");

	WriteDSReg();

	for (i = 0; i < 200; i++)
		cell_length[i] = new int[200];

	fLastGunFireTime = DSTimer() * (float)time_factor;
	draw_flags = new BOOL *[200];

	for (i = 0; i < 200; i++)
		draw_flags[i] = new BOOL[200];

	for (i = 0; i < 256; i++)
		DelayKey2[i] = FALSE;

	for (i = 0; i < 256; i++)
		DelayKey2Pause[i] = 0;

	for (i = 0; i < 100; i++)
		closesoundid[i] = 99999.0f;

	strcpy_s(pCMyApp->current_levelname, "level1.map");
	strcpy_s(levelname, "level1");

	outside = 0;
	perf_flag = 0;   // Timer Selection Flag
	time_factor = 0; // Time Scaling Factor

	ds_inilist[0].x = 640;
	ds_inilist[0].y = 480;

	dsinix = 640;
	dsiniy = 480;
	dsiniwindowed = 1;

	LoadDSini();

	for (i = 0; i < 50; i++) {
		merchantlist[i].object = 0;
	}
	merchantlist[0].object = 0;
	merchantlist[0].price = 10;
	merchantlist[0].qty = 0;
	strcpy_s(merchantlist[0].Text1, "POTION");
	strcpy_s(merchantlist[0].Text2, "POTION");
	merchantlist[0].active = 1;
	merchantlist[0].show = 1;

	merchantlist[1].object = 1;
	merchantlist[1].price = 20;
	merchantlist[1].qty = 0;
	strcpy_s(merchantlist[1].Text1, "BASTARDSWORD");
	strcpy_s(merchantlist[1].Text2, "BASTARDSWORD");
	merchantlist[1].active = 1;
	merchantlist[1].show = 1;

	merchantlist[2].object = 1;
	merchantlist[2].price = 220;
	merchantlist[2].qty = 0;
	strcpy_s(merchantlist[2].Text1, "SUPERFLAMESWORD");
	strcpy_s(merchantlist[2].Text2, "SUPERFLAMESWORD");
	merchantlist[2].active = 1;
	merchantlist[2].show = 1;
	merchantlistcount = 3;

	for (i = 0; i < 50; i++) {
		scrolllist1[i].num = 0;
		strcpy_s(scrolllist1[i].text, "");
	}

	for (i = 0; i < MAX_NUM_PLAYERS; i++) {
		player_list[i].frags = 0;
		player_list[i].x = 500;
		player_list[i].y = 22;
		player_list[i].z = 500;
		player_list[i].dist = 0;
		player_list[i].rot_angle = 0;
		player_list[i].model_id = 0;
		player_list[i].skin_tex_id = 0;
		player_list[i].bIsFiring = FALSE;
		player_list[i].ping = 0;
		player_list[i].health = 20;
		player_list[i].rings = 0;
		player_list[i].keys = 0;
		player_list[i].bIsPlayerAlive = TRUE;
		player_list[i].bStopAnimating = FALSE;
		player_list[i].current_weapon = 0;
		player_list[i].current_car = 0;
		player_list[i].current_frame = 0;
		player_list[i].current_sequence = 0;
		player_list[i].bIsPlayerInWalkMode = TRUE;
		player_list[i].RRnetID = 0;
		player_list[i].bIsPlayerValid = FALSE;
		player_list[i].animationdir = 0;
		player_list[i].gunid = 12;
		player_list[i].guntex = 11;
		player_list[i].volume = 0;
		player_list[i].gold = 0;
		player_list[i].sattack = 0;
		player_list[i].sdie = 0;
		player_list[i].sweapon = 0;
		player_list[i].syell = 0;
		player_list[i].ability = 0;

		strcpy_s(player_list[i].name, "");
		strcpy_s(player_list[i].chatstr, "5");
		strcpy_s(player_list[i].name, "Dungeon Stomp");
		strcpy_s(player_list[i].rname, "Crom");

		player_list[i].ac = 7;
		player_list[i].hd = 1;
		player_list[i].hp = 20;
		player_list[i].damage1 = 1;
		player_list[i].damage2 = 4;
		player_list[i].thaco = 19;
		player_list[i].xp = 0;
		player_list[i].firespeed = 0;
		player_list[i].attackspeed = 0;
		player_list[i].applydamageonce = 0;
		//		player_list[i].gunid=FindModelID("AXE");
		//		player_list[i].guntex=FindGunTexture("AXE");
	}
	for (i = 0; i < MAX_NUM_3DS; i++) {
		player_list2[i].frags = 0;
		player_list2[i].dist = 500;
		player_list2[i].x = 500;
		player_list2[i].y = 22;
		player_list2[i].z = 500;
		player_list2[i].rot_angle = 0;
		player_list2[i].model_id = 0;
		player_list2[i].skin_tex_id = 0;
		player_list2[i].bIsFiring = FALSE;
		player_list2[i].ping = 0;
		player_list2[i].health = 20;
		player_list2[i].rings = 0;
		player_list2[i].keys = 0;
		player_list2[i].bIsPlayerAlive = TRUE;
		player_list2[i].bStopAnimating = FALSE;
		player_list2[i].current_weapon = 0;
		player_list2[i].current_car = 0;
		player_list2[i].current_frame = 0;
		player_list2[i].current_sequence = 0;
		player_list2[i].bIsPlayerInWalkMode = TRUE;
		player_list2[i].RRnetID = 0;
		player_list2[i].bIsPlayerValid = FALSE;
		player_list2[i].animationdir = 0;
		player_list2[i].gunid = 12;
		player_list2[i].guntex = 11;
		player_list2[i].volume = 0;
		player_list2[i].gold = 0;
		player_list2[i].sattack = 0;
		player_list2[i].sdie = 0;
		player_list2[i].sweapon = 0;
		player_list2[i].syell = 0;
		player_list2[i].ability = 0;

		strcpy_s(player_list2[i].name, "");
		strcpy_s(player_list2[i].chatstr, "5");
		strcpy_s(player_list2[i].name, "Dungeon Stomp");

		player_list2[i].ac = 7;
		player_list2[i].hd = 1;
		player_list2[i].hp = 20;
		player_list2[i].damage1 = 1;
		player_list2[i].damage2 = 4;
		player_list2[i].thaco = 19;
		player_list2[i].xp = 0;
		player_list2[i].firespeed = 0;
		player_list2[i].attackspeed = 0;
		player_list2[i].applydamageonce = 0;
	}

	for (i = 0; i < MAX_NUM_MONSTERS; i++) {
		monster_list[i].frags = 0;
		monster_list[i].x = 500;
		monster_list[i].y = 22;
		monster_list[i].z = 500;
		monster_list[i].dist = 500;
		monster_list[i].rot_angle = 0;
		monster_list[i].model_id = 0;
		monster_list[i].skin_tex_id = 0;
		monster_list[i].bIsFiring = FALSE;
		monster_list[i].ping = 0;
		monster_list[i].health = 5;
		monster_list[i].bIsPlayerAlive = TRUE;
		monster_list[i].bStopAnimating = FALSE;
		monster_list[i].current_weapon = 0;
		monster_list[i].current_car = 0;
		monster_list[i].current_frame = 0;
		monster_list[i].current_sequence = 2;
		monster_list[i].bIsPlayerInWalkMode = TRUE;
		monster_list[i].RRnetID = 0;
		monster_list[i].bIsPlayerValid = FALSE;
		monster_list[i].animationdir = 0;
		monster_list[i].volume = 0;
		monster_list[i].gold = 0;
		monster_list[i].rings = 0;
		monster_list[i].keys = 0;
		monster_list[i].ability = 0;
		monster_list[i].sattack = 0;
		monster_list[i].sdie = 0;
		monster_list[i].sweapon = 0;
		monster_list[i].syell = 0;

		strcpy_s(monster_list[i].name, "");
		strcpy_s(monster_list[i].chatstr, "5");
		strcpy_s(monster_list[i].name, "Dungeon Stomp");

		monster_list[i].ac = 7;
		monster_list[i].hd = 1;
		monster_list[i].hp = 8;
		monster_list[i].damage1 = 1;
		monster_list[i].damage2 = 8;
		monster_list[i].thaco = 19;
		monster_list[i].xp = 0;
		monster_list[i].firespeed = 0;
		monster_list[i].attackspeed = 0;
		monster_list[i].applydamageonce = 0;
	}
	for (i = 0; i < MAX_NUM_ITEMS; i++) {
		item_list[i].frags = 0;
		item_list[i].x = 500;
		item_list[i].y = 22;
		item_list[i].z = 500;
		item_list[i].dist = 500;
		item_list[i].rot_angle = 0;
		item_list[i].model_id = 0;
		item_list[i].skin_tex_id = 0;
		item_list[i].bIsFiring = FALSE;
		item_list[i].ping = 0;
		item_list[i].health = 5;
		item_list[i].bIsPlayerAlive = TRUE;
		item_list[i].bStopAnimating = FALSE;
		item_list[i].current_weapon = 0;
		item_list[i].current_car = 0;
		item_list[i].current_frame = 0;
		item_list[i].current_sequence = 2;
		item_list[i].bIsPlayerInWalkMode = TRUE;
		item_list[i].RRnetID = 0;
		item_list[i].bIsPlayerValid = FALSE;
		item_list[i].animationdir = 0;
		item_list[i].gold = 0;
		item_list[i].ability = 0;
		item_list[i].firespeed = 0;
		strcpy_s(item_list[i].name, "");
		strcpy_s(item_list[i].chatstr, "5");
		strcpy_s(item_list[i].name, "Dungeon Stomp");
	}

	for (i = 0; i < MAX_NUM_GUNS; i++) {
		your_gun[i].model_id = 0;
		your_gun[i].current_frame = 0;
		your_gun[i].current_sequence = 0;

		if (i <= 0)
			your_gun[i].active = 1;
		else
			your_gun[i].active = 0;

		//		your_gun[i].active = 1;
		your_gun[i].damage1 = 1;
		your_gun[i].damage2 = 4;
	}

	for (i = 0; i < MAX_MISSLE; i++) {
		your_missle[i].model_id = 10;
		your_missle[i].skin_tex_id = 137;
		your_missle[i].current_frame = 0;
		your_missle[i].current_sequence = 0;
		your_missle[i].active = 0;
		your_missle[i].sexplode = 0;
		your_missle[i].smove = 0;
		your_missle[i].qdist = 0.0f;
		your_missle[i].x_offset = 0;
		your_missle[i].y_offset = 0;
		your_missle[i].z_offset = 0;
	}

	MyRRnetID = 1;
	num_players = 0;
	trueplayernum = 0;
	MyPing = 0;
	MyHealth = 100;
	current_level = 1;

	for (i = 0; i < 256; i++)
		DelayKey2[i] = FALSE;

	for (i = 0; i <= 360; i++) {
		fangle = (float)i * k;
		sin_table[i] = (float)sin(fangle);
		cos_table[i] = (float)cos(fangle);
	}

	showchar = 1;
	showdisplay = 1;
	showdice = 1;

	debugscreensize = 0;
	debugtimer = 0;

	jump = 0;
	jumpcount = 0;
	turnoffscreentext = 0;

	num_monsters = 0;
	currentmodelid = 25;
	currentskinid = 148;

	countmodellist = 0;
	currentmodellist = 0;
	num_your_missles = 0;
	usespell = 0;
	cameraf.x = 0;
	cameraf.y = 100;
	cameraf.z = 0;
	pressopendoor = 0;

	specularon = 0;
	betamode = 0;
	drawsphere = 0;
	switchcontrols = 0;
	currentmodellist = 0;

	solid = 1;
	wireframe = 0;
	point = 0;
	forcemainserver = 0;

	m_vEyePt.x = 780;
	m_vEyePt.y = 160;
	m_vEyePt.z = 780;

	m_vLookatPt.x = 780;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 780;

	saveplocation = m_vLookatPt;
	m_vEyePt.x = 700;
	m_vEyePt.y = 160;
	m_vEyePt.z = 700;

	m_vLookatPt.x = 700;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 700;

	model_x = 780;
	model_y = 22;
	model_z = 780;

	iswindowed = 0;

	strcpy_s(gActionMessage, "Dungeon Stomp Version 1.80");
	UpdateScrollList(0, 245, 255);
	strcpy_s(gActionMessage, "Press the ALT key for menu.");
	UpdateScrollList(0, 245, 255);

	strcpy_s(gActionMessage, "Press V to change views.");
	UpdateScrollList(0, 245, 255);
	strcpy_s(gActionMessage, "Press L to listen at doors.");
	UpdateScrollList(0, 245, 255);

	strcpy_s(gActionMessage, "F2=Load F3=Save");
	UpdateScrollList(0, 245, 255);

	gvel = D3DVECTOR(0, 0, 0);

	model_x = 0;
	model_y = 0;
	model_z = 0;
	savevelocity = D3DVECTOR(1, 1, 1);

	your_gun[0].sound_id = SND_FIRE_AR15;
	your_gun[1].sound_id = SND_FIRE_MP5;
	current_gun = 0;

	PrintMemAllocated(sizeof(OBJECTLIST[MAX_OBJECTLIST]), "oblist");
	PrintMemAllocated(sizeof(OBJECTDATA[MAX_OBJECTDATA]), "obdata");

	PrintMemAllocated(sizeof(D3DVERTEX[MAX_NUM_VERTICES]), "src_v");

	PrintMemAllocated(sizeof(PLAYER[MAX_NUM_PLAYERS]), "player_list");
	//	PrintMemAllocated(sizeof(PLAYER[10]), "car_list");
	//	PrintMemAllocated(sizeof(PLAYER[10]), "debug");
	PrintMemAllocated(sizeof(GUNLIST[MAX_NUM_GUNS]), "your_gun");
	//	PrintMemAllocated(sizeof(GUNLIST[10]), "other_players_guns");
	PrintMemAllocated(sizeof(PLAYERMODELDATA[MAX_NUM_PMMODELS]), "pmdata");

	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "poly_clip_flags");
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "poly_clip");
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "texture_list_buffer");
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "dp_command_index_mode");
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "dp_commands");
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "verts_per_poly");
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "faces_per_poly");
	PrintMemAllocated(sizeof(int[MAX_NUM_FACE_INDICES]), "src_f");

	PrintMemAllocated(sizeof(BOOL[MAX_NUM_QUADS]), "oblist_overdraw_flags");
	PrintMemAllocated(sizeof(BOOL[MAX_NUM_QUADS]), "oblist_overlite_flags");

	PrintMemAllocated(total_allocated_memory_count, "TOTAL");

	strcpy_s(ipaddress, "");

	getaddr();

	srand((unsigned)time(NULL));

	InputValues = new float[10000];
	InputValues2 = new float[10000];

	main_window_handle = m_hWnd;

	for (i = 0; i < 10000; i++) {
		m_DisplayMessage[i] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
		m_DisplayMessageFont[i] = 0;
	}

	initDSTimer();

	DSOnlineInit();
	PostServer();

	MakeDice();
	LoadSound();
}

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

BOOL CMyD3DApplication::LoadRR_Resources() {

	char level[80];

	PrintMessage(m_hWnd, "Starting Dungeon Stomp Engine Version 1.80", NULL, SCN_AND_FILE);
	PrintMessage(m_hWnd, "Copyright 2001-2021, Aptisense", NULL, SCN_AND_FILE);
	PrintMessage(m_hWnd, "By Mark Longo", NULL, SCN_AND_FILE);
	PrintMessage(m_hWnd, "Visit www.aptisense.com", NULL, SCN_AND_FILE);

	if (!pCWorld->LoadSoundFiles(m_hWnd, "sounds.dat")) {
		PrintMessage(m_hWnd, "LoadWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (!pCWorld->LoadImportedModelList(m_hWnd, "modellist.dat")) {
		PrintMessage(m_hWnd, "LoadImportedModelList failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (!pCWorld->LoadObjectData(m_hWnd, "objects.dat")) {
		PrintMessage(m_hWnd, "LoadObjectData failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	strcpy_s(level, levelname);
	strcat_s(level, ".map");

	if (!pCWorld->LoadWorldMap(m_hWnd, level)) {
		PrintMessage(m_hWnd, "LoadWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	strcpy_s(level, levelname);
	strcat_s(level, ".cmp");

	if (!pCWorld->InitPreCompiledWorldMap(m_hWnd, level)) {
		PrintMessage(m_hWnd, "InitPreCompiledWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	strcpy_s(level, levelname);
	strcat_s(level, ".mod");

	pCWorld->LoadMod(m_hWnd, level);

	currentmodelid = model_list[currentmodellist].model_id;
	currentskinid = model_list[currentmodellist].modeltexture;

	// set start position

	UpdateMainPlayer();

	player_list[trueplayernum].gunid = FindModelID("AXE");
	player_list[trueplayernum].guntex = FindGunTexture("AXE");

	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT) {

	CMyD3DApplication d3dApp;
	HRESULT hr;

	GetCurrentDirectory(255, pCMyApp->currentdir);
	/*
	_CrtSetDbgFlag (
	       _CRTDBG_ALLOC_MEM_DF |
	        _CRTDBG_LEAK_CHECK_DF);
	    _CrtSetReportMode ( _CRT_ERROR,
	        _CRTDBG_MODE_DEBUG);
*/
	d3dApp.hInstApp = hInst;

	if (FAILED(d3dApp.Create(hInst, strCmdLine)))
		return 0;

	pCMyApp->m_bBufferPaused = FALSE;

	//
	// Create the DInput object
	//
	if (FAILED(d3dApp.CreateDInput(d3dApp.Get_hWnd()))) {
		return FALSE;
	}

	if (FAILED(hr = CoInitialize(NULL)))
		return FALSE;

	// Create a keyboard device
	if (FAILED(d3dApp.CreateInputDevice(d3dApp.Get_hWnd(), g_Keyboard_pDI,
	                                    g_Keyboard_pdidDevice2,
	                                    GUID_SysKeyboard, &c_dfDIKeyboard,
	                                    DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
		return FALSE;
	}

	d3dApp.InitRRvariables();

	// Textures need to have been loaded by now

	d3dApp.LoadRR_Resources();

	// SetMenu(d3dApp.Get_hWnd(), gmenuhandle);
	return d3dApp.Run();

	PrintMessage(NULL, "Quitting", NULL, LOGFILE_ONLY);

	d3dApp.DestroyInputDevice();
	d3dApp.DestroyDInput();

	if (SUCCEEDED(hr)) {
		// Write information to the registry
		// WriteRegisteryInfo();
	}

	CoUninitialize();

	// CloseHandle(g_hDPlaySampleRegKey);
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit() {
	m_pBackground[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 0, 0.0f, 0.6f);
	m_pBackground[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 0, 0.0f, 0.0f);
	m_pBackground[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 0, 1.0f, 0.6f);
	m_pBackground[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 0, 1.0f, 0.0f);

	return S_OK;
}
int movement = 1;
//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove(FLOAT fTimeKey) {

	float r = 15.0f;
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	float lx, ly, lz;
	float radius = 20.0f; // used for flashlight
	float gun_angle;
	float step_left_angy;
	int i;

	D3DVECTOR MissleMove;
	D3DVECTOR MissleVelocity;

	FLOAT tu_left;
	FLOAT tu_right;
	int misslespot = 0;

	fTimeKeysave = fTimeKey;

	// check that angy is between 0 and 360 degrees
	float cameradist = 50.0f;

	UpdateMainPlayer();

	if (angy >= 360)
		angy = angy - 360;

	if (angy < 0)
		angy += 360;

	// slow down in water

	//	if (closesoundid[0] <100){
	//			playerspeedlevel=80.0f;
	//	}
	//	else{
	//			playerspeedlevel=180.0f;
	//	}

	// wrap background pic through 360 degrees about y axis
	// we want to only display one quater of the background
	// texture at a time, because our Field Of View is 90 degrees.

	// texture co-ordinates     angle in degrees
	//   0 to .25				[  0 to  90]
	// .25 to .5				[ 90 to 180]
	// .50 to .75				[180 to 270]
	// .75 to  0				[270 to 360]
	//   0 to .25				[  0 to 90]

	// tu_left is the vertical texture co-ordinate for the left
	// hand side of background rectangle
	tu_left = angy / 360.0000f;

	// tu_right is the vertical texture co-ordinate for the right
	// hand side of background rectangle
	tu_right = tu_left + 0.250f;

	m_pBackground[0].tu = tu_left;
	m_pBackground[1].tu = tu_left;
	m_pBackground[2].tu = tu_right;
	m_pBackground[3].tu = tu_right;

	gun_angle = -angy + (float)90;

	if (gun_angle >= 360)
		gun_angle = gun_angle - 360;
	if (gun_angle < 0)
		gun_angle = gun_angle + 360;

	i = current_gun;
	your_gun[i].rot_angle = gun_angle;
	your_gun[i].x = m_vEyePt.x;
	your_gun[i].y = (float)9 + m_vEyePt.y - 22;
	your_gun[i].z = m_vEyePt.z;

	for (i = 0; i < num_light_sources; i++)
		m_pd3dDevice->LightEnable((DWORD)i, FALSE);

	num_light_sources = 0;

	// Set up the light structure
	D3DLIGHT7 light;
	ZeroMemory(&light, sizeof(D3DLIGHT7));

	light.dcvDiffuse.r = 1.0f;
	light.dcvDiffuse.g = 1.0f;
	light.dcvDiffuse.b = 1.0f;

	light.dcvAmbient.r = 0.3f;
	light.dcvAmbient.g = 0.3f;
	light.dcvAmbient.b = 0.3f;

	light.dvRange = 500.0f; // D3DLIGHT_RANGE_MAX

	// Calculate the flashlight's lookat point, from
	// the player's view direction angy.

	lx = m_vEyePt.x + radius * sinf(angy * k);
	ly = 0;
	lz = m_vEyePt.z + radius * cosf(angy * k);

	// Calculate direction vector for flashlight
	dir_x = lx - m_vEyePt.x;
	dir_y = 0; // ly - m_vEyePt.y;
	dir_z = lz - m_vEyePt.z;

	// set flashlight's position to player's position
	pos_x = player_list[trueplayernum].x;
	pos_y = player_list[trueplayernum].y;
	pos_z = player_list[trueplayernum].z;

	if (lighttype == 0) {
		light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);
		light.dvDirection = D3DVECTOR(dir_x, dir_y, dir_z);
		light.dvFalloff = .1f;
		light.dvTheta = .6f; // spotlight's inner cone
		light.dvPhi = 1.3f;  // spotlight's outer cone
		light.dvAttenuation0 = 1.0f;
		light.dltType = D3DLIGHT_SPOT;
	} else {

		light.dltType = D3DLIGHT_POINT;

		if (strstr(your_gun[current_gun].gunname, "LIGHTNINGSWORD")) {
			light.dcvAmbient.r = 1.0f;
			light.dcvAmbient.g = 1.0f;
			light.dcvAmbient.b = 1.0f;
		} else if (strstr(your_gun[current_gun].gunname, "FLAME") != NULL) {
			light.dcvAmbient.r = 1.0f;
			light.dcvAmbient.g = 0.2f;
			light.dcvAmbient.b = 0.3f;
		} else {
			light.dcvAmbient.r = 0.4f;
			light.dcvAmbient.g = 0.3f;
			light.dcvAmbient.b = 1.0f;
		}

		light.dcvDiffuse.r = light.dcvAmbient.r;
		light.dcvDiffuse.g = light.dcvAmbient.g;
		light.dcvDiffuse.b = light.dcvAmbient.b;

		light.dcvSpecular.r = 0.0f;
		light.dcvSpecular.g = 0.0f;
		light.dcvSpecular.b = 0.0f;
		light.dvRange = 200.0f;
		light.dvPosition.x = pos_x;
		light.dvPosition.y = pos_y;
		light.dvPosition.z = pos_z;

		light.dvAttenuation0 = 1.0f;
	}

	if (bIsFlashlightOn == TRUE) {
		m_pd3dDevice->SetLight(num_light_sources, &light);
		m_pd3dDevice->LightEnable((DWORD)num_light_sources, TRUE);
		num_light_sources++;
	}

	if (GetAsyncKeyState(0x44) < 0) { // d -

		model_y -= 1;
	}
	if (GetAsyncKeyState(0x45) < 0) { // e -

		model_y += 1;
	}

	r = (playerspeed)*fTimeKey;
	currentspeed = r;

	savevelocity = D3DVECTOR(0, 0, 0);

	direction = 0;
	if (playermove == 1 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {

		direction = 1;
		directionlast = 1;
	}

	if (playermove == 4 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {
		direction = -1;
		directionlast = -1;
	}

	bool addVel = false;

	if (movespeed < playerspeedmax && directionlast != 0) {
		addVel = true;
		if (direction) {

			if (moveaccel * movetime >= playerspeedlevel) {
				movespeed = playerspeedlevel * fTimeKey;
			} else {
				movetime = movetime + fTimeKey;
				movespeed = moveaccel * (0.5f * movetime * movetime);
				movespeedsave = movespeed;
				movespeed = movespeed - movespeedold;
				movespeedold = movespeedsave;
			}

			r = movespeed;
		} else {

			movetime = movetime - fTimeKey;

			if (movetime <= 0.0) {
				directionlast = 0;
				movetime = 0;
				r = 0;
			} else {
				movespeed = moveaccel * (0.5f * movetime * movetime);

				movespeedsave = movespeed;
				movespeed = movespeed - movespeedold;
				movespeedold = movespeedsave;
			}
			r = -1 * movespeed;
		}
		savevelocity.x = directionlast * r * sinf(angy * k);
		savevelocity.y = 0.0f;
		savevelocity.z = directionlast * r * cosf(angy * k);
	} else {

		movespeed = 0.0f;
		movetime = 0.0f;
		movespeedold = 0.0f;
		r = 0.0f;
	}

	if (playermovestrife == 6 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {

		step_left_angy = angy - 90;

		if (step_left_angy < 0)
			step_left_angy += 360;

		if (step_left_angy >= 360)
			step_left_angy = step_left_angy - 360;

		r = (playerspeed)*fTimeKey;

		if (addVel) {

			savevelocity.x = r * sinf(step_left_angy * k) + savevelocity.x;
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k) + savevelocity.z;
		} else {
			savevelocity.x = r * sinf(step_left_angy * k);
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k);
		}

		if (Magnitude(gvel) < maxgvel) {
			gvel = gvel + savevelocity;
		}
	}

	if (playermovestrife == 7 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {

		step_left_angy = angy + 90;

		if (step_left_angy < 0)
			step_left_angy += 360;

		if (step_left_angy >= 360)
			step_left_angy = step_left_angy - 360;

		r = (playerspeed)*fTimeKey;

		if (addVel) {

			savevelocity.x = r * sinf(step_left_angy * k) + savevelocity.x;
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k) + savevelocity.z;
		} else {
			savevelocity.x = r * sinf(step_left_angy * k);
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k);
		}

		if (Magnitude(gvel) < maxgvel) {
			gvel = gvel + savevelocity;
		}
	}

	// if (player_list[trueplayernum].current_sequence != 2) {
	//	if (playermove == 0) {

	//		if (savelastmove != playermove && jump == 0) {
	//			if (playermovestrife == 0)
	//				SetPlayerAnimationSequence(trueplayernum, 0);
	//		}
	//	} else {
	//		if (savelastmove != playermove && jump == 0) {
	//			SetPlayerAnimationSequence(trueplayernum, 1);
	//		}
	//	}

	//	if (playermovestrife == 0) {
	//		if (playermovestrife != savelaststrifemove && jump == 0) {
	//			if (playermove == 0)
	//				SetPlayerAnimationSequence(trueplayernum, 0);
	//		}
	//	} else {
	//		if (playermovestrife != savelaststrifemove && jump == 0)
	//			SetPlayerAnimationSequence(trueplayernum, 1);
	//	}
	//}

	if (player_list[trueplayernum].current_sequence != 2) {

		if ((playermove == 1 || playermove == 4) && movement == 0) {
			if (savelastmove != playermove && jump == 0) {
				SetPlayerAnimationSequence(trueplayernum, 1);
			}

			movement = 1;
		} else if (playermove == 0 && movement == 1) {
			if (savelastmove != playermove && jump == 0) {
				SetPlayerAnimationSequence(trueplayernum, 0);
			}

			movement = 0;
		}
	}

	gvel = savevelocity;
	savelastmove = playermove;
	savelaststrifemove = playermovestrife;
	saveoldvelocity = savevelocity;

	D3DVECTOR savepos;
	D3DVECTOR result;

	savepos = m_vEyePt;

	eRadius = D3DVECTOR(spheresize, sphereheight, spheresize);

	if (collisioncode == 0) {
		loadcollisionmap(m_vEyePt, gvel, eRadius);
		result = finalv;
	} else {
		result = collideWithWorld(m_vEyePt / eRadius, (gvel) / eRadius);
		result = result * eRadius;
	}

	m_vEyePt.x = result.x;
	m_vEyePt.y = result.y;
	m_vEyePt.z = result.z;

	if (jump == 1) {

		if (jumpvdir == 0) {

			jumpcount = 0.0f;
			savevelocity.x = 0.0f;
			savevelocity.y = (float)(400.0f) * fTimeKey;
			savevelocity.z = 0.0f;
			jumpv = savevelocity;

			if (maingameloop)
				jumpcount++;

			if (jumpv.y <= 1.0f) {
				jumpv.y = 0.0f;
			}
		}
	}

	if (jumpstart == 1) {
		lastjumptime = 0.0f;
		jumpstart = 0;
		cleanjumpspeed = 600.0f;
		totaldist = 0.0f;
		gravityvector.y = -50.0f;
	}

	if (lastcollide == 1) {
		gravitytime = gravitytime + fTimeKey;
	}

	modellocation = m_vEyePt;

	savevelocity.x = 0.0f;
	savevelocity.y = (cleanjumpspeed * gravitytime) + -2600.0f * (0.5f * gravitytime * gravitytime);
	savevelocity.z = 0.0f;

	saveoldvelocity = savevelocity;
	savevelocity.y = (savevelocity.y - gravityvectorold.y);
	gravityvectorold.y = saveoldvelocity.y;

	if (savevelocity.y == 0.0f && jump == 0)
		savevelocity.y = -80.0f * fTimeKey;

	if (savevelocity.y <= -80.0f)
		savevelocity.y = -80.0f;

	foundcollisiontrue = 0;

	eRadius = D3DVECTOR(spheresize, sphereheight, spheresize);

	if (collisioncode == 0) {
		loadcollisionmap(m_vEyePt, savevelocity, eRadius);
		result = finalv;
	} else {
		result = collideWithWorld(m_vEyePt / eRadius, (savevelocity) / eRadius);
		result = result * eRadius;
	}
	m_vEyePt.x = result.x;
	m_vEyePt.y = result.y;
	m_vEyePt.z = result.z;

	if (foundcollisiontrue == 0) {
		nojumpallow = 1;

		if (lastcollide == 1) {
			lastjumptime = gravitytime;
			totaldist = totaldist + savevelocity.y;
		}

		lastcollide = 1;

		gravityvector.y = -50.0f;
		if (gravitydropcount == 0)
			gravitydropcount = 1;
	} else {
		// something is under us

		if (lastcollide == 1 && savevelocity.y <= 0) {
			if (gravitytime >= 0.4f)
				PlayWavSound(SoundID("jump_land"), 100);

			gravityvector.y = 0.0f;
			gravityvectorold.y = 0.0f;
			cleanjumpspeed = -200.0f;

			lastcollide = 0;
			jump = 0;

			gravitytime = 0.0f;
		} else if (lastcollide == 1 && savevelocity.y > 0) {
			if (gravitytime >= 0.4f)
				PlayWavSound(SoundID("jump_land"), 100);

			cleanjumpspeed = -200.0f;
			lastcollide = 0;
			gravitytime = 0.0f;
			gravityvectorold.y = 0.0f;
		}
		nojumpallow = 0;
		gravitydropcount = 0;
	}

	modellocation = m_vEyePt;

	if (look_up_ang < -89.3f)
		look_up_ang = -89.3f;

	if (look_up_ang > 89.3f)
		look_up_ang = 89.3f;

	float newangle = 0;
	newangle = fixangle(look_up_ang, 90);

	m_vLookatPt.x = m_vEyePt.x + cameradist * sinf(newangle * k) * sinf(angy * k);
	m_vLookatPt.y = m_vEyePt.y + cameradist * cosf(newangle * k);
	m_vLookatPt.z = m_vEyePt.z + cameradist * sinf(newangle * k) * cosf(angy * k);

	MissleSave = m_vLookatPt - m_vEyePt;

	playermove = 0;
	playermovestrife = 0;

	D3DVECTOR vw1, vw2, vw3;
	D3DVECTOR pNormal;

	realEye.x = m_vEyePt.x + 14.0f * sinf(angy * k) * -50.0f;
	realEye.y = m_vEyePt.y;
	realEye.z = m_vEyePt.z + 14.0f * cosf(angy * k) * -50.0f;

	EyeTrue = m_vEyePt;

	EyeTrue.y = m_vEyePt.y + cameraheight;

	LookTrue = m_vLookatPt;
	LookTrue.y = m_vLookatPt.y + cameraheight;
	cameraf.x = LookTrue.x;
	cameraf.y = LookTrue.y;
	cameraf.z = LookTrue.z;

	modellocation = m_vEyePt;

	GunTrue = EyeTrue;

	D3DUtil_SetViewMatrix(matView, EyeTrue, LookTrue, m_vUpVec);
	D3DUtil_SetProjectionMatrix(matProj, FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);

	D3DUtil_SetIdentityMatrix(matWorld);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

	if (dialogpause == 0)
		MoveMonsters();

	UpdateMainPlayer();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove2(FLOAT fTimeKey) {

	float r = 25.0f;
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	float lx, ly, lz;
	float radius = 20.0f; // used for flashlight
	float gun_angle;
	float cameraback = 0;

	int i;

	D3DVECTOR MissleMove;
	D3DVECTOR MissleVelocity;
	D3DVECTOR stuckvelocity;
	D3DVECTOR savepos;
	D3DVECTOR saveposeye;

	FLOAT tu_left;
	FLOAT tu_right;
	int misslespot = 0;
	// check that angy is between 0 and 360 degrees
	float cameradist = 150.0f;

	// if (maingameloop2)
	fTimeKeysave = fTimeKey;
	float step_left_angy;
	//	float step_right_angy;

	r = playerspeed;

	//	r = 11.0f;
	if (angy >= 360)
		angy = angy - 360;

	if (angy < 0)
		angy += 360;

	UpdateMainPlayer();

	// wrap background pic through 360 degrees about y axis
	// we want to only display one quater of the background
	// texture at a time, because our Field Of View is 90 degrees.

	// texture co-ordinates     angle in degrees
	//   0 to .25				[  0 to  90]
	// .25 to .5				[ 90 to 180]
	// .50 to .75				[180 to 270]
	// .75 to  0				[270 to 360]
	//   0 to .25				[  0 to 90]

	// tu_left is the vertical texture co-ordinate for the left
	// hand side of background rectangle
	tu_left = angy / 360.0000f;

	// tu_right is the vertical texture co-ordinate for the right
	// hand side of background rectangle
	tu_right = tu_left + 0.250f;

	m_pBackground[0].tu = tu_left;
	m_pBackground[1].tu = tu_left;
	m_pBackground[2].tu = tu_right;
	m_pBackground[3].tu = tu_right;

	gun_angle = -angy + (float)90;

	if (gun_angle >= 360)
		gun_angle = gun_angle - 360;
	if (gun_angle < 0)
		gun_angle = gun_angle + 360;

	i = current_gun;
	your_gun[i].rot_angle = gun_angle;
	your_gun[i].x = m_vEyePt.x;
	your_gun[i].y = (float)9 + m_vEyePt.y - 22;
	your_gun[i].z = m_vEyePt.z;

	for (i = 0; i < num_light_sources; i++)
		m_pd3dDevice->LightEnable((DWORD)i, FALSE);

	num_light_sources = 0;

	// Set up the light structure
	D3DLIGHT7 light;
	ZeroMemory(&light, sizeof(D3DLIGHT7));

	light.dcvDiffuse.r = 1.0f;
	light.dcvDiffuse.g = 1.0f;
	light.dcvDiffuse.b = 1.0f;

	light.dcvAmbient.r = 0.3f;
	light.dcvAmbient.g = 0.3f;
	light.dcvAmbient.b = 0.3f;

	light.dcvSpecular.r = 1.0f;
	light.dcvSpecular.g = 1.0f;
	light.dcvSpecular.b = 1.0f;

	light.dvRange = 500.0f; // D3DLIGHT_RANGE_MAX

	// Calculate the flashlight's lookat point, from
	// the player's view direction angy.

	lx = m_vEyePt.x + radius * sinf(angy * k);
	ly = 0;
	lz = m_vEyePt.z + radius * cosf(angy * k);

	// Calculate direction vector for flashlight
	dir_x = lx - m_vEyePt.x;
	dir_y = 0; // ly - m_vEyePt.y;
	dir_z = lz - m_vEyePt.z;

	// set flashlight's position to player's position

	pos_x = player_list[trueplayernum].x;
	pos_y = player_list[trueplayernum].y;
	pos_z = player_list[trueplayernum].z;

	if (lighttype == 0) {
		light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);
		light.dvDirection = D3DVECTOR(dir_x, dir_y, dir_z);
		light.dvFalloff = .1f;
		light.dvTheta = .6f; // spotlight's inner cone
		light.dvPhi = 1.3f;  // spotlight's outer cone
		light.dvAttenuation0 = 1.0f;
		light.dltType = D3DLIGHT_SPOT;
	} else {

		light.dltType = D3DLIGHT_POINT;

		if (strstr(your_gun[current_gun].gunname, "LIGHTNINGSWORD")) {
			light.dcvAmbient.r = 1.0f;
			light.dcvAmbient.g = 1.0f;
			light.dcvAmbient.b = 1.0f;
		} else if (strstr(your_gun[current_gun].gunname, "FLAME") != NULL) {
			light.dcvAmbient.r = 1.0f;
			light.dcvAmbient.g = 0.2f;
			light.dcvAmbient.b = 0.3f;
		} else {
			light.dcvAmbient.r = 0.4f;
			light.dcvAmbient.g = 0.3f;
			light.dcvAmbient.b = 1.0f;
		}

		light.dcvDiffuse.r = light.dcvAmbient.r;
		light.dcvDiffuse.g = light.dcvAmbient.g;
		light.dcvDiffuse.b = light.dcvAmbient.b;

		light.dcvSpecular.r = 0.0f;
		light.dcvSpecular.g = 0.0f;
		light.dcvSpecular.b = 0.0f;
		light.dvRange = 200.0f;
		light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);

		light.dvAttenuation0 = 1.0f;
	}

	if (bIsFlashlightOn == TRUE) {
		m_pd3dDevice->SetLight(num_light_sources, &light);
		m_pd3dDevice->LightEnable((DWORD)num_light_sources, TRUE);
		num_light_sources++;
	}

	if (GetAsyncKeyState(0x44) < 0) { // d -

		model_y -= 1;
	}
	if (GetAsyncKeyState(0x45) < 0) { // e -

		model_y += 1;
	}
	// Calculate the players lookat point, from
	// the player's view direction angy.

	r = (playerspeed)*fTimeKey;

	currentspeed = r;
	// jump
	cameraback = -120.0f;
	savevelocity = D3DVECTOR(0, 0, 0);

	savevelocity = D3DVECTOR(0, 0, 0);

	direction = 0;
	if (playermove == 1 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {
		direction = 1;
		directionlast = 1;
	}

	if (playermove == 4 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {
		direction = -1;
		directionlast = -1;
	}

	bool addVel = false;

	if (movespeed < playerspeedmax && directionlast != 0) {
		addVel = true;
		if (direction) {

			if (moveaccel * movetime >= playerspeedlevel) {
				movespeed = playerspeedlevel * fTimeKey;
			} else {
				movetime = movetime + fTimeKey;
				movespeed = moveaccel * (0.5f * movetime * movetime);
				movespeedsave = movespeed;
				movespeed = movespeed - movespeedold;
				movespeedold = movespeedsave;
			}

			r = movespeed;
		} else {

			movetime = movetime - fTimeKey;

			if (movetime <= 0.0) {
				directionlast = 0;
				movetime = 0;
				r = 0;
			} else {
				movespeed = moveaccel * (0.5f * movetime * movetime);

				movespeedsave = movespeed;
				movespeed = movespeed - movespeedold;
				movespeedold = movespeedsave;
			}
			r = -1 * movespeed;
		}

		savevelocity.x = directionlast * r * sinf(angy * k);
		savevelocity.y = 0.0f;
		savevelocity.z = directionlast * r * cosf(angy * k);
	} else {

		movespeed = 0.0f;
		movetime = 0.0f;
		movespeedold = 0.0f;
		r = 0.0f;
	}

	if (playermovestrife == 6 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {

		step_left_angy = angy - 90;

		if (step_left_angy < 0)
			step_left_angy += 360;

		if (step_left_angy >= 360)
			step_left_angy = step_left_angy - 360;

		r = (playerspeed)*fTimeKey;

		if (addVel) {

			savevelocity.x = r * sinf(step_left_angy * k) + savevelocity.x;
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k) + savevelocity.z;
		} else {
			savevelocity.x = r * sinf(step_left_angy * k);
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k);
		}

		// savevelocity.x = r * sinf(step_left_angy * k);
		// savevelocity.y = 0.0f;
		// savevelocity.z = r * cosf(step_left_angy * k);

		if (Magnitude(gvel) < maxgvel) {
			gvel = gvel + savevelocity;
		}
	}

	if (playermovestrife == 7 && player_list[trueplayernum].bIsPlayerAlive == TRUE) {

		step_left_angy = angy + 90;

		if (step_left_angy < 0)
			step_left_angy += 360;

		if (step_left_angy >= 360)
			step_left_angy = step_left_angy - 360;

		r = (playerspeed)*fTimeKey;

		if (addVel) {

			savevelocity.x = r * sinf(step_left_angy * k) + savevelocity.x;
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k) + savevelocity.z;
		} else {
			savevelocity.x = r * sinf(step_left_angy * k);
			savevelocity.y = 0.0f;
			savevelocity.z = r * cosf(step_left_angy * k);
		}

		// savevelocity.x = r * sinf(step_left_angy * k);
		// savevelocity.y = 0.0f;
		// savevelocity.z = r * cosf(step_left_angy * k);

		if (Magnitude(gvel) < maxgvel) {
			gvel = gvel + savevelocity;
		}
	}

	if (player_list[trueplayernum].current_sequence != 2) {
		if (playermove == 0) {

			if (savelastmove != playermove && jump == 0) {
				if (playermovestrife == 0)
					SetPlayerAnimationSequence(trueplayernum, 0);
			}
		} else {
			if (savelastmove != playermove && jump == 0) {
				SetPlayerAnimationSequence(trueplayernum, 1);
			}
		}

		if (playermovestrife == 0) {
			if (playermovestrife != savelaststrifemove && jump == 0) {
				if (playermove == 0)
					SetPlayerAnimationSequence(trueplayernum, 0);
			}
		} else {
			if (playermovestrife != savelaststrifemove && jump == 0)
				SetPlayerAnimationSequence(trueplayernum, 1);
		}
	}

	savelaststrifemove = playermovestrife;
	savelastmove = playermove;

	// friction
	gvel = savevelocity;

	// HORIZONTAL COLLISION

	D3DVECTOR result;
	eRadius = D3DVECTOR(spheresize, sphereheight, spheresize);

	if (collisioncode == 0) {
		loadcollisionmap(m_vLookatPt, gvel, eRadius);
		result = finalv;
	} else {
		result = collideWithWorld(m_vLookatPt / eRadius, (gvel) / eRadius);
		result = result * eRadius;
	}

	m_vLookatPt.x = result.x;
	m_vLookatPt.y = result.y;
	m_vLookatPt.z = result.z;

	// GRAVITY

	if (jump == 1) {
		if (jumpvdir == 0) {

			jumpcount = 0.0f;
			savevelocity.x = 0.0f;
			savevelocity.y = (float)(400.0f) * fTimeKey;
			savevelocity.z = 0.0f;
			jumpv = savevelocity;

			if (maingameloop)
				jumpcount++;

			if (jumpv.y <= 1.0f) {
				jumpv.y = 0.0f;
			}
		}
	}

	foundcollisiontrue = 0;

	if (jumpstart == 1) {
		lastjumptime = 0.0f;
		jumpstart = 0;
		cleanjumpspeed = 600.0f;
		totaldist = 0.0f;
	}

	if (lastcollide == 1)
		gravitytime = gravitytime + fTimeKey;

	savevelocity.x = 0.0f;

	savevelocity.y = (cleanjumpspeed * gravitytime) + -2600.0f * (0.5f * gravitytime * gravitytime);
	savevelocity.z = 0.0f;

	saveoldvelocity = savevelocity;
	savevelocity.y = (savevelocity.y - gravityvectorold.y);
	gravityvectorold.y = saveoldvelocity.y;

	if (savevelocity.y == 0.0f && jump == 0)
		savevelocity.y = -80.0f * fTimeKey;

	if (savevelocity.y <= -80.0f)
		savevelocity.y = -80.0f;
	foundcollisiontrue = 0;

	eRadius = D3DVECTOR(spheresize, sphereheight, spheresize);

	if (collisioncode == 0) {
		loadcollisionmap(m_vLookatPt, savevelocity, eRadius);
		result = finalv;
	} else {
		result = collideWithWorld(m_vLookatPt / eRadius, (savevelocity) / eRadius);
		result = result * eRadius;
	}
	m_vLookatPt.x = result.x;
	m_vLookatPt.y = result.y;
	m_vLookatPt.z = result.z;

	if (foundcollisiontrue == 0) {
		nojumpallow = 1;

		if (lastcollide == 1) {
			lastjumptime = gravitytime;

			totaldist = totaldist + savevelocity.y;
		}

		lastcollide = 1;

		gravityvector.y = -50.0f;
		if (gravitydropcount == 0)
			gravitydropcount = 1;
	} else {
		// something is under us

		if (lastcollide == 1 && savevelocity.y <= 0) {
			if (gravitytime >= 0.4f)
				PlayWavSound(SoundID("jump_land"), 100);

			gravityvector.y = 0.0f;
			gravityvectorold.y = 0.0f;
			cleanjumpspeed = -200.0f;

			lastcollide = 0;
			jump = 0;

			gravitytime = 0.0f;
		} else if (lastcollide == 1 && savevelocity.y > 0) {
			if (gravitytime >= 0.4f)
				PlayWavSound(SoundID("jump_land"), 100);

			cleanjumpspeed = -200.0f;
			lastcollide = 0;
			gravitytime = 0.0f;
			gravityvectorold.y = 0.0f;
		}
		nojumpallow = 0;
		gravitydropcount = 0;
	}

	modellocation = m_vLookatPt;

	// CAMERA

	eRadius = D3DVECTOR(10.0f, 10.0f, 10.0f);
	gvelocity.x = 0;
	gvelocity.y = 100.0f;
	gvelocity.z = 0;

	float newangle = 0;
	newangle = fixangle(angy, rotate_camera);

	savevelocity.x = cameraback * sinf(newangle * k);
	savevelocity.y = 180.0f * sinf(look_up_ang * k);
	savevelocity.z = cameraback * cosf(newangle * k);

	savepos = m_vLookatPt;
	savepos.y = m_vLookatPt.y + 100.0f;

	result = collideWithWorld(m_vLookatPt / eRadius, (savevelocity) / eRadius);
	result = result * eRadius;

	m_vEyePt.x = result.x;
	m_vEyePt.y = result.y;
	m_vEyePt.z = result.z;

	// BOUNDING BOX
	savevelocity.x = -500.0f * sinf(angy * k);
	savevelocity.y = 0.00f;
	savevelocity.z = -500.0f * cosf(angy * k);

	realEye = m_vEyePt + savevelocity;

	playermovestrife = 0;
	playermove = 0;

	MoveCamera();

	EyeTrue.x = cameraf.x;
	EyeTrue.y = cameraf.y;
	EyeTrue.z = cameraf.z;

	LookTrue = m_vLookatPt;
	modellocation = LookTrue;

	D3DUtil_SetViewMatrix(matView, EyeTrue, LookTrue, m_vUpVec);
	D3DUtil_SetProjectionMatrix(matProj, FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);

	D3DUtil_SetIdentityMatrix(matWorld);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

	if (dialogpause == 0)
		MoveMonsters();

	UpdateMainPlayer();

	return S_OK;
}

void CMyD3DApplication::InitBackgroundWrap() {
	DDSURFACEDESC2 ddsd;

	if (lpddsImagePtr[0] == NULL) {
		PrintMessage(NULL, "CMyD3DApplication::InitBackgroundWrap - ERROR lpddsImagePtr[0] = 0 !", NULL, LOGFILE_ONLY);
		return;
	}

	ddsd.dwSize = sizeof(ddsd);
	lpddsImagePtr[0]->GetSurfaceDesc(&ddsd);

	src_backgnd_width = ddsd.dwWidth;
	src_backgnd_height = ddsd.dwHeight;

	PrintMessage(NULL, "\nbackground texture width  = ", _itoa(ddsd.dwWidth), LOGFILE_ONLY);
	PrintMessage(NULL, "background texture height  = ", _itoa(ddsd.dwHeight), LOGFILE_ONLY);
}

BOOL CMyD3DApplication::DrawBackground() {
	float precentage_width_left;
	int src_width_left;
	int dest_right_rect;

	if (m_pddsRenderTarget == NULL)
		return FALSE;

	// if(bShowSky == FALSE)
	//	return TRUE;

	src_width_left = (int)(src_backgnd_width * (angy / 360.0f));

	if ((angy >= 0) && (angy <= 270)) {
		rc_src.left = src_width_left;
		rc_src.right = src_width_left + (src_backgnd_width / 4);
		rc_src.top = 0;
		rc_src.bottom = src_backgnd_height;

		m_pddsRenderTarget->Blt(NULL,
		                        lpddsImagePtr[0],
		                        &rc_src,
		                        NULL,
		                        NULL);
	} else {
		// need to draw the background pic in two parts
		// for when veiwing angle > 270 degrees but lass than
		// 360 degrees.

		// draw left hand rectangle
		dest_right_rect = (int)(m_ddsdRenderTarget.dwWidth * (angy / 360.0f));

		rc_src.left = src_width_left;
		rc_src.right = src_backgnd_width;
		rc_src.top = 0;
		rc_src.bottom = src_backgnd_height;

		precentage_width_left = (float)(src_backgnd_width - src_width_left);
		precentage_width_left = precentage_width_left / ((float)src_backgnd_width / 4.0f);

		rc_dest.left = 0;
		rc_dest.right = (int)((float)m_ddsdRenderTarget.dwWidth * precentage_width_left);
		rc_dest.top = 0;
		rc_dest.bottom = m_ddsdRenderTarget.dwHeight;

		m_pddsRenderTarget->Blt(&rc_dest,
		                        lpddsImagePtr[0],
		                        &rc_src,
		                        NULL,
		                        NULL);

		// draw right hand rectangle

		// Note src_backgnd_width/4 represents an FOV of 90 degrees
		rc_src.left = 0;
		rc_src.right = src_width_left + (src_backgnd_width / 4) - src_backgnd_width;
		rc_src.top = 0;
		rc_src.bottom = src_backgnd_height;

		rc_dest.left = (int)((float)m_ddsdRenderTarget.dwWidth * precentage_width_left);
		rc_dest.right = m_ddsdRenderTarget.dwWidth;
		rc_dest.top = 0;
		rc_dest.bottom = m_ddsdRenderTarget.dwHeight;

		m_pddsRenderTarget->Blt(&rc_dest,
		                        lpddsImagePtr[0],
		                        &rc_src,
		                        NULL, NULL);
	}

	return TRUE;
}

BOOL CMyD3DApplication::RrBltCrossHair(LPDIRECTDRAWSURFACE7 lpddsTexturePtr) {
	RECT rc_crosshair;

	rc_crosshair.left = 0;
	rc_crosshair.top = 0;
	rc_crosshair.right = 30;
	rc_crosshair.bottom = 30;

	if (m_pddsRenderTarget == NULL)
		return FALSE;

	m_pddsRenderTarget->BltFast((m_ddsdRenderTarget.dwWidth / 2) - 15,
	                            (m_ddsdRenderTarget.dwHeight / 2) - 15,
	                            lpddsImagePtr[1],
	                            &rc_crosshair,
	                            DDBLTFAST_SRCCOLORKEY);

	return TRUE;
}

void CMyD3DApplication::AddPlayer(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, int ability) {

	if (monsterenable == 0)
		return;

	player_list[num_players].bIsPlayerValid = TRUE;
	player_list[num_players].bIsPlayerAlive = TRUE;
	player_list[num_players].x = x;
	player_list[num_players].y = y;
	player_list[num_players].z = z;
	player_list[num_players].rot_angle = rot_angle;
	player_list[num_players].model_id = (int)monsterid;
	player_list[num_players].skin_tex_id = (int)monstertexture;

	player_list[num_players].current_sequence = (int)0;
	player_list[num_players].current_frame = (int)0;

	if (monsterid == 0 || monsterid == 2) {
		player_list[num_players].draw_external_wep = TRUE;
	}

	else {
		player_list[num_players].draw_external_wep = FALSE;
	}

	player_list[num_players].monsterid = (int)monnum;

	player_list[num_players].ability = ability;

	player_list[num_players].ac = 7;
	player_list[num_players].hd = 1;
	player_list[num_players].hp = 20;
	player_list[num_players].damage1 = 1;
	player_list[num_players].damage2 = 4;
	player_list[num_players].thaco = 19;
	player_list[num_players].xp = 0;

	num_players++;
	countplayers++;
}

void CMyD3DApplication::AddMonster(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, int s1, int s2, int s3, int s4, int s5, int s6, char damage[80], int thaco, char name[80], float speed, int ability) {

	if (monsterenable == 0)
		return;

	char build[80];
	int count = 0;
	char build2[80];
	int count2 = 0;
	int flag = 0;

	monster_list[num_monsters].bIsPlayerValid = TRUE;
	monster_list[num_monsters].bIsPlayerAlive = TRUE;
	monster_list[num_monsters].x = x;
	monster_list[num_monsters].y = y;
	monster_list[num_monsters].z = z;
	monster_list[num_monsters].rot_angle = rot_angle;
	monster_list[num_monsters].model_id = (int)monsterid;
	monster_list[num_monsters].skin_tex_id = (int)monstertexture;

	monster_list[num_monsters].current_sequence = (int)1;

	int gd = random_num(5) + 1;

	monster_list[num_monsters].current_frame = (int)40 + gd;
	monster_list[num_monsters].animationdir = 0;

	if (gd == 1 || gd == 2)
		monster_list[num_monsters].animationdir = 1;

	strcpy_s(monster_list[num_monsters].rname, name);
	monster_list[num_monsters].speed = speed;
	monster_list[num_monsters].sattack = DSound_Replicate_Sound(s1);
	monster_list[num_monsters].sdie = DSound_Replicate_Sound(s2);
	monster_list[num_monsters].sweapon = DSound_Replicate_Sound(s3);
	monster_list[num_monsters].syell = DSound_Replicate_Sound(s4);

	monster_list[num_monsters].ac = s5;
	monster_list[num_monsters].hd = s6;

	monster_list[num_monsters].attackspeed = 0;
	monster_list[num_monsters].firespeed = 0;

	monster_list[num_monsters].thaco = thaco;

	int raction = random_num(s6 * 8) + s6;

	monster_list[num_monsters].hp = raction;
	monster_list[num_monsters].health = raction;

	int i = 0;

	for (i = 0; i < (int)strlen(damage); i++) {

		if (damage[i] == 'd') {
			flag = 1;
		} else {

			if (flag == 0) {
				build[count++] = damage[i];
			} else {
				build2[count2++] = damage[i];
			}
		}
	}

	build[count] = '\0';
	build2[count2] = '\0';

	monster_list[num_monsters].damage1 = atoi(build);
	monster_list[num_monsters].damage2 = atoi(build2);

	strcpy_s(monster_list[num_monsters].chatstr, "5");

	if (monsterid == 0 || monsterid == 2) {
		monster_list[num_monsters].draw_external_wep = TRUE;
	}

	else {
		monster_list[num_monsters].draw_external_wep = FALSE;
	}

	monster_list[num_monsters].monsterid = (int)monnum;

	monster_list[num_monsters].ability = (int)ability;

	monster_list[num_monsters].bStopAnimating = FALSE;

	if (ability == 3) {
		SetMonsterAnimationSequence(num_monsters, 12);
		monster_list[num_monsters].bIsPlayerAlive = FALSE;
		monster_list[num_monsters].bIsPlayerValid = FALSE;
		monster_list[num_monsters].current_frame = 183;
		monster_list[num_monsters].bStopAnimating = TRUE;
	}

	if (ability == 1)
		monster_list[num_monsters].bStopAnimating = TRUE;

	//	monster_list[num_monsters].player_type=ability;

	strcpy_s(monster_list[num_monsters].rname, name);

	num_monsters++;
	countplayers++;
}

void CMyD3DApplication::AddModel(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, char modelid[80], char modeltexture[80], int ability) {

	if (monsterenable == 0)
		return;

	player_list2[num_players2].bIsPlayerValid = TRUE;

	player_list2[num_players2].x = x;
	player_list2[num_players2].y = y;
	player_list2[num_players2].z = z;
	player_list2[num_players2].rot_angle = rot_angle;
	player_list2[num_players2].model_id = (int)monsterid;
	player_list2[num_players2].skin_tex_id = (int)monstertexture;

	player_list2[num_players2].current_sequence = 0;
	player_list2[num_players2].ability = ability;
	player_list2[num_players2].draw_external_wep = FALSE;

	player_list2[num_players2].monsterid = (int)monnum;

	strcpy_s(player_list2[num_players2].rname, modelid);
	strcpy_s(player_list2[num_players2].texturename, modeltexture);
	if (strstr(modelid, "switch") != NULL) {

		switchmodify[countswitches].num = num_players2;
		switchmodify[countswitches].objectid = (int)monsterid;
		switchmodify[countswitches].active = 0;

		if (rot_angle >= 0.0f && rot_angle < 90.0f) {
			switchmodify[countswitches].direction = 1;
			switchmodify[countswitches].savelocation = x;
		}
		if (rot_angle >= 90.0f && rot_angle < 180.0f) {
			switchmodify[countswitches].direction = 2;
			switchmodify[countswitches].savelocation = z;
		}
		if (rot_angle >= 180.0f && rot_angle < 270.0f) {
			switchmodify[countswitches].direction = 3;
			switchmodify[countswitches].savelocation = x;
		}
		if (rot_angle >= 270.0f && rot_angle <= 360.0f) {
			switchmodify[countswitches].direction = 4;
			switchmodify[countswitches].savelocation = z;
		}
		switchmodify[countswitches].x = x;
		switchmodify[countswitches].y = y;
		switchmodify[countswitches].z = z;
		switchmodify[countswitches].count = 0;
		countswitches++;
	}

	num_players2++;
}

void CMyD3DApplication::AddItem(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, char modelid[80], char modeltexture[80], int ability) {

	if (monsterenable == 0)
		return;
	item_list[itemlistcount].bIsPlayerValid = TRUE;
	item_list[itemlistcount].bIsPlayerAlive = TRUE;
	item_list[itemlistcount].x = x;
	item_list[itemlistcount].y = y;
	item_list[itemlistcount].z = z;
	item_list[itemlistcount].rot_angle = rot_angle;
	item_list[itemlistcount].model_id = (int)monsterid;
	item_list[itemlistcount].skin_tex_id = (int)monstertexture;

	item_list[itemlistcount].current_sequence = 0;
	item_list[itemlistcount].current_frame = 85;

	item_list[itemlistcount].draw_external_wep = FALSE;

	item_list[itemlistcount].monsterid = (int)monnum;

	item_list[itemlistcount].ability = (int)ability;
	item_list[itemlistcount].gold = (int)ability;
	strcpy_s(item_list[itemlistcount].rname, modelid);
	strcpy_s(item_list[itemlistcount].texturename, modeltexture);

	itemlistcount++;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render() {

	int i, j;
	int last_texture_number;
	int vert_index;
	int face_index;
	int cell_x, cell_z;
	int texture_alias_number;
	int icnt;
	float fperpoly = 0;
	float qdist = 0;
	int lookside = 1;
	float mintextdist = 99999;

	LPDIRECTDRAWSURFACE7 lpDDsurface;
	HRESULT hr;

	ap_cnt = 0;
	lookside = (numberofsquares) / 2;

	if (outside == 1)
		numberofsquares = 15;
	else
		numberofsquares = 11;

	FirePlayerMissle(player_list[trueplayernum].x, player_list[trueplayernum].y,
	                 player_list[trueplayernum].z,
	                 angy, trueplayernum, 0, D3DVECTOR(0, 0, 0), look_up_ang);

	lastmaterial = -1;

	IsRenderingOk = TRUE;

	if (rendering_first_frame == TRUE) {
		RRAppActive = TRUE;
		PrintMessage(m_hWnd, "RenderScene : rendering first frame", NULL, LOGFILE_ONLY);
	}

	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
	                    RGBA_MAKE(0, 0, 0, 0), 1.0f, 0L);

	D3DCLIPSTATUS status = { D3DCLIPSTATUS_EXTENTS2, 0, 2048.0f, 0.0f, 2048.0f, 0.0f, 0.0f, 0.0f };

	numnormals = 0;

	for (i = 0; i < 100; i++)
		closesoundid[i] = 99999.0f;

	number_of_polys_per_frame = 0;
	num_triangles_in_scene = 0;
	num_verts_in_scene = 0;
	num_dp_commands_in_scene = 0;

	poly_cnt = 0;
	cnt = 0;
	icnt = 0;
	face_index = 0;
	cnt_f = 0;

	cell_x = (int)(m_vEyePt.x / 256);
	cell_z = (int)(m_vEyePt.z / 256);

	player_x = m_vEyePt.x;
	player_z = m_vEyePt.z;

	for (i = 0; i < MAX_NUM_VERTICES; i++) {
		src_on[i] = 1;
		src_collide[i] = 1;
	}

	ScreenTime = DSTimer(); // Get current time in seconds

	if ((((float)(ScreenTime - ScreenTimeLast) * time_factor) > 5.0f) && showscreentimer == 1) {
		showscreentimer = 0;
	}
	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);

	if (showscreentimer == 1) {
		DisplayDialogText(screentext, 0.0f);
	}

	// MakeBoundingBox();

	for (i = 0; i < MAX_NUM_QUADS; i++) {
		oblist_overdraw_flags[i] = FALSE;
		oblist_overlite_flags[i] = FALSE;
	}

	if (angy >= 360)
		angy = angy - 360;

	if (angy < 0)
		angy += 360;

	FreeSlave();
	ComputeCells(lookside, icnt, cell_x, cell_z);
	DetermineItem(icnt, cell_x, cell_z, vp);

	drawthistri = 1;

	DrawMissle();
	WakeUpMonsters();

	x_off = 0;
	y_off = 0;
	z_off = 0;

	MonsterInRange();
	DrawPlayers();
	OpenChest();
	DrawItems();
	DrawMonsters();
	MakeBoundingBox();
	DrawPlayerGun();
	DrawMissles();

	tempvcounter = 0;

	if (number_of_polys_per_frame == 0) {
		PrintMessage(NULL, "number_of_polys_per_frame = 0", NULL, LOGFILE_ONLY);
		return TRUE;
	}
	car_speed = (float)0;

	hr = m_pd3dDevice->BeginScene();
	if (hr != D3D_OK) {
		PrintMessage(NULL, "BeginScene : FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (m_pd3dDevice->SetClipStatus(&status) != D3D_OK) {
		PrintMessage(NULL, "SetClipStatus : FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE) != D3D_OK)
		return FALSE;

	if (m_tableFog == FALSE) {
	} else {
		// Set the background to blue
		m_pd3dDevice->Clear(0,
		                    NULL,
		                    D3DCLEAR_TARGET,
		                    RGBA_MAKE(0, 0, 80, 0),
		                    1.0f,
		                    0L);
	}

	last_texture_number = 1000;
	vert_index = 0;
	face_index = 0;
	ap_cnt = 0;

	num_polys_per_frame = number_of_polys_per_frame;

	if (polysorton)
		SortPoly(number_of_polys_per_frame);

	if (m_pd3dDevice->SetTexture(0, NULL) != DD_OK)
		PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

	int lsort = 0;

	for (lsort = number_of_polys_per_frame - 1; lsort >= 0; lsort--) {
		if (polysorton) {
			i = psort[Sorted[lsort]].vert_index;
			vert_index = psort[Sorted[lsort]].srcstart;
			fperpoly = (float)psort[Sorted[lsort]].srcfstart;
			face_index = psort[lsort].srcfstart;
		} else {
			i = psort[lsort].vert_index;
			vert_index = psort[lsort].srcstart;
			fperpoly = (float)psort[lsort].srcfstart;
			face_index = psort[lsort].srcfstart;
		}

		texture_alias_number = texture_list_buffer[i];

		texture_number = TexMap[texture_alias_number].texture;

		EnableMaterialFlares(texture_alias_number);

		if (texture_number >= NumTextures)
			texture_number = 0;

		lpDDsurface = lpddsImagePtr[texture_number];

		if ((TexMap[texture_alias_number].is_alpha_texture == TRUE) &&
		    (bEnableAlphaTransparency == TRUE)) {
			alpha_poly_index[ap_cnt] = i;
			alpha_vert_index[ap_cnt] = vert_index;
			alpha_face_index[ap_cnt] = face_index;
			alpha_face_src[ap_cnt] = (int)fperpoly;

			vert_index += verts_per_poly[i];
			if (dp_command_index_mode[i] == USE_INDEXED_DP)
				face_index += faces_per_poly[i] * 3;

			ap_cnt++;
		} else {
			SetTexture(last_texture_number, lpDDsurface);

			// draw objects.dat
			DrawNonIndexed(i, vert_index, last_texture_number);
			// draw 3ds
			DrawIndexed(i, lsort, last_texture_number, vert_index);
		}

	} // end for i

	if (bEnableAlphaTransparency) {
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,
		                             D3DBLEND_SRCCOLOR);

		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
		                             D3DBLEND_INVSRCCOLOR);

		for (j = 0; j < ap_cnt; j++) {
			i = alpha_poly_index[j];

			vert_index = alpha_vert_index[j];
			face_index = alpha_face_index[j];

			texture_alias_number = texture_list_buffer[i];
			texture_number = TexMap[texture_alias_number].texture;

			SetFlamesMaterial();

			if (texture_number >= NumTextures)
				texture_number = 0;

			lpDDsurface = lpddsImagePtr[texture_number];

			if (last_texture_number != texture_number) {
				if (lpDDsurface == NULL)
					PrintMessage(NULL, "Can't find texture", NULL, LOGFILE_ONLY);
				else {
					if (showtexture) {
						// 94-101 (95-102 in file) - Make the torch light bright
						if (texture_number >= 94 && texture_number <= 101) {
							m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,
							                             D3DBLEND_ONE);

							m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
							                             D3DBLEND_ONE);
						} else {
							m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,
							                             D3DBLEND_SRCCOLOR);

							m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
							                             D3DBLEND_INVSRCCOLOR);
						}

						if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
							PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
					}
				}
			}

			DrawAlphaNonIndexed(i, vert_index, last_texture_number);
			DrawAlphaIndexed(i, last_texture_number, j, vert_index);
		} // end for i

		DrawFlare();
	} // end if

	SetOutside(lpDDsurface);

	DrawPlayerSphere(lpDDsurface);

	if (m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE) != D3D_OK)
		return FALSE;

	DrawCrossHair(lpDDsurface);
	ShowScreenOverlays(lpDDsurface);

	if (zbufferenable == 1)
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);

	DisplayPlayerHUD();
	SetTextureStage();

	num_verts_per_frame = vert_index;

	if (rendering_first_frame == TRUE) {
		_itoa_s(number_of_polys_per_frame, buffer, _countof(buffer), 10);
		PrintMessage(NULL, "num_poly = ", buffer, LOGFILE_ONLY);
	}

	if (m_pd3dDevice->GetClipStatus(&status) != D3D_OK) {
		PrintMessage(NULL, "GetClipStatus : FAILED", NULL, LOGFILE_ONLY);
		// return FALSE;
	}

	hr = m_pd3dDevice->EndScene();
	if (hr != D3D_OK) {
		PrintMessage(NULL, "EndScene : FAILED", NULL, LOGFILE_ONLY);
		// return FALSE;
	}

	rendering_first_frame = FALSE;

	return S_OK;
}

void CMyD3DApplication::SetTextureStage() {
	if (filtertype == 0) {
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
	} else if (filtertype == 1) {

		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	} else if (filtertype == 2) {
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_ANISOTROPIC);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_ANISOTROPIC);
	}
}

void CMyD3DApplication::DisplayPlayerHUD() {
	if (displaycap == 1) {
		if (filtertype > 0) {
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
		}
		DisplayPlayerCaption();
	}
}

void CMyD3DApplication::DrawFlare() {
	if (flarestart == 1) {
		if (zbufferenable == 1)
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);

		if (m_pd3dDevice->SetTexture(0, NULL) != DD_OK)
			PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

		// this will cause an alpha fade it believe it or not

		/*
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   TRUE);
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		*/
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
		                            (LPVOID)&flare[0], 4, 0);

		if (zbufferenable == 1)
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
	}
}

void CMyD3DApplication::ShowScreenOverlays(LPDIRECTDRAWSURFACE7 &lpDDsurface) {
	if (showpanel == 1) {
		for (int i = 0; i < 4; i++) {
			int sdice = 0;

			if (maingameloop) {
				if (showlisten > 0) {
					showlisten++;
					if (showlisten > 240)
						showlisten = 0;
				}

				if (showsavingthrow > 0) {
					showsavingthrow++;
					if (showsavingthrow > 240)
						showsavingthrow = 0;
				}
			}

			if (i == 0 || i == 1)
				sdice = 1;

			if (showlisten >= 1 && i == 2)
				sdice = 1;
			if (showsavingthrow >= 1 && i == 3)
				sdice = 1;

			if (sdice == 1) {
				int bground = pCMyApp->FindTextureAlias(dice[i].name);
				texture_number = TexMap[bground].texture;
				lpDDsurface = lpddsImagePtr[texture_number];

				if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
					PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

				if (zbufferenable == 1)
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);

				m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
				                            (LPVOID)&dice[i].dicebox, 4, 0);

				if (zbufferenable == 1)
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
			}
		}

		if (filtertype > 0) {
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
		}

		int bground = pCMyApp->FindTextureAlias("fontA");
		texture_number = TexMap[bground].texture;
		lpDDsurface = lpddsImagePtr[texture_number];

		if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
			PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

		if (zbufferenable == 1)
			m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);

		if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
			PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

		int lastfont = 0;

		for (int i = 0; i < ((countmessage / 4)); i += 1) {
			if (m_DisplayMessageFont[i * 4] != lastfont) {
				lastfont = m_DisplayMessageFont[i * 4];
				if (lastfont == 0) {
					bground = pCMyApp->FindTextureAlias("fontA");
					texture_number = TexMap[bground].texture;
					lpDDsurface = lpddsImagePtr[texture_number];
					if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
						PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

					if (bEnableAlphaTransparency) {
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
					}
				} else if (lastfont == 1) {
					bground = pCMyApp->FindTextureAlias("box2");
					texture_number = TexMap[bground].texture;
					lpDDsurface = lpddsImagePtr[texture_number];

					if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
						PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

					if (bEnableAlphaTransparency) {
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,
						                             D3DBLEND_SRCCOLOR);

						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
						                             D3DBLEND_INVSRCCOLOR);
					}
				} else if (lastfont == 2) {
					bground = pCMyApp->FindTextureAlias("box1");
					texture_number = TexMap[bground].texture;
					lpDDsurface = lpddsImagePtr[texture_number];

					if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
						PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
				} else if (lastfont == 3) {
					bground = pCMyApp->FindTextureAlias("dungeonmaster");
					texture_number = TexMap[bground].texture;
					lpDDsurface = lpddsImagePtr[texture_number];

					if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
						PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
				}
			}

			for (int q = 0; q < ((countmessage)); q += 1) {
				m_DisplayMessage[q].rhw = 1.0f;
			}

			m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
			                            (LPVOID)&m_DisplayMessage[i * (int)4], 4, 0);
		}
	}
}

void CMyD3DApplication::DrawCrossHair(LPDIRECTDRAWSURFACE7 &lpDDsurface) {
	// draw crosshair

	if (perspectiveview == 1) {
		if (crosshairenabled) {
			if (zbufferenable == 1)
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);

			int bground = pCMyApp->FindTextureAlias("crosshair");
			texture_number = TexMap[bground].texture;
			lpDDsurface = lpddsImagePtr[texture_number];

			if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
				PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

			m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
			                            (LPVOID)&crosshair[0], 4, 0);

			if (zbufferenable == 1)
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
		}
	}
}

void CMyD3DApplication::DrawPlayerSphere(LPDIRECTDRAWSURFACE7 &lpDDsurface) {
	if (drawsphere) {

		int bground = pCMyApp->FindTextureAlias("coble10");

		texture_number = TexMap[bground].texture;
		lpDDsurface = lpddsImagePtr[texture_number];

		if (showtexture)
			if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
				PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

		for (int i = 0; i < countboundingbox / 4; i++) {

			if (m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			                                D3DFVF_VERTEX, (LPVOID)&boundingbox[i * (int)4],
			                                (int)4, D3DDP_WAIT) != D3D_OK) {
			}
		}

		if (perspectiveview == 1) {
			D3DUtil_SetTranslateMatrix(matWorld, D3DVECTOR(m_vEyePt.x, m_vEyePt.y, m_vEyePt.z));
		} else {
			D3DUtil_SetTranslateMatrix(matWorld, D3DVECTOR(m_vLookatPt.x, m_vLookatPt.y, m_vLookatPt.z));
		}

		m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
		                                   g_pObstacleVertices2, g_dwNumObstacleVertices2,
		                                   g_pObstacleIndices2, g_dwNumObstacleIndices2, 0);
	}
}

void CMyD3DApplication::SetOutside(LPDIRECTDRAWSURFACE7 &lpDDsurface) {
	if (outside == 1) {
		D3DUtil_SetTranslateMatrix(matWorld, D3DVECTOR(startpos[0].x,
		                                               startpos[0].y, startpos[0].z));

		D3DMATERIAL7 mtrl;
		D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);
		mtrl.emissive.r = 1.0f;
		mtrl.emissive.g = 1.0f;
		mtrl.emissive.b = 1.0f;
		// lastmaterial=1;
		m_pd3dDevice->SetMaterial(&mtrl);

		int bground = pCMyApp->FindTextureAlias("background");
		texture_number = TexMap[bground].texture;
		lpDDsurface = lpddsImagePtr[texture_number];

		if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
			PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
		                                   g_pObstacleVertices, g_dwNumObstacleVertices,
		                                   g_pObstacleIndices, g_dwNumObstacleIndices, 0);
	}
}

void CMyD3DApplication::DrawAlphaIndexed(int i, int &last_texture_number, int j, int &vert_index) {

	D3DPRIMITIVETYPE command;
	D3DVECTOR vw1, vw2, vw3;
	float workx, worky, workz;
	DWORD dwIndexCount = 0;
	DWORD dwVertexCount = 0;
	int face_index = 0;

	if (dp_command_index_mode[i] == USE_INDEXED_DP) {
		dwIndexCount = faces_per_poly[i] * 3;
		dwVertexCount = verts_per_poly[i];
		command = dp_commands[i];

		last_texture_number = texture_number;

		for (int t = 0; t < (int)dwIndexCount; t++) {
			face_index = alpha_face_src[j];
			int f_index = src_f[face_index + t];

			memset(&temp_v[t], 0, sizeof(D3DVERTEX));
			memcpy(&temp_v[t], &src_v[vert_index + f_index],
			       sizeof(D3DVERTEX));
		}
		int counttri = 0;

		for (int t = 0; t < (int)dwIndexCount; t++) {

			// ijump
			if (counttri == 0) {

				vw1.x = temp_v[t].x;
				vw1.y = temp_v[t].y;
				vw1.z = temp_v[t].z;

				vw2.x = temp_v[t + 1].x;
				vw2.y = temp_v[t + 1].y;
				vw2.z = temp_v[t + 1].z;

				vw3.x = temp_v[t + 2].x;
				vw3.y = temp_v[t + 2].y;
				vw3.z = temp_v[t + 2].z;
				// calculate the NORMAL for the road using the CrossProduct <-important!

				D3DVECTOR vDiff = vw1 - vw2;
				D3DVECTOR vDiff2 = vw3 - vw2;

				D3DVECTOR vCross = CrossProduct(vDiff, vDiff2);

				D3DVECTOR final = Normalize(vCross);

				workx = -final.x;
				worky = -final.y;
				workz = -final.z;
			}

			counttri++;
			if (counttri > 2)
				counttri = 0;

			temp_v[t].nx = workx;
			temp_v[t].ny = worky;
			temp_v[t].nz = workz;

			if (normalon) {
				numnormals = 0;
				normal_line[numnormals].x = temp_v[t].x;
				normal_line[numnormals].y = temp_v[t].y;
				normal_line[numnormals].z = temp_v[t].z;
				normal_line[numnormals].color = 10;
				numnormals++;
				normal_line[numnormals].x = temp_v[t].x + temp_v[t].nx * normalscale;
				normal_line[numnormals].y = temp_v[t].y + temp_v[t].ny * normalscale;
				normal_line[numnormals].z = temp_v[t].z + temp_v[t].nz * normalscale;
				normal_line[numnormals].color = 10;
				numnormals++;

				if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
				                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
				}
			}
		}

		if (m_pd3dDevice->DrawPrimitive(command,
		                                D3DFVF_VERTEX,
		                                (LPVOID)&temp_v[0],
		                                dwIndexCount,
		                                NULL) != D3D_OK) {
			PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
			// return FALSE;
		}

		last_texture_number = texture_number;

		face_index += dwIndexCount;
		vert_index += dwVertexCount;
	}
}

void CMyD3DApplication::DrawAlphaNonIndexed(int i, int &vert_index, int &last_texture_number) {
	if (dp_command_index_mode[i] == USE_NON_INDEXED_DP) {
		//				D3DFVF_TLVERTEX
		if (src_on[vert_index] == 1) {
			if (m_pd3dDevice->DrawPrimitive(dp_commands[i], D3DFVF_VERTEX,
			                                (LPVOID)&src_v[vert_index], verts_per_poly[i], NULL) != D3D_OK) {
				PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
				// return FALSE;
			}
		}
		if (normalon) {
			int vertloop = 0;

			for (vertloop = 0; vertloop < verts_per_poly[i]; vertloop++) {

				numnormals = 0;
				normal_line[numnormals].x = src_v[vert_index + vertloop].x;
				normal_line[numnormals].y = src_v[vert_index + vertloop].y;
				normal_line[numnormals].z = src_v[vert_index + vertloop].z;
				normal_line[numnormals].color = 10;
				numnormals++;
				normal_line[numnormals].x = src_v[vert_index + vertloop].x + src_v[vert_index + vertloop].nx * normalscale;
				normal_line[numnormals].y = src_v[vert_index + vertloop].y + src_v[vert_index + vertloop].ny * normalscale;
				normal_line[numnormals].z = src_v[vert_index + vertloop].z + src_v[vert_index + vertloop].nz * normalscale;
				normal_line[numnormals].color = 10;
				numnormals++;

				if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
				                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
				}
			}
		}

		last_texture_number = texture_number;
		vert_index += verts_per_poly[i];
	}
}

void CMyD3DApplication::SetFlamesMaterial() {
	if (menuflares == 1) {
		if (texture_number >= 94 && texture_number <= 101 ||  // flame
		    texture_number >= 205 && texture_number <= 209 || // flare
		    texture_number >= 278 && texture_number <= 287 || // shine
		    texture_number >= 288 && texture_number <= 295    //|| explode
		    // texture_number >= 369 && texture_number <= 377   //blood
		) {
			if (lastmaterial == -1 || lastmaterial == 0) {
				D3DMATERIAL7 mtrl;
				D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);

				mtrl.emissive.r = 1.0f;
				mtrl.emissive.g = 1.0f;
				mtrl.emissive.b = 1.0f;
				lastmaterial = 1;
				m_pd3dDevice->SetMaterial(&mtrl);
			}
		} else {
			if (lastmaterial == 1 || lastmaterial == -1.0f) {
				D3DMATERIAL7 mtrl;
				D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);
				lastmaterial = 0;

				mtrl.dcvSpecular.r = 0.3f;
				mtrl.dcvSpecular.g = 0.3f;
				mtrl.dcvSpecular.b = 0.3f;
				mtrl.dcvSpecular.a = 0.0f;

				mtrl.power = 40.0f;
				m_pd3dDevice->SetMaterial(&mtrl);
			}
		}
	}
}

void CMyD3DApplication::DrawIndexed(int i, int fakel, int &last_texture_number, int vert_index) {
	D3DPRIMITIVETYPE command;
	int face_index = 0;
	D3DVECTOR vw1, vw2, vw3;
	float workx, worky, workz;

	if (dp_command_index_mode[i] == USE_INDEXED_DP) {

		int dwIndexCount = faces_per_poly[i] * 3;
		int dwVertexCount = verts_per_poly[i];
		command = dp_commands[i];

		if (polysorton) {
			dwIndexCount = psort[Sorted[fakel]].facesperpoly * 3;
			dwVertexCount = psort[Sorted[fakel]].vertsperpoly;
			command = dp_commands[Sorted[fakel]];
			face_index = psort[Sorted[fakel]].srcfstart;
		} else {
			dwIndexCount = psort[fakel].facesperpoly * 3;
			dwVertexCount = psort[fakel].vertsperpoly;
			command = dp_commands[fakel];
			face_index = psort[fakel].srcfstart;
		}

		last_texture_number = texture_number;

		for (int t = 0; t < (int)dwIndexCount; t++) {
			int f_index = src_f[face_index + t];
			memset(&temp_v[t], 0, sizeof(D3DVERTEX));
			memcpy(&temp_v[t], &src_v[vert_index + f_index],
			       sizeof(D3DVERTEX));
		}

		tempvcounter = tempvcounter + dwIndexCount;

		int counttri = 0;

		for (int t = 0; t < (int)dwIndexCount; t++) {
			if (counttri == 0) {

				vw1.x = temp_v[t].x;
				vw1.y = temp_v[t].y;
				vw1.z = temp_v[t].z;

				vw2.x = temp_v[t + 1].x;
				vw2.y = temp_v[t + 1].y;
				vw2.z = temp_v[t + 1].z;

				vw3.x = temp_v[t + 2].x;
				vw3.y = temp_v[t + 2].y;
				vw3.z = temp_v[t + 2].z;

				// calculate the NORMAL for the road using the CrossProduct <-important!

				D3DVECTOR vDiff = vw1 - vw2;
				D3DVECTOR vDiff2 = vw3 - vw2;

				D3DVECTOR vCross = CrossProduct(vDiff, vDiff2);

				D3DVECTOR final = Normalize(vCross);

				workx = -final.x;
				worky = -final.y;
				workz = -final.z;
			}

			counttri++;
			if (counttri > 2)
				counttri = 0;

			temp_v[t].nx = workx;
			temp_v[t].ny = worky;
			temp_v[t].nz = workz;

		}

		if (dwIndexCount > 0) {
			// Average normals for shared vertices so lighting stays smooth
			std::vector<D3DVECTOR> smoothedNormals(dwIndexCount);
			const float normalShareEpsilon = 0.0005f;
			for (int v = 0; v < (int)dwIndexCount; ++v) {
				float nx = temp_v[v].nx;
				float ny = temp_v[v].ny;
				float nz = temp_v[v].nz;
				const float vx = temp_v[v].x;
				const float vy = temp_v[v].y;
				const float vz = temp_v[v].z;
				for (int u = 0; u < (int)dwIndexCount; ++u) {
					if (u == v)
						continue;
					if (fabsf(vx - temp_v[u].x) < normalShareEpsilon &&
					    fabsf(vy - temp_v[u].y) < normalShareEpsilon &&
					    fabsf(vz - temp_v[u].z) < normalShareEpsilon) {
						nx += temp_v[u].nx;
						ny += temp_v[u].ny;
						nz += temp_v[u].nz;
					}
				}
				float lenSq = nx * nx + ny * ny + nz * nz;
				if (lenSq > 0.000001f) {
					float invLen = 1.0f / sqrtf(lenSq);
					nx *= invLen;
					ny *= invLen;
					nz *= invLen;
				} else {
					nx = 0.0f;
					ny = 0.0f;
					nz = 0.0f;
				}
				smoothedNormals[v] = D3DVECTOR(nx, ny, nz);
			}
			for (int v = 0; v < (int)dwIndexCount; ++v) {
				temp_v[v].nx = smoothedNormals[v].x;
				temp_v[v].ny = smoothedNormals[v].y;
				temp_v[v].nz = smoothedNormals[v].z;
			}
		}

		if (normalon) {
			for (int t = 0; t < (int)dwIndexCount; t++) {
				numnormals = 0;
				normal_line[numnormals].x = temp_v[t].x;
				normal_line[numnormals].y = temp_v[t].y;
				normal_line[numnormals].z = temp_v[t].z;
				normal_line[numnormals].color = 10;
				numnormals++;
				normal_line[numnormals].x = temp_v[t].x + temp_v[t].nx * normalscale;
				normal_line[numnormals].y = temp_v[t].y + temp_v[t].ny * normalscale;
				normal_line[numnormals].z = temp_v[t].z + temp_v[t].nz * normalscale;
				normal_line[numnormals].color = 10;
				numnormals++;

				if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
				                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
				}
			}
		}

		if (m_pd3dDevice->DrawPrimitive(command,
		                                D3DFVF_VERTEX,
		                                (LPVOID)&temp_v[0],
		                                dwIndexCount,
		                                NULL) != D3D_OK) {
			PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
			// return FALSE;
		}

		last_texture_number = texture_number;
	}
}

void CMyD3DApplication::DrawNonIndexed(int i, int vert_index, int &last_texture_number) {
	if (dp_command_index_mode[i] == USE_NON_INDEXED_DP) {

		if (src_on[vert_index] == 1) {
			if (m_pd3dDevice->DrawPrimitive(dp_commands[i], D3DFVF_VERTEX,
			                                (LPVOID)&src_v[vert_index], verts_per_poly[i], NULL) != D3D_OK) {
				PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
				// return;
			}
		}
		if (normalon) {
			int vertloop = 0;

			for (vertloop = 0; vertloop < verts_per_poly[i]; vertloop++) {

				numnormals = 0;
				normal_line[numnormals].x = src_v[vert_index + vertloop].x;
				normal_line[numnormals].y = src_v[vert_index + vertloop].y;
				normal_line[numnormals].z = src_v[vert_index + vertloop].z;
				normal_line[numnormals].color = 10;
				numnormals++;
				normal_line[numnormals].x = src_v[vert_index + vertloop].x + src_v[vert_index + vertloop].nx * normalscale;
				normal_line[numnormals].y = src_v[vert_index + vertloop].y + src_v[vert_index + vertloop].ny * normalscale;
				normal_line[numnormals].z = src_v[vert_index + vertloop].z + src_v[vert_index + vertloop].nz * normalscale;
				normal_line[numnormals].color = 10;
				numnormals++;

				if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
				                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
				}
			}
		}
		last_texture_number = texture_number;
	}
}

void CMyD3DApplication::SetTexture(int last_texture_number, const LPDIRECTDRAWSURFACE7 &lpDDsurface) {
	if (last_texture_number != texture_number) {
		if (lpDDsurface == NULL)
			PrintMessage(NULL, "Can't find texture", NULL, LOGFILE_ONLY);
		else {
			if (showtexture) {
				if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
					PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
			}
		}
	}
}

void CMyD3DApplication::EnableMaterialFlares(int texture_alias_number) {
	if (menuflares == 1) {
		if (strstr(TexMap[texture_alias_number].tex_alias_name, "@") != NULL ||
		    strstr(TexMap[texture_alias_number].tex_alias_name, "flare") != NULL ||
		    strstr(TexMap[texture_alias_number].tex_alias_name, "goldpl") != NULL ||
		    strstr(TexMap[texture_alias_number].tex_alias_name, "lava") != NULL ||
		    strstr(TexMap[texture_alias_number].tex_alias_name, "missle") != NULL) {

			if (lastmaterial == -1 || lastmaterial == 0) {
				D3DMATERIAL7 mtrl;
				D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);
				mtrl.emissive.r = 1.0f;
				mtrl.emissive.g = 1.0f;
				mtrl.emissive.b = 1.0f;
				lastmaterial = 1;
				m_pd3dDevice->SetMaterial(&mtrl);
			}
		} else {

			if (lastmaterial == 1 || lastmaterial == -1.0f) {
				D3DMATERIAL7 mtrl;
				D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);
				lastmaterial = 0;
				m_pd3dDevice->SetMaterial(&mtrl);
			}
		}
	}
}

void CMyD3DApplication::DrawMissles() {

	D3DVECTOR vw1, vw2;

	fDot2 = 0.0f;
	weapondrop = 0;

	fDot2 = 0.0f;
	for (int misslecount = 0; misslecount < MAX_MISSLE; misslecount++) {

		if (your_missle[misslecount].active == 2) {

			wx = your_missle[misslecount].x;
			wy = your_missle[misslecount].y;
			wz = your_missle[misslecount].z;

			int cresult;
			int tex;
			tex = your_missle[misslecount].skin_tex_id;

			D3DVECTOR collidenow;
			D3DVECTOR normroadold;
			D3DVECTOR work1, work2, vDiff;
			normroadold.x = 50;
			normroadold.y = 0;
			normroadold.z = 0;

			work1.x = m_vEyePt.x;
			work1.y = m_vEyePt.y;
			work1.z = m_vEyePt.z;

			work2.x = wx;
			work2.y = wy;
			work2.z = wz;

			work1.y = wy;

			vDiff = work1 - work2;
			vDiff = Normalize(vDiff);

			vw1.x = work1.x;
			vw1.y = work1.y;
			vw1.z = work1.z;

			vw2.x = work2.x;
			vw2.y = work2.y;
			vw2.z = work2.z;

			vDiff = vw1 - vw2;

			D3DVECTOR final, final2;
			D3DVECTOR m, n;

			final = Normalize(vDiff);
			final2 = Normalize(normroadold);

			float fDot = dot(final, final2);
			float convangle;
			convangle = (float)acos(fDot) / k;

			fDot = convangle;

			if (vw2.z < vw1.z) {
			} else {
				fDot = 180.0f + (180.0f - fDot);
			}

			normroadold.x = 0;
			normroadold.y = 50;
			normroadold.z = 0;

			work1.x = m_vEyePt.x;
			work1.y = m_vEyePt.y;
			work1.z = m_vEyePt.z;

			work1 = EyeTrue;

			work2.x = wx;
			work2.y = wy;
			work2.z = wz;

			vDiff = work1 - work2;
			vDiff = Normalize(vDiff);

			vw1.x = work1.x;
			vw1.y = work1.y;
			vw1.z = work1.z;

			vw2.x = work2.x;
			vw2.y = work2.y;
			vw2.z = work2.z;

			vDiff = vw1 - vw2;

			final = Normalize(vDiff);
			final2 = Normalize(normroadold);

			fDot2 = dot(final, final2);

			fDot2last = fDot2;

			convangle = (float)acos(fDot2) / k;

			fDot2 = convangle;
			fDot2 = -1 * fixangle(fDot2, -90);

			fDot2last = fDot2;

			// Show blood splatter

			int bloodmodel = 100;

			if (your_missle[misslecount].critical) {
				bloodmodel = 124;
			}

			PlayerToD3DVertList(bloodmodel,
			                    0,
			                    (int)fixangle(fDot, 180),
			                    tex,
			                    USE_PLAYERS_SKIN);

			fDot2 = 0.0f;

			if (maingameloop2) {
				cresult = CycleBitMap(your_missle[misslecount].skin_tex_id);
				if (cresult != -1) {

					if (cresult < tex) {
						your_missle[misslecount].active = 0;
					} else {
						tex = cresult;
						your_missle[misslecount].skin_tex_id = tex;
					}
				}
			}
		}
	}
}

void CMyD3DApplication::DrawPlayerGun() {
	// DRAW YOUR GUN ///////////////////////////////////////////

	BOOL use_player_skins_flag = false;
	int i = 0;
	float skx, sky;
	int ob_type;
	int angle;

	if (player_list[trueplayernum].model_id == 0 &&
	    player_list[trueplayernum].bIsPlayerAlive &&
	    strstr(your_gun[current_gun].gunname, "SCROLL") == NULL) {
		skx = (float)0.40000000 / (float)256;
		sky = (float)1.28000000 / (float)256;
		use_player_skins_flag = 1;

		i = current_gun;
		i = 0;
		if (current_gun < num_your_guns) {

			ob_type = player_list[trueplayernum].gunid;
			current_frame = player_list[trueplayernum].current_frame;
			angle = (int)player_list[trueplayernum].gunangle;

			if (perspectiveview == 1) {
				weapondrop = 1;
				fDot2 = look_up_ang;
			} else {
				weapondrop = 0;
				fDot2 = 0.0f;
			}
			if (weapondrop == 0) {
				wx = player_list[trueplayernum].x;
				wy = player_list[trueplayernum].y;
				wz = player_list[trueplayernum].z;
			} else {

				wx = GunTruesave.x;
				wy = GunTruesave.y + 10.0f;
				wz = GunTruesave.z;
			}

			int nextFrame = GetNextFramePlayer();

			PlayerToD3DVertList(ob_type,
			                    current_frame,
			                    angle,
			                    player_list[trueplayernum].guntex,
			                    USE_DEFAULT_MODEL_TEX, nextFrame);
			fDot2 = 0.0f;
			weapondrop = 0;

			//				}
		}
	} else if (player_list[trueplayernum].model_id != 0 &&
	           player_list[trueplayernum].bIsPlayerAlive &&
	           strstr(your_gun[current_gun].gunname, "SCROLL") == NULL) {

		ob_type = player_list[trueplayernum].gunid;
		current_frame = player_list[trueplayernum].current_frame;
		angle = (int)player_list[trueplayernum].gunangle;
		if (perspectiveview == 1) {
			weapondrop = 1;
			fDot2 = look_up_ang;
		} else {
			weapondrop = 0;
			fDot2 = 0.0f;
		}
		if (weapondrop == 0) {
			wx = player_list[trueplayernum].x;
			wy = player_list[trueplayernum].y;
			wz = player_list[trueplayernum].z;
		} else {

			wx = GunTruesave.x;
			wy = GunTruesave.y;
			wz = GunTruesave.z;
		}

		int getgunid = currentmodellist;

		if (strcmp(model_list[getgunid].monsterweapon, "NONE") != 0) {

			PlayerToD3DVertList(FindModelID(model_list[getgunid].monsterweapon),
			                    current_frame,
			                    angle,
			                    FindGunTexture(model_list[getgunid].monsterweapon),
			                    USE_DEFAULT_MODEL_TEX);
		}
	}
}

void CMyD3DApplication::DrawMonsters() {
	int cullflag = 0;
	merchantfound = 0;
	BOOL use_player_skins_flag = false;
	int getgunid = 0;

	for (int i = 0; i < num_monsters; i++) {

		if (monster_list[i].bIsPlayerValid == TRUE && monster_list[i].bStopAnimating == FALSE ||
		    monster_list[i].bIsPlayerValid == FALSE && monster_list[i].bStopAnimating == FALSE ||
		    monster_list[i].bIsPlayerAlive == FALSE && monster_list[i].bStopAnimating == TRUE && showmonstermenu == 1

		) {

			cullflag = 0;
			for (int cullloop = 0; cullloop < monstercount; cullloop++) {

				if (monstercull[cullloop] == monster_list[i].monsterid) {
					cullflag = 1;
					break;
				}
			}

			if (cullflag == 1) {
				wx = monster_list[i].x;
				wy = monster_list[i].y;
				wz = monster_list[i].z;

				float qdist = FastDistance(player_list[trueplayernum].x - wx, player_list[trueplayernum].y - wy, player_list[trueplayernum].z - wz);

				if (strcmp(monster_list[i].rname, "CENTAUR") == 0 && qdist <= 100.0f && merchantfound == 0) {
					// just found
					merchantfound = 1;
					DisplayDialogText("Press SPACE to buy and sell items", 0.0f);
				}

				D3DVECTOR work1;
				work1.x = wx;
				work1.y = wy;
				work1.z = wz;
				int monsteron = 0;

				if (perspectiveview == 0)
					monsteron = CalculateView(realEye, work1, 20.0f);
				else
					monsteron = CalculateView(realEye, work1, 20.0f);
				if (monsteron) {

					use_player_skins_flag = 0;
					current_frame = monster_list[i].current_frame;

					int nextFrame = GetNextFrame(i);

					PlayerToD3DVertList(monster_list[i].model_id,
					                    monster_list[i].current_frame, (int)monster_list[i].rot_angle,
					                    monster_list[i].skin_tex_id,
					                    USE_PLAYERS_SKIN, nextFrame);

					for (int q = 0; q < countmodellist; q++) {

						if (strcmp(model_list[q].name, monster_list[i].rname) == 0) {
							getgunid = q;
							break;
						}
					}

					if (strcmp(model_list[getgunid].monsterweapon, "NONE") != 0 && monster_list[i].bIsPlayerAlive == TRUE) {

						PlayerToD3DVertList(FindModelID(model_list[getgunid].monsterweapon),
						                    monster_list[i].current_frame, (int)monster_list[i].rot_angle,
						                    FindGunTexture(model_list[getgunid].monsterweapon),
						                    USE_PLAYERS_SKIN, nextFrame);
					}
				} else {
				}
			}
		}
	}
}

void CMyD3DApplication::DrawItems() {
	BOOL use_player_skins_flag = false;
	int cullflag = 0;
	for (int i = 0; i < itemlistcount; i++) {

		if (item_list[i].bIsPlayerValid == TRUE) {

			cullflag = 0;
			for (int cullloop = 0; cullloop < monstercount; cullloop++) {
				if (monstercull[cullloop] == item_list[i].monsterid) {
					cullflag = 1;
					break;
				}
			}

			if (item_list[i].monsterid == 9999 && item_list[i].bIsPlayerAlive == TRUE)
				cullflag = 1;

			if (cullflag == 1) {
				wx = item_list[i].x;
				wy = item_list[i].y;
				wz = item_list[i].z;

				D3DVECTOR work1;
				work1.x = wx;
				work1.y = wy;
				work1.z = wz;
				int monsteron = 0;

				if (perspectiveview == 0)
					monsteron = CalculateView(realEye, work1, 20.0f);
				else
					monsteron = CalculateView(realEye, work1, 20.0f);
				if (monsteron) {

					use_player_skins_flag = 1;
					current_frame = item_list[i].current_frame;

					int mtexlookup;

					if (strcmp(item_list[i].rname, "COIN") == 0) {

						if (maingameloop)
							item_list[i].rot_angle = fixangle(item_list[i].rot_angle, +10.0f);
						PlayerToD3DVertList(item_list[i].model_id,
						                    item_list[i].current_frame, (int)item_list[i].rot_angle,
						                    1,
						                    USE_DEFAULT_MODEL_TEX);
					} else if (strcmp(item_list[i].rname, "KEY2") == 0) {

						if (maingameloop)
							item_list[i].rot_angle = fixangle(item_list[i].rot_angle, +10.0f);
						PlayerToD3DVertList(item_list[i].model_id,
						                    item_list[i].current_frame, (int)item_list[i].rot_angle,
						                    1,
						                    USE_DEFAULT_MODEL_TEX);
					} else if (strcmp(item_list[i].rname, "AXE") == 0 ||
					           strcmp(item_list[i].rname, "FLAMESWORD") == 0 ||
					           strcmp(item_list[i].rname, "BASTARDSWORD") == 0 ||
					           strcmp(item_list[i].rname, "BATTLEAXE") == 0 ||
					           strcmp(item_list[i].rname, "ICESWORD") == 0 ||
					           strcmp(item_list[i].rname, "MORNINGSTAR") == 0 ||
					           strcmp(item_list[i].rname, "SPIKEDFLAIL") == 0 ||
					           strcmp(item_list[i].rname, "SPLITSWORD") == 0 ||
					           strcmp(item_list[i].rname, "SUPERFLAMESWORD") == 0 ||
					           strcmp(item_list[i].rname, "LIGHTNINGSWORD") == 0

					) {

						mtexlookup = pCMyApp->FindGunTexture(item_list[i].rname);

						PlayerToD3DVertList(item_list[i].model_id,
						                    item_list[i].current_frame, (int)item_list[i].rot_angle,
						                    mtexlookup,
						                    USE_DEFAULT_MODEL_TEX);
					} else if (strcmp(item_list[i].rname, "POTION") == 0) {

						PlayerToD3DVertList(item_list[i].model_id,
						                    item_list[i].current_frame, (int)item_list[i].rot_angle,
						                    1,
						                    USE_DEFAULT_MODEL_TEX);
					} else {

						int displayitem = 1;
						if (strcmp(item_list[i].rname, "spiral") == 0) {
							displayitem = 0;
						}
						if (displayitem == 1) {

							PlayerToD3DVertList(item_list[i].model_id,
							                    item_list[i].current_frame, (int)item_list[i].rot_angle,
							                    1,
							                    USE_DEFAULT_MODEL_TEX);
						}
					}
				}
			}
		}
	}
}

void CMyD3DApplication::OpenChest() {
	int cullflag = 0;
	BOOL use_player_skins_flag = false;
	float qdist = 0.0f;

	for (int i = 0; i < num_players2; i++) {

		if (player_list2[i].bIsPlayerValid == TRUE) {

			cullflag = 0;
			for (int cullloop = 0; cullloop < monstercount; cullloop++) {
				if (monstercull[cullloop] == player_list2[i].monsterid) {
					cullflag = 1;
					break;
				}
			}

			if (cullflag == 1) {

				wx = player_list2[i].x;
				wy = player_list2[i].y;
				wz = player_list2[i].z;

				D3DVECTOR work1;
				work1.x = wx;
				work1.y = wy;
				work1.z = wz;
				int monsteron = 0;

				if (perspectiveview == 0)
					monsteron = CalculateView(realEye, work1, 20.0f);
				else
					monsteron = CalculateView(realEye, work1, 20.0f);
				if (monsteron) {

					use_player_skins_flag = 1;
					current_frame = player_list2[i].current_frame;

					if (perspectiveview == 0) {
						qdist = FastDistance(m_vLookatPt.x - player_list2[i].x,
						                     m_vLookatPt.y - player_list2[i].y,
						                     m_vLookatPt.z - player_list2[i].z);
					} else {
						qdist = FastDistance(m_vEyePt.x - player_list2[i].x,
						                     m_vEyePt.y - player_list2[i].y,
						                     m_vEyePt.z - player_list2[i].z);
					}

					if (player_list2[i].model_id == 36 && pressopendoor && qdist <= 100.0f ||
					    strstr(player_list2[i].rname, "cdoorclosed") != NULL && pressopendoor && qdist <= 100.0f) {

						player_list2[i].model_id--;
						strcpy_s(player_list2[i].rname, "-");

						PlayWavSound(SoundID("creak1"), 100);

						int gd = 0;

						if (player_list2[i].ability == 1) {
							gd = random_num(100) + 50;
						} else if (player_list2[i].ability == 2) {
							gd = random_num(500) + 500;
						} else if (player_list2[i].ability == 0) {
							gd = 0;
						}

						AddTreasure(player_list2[i].x, player_list2[i].y + 20.0f, player_list2[i].z, gd);
					}

					PlayerToD3DVertList(player_list2[i].model_id,
					                    player_list2[i].current_frame, (int)player_list2[i].rot_angle,
					                    player_list2[i].skin_tex_id,
					                    USE_DEFAULT_MODEL_TEX);
				}
			}
		}
	}
}

void CMyD3DApplication::DrawPlayers() {

	int cullflag = 0;
	BOOL use_player_skins_flag = false;
	int getgunid = 0;

	for (int i = 0; i < num_players; i++) {
		if (player_list[i].bIsPlayerValid == TRUE) {
			cullflag = 0;
			for (int cullloop = 0; cullloop < monstercount; cullloop++) {

				if (monstercull[cullloop] == (int)i + (int)999) {
					cullflag = 1;
					break;
				}
			}

			if (cullflag == 1 || i == trueplayernum) {

				wx = player_list[i].x;
				wy = player_list[i].y;
				wz = player_list[i].z;

				use_player_skins_flag = 1;
				current_frame = player_list[i].current_frame;

				if (player_list[i].bIsPlayerInWalkMode == TRUE) {
					int angle = 360 - (int)player_list[i].rot_angle + 90;

					if (perspectiveview == 1 && trueplayernum == i || trueplayernum == i && drawsphere) {
					} else {

						int nextFrame = GetNextFramePlayer();

						if (player_list[i].bStopAnimating == TRUE) {
							PlayerToD3DVertList(player_list[i].model_id,
							                    player_list[i].current_frame, angle,
							                    player_list[i].skin_tex_id,
							                    USE_DEFAULT_MODEL_TEX);

						} else {
							PlayerToD3DVertList(player_list[i].model_id,
							                    player_list[i].current_frame, angle,
							                    player_list[i].skin_tex_id,
							                    USE_DEFAULT_MODEL_TEX, nextFrame);
						}

						if (trueplayernum != i &&
						    player_list[i].bIsPlayerAlive

						) {
							wx = player_list[i].x;
							wy = player_list[i].y;
							wz = player_list[i].z;

							int gun_angle;
							gun_angle = -(int)player_list[i].rot_angle + (int)90;

							if (gun_angle >= 360)
								gun_angle = gun_angle - 360;
							if (gun_angle < 0)
								gun_angle = gun_angle + 360;

							nextFrame = GetNextFramePlayer();

							if (player_list[i].model_id != 0) {

								for (int q = 0; q < countmodellist; q++) {
									if (model_list[q].model_id == player_list[i].model_id) {
										getgunid = q;
										break;
									}
								}

								if (strcmp(model_list[getgunid].monsterweapon, "NONE") != 0 && player_list[i].bIsPlayerAlive == TRUE) {

									PlayerToD3DVertList(FindModelID(model_list[getgunid].monsterweapon),
									                    player_list[i].current_frame, (int)gun_angle,
									                    FindGunTexture(model_list[getgunid].monsterweapon),
									                    USE_PLAYERS_SKIN, nextFrame);
								}
							} else {
								PlayerToD3DVertList(player_list[i].gunid,
								                    player_list[i].current_frame, gun_angle,
								                    player_list[i].guntex,
								                    USE_DEFAULT_MODEL_TEX, nextFrame);
							}
						}
					}
				}
			}
		}
	}
}

void CMyD3DApplication::MonsterInRange() {
	for (int i = 0; i < num_players; i++) {
		if (player_list[i].bIsPlayerValid == TRUE && i != trueplayernum) {

			float qdist = FastDistance(
			    player_list[trueplayernum].x - player_list[i].x,
			    player_list[trueplayernum].y - player_list[i].y,
			    player_list[trueplayernum].z - player_list[i].z);

			player_list[i].dist = qdist;
			//					qdist=0;

			if (qdist < (numberofsquares * monsterdist) / 2) {
				wx = player_list[i].x;
				wy = player_list[i].y;
				wz = player_list[i].z;

				D3DVECTOR work2, work1;

				work1.x = player_list[i].x;
				work1.y = player_list[i].y;
				work1.z = player_list[i].z;
				int monsteron;

				monsteron = SceneInBox(work1);

				if (monsteron) {

					monstertype[monstercount] = 5;
					monsterobject[monstercount] = player_list[i].model_id;
					monsterangle[monstercount] = (int)player_list[i].rot_angle;
					monstercull[monstercount] = (int)i + (int)999;
					monstercount++;
				}
			}
		}
	}
}

void CMyD3DApplication::WakeUpMonsters() {
	int ob_type = 0;
	int monsteron = 0;
	float qdist = 0.0f;
	int angle = 0;

	for (int q = 0; q < pCMyApp->oblist_length; q++) {

		wx = oblist[q].x;
		wy = oblist[q].y;
		wz = oblist[q].z;

		angle = (int)oblist[q].rot_angle;
		ob_type = oblist[q].type;

		if (ob_type == 6) {

			qdist = FastDistance(
			    player_list[trueplayernum].x - wx,
			    player_list[trueplayernum].y - wy,
			    player_list[trueplayernum].z - wz);

			if (qdist <= 1600.0f)
				ObjectToD3DVertList(ob_type, angle, q);
		}

		int montry = 0;

		if (strcmp(oblist[q].name, "!monster1") == 0 && oblist[q].monstertexture > 0 && monsterenable == 1) {

			for (montry = 0; montry < num_monsters; montry++) {
				if (oblist[q].monsterid == monster_list[montry].monsterid) {
					qdist = FastDistance(
					    player_list[trueplayernum].x - monster_list[montry].x,
					    player_list[trueplayernum].y - monster_list[montry].y,
					    player_list[trueplayernum].z - monster_list[montry].z);

					monster_list[montry].dist = qdist;

					if (qdist < (numberofsquares * monsterdist) / 2) {
						wx = monster_list[montry].x;
						wy = monster_list[montry].y;
						wz = monster_list[montry].z;

						D3DVECTOR work2, work1;

						work1.x = monster_list[montry].x;
						work1.y = monster_list[montry].y;
						work1.z = monster_list[montry].z;

						monsteron = SceneInBox(work1);

						if (monsteron) {
							monstertype[monstercount] = 0;
							monsterobject[monstercount] = (int)monster_list[montry].model_id;
							monsterangle[monstercount] = (int)monster_list[montry].rot_angle;
							monstercull[monstercount] = oblist[q].monsterid;
							monstercount++;
							monster_list[montry].volume = 100 - (int)((100 * qdist) / ((numberofsquares * monsterdist) / 2));
							if (monster_list[montry].volume > 100)
								monster_list[montry].volume = 100;

							if (monster_list[montry].volume < 10)
								monster_list[montry].volume = 10;
						}

						break;
					}
				}
			}
		}

		if (strcmp(oblist[q].name, "!monster1") == 0 && oblist[q].monstertexture == -1 && monsterenable == 1) {

			// 3ds monster
			for (montry = 0; montry < itemlistcount; montry++) {
				if (oblist[q].monsterid == item_list[montry].monsterid && item_list[montry].bIsPlayerAlive == TRUE) {
					qdist = FastDistance(
					    player_list[trueplayernum].x - item_list[montry].x,
					    player_list[trueplayernum].y - item_list[montry].y,
					    player_list[trueplayernum].z - item_list[montry].z);

					item_list[montry].dist = qdist;

					if (qdist < (numberofsquares * monsterdist) / 2) {
						wx = item_list[montry].x;
						wy = item_list[montry].y;
						wz = item_list[montry].z;

						D3DVECTOR work2, work1;

						work1.x = item_list[montry].x;
						work1.y = item_list[montry].y;
						work1.z = item_list[montry].z;

						monsteron = SceneInBox(work1);
						if (monsteron) {
							monstertype[monstercount] = 2;
							monsterobject[monstercount] = item_list[montry].model_id;
							monsterangle[monstercount] = (int)item_list[montry].rot_angle;
							monstercull[monstercount] = oblist[q].monsterid;
							monstercount++;
						}

						break;
					}
				}
			}
		}

		if (strcmp(oblist[q].name, "!monster1") == 0 && oblist[q].monstertexture == 0 && monsterenable == 1) {

			for (montry = 0; montry < num_players2; montry++) {
				if (oblist[q].monsterid == player_list2[montry].monsterid) {
					qdist = FastDistance(
					    player_list[trueplayernum].x - player_list2[montry].x,
					    player_list[trueplayernum].y - player_list2[montry].y,
					    player_list[trueplayernum].z - player_list2[montry].z);
					player_list2[montry].dist = qdist;

					if (qdist < (numberofsquares * monsterdist) / 2 || outside == 1) {
						wx = player_list2[montry].x;
						wy = player_list2[montry].y;
						wz = player_list2[montry].z;

						D3DVECTOR work2, work1;

						work1.x = player_list2[montry].x;
						work1.y = player_list2[montry].y;
						work1.z = player_list2[montry].z;
						int monsteron;

						monsteron = SceneInBox(work1);

						if (monsteron) {
							monstertype[monstercount] = 1;
							monsterobject[monstercount] = player_list2[montry].model_id;
							monsterangle[monstercount] = (int)player_list2[montry].rot_angle;
							monstercull[monstercount] = oblist[q].monsterid;
							monstercount++;
						}
						break;
					}
				}
			}
		}
	}
}

void CMyD3DApplication::DrawMissle() {
	int ob_type = 0;

	for (int misslecount = 0; misslecount < MAX_MISSLE; misslecount++) {

		if (your_missle[misslecount].active == 1) {
			wx = your_missle[misslecount].x;
			wy = your_missle[misslecount].y;
			wz = your_missle[misslecount].z;

			ob_type = your_missle[misslecount].model_id;
			current_frame = your_missle[misslecount].current_frame;
			int angle = (int)your_missle[misslecount].rot_angle;

			PlayerToD3DVertList(ob_type,
			                    current_frame,
			                    angle,
			                    your_missle[misslecount].skin_tex_id,
			                    USE_DEFAULT_MODEL_TEX);
		}
	}
}

void CMyD3DApplication::DetermineItem(int icnt, int &cell_x, int &cell_z, D3DVIEWPORT7 &vp) {
	int oblist_index = 0;
	float qdist = 0;
	int ob_type = 0;
	int angle = 0;

	number_of_polys_per_frame = 0;
	cnt = 0;
	int scount = 0;

	int vol_length = icnt;
	monstercount = 0;
	g_ob_vert_count = 0;
	for (int vi = 0; vi < vol_length; vi++) {
		cell_x = vol_x[vi];
		cell_z = vol_z[vi];

		if (scount >= numberofsquares) {
			scount = 0;
			bSkipThisCell = TRUE;
		} else {
			bSkipThisCell = FALSE;
		}
		scount++;

		// check that this cell is within the map limits
		if (((cell_x >= 0) && (cell_z >= 0)) &&
		    ((cell_x < 200) && (cell_z < 200))) {
			// check if there is a list of map objects allocated to this cell
			if (cell[cell_x][cell_z] != NULL) {
				// get the number of objects which need to be drawn
				// for this cell
				clength = cell_length[cell_x][cell_z];

				for (int q = 0; q < clength; q++) {
					// get the object's map ID
					oblist_index = cell[cell_x][cell_z][q];

					if ((oblist_index >= 0) && (oblist_index < oblist_length)) {
						// check map object list and prevent the same
						// objects being drawn more than once

						if (oblist_overdraw_flags[oblist_index] == FALSE) {
							if (bSkipThisCell == FALSE)
								oblist_overdraw_flags[oblist_index] = TRUE;

							wx = oblist[oblist_index].x;
							wy = oblist[oblist_index].y;
							wz = oblist[oblist_index].z;

							D3DVECTOR work2, work1;

							work1.x = oblist[oblist_index].x;
							work1.y = oblist[oblist_index].y;
							work1.z = oblist[oblist_index].z;
							int monsteron = 0;

							monsteron = SceneInBox(work1);
							if (monsteron) {

								drawthistri = 1;
							} else {

								drawthistri = 0;
							}

							objectcollide = 1;
							objectscroll = 0;
							if (strstr(oblist[oblist_index].name, "nohit") != NULL) {
								objectcollide = 0;
							}
							if (strstr(oblist[oblist_index].name, "scroll") != NULL) {
								objectscroll = 1;
							}

							angle = (int)oblist[oblist_index].rot_angle;
							ob_type = oblist[oblist_index].type;

							int displayobject = 1;

							if (strcmp(oblist[oblist_index].name, "!monster1") != 0 && strcmp(oblist[oblist_index].name, "lamp_post") != 0 && strcmp(oblist[oblist_index].name, "startpos") != 0) {
								if (strstr(oblist[oblist_index].name, "fog") != NULL) {

									qdist = FastDistance(
									    m_vLookatPt.x - wx,
									    0.0f,
									    m_vLookatPt.z - wz);
									if (qdist < 500.0f) {
										float fStart = (float)1000.0;
										float fEnd = 1600.0;

										// Enable fog blending.
										m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);

										// Set the fog color.
										m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0, 0, 0));

										// Set fog parameters.
										m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_NONE);
										m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
										m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fStart));
										m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD *)(&fEnd));
									}
								}
								if (strstr(oblist[oblist_index].name, "text") != NULL) {

									qdist = FastDistance(
									    m_vLookatPt.x - wx,
									    0.0f,
									    m_vLookatPt.z - wz);
									displayobject = 0;
									if (qdist < 500.0f) {

										for (int il = 0; il < textcounter; il++) {
											if (gtext[il].textnum == oblist_index) {

												if (gtext[il].type == 0) {
													strcpy_s(gfinaltext, gtext[il].text);
												} else if (gtext[il].type == 1) {

													if (qdist < 200.0f) {

														DisplayDialogText(gtext[il].text, 0.0f);
													}
												} else if (gtext[il].type == 3) {
													// box
													if (qdist < 200.0f && gtext[il].shown == 0) {
														// box

														if (dialogpause == 0)
															PlayWavSound(SoundID("dmspeak"), 100);
														dialognum = il;
														dialogpause = 1;
														display_box_calc((FLOAT)vp.dwWidth / 2 - 120.0f, (FLOAT)vp.dwHeight / 2 - 30.0f, gtext[il].text, vp, 255, 255, 255, 295.0f, 120.0f, 2);
														display_box((FLOAT)vp.dwWidth - (FLOAT)(vp.dwWidth / 4.0f), 40.0f, gtext[il].text, vp, 255, 255, 255, (FLOAT)vp.dwWidth - 10.0f, (FLOAT)vp.dwHeight / 4, 3);
														DisplayDialogText(gtext[il].text, 0.0f);
													} else {
														// dialogpause=0;
													}
												} else if (gtext[il].type == 2) {

													if (qdist < 200.0f && gtext[il].shown == 0) {

														DisplayDialogText(gtext[il].text, 0.0f);
														strcpy_s(screentext, gtext[il].text);
														gtext[il].shown = 1;
														ScreenTimeLast = ScreenTime; // Get current time in seconds
														showscreentimer = 1;
													}
												}
											}
										}
									}
								}

								if (strstr(oblist[oblist_index].name, "door") != NULL) {
									// door

									qdist = FastDistance(
									    m_vLookatPt.x - wx,
									    m_vLookatPt.y - wy,
									    m_vLookatPt.z - wz);
									OpenDoor(oblist_index, qdist);
								}

								if (strstr(oblist[oblist_index].name, "wallfloor") != NULL) {

									qdist = FastDistance(
									    m_vLookatPt.x - wx,
									    m_vLookatPt.y - wy,
									    m_vLookatPt.z - wz);
									// closest sound

									if (qdist < 400.0f) {
										if (qdist < closesoundid[0])
											closesoundid[0] = qdist;
									}
								}
								if (strstr(oblist[oblist_index].name, "lava") != NULL) {

									qdist = FastDistance(
									    m_vLookatPt.x - wx,
									    m_vLookatPt.y - wy,
									    m_vLookatPt.z - wz);
									// closest sound

									if (qdist < 400.0f) {
										if (qdist < closesoundid[2])
											closesoundid[2] = qdist;
									}
								}
								if (strstr(oblist[oblist_index].name, "flare") != NULL) {

									qdist = FastDistance(
									    m_vLookatPt.x - wx,
									    m_vLookatPt.y - wy,
									    m_vLookatPt.z - wz);
									// closest sound

									if (qdist < 400.0f) {
										if (qdist < closesoundid[4])
											closesoundid[4] = qdist;
									}
								}

								if (strstr(oblist[oblist_index].name, "flame") != NULL) {

									qdist = FastDistance(
									    m_vLookatPt.x - wx,
									    m_vLookatPt.y - wy,
									    m_vLookatPt.z - wz);
									// closest sound

									if (qdist < 400.0f) {
										if (qdist < closesoundid[1])
											closesoundid[1] = qdist;
									}
								}

								if (strcmp(oblist[oblist_index].name, "startpos") == 0) {
									displayobject = 0;
								}

								if (displayobject == 1)
									ObjectToD3DVertList(ob_type, angle, oblist_index);
							}
						}
					}
				} // end for q
			} // end if
		} // end if
	} // end for vi
}

void CMyD3DApplication::ComputeCells(int lookside, int &icnt, int cell_x, int cell_z) {

	// Work out which direction player is facing in
	// i.e north, east, south, or west
	// and store the numbers of the cells which are in view.
	// DungeonStomp works out what cells are visible by simply
	// looking at a rectangular 5 x 6 block of cells directly
	// infront of the player.

	int sqrloop1 = 0;
	int comparedot1 = 0;

	if ((angy >= 315) || (angy <= 45)) // player facing North
	{
		comparedot1 = 0;
		for (int ix = -lookside; ix <= lookside; ix++) {

			for (sqrloop1 = 0; sqrloop1 <= numberofsquares; sqrloop1++) {
				vol_x[icnt + sqrloop1] = cell_x + ix;
				vol_z[icnt + sqrloop1] = cell_z + sqrloop1 - (numberofsquares / 2);
			}
			icnt += numberofsquares;
		}
	}

	if ((angy > 225) && (angy < 315)) // player facing West
	{

		comparedot1 = 1;
		for (int iz = -lookside; iz <= lookside; iz++) {

			for (sqrloop1 = 0; sqrloop1 <= numberofsquares; sqrloop1++) {
				vol_x[icnt + sqrloop1] = cell_x + (numberofsquares / 2) - sqrloop1;
				vol_z[icnt + sqrloop1] = cell_z + iz;
			}
			icnt += numberofsquares;
		}
	}

	if ((angy >= 135) && (angy <= 225)) // player facing South
	{
		comparedot1 = 2;
		for (int ix = -lookside; ix <= lookside; ix++) {

			for (sqrloop1 = 0; sqrloop1 <= numberofsquares; sqrloop1++) {

				vol_x[icnt + sqrloop1] = cell_x + ix;
				vol_z[icnt + sqrloop1] = cell_z + (numberofsquares / 2) - sqrloop1;
			}
			icnt += numberofsquares;
		}
	}

	if ((angy > 45) && (angy < 135)) // player facing East
	{
		comparedot1 = 3;
		for (int iz = -lookside; iz <= lookside; iz++) {

			for (sqrloop1 = 0; sqrloop1 <= numberofsquares; sqrloop1++) {
				vol_x[icnt + sqrloop1] = cell_x - (sqrloop1 - numberofsquares / 2);
				vol_z[icnt + sqrloop1] = cell_z + iz;
			}

			icnt += numberofsquares;
		}
	}
}

int CMyD3DApplication::CheckValidTextureAlias(HWND hwnd, char *alias) {
	int i;
	char *buffer2;

	for (i = 0; i < number_of_tex_aliases; i++) {
		buffer2 = TexMap[i].tex_alias_name;

		if (_strcmpi(buffer2, alias) == 0) {
			return i;
		}
	}
	MessageBox(hwnd, alias, NULL, MB_OK);

	return -1; // error
}

int CMyD3DApplication::FindTextureAlias(char *alias) {
	int i;
	char *buffer2;

	for (i = 0; i < number_of_tex_aliases; i++) {
		buffer2 = TexMap[i].tex_alias_name;

		if (_strcmpi(buffer2, alias) == 0) {
			return i;
		}
	}

	return -1; // error
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects() {
	// Size the background
	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);
	ASPECT_RATIO = ((FLOAT)vp.dwHeight) / vp.dwWidth;
	m_pBackground[0].sy = (FLOAT)((vp.dwHeight / 2.0) + 30.0);
	m_pBackground[2].sy = (FLOAT)((vp.dwHeight / 2.0) + 30.0);
	m_pBackground[2].sx = (FLOAT)vp.dwWidth;
	m_pBackground[3].sx = (FLOAT)vp.dwWidth;

	viewportheight = (FLOAT)vp.dwHeight;
	viewportwidth = (FLOAT)vp.dwWidth;

	flare[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 1, 0.0f, 0.6f);
	flare[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 1, 0.0f, 0.0f);
	flare[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 1, 1.0f, 0.6f);
	flare[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5, -1, 1, 1.0f, 0.0f);

	flare[0].sy = (FLOAT)((vp.dwHeight) + 30.0);
	flare[2].sy = (FLOAT)((vp.dwHeight) + 30.0);
	flare[2].sx = (FLOAT)vp.dwWidth;
	flare[3].sx = (FLOAT)vp.dwWidth;

	// Create and set up the object material
	D3DMATERIAL7 mtrl;

	D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.dcvSpecular.r = 0.3f;
	mtrl.dcvSpecular.g = 0.3f;
	mtrl.dcvSpecular.b = 0.3f;
	mtrl.dcvSpecular.a = 0.0f;

	mtrl.power = 40.0f;
	m_pd3dDevice->SetMaterial(&mtrl);

	// Set the transform matrices
	D3DUtil_SetIdentityMatrix(matWorld);
	D3DUtil_SetProjectionMatrix(matProj, 1.57f, ASPECT_RATIO, 1.0f, 900.0f);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

	// Turn on lighting. Light will be set during FrameMove() call
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, bEnableLighting);

	if (!pCWorld->LoadRRTextures(m_hWnd, "textures.dat"))
		PrintMessage(m_hWnd, "LoadRRTextures failed", NULL, LOGFILE_ONLY);

	// Set miscellaneous renderstates
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);

	// Set texture renderstates
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);

	// D3DTFG_POINT        = 1
	// D3DTFG_LINEAR       = 2
	// D3DTFG_FLATCUBIC    = 3
	// D3DTFG_GAUSSIANCUBIC= 4
	// D3DTFG_ANISOTROPIC  = 5

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup() {
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects() {
	D3DTextr_InvalidateAllTextures();
	return S_OK;
}

void PrintMessage(HWND hwnd, char *message1, char *message2, int message_mode) {

	FILE *fp;
	char tmessage[100];

	if ((message1 == NULL) && (message2 == NULL))
		return;

	if ((message1 != NULL) && (message2 != NULL)) {
		strcpy_s(tmessage, message1);
		strcat_s(tmessage, message2);
	} else {
		if (message1 != NULL)
			strcpy_s(tmessage, message1);

		if (message2 != NULL)
			strcpy_s(tmessage, message2);
	}

	if (logfile_start_flag == TRUE) {
		if (fopen_s(&fp, "ds.log", "w") != 0) {
		}

		fprintf(fp, "%s\n\n", "RR Logfile");
	} else {
		if (fopen_s(&fp, "ds.log", "a") != 0) {
		}
	}

	logfile_start_flag = FALSE;

	if (fp == NULL) {
		MessageBox(hwnd, "Can't load logfile", "File Error", MB_OK);
		fclose(fp);
		return;
	}

	char buf[500];
	time_t current_time = time(NULL);
	struct tm *t = localtime(&current_time);

	strcpy_s(buf, "");
	if (strftime(buf, 100, "%b-%d-%Y-%H-%M-%S ", t) <= 0)
		strcpy_s(buf, "");

	strcat_s(buf, tmessage);
	// fprintf( fp, " %s\n", tmessage );
	fprintf(fp, " %s\n", buf);

	if (message_mode != LOGFILE_ONLY && pCMyApp->turnoffscreentext == 0) {
		hdc = GetDC(hwnd);

		SetTextColor(hdc, RGB(255, 255, 0));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, PrintMsgX, PrintMsgY, tmessage, strlen(tmessage));
		PrintMsgY += 20;
		ReleaseDC(hwnd, hdc);
	}

	fclose(fp);
}

char *_itoa(int x) {
	_itoa_s(x, itoabuffer, _countof(itoabuffer), 10);
	return itoabuffer;
}

void PrintMemAllocated(int mem, char *message) {
	FILE *fp;
	char buffer[100];
	char buffer2[100];
	int mem_kb;

	if (logfile_start_flag == TRUE) {
		if (fopen_s(&fp, "ds.log", "w") != 0) {
		}

		fprintf(fp, "%s\n\n", "RR Logfile");
	} else {
		if (fopen_s(&fp, "ds.log", "a") != 0) {
		}
	}

	logfile_start_flag = FALSE;

	if (fp == NULL) {
		MessageBox(NULL, "Can't load logfile", "File Error", MB_OK);
		fclose(fp);
		return;
	}

	strcpy_s(buffer, "memory allocated for ");
	strcat_s(buffer, message);
	strcat_s(buffer, " = ");

	mem_kb = mem / 1024;
	_itoa_s(mem_kb, buffer2, _countof(buffer2), 10);
	strcat_s(buffer, buffer2);
	strcat_s(buffer, " KB");

	fprintf(fp, " %s\n", buffer);

	total_allocated_memory_count += mem;

	fclose(fp);
}

HRESULT CMyD3DApplication::AnimateGuns() {

	int startframe;
	int currframe;
	int stopframe;
	int modid;
	int skipamount = 3;
	int curr_seq;

	modid = your_gun[current_gun].model_id;

	currframe = your_gun[current_gun].current_frame;
	stopframe = pmdata[modid].sequence_stop_frame[your_gun[current_gun].current_sequence];
	startframe = pmdata[modid].sequence_start_frame[your_gun[current_gun].current_sequence];

	if (your_gun[current_gun].current_sequence == 2) {
		skipamount = 1;
	}

	if (animationdirgun == 0) {
		if (currframe >= stopframe) {
			your_gun[current_gun].current_sequence = 0;
			curr_seq = your_gun[current_gun].current_sequence;
			your_gun[current_gun].current_frame = pmdata[modid].sequence_stop_frame[curr_seq];
			animationdirgun = 1;
		} else {
			animategunslow++;
			if (animategunslow >= skipamount) {
				your_gun[current_gun].current_frame++;
				animategunslow = 0;
			}
		}
	} else {

		if (currframe <= startframe) {
			your_gun[current_gun].current_sequence = 0;
			curr_seq = your_gun[current_gun].current_sequence;
			your_gun[current_gun].current_frame = pmdata[modid].sequence_start_frame[curr_seq];
			animationdirgun = 0;
		} else {

			animategunslow++;
			if (animategunslow >= skipamount) {
				your_gun[current_gun].current_frame--;
				animategunslow = 0;
			}
		}
	}

	return 0;
}

HRESULT CMyD3DApplication::AnimateCharacters() {
	int i;
	int startframe;
	int curr_frame;
	int stop_frame;
	int curr_seq;
	static LONGLONG fLastTimeRun = 0;
	static LONGLONG fLastTimeRunLast = 0;

	// TODO:
	int jump = 0;

	// Only take damge from one swing
	/*if (player_list[trueplayernum].current_frame == 52) {
	    for (i = 0; i < num_monsters; i++) {
	        monster_list[i].takedamageonce = 0;
	    }
	}*/

	for (int i = 0; i < 1; i++) {

		int mod_id = player_list[i].model_id;
		curr_frame = player_list[i].current_frame;
		stop_frame = pmdata[mod_id].sequence_stop_frame[player_list[i].current_sequence];
		startframe = pmdata[mod_id].sequence_start_frame[player_list[i].current_sequence];
		if (player_list[i].bStopAnimating == FALSE) {

			// if (player_list[i].animationdir == 0)
			//{
			if (curr_frame >= stop_frame) {
				curr_seq = player_list[i].current_sequence;

				player_list[i].animationdir = 1;

				if (player_list[i].current_frame == 71) {
					if (i == trueplayernum) {
						// current player
						jump = 0;

						if (runflag == 1 && jump == 0) {
							SetPlayerAnimationSequence(i, 1);
						} else if (runflag == 1 && jump == 1) {
						} else {
							SetPlayerAnimationSequence(i, 0);
						}
					} else {

						// SetPlayerAnimationSequence(i, 0);
					}
				}

				player_list[i].current_frame = pmdata[mod_id].sequence_start_frame[curr_seq];

				if (i == trueplayernum && curr_seq == 1 && runflag == 1) {
				} else {
					if (curr_seq == 0 || curr_seq == 1 || curr_seq == 6) {
						if (i == trueplayernum && curr_seq == 0 && jump == 0) {

							// TODO:Remove
							SetPlayerAnimationSequence(i, 0);

							int raction = random_num(8);

							// if (raction == 0)
							//	SetPlayerAnimationSequence(i, 7);// flip
							// else if (raction == 1)
							//	SetPlayerAnimationSequence(i, 8);// Salute
							// else if (raction == 2)
							//	SetPlayerAnimationSequence(i, 9);// Taunt
							// else if (raction == 3)
							//	SetPlayerAnimationSequence(i, 10);// Wave
							// else if (raction == 4)
							//	SetPlayerAnimationSequence(i, 11); // Point
							// else
							//	SetPlayerAnimationSequence(i, 0);
						}
					} else {
						if (runflag == 1 && i == trueplayernum && jump == 0) {
							SetPlayerAnimationSequence(i, 1);
						} else {
							if (i == trueplayernum && jump == 1) {
							} else {
								SetPlayerAnimationSequence(i, 0);
							}
						}
					}
				}
			} else {
				player_list[i].current_frame++;
			}
			//}
			// else
			//{
			//	if (curr_frame <= startframe)
			//	{
			//		curr_seq = player_list[i].current_sequence;
			//		player_list[i].current_frame = pmdata[mod_id].sequence_start_frame[curr_seq];
			//		player_list[i].animationdir = 0;
			//	}
			//	else
			//	{
			//		player_list[i].current_frame--;
			//	}
			//}
		}

		if (player_list[i].current_frame == 183 || player_list[i].current_frame == 189 || player_list[i].current_frame == 197) {
			// player is dead
			player_list[i].bStopAnimating = TRUE;
		}
	}

	GetItem();
	MonsterHit();

	for (i = 0; i < num_monsters; i++) {
		int mod_id = monster_list[i].model_id;
		curr_frame = monster_list[i].current_frame;
		stop_frame = pmdata[mod_id].sequence_stop_frame[monster_list[i].current_sequence];
		startframe = pmdata[mod_id].sequence_start_frame[monster_list[i].current_sequence];
		if (monster_list[i].bStopAnimating == FALSE) {
			if (monster_list[i].animationdir == 0) {
				if (curr_frame >= stop_frame) {
					curr_seq = monster_list[i].current_sequence;
					monster_list[i].current_frame = pmdata[mod_id].sequence_stop_frame[curr_seq];
					// monster_list[i].animationdir = 1;

					SetMonsterAnimationSequence(i, 0);

					if (monster_list[i].current_frame == 183 || monster_list[i].current_frame == 189 || monster_list[i].current_frame == 197) {
						monster_list[i].bStopAnimating = TRUE;
					}
				} else {
					if (monster_list[i].current_frame != 183 || monster_list[i].current_frame != 189 || monster_list[i].current_frame != 197) {
						monster_list[i].current_frame++;
					}
				}
			} else {
				if (curr_frame <= startframe) {
					curr_seq = monster_list[i].current_sequence;
					monster_list[i].current_frame = pmdata[mod_id].sequence_start_frame[curr_seq];
					monster_list[i].animationdir = 0;
				} else {
					monster_list[i].current_frame--;
				}
			}
		}
	}

	for (i = 0; i < itemlistcount; i++) {
		item_list[i].current_frame++;

		if (item_list[i].current_frame > 91)
			item_list[i].current_frame = 87;
	}

	return 0;
}

void CMyD3DApplication::SetPlayerAnimationSequence(int player_number, int sequence_number) {
	int model_id;
	int start_frame;

	if (player_list[trueplayernum].bIsPlayerAlive == FALSE && player_number == trueplayernum)
		return;

	if (player_list[player_number].bStopAnimating == TRUE)
		return;

	player_list[player_number].current_sequence = sequence_number;
	model_id = player_list[player_number].model_id;
	player_list[player_number].animationdir = 0;

	start_frame = pmdata[model_id].sequence_start_frame[sequence_number];

	if (start_frame == 66) {
		start_frame = 70;
		player_list[player_number].animationdir = 1;
	}
	player_list[player_number].current_frame = start_frame;
}

void CMyD3DApplication::SetMonsterAnimationSequence(int player_number, int sequence_number) {
	int model_id;
	int start_frame;

	// turned on again for mulitplayer this is a problem getting a sequence out of order

	if (monster_list[player_number].bIsPlayerValid == FALSE)
		return;

	monster_list[player_number].current_sequence = sequence_number;

	model_id = monster_list[player_number].model_id;

	monster_list[player_number].animationdir = 0;
	start_frame = pmdata[model_id].sequence_start_frame[sequence_number];
	monster_list[player_number].current_frame = start_frame;
}

void CMyD3DApplication::DisplayScores() {
	int i;
	int offset_y = 140;
	HDC hdc;
	char buf[256];
	char buf2[256];
	HRESULT LastError;

	if (display_scores == FALSE)
		return;

	if (GetFramework()->GetBackBuffer() == NULL)
		return;

	LastError = GetFramework()->GetBackBuffer()->GetDC(&hdc);

	if (LastError == DD_OK) {
		SetTextColor(hdc, RGB(255, 255, 0));
		SetBkColor(hdc, RGB(0, 0, 0));
		SetBkMode(hdc, TRANSPARENT);

		TextOut(hdc, 50, offset_y, "Scores", 6);
		offset_y += 20;

		TextOut(hdc, 50, offset_y, "No.", 3);
		TextOut(hdc, 90, offset_y, "Name", 4);
		TextOut(hdc, 200, offset_y, "Frags", 5);
		TextOut(hdc, 300, offset_y, "RRnetID", 7);
		TextOut(hdc, 380, offset_y, "Ping", 4);

		offset_y += 20;

		for (i = 50; i < 410; i += 10)
			TextOut(hdc, i, offset_y, "--", 2);

		offset_y += 20;

		i = 0;

		for (i = 0; i < num_players; i++) {
			if (player_list[i].bIsPlayerValid == TRUE) {
				_itoa_s(i, buf, _countof(buf), 10);
				TextOut(hdc, 50, offset_y, buf, strlen(buf));

				strcpy_s(buf, player_list[i].name);
				TextOut(hdc, 90, offset_y, buf, strlen(buf));

				_itoa_s(player_list[i].frags, buf, 256, 10);
				TextOut(hdc, 200, offset_y, buf, strlen(buf));

				_itoa_s((int)player_list[i].RRnetID, buf, _countof(buf), 10);
				TextOut(hdc, 300, offset_y, buf, strlen(buf));

				_itoa_s(player_list[i].ping, buf, 256, 10);
				TextOut(hdc, 380, offset_y, buf, strlen(buf));

				offset_y += 20;
			}
		}
		offset_y += 20;

		for (i = 50; i < 410; i += 10)
			TextOut(hdc, i, offset_y, "--", 2);

		strcpy_s(buf, "Rx = ");
		_itoa_s(num_packets_received_ps, buf2, _countof(buf2), 10);
		strcat_s(buf, buf2);
		strcat_s(buf, "     Tx = ");
		_itoa_s(num_packets_sent_ps, buf2, _countof(buf2), 10);
		strcat_s(buf, buf2);

		offset_y += 20;
		TextOut(hdc, 50, offset_y, buf, strlen(buf));
	} else
		PrintMessage(NULL, "DisplayScores() - GetDC FAILED", NULL, NULL);

	GetFramework()->GetBackBuffer()->ReleaseDC(hdc);
}

void CMyD3DApplication::ClearObjectList() {

	int cell_z;
	int cell_x;
	int q = 0;

	for (q = 0; q < pCMyApp->oblist_length; q++) {

		delete oblist[q].light_source;
	}
	delete oblist;
	oblist = new OBJECTLIST[3000];

	for (cell_z = 0; cell_z < 200; cell_z++) {
		for (cell_x = 0; cell_x < 200; cell_x++) {

			if (pCMyApp->cell[cell_x][cell_z] != NULL) {
				delete pCMyApp->cell[cell_x][cell_z];
				pCMyApp->cell[cell_x][cell_z] = NULL;
			}
		}
	}

	for (int i = 0; i < MAX_NUM_ITEMS; i++) {
		item_list[i].frags = 0;
		item_list[i].x = 500;
		item_list[i].y = 22;
		item_list[i].z = 500;
		item_list[i].rot_angle = 0;
		item_list[i].model_id = 0;
		item_list[i].skin_tex_id = 0;
		item_list[i].bIsFiring = FALSE;
		item_list[i].ping = 0;
		item_list[i].health = 5;
		item_list[i].bIsPlayerAlive = TRUE;
		item_list[i].bStopAnimating = FALSE;
		item_list[i].current_weapon = 0;
		item_list[i].current_car = 0;
		item_list[i].current_frame = 0;
		item_list[i].current_sequence = 2;
		item_list[i].bIsPlayerInWalkMode = TRUE;
		item_list[i].RRnetID = 0;
		item_list[i].bIsPlayerValid = FALSE;
		item_list[i].animationdir = 0;
		item_list[i].gold = 0;
		strcpy_s(item_list[i].name, "");
		strcpy_s(item_list[i].chatstr, "5");
		strcpy_s(item_list[i].name, "Dungeon Stomp");
	}
}

int CMyD3DApplication::collisiondetection(int i) {

	float xp[4];
	float yp[4];

	int result = 0;

	i = 0;

	if (mx[3] == 0 && my[3] == 0 && mz[3] == 0) {
		xp[0] = mx[i + 3];
		xp[0] = mx[i + 1];
		xp[1] = mx[i];
		xp[2] = mx[i + 2];
		yp[0] = mz[i + 3];
		yp[0] = mz[i + 1];
		yp[1] = mz[i];
		yp[2] = mz[i + 2];

		result = 0;
		result = pnpoly(3, xp, yp, model_x, model_z);
	} else {
		xp[0] = mx[i + 3];
		xp[1] = mx[i + 1];
		xp[2] = mx[i];
		xp[3] = mx[i + 2];
		yp[0] = mz[i + 3];
		yp[1] = mz[i + 1];
		yp[2] = mz[i];
		yp[3] = mz[i + 2];

		result = pnpoly(4, xp, yp, model_x, model_z);
	}

	if (result) {
		planex = mx[i];
		planey = my[i];
		planez = mz[i];

		planex2 = mx[i + 1];
		planey2 = my[i + 1];
		planez2 = mz[i + 1];

		planex3 = mx[i + 2];
		planey3 = my[i + 2];
		planez3 = mz[i + 2];

		calculate_y_location();

		if (calcy > model_y) {
			// above me
			calcy = lastmodely;
		} else {
			// below me

			if (calcy >= lastmodely) {

				lastmodely = calcy;
			} else {

				calcy = lastmodely;
			}
		}
	}

	return 1;
}

HRESULT CreateSphere(D3DVERTEX **ppVertices, DWORD *pdwNumVertices,
                     WORD **ppIndices, DWORD *pdwNumIndices,
                     FLOAT fRadius, DWORD dwNumRings) {
	// Allocate memory for the vertices and indices
	// Allocate memory for the vertices and indices
	DWORD dwNumVertices = (dwNumRings * (2 * dwNumRings + 1) + 2);
	DWORD dwNumIndices = 6 * (dwNumRings * 2) * ((dwNumRings - 1) + 1);
	D3DVERTEX *pVertices = new D3DVERTEX[dwNumVertices];
	WORD *pIndices = new WORD[dwNumIndices];

	(*ppVertices) = pVertices;
	(*ppIndices) = pIndices;

	// Counters
	WORD x, y, vtx = 0, index = 0;

	// Angle deltas for constructing the sphere's vertices
	FLOAT fDAng = g_PI / dwNumRings;
	FLOAT fDAngY0 = fDAng;

	// Make the middle of the sphere
	for (y = 0; y < dwNumRings; y++) {
		FLOAT y0 = (FLOAT)cos(fDAngY0);
		FLOAT r0 = (FLOAT)sin(fDAngY0);
		FLOAT tv = (1.0f - y0) / 2;

		for (x = 0; x < (dwNumRings * 2) + 1; x++) {
			FLOAT fDAngX0 = x * fDAng;

			D3DVECTOR v(r0 * (FLOAT)sin(fDAngX0), y0, r0 * (FLOAT)cos(fDAngX0) * -1);

			//            FLOAT tu = 1.0f - x/(dwNumRings*2.0f);

			FLOAT tu = 1.0f - x / (dwNumRings / 2.0f);

			*pVertices++ = D3DVERTEX(fRadius * v, v, tu, tv);
			vtx++;
		}
		fDAngY0 += fDAng;
	}

	for (y = 0; y < dwNumRings - 1; y++) {
		for (x = 0; x < (dwNumRings * 2); x++) {
			*pIndices++ = (WORD)((y + 0) * (dwNumRings * 2 + 1) + (x + 0));
			*pIndices++ = (WORD)((y + 1) * (dwNumRings * 2 + 1) + (x + 0));
			*pIndices++ = (WORD)((y + 0) * (dwNumRings * 2 + 1) + (x + 1));
			*pIndices++ = (WORD)((y + 0) * (dwNumRings * 2 + 1) + (x + 1));
			*pIndices++ = (WORD)((y + 1) * (dwNumRings * 2 + 1) + (x + 0));
			*pIndices++ = (WORD)((y + 1) * (dwNumRings * 2 + 1) + (x + 1));
			index += 6;
		}
	}
	// Make top and bottom

	D3DVECTOR vy(0.0f, 1.0f, 0.0f);
	WORD wSouthVtx;
	WORD wNorthVtx = vtx;

	*pVertices++ = D3DVERTEX(fRadius * vy, vy, 0.5f, 0.0f);
	vtx++;
	wSouthVtx = vtx;
	*pVertices++ = D3DVERTEX(-fRadius * vy, -vy, 0.5f, 1.0f);
	vtx++;

	for (x = 0; x < (dwNumRings * 2); x++) {
		WORD p1 = wSouthVtx;
		WORD p2 = (WORD)((y) * (dwNumRings * 2 + 1) + (x + 1));
		WORD p3 = (WORD)((y) * (dwNumRings * 2 + 1) + (x + 0));

		*pIndices++ = p1;
		*pIndices++ = p3;
		*pIndices++ = p2;
		index += 3;
	}
	for (x = 0; x < (dwNumRings * 2); x++) {
		WORD p1 = wNorthVtx;
		WORD p2 = (WORD)((0) * (dwNumRings * 2 + 1) + (x + 1));
		WORD p3 = (WORD)((0) * (dwNumRings * 2 + 1) + (x + 0));

		*pIndices++ = p1;
		*pIndices++ = p3;
		*pIndices++ = p2;
		index += 3;
	}

	(*pdwNumVertices) = vtx;
	(*pdwNumIndices) = index;

	return S_OK;
}

int CMyD3DApplication::pnpoly(int npol, float *xp, float *yp, float x, float y) {
	int i, j, c = 0;

	for (i = 0, j = npol - 1; i < npol; j = i++) {
		if ((((yp[i] <= y) && (y < yp[j])) ||
		     ((yp[j] <= y) && (y < yp[i]))) &&
		    (x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))

			c = !c;
	}
	return c;
}

void CMyD3DApplication::calculate_y_location() {

	// this is intense!  Calculate the intersection of a line and a polygon
	// in 3d space ... so i can adjust the Y location

	D3DVECTOR vDiff, vDiff2;
	D3DVECTOR normroad;
	D3DVECTOR normroadold;
	D3DVECTOR normroad3;
	D3DVECTOR result;
	D3DVECTOR vw1, vw2, vw3;
	D3DVECTOR final3;

	// Line intersects a plane formula is:

	// x = p + (a-p) *N
	//         -------- *V
	//            V*N

	// a     PLANE
	normroadold.x = planex;
	normroadold.y = planey;
	normroadold.z = planez;

	// p    LINE
	normroad.x = model_x;
	normroad.y = 10000;
	normroad.z = model_z;

	normroad3.x = model_x;
	normroad3.y = -10000;
	normroad3.z = model_z;

	vDiff2 = normroad - normroad3;  // V
	vDiff = normroadold - normroad; // a-p

	vw1.x = planex;
	vw1.y = planey;
	vw1.z = planez;

	vw2.x = planex2;
	vw2.y = planey2;
	vw2.z = planez2;

	vw3.x = planex3;
	vw3.y = planey3;
	vw3.z = planez3;

	D3DVECTOR vDiffwork = vw1 - vw2;
	D3DVECTOR vDiffwork2 = vw3 - vw2;
	D3DVECTOR vCross = CrossProduct(vDiffwork, vDiffwork2);
	final3 = Normalize(vCross);

	float fDot = DotProduct(vDiff, final3);

	float fDot2 = DotProduct(vDiff2, final3);

	result = normroad + ((fDot / fDot2) * vDiff2);
	calcy = result.y;
}

void CMyD3DApplication::ObjectCollision() {

	float centroidx;
	float centroidy;
	float centroidz;

	float qdist = 0;
	int i = 0;
	int count = 0;

	calcy = 0;
	lastmodely = -9999;
	foundcollision = FALSE;
	nearestDistance = -99;
	int vertcount = 0;
	int vertnum = 0;

	colPack2.foundCollision = false;
	colPack2.nearestDistance = 10000000;
	D3DVECTOR realpos;

	vertnum = verts_per_poly[vertcount];

	for (i = 0; i < g_ob_vert_count; i++) {
		if (count == 0 && src_collide[i] == 1) {

			mxc[0] = src_v[i].x;
			myc[0] = src_v[i].y;
			mzc[0] = src_v[i].z;

			mxc[1] = src_v[i + 1].x;
			myc[1] = src_v[i + 1].y;
			mzc[1] = src_v[i + 1].z;

			mxc[2] = src_v[i + 2].x;
			myc[2] = src_v[i + 2].y;
			mzc[2] = src_v[i + 2].z;

			//  3 2
			//  1 0

			centroidx = (mxc[0] + mxc[1] + mxc[2]) * 0.3333333333333f;
			centroidy = (myc[0] + myc[1] + myc[2]) * 0.3333333333333f;
			centroidz = (mzc[0] + mzc[1] + mzc[2]) * 0.3333333333333f;
			qdist = FastDistance(collisionPackage.realpos.x - centroidx,
			                     collisionPackage.realpos.y - centroidy,
			                     collisionPackage.realpos.z - centroidz);

			if (qdist < collisiondist) {
				calculate_block_location();
			}
			if (vertnum == 4) {
				mxc[0] = src_v[i + 1].x;
				myc[0] = src_v[i + 1].y;
				mzc[0] = src_v[i + 1].z;

				mxc[1] = src_v[i + 3].x;
				myc[1] = src_v[i + 3].y;
				mzc[1] = src_v[i + 3].z;

				mxc[2] = src_v[i + 2].x;
				myc[2] = src_v[i + 2].y;
				mzc[2] = src_v[i + 2].z;

				centroidx = (mxc[0] + mxc[1] + mxc[2]) * 0.3333333333333f;
				centroidy = (myc[0] + myc[1] + myc[2]) * 0.3333333333333f;
				;
				centroidz = (mzc[0] + mzc[1] + mzc[2]) * 0.3333333333333f;
				qdist = FastDistance(collisionPackage.realpos.x - centroidx,
				                     collisionPackage.realpos.y - centroidy,
				                     collisionPackage.realpos.z - centroidz);

				if (qdist < collisiondist)
					calculate_block_location();
			}
		}
		count++;
		if (count > vertnum - 1) {
			count = 0;
			vertcount++;
			vertnum = verts_per_poly[vertcount];
		}
	}

	vertnum = 3;
	count = 0;

	count = 0;
	vertnum = 4;

	for (i = 0; i < countboundingbox; i++) {

		if (count == 0) {
			mxc[0] = boundingbox[i].x;
			myc[0] = boundingbox[i].y;
			mzc[0] = boundingbox[i].z;

			mxc[1] = boundingbox[i + 1].x;
			myc[1] = boundingbox[i + 1].y;
			mzc[1] = boundingbox[i + 1].z;

			mxc[2] = boundingbox[i + 2].x;
			myc[2] = boundingbox[i + 2].y;
			mzc[2] = boundingbox[i + 2].z;

			//  3 2
			//  1 0

			centroidx = (mxc[0] + mxc[1] + mxc[2]) * 0.3333333333333f;
			centroidy = (myc[0] + myc[1] + myc[2]) * 0.3333333333333f;
			centroidz = (mzc[0] + mzc[1] + mzc[2]) * 0.3333333333333f;

			qdist = FastDistance(collisionPackage.realpos.x - centroidx,
			                     collisionPackage.realpos.y - centroidy,
			                     collisionPackage.realpos.z - centroidz);

			if (qdist < collisiondist + 200.0f)
				calculate_block_location();

			if (vertnum == 4) {
				mxc[0] = boundingbox[i + 1].x;
				myc[0] = boundingbox[i + 1].y;
				mzc[0] = boundingbox[i + 1].z;

				mxc[1] = boundingbox[i + 3].x;
				myc[1] = boundingbox[i + 3].y;
				mzc[1] = boundingbox[i + 3].z;

				mxc[2] = boundingbox[i + 2].x;
				myc[2] = boundingbox[i + 2].y;
				mzc[2] = boundingbox[i + 2].z;

				centroidx = (mxc[0] + mxc[1] + mxc[2]) * 0.3333333333333f;
				centroidy = (myc[0] + myc[1] + myc[2]) * 0.3333333333333f;

				centroidz = (mzc[0] + mzc[1] + mzc[2]) * 0.3333333333333f;
				qdist = FastDistance(collisionPackage.realpos.x - centroidx,
				                     collisionPackage.realpos.y - centroidy,
				                     collisionPackage.realpos.z - centroidz);

				if (qdist < collisiondist + 200.0f)
					calculate_block_location();
			}
		}
		count++;
		if (count > vertnum - 1) {
			count = 0;
		}
	}
}

void CMyD3DApplication::calculate_block_location() {

	TCollisionPacket colPackage;
	// plane data

	D3DVECTOR p1, p2, p3;
	D3DVECTOR pNormal;
	D3DVECTOR pOrigin;
	D3DVECTOR v1, v2;
	D3DVECTOR source;

	D3DVECTOR velocity;

	int nohit = 0;

	p1.x = mxc[0] / eRadius.x;
	p1.y = myc[0] / eRadius.y;
	p1.z = mzc[0] / eRadius.z;

	p2.x = mxc[1] / eRadius.x;
	p2.y = myc[1] / eRadius.y;
	p2.z = mzc[1] / eRadius.z;

	p3.x = mxc[2] / eRadius.x;
	p3.y = myc[2] / eRadius.y;
	p3.z = mzc[2] / eRadius.z;

	// check embedded

	VECTOR pp1;
	VECTOR pp2;
	VECTOR pp3;

	pp1.x = p1.x;
	pp1.y = p1.y;
	pp1.z = p1.z;

	pp2.x = p2.x;
	pp2.y = p2.y;
	pp2.z = p2.z;

	pp3.x = p3.x;
	pp3.y = p3.y;
	pp3.z = p3.z;

	VECTOR vel;
	VECTOR pos;

	checkTriangle(&collisionPackage, pp1, pp2, pp3);

	return;
}

float FastDistance(float fx, float fy, float fz) {

	int temp;
	int x, y, z;

	x = (int)fabs(fx) * 1024;
	y = (int)fabs(fy) * 1024;
	z = (int)fabs(fz) * 1024;
	if (y < x) {
		temp = x;
		x = y;
		y = temp;
	}
	if (z < y) {
		temp = y;
		y = z;
		z = temp;
	}
	if (y < x) {
		temp = x;
		x = y;
		y = temp;
	}
	int dist = (z + 11 * (y >> 5) + (x >> 2));
	return ((float)(dist >> 10));
}

void SWAP(int &x, int &y, int &temp) {

	temp = x;
	x = y;
	y = temp;
}

HRESULT CreateSphere2(D3DVERTEX **ppVertices,
                      DWORD *pdwNumVertices,
                      WORD **ppIndices, DWORD *pdwNumIndices,
                      FLOAT fRadius, DWORD dwNumRings) {
	// Allocate memory for the vertices and indices
	DWORD dwNumVertices = (dwNumRings * (2 * dwNumRings + 1) + 2);
	DWORD dwNumIndices = 6 * (dwNumRings * 2) * ((dwNumRings - 1) + 1);
	D3DVERTEX *pVertices = new D3DVERTEX[dwNumVertices];
	WORD *pIndices = new WORD[dwNumIndices];

	(*ppVertices) = pVertices;
	(*ppIndices) = pIndices;

	// Counters
	WORD x, y, vtx = 0, index = 0;

	// Angle deltas for constructing the sphere's vertices
	FLOAT fDAng = g_PI / dwNumRings;
	FLOAT fDAngY0 = fDAng;

	// Make the middle of the sphere
	for (y = 0; y < dwNumRings; y++) {
		FLOAT y0 = (FLOAT)cos(fDAngY0);
		FLOAT r0 = (FLOAT)sin(fDAngY0);
		FLOAT tv = (1.0f - y0) / 2;

		for (x = 0; x < (dwNumRings * 2) + 1; x++) {
			FLOAT fDAngX0 = x * fDAng;

			D3DVECTOR v(r0 * (FLOAT)sin(fDAngX0), y0 * 2.0f, r0 * (FLOAT)cos(fDAngX0));
			FLOAT tu = 1.0f - x / (dwNumRings * 2.0f);

			*pVertices++ = D3DVERTEX(fRadius * v, v, tu, tv);
			vtx++;
		}
		fDAngY0 += fDAng;
	}

	for (y = 0; y < dwNumRings - 1; y++) {
		for (x = 0; x < (dwNumRings * 2); x++) {
			*pIndices++ = (WORD)((y + 0) * (dwNumRings * 2 + 1) + (x + 0));
			*pIndices++ = (WORD)((y + 1) * (dwNumRings * 2 + 1) + (x + 0));
			*pIndices++ = (WORD)((y + 0) * (dwNumRings * 2 + 1) + (x + 1));
			*pIndices++ = (WORD)((y + 0) * (dwNumRings * 2 + 1) + (x + 1));
			*pIndices++ = (WORD)((y + 1) * (dwNumRings * 2 + 1) + (x + 0));
			*pIndices++ = (WORD)((y + 1) * (dwNumRings * 2 + 1) + (x + 1));
			index += 6;
		}
	}
	// Make top and bottom
	// vy*2 to fix sphere bottoms
	D3DVECTOR vy(0.0f, 1.0f, 0.0f);
	WORD wNorthVtx = vtx;
	*pVertices++ = D3DVERTEX(fRadius * vy * 2, vy, 0.5f, 0.0f);
	vtx++;
	WORD wSouthVtx = vtx;
	*pVertices++ = D3DVERTEX(-fRadius * vy * 2, -vy, 0.5f, 1.0f);
	vtx++;

	for (x = 0; x < (dwNumRings * 2); x++) {
		WORD p1 = wSouthVtx;
		WORD p2 = (WORD)((y) * (dwNumRings * 2 + 1) + (x + 1));
		WORD p3 = (WORD)((y) * (dwNumRings * 2 + 1) + (x + 0));

		*pIndices++ = p1;
		*pIndices++ = p3;
		*pIndices++ = p2;
		index += 3;
	}

	for (x = 0; x < (dwNumRings * 2); x++) {
		WORD p1 = wNorthVtx;
		WORD p2 = (WORD)((0) * (dwNumRings * 2 + 1) + (x + 1));
		WORD p3 = (WORD)((0) * (dwNumRings * 2 + 1) + (x + 0));

		*pIndices++ = p1;
		*pIndices++ = p3;
		*pIndices++ = p2;
		index += 3;
	}

	(*pdwNumVertices) = vtx;
	(*pdwNumIndices) = index;

	return S_OK;
}

D3DVECTOR CMyD3DApplication::collideWithWorld(D3DVECTOR position, D3DVECTOR velocity) {

	// SILENCERS
	D3DVECTOR final;

	// All hard-coded distances in this function is
	// scaled to fit the setting above..
	float unitsPerMeter = 100.0f;
	float unitScale = unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.005f * unitScale;

	// do we need to worry?
	if (collisionRecursionDepth > 15) {
		collisionRecursionDepth = 0;
		return position;
	}

	// Ok, we need to worry:

	VECTOR vel;
	vel.x = (float)velocity.x;
	vel.y = (float)velocity.y;
	vel.z = (float)velocity.z;

	VECTOR pos;

	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;

	collisionPackage.velocity = vel;
	collisionPackage.normalizedVelocity = vel;
	collisionPackage.normalizedVelocity.normalize();
	collisionPackage.velocityLength = vel.length();
	collisionPackage.basePoint = pos;
	collisionPackage.foundCollision = false;
	collisionPackage.nearestDistance = 10000000;
	collisionPackage.realpos.x = pos.x * eRadius.x;
	collisionPackage.realpos.y = pos.y * eRadius.y;
	collisionPackage.realpos.z = pos.z * eRadius.z;

	// Check for collision (calls the collision routines)
	// Application specific!!

	ObjectCollision();
	// If no collision we just move along the velocity
	if (collisionPackage.foundCollision == false) {
		final.x = pos.x + vel.x;
		final.y = pos.y + vel.y;
		final.z = pos.z + vel.z;
		collisionRecursionDepth = 0;
		return final;
	}

	if (collisionPackage.nearestDistance == 0.0f) {
		// quick fix for an embedded object - this needs a lot of work this area here we should un embed ourselves simply by looping through objects and push out if we are embedded, i will fix this one day mark my words , i will fix it and i will be quite pleased about that such thing

		final.x = pos.x;
		final.y = pos.y;
		final.z = pos.z;
		collisionRecursionDepth = 0;
		return final;
	}

	// *** Collision occured ***
	// The original destination point
	VECTOR destinationPoint = pos + vel;
	VECTOR newSourcePoint = pos;

	VECTOR V = vel;

	/*
	    //added by silencer ver 2.0 new untested unconfirmed
	    if(collisionPackage.nearestDistance==0.0f){
	        final.x = pos.x + vel.x;
	        final.y = pos.y + vel.y;
	        final.z = pos.z + vel.z;
	        return final;
	    }
	*/

	V.SetLength(collisionPackage.nearestDistance);

	newSourcePoint = collisionPackage.basePoint + V;

	VECTOR slidePlaneNormal = newSourcePoint - collisionPackage.intersectionPoint;

	// fixed by tele forgot to normalize slideplane
	slidePlaneNormal.normalize();

	// silencer ver 1.0
	// VECTOR displacementVector=slidePlaneNormal * veryCloseDistance;

	// silencer ver 2.0 - i think it fixed it can u believe it 2 years and these 5 lines did it.
	float factor;

	V.SetLength(veryCloseDistance);

	factor = veryCloseDistance / (V.x * slidePlaneNormal.x + V.y * slidePlaneNormal.y + V.z * slidePlaneNormal.z);

	// VECTOR V2;
	// V2.SetLength(1);
	// V=V2;

	V.SetLength(1);

	VECTOR displacementVector = V * veryCloseDistance * factor;

	if ((V.x * slidePlaneNormal.x + V.y * slidePlaneNormal.y + V.z * slidePlaneNormal.z) != 0.0f) {
		newSourcePoint = newSourcePoint + displacementVector;
		collisionPackage.intersectionPoint = collisionPackage.intersectionPoint + displacementVector;
	}

	// Determine the sliding plane
	VECTOR slidePlaneOrigin = collisionPackage.intersectionPoint;

	PLANE slidingPlane(slidePlaneOrigin, slidePlaneNormal);

	// Again, sorry about formatting.. but look carefully ;)

	VECTOR newDestinationPoint = destinationPoint - slidePlaneNormal * slidingPlane.signedDistanceTo(destinationPoint);

	// Generate the slide vector, which will become our new
	// velocity vector for the next iteration

	VECTOR newVelocityVector = newDestinationPoint - collisionPackage.intersectionPoint;

	// Recurse:
	// dont recurse if the new velocity is very small

	if (newVelocityVector.length() < veryCloseDistance)

	{
		final.x = newSourcePoint.x;
		final.y = newSourcePoint.y;
		final.z = newSourcePoint.z;
		collisionRecursionDepth = 0;

		return final;
	}

	collisionRecursionDepth++;

	D3DVECTOR newP;
	D3DVECTOR newV;

	newP.x = newSourcePoint.x;
	newP.y = newSourcePoint.y;
	newP.z = newSourcePoint.z;

	newV.x = newVelocityVector.x;
	newV.y = newVelocityVector.y;
	newV.z = newVelocityVector.z;

	return collideWithWorld(newP, newV);
}

void CMyD3DApplication::UpdateMainPlayer() {

	int i;

	int gun_angle;
	gun_angle = -(int)angy + (int)90;

	if (gun_angle >= 360)
		gun_angle = gun_angle - 360;
	if (gun_angle < 0)
		gun_angle = gun_angle + 360;

	if (pCMyApp->multiplay_flag) {

		for (i = 0; i < pCMyApp->num_players; i++) {
			if (pCMyApp->player_list[i].RRnetID == pCMyApp->MyRRnetID) {
				player_list[i].x = modellocation.x;
				player_list[i].y = modellocation.y;
				player_list[i].z = modellocation.z;
				player_list[i].rot_angle = angy;
				player_list[i].bIsPlayerValid = TRUE;
				player_list[i].skin_tex_id = currentskinid;
				player_list[i].model_id = currentmodelid;
				player_list[i].gunangle = gun_angle;
				player_list[i].gunx = modellocation.x;
				player_list[i].guny = modellocation.y;
				player_list[i].gunz = modellocation.z;
				player_list[i].draw_external_wep = FALSE;
				trueplayernum = i;
				GunTruesave = GunTrue;
			}
		}
	} else {
		num_players = 1;

		player_list[0].x = modellocation.x;
		player_list[0].y = modellocation.y;
		player_list[0].z = modellocation.z;
		player_list[0].skin_tex_id = currentskinid;
		player_list[0].rot_angle = angy;
		player_list[0].bIsPlayerValid = TRUE;
		player_list[0].draw_external_wep = FALSE;
		player_list[0].model_id = currentmodelid;
		player_list[0].gunangle = gun_angle;
		player_list[0].gunx = modellocation.x;
		player_list[0].guny = modellocation.y;
		player_list[0].gunz = modellocation.z;

		GunTruesave = GunTrue;
		trueplayernum = 0;
	}
}

void CMyD3DApplication::OnKeyDown(WPARAM wParam) {

	if (openingscreen == 4) {
		if (wParam == 13) {
			if (ambientlighton == 0)
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, NULL);
			current_gun = 0;
			gravitytime = 0.0f;

			angy = saveangy;
			look_up_ang = 35.0f;
			perspectiveview = 0;
			m_vLookatPt = saveplocation;
			m_vLookatPt.y += 100.0f;
			m_vEyePt = m_vLookatPt;
			modellocation = m_vLookatPt;
			UpdateMainPlayer();

			PlayWavSound(SoundID("goal2"), 100);
			StartFlare(2);
			openingscreen = 0;
			usespell = 0;
			spellhiton = 0;
		}
	}
	if (openingscreen == 0) {

		if (!bInTalkMode) {
			if (wParam == 'v' || wParam == 'V') {
				SwitchView();
			}
		}

		if (wParam == VK_F3) {

			if (player_list[trueplayernum].bIsPlayerAlive == TRUE) {
				if (save_game("")) {
					strcpy_s(gActionMessage, "Saving game...");
					UpdateScrollList(0, 245, 255);
				} else {
					strcpy_s(gActionMessage, "Saving game aborted...");
					UpdateScrollList(0, 245, 255);
				}
			}
		}

		/*
		if (wParam == VK_F9)
		{
		    DDSURFACEDESC2 ddsd;
		    ddsd.dwSize = sizeof(DDSURFACEDESC2);
		    m_pddsRenderTarget->GetSurfaceDesc(&ddsd);
		    CopySurface(m_pddsRenderTarget, 0);
		}
		if (wParam == VK_F11)
		{
		    DDSURFACEDESC2 ddsd;
		    ddsd.dwSize = sizeof(DDSURFACEDESC2);
		    m_pddsRenderTarget->GetSurfaceDesc(&ddsd);
		    CopySurface(m_pddsRenderTarget, 1);
		}

		*/

		if (wParam == VK_F2) {

			if (load_game("")) {
				strcpy_s(gActionMessage, "Loading game...");
				UpdateScrollList(0, 245, 255);
			} else {
				strcpy_s(gActionMessage, "Load game aborted...");
				UpdateScrollList(0, 245, 255);
			}
		}
		listendoor = 0;
		if (!bInTalkMode) {
			if (wParam == 'x' || wParam == 'X') {

				if (betamode == 1) {
					player_list[trueplayernum].xp += 1000;
					LevelUp(player_list[trueplayernum].xp);
				}
			}

			if (wParam == 'H' || wParam == 'h') {

				if (betamode == 1)
					player_list[trueplayernum].health = player_list[trueplayernum].health + 5;
			}

			if (wParam == 'k' || wParam == 'K') {

				if (betamode == 1)
					player_list[trueplayernum].keys = player_list[trueplayernum].keys + 1;
			}

			if (wParam == 'P' || wParam == 'p') {
				if (showpanel == 0) {
					displaycap = 1;
					showpanel = 1;
				} else {
					displaycap = 0;
					showpanel = 0;
				}
			}

			if (wParam == 32 && merchantfound == 1) {

				saveangy = angy;
				saveplocation = m_vLookatPt;
				saveplocation = modellocation;
				merchanteye = m_vEyePt;
				merchantLook = m_vLookatPt;
				merchantview = perspectiveview;
				merchantangy = angy;

				m_vEyePt.x = 700;
				m_vEyePt.y = 160;
				m_vEyePt.z = 700;

				m_vLookatPt.x = 700;
				m_vLookatPt.y = 160;
				m_vLookatPt.z = 700;
				perspectiveview = 0;

				merchantmode = 0;
				merchantmodelid = currentmodelid;
				merchanttextureid = currentskinid;
				merchantmousebutton = 0;

				currentmodellist = 0;

				if (!pCWorld->LoadMerchantFiles(m_hWnd, "merchantbuy0.txt")) {
				}

				if (merchantlist[0].object == 1) {
					currentmodelid = FindModelID(merchantlist[0].Text1);
					currentskinid = FindGunTexture(merchantlist[0].Text1);
				} else {
					currentmodelid = FindModelID(merchantlist[0].Text1);
					currentskinid = model_list[currentmodelid].modeltexture;
				}

				openingscreen = 3;

				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x00f0f0f0);
			}
			if (wParam == 13 && dialogpause == 1 || wParam == 32 && dialogpause == 1) {

				gtext[dialognum].shown = 1;
				dialogpause = 0;
			}

			if (wParam == 'O' || wParam == 'o') {
				if (displaycap == 0) {

					displaycap = 1;
				} else {
					displaycap = 0;
				}
			}

			if (wParam == 'L' || wParam == 'l') {
				listendoor = 1;
			}
		}

		if ((wParam == VK_DELETE || wParam == 'z' || wParam == 'Z') && !bInTalkMode) {
			CyclePreviousWeapon();
		}

		if ((wParam == VK_INSERT || wParam == 'q' || wParam == 'Q') && !bInTalkMode) {

			CycleNextWeapon();
		}
	} else {

		if (wParam == VK_F2) {

			if (load_game("")) {
				strcpy_s(gActionMessage, "Loading game...");
				UpdateScrollList(0, 245, 255);
				openingscreen = 0;
				m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, NULL);
			} else {
				strcpy_s(gActionMessage, "Load game aborted...");
				UpdateScrollList(0, 245, 255);
			}
		}

		if (wParam == VK_LEFT && openingscreen == 4) {

			currentmodellist--;

			if (currentmodellist < 0)
				currentmodellist = countmodellist - 1;

			while (model_list[currentmodellist].mtype == 1) {

				currentmodellist--;
				if (currentmodellist < 0)
					currentmodellist = countmodellist - 1;
			}

			currentmodelid = model_list[currentmodellist].model_id;
			currentskinid = model_list[currentmodellist].modeltexture;
		}

		if (wParam == VK_RIGHT && openingscreen == 4) {

			currentmodellist++;

			if (currentmodellist >= countmodellist)
				currentmodellist = 0;

			while (model_list[currentmodellist].mtype == 1) {
				currentmodellist++;
				if (currentmodellist >= countmodellist)
					currentmodellist = 0;
			}

			currentmodelid = model_list[currentmodellist].model_id;
			currentskinid = model_list[currentmodellist].modeltexture;
		}

		if (wParam == VK_F4) {

			SendMessage(m_hWnd, WM_CLOSE, 0, 0);
		}

		if (wParam == 13 || wParam == VK_F1) {

			if (openingscreen == 1)
				NewGame();
		}

		/*
		if (wParam == VK_F9)
		{
		    DDSURFACEDESC2 ddsd;
		    ddsd.dwSize = sizeof(DDSURFACEDESC2);
		    m_pddsRenderTarget->GetSurfaceDesc(&ddsd);
		    CopySurface(m_pddsRenderTarget, 0);
		}
		*/
	}
}

void CMyD3DApplication::SwitchView() {
	if (perspectiveview == 0) {
		look_up_ang = 0.0f;
		m_vEyePt = m_vLookatPt;
		perspectiveview = 1;
		rotate_camera = 0;
	} else {
		look_up_ang = 30.0f;
		rotate_camera = 0;
		m_vLookatPt = m_vEyePt;
		perspectiveview = 0;
	}
}

void CMyD3DApplication::CyclePreviousWeapon() {

	int loopguns = current_gun - 1;

	if (loopguns < 0)
		loopguns = num_your_guns;

	while (your_gun[loopguns].active == 0) {
		loopguns--;
		if (loopguns < 0)
			loopguns = num_your_guns;
	}

	if (current_gun != loopguns)
		PlayWavSound(SoundID("switch"), 100);

	current_gun = loopguns;

	if (strstr(your_gun[current_gun].gunname, "SCROLL") != NULL) {
		usespell = 1;
	} else {

		usespell = 0;
	}
	player_list[trueplayernum].gunid = your_gun[current_gun].model_id;
	player_list[trueplayernum].guntex = your_gun[current_gun].skin_tex_id;
	player_list[trueplayernum].damage1 = your_gun[current_gun].damage1;
	player_list[trueplayernum].damage2 = your_gun[current_gun].damage2;

	if (strstr(your_gun[loopguns].gunname, "FLAME") != NULL ||
	    strstr(your_gun[loopguns].gunname, "ICE") != NULL ||
	    strstr(your_gun[loopguns].gunname, "LIGHTNINGSWORD") != NULL) {
		bIsFlashlightOn = TRUE;
		lighttype = 2;
	} else {
		lighttype = 0;
		bIsFlashlightOn = FALSE;
	}
	MakeDamageDice();
}

void CMyD3DApplication::CycleNextWeapon() {
	int loopguns = current_gun + 1;
	if (loopguns >= num_your_guns)
		loopguns = 0;

	while (your_gun[loopguns].active == 0) {
		loopguns++;
		if (loopguns >= num_your_guns)
			loopguns = 0;
	}

	if (current_gun != loopguns)
		PlayWavSound(SoundID("switch"), 100);

	current_gun = loopguns;

	if (strstr(your_gun[current_gun].gunname, "SCROLL") != NULL) {
		usespell = 1;
	} else {

		usespell = 0;
	}

	player_list[trueplayernum].gunid = your_gun[current_gun].model_id;
	player_list[trueplayernum].guntex = your_gun[current_gun].skin_tex_id;
	player_list[trueplayernum].damage1 = your_gun[current_gun].damage1;
	player_list[trueplayernum].damage2 = your_gun[current_gun].damage2;

	MakeDamageDice();

	if (strstr(your_gun[loopguns].gunname, "FLAME") != NULL ||
	    strstr(your_gun[loopguns].gunname, "ICE") != NULL ||
	    strstr(your_gun[loopguns].gunname, "LIGHTNINGSWORD") != NULL) {
		bIsFlashlightOn = TRUE;
		lighttype = 2;
	} else {
		lighttype = 0;
		bIsFlashlightOn = FALSE;
	}
}

void CMyD3DApplication::OnCharDown(WPARAM wParam) {

	if (player_list[trueplayernum].bIsPlayerAlive == FALSE) {
		if (wParam == 32) {
			perspectiveview = 1;
			look_up_ang = 0.0f;
			ResetDeadPlayer();
			SetStartSpot();

			sprintf_s(gActionMessage, "Welcome back from the dead...");
			UpdateScrollList(0, 245, 255);
		}
		return;
	}

	if (bInTalkMode)
		return;

	if (wParam == 49) {
		if (your_gun[0].active == 1) {
			SwitchGun(0);
			PlayWavSound(SoundID("switch"), 100);
		}
	}
	if (wParam == 50) {
		if (your_gun[1].active == 1) {

			PlayWavSound(SoundID("switch"), 100);
			SwitchGun(1);
		}
	}
	if (wParam == 51) {
		if (your_gun[2].active == 1) {
			SwitchGun(2);
			PlayWavSound(SoundID("switch"), 100);
		}
	}
	if (wParam == 52) {
		if (your_gun[3].active == 1) {
			SwitchGun(3);
			PlayWavSound(SoundID("switch"), 100);
		}
	}
	if (wParam == 53) {
		if (your_gun[4].active == 1) {
			SwitchGun(4);
			PlayWavSound(SoundID("switch"), 100);
		}
	}
	if (wParam == 54) {
		if (your_gun[5].active == 1) {
			SwitchGun(5);
			PlayWavSound(SoundID("switch"), 100);
		}
	}

	if (wParam == 55) {
		if (your_gun[6].active == 1) {
			SwitchGun(6);
			PlayWavSound(SoundID("switch"), 100);
		}
	}

	if (wParam == 56) {
		if (your_gun[7].active == 1) {
			SwitchGun(7);
			PlayWavSound(SoundID("switch"), 100);
		}
	}
	if (wParam == 57) {
		if (your_gun[8].active == 1) {
			SwitchGun(8);
			PlayWavSound(SoundID("switch"), 100);
		}
	}
	if (wParam == '0') {
		if (your_gun[9].active == 1) {
			SwitchGun(9);
			PlayWavSound(SoundID("switch"), 100);
		}
	}

	if (wParam == 'r' || wParam == 'R') {
		if (your_gun[18].active == 1) {
			SwitchGun(18);
			PlayWavSound(SoundID("switch"), 100);
			criticalhiton = 0;
		}
	}
	if (wParam == 't' || wParam == 'T') {
		if (your_gun[19].active == 1) {
			SwitchGun(19);
			PlayWavSound(SoundID("switch"), 100);
			criticalhiton = 0;
		}
	}
	if (wParam == 'y' || wParam == 'Y') {
		if (your_gun[20].active == 1) {
			SwitchGun(20);
			PlayWavSound(SoundID("switch"), 100);
			criticalhiton = 0;
		}
	}
	if (wParam == 'u' || wParam == 'U') {
		if (your_gun[21].active == 1) {
			SwitchGun(21);
			PlayWavSound(SoundID("switch"), 100);
			criticalhiton = 0;
		}
	}

	if (wParam == '-') {
		gammasetting = gammasetting - 0.01f;
		if (pCMyApp->gammasetting <= 0.01f)
			pCMyApp->gammasetting = 0.01f;

		SetGamma();
	}

	if (wParam == '+') {
		gammasetting = gammasetting + 0.01f;
		SetGamma();
	}
}

void CMyD3DApplication::FirePlayerMissle(float x, float y, float z, float angy, int owner, int shoot, D3DVECTOR velocity, float lookangy) {

	D3DVECTOR MissleMove;
	D3DVECTOR MissleVelocity;
	int misslecount = 0;
	int misslespot = 0;
	int gun_angle;

	if (shoot == 0) {
		gun_angle = -(int)angy + (int)90;

		if (gun_angle >= 360)
			gun_angle = gun_angle - 360;
		if (gun_angle < 0)
			gun_angle = gun_angle + 360;
	} else {

		gun_angle = (int)angy;
	}

	if (your_gun[current_gun].x_offset <= 0) {
		firemissle = 0;
	} else if (firemissle == 1 ||
	           shoot == 1) {

		firemissle = 0;

		MissleVelocity.x = 32.0f * sinf(angy * k);

		if (perspectiveview == 0) {

			MissleVelocity.y = 32.0f * sinf(0 * k);
		} else {

			float newangle = 0;
			newangle = fixangle(look_up_ang, 90);

			MissleVelocity.y = 32.0f * sinf(newangle * k);
		}

		MissleVelocity.z = 32.0f * cosf(angy * k);

		if (shoot == 1)
			MissleVelocity = velocity;

		for (misslecount = 0; misslecount < MAX_MISSLE; misslecount++) {

			if (your_missle[misslecount].active == 0) {

				misslespot = misslecount;
				break;
			}
		}

		float r = 30.0f;
		D3DVECTOR savevelocity;
		savevelocity.x = r * sinf(angy * k);
		if (perspectiveview == 1) {
			savevelocity.y = r * sinf(lookangy * k);
		} else {
			savevelocity.y = r * sinf(0.0f * k);
		}
		savevelocity.z = r * cosf(angy * k);

		if (perspectiveview == 1)
			savevelocity = MissleSave;

		if (strstr(your_gun[current_gun].gunname, "SCROLL-MAGICMISSLE") != NULL) {
			your_missle[misslespot].model_id = 103;
			your_missle[misslespot].skin_tex_id = 205;
		} else if (strstr(your_gun[current_gun].gunname, "SCROLL-FIREBALL") != NULL) {
			your_missle[misslespot].model_id = 104;
			your_missle[misslespot].skin_tex_id = 288;
		} else if (strstr(your_gun[current_gun].gunname, "SCROLL-LIGHTNING") != NULL) {
			your_missle[misslespot].model_id = 105;
			your_missle[misslespot].skin_tex_id = 278;
		} else if (strstr(your_gun[current_gun].gunname, "SCROLL-HEALING") != NULL) {
			if (player_list[trueplayernum].health < player_list[trueplayernum].hp) {

				int roll = 0;
				roll = random_num(8) + 1;

				int hp = (roll * (int)(player_list[trueplayernum].hd));
				player_list[trueplayernum].health = player_list[trueplayernum].health + hp;

				if (player_list[trueplayernum].health > player_list[trueplayernum].hp)
					player_list[trueplayernum].health = player_list[trueplayernum].hp;

				PlayWavSound(SoundID("potion"), 100);

				PlayWavSound(SoundID("chant"), 100);
				StartFlare(3);
				// update healing dice, no roll
				dice[1].roll = 0;
				sprintf_s(dice[1].name, "die8s%d", roll);

				sprintf_s(gActionMessage, "You heal by %d health", hp);
				UpdateScrollList(0, 245, 255);
				your_gun[current_gun].x_offset--;
				if (your_gun[current_gun].x_offset <= 0) {
					your_gun[current_gun].x_offset = 0;
					your_gun[current_gun].active = 0;
				}

				LevelUp(player_list[trueplayernum].xp);
			}
			return;
		}
		// set length to 1
		savevelocity = calculatemisslelength(savevelocity);
		your_missle[misslespot].current_frame = 0;
		your_missle[misslespot].current_sequence = 0;
		your_missle[misslespot].x = x;
		your_missle[misslespot].y = y + 30.0f;
		your_missle[misslespot].z = z;
		your_missle[misslespot].rot_angle = (float)gun_angle;
		your_missle[misslespot].velocity = savevelocity;
		your_missle[misslespot].active = 1;
		your_missle[misslespot].owner = (int)owner;

		your_missle[misslespot].playernum = (int)owner;
		your_missle[misslespot].playertype = (int)1;
		your_missle[misslespot].guntype = current_gun;

		lastgun = current_gun;

		int attackbonus = your_gun[lastgun].sattack;
		int damagebonus = your_gun[lastgun].sdamage;
		int weapondamage = your_gun[lastgun].damage2;
		int action;

		action = random_num(weapondamage) + 1;
		sprintf_s(dice[1].name, "%ss%d", dice[1].prefix, action);
		mdmg = action;

		your_missle[misslespot].dmg = action;
		your_gun[current_gun].x_offset--;

		if (your_gun[current_gun].x_offset <= 0) {
			your_gun[current_gun].x_offset = 0;
			your_gun[current_gun].active = 0;
		}
		float qdist = FastDistance(
		    player_list[trueplayernum].x - your_missle[misslespot].x,
		    player_list[trueplayernum].y - your_missle[misslespot].y,
		    player_list[trueplayernum].z - your_missle[misslespot].z);
		your_missle[misslespot].qdist = qdist;

		if (your_missle[misslespot].sexplode != 0) {
			DSound_Delete_Sound(your_missle[misslespot].sexplode);
			your_missle[misslespot].sexplode = 0;
		}
		if (your_missle[misslespot].smove != 0)
			DSound_Delete_Sound(your_missle[misslespot].smove);
		your_missle[misslespot].smove = 0;

		your_missle[misslespot].sexplode = DSound_Replicate_Sound(SoundID("explod2"));

		if (current_gun == 18)
			your_missle[misslespot].smove = DSound_Replicate_Sound(SoundID("spell1"));
		else if (current_gun == 19)
			your_missle[misslespot].smove = DSound_Replicate_Sound(SoundID("spell2"));
		if (current_gun == 20)
			your_missle[misslespot].smove = DSound_Replicate_Sound(SoundID("spell3"));

		num_your_missles++;

		dice[0].roll = 0;
		dice[1].roll = 1;

		PlayWavSound(your_missle[misslecount].smove, 100);
	}

	saveoldvelocity = savevelocity;

	int pspeed = (18 - player_list[trueplayernum].hd);
	if (pspeed < 6)
		pspeed = 6;

	if (player_list[trueplayernum].firespeed == pspeed / 2 && current_gun == lastgun) {
		int attackbonus = your_gun[lastgun].sattack;
		int damagebonus = your_gun[lastgun].sdamage;
		int weapondamage = your_gun[lastgun].damage2;

		sprintf_s(dice[1].name, "%ss%d", dice[1].prefix, mdmg);
		dice[1].roll = 0;
	}

	for (misslecount = 0; misslecount < MAX_MISSLE; misslecount++) {

		if (your_missle[misslecount].active == 1) {
			MissleMove.x = your_missle[misslecount].x;
			MissleMove.y = your_missle[misslecount].y;
			MissleMove.z = your_missle[misslecount].z;

			D3DVECTOR collidenow;
			D3DVECTOR saveeye;
			float qdist = FastDistance(
			    player_list[trueplayernum].x - your_missle[misslecount].x,
			    player_list[trueplayernum].y - your_missle[misslecount].y,
			    player_list[trueplayernum].z - your_missle[misslecount].z);
			your_missle[misslecount].qdist = qdist;

			collidenow.x = MissleMove.x;
			collidenow.y = MissleMove.y;
			collidenow.z = MissleMove.z;

			excludebox = 1;
			MakeBoundingBox();
			excludebox = 0;
			saveeye = m_vEyePt;
			m_vEyePt = collidenow;

			eRadius = D3DVECTOR(10.0f, 10.0f, 10.0f);

			float realspeed;

			savevelocity = your_missle[misslecount].velocity;

			savevelocity = (savevelocity) * 600.0f * fTimeKeysave;

			realspeed = 10.0f;
			foundcollisiontrue = 0;
			D3DVECTOR result;
			result = collideWithWorld(collidenow / eRadius, (savevelocity) / eRadius);
			result = result * eRadius;

			if (foundcollisiontrue == 1) {
				your_missle[misslecount].active = 2;

				int volume;
				volume = 100 - (int)((100 * your_missle[misslecount].qdist) / ((numberofsquares * monsterdist) / 2));

				if (volume < 10)
					volume = 10;

				if (volume > 100)
					volume = 100;

				PlayWavSound(your_missle[misslecount].sexplode, volume);
			} else {

				int volume;
				volume = 100 - (int)((100 * your_missle[misslecount].qdist) / ((numberofsquares * monsterdist) / 2));

				if (volume < 10)
					volume = 10;

				if (volume > 100)
					volume = 100;

				DSound_Set_Volume(your_missle[misslecount].smove, volume);

				your_missle[misslecount].x = result.x;
				your_missle[misslecount].y = result.y;
				your_missle[misslecount].z = result.z;
			}

			m_vEyePt = saveeye;

			your_missle[misslecount].qdist = qdist;

			if (qdist > culldist) {
				your_missle[misslecount].active = 0;
			} else {
				// Set up the light structure
				D3DLIGHT7 light;
				ZeroMemory(&light, sizeof(D3DLIGHT7));

				if (current_gun == 18) {
					light.dcvAmbient.r = 1.0f;
					light.dcvAmbient.g = 1.0f;
					light.dcvAmbient.b = 1.0f;
				} else if (current_gun == 19) {
					light.dcvAmbient.r = 1.0f;
					light.dcvAmbient.g = 0.2f;
					light.dcvAmbient.b = 0.3f;
				} else {
					light.dcvAmbient.r = 0.4f;
					light.dcvAmbient.g = 0.3f;
					light.dcvAmbient.b = 1.0f;
				}

				light.dcvDiffuse.r = light.dcvAmbient.r;
				light.dcvDiffuse.g = light.dcvAmbient.g;
				light.dcvDiffuse.b = light.dcvAmbient.b;

				light.dcvSpecular.r = 0.2f;
				light.dcvSpecular.g = 0.2f;
				light.dcvSpecular.b = 0.2f;

				light.dvRange = 100.0f;

				light.dvPosition = D3DVECTOR(your_missle[misslecount].x,
				                             your_missle[misslecount].y, your_missle[misslecount].z);

				light.dvAttenuation0 = 1.0f;
				light.dltType = D3DLIGHT_POINT;

				m_pd3dDevice->SetLight(num_light_sources, &light);
				m_pd3dDevice->LightEnable((DWORD)num_light_sources, TRUE);
				num_light_sources++;
			}
		}
	}

	ApplyMissleDamage(1);
}

int CMyD3DApplication::DisplayDamage(float x, float y, float z, int owner, int id, bool criticalhit) {

	D3DVECTOR MissleMove;
	D3DVECTOR MissleVelocity;
	int misslecount = 0;
	int misslespot = 0;
	int gun_angle = 0;

	MissleVelocity.x = 0.0f;
	MissleVelocity.y = 0.0f;
	MissleVelocity.z = 0.0f;

	float monstersize = 25.0f;

	for (int i = 0; i < num_monsters; i++) {
		for (int cullloop = 0; cullloop < monstercount; cullloop++) {
			if (monstercull[cullloop] == monster_list[id].monsterid) {
				monstersize = monstertrueheight[cullloop];

				monstersize = monstersize / 4.0f;
			}
		}
	}

	for (misslecount = 0; misslecount < MAX_MISSLE; misslecount++) {
		if (your_missle[misslecount].active == 0) {
			misslespot = misslecount;
			break;
		}
	}
	your_missle[misslespot].model_id = 104;
	your_missle[misslespot].skin_tex_id = 370;

	your_missle[misslespot].current_frame = 0;
	your_missle[misslespot].current_sequence = 0;
	your_missle[misslespot].x = x;
	your_missle[misslespot].y = y + monstersize;
	your_missle[misslespot].z = z;
	your_missle[misslespot].rot_angle = (float)gun_angle;
	your_missle[misslespot].velocity = savevelocity;
	your_missle[misslespot].active = 2;
	your_missle[misslespot].owner = (int)owner;

	your_missle[misslespot].playernum = (int)owner;
	your_missle[misslespot].playertype = (int)1;
	your_missle[misslespot].guntype = current_gun;

	your_missle[misslespot].critical = criticalhit;

	return misslespot;
}

void CMyD3DApplication::FireMonsterMissle(int monsterid, int type) {

	D3DVECTOR MissleMove;
	D3DVECTOR MissleVelocity;
	int misslecount = 0;
	int misslespot = 0;
	float gun_angle;
	float angy;

	angy = monster_list[monsterid].rot_angle;
	gun_angle = angy;

	angy = 360 - angy;

	angy = fixangle(angy, 90);

	for (misslecount = 0; misslecount < MAX_MISSLE; misslecount++) {
		if (your_missle[misslecount].active == 0) {

			misslespot = misslecount;
			break;
		}
	}

	float r = 30.0f;
	D3DVECTOR savevelocity;

	D3DVECTOR slope;
	D3DVECTOR slope1;

	slope.x = monster_list[monsterid].x;
	slope.y = monster_list[monsterid].y;
	slope.z = monster_list[monsterid].z;

	slope1.x = player_list[monster_list[monsterid].closest].x;
	slope1.y = player_list[monster_list[monsterid].closest].y + 10.0f;
	slope1.z = player_list[monster_list[monsterid].closest].z;

	slope = Normalize(slope1 - slope);

	savevelocity.x = r * sinf(angy * k);
	savevelocity.y = r * slope.y;
	savevelocity.z = r * cosf(angy * k);

	your_missle[misslespot].model_id = 102;

	float qdist = FastDistance(
	    player_list[trueplayernum].x - your_missle[misslespot].x,
	    player_list[trueplayernum].y - your_missle[misslespot].y,
	    player_list[trueplayernum].z - your_missle[misslespot].z);
	your_missle[misslespot].qdist = qdist;

	int volume;
	volume = 100 - (int)((100 * your_missle[misslespot].qdist) / ((numberofsquares * monsterdist) / 2));

	if (volume < 10)
		volume = 10;

	if (volume > 100)
		volume = 100;

	if (your_missle[misslespot].smove != 0) {
		DSound_Delete_Sound(your_missle[misslespot].smove);
		your_missle[misslespot].smove = 0;
	}

	if (type == 1) {
		your_missle[misslespot].model_id = 103;
		your_missle[misslespot].skin_tex_id = 205;
		your_missle[misslespot].smove = DSound_Replicate_Sound(SoundID("spell1"));
	} else if (type == 2) {
		your_missle[misslespot].model_id = 104;
		your_missle[misslespot].skin_tex_id = 288;
		your_missle[misslespot].smove = DSound_Replicate_Sound(SoundID("spell2"));
	} else if (type == 3) {
		your_missle[misslespot].model_id = 105;
		your_missle[misslespot].skin_tex_id = 278;
		your_missle[misslespot].smove = DSound_Replicate_Sound(SoundID("spell3"));
	} else if (type == 4) {
		your_missle[misslespot].model_id = 105;
		your_missle[misslespot].skin_tex_id = 278;
	}

	savevelocity = calculatemisslelength(savevelocity);

	PlayWavSound(your_missle[misslecount].smove, volume);

	your_missle[misslespot].current_frame = 0;
	your_missle[misslespot].current_sequence = 0;
	your_missle[misslespot].x = monster_list[monsterid].x;
	your_missle[misslespot].y = monster_list[monsterid].y + 10.0f;
	your_missle[misslespot].z = monster_list[monsterid].z;
	your_missle[misslespot].rot_angle = (float)gun_angle;
	your_missle[misslespot].velocity = savevelocity;
	your_missle[misslespot].active = 1;
	your_missle[misslespot].owner = monsterid;
	your_missle[misslespot].y_offset = (float)type;
	your_missle[misslespot].playernum = (int)monsterid;
	your_missle[misslespot].playertype = (int)0;

	if (your_missle[misslespot].sexplode != 0) {
		DSound_Delete_Sound(your_missle[misslespot].sexplode);
		your_missle[misslespot].sexplode = 0;
	}
	your_missle[misslespot].sexplode = DSound_Replicate_Sound(SoundID("explod2"));

	D3DVECTOR velocity;
	velocity = D3DVECTOR(0, 0, 0);

	num_your_missles++;
}

HRESULT CMyD3DApplication::RenderOpeningScreen() {
	D3DVECTOR vw1, vw2, vw3;
	float workx, worky, workz;

	int i, j, t;
	int angle;
	int last_texture_number;

	int vert_index;
	int face_index;
	int cell_x, cell_z;
	//	int curr_wep;
	//	int curr_car;
	int f_index;
	int texture_alias_number;
	int icnt;

	D3DMATRIX matRotateSky;
	float qdist = 0;

	DWORD dwIndexCount;
	DWORD dwVertexCount;
	DWORD color_key = 0;
	BOOL use_player_skins_flag = TRUE;
	LPDIRECTDRAWSURFACE7 lpDDsurface;

	D3DPRIMITIVETYPE command;
	HRESULT hr;
	int ap_cnt = 0;

	int lookforward = 15;
	int lookside = 3;
	int font = 0;

	dialogpause = 0;
	IsRenderingOk = TRUE;

	if (rendering_first_frame == TRUE) {
		RRAppActive = TRUE;
		PrintMessage(m_hWnd, "RenderScene : rendering first frame", NULL, LOGFILE_ONLY);
	}

	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
	                    0x00000000, 1.0f, 0L);

	D3DCLIPSTATUS status = { D3DCLIPSTATUS_EXTENTS2, 0, 2048.0f, 0.0f, 2048.0f, 0.0f, 0.0f, 0.0f };

	numnormals = 0;

	number_of_polys_per_frame = 0;
	num_triangles_in_scene = 0;
	num_verts_in_scene = 0;
	num_dp_commands_in_scene = 0;

	poly_cnt = 0;
	cnt = 0;
	icnt = 0;
	face_index = 0;
	cnt_f = 0;

	cell_x = (int)(m_vEyePt.x / 256);
	cell_z = (int)(m_vEyePt.z / 256);

	player_x = m_vEyePt.x;
	player_z = m_vEyePt.z;

	MakeBoundingBox();
	for (i = 0; i < MAX_NUM_QUADS; i++) {
		oblist_overdraw_flags[i] = FALSE;
		oblist_overlite_flags[i] = FALSE;
	}

	if (angy >= 360)
		angy = angy - 360;

	if (angy < 0)
		angy += 360;

	// Work out which direction player is facing in
	// i.e north, east, south, or west
	// and store the numbers of the cells which are in view.
	// DungeonStomp works out what cells are visible by simply
	// looking at a rectangular 5 x 6 block of cells directly
	// infront of the player.

	number_of_polys_per_frame = 0;
	cnt = 0;

	monstercount = 0;
	g_ob_vert_count = 0;

	int cullloop = 0;
	int cullflag = 0;

	i = trueplayernum;

	wx = 750.0f;
	wy = 90.00f;
	wz = 750.0f;

	x_off = 0;
	y_off = 0;
	z_off = 0;

	drawthistri = 1;
	use_player_skins_flag = 1;
	current_frame = player_list[i].current_frame;
	angle = 360 - (int)player_list[i].rot_angle + 90;

	int nextFrame = GetNextFramePlayer();
	// frame88
	if (openingscreen == 3) {
		PlayerToD3DVertList(player_list[i].model_id,
		                    93, angle,
		                    player_list[i].skin_tex_id,
		                    USE_DEFAULT_MODEL_TEX, nextFrame);
	} else {
		PlayerToD3DVertList(player_list[i].model_id,
		                    player_list[i].current_frame, angle,
		                    player_list[i].skin_tex_id,
		                    USE_DEFAULT_MODEL_TEX, nextFrame);
	}

	// DRAW YOUR GUN ///////////////////////////////////////////

	use_player_skins_flag = 1;

	i = 0;

	int ob_type = player_list[i].gunid;
	current_frame = player_list[i].current_frame;
	angle = (int)player_list[i].gunangle;

	int getgunid = currentmodellist;

	if (openingscreen != 3) {
		if (strcmp(model_list[getgunid].monsterweapon, "NONE") != 0) {
			PlayerToD3DVertList(FindModelID(model_list[getgunid].monsterweapon),
			                    current_frame,
			                    angle,
			                    FindGunTexture(model_list[getgunid].monsterweapon),
			                    USE_DEFAULT_MODEL_TEX, nextFrame);
		}
	}

	fDot2 = 0.0f;
	weapondrop = 0;
	car_speed = (float)0;

	hr = m_pd3dDevice->BeginScene();

	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x00f0f0f0);

	D3DMATERIAL7 mtrl;

	D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);

	mtrl.dcvSpecular.g = 0.3f;
	mtrl.dcvSpecular.b = 0.3f;
	mtrl.dcvSpecular.a = 0.0f;

	mtrl.power = 40.0f;
	m_pd3dDevice->SetMaterial(&mtrl);

	lastmaterial = 0;

	if (hr != D3D_OK) {
		PrintMessage(NULL, "BeginScene : FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (m_pd3dDevice->SetClipStatus(&status) != D3D_OK) {
		PrintMessage(NULL, "SetClipStatus : FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE) != D3D_OK)
		return FALSE;

	///////////////////////////////////////////////////////////////////////
	//
	// If the fog is not exponential,
	//		Draw the cloud background we will animate
	//
	if (m_tableFog == FALSE) {
		// set texture to texture 0, which is our background texture
		//		m_pd3dDevice->SetTexture( 0, lpddsImagePtr[0]);
		//		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
		//                                 m_pBackground, 4, 0 );
	} else {
		//
		// Set the background to blue
		//
		m_pd3dDevice->Clear(0,
		                    NULL,
		                    D3DCLEAR_TARGET,
		                    RGBA_MAKE(0, 0, 80, 0),
		                    1.0f,
		                    0L);
	}

	last_texture_number = 1000;
	vert_index = 0;
	face_index = 0;
	ap_cnt = 0;
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x00f0f0f0);
	num_polys_per_frame = number_of_polys_per_frame;

	for (i = 0; i < number_of_polys_per_frame; i++) {
		texture_alias_number = texture_list_buffer[i];
		texture_number = TexMap[texture_alias_number].texture;

		if (texture_number >= NumTextures)
			texture_number = 0;

		lpDDsurface = lpddsImagePtr[texture_number];

		if ((TexMap[texture_alias_number].is_alpha_texture == TRUE) &&
		    (bEnableAlphaTransparency == TRUE)) {
			alpha_poly_index[ap_cnt] = i;
			alpha_vert_index[ap_cnt] = vert_index;
			alpha_face_index[ap_cnt] = face_index;
			vert_index += verts_per_poly[i];
			if (dp_command_index_mode[i] == USE_INDEXED_DP)
				face_index += faces_per_poly[i] * 3;
			ap_cnt++;
		} else {
			if (last_texture_number != texture_number) {
				if (lpDDsurface == NULL)
					PrintMessage(NULL, "Can't find texture", NULL, LOGFILE_ONLY);
				else {
					if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
						PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
				}
			}

			if (dp_command_index_mode[i] == USE_NON_INDEXED_DP) {
				if (m_pd3dDevice->DrawPrimitive(dp_commands[i], D3DFVF_VERTEX,
				                                (LPVOID)&src_v[vert_index], verts_per_poly[i], NULL) != D3D_OK) {
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
					return FALSE;
				}

				int vertloop = 0;
				if (normalon) {
					for (vertloop = 0; vertloop < verts_per_poly[i]; vertloop++) {

						numnormals = 0;
						normal_line[numnormals].x = src_v[vert_index + vertloop].x;
						normal_line[numnormals].y = src_v[vert_index + vertloop].y;
						normal_line[numnormals].z = src_v[vert_index + vertloop].z;
						normal_line[numnormals].color = 10;
						numnormals++;
						normal_line[numnormals].x = src_v[vert_index + vertloop].x + src_v[vert_index + vertloop].nx * normalscale;
						normal_line[numnormals].y = src_v[vert_index + vertloop].y + src_v[vert_index + vertloop].ny * normalscale;
						normal_line[numnormals].z = src_v[vert_index + vertloop].z + src_v[vert_index + vertloop].nz * normalscale;
						normal_line[numnormals].color = 10;
						numnormals++;

						if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
						                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
						}
					}
				}

				last_texture_number = texture_number;
				vert_index += verts_per_poly[i];
			}

			if (dp_command_index_mode[i] == USE_INDEXED_DP) {
				dwIndexCount = faces_per_poly[i] * 3;
				dwVertexCount = verts_per_poly[i];
				command = dp_commands[i];

				last_texture_number = texture_number;

				for (t = 0; t < (int)dwIndexCount; t++) {
					f_index = src_f[face_index + t];

					memset(&temp_v[t], 0, sizeof(D3DVERTEX));
					memcpy(&temp_v[t], &src_v[vert_index + f_index],
					       sizeof(D3DVERTEX));
				}

				int counttri = 0;

				for (t = 0; t < (int)dwIndexCount; t++) {

					// ijump
					if (counttri == 0) {

						vw1.x = temp_v[t].x;
						vw1.y = temp_v[t].y;
						vw1.z = temp_v[t].z;

						vw2.x = temp_v[t + 1].x;
						vw2.y = temp_v[t + 1].y;
						vw2.z = temp_v[t + 1].z;

						vw3.x = temp_v[t + 2].x;
						vw3.y = temp_v[t + 2].y;
						vw3.z = temp_v[t + 2].z;

						// calculate the NORMAL for the road using the CrossProduct <-important!

						D3DVECTOR vDiff = vw1 - vw2;
						D3DVECTOR vDiff2 = vw3 - vw2;

						D3DVECTOR vCross = CrossProduct(vDiff, vDiff2);

						D3DVECTOR final = Normalize(vCross);

						workx = -final.x;
						worky = -final.y;
						workz = -final.z;
					}

					counttri++;
					if (counttri > 2)
						counttri = 0;

					temp_v[t].nx = workx;
					temp_v[t].ny = worky;
					temp_v[t].nz = workz;
					if (normalon) {
						numnormals = 0;
						normal_line[numnormals].x = temp_v[t].x;
						normal_line[numnormals].y = temp_v[t].y;
						normal_line[numnormals].z = temp_v[t].z;
						normal_line[numnormals].color = 10;
						numnormals++;
						normal_line[numnormals].x = temp_v[t].x + temp_v[t].nx * normalscale;
						normal_line[numnormals].y = temp_v[t].y + temp_v[t].ny * normalscale;
						normal_line[numnormals].z = temp_v[t].z + temp_v[t].nz * normalscale;
						normal_line[numnormals].color = 10;
						numnormals++;

						if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
						                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
						}
					}
				}

				if (m_pd3dDevice->DrawPrimitive(command,
				                                D3DFVF_VERTEX,
				                                (LPVOID)&temp_v[0],
				                                dwIndexCount,
				                                NULL) != D3D_OK) {
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
					return FALSE;
				}

				last_texture_number = texture_number;

				face_index += dwIndexCount;
				vert_index += dwVertexCount;
			}
		}
	} // end for i

	// titlepage
	int bg = pCMyApp->FindTextureAlias("dungeont");
	texture_number = TexMap[bg].texture;
	lpDDsurface = lpddsImagePtr[texture_number];

	if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
		PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

	float adjust = 200.0f;

	m_DisplayMessage[0].sy = (FLOAT)410;
	m_DisplayMessage[1].sy = (FLOAT)60;
	m_DisplayMessage[2].sy = (FLOAT)410;
	m_DisplayMessage[3].sy = (FLOAT)60;

	m_DisplayMessage[0].sx = (FLOAT)viewportwidth / 2 - 350;
	m_DisplayMessage[1].sx = (FLOAT)viewportwidth / 2 - 350;
	m_DisplayMessage[2].sx = (FLOAT)viewportwidth / 2 + 350;
	m_DisplayMessage[3].sx = (FLOAT)viewportwidth / 2 + 350;

	m_DisplayMessage[0].tu = 0.0f;
	m_DisplayMessage[0].tv = 1.0f;

	m_DisplayMessage[1].tu = 0.0f;
	m_DisplayMessage[1].tv = 0.0f;

	m_DisplayMessage[2].tu = 1.0f;
	m_DisplayMessage[2].tv = 1.0f;

	m_DisplayMessage[3].tu = 1.0f;
	m_DisplayMessage[3].tv = 0.0f;

	m_DisplayMessage[0].color = RGBA_MAKE(255, 255, 255, 0);
	m_DisplayMessage[1].color = RGBA_MAKE(255, 255, 255, 0);
	m_DisplayMessage[2].color = RGBA_MAKE(255, 255, 255, 0);
	m_DisplayMessage[3].color = RGBA_MAKE(255, 255, 255, 0);

	countmessage = 4;

	float drop = 40.0f;

	i = 0;

	if (openingscreen == 1) {

		for (j = 0; j < 5; j++) {

			if (j == 7) {
				m_DisplayMessage[4 + i].sy = (FLOAT)(210.0f);
				m_DisplayMessage[5 + i].sy = (FLOAT)(60.0f);
				m_DisplayMessage[6 + i].sy = (FLOAT)(210.0f);
				m_DisplayMessage[7 + i].sy = (FLOAT)(60.0f);

				m_DisplayMessage[4 + i].sx = (FLOAT)viewportwidth / 4 - 150;
				m_DisplayMessage[5 + i].sx = (FLOAT)viewportwidth / 4 - 150;
				m_DisplayMessage[6 + i].sx = (FLOAT)viewportwidth / 4 + 150;
				m_DisplayMessage[7 + i].sx = (FLOAT)viewportwidth / 4 + 150;
			} else {

				m_DisplayMessage[4 + i].sy = (FLOAT)(viewportheight / 4) + 30.0f + drop;
				m_DisplayMessage[5 + i].sy = (FLOAT)(viewportheight / 4) - 0.0f + drop;
				m_DisplayMessage[6 + i].sy = (FLOAT)(viewportheight / 4) + 30.0f + drop;
				m_DisplayMessage[7 + i].sy = (FLOAT)(viewportheight / 4) - 0.0f + drop;

				m_DisplayMessage[4 + i].sx = (FLOAT)10.0f;
				m_DisplayMessage[5 + i].sx = (FLOAT)10.0f;
				m_DisplayMessage[6 + i].sx = (FLOAT)140.0f;
				m_DisplayMessage[7 + i].sx = (FLOAT)140.0f;
			}

			m_DisplayMessage[4 + i].tu = 0.0f;
			m_DisplayMessage[4 + i].tv = 1.0f;

			m_DisplayMessage[5 + i].tu = 0.0f;
			m_DisplayMessage[5 + i].tv = 0.0f;

			m_DisplayMessage[6 + i].tu = 1.0f;
			m_DisplayMessage[6 + i].tv = 1.0f;

			m_DisplayMessage[7 + i].tu = 1.0f;
			m_DisplayMessage[7 + i].tv = 0.0f;

			m_DisplayMessage[4 + i].color = RGBA_MAKE(255, 255, 255, 0);
			m_DisplayMessage[5 + i].color = RGBA_MAKE(255, 255, 255, 0);
			m_DisplayMessage[6 + i].color = RGBA_MAKE(255, 255, 255, 0);
			m_DisplayMessage[7 + i].color = RGBA_MAKE(255, 255, 255, 0);

			countmessage += 4;
			i += 4;
			drop += 35.0f;
		}
	}

	i = 0;
	if (openingscreen == 3) {

		for (j = 0; j < 5; j++) {

			m_DisplayMessage[4 + i].sy = (FLOAT)(viewportheight / 4) + 30.0f + drop;
			m_DisplayMessage[5 + i].sy = (FLOAT)(viewportheight / 4) - 0.0f + drop;
			m_DisplayMessage[6 + i].sy = (FLOAT)(viewportheight / 4) + 30.0f + drop;
			m_DisplayMessage[7 + i].sy = (FLOAT)(viewportheight / 4) - 0.0f + drop;

			m_DisplayMessage[4 + i].sx = (FLOAT)10.0f;
			m_DisplayMessage[5 + i].sx = (FLOAT)10.0f;
			m_DisplayMessage[6 + i].sx = (FLOAT)140.0f;
			m_DisplayMessage[7 + i].sx = (FLOAT)140.0f;

			m_DisplayMessage[4 + i].tu = 0.0f;
			m_DisplayMessage[4 + i].tv = 1.0f;

			m_DisplayMessage[5 + i].tu = 0.0f;
			m_DisplayMessage[5 + i].tv = 0.0f;

			m_DisplayMessage[6 + i].tu = 1.0f;
			m_DisplayMessage[6 + i].tv = 1.0f;

			m_DisplayMessage[7 + i].tu = 1.0f;
			m_DisplayMessage[7 + i].tv = 0.0f;

			m_DisplayMessage[4 + i].color = RGBA_MAKE(255, 255, 255, 0);
			m_DisplayMessage[5 + i].color = RGBA_MAKE(255, 255, 255, 0);
			m_DisplayMessage[6 + i].color = RGBA_MAKE(255, 255, 255, 0);
			m_DisplayMessage[7 + i].color = RGBA_MAKE(255, 255, 255, 0);

			countmessage += 4;
			i += 4;
			drop += 35.0f;
		}
	}

	font = countmessage;

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);
	viewportheight = (FLOAT)vp.dwHeight;
	viewportwidth = (FLOAT)vp.dwWidth;
	char junk[255];

	int year;

	time_t now;
	// `time()` returns the current time of the system as a `time_t` value
	time(&now);
	struct tm *local = localtime(&now);
	year = local->tm_year + 1900; // get year since 1900

	sprintf_s(junk, "Copyright 2001-%d", year);

	display_message(viewportwidth - 260.0f, viewportheight - 100.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	strcpy_s(junk, "Aptisense");
	display_message(viewportwidth - 260.0f, viewportheight - 80.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	strcpy_s(junk, "by Mark Longo");
	display_message(viewportwidth - 260.0f, viewportheight - 60.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	strcpy_s(junk, "mlongo@aptisense.com");
	display_message(viewportwidth - 260.0f, viewportheight - 40.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	strcpy_s(junk, "www.aptisense.com");
	display_message(viewportwidth - 260.0f, viewportheight - 20.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

	if (openingscreen == 3) {
		// merchant
		strcpy_s(junk, merchantlist[currentmodellist].Text2);
		display_message(viewportwidth / 2 - 220.0f, viewportheight / 3, junk, vp, 255, 255, 0, 24, 24, 0);
		sprintf_s(junk, "%d GOLD", merchantlist[merchantcurrent].price);
		display_message(viewportwidth / 2 - 220.0f, viewportheight / 3 + 40.0f, junk, vp, 255, 255, 0, 24, 24, 0);

		sprintf_s(junk, "Dungeon Stomp 1.80");
		display_message(5.0f, (FLOAT)vp.dwHeight - 150.0f - 14.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

		sprintf_s(junk, "AREA: ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 10.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%s", gfinaltext);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 10.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		statusbardisplay((float)player_list[trueplayernum].hp, (float)player_list[trueplayernum].hp, 1);

		sprintf_s(junk, "HP  : ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 24.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

		sprintf_s(junk, "%s %d/%d", statusbar, player_list[trueplayernum].health, player_list[trueplayernum].hp);
		display_message(0.0f + 65.0f, (FLOAT)vp.dwHeight - 150.0f + 24.0f, junk, vp, 255, 255, 255, 12.5, 16, 0);

		statusbardisplay((float)player_list[trueplayernum].health, (float)player_list[trueplayernum].hp, 0);
		sprintf_s(junk, "%s", statusbar);
		display_message(0.0f + 65.0f, (FLOAT)vp.dwHeight - 150.0f + 24.0f, junk, vp, 255, 255, 255, 12.5, 16, 0);

		sprintf_s(junk, "WPN : ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 38.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

		char junk3[255];
		if (strstr(your_gun[current_gun].gunname, "SCROLL-MAGICMISSLE") != NULL) {
			strcpy_s(junk3, "MAGIC MISSLE");
			sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
		} else if (strstr(your_gun[current_gun].gunname, "SCROLL-FIREBALL") != NULL) {
			strcpy_s(junk3, "FIREBALL");
			sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
		} else if (strstr(your_gun[current_gun].gunname, "SCROLL-LIGHTNING") != NULL) {
			strcpy_s(junk3, "LIGHTNING");
			sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
		} else if (strstr(your_gun[current_gun].gunname, "SCROLL-HEALING") != NULL) {
			strcpy_s(junk3, "HEALING");
			sprintf_s(junk, "%s: %d", junk3, (int)your_gun[current_gun].x_offset);
		}

		else {
			sprintf_s(junk, "%s", your_gun[current_gun].gunname);
		}
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 38.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "DMG :");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 52.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%dD%d", player_list[trueplayernum].damage1, player_list[trueplayernum].damage2);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 52.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "BNS : ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 66.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

		int gunmodel = 0;
		for (int a = 0; a < num_your_guns; a++) {

			if (your_gun[a].model_id == player_list[trueplayernum].gunid) {

				gunmodel = a;
			}
		}

		int attackbonus = your_gun[gunmodel].sattack;
		int damagebonus = your_gun[gunmodel].sdamage;

		sprintf_s(junk, "+%d/%+d", attackbonus, damagebonus);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 66.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "XP  : ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 80.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%d", player_list[trueplayernum].xp);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 80.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "LVL : ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 94.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%d", player_list[trueplayernum].hd);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 94.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "ARMR: ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 108.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%d", player_list[trueplayernum].ac);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 108.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "THAC: ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 122.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%d", player_list[trueplayernum].thaco);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 122.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "GOLD: ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 136.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%d", player_list[trueplayernum].gold);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 136.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);

		sprintf_s(junk, "KEYS: ");
		display_message(0.0f, (FLOAT)vp.dwHeight - 150.0f + 150.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
		sprintf_s(junk, "%d", player_list[trueplayernum].keys);
		display_message(0.0f + 60.0f, (FLOAT)vp.dwHeight - 150.0f + 150.0f, junk, vp, 0, 245, 255, 12.5, 16, 0);
	}

	if (filtertype == 0) {
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
	} else if (filtertype == 1) {

		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	} else if (filtertype == 2) {
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_ANISOTROPIC);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_ANISOTROPIC);
	}

	if (bEnableAlphaTransparency) {
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,
		                             D3DBLEND_SRCCOLOR);

		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
		                             D3DBLEND_INVSRCCOLOR);

		for (j = 0; j < ap_cnt; j++) {
			i = alpha_poly_index[j];
			vert_index = alpha_vert_index[j];
			face_index = alpha_face_index[j];
			texture_alias_number = texture_list_buffer[i];
			texture_number = TexMap[texture_alias_number].texture;

			if (texture_number >= NumTextures)
				texture_number = 0;

			lpDDsurface = lpddsImagePtr[texture_number];

			if (last_texture_number != texture_number) {
				if (lpDDsurface == NULL)
					PrintMessage(NULL, "Can't find texture", NULL, LOGFILE_ONLY);
				else {
					if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
						PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
				}
			}

			if (dp_command_index_mode[i] == USE_NON_INDEXED_DP) {
				if (m_pd3dDevice->DrawPrimitive(dp_commands[i], D3DFVF_VERTEX,
				                                (LPVOID)&src_v[vert_index], verts_per_poly[i], NULL) != D3D_OK) {
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
					return FALSE;
				}
				int vertloop = 0;
				if (normalon) {
					for (vertloop = 0; vertloop < verts_per_poly[i]; vertloop++) {

						numnormals = 0;
						normal_line[numnormals].x = src_v[vert_index + vertloop].x;
						normal_line[numnormals].y = src_v[vert_index + vertloop].y;
						normal_line[numnormals].z = src_v[vert_index + vertloop].z;
						normal_line[numnormals].color = 10;
						numnormals++;
						normal_line[numnormals].x = src_v[vert_index + vertloop].x + src_v[vert_index + vertloop].nx * normalscale;
						normal_line[numnormals].y = src_v[vert_index + vertloop].y + src_v[vert_index + vertloop].ny * normalscale;
						normal_line[numnormals].z = src_v[vert_index + vertloop].z + src_v[vert_index + vertloop].nz * normalscale;
						normal_line[numnormals].color = 10;
						numnormals++;

						if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
						                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
						}
					}
				}
				last_texture_number = texture_number;
				vert_index += verts_per_poly[i];
			}

			if (dp_command_index_mode[i] == USE_INDEXED_DP) {
				dwIndexCount = faces_per_poly[i] * 3;
				dwVertexCount = verts_per_poly[i];
				command = dp_commands[i];

				last_texture_number = texture_number;

				for (t = 0; t < (int)dwIndexCount; t++) {
					f_index = src_f[face_index + t];

					memset(&temp_v[t], 0, sizeof(D3DVERTEX));
					memcpy(&temp_v[t], &src_v[vert_index + f_index],
					       sizeof(D3DVERTEX));
				}

				int counttri = 0;

				for (t = 0; t < (int)dwIndexCount; t++) {
					if (counttri == 0) {

						vw1.x = temp_v[t].x;
						vw1.y = temp_v[t].y;
						vw1.z = temp_v[t].z;

						vw2.x = temp_v[t + 1].x;
						vw2.y = temp_v[t + 1].y;
						vw2.z = temp_v[t + 1].z;

						vw3.x = temp_v[t + 2].x;
						vw3.y = temp_v[t + 2].y;
						vw3.z = temp_v[t + 2].z;

						// calculate the NORMAL for the road using the CrossProduct <-important!

						D3DVECTOR vDiff = vw1 - vw2;
						D3DVECTOR vDiff2 = vw3 - vw2;

						D3DVECTOR vCross = CrossProduct(vDiff, vDiff2);

						D3DVECTOR final = Normalize(vCross);

						workx = -final.x;
						worky = -final.y;
						workz = -final.z;
					}

					counttri++;
					if (counttri > 2)
						counttri = 0;

					temp_v[t].nx = workx;
					temp_v[t].ny = worky;
					temp_v[t].nz = workz;

					if (normalon) {
						numnormals = 0;
						normal_line[numnormals].x = temp_v[t].x;
						normal_line[numnormals].y = temp_v[t].y;
						normal_line[numnormals].z = temp_v[t].z;
						normal_line[numnormals].color = 10;
						numnormals++;
						normal_line[numnormals].x = temp_v[t].x + temp_v[t].nx * normalscale;
						normal_line[numnormals].y = temp_v[t].y + temp_v[t].ny * normalscale;
						normal_line[numnormals].z = temp_v[t].z + temp_v[t].nz * normalscale;
						normal_line[numnormals].color = 10;
						numnormals++;

						if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
						                                (LPVOID)&normal_line[0], 2, NULL) != D3D_OK) {
						}
					}
				}

				if (m_pd3dDevice->DrawPrimitive(command,
				                                D3DFVF_VERTEX,
				                                (LPVOID)&temp_v[0],
				                                dwIndexCount,
				                                NULL) != D3D_OK) {
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);
					return FALSE;
				}

				last_texture_number = texture_number;

				face_index += dwIndexCount;
				vert_index += dwVertexCount;
			}
		} // end for i

	} // end if

	if (normalon) {
		for (i = 0; i < numnormals; i += 2) {

			if (m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX,
			                                (LPVOID)&normal_line[i], 2, NULL) != D3D_OK) {
			}
		}
	}

	if (openingscreen == 3) {

		int bground = pCMyApp->FindTextureAlias("button4");
		texture_number = TexMap[bground].texture;
		lpDDsurface = lpddsImagePtr[texture_number];

		if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
			PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
	} else {

		int bground = pCMyApp->FindTextureAlias("background");
		texture_number = TexMap[bground].texture;
		lpDDsurface = lpddsImagePtr[texture_number];

		if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
			PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);
	}

	D3DUtil_SetRotateYMatrix(matRotateSky, rotateprop);
	D3DMath_MatrixMultiply(matWorld, matWorld, matRotateSky);

	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
	                                   g_pObstacleVertices, g_dwNumObstacleVertices,
	                                   g_pObstacleIndices, g_dwNumObstacleIndices, 0);

	if (m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE) != D3D_OK)
		return FALSE;

	num_verts_per_frame = vert_index;

	if (rendering_first_frame == TRUE) {
		_itoa_s(number_of_polys_per_frame, buffer, _countof(buffer), 10);
		PrintMessage(NULL, "num_poly = ", buffer, LOGFILE_ONLY);
	}

	if (m_pd3dDevice->GetClipStatus(&status) != D3D_OK) {
		PrintMessage(NULL, "GetClipStatus : FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);

	for (i = 0; i < ((countmessage / 4)); i += 1) {

		char junk[255];

		if (openingscreen >= 1) {
			if (i == buttonselect)
				sprintf_s(junk, "po%d", i);
			else
				sprintf_s(junk, "pb%d", i);
		}

		if (openingscreen == 3) {
			if (i == buttonselect)
				sprintf_s(junk, "pbmo%d", i);
			else
				sprintf_s(junk, "pbm%d", i);
		}

		if (merchantmode == 1) {
			if (strcmp(junk, "pbm4") == 0) {
				strcpy_s(junk, "pbm6");
			}
			if (strcmp(junk, "pbmo4") == 0) {
				strcpy_s(junk, "pbmo6");
			}
			if (strcmp(junk, "pbm3") == 0) {
				strcpy_s(junk, "pbm7");
			}
			if (strcmp(junk, "pbmo3") == 0) {
				strcpy_s(junk, "pbmo7");
			}
		}

		if (i >= font / 4)
			sprintf_s(junk, "fontA");

		bg = pCMyApp->FindTextureAlias(junk);
		texture_number = TexMap[bg].texture;
		lpDDsurface = lpddsImagePtr[texture_number];

		if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
			PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
		                            (LPVOID)&m_DisplayMessage[i * (int)4], 4, 0);
	}

	hr = m_pd3dDevice->EndScene();
	if (hr != D3D_OK) {
		PrintMessage(NULL, "EndScene : FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	menumouseselect();

	rendering_first_frame = FALSE;

	return S_OK;
}

HRESULT CMyD3DApplication::FrameMoveOpeningScreen(FLOAT fTimeKey) {

	float r = 15.0f;
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	float lx, ly, lz;
	float radius = 20.0f; // used for flashlight
	float gun_angle;
	int i;

	D3DVECTOR MissleMove;
	D3DVECTOR MissleVelocity;

	FLOAT tu_left;
	FLOAT tu_right;
	int misslespot = 0;
	// check that angy is between 0 and 360 degrees
	float cameradist = 150.0f;

	angy = 50;

	if (angy >= 360)
		angy = angy - 360;

	if (angy < 0)
		angy += 360;

	UpdateMainPlayer();

	// wrap background pic through 360 degrees about y axis
	// we want to only display one quater of the background
	// texture at a time, because our Field Of View is 90 degrees.

	// texture co-ordinates     angle in degrees
	//   0 to .25				[  0 to  90]
	// .25 to .5				[ 90 to 180]
	// .50 to .75				[180 to 270]
	// .75 to  0				[270 to 360]
	//   0 to .25				[  0 to 90]

	// tu_left is the vertical texture co-ordinate for the left
	// hand side of background rectangle
	tu_left = angy / 360.0000f;

	// tu_right is the vertical texture co-ordinate for the right
	// hand side of background rectangle
	tu_right = tu_left + 0.250f;

	m_pBackground[0].tu = tu_left;
	m_pBackground[1].tu = tu_left;
	m_pBackground[2].tu = tu_right;
	m_pBackground[3].tu = tu_right;

	gun_angle = -angy + (float)90;

	if (gun_angle >= 360)
		gun_angle = gun_angle - 360;
	if (gun_angle < 0)
		gun_angle = gun_angle + 360;

	i = current_gun;
	your_gun[i].rot_angle = gun_angle;
	your_gun[i].x = m_vEyePt.x;
	your_gun[i].y = (float)9 + m_vEyePt.y - 22;
	your_gun[i].z = m_vEyePt.z;

	for (i = 0; i < num_light_sources; i++)
		m_pd3dDevice->LightEnable((DWORD)i, FALSE);

	num_light_sources = 0;

	// Set up the light structure
	D3DLIGHT7 light;
	ZeroMemory(&light, sizeof(D3DLIGHT7));

	light.dcvDiffuse.r = 1.0f;
	light.dcvDiffuse.g = 1.0f;
	light.dcvDiffuse.b = 1.0f;

	light.dcvAmbient.r = 0.3f;
	light.dcvAmbient.g = 0.3f;
	light.dcvAmbient.b = 0.3f;

	light.dvRange = 500.0f; // D3DLIGHT_RANGE_MAX

	// Calculate the flashlight's lookat point, from
	// the player's view direction angy.

	lx = m_vEyePt.x + radius * sinf(angy * k);
	ly = 0;
	lz = m_vEyePt.z + radius * cosf(angy * k);

	// Calculate direction vector for flashlight
	dir_x = lx - m_vEyePt.x;
	dir_y = 0; // ly - m_vEyePt.y;
	dir_z = lz - m_vEyePt.z;

	// set flashlight's position to player's position
	pos_x = m_vLookatPt.x;
	pos_y = m_vLookatPt.y; // m_vEyePt.y;
	pos_z = m_vLookatPt.z;

	if (lighttype == 0) {
		light.dvPosition = D3DVECTOR(pos_x, pos_y, pos_z);
		light.dvDirection = D3DVECTOR(dir_x, dir_y, dir_z);
		light.dvFalloff = .1f;
		light.dvTheta = .6f; // spotlight's inner cone
		light.dvPhi = 1.3f;  // spotlight's outer cone
		light.dvAttenuation0 = 1.0f;
		light.dltType = D3DLIGHT_SPOT;

		// Set the light
	} else {
		light.dvPosition = D3DVECTOR(m_vEyePt.x, m_vEyePt.y, m_vEyePt.z);
		light.dvAttenuation0 = 1.0f;
		light.dvRange = 281.0f;
		light.dltType = D3DLIGHT_POINT;
	}

	if (bIsFlashlightOn == TRUE) {
		m_pd3dDevice->SetLight(num_light_sources, &light);
		m_pd3dDevice->LightEnable((DWORD)num_light_sources, TRUE);
		num_light_sources++;
	}

	if (GetAsyncKeyState(0x44) < 0) { // d -

		model_y -= 1;
	}
	if (GetAsyncKeyState(0x45) < 0) { // e -

		model_y += 1;
	}
	// Calculate the players lookat point, from
	// the player's view direction angy.

	savevelocity = D3DVECTOR(0, 0, 0);
	if (playermove == 1) {
		savevelocity.x = r * sinf(angy * k);
		savevelocity.y = r * sinf(0.0f * k);
		savevelocity.z = r * cosf(angy * k);
	}
	if (playermove == 4) {
		savevelocity.x = -r * sinf(angy * k);
		savevelocity.y = -r * sinf(0.0f * k);
		;
		savevelocity.z = -r * cosf(angy * k);
	}

	if (player_list[trueplayernum].current_sequence != 2) {
		if (playermove == 0) {
			if (savelastmove != playermove)
				SetPlayerAnimationSequence(trueplayernum, 0);
		} else {

			if (savelastmove != playermove)
				SetPlayerAnimationSequence(trueplayernum, 1);
		}
	}

	savelastmove = playermove;

	saveoldvelocity = savevelocity;

	// HORIZONTAL COLLISION

	D3DVECTOR result;
	eRadius = D3DVECTOR(spheresize, sphereheight, spheresize);

	result = collideWithWorld(m_vLookatPt / eRadius, savevelocity / eRadius);
	result = result * eRadius;

	m_vLookatPt.x = result.x;
	m_vLookatPt.y = 70.0f;
	m_vLookatPt.z = result.z;

	// CAMERA COLLISION

	m_vEyePt.x = m_vLookatPt.x + cameradist * -sinf(angy * k);
	m_vEyePt.y = m_vLookatPt.y + cameradist * sinf(look_up_ang * k);
	m_vEyePt.z = m_vLookatPt.z + cameradist * -cosf(angy * k);

	savevelocity.x = r * 20 * sinf(angy * k);

	savevelocity.y = 70.0f;
	savevelocity.z = r * 20 * cosf(angy * k);

	eRadius = D3DVECTOR(5.0f, 5.0f, 5.0f);

	result = m_vLookatPt + savevelocity;
	m_vEyePt.x = result.x;
	m_vEyePt.y = 90.0f;
	m_vEyePt.z = result.z;

	modellocation.x = m_vLookatPt.x;
	modellocation.y = m_vLookatPt.y;
	modellocation.z = m_vLookatPt.z;

	playermovestrife = 0;
	playermove = 0;

	EyeTrue = m_vEyePt;
	EyeTrue.y = m_vEyePt.y + cameraheight;

	LookTrue = m_vLookatPt;
	LookTrue.y = m_vLookatPt.y + cameraheight;

	D3DUtil_SetViewMatrix(matView, EyeTrue, LookTrue, m_vUpVec);
	D3DUtil_SetProjectionMatrix(matProj, FOV, ASPECT_RATIO, Z_NEAR, Z_FAR);
	D3DUtil_SetIdentityMatrix(matWorld);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matView);
	m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);
	UpdateMainPlayer();

	return S_OK;
}

void CMyD3DApplication::display_font(float x, float y, char text[1000], D3DVIEWPORT7 vp, int r, int g, int b) {

	float tuad = .0625f;
	float tvad = .0625f;

	float fontsize;
	int intTextLength = 0;
	int i = 0;
	float itrue = 0;
	float tu = 1.0f, tv = 1.0f;
	int textlen = 0;
	int textmid;
	char lefttext[500];
	char righttext[500];
	char newtext[500];

	char reversetext[500];
	float adjust = 0.0f;
	float adjust2 = 0.0f;

	int j = 2;

	int countl = 0;
	int countr = 0;
	int countreverse = 0;
	float xcol;
	xcol = x;
	textlen = strlen(text);
	textmid = textlen / 2;

	fontsize = 5;

	if (perspectiveview == 1)
		fontsize = 5;

	fontsize = 4;

	int flip = 0;

	for (i = 0; i < textlen; i++) {

		if (i < textmid) {

			lefttext[countl++] = text[i];
		} else {

			righttext[countr++] = text[i];
		}
	}

	lefttext[countl] = '\0';
	righttext[countr] = '\0';

	strcpy_s(newtext, righttext);
	countreverse = strlen(lefttext);

	for (i = 1; i <= (int)strlen(lefttext); i++) {

		reversetext[i - 1] = lefttext[countreverse - i];
	}

	reversetext[i - 1] = '\0';

	y = 40;

	for (j = 0; j < 2; j++) {

		if (j == 1) {
			strcpy_s(newtext, reversetext);
			if (flip == 0)
				flip = 1;
		}

		intTextLength = strlen(newtext);

		for (i = 0; i < intTextLength; i++) {

			switch (newtext[i]) {

			case '0':

				tu = 1.0f;
				tv = 4.0f;

				break;
			case '1':

				tu = 2.0f;
				tv = 4.0f;

				break;
			case '2':

				tu = 3.0f;
				tv = 4.0f;

				break;
			case '3':

				tu = 4.0f;
				tv = 4.0f;
				break;

			case '4':

				tu = 5.0f;
				tv = 4.0f;

				break;
			case '5':

				tu = 6.0f;
				tv = 4.0f;

				break;
			case '6':

				tu = 7.0f;
				tv = 4.0f;

				break;
			case '7':

				tu = 8.0f;
				tv = 4.0f;
				break;
			case '8':

				tu = 9.0f;
				tv = 4.0f;

				break;
			case '9':

				tu = 10.0f;
				tv = 4.0f;

				break;
			case ':':

				tu = 11.0f;
				tv = 4.0f;

				break;
			case '.':

				tu = 15.0f;
				tv = 3.0f;

				break;
			case '+':

				tu = 12.0f;
				tv = 3.0f;

				break;
			case ',':

				tu = 13.0f;
				tv = 3.0f;

				break;

			case '-':

				tu = 14.0f;
				tv = 3.0f;

				break;
			case '/':

				tu = 16.0f;
				tv = 3.0f;

				break;

			case 'A':

				tu = 2.0f;
				tv = 5.0f;

				break;

			case 'B':

				tu = 3.0f;
				tv = 5.0f;

				break;

			case 'C':

				tu = 4.0f;
				tv = 5.0f;

				break;

			case 'D':

				tu = 5.0f;
				tv = 5.0f;

				break;

			case 'E':

				tu = 6.0f;
				tv = 5.0f;

				break;

			case 'F':

				tu = 7.0f;
				tv = 5.0f;

				break;

			case 'G':

				tu = 8.0f;
				tv = 5.0f;

				break;
			case 'H':

				tu = 9.0f;
				tv = 5.0f;

				break;
			case 'I':

				tu = 10.0f;
				tv = 5.0f;

				break;
			case 'J':

				tu = 11.0f;
				tv = 5.0f;

				break;

			case 'K':

				tu = 12.0f;
				tv = 5.0f;

				break;

			case 'L':

				tu = 13.0f;
				tv = 5.0f;

				break;

			case 'M':

				tu = 14.0f;
				tv = 5.0f;

				break;
			case 'N':

				tu = 15.0f;
				tv = 5.0f;

				break;

			case 'O':

				tu = 16.0f;
				tv = 5.0f;

				break;
			case 'P':

				tu = 1.0f;
				tv = 6.0f;

				break;
			case 'Q':

				tu = 2.0f;
				tv = 6.0f;

				break;
			case 'R':

				tu = 3.0f;
				tv = 6.0f;

				break;
			case 'S':

				tu = 4.0f;
				tv = 6.0f;

				break;
			case 'T':

				tu = 5.0f;
				tv = 6.0f;

				break;
			case 'U':

				tu = 6.0f;
				tv = 6.0f;

				break;
			case 'V':

				tu = 7.0f;
				tv = 6.0f;

				break;
			case 'W':

				tu = 8.0f;
				tv = 6.0f;

				break;
			case 'X':

				tu = 9.0f;
				tv = 6.0f;

				break;
			case 'Y':

				tu = 10.0f;
				tv = 6.0f;

				break;
			case 'Z':

				tu = 11.0f;
				tv = 6.0f;

				break;

			case 'a':

				tu = 2.0f;
				tv = 7.0f;

				break;

			case 'b':

				tu = 3.0f;
				tv = 7.0f;

				break;

			case 'c':

				tu = 4.0f;
				tv = 7.0f;

				break;

			case 'd':

				tu = 5.0f;
				tv = 7.0f;

				break;

			case 'e':

				tu = 6.0f;
				tv = 7.0f;

				break;

			case 'f':

				tu = 7.0f;
				tv = 7.0f;

				break;

			case 'g':

				tu = 8.0f;
				tv = 7.0f;

				break;
			case 'h':

				tu = 9.0f;
				tv = 7.0f;

				break;
			case 'i':

				tu = 10.0f;
				tv = 7.0f;

				break;
			case 'j':

				tu = 11.0f;
				tv = 7.0f;

				break;

			case 'k':

				tu = 12.0f;
				tv = 7.0f;

				break;

			case 'l':

				tu = 13.0f;
				tv = 7.0f;

				break;

			case 'm':

				tu = 14.0f;
				tv = 7.0f;

				break;
			case 'n':

				tu = 15.0f;
				tv = 7.0f;

				break;

			case 'o':

				tu = 16.0f;
				tv = 7.0f;

				break;
			case 'p':

				tu = 1.0f;
				tv = 8.0f;

				break;
			case 'q':

				tu = 2.0f;
				tv = 8.0f;

				break;
			case 'r':

				tu = 3.0f;
				tv = 8.0f;

				break;
			case 's':

				tu = 4.0f;
				tv = 8.0f;

				break;
			case 't':

				tu = 5.0f;
				tv = 8.0f;

				break;
			case 'u':

				tu = 6.0f;
				tv = 8.0f;

				break;
			case 'v':

				tu = 7.0f;
				tv = 8.0f;

				break;
			case 'w':

				tu = 8.0f;
				tv = 8.0f;

				break;
			case 'x':

				tu = 9.0f;
				tv = 8.0f;

				break;
			case 'y':

				tu = 10.0f;
				tv = 8.0f;

				break;
			case 'z':

				tu = 11.0f;
				tv = 8.0f;

				break;

			case ' ':

				tu = 1.0f;
				tv = 3.0f;
				break;
			case '|':

				tu = 1.0f;
				tv = 1.0f;
				break;
			case '`':

				tu = 2.0f;
				tv = 1.0f;
				break;
			default:
				tu = 2.0f;
				tv = 9.0f;

				break;
			}

			if (j == 0)
				itrue = (float)-i - 1;
			else
				itrue = (float)i;

			if (flip == 1) {
				flip = 2;
				adjust = 2.0f;
			}

			float amount = 1.5f;

			if (j == 0) {
				adjust += amount;
			} else {
				adjust -= amount;
			}

			long currentcolour = 0;
			currentcolour = RGBA_MAKE(255, 255, 255, 0);

			RGBA_MAKE(0, 0, 0, 0);
			m_BackgroundMesh[countdisplay] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * tu, tvad * (tv - 1.0f));

			m_BackgroundMesh[countdisplay].sx = (x + (itrue * fontsize)) + adjust;
			m_BackgroundMesh[countdisplay].sy = y;

			bubble[countdisplay].x = m_BackgroundMesh[countdisplay].sx;
			bubble[countdisplay].y = m_BackgroundMesh[countdisplay].sy;
			bubble[countdisplay].tu = m_BackgroundMesh[countdisplay].tu;
			bubble[countdisplay].tv = m_BackgroundMesh[countdisplay].tv;

			bubble[countdisplay].z = 0;

			countdisplay++;
			m_BackgroundMesh[countdisplay] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * tu, tvad * tv);

			m_BackgroundMesh[countdisplay].sx = (x + (itrue * fontsize)) + adjust;
			m_BackgroundMesh[countdisplay].sy = y - fontsize;

			bubble[countdisplay].x = m_BackgroundMesh[countdisplay].sx;
			bubble[countdisplay].y = m_BackgroundMesh[countdisplay].sy;
			bubble[countdisplay].tu = m_BackgroundMesh[countdisplay].tu;
			bubble[countdisplay].tv = m_BackgroundMesh[countdisplay].tv;
			bubble[countdisplay].z = 0;

			countdisplay++;

			m_BackgroundMesh[countdisplay] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * (tu - 1.0f), tvad * (tv - 1.0f));
			m_BackgroundMesh[countdisplay].sx = (x + fontsize + (itrue * fontsize)) + adjust;
			m_BackgroundMesh[countdisplay].sy = y;

			bubble[countdisplay].x = m_BackgroundMesh[countdisplay].sx;
			bubble[countdisplay].y = m_BackgroundMesh[countdisplay].sy;
			bubble[countdisplay].tu = m_BackgroundMesh[countdisplay].tu;
			bubble[countdisplay].tv = m_BackgroundMesh[countdisplay].tv;
			bubble[countdisplay].z = 0;

			countdisplay++;

			m_BackgroundMesh[countdisplay] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * (tu - 1.0f), tvad * tv);
			m_BackgroundMesh[countdisplay].sx = (x + fontsize + (itrue * fontsize)) + adjust;
			m_BackgroundMesh[countdisplay].sy = y - fontsize;

			bubble[countdisplay].x = m_BackgroundMesh[countdisplay].sx;
			bubble[countdisplay].y = m_BackgroundMesh[countdisplay].sy;
			bubble[countdisplay].tu = m_BackgroundMesh[countdisplay].tu;
			bubble[countdisplay].tv = m_BackgroundMesh[countdisplay].tv;
			bubble[countdisplay].z = 0;

			countdisplay++;
		}
	}
}

void CMyD3DApplication::DisplayPlayerCaption() {

	int i;
	LPDIRECTDRAWSURFACE7 lpDDsurface;
	float x, y, z;

	float pangle = 0;

	int countit = 0;
	int cullloop = 0;
	int cullflag = 0;
	int num = 0;
	int len = 0;
	int count = 0;
	char junk2[2000];
	int flag = 1;
	float yadjust = 0;

	if (showtexture == 0)
		return;

	D3DMATRIX matRotate;
	int j = 0;

	if (menuflares == 1) {
		if (lastmaterial == 0) {
			D3DMATERIAL7 mtrl;
			D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);
			mtrl.emissive.r = 1.0f;
			mtrl.emissive.g = 1.0f;
			mtrl.emissive.b = 1.0f;
			lastmaterial = 1;
			m_pd3dDevice->SetMaterial(&mtrl);
		}
	}

	int bground = pCMyApp->FindTextureAlias("fontA");
	texture_number = TexMap[bground].texture;
	lpDDsurface = lpddsImagePtr[texture_number];

	if (m_pd3dDevice->SetTexture(0, lpDDsurface) != DD_OK)
		PrintMessage(NULL, "SetTexture FAILED", NULL, LOGFILE_ONLY);

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);

	for (j = 0; j < num_monsters; j++) {

		cullflag = 0;
		for (cullloop = 0; cullloop < monstercount; cullloop++) {
			if (monstercull[cullloop] == monster_list[j].monsterid) {
				cullflag = 1;
				break;
			}
		}

		flag = 1;
		num = 0;
		count = 0;
		yadjust = 0.0f;

		if (monster_list[j].bIsPlayerValid && cullflag == 1 && monster_list[j].bStopAnimating == FALSE) {

			len = strlen(monster_list[j].chatstr);

			if (len > 0)
				len--;

			while (flag) {
				count = 0;

				while (monster_list[j].chatstr[num] != '!') {

					junk2[count] = monster_list[j].chatstr[num];
					count++;
					num++;
					if (num >= len)
						break;
				}
				if (monster_list[j].chatstr[num] == '!')
					num++;

				junk2[count] = '\0';

				if (num >= len || len == 0)
					flag = 0;

				x = monster_list[j].x;
				y = monster_list[j].y + 28.0f - yadjust;
				z = monster_list[j].z;

				yadjust += 6.0f;

				countdisplay = 0;
				display_font(0.0f, 0.0f, junk2, vp, 255, 255, 0);

				D3DUtil_SetTranslateMatrix(matWorld, D3DVECTOR(x, y, z));
				D3DUtil_SetRotateYMatrix(matRotate, (angy * k + (int)3.14));
				D3DMath_MatrixMultiply(matWorld, matRotate, matWorld);

				m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

				for (i = 0; i < ((countdisplay / 4)); i += 1) {

					m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
					                            (LPVOID)&bubble[i * (int)4], 4, 0);
				}
			}
		}
	}

	if (multiplay_flag == TRUE) {

		for (j = 0; j < num_players; j++) {

			cullflag = 0;
			for (cullloop = 0; cullloop < monstercount; cullloop++) {

				if (monstercull[cullloop] == (int)j + (int)999) {
					if (player_list[j].bIsPlayerAlive == TRUE) {

						cullflag = 1;
						break;
					}
				}
			}

			flag = 1;
			num = 0;
			count = 0;
			yadjust = 0.0f;

			if (player_list[j].bIsPlayerValid && cullflag == 1 && player_list[j].bStopAnimating == FALSE ||
			    trueplayernum == j && perspectiveview == 0 && player_list[j].bIsPlayerAlive == TRUE) {

				char savehp[200];

				statusbardisplay((float)player_list[j].hp, (float)player_list[j].hp, 1);
				strcpy_s(savehp, statusbar);
				statusbardisplay((float)player_list[j].health, (float)player_list[j].hp, 0);

				for (int jj = 0; jj < (int)strlen(statusbar); jj++) {

					savehp[jj] = statusbar[jj];
				}

				sprintf_s(player_list[j].chatstr, "%s AC:%d!HP:%d %s",
				          player_list[j].rname,
				          player_list[trueplayernum].thaco - player_list[j].ac,
				          player_list[j].health, savehp);

				len = strlen(player_list[j].chatstr);

				if (len > 0)
					len--;

				while (flag) {
					count = 0;

					while (player_list[j].chatstr[num] != '!') {

						junk2[count] = player_list[j].chatstr[num];
						count++;
						num++;
						if (num >= len)
							break;
					}
					if (player_list[j].chatstr[num] == '!')
						num++;

					junk2[count] = '\0';

					if (num >= len || len == 0)
						flag = 0;

					x = player_list[j].x;
					y = player_list[j].y + 28.0f - yadjust;
					z = player_list[j].z;

					yadjust += 6.0f;

					countdisplay = 0;
					display_font(0.0f, 0.0f, junk2, vp, 255, 255, 0);

					D3DUtil_SetTranslateMatrix(matWorld, D3DVECTOR(x, y, z));
					D3DUtil_SetRotateYMatrix(matRotate, (angy * k + (int)3.14));
					D3DMath_MatrixMultiply(matWorld, matRotate, matWorld);

					m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					for (i = 0; i < ((countdisplay / 4)); i += 1) {

						m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX,
						                            (LPVOID)&bubble[i * (int)4], 4, 0);
					}
				}
			}
		}
	}
}

void CMyD3DApplication::dump2(char *u, char *s, float a, float b, float c, float tu, float tv) {
	/*
	FILE* fp;
	//	CTime t;
	//	t = CTime::GetCurrentTime();
	if ((fp = fopen("c:\\tri.log", "a")) == NULL)
	{
	}

	if (strcmp(u, "TRI") == 0)
	{
	    fprintf(fp, "TRITEX %-4.2f %-4.2f %-4.2f %-4.2f %-4.2f\n", a, b, c, tu, tv);
	}
	else if (strcmp(u, "-") == 0)
	{
	    fprintf(fp, "%s\n", "-------------");
	}
	else if (strcmp(u, "TEXTURE") == 0)
	{
	    fprintf(fp, "%s\n", s);
	}

	else
	{
	    fprintf(fp, "%-4.2f %-4.2f %-4.2f %-4.2f %-4.2f\n", a, b, c, tu, tv);
	}
	fclose(fp);
	*/
}

void CMyD3DApplication::debug_me2(char *u, char *s, int a, int b) {

	return;
	FILE *fp;

	if (fopen_s(&fp, "c:\\rr.log", "a") != 0) {
	}

	fprintf(fp, "%-21s %-34s %-4d %-4d\n", u, s, a, b);
	fclose(fp);
}

void CMyD3DApplication::getaddr() {
}

void CMyD3DApplication::CalculateNormals() {

	float x, y, z;
	int i, j;
	float x1, y1, z1;
	D3DVECTOR work;
	D3DVECTOR work1;
	D3DVECTOR result;

	D3DVECTOR sum;
	int counthits;

	for (i = 0; i < cnt / 2; i++) {
		sum = D3DVECTOR(0, 0, 0);
		counthits = 0;
		x = src_v[i].nx;
		y = src_v[i].ny;
		z = src_v[i].nz;
		for (j = 0; j < cnt / 2; j++) {

			x1 = src_v[j].nx;
			y1 = src_v[j].ny;
			z1 = src_v[j].nz;

			if (x1 == x &&
			    y1 == y &&
			    z1 == z) {

				work.x = x;
				work.y = y;
				work.z = z;

				sum = (work + work);
				counthits++;
			}
		}
		src_v[i].nx = sum.x / counthits;
		src_v[i].ny = sum.y / counthits;
		src_v[i].nz = sum.z / counthits;
	}
}

void CMyD3DApplication::MoveMonsters() {
	// 0 = Normal
	// 1 = slave
	// 2 = line of site
	// 3 = dead
	// 4 = statue
	// 5 = 250 distance
	// 6 = fire only

	if (monstermoveon == 0)
		return;

	// Protect against a long game PAUSE and mosnters running off the screen.
	if (fTimeKeysave > 1.0f) {
		fTimeKeysave = 0;
	}

	D3DVECTOR work1, work2, work3, vDiff, vw1, vw2;
	int i;

	D3DVECTOR saveeyept, inarea, inarea2, pIPoint, inarea3;
	int cullloop2 = 0;
	int raction = 0;
	int skipmonster = 0;

	D3DVECTOR result;
	D3DVECTOR savepos;

	int cullloop = 0;
	int cullflag = 0;

	D3DVECTOR collidenow;
	D3DVECTOR normroadold;

	saveeyept = m_vEyePt;
	normroadold.x = 50;
	normroadold.y = 0;
	normroadold.z = 0;

	MakeBoundingBox();

	if (player_list[trueplayernum].bIsPlayerAlive == FALSE)
		return;

	for (i = 0; i < num_monsters; i++) {
		cullflag = 0;
		for (cullloop = 0; cullloop < monstercount; cullloop++) {

			if (monstercull[cullloop] == monster_list[i].monsterid) {
				if (monstertype[cullloop] == 0) {

					// is it cloest to me
					if (monster_list[i].closest == trueplayernum) {
						if (monster_list[i].current_frame == 183 || monster_list[i].current_frame == 189 || monster_list[i].current_frame == 197) {
							monster_list[i].bStopAnimating = TRUE;
						}

						if (monster_list[i].bStopAnimating == FALSE)
							cullflag = 1;

						if (strcmp(monster_list[i].rname, "SLAVE") == 0 || strcmp(monster_list[i].rname, "CENTAUR") == 0 ||
						    monster_list[i].ability == 2 || monster_list[i].ability == 5) {

							D3DVECTOR work2, work1;

							work1.x = player_list[trueplayernum].x;
							work1.y = player_list[trueplayernum].y;
							work1.z = player_list[trueplayernum].z;

							work2.x = monster_list[i].x;
							work2.y = monster_list[i].y;
							work2.z = monster_list[i].z;
							int monsteron = 0;

							monsteron = CalculateViewMonster(work2, work1, 60.0f, monster_list[i].rot_angle);
							// monster see
							if (monsteron) {

								if (monster_list[i].ability == 2)
									monster_list[i].ability = 0;

								int t = 0;
								int cullloop2 = 0;
								for (t = 0; t < num_monsters; t++) {
									for (cullloop2 = 0; cullloop2 < monstercount; cullloop2++) {
										if (monstercull[cullloop2] == monster_list[t].monsterid) {

											if (monster_list[t].ability == 2 &&
											    monster_list[t].dist < 600.0f) {
												monster_list[t].ability = 0;
											}

											if (monster_list[t].ability == 5 &&
											    monster_list[t].dist < 250.0f) {
												monster_list[t].ability = 0;
											}
										}
									}
								}
							}

							cullflag = 0;
						}
					}
				}
				break;
			}
		}

		if (cullflag) {

			if (perspectiveview == 1) {
				work1.x = player_list[trueplayernum].x;
				// work1.y= player_list[trueplayernum].y;
				work1.y = 0.0f;
				work1.z = player_list[trueplayernum].z;
			} else {

				work1.x = m_vLookatPt.x;
				// work1.y= m_vLookatPt.y;
				work1.y = 0.0f;
				work1.z = m_vLookatPt.z;
			}

			work2.x = monster_list[i].x;
			// work2.y= monster_list[i].y;
			work2.y = 0.0f;
			work2.z = monster_list[i].z;
			vDiff = work1 - work2;

			vDiff = Normalize(vDiff);

			currentmonstercollisionid = monster_list[i].monsterid;

			vw1.x = work1.x;
			vw1.y = (float)0;

			vw1.z = work1.z;

			vw2.x = work2.x;
			vw2.y = (float)0;
			vw2.z = work2.z;

			D3DVECTOR vDiff = vw1 - vw2;

			skipmonster = 0;

			if (monster_list[i].current_frame == 50) {
				PlayWavSound(monster_list[i].sweapon, monster_list[i].volume);
			} else {
				monster_list[i].applydamageonce = 0;
			}

			if (Magnitude(vDiff) < 80.0f) {
				skipmonster = 1;

				// if there are close attack right away and set attack speed
				if (monster_list[i].attackspeed <= 0) {

					SetMonsterAnimationSequence(i, 2);
					monster_list[i].attackspeed = 30 - (monster_list[i].hd);
					if (monster_list[i].attackspeed <= 0)
						monster_list[i].attackspeed = 0;
				} else {

					if (maingameloop) {
						monster_list[i].attackspeed--;
						if (monster_list[i].attackspeed <= 0)
							monster_list[i].attackspeed = 0;
					}

					if (monster_list[i].current_frame == 53 ||
					    monster_list[i].current_frame == 83 ||
					    monster_list[i].current_frame == 94 ||
					    monster_list[i].current_frame == 111 ||
					    monster_list[i].current_frame == 122 ||
					    monster_list[i].current_frame == 134) {

						raction = random_num(10);

						if (raction == 0) {
							PlayWavSound(monster_list[i].syell, monster_list[i].volume);
						}

						raction = random_num(5);

						switch (raction) {
						case 0:

							SetMonsterAnimationSequence(i, 7);
							break;
						case 1:

							if (raction != 0) {
								int raction2 = random_num(4);
								if (raction2 == 0)
									PlayWavSound(monster_list[i].syell, monster_list[i].volume);
							}

							SetMonsterAnimationSequence(i, 8);
							break;
						case 2:
							SetMonsterAnimationSequence(i, 9);
							break;
						case 3:

							SetMonsterAnimationSequence(i, 10);
							break;
						case 4:

							if (raction != 0) {
								int raction2 = random_num(3);
								if (raction2 == 0)
									PlayWavSound(monster_list[i].syell, monster_list[i].volume);
							}

							SetMonsterAnimationSequence(i, 11);
							break;
						case 5:
							break;
						}
					}
				}

				if (monster_list[i].current_sequence == 1 || monster_list[i].current_sequence == 0) {

					if (monster_list[i].attackspeed == 0)
						SetMonsterAnimationSequence(i, 2);
				}
				if (monster_list[i].current_sequence == 2 && monster_list[i].current_frame == 53) {

					monster_list[i].attackspeed = 0;
				}

				if (monster_list[i].current_frame >= 50 && monster_list[i].current_frame <= 50) {

					D3DVECTOR work2, work1;

					work1.x = player_list[trueplayernum].x;
					work1.y = player_list[trueplayernum].y;
					work1.z = player_list[trueplayernum].z;

					work2.x = monster_list[i].x;
					work2.y = monster_list[i].y;
					work2.z = monster_list[i].z;
					int monsteron = 0;

					monsteron = CalculateViewMonster(work2, work1, 45.0f, monster_list[i].rot_angle);
					if (monster_list[i].current_sequence != 5 && monsteron && monster_list[i].dist <= 350.0f) {

						int action = random_num(20) + 1;
						if (action >= monster_list[i].thaco - player_list[trueplayernum].ac) {
							if (monster_list[i].applydamageonce == 0) {

								action = random_num(monster_list[i].damage2) + 1;
								sprintf_s(gActionMessage, "A %s hit you for %d damage", monster_list[i].rname, action);
								UpdateScrollList(255, 0, 0);
								StartFlare(1);
								ApplyPlayerDamage(trueplayernum, action);
								monster_list[i].applydamageonce = 1;
							}
						}
					}
				}
			} else {

				if (monster_list[i].current_sequence == 0) {

					raction = random_num(20);
					if (monster_list[i].ability != 6) {

						switch (raction) {
						case 0:
							SetMonsterAnimationSequence(i, 7); // flip
							PlayWavSound(monster_list[i].syell, monster_list[i].volume);

							break;
						case 1:
							SetMonsterAnimationSequence(i, 8);
							break;
						case 2:
							SetMonsterAnimationSequence(i, 9); // Taunt
							PlayWavSound(monster_list[i].syell, monster_list[i].volume);

							break;
						case 3:
							SetMonsterAnimationSequence(i, 10); // Wave
							break;
						case 4:
							SetMonsterAnimationSequence(i, 11); // Point
							PlayWavSound(monster_list[i].syell, monster_list[i].volume);
							break;
						case 5:
							SetMonsterAnimationSequence(i, 0);
							break;
						default:
							SetMonsterAnimationSequence(i, 1);
							break;
						}
					}

					int firetype = 0;
					int raction = 0;
					int mspeed = 0;

					if (
					    strstr(monster_list[i].rname, "OGRE") != NULL ||
					    strstr(monster_list[i].rname, "OGRO") != NULL ||
					    strstr(monster_list[i].rname, "PHANTOM") != NULL) {

						firetype = 1;
					} else if (strstr(monster_list[i].rname, "WRAITH") != NULL ||
					           strstr(monster_list[i].rname, "BAUUL") != NULL ||
					           strstr(monster_list[i].rname, "FAERIE") != NULL ||
					           strstr(monster_list[i].rname, "FULIMO") != NULL ||
					           strstr(monster_list[i].rname, "MUMMY") != NULL ||
					           strstr(monster_list[i].rname, "HYDRA") != NULL ||
					           strstr(monster_list[i].rname, "IMP") != NULL ||
					           strstr(monster_list[i].rname, "SORCERER") != NULL ||
					           strstr(monster_list[i].rname, "NECROMANCER") != NULL ||
					           strstr(monster_list[i].rname, "OKTA") != NULL ||
					           strstr(monster_list[i].rname, "SORB") != NULL) {

						raction = random_num(2) + 1;

						if (raction == 1)
							firetype = 1;
						else if (raction == 2)
							firetype = 2;
					} else if (strstr(monster_list[i].rname, "DEMON") != NULL ||
					           strstr(monster_list[i].rname, "DEMONESS") != NULL ||
					           strstr(monster_list[i].rname, "FAERIE") != NULL) {

						raction = random_num(3) + 1;

						if (raction == 1)
							firetype = 1;
						else if (raction == 2)
							firetype = 2;
						else if (raction == 3)
							firetype = 3;
					}

					if (monster_list[i].dist > 200.0f) {
						if (monster_list[i].firespeed == 0 && firetype > 0) {
							mspeed = (int)25 - (int)monster_list[i].hd;

							if (mspeed < 6)
								mspeed = 6;

							monster_list[i].firespeed = mspeed;
							FireMonsterMissle(i, firetype);
						}
					}
				}
			}

			if (monster_list[i].current_sequence != 1) {

				skipmonster = 1;
			}

			D3DVECTOR final, final2;
			D3DVECTOR m, n;

			final = Normalize(vDiff);
			final2 = Normalize(normroadold);

			float fDot = dot(final, final2);
			float convangle;
			convangle = (float)acos(fDot) / k;
			raction = random_num(1);
			if (raction == 0)
				raction = random_num(5);
			else
				raction = -1 * random_num(5);

			fDot = convangle;

			if (vw2.z < vw1.z) {
			} else {
				fDot = 180.0f + (180.0f - fDot);
			}

			if (monster_list[i].ability == 4) {
				monster_list[i].current_frame = 0;
				monster_list[i].current_sequence = 0;
				skipmonster = 1;
			} else {

				monster_list[i].rot_angle = fDot;
			}

			if (monster_list[i].ability == 6) {
				// shot only
				skipmonster = 1;
			}

			if (skipmonster == 0) {

				collidenow.x = monster_list[i].x;
				collidenow.y = monster_list[i].y;
				collidenow.z = monster_list[i].z;

				inarea = collidenow + final * 2.0f;
				inarea2.x = inarea.z;
				inarea2.z = inarea.x;
				inarea2.y = inarea.y;

				MakeBoundingBox();

				eRadius = D3DVECTOR((float)monsterx[cullloop], monsterheight[cullloop], (float)monsterx[cullloop]);

				savepos = collidenow;

				float realspeed;

				realspeed = (monster_list[i].speed * 10.0f) * fTimeKeysave;

				if (monster_list[i].health <= 2 && monster_list[i].health != monster_list[i].hp && (monster_list[i].hd < player_list[trueplayernum].hd)) {
					// RUN  AWAY!!!!!!!!!!!!!!!!!!!
					final = final * -1;
				}

				D3DVECTOR a = final * realspeed;

				result = collideWithWorld(collidenow / eRadius, (final * realspeed) / eRadius);
				result = result * eRadius;

				monster_list[i].x = result.x;
				monster_list[i].y = result.y;
				monster_list[i].z = result.z;

				MakeBoundingBox();
			}

			if (monster_list[i].current_sequence == 1) {
				MakeBoundingBox();
				final.x = 0.0f;
				final.y = -15.5f;
				// fixed april 2005
				final.y = (float)(-300.0f) * fTimeKeysave;
				final.z = 0.0f;

				collidenow.x = monster_list[i].x;
				collidenow.y = monster_list[i].y;
				collidenow.z = monster_list[i].z;
				eRadius = D3DVECTOR((float)monsterx[cullloop], monsterheight[cullloop], (float)monsterx[cullloop]);

				result = collideWithWorld(collidenow / eRadius, (final) / eRadius);
				result = result * eRadius;
				monster_list[i].x = result.x;
				monster_list[i].y = result.y;
				monster_list[i].z = result.z;

				MakeBoundingBox();
			}
		}
	}

	currentmonstercollisionid = -1;
}

void CMyD3DApplication::MakeBoundingBox() {

	int montry = 0;
	int cullloop = 0;
	int cullflag = 0;
	int i;
	countboundingbox = 0;

	if (excludebox == 0)
		for (i = 0; i < num_players; i++) {
			cullflag = 0;
			for (cullloop = 0; cullloop < monstercount; cullloop++) {
				if (monstercull[cullloop] == (int)i + (int)999) {
					if (monstertype[cullloop] == 5 && i != trueplayernum && player_list[i].bIsPlayerAlive == TRUE) {

						wx = player_list[i].x;
						wy = player_list[i].y;
						wz = player_list[i].z;

						PlayerNonIndexedBox(0, 0, (int)player_list[i].rot_angle);
						monsterheight[cullloop] = objectheight;
						monsterx[cullloop] = objectx;
						monsterz[cullloop] = objectz;
					}
					break;
				}
			}
		}

	for (i = 0; i < num_players2; i++) {
		cullflag = 0;
		for (cullloop = 0; cullloop < monstercount; cullloop++) {
			if (monstercull[cullloop] == player_list2[i].monsterid) {
				if (monstertype[cullloop] == 1) {

					wx = player_list2[i].x;
					wy = player_list2[i].y;
					wz = player_list2[i].z;

					PlayerIndexedBox(monsterobject[cullloop], 0, (int)player_list2[i].rot_angle);

					monsterheight[cullloop] = objectheight;
					monsterx[cullloop] = objectx;
					monsterz[cullloop] = objectz;
				}
				break;
			}
		}
	}

	// heee
	if (excludebox == 0)
		for (i = 0; i < num_monsters; i++) {
			cullflag = 0;
			for (cullloop = 0; cullloop < monstercount; cullloop++) {

				if (monstercull[cullloop] == monster_list[i].monsterid) {

					if (monster_list[i].bIsPlayerAlive == TRUE &&
					    monster_list[i].bIsPlayerValid == TRUE) {

						if (monstertype[cullloop] == 0 && currentmonstercollisionid != monster_list[i].monsterid) {

							wx = monster_list[i].x;
							wy = monster_list[i].y;
							wz = monster_list[i].z;

							PlayerNonIndexedBox(monsterobject[cullloop], 0, (int)monster_list[i].rot_angle);
							monstertrueheight[cullloop] = objecttrueheight;
							monsterheight[cullloop] = objectheight;
							monsterx[cullloop] = objectx;
							monsterz[cullloop] = objectz;
						}
					}
					break;
				}
			}
		}
}

void CMyD3DApplication::PlayerIndexedBox(int pmodel_id, int curr_frame, int angle) {
	int i, j;
	int num_verts_per_poly;
	int num_faces_per_poly;
	int num_poly;
	int poly_command;
	int i_count, face_i_count;
	float x, y, z;
	float rx, ry, rz;
	float tx, ty;
	int count_v = 0;

	float min_x, min_y, min_z;
	float max_x, max_y, max_z;

	x_off = 0;
	y_off = 0;
	z_off = 0;

	min_x = wx;
	min_y = wy;
	min_z = wz;
	max_x = wx;
	max_y = wy;
	max_z = wz;

	D3DVECTOR v1, v2, vw1, vw2, vw3, vw4, vDiff2, vDiff3, vw5;

	if (curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	curr_frame = 0;
	cosine = cos_table[angle];
	sine = sin_table[angle];

	i_count = 0;
	face_i_count = 0;

	//	if(rendering_first_frame == TRUE)
	//		fp = fopen("ds.txt","a");

	// process and transfer the model data from the pmdata structure
	// to the array of D3DVERTEX structures, src_v

	num_poly = pmdata[pmodel_id].num_polys_per_frame;

	for (i = 0; i < num_poly; i++) {
		poly_command = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_verts_per_object[i];
		num_faces_per_poly = pmdata[pmodel_id].num_faces_per_object[i];
		count_v = 0;
		for (j = 0; j < num_verts_per_poly; j++) {

			x = pmdata[pmodel_id].w[curr_frame][i_count].x + x_off;
			z = pmdata[pmodel_id].w[curr_frame][i_count].y + y_off;
			y = pmdata[pmodel_id].w[curr_frame][i_count].z + z_off;

			rx = wx + (x * cosine - z * sine);
			ry = wy + y;
			rz = wz + (x * sine + z * cosine);

			tx = pmdata[pmodel_id].t[i_count].x * pmdata[pmodel_id].skx;
			ty = pmdata[pmodel_id].t[i_count].y * pmdata[pmodel_id].sky;

			if (D3DVAL(rx) > max_x)
				max_x = D3DVAL(rx);
			if (D3DVAL(rx) < min_x)
				min_x = D3DVAL(rx);

			if (D3DVAL(ry) > max_y)
				max_y = D3DVAL(ry);
			if (D3DVAL(ry) < min_y)
				min_y = D3DVAL(ry);

			if (D3DVAL(rz) > max_z)
				max_z = D3DVAL(rz);
			if (D3DVAL(rz) < min_z)
				min_z = D3DVAL(rz);

			i_count++;

		} // end for j

	} // end for vert_cnt

	// got min & max make box

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	// 2nd

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	// 3rd

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	// 4th
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;
	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	// 5th

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;
	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;
	countboundingbox++;

	objectheight = max_y - min_y;
	objectx = max_x - min_x;
	objectz = max_z - min_z;

	return;
}

void CMyD3DApplication::PlayerNonIndexedBox(int pmodel_id, int curr_frame, int angle) {
	int i, j;
	int num_verts_per_poly;
	int num_poly;
	int i_count;
	short v_index;
	float x, y, z;
	float rx, ry, rz;
	float tx, ty;
	int count_v;

	float min_x, min_y, min_z;
	float max_x, max_y, max_z;

	x_off = 0;
	y_off = 0;
	z_off = 0;

	min_x = wx;
	min_y = wy;
	min_z = wz;
	max_x = wx;
	max_y = wy;
	max_z = wz;

	vert_ptr tp;

	D3DPRIMITIVETYPE p_command;
	BOOL error = TRUE;

	D3DVECTOR v1, v2, vw1, vw2, vw3, vw4, vDiff2, vDiff3, vw5;

	if (angle >= 360)
		angle = angle - 360;
	if (angle < 0)
		angle += 360;

	cosine = cos_table[angle];
	sine = sin_table[angle];

	if (curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	i_count = 0;

	num_poly = pmdata[pmodel_id].num_polys_per_frame;

	for (i = 0; i < num_poly; i++) {
		p_command = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_vert[i];

		count_v = 0;

		for (j = 0; j < num_verts_per_poly; j++) {
			v_index = pmdata[pmodel_id].f[i_count];

			tp = &pmdata[pmodel_id].w[curr_frame][v_index];
			x = tp->x + x_off;
			z = tp->y + y_off;
			y = tp->z + z_off;

			rx = wx + (x * cosine - z * sine);
			ry = wy + y;
			rz = wz + (x * sine + z * cosine);

			tx = pmdata[pmodel_id].t[i_count].x * pmdata[pmodel_id].skx;
			ty = pmdata[pmodel_id].t[i_count].y * pmdata[pmodel_id].sky;
			if (D3DVAL(rx) > max_x)
				max_x = D3DVAL(rx);
			if (D3DVAL(rx) < min_x)
				min_x = D3DVAL(rx);

			if (D3DVAL(ry) > max_y)
				max_y = D3DVAL(ry);
			if (D3DVAL(ry) < min_y)
				min_y = D3DVAL(ry);

			if (D3DVAL(rz) > max_z)
				max_z = D3DVAL(rz);
			if (D3DVAL(rz) < min_z)
				min_z = D3DVAL(rz);

			i_count++;
		}
	}

	objectheight = max_y - min_y;
	objectx = max_x - min_x;
	objectz = max_z - min_z;
	objecty = max_y - min_y;

	if (objectx > 80.0f || objectz > 80.0f) {
		objectx = 35.0f;
	} else {
		objectx = 20.0f;
	}

	objecttrueheight = objectheight;
	if (objectheight < 150.0f) {
		objectheight = 50.0f;
	} else {
		objectheight = 70.0f;
	}

	min_x = wx - objectx;
	max_x = wx + objectx;

	min_y = wy - objecty / 2;
	max_y = wy + objecty / 2;

	min_z = wz - objectx;
	max_z = wz + objectx;

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	// 2nd

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	// 3rd

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;

	// 4th
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;

	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;
	countboundingbox++;
	boundingbox[countboundingbox].x = max_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	// 5th

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = max_y;
	boundingbox[countboundingbox].z = min_z;
	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = max_z;

	countboundingbox++;

	boundingbox[countboundingbox].x = min_x;
	boundingbox[countboundingbox].y = min_y;
	boundingbox[countboundingbox].z = min_z;
	countboundingbox++;

	return;
}

void CMyD3DApplication::GetItem() {

	int i = 0;
	int cullflag = 0;
	int cullloop = 0;
	float qdist;
	char level[80];
	int foundsomething = 0;

	for (i = 0; i < itemlistcount; i++) {
		cullflag = 0;
		for (cullloop = 0; cullloop < monstercount; cullloop++) {
			if (monstercull[cullloop] == item_list[i].monsterid) {
				cullflag = 1;
				break;
			}
		}

		if (item_list[i].monsterid == 9999 && item_list[i].bIsPlayerAlive == TRUE)
			cullflag = 1;

		if (cullflag == 1) {

			wx = item_list[i].x;
			wy = item_list[i].y;
			wz = item_list[i].z;

			if (perspectiveview == 0) {
				qdist = FastDistance(
				    m_vLookatPt.x - wx,
				    m_vLookatPt.y - wy,
				    m_vLookatPt.z - wz);
			} else {
				qdist = FastDistance(m_vEyePt.x - wx,
				                     m_vEyePt.y - wy,
				                     m_vEyePt.z - wz);
			}

			if (qdist < 70.0f) {

				if (strcmp(item_list[i].rname, "spiral") == 0) {

					foundsomething = 1;
					pCMyApp->num_players2 = 0;
					pCMyApp->itemlistcount = 0;
					pCMyApp->num_monsters = 0;

					pCMyApp->ClearObjectList();
					ResetSound();
					pCWorld->LoadSoundFiles(m_hWnd, "sounds.dat");

					sprintf_s(levelname, "level%d", item_list[i].ability);
					sprintf_s(gActionMessage, "Loading %s...", levelname);
					UpdateScrollList(0, 245, 255);
					current_level = item_list[i].ability;
					strcpy_s(level, levelname);
					strcat_s(level, ".map");

					if (!pCWorld->LoadWorldMap(m_hWnd, level)) {
						// PrintMessage(m_hWnd, "LoadWorldMap failed", NULL, LOGFILE_ONLY);
						// return FALSE;
					}

					strcpy_s(level, levelname);
					strcat_s(level, ".cmp");

					if (!pCWorld->InitPreCompiledWorldMap(m_hWnd, level)) {
						// PrintMessage(m_hWnd, "InitPreCompiledWorldMap failed", NULL, LOGFILE_ONLY);
						// return FALSE;
					}

					strcpy_s(level, levelname);
					strcat_s(level, ".mod");

					pCWorld->LoadMod(m_hWnd, level);

					ResetPlayer();
					SetStartSpot();
					PlayWavSound(SoundID("goal2"), 100);
					StartFlare(2);
				} else if (strcmp(item_list[i].rname, "AXE") == 0 ||
				           strcmp(item_list[i].rname, "FLAMESWORD") == 0 ||
				           strcmp(item_list[i].rname, "BASTARDSWORD") == 0 ||
				           strcmp(item_list[i].rname, "BATTLEAXE") == 0 ||
				           strcmp(item_list[i].rname, "ICESWORD") == 0 ||
				           strcmp(item_list[i].rname, "MORNINGSTAR") == 0 ||
				           strcmp(item_list[i].rname, "SPIKEDFLAIL") == 0 ||
				           strcmp(item_list[i].rname, "SPLITSWORD") == 0 ||
				           strcmp(item_list[i].rname, "SUPERFLAMESWORD") == 0 ||
				           strcmp(item_list[i].rname, "LIGHTNINGSWORD") == 0 ||
				           strstr(item_list[i].rname, "SCROLL") != NULL) {

					foundsomething = 1;

					if (strstr(item_list[i].rname, "SCROLL") != NULL) {

						if (strstr(item_list[i].rname, "SCROLL-MAGICMISSLE") != NULL) {
							sprintf_s(gActionMessage, "You found a scroll of magic missle");
						} else if (strstr(item_list[i].rname, "SCROLL-FIREBALL") != NULL) {
							sprintf_s(gActionMessage, "You found a scroll of fireball");
						} else if (strstr(item_list[i].rname, "SCROLL-LIGHTNING") != NULL) {
							sprintf_s(gActionMessage, "You found a scroll of lightning");
						} else if (strstr(item_list[i].rname, "SCROLL-HEALING") != NULL) {
							sprintf_s(gActionMessage, "You found a scroll of healing");
						}
					} else {

						sprintf_s(gActionMessage, "You found a %s", item_list[i].rname);
					}
					UpdateScrollList(0, 245, 255);

					// you got something good! (weapon)
					item_list[i].bIsPlayerAlive = FALSE;
					PlayWavSound(SoundID("potion"), 100);

					for (int q = 0; q <= num_your_guns; q++) {
						char junk[255];

						strcpy_s(junk, item_list[i].rname);
						junk[strlen(junk) - 1] = '\0';

						if (your_gun[q].model_id == item_list[i].model_id ||
						    strstr(your_gun[q].gunname, junk) != NULL &&
						        strstr(your_gun[q].gunname, "SCROLL") != NULL) {
							your_gun[q].active = 1;
							your_gun[q].x_offset = your_gun[q].x_offset + 1;
						}
					}
				} else if (strcmp(item_list[i].rname, "POTION") == 0) {

					if (player_list[trueplayernum].health < player_list[trueplayernum].hp) {
						item_list[i].bIsPlayerAlive = FALSE;
						item_list[i].bIsPlayerValid = FALSE;
						int hp = random_num(8) + 1;
						player_list[trueplayernum].health = player_list[trueplayernum].health + hp;

						if (player_list[trueplayernum].health > player_list[trueplayernum].hp)
							player_list[trueplayernum].health = player_list[trueplayernum].hp;

						PlayWavSound(SoundID("potion"), 100);
						StartFlare(3);
						sprintf_s(gActionMessage, "You found a potion worth %d health", hp);
						UpdateScrollList(0, 255, 255);
						foundsomething = 1;
						LevelUp(player_list[trueplayernum].xp);
					}
				} else if (strcmp(item_list[i].rname, "cheese1") == 0) {

					if (player_list[trueplayernum].health < player_list[trueplayernum].hp) {
						item_list[i].bIsPlayerAlive = FALSE;
						item_list[i].bIsPlayerValid = FALSE;
						int hp = random_num(3) + 1;
						player_list[trueplayernum].health = player_list[trueplayernum].health + hp;

						if (player_list[trueplayernum].health > player_list[trueplayernum].hp)
							player_list[trueplayernum].health = player_list[trueplayernum].hp;

						PlayWavSound(SoundID("potion"), 100);
						StartFlare(3);
						sprintf_s(gActionMessage, "You found some cheese %d health", hp);
						UpdateScrollList(0, 255, 255);
						foundsomething = 1;
						LevelUp(player_list[trueplayernum].xp);
					}
				} else if (strcmp(item_list[i].rname, "bread1") == 0) {

					if (player_list[trueplayernum].health < player_list[trueplayernum].hp) {
						item_list[i].bIsPlayerAlive = FALSE;
						item_list[i].bIsPlayerValid = FALSE;
						int hp = random_num(5) + 1;
						player_list[trueplayernum].health = player_list[trueplayernum].health + hp;

						if (player_list[trueplayernum].health > player_list[trueplayernum].hp)
							player_list[trueplayernum].health = player_list[trueplayernum].hp;

						PlayWavSound(SoundID("potion"), 100);
						StartFlare(3);
						sprintf_s(gActionMessage, "You found some bread %d health", hp);
						UpdateScrollList(0, 245, 255);
						foundsomething = 1;
						LevelUp(player_list[trueplayernum].xp);
					}
				}

				else if (strcmp(item_list[i].rname, "COIN") == 0) {
					if (item_list[i].gold == 0)
						item_list[i].gold = 5;

					player_list[trueplayernum].gold += item_list[i].gold;
					player_list[trueplayernum].xp += item_list[i].gold;
					LevelUp(player_list[trueplayernum].xp);
					PlayWavSound(SoundID("coin"), 100);
					item_list[i].bIsPlayerAlive = FALSE;
					item_list[i].bIsPlayerValid = FALSE;

					foundsomething = 1;
					sprintf_s(gActionMessage, "You found %d coin", item_list[i].gold);
					UpdateScrollList(0, 255, 100);
					LevelUp(player_list[trueplayernum].xp);
				} else if (strcmp(item_list[i].rname, "GOBLET") == 0) {

					if (item_list[i].gold == 0)
						item_list[i].gold = 100;

					player_list[trueplayernum].gold += item_list[i].gold;
					player_list[trueplayernum].xp += item_list[i].gold;
					PlayWavSound(SoundID("coin"), 100);
					item_list[i].bIsPlayerAlive = FALSE;
					item_list[i].bIsPlayerValid = FALSE;

					foundsomething = 1;
					sprintf_s(gActionMessage, "You found a goblet worth %d coin", item_list[i].gold);
					UpdateScrollList(0, 245, 255);
					LevelUp(player_list[trueplayernum].xp);
				}

				else if (strcmp(item_list[i].rname, "KEY2") == 0) {

					player_list[trueplayernum].keys++;
					player_list[trueplayernum].xp += 10;
					PlayWavSound(SoundID("potion"), 100);
					item_list[i].bIsPlayerAlive = FALSE;
					item_list[i].bIsPlayerValid = FALSE;

					foundsomething = 1;
					sprintf_s(gActionMessage, "You found a key");
					UpdateScrollList(0, 245, 255);
					LevelUp(player_list[trueplayernum].xp);
				} else if (strcmp(item_list[i].rname, "diamond") == 0) {

					player_list[trueplayernum].gold += item_list[i].gold;
					player_list[trueplayernum].xp += item_list[i].gold;
					PlayWavSound(SoundID("coin"), 100);
					item_list[i].bIsPlayerAlive = FALSE;
					item_list[i].bIsPlayerValid = FALSE;
					foundsomething = 1;
					sprintf_s(gActionMessage, "You found a diamond worth %d coin", item_list[i].gold);
					UpdateScrollList(0, 245, 255);
					LevelUp(player_list[trueplayernum].xp);
				} else if (strcmp(item_list[i].rname, "armour") == 0) {

					player_list[trueplayernum].xp += 10;
					player_list[trueplayernum].ac = player_list[trueplayernum].ac - 1;
					foundsomething = 1;
					sprintf_s(gActionMessage, "You found some armour");
					UpdateScrollList(0, 245, 255);
					StartFlare(2);
					PlayWavSound(SoundID("potion"), 100);
					PlayWavSound(SoundID("goal4"), 100);
					item_list[i].bIsPlayerAlive = FALSE;
					item_list[i].bIsPlayerValid = FALSE;
				}
			}
		}
	}
}

void CMyD3DApplication::MonsterHit() {

	int j = 0;

	int i = 0;
	int cullflag = 0;
	int cullloop = 0;
	float qdist;
	int action;
	int actionnum = 0;
	int criticaldamage = 0;
	char savehp[200];

	int hitplayer = 0;
	if (player_list[trueplayernum].current_frame >= 50 && player_list[trueplayernum].current_frame <= 50) {

		action = random_num(dice[0].sides) + 1;
		actionnum = action;
		dice[0].roll = 0;
		sprintf_s(dice[0].name, "%ss%d", dice[0].prefix, action);

		dice[1].roll = 0;
		sprintf_s(dice[1].name, "dieblank");
	}

	cullflag = 0;
	float lastdist = 99999;
	int savepos = -1;
	for (i = 0; i < num_monsters; i++) {

		for (cullloop = 0; cullloop < monstercount; cullloop++) {

			if (monstercull[cullloop] == monster_list[i].monsterid) {

				statusbardisplay((float)monster_list[i].hp, (float)monster_list[i].hp, 1);
				strcpy_s(savehp, statusbar);
				statusbardisplay((float)monster_list[i].health, (float)monster_list[i].hp, 0);

				for (j = 0; j < (int)strlen(statusbar); j++) {

					savehp[j] = statusbar[j];
				}

				// monsterhitdisplay

				sprintf_s(monster_list[i].chatstr, "%s AC:%d!HP:%d %s",
				          monster_list[i].rname,
				          player_list[trueplayernum].thaco - monster_list[i].ac,
				          monster_list[i].health, savehp);

				if (monster_list[i].current_sequence != 5 &&
				    monster_list[i].current_frame != 183 &&
				    monster_list[i].current_frame != 189 &&
				    monster_list[i].current_frame != 197 && monster_list[i].health > 0) {
					if (monster_list[i].dist < lastdist && monster_list[i].bIsPlayerAlive == TRUE &&
					    strcmp(monster_list[i].rname, "SLAVE") != 0 &&
					    strcmp(monster_list[i].rname, "CENTAUR") != 0) {
						cullflag = 1;
						lastdist = monster_list[i].dist;
						savepos = i;
					}
				}
			}
		}
	}

	if (savepos == -1) {

		if (actionnum != 0)
			PlayerHit(actionnum);
		return;
	}

	i = savepos;

	if (cullflag == 1) {

		wx = monster_list[i].x;
		wy = monster_list[i].y;
		wz = monster_list[i].z;

		if (perspectiveview == 0) {
			qdist = FastDistance(
			    m_vLookatPt.x - wx,
			    m_vLookatPt.y - wy,
			    m_vLookatPt.z - wz);
		} else {
			qdist = FastDistance(
			    m_vEyePt.x - wx,
			    m_vEyePt.y - wy,
			    m_vEyePt.z - wz);
		}

		if (qdist < 100.0f && player_list[trueplayernum].current_frame >= 50 && player_list[trueplayernum].current_frame <= 50) {

			D3DVECTOR work2, work1;

			hitplayer = 1;
			work1.x = wx;
			work1.y = wy;
			work1.z = wz;
			int monsteron = 0;
			if (perspectiveview == 0)
				monsteron = CalculateView(m_vLookatPt, work1, 45.0f);
			else
				monsteron = CalculateView(m_vEyePt, work1, 45.0f);
			if (player_list[trueplayernum].current_sequence != 5 && monsteron) {

				action = random_num(dice[0].sides) + 1;
				actionnum = action;
				if (action == 20)
					criticaldamage = 1;

				dice[0].roll = 0;
				sprintf_s(dice[0].name, "%ss%d", dice[0].prefix, action);

				int attackbonus;
				int damagebonus;
				int weapondamage;

				int a;

				int gunmodel = 0;
				for (a = 0; a < num_your_guns; a++) {

					if (your_gun[a].model_id == player_list[trueplayernum].gunid) {

						gunmodel = a;
					}
				}

				attackbonus = your_gun[gunmodel].sattack;
				damagebonus = your_gun[gunmodel].sdamage;
				weapondamage = your_gun[gunmodel].damage2;
				action = action + attackbonus;

				if (action >= player_list[trueplayernum].thaco - monster_list[i].ac) {
					action = random_num(weapondamage) + 1;

					int painaction = random_num(3);
					SetMonsterAnimationSequence(i, 3 + painaction);

					dice[1].roll = 0;
					sprintf_s(dice[1].name, "%ss%d", dice[1].prefix, action);

					int bloodspot = -1;

					if (criticaldamage == 1) {

						action = (action + (damagebonus)) * 2;
						criticalhiton = 1;
						PlayWavSound(SoundID("natural20"), 100);
						sprintf_s(gActionMessage, "Wow. A natural 20, double damage.");
						UpdateScrollList(0, 245, 255);
						sprintf_s(gActionMessage, "You hit a  %s for %d damage", monster_list[i].rname, action);
						UpdateScrollList(0, 245, 255);
						bloodspot = DisplayDamage(monster_list[i].x, monster_list[i].y - 10.0f, monster_list[i].z, trueplayernum, i, true);
						hitmonster = 1;
					} else {
						action = action + damagebonus;
						sprintf_s(gActionMessage, "You hit a %s for %d damage", monster_list[i].rname, action);
						UpdateScrollList(0, 245, 255);
						bloodspot = DisplayDamage(monster_list[i].x, monster_list[i].y - 10.0f, monster_list[i].z, trueplayernum, i, false);
						hitmonster = 1;
					}

					monster_list[i].health = monster_list[i].health - (int)action;
					monster_list[i].ability = 0;
					int t = 0;
					int cullloop2 = 0;
					// monstersee
					for (t = 0; t < num_monsters; t++) {
						for (cullloop2 = 0; cullloop2 < monstercount; cullloop2++) {
							if (monstercull[cullloop2] == monster_list[t].monsterid) {

								if (monster_list[t].ability == 2 &&
								    monster_list[t].dist < 600.0f

								) {
									monster_list[t].ability = 0;
								}
							}
						}
					}

					PlayWavSound(monster_list[i].sattack, 100);

					if (monster_list[i].health <= 0) {
						if (bloodspot != -1) {
							your_missle[bloodspot].y = your_missle[bloodspot].y - 30.0f;
							your_missle[bloodspot].active = 0;
						}
						PlayWavSound(monster_list[i].sdie, 100);

						int deathaction = random_num(3);

						SetMonsterAnimationSequence(i, 12 + deathaction);
						monster_list[i].bIsPlayerAlive = FALSE;
						monster_list[i].bIsPlayerValid = FALSE;
						monster_list[i].health = 0;
						int gd = (random_num(monster_list[i].hd * 10) + monster_list[i].hd * 10) + 1;
						int xp = XpPoints(monster_list[i].hd, monster_list[i].hp);

						sprintf_s(gActionMessage, "You killed a %s worth %d xp.", monster_list[i].rname, xp);
						UpdateScrollList(0, 245, 255);
						player_list[trueplayernum].frags++;

						AddTreasure(monster_list[i].x, monster_list[i].y, monster_list[i].z, gd);

						player_list[trueplayernum].xp += xp;
						LevelUp(player_list[trueplayernum].xp);
					}

					statusbardisplay((float)monster_list[i].hp, (float)monster_list[i].hp, 1);
					strcpy_s(savehp, statusbar);
					statusbardisplay((float)monster_list[i].health, (float)monster_list[i].hp, 0);

					for (j = 0; j < (int)strlen(statusbar); j++) {

						savehp[j] = statusbar[j];
					}

					sprintf_s(monster_list[i].chatstr, "%s AC:%d!HP:%d %s",
					          monster_list[i].rname,
					          player_list[trueplayernum].thaco - monster_list[i].ac,
					          monster_list[i].health, savehp);

					return;
				}
			}
		}
	}

	if (hitplayer == 0)
		PlayerHit(actionnum);
}

void CMyD3DApplication::PlayerHit(int actionnum) {

	int j = 0;

	int i = 0;
	int cullflag = 0;
	int cullloop = 0;
	float qdist;
	int action;
	int criticaldamage = 0;
	char savehp[200];

	cullflag = 0;
	float lastdist = 99999;
	int savepos = -1;
	for (i = 0; i < num_players; i++) {

		for (cullloop = 0; cullloop < monstercount; cullloop++) {

			if (monstercull[cullloop] == (int)i + (int)999) {

				statusbardisplay((float)player_list[i].hp, (float)player_list[i].hp, 1);
				strcpy_s(savehp, statusbar);
				statusbardisplay((float)player_list[i].health, (float)player_list[i].hp, 0);

				for (j = 0; j < (int)strlen(statusbar); j++) {

					savehp[j] = statusbar[j];
				}

				sprintf_s(player_list[i].chatstr, "%s: %s", player_list[i].name, savehp);

				if (player_list[i].current_sequence != 5 &&
				    player_list[i].current_frame != 183 &&
				    player_list[i].current_frame != 189 &&
				    player_list[i].current_frame != 197 && player_list[i].health > 0) {

					if (player_list[i].dist < lastdist && player_list[i].bIsPlayerAlive == TRUE) {
						cullflag = 1;
						lastdist = player_list[i].dist;
						savepos = i;
					}
				}
			}
		}
	}

	if (savepos == -1)
		return;

	i = savepos;

	if (cullflag == 1) {

		wx = player_list[i].x;
		wy = player_list[i].y;
		wz = player_list[i].z;

		if (perspectiveview == 0) {
			qdist = FastDistance(
			    m_vLookatPt.x - wx,
			    m_vLookatPt.y - wy,
			    m_vLookatPt.z - wz);
		} else {
			qdist = FastDistance(
			    m_vEyePt.x - wx,
			    m_vEyePt.y - wy,
			    m_vEyePt.z - wz);
		}

		if (qdist < 100.0f && player_list[trueplayernum].current_frame >= 50 && player_list[trueplayernum].current_frame <= 50) {

			D3DVECTOR work2, work1;

			work1.x = wx;
			work1.y = wy;
			work1.z = wz;
			int monsteron = 0;
			if (perspectiveview == 0)
				monsteron = CalculateView(m_vLookatPt, work1, 45.0f);
			else
				monsteron = CalculateView(m_vEyePt, work1, 45.0f);
			if (player_list[trueplayernum].current_sequence != 5 && monsteron) {

				action = actionnum;

				if (action == 20)
					criticaldamage = 1;

				dice[0].roll = 0;
				sprintf_s(dice[0].name, "%ss%d", dice[0].prefix, action);

				int attackbonus;
				int damagebonus;
				int weapondamage;

				int a;

				int gunmodel = 0;
				for (a = 0; a < num_your_guns; a++) {

					if (your_gun[a].model_id == player_list[trueplayernum].gunid) {

						gunmodel = a;
					}
				}

				attackbonus = your_gun[gunmodel].sattack;
				damagebonus = your_gun[gunmodel].sdamage;
				weapondamage = your_gun[gunmodel].damage2;
				action = actionnum + attackbonus;
				//					action=999;
				if (action >= player_list[trueplayernum].thaco - player_list[i].ac) {
					action = random_num(weapondamage) + 1;

					int painaction = random_num(3);
					SetPlayerAnimationSequence(i, 3 + painaction);

					dice[1].roll = 0;
					sprintf_s(dice[1].name, "%ss%d", dice[1].prefix, action);

					if (criticaldamage == 1) {
						action = (action + (damagebonus)) * 2;
						criticalhiton = 1;
						PlayWavSound(SoundID("natural20"), 100);
						sprintf_s(gActionMessage, "%s rolled a natural 20, double damage.", player_list[trueplayernum].name);
						UpdateScrollList(0, 245, 255);
						sprintf_s(gActionMessage, "%s hit %s for %d damage", player_list[trueplayernum].name, player_list[i].name, action);
						UpdateScrollList(0, 245, 255);
					} else {
						action = action + damagebonus;
						sprintf_s(gActionMessage, "%s hit %s for %d damage", player_list[trueplayernum].name, player_list[i].name, action);
						UpdateScrollList(0, 245, 255);
					}

					player_list[i].health = player_list[i].health - (int)action;

					PlayWavSound(player_list[i].sattack, 100);

					if (player_list[i].health <= 0) {

						PlayWavSound(player_list[i].sdie, 100);

						int deathaction = random_num(3);
						SetPlayerAnimationSequence(i, 12 + deathaction);
						player_list[i].bIsPlayerAlive = FALSE;

						player_list[i].health = 0;
						int gd = (random_num(player_list[i].hd * 10) + player_list[i].hd * 10) + 1;
						int xp = XpPoints(player_list[i].hd, player_list[i].hp);
						sprintf_s(gActionMessage, "%s killed  %s worth %d xp.", player_list[trueplayernum].name, player_list[i].name, xp);
						UpdateScrollList(0, 245, 255);

						AddTreasure(player_list[i].x, player_list[i].y, player_list[i].z, gd);

						player_list[trueplayernum].xp += xp;
						LevelUp(player_list[trueplayernum].xp);
						player_list[trueplayernum].frags++;
					}

					statusbardisplay((float)player_list[i].hp, (float)player_list[i].hp, 1);
					strcpy_s(savehp, statusbar);
					statusbardisplay((float)player_list[i].health, (float)player_list[i].hp, 0);

					for (j = 0; j < (int)strlen(statusbar); j++) {

						savehp[j] = statusbar[j];
					}

					sprintf_s(player_list[i].chatstr, "%s: %s", player_list[i].name, savehp);

					return;
				}
			}
		}
	}
}

float fixangle(float angle, float adjust) {

	float chunka = 0;

	if (adjust >= 0) {
		angle = angle + adjust;
		if (angle >= 360) {
			chunka = (float)fabs((float)angle) - 360.0f;
			if (chunka < 0)
				chunka = 0;
			angle = chunka;
		}
	} else {
		if (angle + adjust < 0) {

			chunka = adjust + angle;
			chunka = 360.0f - (float)fabs((float)chunka);
			if (chunka > 360)
				chunka = 360;

			angle = chunka;
		} else
			angle = angle + adjust;
	}

	return angle;
}

int CMyD3DApplication::CalculateView(D3DVECTOR EyeBall, D3DVECTOR LookPoint, float angle) {

	D3DVECTOR final, final2;
	D3DVECTOR m, n, vw1, vw2, work2, work1, vDiff, normroadold;
	float fDot;
	float convangle;
	float anglework = 0;
	int monsteron = 0;

	work2.x = EyeBall.x;
	work2.y = EyeBall.y;
	work2.z = EyeBall.z;

	if (outside == 1) {
		angle = angle + 40.0f;
	}

	work1.x = LookPoint.x;
	work1.y = LookPoint.y;
	work1.z = LookPoint.z;
	vDiff = work1 - work2;

	vDiff = Normalize(vDiff);

	vw1.x = work1.x;
	vw1.y = (float)0;
	vw1.z = work1.z;

	vw2.x = work2.x;
	vw2.y = (float)0;
	vw2.z = work2.z;

	vDiff = vw1 - vw2;

	normroadold.x = 50;
	normroadold.y = 0;
	normroadold.z = 0;

	final = Normalize(vDiff);
	final2 = Normalize(normroadold);

	fDot = dot(final, final2);

	convangle = (float)acos(fDot) / k;

	fDot = convangle;

	if (vw2.z < vw1.z) {
	} else {
		fDot = (180.0f + (180.0f - fDot));
	}

	fDot = (360.00f) - fDot;

	anglework = fixangle(fDot, +90.0f);

	if (angy + angle >= 360.0f) {
		if (anglework <= fixangle(angy, +angle) && anglework >= 0.0f) {
			monsteron = 1;
		}
		if (anglework >= fixangle(angy, -angle) && anglework <= 360.0f) {
			monsteron = 1;
		}
	} else if (angy - angle <= 0.0f) {
		if (anglework <= fixangle(angy, +angle) && anglework >= 0.0f) {
			monsteron = 1;
		}
		if (anglework >= fixangle(angy, -angle) && anglework <= 360.0f) {
			monsteron = 1;
		}
	} else {

		if (anglework <= fixangle(angy, +angle) && anglework >= fixangle(angy, -angle))
			monsteron = 1;
	}

	return monsteron;
}

int CMyD3DApplication::CalculateViewMonster(D3DVECTOR EyeBall, D3DVECTOR LookPoint, float angle, float angy) {

	//	return 1;
	D3DVECTOR final, final2;
	D3DVECTOR m, n, vw1, vw2, work2, work1, vDiff, normroadold;
	float fDot;
	float convangle;
	float anglework = 0;
	int monsteron = 0;

	work2.x = EyeBall.x;
	work2.y = EyeBall.y;
	work2.z = EyeBall.z;

	work1.x = LookPoint.x;
	work1.y = LookPoint.y;
	work1.z = LookPoint.z;
	vDiff = work1 - work2;

	vDiff = Normalize(vDiff);

	vw1.x = work1.x;
	vw1.y = (float)0;
	vw1.z = work1.z;

	vw2.x = work2.x;
	vw2.y = (float)0;
	vw2.z = work2.z;

	vDiff = vw1 - vw2;

	normroadold.x = 50;
	normroadold.y = 0;
	normroadold.z = 0;

	final = Normalize(vDiff);
	final2 = Normalize(normroadold);

	fDot = dot(final, final2);

	convangle = (float)acos(fDot) / k;

	fDot = convangle;

	if (vw2.z < vw1.z) {
	} else {
		fDot = (180.0f + (180.0f - fDot));
	}

	anglework = fDot;

	if (angy + angle >= 360.0f) {
		if (anglework <= fixangle(angy, +angle) && anglework >= 0.0f) {
			monsteron = 1;
		}
		if (anglework >= fixangle(angy, -angle) && anglework <= 360.0f) {
			monsteron = 1;
		}
	} else if (angy - angle <= 0.0f) {
		if (anglework <= fixangle(angy, +angle) && anglework >= 0.0f) {
			monsteron = 1;
		}
		if (anglework >= fixangle(angy, -angle) && anglework <= 360.0f) {
			monsteron = 1;
		}
	} else {

		if (anglework <= fixangle(angy, +angle) && anglework >= fixangle(angy, -angle))
			monsteron = 1;
	}

	return monsteron;
}

int CMyD3DApplication::random_num(int num) {

	UINT rndNum;

	rndNum = rand() % num;

	return rndNum;
}

void SortPoly(int num_poly) {

	// holy it's fast
	/*
	    udword Nb = num_poly;
	    for(udword i=0;i<Nb;i++)
	    {
	        InputValues[i] = psort[ i].dist;
	    }

	    Sorted = RS.Sort(InputValues, Nb).GetIndices();
	*/

	udword Nb = num_poly;
	for (udword i = 0; i < Nb; i++) {
		InputValues[i] = (float)psort[i].texture;
	}

	Sorted = RS.Sort(InputValues, Nb).GetIndices();

	return;
}

int CMyD3DApplication::SceneInBox(D3DVECTOR point) {

	float xp[5];
	float yp[5];

	float mx[5];

	float mz[5];

	float eyex;
	float eyez;
	float boxsize = 810.0f;
	float pntx;
	float pnty;
	int result;
	float xx, zz;
	float cosine, sine;
	D3DVECTOR work1;

	eyex = m_vLookatPt.x;
	eyez = m_vLookatPt.z;

	eyex = 0;
	eyez = 0;

	int angle = 0;

	angle = (int)360 - (int)angy;

	cosine = cos_table[angle];
	sine = sin_table[angle];

	// left side of me
	xp[0] = eyex - 1100.0f;
	xp[1] = eyex - 800.0f;
	// right side of me
	xp[2] = eyex + 800.0f;
	xp[3] = eyex + 1100.0f;

	// behind me
	yp[1] = eyez - 900.0f;
	yp[2] = eyez - 900.0f;

	// front of me
	yp[3] = eyez + 1900.0f;
	yp[0] = eyez + 1900.0f;

	if (outside == 1) {
		// left side of me
		xp[0] = eyex - 75900.0f;
		xp[1] = eyex - 75600.0f;
		// right side of me
		xp[2] = eyex + 75600.0f;
		xp[3] = eyex + 75900.0f;

		// front of me
		yp[0] = eyez + 75900.0f;
		// behind me
		yp[1] = eyez - 75750.0f;
		yp[2] = eyez - 75750.0f;
		// front of me
		yp[3] = eyez + 75900.0f;
	}

	pntx = point.x;
	pnty = point.z;

	xx = m_vLookatPt.x;
	zz = m_vLookatPt.z;

	mx[0] = xx + (xp[0] * cosine - yp[0] * sine);
	mz[0] = zz + (xp[0] * sine + yp[0] * cosine);

	mx[1] = xx + (xp[1] * cosine - yp[1] * sine);
	mz[1] = zz + (xp[1] * sine + yp[1] * cosine);

	mx[2] = xx + (xp[2] * cosine - yp[2] * sine);
	mz[2] = zz + (xp[2] * sine + yp[2] * cosine);

	mx[3] = xx + (xp[3] * cosine - yp[3] * sine);
	mz[3] = zz + (xp[3] * sine + yp[3] * cosine);

	showview[0].x = mx[2];
	showview[0].y = m_vLookatPt.y;
	showview[0].z = mz[2];
	showview[0].nx = 0;
	showview[0].ny = 1.0f;
	showview[0].nz = 0;
	showview[0].tu = 0;
	showview[0].tv = 0;

	showview[1].x = mx[1];
	showview[1].y = m_vLookatPt.y;
	showview[1].z = mz[1];
	showview[1].nx = 0;
	showview[1].ny = 1.0f;
	showview[1].nz = 0;
	showview[1].tu = 1.0f;
	showview[1].tv = 0;

	showview[2].x = mx[3];
	showview[2].y = m_vLookatPt.y;
	showview[2].z = mz[3];
	showview[2].nx = 0;
	showview[2].ny = 1.0f;
	showview[2].nz = 0;
	showview[2].tu = 0;
	showview[2].tv = 1.0f;

	showview[3].x = mx[0];
	showview[3].y = m_vLookatPt.y;
	showview[3].z = mz[0];
	showview[3].nx = 0;
	showview[3].ny = 1.0f;
	showview[3].nz = 0;
	showview[3].tu = 1.0f;
	showview[3].tv = 1.0f;

	result = pnpoly(4, mx, mz, pntx, pnty);

	if (result)
		return 1;

	return 0;
}

void CMyD3DApplication::MoveCamera() {

	D3DVECTOR work1, work2, work3, vDiff, vw1, vw2;

	D3DVECTOR saveeyept, inarea, inarea2, pIPoint, inarea3;
	int cullloop2 = 0;
	int raction = 0;
	int skipmonster = 0;
	float magn = 0;
	D3DVECTOR movef;
	D3DVECTOR result;
	float limit = 5.0f;
	D3DVECTOR eyes;

	int cullloop = 0;
	int cullflag = 0;

	D3DVECTOR collidenow;
	D3DVECTOR normroadold;

	if (fTimeKeysave > 0.2f) {
		cameraf.x = m_vEyePt.x;
		cameraf.y = m_vEyePt.y;
		cameraf.z = m_vEyePt.z;

		return;
	}

	normroadold.x = 50;
	normroadold.y = 0;
	normroadold.z = 0;

	work1.x = m_vEyePt.x;
	work1.y = m_vEyePt.y;
	work1.z = m_vEyePt.z;

	work2.x = cameraf.x;
	work2.y = cameraf.y;
	work2.z = cameraf.z;

	vDiff = work1 - work2;
	vDiff = Normalize(vDiff);

	vw1.x = work1.x;
	vw1.y = work1.y;
	vw1.z = work1.z;

	vw2.x = work2.x;
	vw2.y = work2.y;
	vw2.z = work2.z;

	vDiff = vw1 - vw2;

	magn = Magnitude(vDiff);

	if (magn < 1.0f) {
		// sprintf_s(gActionMessage,"LESS 5 magn %10.10f",magn);
		// UpdateScrollList(0,245,255);
	}

	else {
		D3DVECTOR final, final2;
		D3DVECTOR m, n;

		final = Normalize(vDiff);
		final2 = Normalize(normroadold);

		float fDot = dot(final, final2);
		float convangle;
		convangle = (float)acos(fDot) / k;

		fDot = convangle;

		if (vw2.z < vw1.z) {
		} else {
			fDot = 180.0f + (180.0f - fDot);
		}

		cameraf.dir = fDot;

		collidenow.x = cameraf.x;
		collidenow.y = cameraf.y;
		collidenow.z = cameraf.z;

		MakeBoundingBox();
		eRadius = D3DVECTOR(5.0f, 5.0f, 5.0f);

		float speed = magn / 3;

		D3DVECTOR vscale;
		float vsize = 0;
		float vspeed = 0;
		vscale = Normalize(final);
		vsize = Magnitude(vscale);

		vspeed = 5.0f * magn;

		movef = ((vspeed * Normalize(final)) * (fTimeKeysave));

		if (magn > 100.0f) {
			result = collidenow + (movef);
		} else {
			result = collideWithWorld(collidenow / eRadius, (movef) / eRadius);
			result = result * eRadius;
		}

		cameraf.x = result.x;
		cameraf.y = result.y;
		cameraf.z = result.z;
	}
}

int CMyD3DApplication::CycleBitMap(int i) {

	char texname[200];
	char junk[200];
	char junk2[200];

	char *p;
	int talias;
	int tnum;
	int num = 0;
	int count = 0;
	int result;

	talias = i;

	strcpy_s(texname, TexMap[talias].tex_alias_name);

	p = strstr(texname, "@");

	if (p != NULL) {
		if (maingameloop2 == 0)
			return FindTextureAlias(texname);
		strcpy_s(junk, p + 1);

		while (texname[num] != '@')
			junk2[count++] = texname[num++];

		junk2[count] = '\0';

		tnum = atoi(junk);

		tnum++;

		sprintf_s(junk, "%s@%d", junk2, tnum);

		result = FindTextureAlias(junk);
		if (result == -1) {
			sprintf_s(junk, "%s@1", junk2);
			result = FindTextureAlias(junk);
		}

		return result;
	}

	return -1;
}

int CMyD3DApplication::MakeDice() {
	float x1, x2, y1, y2;
	float scale = 16.0f;
	float adjust = 100.0f;

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);
	viewportheight = (FLOAT)vp.dwHeight;
	viewportwidth = (FLOAT)vp.dwWidth;
	x1 = 70.0f + scale;
	x2 = 10.0f;

	y1 = 110.0f + scale;
	y2 = 50.0f;

	dice[0].dicebox[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
	dice[0].dicebox[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 0);
	dice[0].dicebox[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 1);
	dice[0].dicebox[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 0);

	dice[0].dicebox[1].sx = (viewportwidth / 2) - x1;
	dice[0].dicebox[3].sx = (viewportwidth / 2) - x2;
	dice[0].dicebox[0].sx = (viewportwidth / 2) - x1;
	dice[0].dicebox[2].sx = (viewportwidth / 2) - x2;

	dice[0].dicebox[1].sy = viewportheight - y1;
	dice[0].dicebox[3].sy = viewportheight - y1;
	dice[0].dicebox[0].sy = viewportheight - y2;
	dice[0].dicebox[2].sy = viewportheight - y2;
	dice[0].sides = 20;
	strcpy_s(dice[0].name, "die20s20");
	strcpy_s(dice[0].prefix, "die20");
	dice[0].rollnum = 8;
	dice[0].roll = 0;
	dice[0].rollmax = 5;

	dice[1].dicebox[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
	dice[1].dicebox[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 0);
	dice[1].dicebox[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 1);
	dice[1].dicebox[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 0);

	dice[1].dicebox[1].sx = (viewportwidth / 2) + x2;
	dice[1].dicebox[3].sx = (viewportwidth / 2) + x1;
	dice[1].dicebox[0].sx = (viewportwidth / 2) + x2;
	dice[1].dicebox[2].sx = (viewportwidth / 2) + x1;

	dice[1].dicebox[1].sy = viewportheight - y1;
	dice[1].dicebox[3].sy = viewportheight - y1;
	dice[1].dicebox[0].sy = viewportheight - y2;
	dice[1].dicebox[2].sy = viewportheight - y2;

	strcpy_s(dice[1].name, "die4s4");
	strcpy_s(dice[1].prefix, "die4");
	dice[1].rollnum = 4;
	dice[1].roll = 0;
	dice[1].sides = 4;
	dice[1].rollmax = 3;

	dice[2].dicebox[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
	dice[2].dicebox[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 0);
	dice[2].dicebox[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 1);
	dice[2].dicebox[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 0);

	dice[2].dicebox[1].sx = (viewportwidth / 2) + adjust + x2;
	dice[2].dicebox[3].sx = (viewportwidth / 2) + adjust + x1;
	dice[2].dicebox[0].sx = (viewportwidth / 2) + adjust + x2;
	dice[2].dicebox[2].sx = (viewportwidth / 2) + adjust + x1;

	dice[2].dicebox[1].sy = viewportheight - y1;
	dice[2].dicebox[3].sy = viewportheight - y1;
	dice[2].dicebox[0].sy = viewportheight - y2;
	dice[2].dicebox[2].sy = viewportheight - y2;

	strcpy_s(dice[2].name, "die6s6");
	strcpy_s(dice[2].prefix, "die6");
	dice[2].rollnum = 6;
	dice[2].roll = 0;
	dice[2].sides = 6;
	dice[2].rollmax = 4;

	dice[3].dicebox[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
	dice[3].dicebox[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 0);
	dice[3].dicebox[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 1);
	dice[3].dicebox[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 0);

	dice[3].dicebox[1].sx = (viewportwidth / 2) + (adjust * 2.0f) + x2;
	dice[3].dicebox[3].sx = (viewportwidth / 2) + (adjust * 2.0f) + x1;
	dice[3].dicebox[0].sx = (viewportwidth / 2) + (adjust * 2.0f) + x2;
	dice[3].dicebox[2].sx = (viewportwidth / 2) + (adjust * 2.0f) + x1;

	dice[3].dicebox[1].sy = viewportheight - y1;
	dice[3].dicebox[3].sy = viewportheight - y1;
	dice[3].dicebox[0].sy = viewportheight - y2;
	dice[3].dicebox[2].sy = viewportheight - y2;
	dice[3].sides = 20;
	strcpy_s(dice[3].name, "die20s20");
	strcpy_s(dice[3].prefix, "die20");
	dice[3].rollnum = 8;
	dice[3].roll = 0;
	dice[3].rollmax = 5;

	x1 = 70.0f + scale;
	x2 = 10.0f;
	y1 = 100.0f + scale;
	y2 = 40.0f;

	crosshair[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
	crosshair[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 0);
	crosshair[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 1);
	crosshair[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 0);

	int offset = 5;

	crosshair[0].sx = (viewportwidth / 2) - offset;
	crosshair[1].sx = (viewportwidth / 2) - offset;
	crosshair[2].sx = (viewportwidth / 2) + offset;
	crosshair[3].sx = (viewportwidth / 2) + offset;

	crosshair[0].sy = (viewportheight / 2) + offset;
	crosshair[1].sy = (viewportheight / 2) - offset;
	crosshair[2].sy = (viewportheight / 2) + offset;
	crosshair[3].sy = (viewportheight / 2) - offset;

	return 1;
}

void CMyD3DApplication::MakeDamageDice() {

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);
	viewportheight = (FLOAT)vp.dwHeight;
	viewportwidth = (FLOAT)vp.dwWidth;
	float x1, x2, y1, y2;
	float scale = 16.0f;

	x1 = 70.0f + scale;
	x2 = 10.0f;
	y1 = 110.0f + scale;
	y2 = 50.0f;

	int a;

	int gunmodel = 0;
	for (a = 0; a < num_your_guns; a++) {
		if (your_gun[a].model_id == player_list[trueplayernum].gunid) {
			gunmodel = a;
		}
	}

	if (your_gun[gunmodel].damage2 == 4) {
		strcpy_s(dice[1].name, "die4s4");
		strcpy_s(dice[1].prefix, "die4");
		dice[1].rollnum = 4;
		dice[1].roll = 0;
		dice[1].sides = 4;
		dice[1].rollmax = 3;
	} else if (your_gun[gunmodel].damage2 == 6) {
		strcpy_s(dice[1].name, "die6s6");
		strcpy_s(dice[1].prefix, "die6");
		dice[1].rollnum = 6;
		dice[1].roll = 0;
		dice[1].sides = 6;
		dice[1].rollmax = 4;
	} else if (your_gun[gunmodel].damage2 == 8) {
		strcpy_s(dice[1].name, "die8s8");
		strcpy_s(dice[1].prefix, "die8");
		dice[1].rollnum = 8;
		dice[1].roll = 0;
		dice[1].sides = 8;
		dice[1].rollmax = 5;
	} else if (your_gun[gunmodel].damage2 == 10) {

		strcpy_s(dice[1].name, "die10s10");
		strcpy_s(dice[1].prefix, "die10");
		dice[1].rollnum = 10;
		dice[1].roll = 0;
		dice[1].sides = 10;
		dice[1].rollmax = 5;
	} else if (your_gun[gunmodel].damage2 == 12) {

		strcpy_s(dice[1].name, "die12s12");
		strcpy_s(dice[1].prefix, "die12");
		dice[1].rollnum = 12;
		dice[1].roll = 0;
		dice[1].sides = 12;
		dice[1].rollmax = 4;
	}

	dice[1].dicebox[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
	dice[1].dicebox[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 0);
	dice[1].dicebox[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 1);
	dice[1].dicebox[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 0);

	dice[1].dicebox[1].sx = (viewportwidth / 2) + x2;
	dice[1].dicebox[3].sx = (viewportwidth / 2) + x1;
	dice[1].dicebox[0].sx = (viewportwidth / 2) + x2;
	dice[1].dicebox[2].sx = (viewportwidth / 2) + x1;

	dice[1].dicebox[1].sy = viewportheight - y1;
	dice[1].dicebox[3].sy = viewportheight - y1;
	dice[1].dicebox[0].sy = viewportheight - y2;
	dice[1].dicebox[2].sy = viewportheight - y2;
}

void CMyD3DApplication::SetDiceTexture() {

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);
	viewportheight = (FLOAT)vp.dwHeight;
	viewportwidth = (FLOAT)vp.dwWidth;

	for (int i = 0; i < numdice; i++) {

		if (dice[i].roll == 1) {
			// roll the die

			if (maingameloop)
				dice[i].rollnum++;
			if (dice[i].rollnum > dice[i].rollmax) {

				dice[i].rollnum = 1;
			}
			sprintf_s(dice[i].name, "%sr%d", dice[i].prefix, dice[i].rollnum);
		}
	}

	char junk[255];

	if (usespell == 1) {
		spellhiton = player_list[trueplayernum].hd;
	} else {
		spellhiton = 0;
	}

	if (hitmonster == 1 || usespell == 1) {

		if (strstr(your_gun[current_gun].gunname, "SCROLL-HEALING") != NULL) {
			strcpy_s(junk, "Attack   Heal");
		} else {
			strcpy_s(junk, "Attack   Damage");
		}
	} else {
		strcpy_s(junk, "Attack");
	}

	if (numdice == 3 && hitmonster == 1) {
		// strcpy_s(junk,"Attack  Damage");
	} else if (numdice == 3 && hitmonster == 0) {
		strcpy_s(junk, "Attack");
	}

	if (showsavingthrow > 0) {
		display_message((viewportwidth / 2) + 222.0f, viewportheight - 30.0f, "Save", vp, 255, 255, 0, 12.5, 16, 0);
		if (savefailed == 1)
			display_message((viewportwidth / 2) + 212.0f, viewportheight - 15.0f, "Failed", vp, 255, 255, 0, 12.5, 16, 0);
	}
	if (showlisten > 0) {

		display_message((viewportwidth / 2) + 111.0f, viewportheight - 30.0f, "Listen", vp, 255, 255, 0, 12.5, 16, 0);
		if (listenfailed == 1)
			display_message((viewportwidth / 2) + 111.0f, viewportheight - 15.0f, "Failed", vp, 255, 255, 0, 12.5, 16, 0);
	}

	display_message((viewportwidth / 2) - 86.0f, viewportheight - 30.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);

	if (criticalhiton == 1) {
		strcpy_s(junk, "Critical Hit  X 2");
		display_message((viewportwidth / 2) - 95.0f, viewportheight - 145.0f, junk, vp, 255, 255, 0, 12.5, 16, 0);
	}
	if (spellhiton > 1) {
		sprintf_s(junk, "           X %d", spellhiton);
		display_message((viewportwidth / 2) - 95.0f, viewportheight - 130.0f, junk, vp, 0, 255, 0, 12.5, 16, 0);
	} else {
		int gunmodel = 0;
		for (int a = 0; a < num_your_guns; a++) {

			if (your_gun[a].model_id == player_list[trueplayernum].gunid) {

				gunmodel = a;
			}
		}

		int attackbonus = your_gun[gunmodel].sattack;
		int damagebonus = your_gun[gunmodel].sdamage;

		if (attackbonus > 0 || damagebonus > 0) {
			sprintf_s(junk, "   +%d      +%d", attackbonus, damagebonus);
			display_message((viewportwidth / 2) - 95.0f, viewportheight - 130.0f, junk, vp, 0, 255, 0, 12.5, 16, 0);
		}
	}
}

int CMyD3DApplication::thaco(int ac, int thaco) {

	int result;

	result = thaco - ac;

	return result;
}

int CMyD3DApplication::XpPoints(int hd, int hp) {

	int bxp;
	int hpxp;
	int xp;

	switch (hd) {

	case 1:

		bxp = 10;
		hpxp = 1 * hp;
		xp = bxp + hpxp;
		break;
	case 2:
		bxp = 20;
		hpxp = 2 * hp;
		xp = bxp + hpxp;

		break;
	case 3:
		bxp = 35;
		hpxp = 3 * hp;
		xp = bxp + hpxp;

		break;
	case 4:
		bxp = 60;
		hpxp = 4 * hp;
		xp = bxp + hpxp;

		break;
	case 5:
		bxp = 90;
		hpxp = 5 * hp;
		xp = bxp + hpxp;

		break;
	case 6:
		bxp = 150;
		hpxp = 6 * hp;
		xp = bxp + hpxp;

		break;
	case 7:
		bxp = 225;
		hpxp = 8 * hp;
		xp = bxp + hpxp;

		break;
	case 8:
		bxp = 375;
		hpxp = 10 * hp;
		xp = bxp + hpxp;

		break;
	case 9:
		bxp = 600;
		hpxp = 12 * hp;
		xp = bxp + hpxp;

		break;
	case 10:
		bxp = 900;
		hpxp = 14 * hp;
		xp = bxp + hpxp;
	case 99:
		bxp = 200;
		hpxp = 0;
		xp = hd * bxp;
		break;
	}

	return xp;
}

int CMyD3DApplication::LevelUpXPNeeded(int xp) {

	int countlevels = 0;

	if (xp >= 0 && xp <= 2000) {
		countlevels = 2000;
	} else if (xp >= 2001 && xp <= 4000) {
		countlevels = 4000;
	} else if (xp >= 4001 && xp <= 8000) {
		countlevels = 8000;
	} else if (xp >= 8001 && xp <= 18000) {
		countlevels = 18000;
	} else if (xp >= 18001 && xp <= 35000) {
		countlevels = 35000;
	} else if (xp >= 35001 && xp <= 70000) {
		countlevels = 70000;
	} else if (xp >= 70001 && xp <= 125000) {
		countlevels = 125000;
	} else if (xp >= 125001 && xp <= 250000) {
		countlevels = 250000;
	} else if (xp >= 250001 && xp <= 500000) {
		countlevels = 500000;
	} else if (xp >= 500001 && xp <= 750000) {
		countlevels = 750000;
	} else if (xp >= 750001 && xp <= 1000000) {
		countlevels = 1000000;
	}

	else if (xp >= 1000001 && xp <= 1250000) {
		countlevels = 1250000;
	} else if (xp >= 1250001 && xp <= 1500000) {
		countlevels = 1500000;
	} else if (xp >= 1500001 && xp <= 1750000) {
		countlevels = 1750000;
	} else if (xp >= 1750001 && xp <= 2000000) {
		countlevels = 2000000;
	} else if (xp >= 2000001 && xp <= 2250000) {
		countlevels = 2250000;
	} else if (xp >= 2250001 && xp <= 2500000) {
		countlevels = 2500000;
	} else if (xp >= 2500001 && xp <= 2750000) {
		countlevels = 2750000;
	} else if (xp >= 2750001 && xp <= 3000000) {
		countlevels = 3000000;
	} else if (xp >= 3000001 && xp <= 3250000) {
		countlevels = 3250000;
	} else if (xp >= 3250001 && xp <= 3500000) {
		countlevels = 3500000;
	} else if (xp >= 3500001 && xp <= 3750000) {
		countlevels = 3750000;
	} else if (xp >= 3750001 && xp <= 4000000) {
		countlevels = 4000000;
	} else if (xp >= 4000001 && xp <= 4250000) {
		countlevels = 4250000;
	} else if (xp >= 4250001 && xp <= 4500000) {
		countlevels = 4500000;
	} else if (xp >= 4500001 && xp <= 4750000) {
		countlevels = 4750000;
	} else if (xp >= 4750001 && xp <= 5000000) {
		countlevels = 5000000;
	} else if (xp >= 5000001 && xp <= 5250000) {
		countlevels = 5250000;
	} else if (xp >= 5250001 && xp <= 5500000) {
		countlevels = 5500000;
	} else if (xp >= 5500001 && xp <= 5750000) {
		countlevels = 5750000;
	} else if (xp >= 5750001 && xp <= 6000000) {
		countlevels = 6000000;
	} else if (xp >= 6000001 && xp <= 6250000) {
		countlevels = 6250000;
	} else if (xp >= 6250001 && xp <= 6500000) {
		countlevels = 6500000;
	} else if (xp >= 6500001 && xp <= 6750000) {
		countlevels = 6750000;
	} else if (xp >= 6750001 && xp <= 7000000) {
		countlevels = 7000000;
	} else if (xp >= 7000001 && xp <= 7250000) {
		countlevels = 7250000;
	} else if (xp >= 7250001 && xp <= 7500000) {
		countlevels = 7500000;
	} else if (xp >= 7500001 && xp <= 7750000) {
		countlevels = 7750000;
	} else if (xp >= 7750001 && xp <= 8000000) {
		countlevels = 8000000;
	} else if (xp >= 8000001 && xp <= 8250000) {
		countlevels = 8250000;
	} else if (xp >= 8250001 && xp <= 8500000) {
		countlevels = 8500000;
	} else if (xp >= 8500001 && xp <= 8750000) {
		countlevels = 8750000;
	} else if (xp >= 8750001 && xp <= 9000000) {
		countlevels = 9000000;
	} else if (xp >= 9000001 && xp <= 9250000) {
		countlevels = 9250000;
	} else if (xp >= 9250001 && xp <= 9500000) {
		countlevels = 9500000;
	} else if (xp >= 9500001 && xp <= 9750000) {
		countlevels = 9750000;
	} else if (xp >= 9750001 && xp <= 10000000) {
		countlevels = 10000000;
	} else if (xp >= 10000001 && xp <= 10250000) {
		countlevels = 10250000;
	} else if (xp >= 10250001 && xp <= 10500000) {
		countlevels = 10500000;
	} else if (xp >= 10500001 && xp <= 10750000) {
		countlevels = 10750000;
	} else if (xp >= 10750001 && xp <= 11000000) {
		countlevels = 11000000;
	} else if (xp >= 11000001 && xp <= 11250000) {
		countlevels = 11250000;
	} else if (xp >= 11250001 && xp <= 11500000) {
		countlevels = 11500000;
	} else if (xp >= 11500001 && xp <= 11750000) {
		countlevels = 11750000;
	} else if (xp >= 11750001 && xp <= 12000000) {
		countlevels = 12000000;
	} else if (xp >= 12000001 && xp <= 12250000) {
		countlevels = 12250000;
	} else if (xp >= 12250001 && xp <= 12500000) {
		countlevels = 12500000;
	} else if (xp >= 12500001 && xp <= 12750000) {
		countlevels = 12750000;
	} else if (xp >= 12750001 && xp <= 13000000) {
		countlevels = 13000000;
	} else if (xp >= 13000001 && xp <= 13250000) {
		countlevels = 13250000;
	} else if (xp >= 13250001 && xp <= 13500000) {
		countlevels = 13500000;
	}

	return countlevels;
}

int CMyD3DApplication::LevelUp(int xp) {

	int countlevels = 0;
	int adjust = 800;

	if (xp > 0 && xp <= 2000) {

		countlevels = 1;
	} else if (xp >= 2001 && xp <= 4000) {
		countlevels = 2;
	} else if (xp >= 4001 && xp <= 8000) {
		countlevels = 3;
	} else if (xp >= 8001 && xp <= 18000) {
		countlevels = 4;
	} else if (xp >= 18001 && xp <= 35000) {
		countlevels = 5;
	} else if (xp >= 35001 && xp <= 70000) {
		countlevels = 6;
	} else if (xp >= 70001 && xp <= 125000) {
		countlevels = 7;
	} else if (xp >= 125001 && xp <= 250000) {
		countlevels = 8;
	} else if (xp >= 250001 && xp <= 500000) {
		countlevels = 9;
	} else if (xp >= 500001 && xp <= 750000) {
		countlevels = 10;
	} else if (xp >= 750001 && xp <= 1000000) {
		countlevels = 11;
	}

	else if (xp >= 1000001 && xp <= 1250000) {
		countlevels = 12;
	} else if (xp >= 1250001 && xp <= 1500000) {
		countlevels = 13;
	} else if (xp >= 1500001 && xp <= 1750000) {
		countlevels = 14;
	} else if (xp >= 1750001 && xp <= 2000000) {
		countlevels = 15;
	} else if (xp >= 2000001 && xp <= 2250000) {
		countlevels = 16;
	} else if (xp >= 2250001 && xp <= 2500000) {
		countlevels = 17;
	} else if (xp >= 2500001 && xp <= 2750000) {
		countlevels = 18;
	} else if (xp >= 2750001 && xp <= 3000000) {
		countlevels = 19;
	} else if (xp >= 3000001 && xp <= 3250000) {
		countlevels = 20;
	} else if (xp >= 3250001 && xp <= 3500000) {
		countlevels = 21;
	} else if (xp >= 3500001 && xp <= 3750000) {
		countlevels = 22;
	} else if (xp >= 3750001 && xp <= 4000000) {
		countlevels = 23;
	} else if (xp >= 4000001 && xp <= 4250000) {
		countlevels = 24;
	} else if (xp >= 4250001 && xp <= 4500000) {
		countlevels = 25;
	} else if (xp >= 4500001 && xp <= 4750000) {
		countlevels = 26;
	} else if (xp >= 4750001 && xp <= 5000000) {
		countlevels = 27;
	} else if (xp >= 5000001 && xp <= 5250000) {
		countlevels = 28;
	} else if (xp >= 5250001 && xp <= 5500000) {
		countlevels = 29;
	} else if (xp >= 5500001 && xp <= 5750000) {
		countlevels = 30;
	} else if (xp >= 5750001 && xp <= 6000000) {
		countlevels = 31;
	} else if (xp >= 6000001 && xp <= 6250000) {
		countlevels = 32;
	} else if (xp >= 6250001 && xp <= 6500000) {
		countlevels = 33;
	} else if (xp >= 6500001 && xp <= 6750000) {
		countlevels = 34;
	} else if (xp >= 6750001 && xp <= 7000000) {
		countlevels = 35;
	} else if (xp >= 7000001 && xp <= 7250000) {
		countlevels = 36;
	} else if (xp >= 7250001 && xp <= 7500000) {
		countlevels = 37;
	} else if (xp >= 7500001 && xp <= 7750000) {
		countlevels = 38;
	} else if (xp >= 7750001 && xp <= 8000000) {
		countlevels = 39;
	} else if (xp >= 8000001 && xp <= 8250000) {
		countlevels = 40;
	} else if (xp >= 8250001 && xp <= 8500000) {
		countlevels = 41;
	} else if (xp >= 8500001 && xp <= 8750000) {
		countlevels = 42;
	} else if (xp >= 8750001 && xp <= 9000000) {
		countlevels = 43;
	} else if (xp >= 9000001 && xp <= 9250000) {
		countlevels = 44;
	} else if (xp >= 9250001 && xp <= 9500000) {
		countlevels = 45;
	} else if (xp >= 9500001 && xp <= 9750000) {
		countlevels = 46;
	} else if (xp >= 9750001 && xp <= 10000000) {
		countlevels = 47;
	} else if (xp >= 10000001 && xp <= 10250000) {
		countlevels = 48;
	} else if (xp >= 10250001 && xp <= 10500000) {
		countlevels = 49;
	} else if (xp >= 10500001 && xp <= 10750000) {
		countlevels = 50;
	} else if (xp >= 10750001 && xp <= 11000000) {
		countlevels = 51;
	} else if (xp >= 11000001 && xp <= 11250000) {
		countlevels = 52;
	} else if (xp >= 11250001 && xp <= 11500000) {
		countlevels = 53;
	} else if (xp >= 11500001 && xp <= 11750000) {
		countlevels = 54;
	} else if (xp >= 11750001 && xp <= 12000000) {
		countlevels = 55;
	} else if (xp >= 12000001 && xp <= 12250000) {
		countlevels = 56;
	} else if (xp >= 12250001 && xp <= 12500000) {
		countlevels = 57;
	} else if (xp >= 12500001 && xp <= 12750000) {
		countlevels = 58;
	} else if (xp >= 12750001 && xp <= 13000000) {
		countlevels = 59;
	} else if (xp >= 13000001 && xp <= 13250000) {
		countlevels = 60;
	} else if (xp >= 13250001 && xp <= 13500000) {
		countlevels = 61;
	}

	if (player_list[trueplayernum].hd < countlevels) {
		player_list[trueplayernum].hd++;

		int raction = random_num(20) + 1;

		if (raction <= player_list[trueplayernum].hd)
			raction = player_list[trueplayernum].hd;

		player_list[trueplayernum].hp = player_list[trueplayernum].hp + raction;
		player_list[trueplayernum].health = player_list[trueplayernum].hp;

		player_list[trueplayernum].thaco--;

		if (player_list[trueplayernum].thaco <= 0)
			player_list[trueplayernum].thaco = 5;

		sprintf_s(gActionMessage, "You went up a level.  Hit Dice: %d", player_list[trueplayernum].hd);
		UpdateScrollList(0, 245, 255);
		StartFlare(2);

		PlayWavSound(SoundID("win"), 100);
	}

	return xp;
}

int CMyD3DApplication::GetNextFrame(int monsterId) {

	int mod_id = monster_list[monsterId].model_id;
	int curr_frame = monster_list[monsterId].current_frame;
	int sequence = monster_list[monsterId].current_sequence;
	int stop_frame = pmdata[mod_id].sequence_stop_frame[monster_list[monsterId].current_sequence];
	int startframe = pmdata[mod_id].sequence_start_frame[monster_list[monsterId].current_sequence];
	int nextFrame = 0;

	if (monster_list[monsterId].bStopAnimating)
		return -1;

	if (curr_frame >= stop_frame) {

		nextFrame = pmdata[mod_id].sequence_start_frame[sequence];

	} else {
		nextFrame = curr_frame + 1;
	}

	return nextFrame;
}

int CMyD3DApplication::GetNextFramePlayer() {

	int mod_id = player_list[trueplayernum].model_id;
	int curr_frame = player_list[trueplayernum].current_frame;
	int stop_frame = pmdata[mod_id].sequence_stop_frame[player_list[trueplayernum].current_sequence];
	int startframe = pmdata[mod_id].sequence_start_frame[player_list[trueplayernum].current_sequence];
	int nextFrame = 0;

	if (curr_frame >= stop_frame) {
		int curr_seq = player_list[trueplayernum].current_sequence;
		nextFrame = pmdata[mod_id].sequence_start_frame[curr_seq];
		player_list[trueplayernum].animationdir = 1;
	} else {
		nextFrame = curr_frame + 1;
	}
	return nextFrame;
}

int CMyD3DApplication::FindModelID(char *p) {

	int i = 0;

	for (i = 0; i < countmodellist; i++) {

		if (strcmp(model_list[i].name, p) == 0) {
			return model_list[i].model_id;
		}
	}

	for (i = 0; i < num_your_guns; i++) {

		if (strcmp(your_gun[i].gunname, p) == 0) {
			return your_gun[i].model_id;
		}
	}

	return 0;
}

int CMyD3DApplication::FindGunTexture(char *p) {

	int i = 0;

	for (i = 0; i < num_your_guns; i++) {

		if (strcmp(your_gun[i].gunname, p) == 0) {

			return your_gun[i].skin_tex_id;
		}
	}

	return 0;
}

int CMyD3DApplication::FreeSlave() {

	int j = 0;
	float qdist = 0.0f;

	for (j = 0; j < num_monsters; j++) {
		float qdist = FastDistance(
		    player_list[trueplayernum].x - monster_list[j].x,
		    player_list[trueplayernum].y - monster_list[j].y,
		    player_list[trueplayernum].z - monster_list[j].z);
		if (qdist < 100.0f) {
			if (monster_list[j].bIsPlayerValid == TRUE) {

				if (strcmp(monster_list[j].rname, "SLAVE") == 0) {
					monster_list[j].bIsPlayerValid = FALSE;
					monster_list[j].bStopAnimating = TRUE;
					monster_list[j].ability = 0;
					PlayWavSound(SoundID("goal1"), 100);
					PlayWavSound(SoundID("thankyou"), 100);
					sprintf_s(gActionMessage, "You set a slave free! 50 xp.");
					UpdateScrollList(0, 245, 255);
					player_list[trueplayernum].xp += 50;
				}
			}
		}
	}
	return 0;
}

int CMyD3DApplication::OpenDoor(int doornum, float dist) {

	int i = 0;
	int j = 0;
	float angle;
	int direction = +5;
	int flag = 1;
	int senddoorinfo = 0;
	int savedoorspot = 0;

	savedoorspot = 0;

	for (i = 0; i < doorcounter; i++) {
		if (door[i].doornum == doornum) {

			savedoorspot = i;
			if (door[i].type == 1) {

				return 2;
			}

			if (door[i].open == 0 && listendoor == 1 && dist <= 100.0f ||
			    door[i].open == 2 && listendoor == 1 && dist <= 100.0f

			) {

				if (door[i].listen == 0) {
					dice[2].roll = 1;
					numdice = 3;
					door[i].listen = 1;
					listendoor = 0;
					int action = random_num(dice[2].sides) + 1;
					dice[2].roll = 0;
					sprintf_s(dice[2].name, "%ss%d", dice[2].prefix, action);
					PlayWavSound(SoundID("diceroll"), 100);
					showlisten = 1;
					if (action > 2) {
						listenfailed = 1;
						strcpy_s(gActionMessage, "Listen at door failed.");
						UpdateScrollList(0, 245, 255);
					} else {

						listenfailed = 0;
						strcpy_s(gActionMessage, "Listening at the door reveals...");
						UpdateScrollList(0, 245, 255);
						ListenAtDoor();
					}
				}
			}

			if (door[i].open == 0 && pressopendoor == 1 && dist <= 100.0f ||
			    door[i].open == 2 && pressopendoor == 1 && dist <= 100.0f) {

				flag = 1;
				if (door[i].key != 0 && door[i].key != -1 || door[i].key == -2) {
					strcpy_s(gActionMessage, "This door is locked");
					UpdateScrollList(0, 245, 255);
					flag = 0;

					if (player_list[trueplayernum].keys > 0) {
						flag = 1;
						strcpy_s(gActionMessage, "You unlocked the door with a key.");

						if (door[i].key > 0)
							door[i].key = 0;

						UpdateScrollList(0, 245, 255);
						player_list[trueplayernum].keys--;

						if (player_list[trueplayernum].keys < 0)
							player_list[trueplayernum].keys = 0;
					}
				}
				if (flag) {

					if (door[i].open == 0)
						door[i].open = 1;
					if (door[i].open == 2)
						door[i].open = 3;
					PlayWavSound(SoundID("dooropen"), 100);
					if (door[i].key == -1 || door[i].key == -2) {
						// lift the door instead
						PlayWavSound(SoundID("stone"), 100);
						door[i].open = -99;
					}

					senddoorinfo = 1;
					for (j = 0; j < num_monsters; j++) {
						float qdist = FastDistance(
						    oblist[doornum].x - monster_list[j].x,
						    oblist[doornum].y - monster_list[j].y,
						    oblist[doornum].z - monster_list[j].z);
						if (qdist < (10 * monsterdist) / 2) {
							if (monster_list[j].bIsPlayerValid == TRUE && monster_list[j].ability == 1) {
								monster_list[j].bStopAnimating = FALSE;
								monster_list[j].ability = 0;
								if (strcmp(monster_list[j].rname, "SLAVE") == 0) {
									sprintf_s(gActionMessage, "You found a slave. 50 xp.");
									UpdateScrollList(0, 245, 255);
									player_list[trueplayernum].xp += 50;
								}
							}
						}
					}
				}
			}

			float openspeed = 5.0f;

			if (door[i].open == -99) {
				float up;
				up = 10.0f;
				if (maingameloop) {

					oblist[doornum].y = oblist[doornum].y + up;
					door[i].up += up;
				}
				if (door[i].up > 160.0f) {

					senddoorinfo = 0;
					door[i].open = -100;
					door[i].y = oblist[doornum].y;
				}
			} else {

				if (door[i].saveangle == 0 || door[i].saveangle == 270) {

					angle = door[i].angle;

					if (door[i].open == 3) {

						if (angle > door[i].saveangle) {
							angle = angle - 105.0f * elapsegametimersave;

							if (angle < 0)
								angle = 0;

							oblist[doornum].rot_angle = (float)angle;
							door[i].angle = angle;

						} else {
							door[i].open = 0;
						}
					} else if (door[i].open == 1) {
						if (angle < door[i].saveangle + 89.0f) {
							angle = angle + 105.0f * elapsegametimersave;

							if (angle > 359.0f)
								angle = 359;

							oblist[doornum].rot_angle = (float)angle;
							door[i].angle = angle;
							door[i].open = 1;
						} else {
							door[i].open = 2;
						}
					}
				} else {

					angle = door[i].angle;

					if (door[i].open == 3) {

						if (angle < door[i].saveangle) {
							angle = angle + 105.0f * elapsegametimersave;

							if (angle > 359.0f)
								angle = 359;

							oblist[doornum].rot_angle = (float)angle;
							door[i].angle = angle;
						} else {
							door[i].open = 0;
						}
					} else if (door[i].open == 1) {
						if (angle > door[i].saveangle - 89.0f) {
							angle = angle - 105.0f * elapsegametimersave;

							if (angle < 0)
								angle = 0;

							oblist[doornum].rot_angle = (float)angle;
							door[i].angle = angle;
							door[i].open = 1;
						} else {
							door[i].open = 2;
						}
					}
				}
			}
		}
	}

	return 1;
}

int CMyD3DApplication::ResetPlayer() {

	DSLevel();

	merchantcurrent = 0;
	merchantfound = 0;

	m_vEyePt.x = 780;
	m_vEyePt.y = 160;
	m_vEyePt.z = 780;

	m_vLookatPt.x = 780;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 780;

	angy = 0;

	gravitytime = 0.0f;

	cameraf.x = m_vEyePt.x;
	cameraf.y = m_vEyePt.y;
	cameraf.z = m_vEyePt.z;
	perspectiveview = 1;
	gravityvector.y = 0.0f;
	if (perspectiveview == 0)
		look_up_ang = 35.0f;
	else
		look_up_ang = 0.0f;

	return 0;

	player_list[trueplayernum].x = 0;
	player_list[trueplayernum].y = 160;
	player_list[trueplayernum].z = 0;
	player_list[trueplayernum].rot_angle = 0;
	player_list[trueplayernum].model_id = 0;
	player_list[trueplayernum].skin_tex_id = 0;
	player_list[trueplayernum].bIsFiring = FALSE;
	player_list[trueplayernum].ping = 0;
	player_list[trueplayernum].health = 20;
	player_list[trueplayernum].bIsPlayerAlive = TRUE;
	player_list[trueplayernum].bStopAnimating = FALSE;
	player_list[trueplayernum].current_weapon = 0;
	player_list[trueplayernum].current_car = 0;
	player_list[trueplayernum].current_frame = 0;
	player_list[trueplayernum].current_sequence = 0;
	player_list[trueplayernum].bIsPlayerInWalkMode = TRUE;
	player_list[trueplayernum].RRnetID = 0;
	player_list[trueplayernum].bIsPlayerValid = FALSE;
	player_list[trueplayernum].animationdir = 0;

	player_list[trueplayernum].volume = 0;
	player_list[trueplayernum].gold = 0;
	player_list[trueplayernum].sattack = 0;
	player_list[trueplayernum].sdie = 0;
	player_list[trueplayernum].sweapon = 0;
	player_list[trueplayernum].syell = 0;

	strcpy_s(player_list[trueplayernum].name, "");
	strcpy_s(player_list[trueplayernum].chatstr, "5");
	strcpy_s(player_list[trueplayernum].name, "Dungeon Stomp");

	player_list[trueplayernum].ac = 7;
	player_list[trueplayernum].hd = 1;
	player_list[trueplayernum].hp = 20;
	player_list[trueplayernum].damage1 = 1;
	player_list[trueplayernum].damage2 = 4;
	player_list[trueplayernum].thaco = 19;
	player_list[trueplayernum].xp = 0;

	m_vEyePt.x = 0;
	m_vEyePt.y = 160;
	m_vEyePt.z = 0;

	m_vLookatPt.x = 0;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 0;

	m_vEyePt.x = 780;
	m_vEyePt.y = 160;
	m_vEyePt.z = 780;

	m_vLookatPt.x = 780;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 780;

	angy = 0;
	return 0;
}

int CMyD3DApplication::ResetSound() {

	DMusic_Stop(currentmidi);
	DSound_Stop_All_Sounds();
	DSound_Delete_All_Sounds();
	DMusic_Delete_All_MIDI();
	DSound_Shutdown();
	DMusic_Shutdown();
	LoadSound();

	return 1;
}

int CMyD3DApplication::UpdateScrollList(int r, int g, int b) {
	strcpy_s(scrolllist1[slistcounter].text, gActionMessage);
	scrolllist1[slistcounter].num = slistcounter;
	scrolllist1[slistcounter].r = r;
	scrolllist1[slistcounter].g = g;
	scrolllist1[slistcounter].b = b;
	sliststart = slistcounter;

	slistcounter++;

	if (slistcounter >= scrolllistnum) {
		slistcounter = 0;
	}

	return 1;
}

int CMyD3DApplication::DisplayDialogText(char *text, float yloc) {

	D3DVIEWPORT7 vp;
	m_pd3dDevice->GetViewport(&vp);

	int i;
	char junk[4048];
	char buf[4048];
	int bufcount = 0;
	float adjust = 0.0f;
	float y = 0.0f;
	dialogtextcount = 0;
	float counter = 0;
	float ystart = 0;
	float xstart = 0;

	ystart = (FLOAT)(viewportheight / 2 - 90.0f) + yloc;

	for (i = 0; i <= (int)strlen(text); i++) {

		counter++;
		if (text[i] == '<' || i >= (int)strlen(text)) {
			buf[bufcount] = '\0';
			bufcount = 0;

			if (counter > xstart) {
				xstart = counter;
			}
			counter = 0;

			strcpy_s(dialogtext[dialogtextcount].text, buf);
			dialogtextcount++;
		} else {
			buf[bufcount] = text[i];

			bufcount++;
		}
	}

	ystart = (FLOAT)(viewportheight / 2.0f) - (dialogtextcount * 13.0f) / 2.0f;

	xstart = (FLOAT)(viewportwidth / 2.0f) - (xstart * 11.2f) / 2.0f;

	ystart += yloc;
	ystart += 13.0f;

	for (i = 0; i < dialogtextcount; i++) {
		sprintf_s(junk, "%s", dialogtext[i].text);
		y = ystart + adjust;
		display_message(xstart + 10.0f, y, junk, vp, 0, 255, 255, 12.5, 16, 0);
		adjust += 13.0f;
	}

	return 1;
}

int CMyD3DApplication::ListenAtDoor() {

	int i = 0;
	int cullflag = 0;
	int cullloop = 0;
	int montry = 0;
	float qdist = 0;
	float wx, wy, wz;

	int q;
	int countsounds = 0;
	for (q = 0; q < pCMyApp->oblist_length; q++) {

		wx = oblist[q].x;
		wy = oblist[q].y;
		wz = oblist[q].z;

		if (strcmp(oblist[q].name, "!monster1") == 0 && oblist[q].monstertexture > 0 && monsterenable == 1) {

			cullflag = 0;

			for (montry = 0; montry < num_monsters; montry++) {
				if (oblist[q].monsterid == monster_list[montry].monsterid) {

					if (monster_list[montry].bIsPlayerAlive == TRUE && monster_list[montry].bStopAnimating == TRUE) {
						qdist = FastDistance(
						    m_vEyePt.x - monster_list[montry].x,
						    m_vEyePt.y - monster_list[montry].y,
						    m_vEyePt.z - monster_list[montry].z);

						if (qdist < 500.0f) {

							sprintf_s(gActionMessage, "You hear a %s. 20 xp.", monster_list[montry].rname);
							UpdateScrollList(0, 245, 255);

							player_list[trueplayernum].xp += 20;

							PlayWavSound(monster_list[i].syell, monster_list[montry].volume);
							countsounds++;
						}
					}
				}
			}
		}
	}
	if (countsounds == 0) {

		int raction = random_num(5);

		switch (raction) {

		case 0:

			sprintf_s(gActionMessage, "You hear a distant moan...");
			break;
		case 1:

			sprintf_s(gActionMessage, "You hear the sound of water...");
			break;
		case 2:

			sprintf_s(gActionMessage, "You hear the clank of chains...");
			break;
		default:
			sprintf_s(gActionMessage, "You hear scratching noises...");
			break;
		}
		UpdateScrollList(0, 245, 255);
	}

	return 1;
}

void CMyD3DApplication::ApplyPlayerDamage(int playerid, int damage) {

	int raction;

	if (player_list[trueplayernum].bIsPlayerAlive == FALSE || player_list[trueplayernum].health <= 0)
		return;
	PlayWavSound(SoundID("pimpact"), 100);
	player_list[trueplayernum].health = player_list[trueplayernum].health - damage;
	PlayWavSound(SoundID("pain1"), 100);

	raction = (int)random_num(3);

	switch (raction) {

	case 0:
		SetPlayerAnimationSequence(trueplayernum, 3); // pain1
		break;
	case 1:
		SetPlayerAnimationSequence(trueplayernum, 4); // pain2
		break;
	case 2:
		SetPlayerAnimationSequence(trueplayernum, 5); // pain3
		break;
	}

	if (player_list[trueplayernum].health <= 0) {
		player_list[trueplayernum].health = 0;

		raction = (int)random_num(3);

		switch (raction) {
		case 0:
			SetPlayerAnimationSequence(trueplayernum, 12); // death1
			break;
		case 1:
			SetPlayerAnimationSequence(trueplayernum, 13); // death2
			break;
		case 2:
			SetPlayerAnimationSequence(trueplayernum, 14); // death3
			break;
		}

		player_list[trueplayernum].bIsPlayerAlive = FALSE;

		if (perspectiveview == 1)
			m_vLookatPt = m_vEyePt;
		// m_vLookatPt=m_vEyePt;

		look_up_ang = 30;
		perspectiveview = 0;
		sprintf_s(gActionMessage, "Great Crom. You are dead!");
		UpdateScrollList(0, 245, 255);
		sprintf_s(gActionMessage, "Press SPACE to rise again...");
		UpdateScrollList(0, 245, 255);
	}
}

void CMyD3DApplication::SetStartSpot() {

	int result;

	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, NULL);

	/*
	if (strcmp(pCMyApp->ds_reg->registered,"0") ==0 && pCMyApp->current_level>=3 && pCMyApp->multiplay_flag==FALSE) {
	    DSPostQuit();
	    Pause(TRUE);
	    UINT resultclick = MessageBox(main_window_handle,"Please register at http://www.murk.on.ca","Register Game",MB_OK);
	    Pause(FALSE);
	    PostQuitMessage(0);
	}
	*/

	if (startposcounter == 0)
		return;

	result = random_num(startposcounter);

	m_vEyePt.x = startpos[result].x;
	m_vEyePt.y = startpos[result].y;
	m_vEyePt.z = startpos[result].z;

	m_vLookatPt = m_vEyePt;
	cameraf.x = m_vLookatPt.x;
	cameraf.y = m_vLookatPt.y;
	cameraf.z = m_vLookatPt.z;
	angy = startpos[result].angle;

	modellocation = m_vEyePt;
	UpdateMainPlayer();
}

void CMyD3DApplication::MonsterControl() {

	int i = 0;
	int j = 0;
	float closest = 99999;
	float qdist = 0;
	for (i = 0; i < num_monsters; i++) {
		closest = 99999;
		for (j = 0; j < num_players; j++) {

			// find closest player
			if (player_list[j].bIsPlayerAlive == TRUE) {
				qdist = FastDistance(
				    monster_list[i].x - player_list[j].x,
				    monster_list[i].y - player_list[j].y,
				    monster_list[i].z - player_list[j].z);

				if (qdist < closest) {
					closest = qdist;
					monster_list[i].closest = j;
				}
			}
		}
	}
}

void CMyD3DApplication::StartFlare(int type) {

	D3DCOLOR raction;
	raction = RGBA_MAKE(0, 0, 0, 0);

	if (type == 1) {
		// red
		flarestart = 1;
		flarecounter = 200;
		flarenum = 160;
		flaretype = 1;
	}

	if (type == 2) {
		// white
		flarestart = 1;
		flarecounter = 200;
		flarenum = 160;
		flaretype = 2;
	}
	if (type == 3) {
		// white
		flarestart = 1;
		flarecounter = 200;
		flarenum = 160;
		flaretype = 3;
	}

	if (type == 4) {
		// black
		flarestart = 1;
		flarecounter = 255;
		flarenum = 160;
		flaretype = 4;
		flarefadeout = 1.0f;
	}

	flare[0].color = raction;
	flare[1].color = raction;
	flare[2].color = raction;
	flare[3].color = raction;
}

void CMyD3DApplication::CycleFlare() {

	D3DCOLOR raction;

	if (flaretype == 1)
		raction = RGBA_MAKE(flarecounter, 0, 0, 0);
	if (flaretype == 2)
		raction = RGBA_MAKE(flarecounter, flarecounter, flarecounter, 0);
	if (flaretype == 3)
		raction = RGBA_MAKE(0, 0, flarecounter, 0);
	if (flaretype == 4)
		raction = RGBA_MAKE(0, 0, 0, flarecounter);

	flare[0].color = raction;
	flare[1].color = raction;
	flare[2].color = raction;
	flare[3].color = raction;

	if (flaretype == 4) {
		flarecounter -= 5;

		if (flarefadeout <= 0) {
			flarecounter = 0;
			flarestart = 0;
		}
	} else {
		flarecounter -= 20;
	}
	if (flarecounter <= 0) {
		flarecounter = 0;
		flarestart = 0;
	}
}

void CMyD3DApplication::ResetMainPlayer() {

	player_list[trueplayernum].frags = 0;
	player_list[trueplayernum].x = 0;
	player_list[trueplayernum].y = 160;
	player_list[trueplayernum].z = 0;
	player_list[trueplayernum].rot_angle = 0;
	player_list[trueplayernum].skin_tex_id = 0;
	player_list[trueplayernum].health = 20;
	player_list[trueplayernum].bIsPlayerAlive = TRUE;
	player_list[trueplayernum].bStopAnimating = FALSE;
	player_list[trueplayernum].current_weapon = 0;
	player_list[trueplayernum].current_car = 0;
	player_list[trueplayernum].current_frame = 0;
	player_list[trueplayernum].current_sequence = 0;
	player_list[trueplayernum].bIsPlayerInWalkMode = TRUE;
	player_list[trueplayernum].animationdir = 0;
	player_list[trueplayernum].volume = 0;
	player_list[trueplayernum].gold = 0;
	player_list[trueplayernum].sattack = 0;
	player_list[trueplayernum].sdie = 0;
	player_list[trueplayernum].sweapon = 0;
	player_list[trueplayernum].syell = 0;
	player_list[trueplayernum].ac = 7;
	player_list[trueplayernum].hd = 1;
	player_list[trueplayernum].hp = 20;
	player_list[trueplayernum].thaco = 19;
	player_list[trueplayernum].xp = 0;
	player_list[trueplayernum].keys = 0;

	bIsFlashlightOn = FALSE;

	m_vEyePt.x = 0;
	m_vEyePt.y = 160;
	m_vEyePt.z = 0;

	m_vLookatPt.x = 0;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 0;

	m_vEyePt.x = 780;
	m_vEyePt.y = 160;
	m_vEyePt.z = 780;

	m_vLookatPt.x = 780;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 780;

	angy = 0;

	sliststart = 0;
	slistcounter = 0;
	strcpy_s(gActionMessage, "Dungeon Stomp Version 1.80");
	UpdateScrollList(0, 245, 255);
	strcpy_s(gActionMessage, "Press L to listen at doors. Press SPACE to open things.");
	UpdateScrollList(0, 245, 255);

	strcpy_s(gActionMessage, "Press V to change views.  Press E to jump.");
	UpdateScrollList(0, 245, 255);

	strcpy_s(gActionMessage, "WASD to move. Press Q and Z to cycle weapons.");
	UpdateScrollList(0, 245, 255);

	strcpy_s(gActionMessage, "F2=Load F3=Save. Good luck!");
	UpdateScrollList(0, 245, 255);
}

void CMyD3DApplication::ResetDeadPlayer() {

	player_list[trueplayernum].x = 0;
	player_list[trueplayernum].y = 160;
	player_list[trueplayernum].z = 0;
	player_list[trueplayernum].rot_angle = 0;
	player_list[trueplayernum].skin_tex_id = 0;
	player_list[trueplayernum].bIsPlayerAlive = TRUE;
	player_list[trueplayernum].bStopAnimating = FALSE;
	player_list[trueplayernum].current_weapon = 0;
	player_list[trueplayernum].current_car = 0;
	player_list[trueplayernum].current_frame = 0;
	player_list[trueplayernum].current_sequence = 0;
	player_list[trueplayernum].bIsPlayerInWalkMode = TRUE;
	player_list[trueplayernum].animationdir = 0;
	player_list[trueplayernum].volume = 0;
	player_list[trueplayernum].sattack = 0;
	player_list[trueplayernum].sdie = 0;
	player_list[trueplayernum].sweapon = 0;
	player_list[trueplayernum].syell = 0;
	player_list[trueplayernum].hp = player_list[trueplayernum].hp - (4 * player_list[trueplayernum].hd);

	if (player_list[trueplayernum].hp < 20) {
		player_list[trueplayernum].hp = 20;
	}

	player_list[trueplayernum].health = player_list[trueplayernum].hp;

	m_vEyePt.x = 0;
	m_vEyePt.y = 160;
	m_vEyePt.z = 0;

	m_vLookatPt.x = 0;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 0;

	m_vEyePt.x = 780;
	m_vEyePt.y = 160;
	m_vEyePt.z = 780;

	m_vLookatPt.x = 780;
	m_vLookatPt.y = 160;
	m_vLookatPt.z = 780;
	angy = 0;
}

void CMyD3DApplication::StartMPGame() {
}

HRESULT CMyD3DApplication::NewGame() {

	if (ambientlighton == 0)
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, NULL);
	current_gun = 0;
	gravitytime = 0.0f;

	if (openingscreen == 2) {
		angy = saveangy;
		look_up_ang = 35.0f;
		perspectiveview = 0;
		m_vLookatPt = saveplocation;
		m_vLookatPt.y += 100.0f;
		m_vEyePt = m_vLookatPt;
		modellocation = m_vLookatPt;
		UpdateMainPlayer();
	} else {

		initDSTimer();
		look_up_ang = 0.0f;
		rotate_camera = 0;
		m_vEyePt = m_vLookatPt;
		perspectiveview = 1;
		angy = 0.0f;
		ResetMainPlayer();

		pCMyApp->current_level = 1;

		load_level("");

		SetStartSpot();
	}
	PlayWavSound(SoundID("goal2"), 100);
	StartFlare(2);
	openingscreen = 0;
	usespell = 0;
	spellhiton = 0;

	return 1;
}

HRESULT CMyD3DApplication::LoadGame() {

	Pause(TRUE);
	if (load_game("")) {
		strcpy_s(gActionMessage, "Loading game...");
		UpdateScrollList(0, 245, 255);
		openingscreen = 0;
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, NULL);
	} else {
		strcpy_s(gActionMessage, "Load game aborted...");
		UpdateScrollList(0, 245, 255);
	}
	Pause(FALSE);
	return 1;
}

HRESULT CMyD3DApplication::ChangeVideo() {

	HRESULT hr;
	Pause(TRUE);

	if (SUCCEEDED(D3DEnum_UserChangeDevice(&m_pDeviceInfo))) {
		if (FAILED(hr = Change3DEnvironment()))
			return 0;
	}
	Pause(FALSE);
	//      }
	return 0;
}

HRESULT CMyD3DApplication::PlayOnline() {

	Pause(TRUE);
	DialogBox(NULL, (LPCTSTR)IDD_PLAYONLINE, main_window_handle,
	          (DLGPROC)DlgPlayOnline);
	Pause(FALSE);
	return 1;
}

HRESULT CMyD3DApplication::Register() {

	Pause(TRUE);
	DialogBox(NULL, (LPCTSTR)IDD_REG, main_window_handle,
	          (DLGPROC)DlgRegister);
	Pause(FALSE);
	return 1;
}

HRESULT CMyD3DApplication::ExitGame() {

	PrintMessage(NULL, "MsgProc - IDM_EXIT", NULL, LOGFILE_ONLY);

	Cleanup3DEnvironment();
	SendMessage(main_window_handle, WM_CLOSE, 0, 0);
	DestroyWindow(main_window_handle);
	PostQuitMessage(0);
	exit(0);

	return 1;
}

HRESULT CMyD3DApplication::GameControls() {

	Pause(TRUE);

	DialogBox((HINSTANCE)GetWindowLong(main_window_handle, GWL_HINSTANCE),
	          MAKEINTRESOURCE(IDD_SELECTINPUTDEVICE), main_window_handle,
	          (DLGPROC)InputDeviceSelectProc);

	Pause(FALSE);

	return 1;
}

HRESULT CMyD3DApplication::SelectInputDevice() {

	// Destroy the old device
	pCMyApp->DestroyInputDevice();

	// Check the dialog controls to see which device type to create

	if (g_bUseKeyboard) {
		SetCursor(NULL);
		pCMyApp->CreateInputDevice(main_window_handle, g_Keyboard_pDI,
		                           g_Keyboard_pdidDevice2,
		                           GUID_SysKeyboard, &c_dfDIKeyboard,
		                           DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		g_Keyboard_pdidDevice2->Acquire();
	}

	if (g_bUseMouse) {
		pCMyApp->CreateInputDevice(main_window_handle, g_pDI,
		                           g_pdidDevice2,
		                           GUID_SysMouse, &c_dfDIMouse,
		                           DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		g_pdidDevice2->Acquire();

		pCMyApp->CreateInputDevice(main_window_handle, g_Keyboard_pDI,
		                           g_Keyboard_pdidDevice2,
		                           GUID_SysKeyboard, &c_dfDIKeyboard,
		                           DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		g_Keyboard_pdidDevice2->Acquire();
	}

	if (g_bUseJoystick) {

		pCMyApp->CreateInputDevice(main_window_handle, g_Keyboard_pDI,
		                           g_Keyboard_pdidDevice2,
		                           GUID_SysKeyboard, &c_dfDIKeyboard,
		                           DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		g_Keyboard_pdidDevice2->Acquire();

		pCMyApp->CreateInputDevice(main_window_handle, g_pDI,
		                           g_pdidDevice2,
		                           g_guidJoystick, &c_dfDIJoystick,
		                           DISCL_EXCLUSIVE | DISCL_FOREGROUND);

		if (g_pdidDevice2 == NULL) {
			UINT resultclick = MessageBox(main_window_handle, "DirectInput Error", "Game Controller not found.", MB_OK);
		}

		g_pdidDevice2->Acquire();

		// Set the range of the joystick axes tp [-1000,+1000]
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = -10;
		diprg.lMax = +10;

		diprg.diph.dwObj = DIJOFS_X; // Set the x-axis range
		g_pdidDevice2->SetProperty(DIPROP_RANGE, &diprg.diph);

		diprg.diph.dwObj = DIJOFS_Y; // Set the y-axis range
		g_pdidDevice2->SetProperty(DIPROP_RANGE, &diprg.diph);

		// Set the dead zone for the joystick axes (because many joysticks
		// aren't perfectly calibrated to be zero when centered).
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 1000; // Here, 1000 = 10%

		dipdw.diph.dwObj = DIJOFS_X; // Set the x-axis deadzone
		g_pdidDevice2->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		dipdw.diph.dwObj = DIJOFS_Y; // Set the y-axis deadzone
		g_pdidDevice2->SetProperty(DIPROP_RANGE, &dipdw.diph);
	}
	return 1;
}

int CMyD3DApplication::SavingThrow(int damage, int player, int level, int missleid, int isplayer, int id) {

	// saving throw = 10 + level of speel + ADJUSTMENT (bonus)
	numdice = 4;
	int action = random_num(dice[3].sides) + 1;

	int save = 0;
	int hd = 1;

	int bonus = 0;

	if (isplayer == 1) {
		bonus = player_list[id].hd / 2;
	} else {
		bonus = monster_list[id].hd / 2;
	}

	if (bonus <= 0)
		bonus = 0;

	if (player == 1) {
		hd = player_list[your_missle[missleid].owner].hd;
	} else {
		hd = monster_list[your_missle[missleid].owner].hd;
	}

	hd = hd / 2;
	if (hd <= 0)
		hd = 1;

	save = 10 + hd;

	if (save >= 20)
		save = 20;

	if (action + bonus >= save) {
		if (isplayer) {
			PlayWavSound(SoundID("savethrow"), 100);
		}

		savefailed = 0;

		if (action == 20 && isplayer) {
			PlayWavSound(SoundID("save20"), 100);
			damage = 0;
		} else {
			damage = damage / 2;
			if (damage <= 0)
				damage = 1;
		}
	} else {
		savefailed = 1;
	}

	if (isplayer == 1) {
		showsavingthrow = 1;
		dice[3].roll = 0;
		sprintf_s(dice[3].name, "%ss%d", dice[3].prefix, action);
	}
	return damage;
}
