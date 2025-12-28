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
#include <time.h>
#include <wininet.h>
#include <dsound.h> // include dsound, dmusic
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include "t3dlib3.h"
#include "Common.h"
#include "Coll.h"
#include <windows.h>
#include <wingdi.h>
#include <string.h>

#define CLAMP(min, x, max) ((x) < (min) ? (min) : (x) > (max) ? (max) \
	                                                          : (x)) // johnfitz

#define BUFFSIZE 500
void loadcollisionmap(D3DVECTOR eyeball, D3DVECTOR v, D3DVECTOR spheresize);

int oldcount = 0;
int cyclecolor = 0;
extern int merchantlistcount;
extern int merchantcurrent;

extern D3DVECTOR saveplocation;
extern D3DVECTOR modellocation;

extern LONGLONG elapsegametimer;
extern LONGLONG elapsegametimerlast;
extern FLOAT elapsegametimersave;

extern LONGLONG gametimerlast;
extern LONGLONG gametimerlastpost;
extern LONGLONG gametimerlast2;
extern LONGLONG gametimerlastdoor;

DDGAMMARAMP DDGammaRampsave;
DDGAMMARAMP DDGammaOldsave;

extern float fTimeKeysave;

extern BOOL g_bUseKeyboard;
extern BOOL g_bUseMouse;
extern BOOL g_bUseJoystick;
extern int musicon;

extern BOOL bIsFlashlightOn;

extern int lighttype;
extern float closesoundid[100];
extern float FastDistance(float fx, float fy, float fz);
extern CMyD3DApplication *pCMyApp;
extern D3DTLVERTEX m_DisplayMessage[10000];
extern int m_DisplayMessageFont[10000];
extern int g_ob_vert_count;
extern int countmessage;
extern int countboundingbox;
extern D3DVECTOR gvelocity;
D3DVECTOR finalv;
vector3 vVector;
int start = 0;
vector<primitive<>> polygonList;
extern D3DVERTEX boundingbox[2000];

float collmax = 160.0f;

extern CLoadWorld *pCWorld;
extern int foundcollisiontrue;

extern HWND main_window_handle;

extern int perspectiveview;
extern int doorcounter;
extern int mdmg;

FLOAT LevelModTime = 0;
FLOAT LevelModLastTime = 0;
int countmodtime = 0;

struct gametext {

	int textnum;
	int type;
	char text[2048];
	int shown;
};

extern struct gametext gtext[200];

extern int textcounter;
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
};
extern struct doors door[200];
BOOL CALLBACK DlgRegister(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

extern int numberofsquares;
extern float monsterdist;

float startregen = 0;
float startregenlast = 0;

void loadcollisionmap(D3DVECTOR eyeball, D3DVECTOR v, D3DVECTOR spheresize) {

	float mxc[24];
	float myc[24];
	float mzc[24];
	float qdist;

	int i;

	int count = 0;
	int vertnum = 0;
	int vertcount = 0;
	float objectnear = 0;

	if (pCMyApp->outside == 1) {
		collmax = 260.0f;
		objectnear = 400.0f;
	} else {
		collmax = 180.0f;
		objectnear = 200.0f;
	}

	polygonList.clear();

	vertnum = pCMyApp->verts_per_poly[vertcount];

	for (i = 0; i < g_ob_vert_count; i++) {

		if (count == 0 && pCMyApp->src_collide[i] == 1) {

			primitive<> curPrim;
			mxc[0] = pCMyApp->src_v[i].x;
			myc[0] = pCMyApp->src_v[i].y;
			mzc[0] = pCMyApp->src_v[i].z;

			mxc[1] = pCMyApp->src_v[i + 1].x;
			myc[1] = pCMyApp->src_v[i + 1].y;
			mzc[1] = pCMyApp->src_v[i + 1].z;

			mxc[2] = pCMyApp->src_v[i + 2].x;
			myc[2] = pCMyApp->src_v[i + 2].y;
			mzc[2] = pCMyApp->src_v[i + 2].z;

			vert<> v0;
			v0.world().x() = mxc[0];
			v0.world().y() = myc[0];
			v0.world().z() = mzc[0];

			vert<> v1;
			v1.world().x() = mxc[1];
			v1.world().y() = myc[1];
			v1.world().z() = mzc[1];

			vert<> v2;
			v2.world().x() = mxc[2];
			v2.world().y() = myc[2];
			v2.world().z() = mzc[2];

			curPrim += v0;
			curPrim += v1;
			curPrim += v2;

			curPrim.calcPlane(false);
			curPrim.setWorldTexture(0.4f, 0.4f);

			qdist = FastDistance(eyeball.x - mxc[0],
			                     eyeball.y - myc[0],
			                     eyeball.z - mzc[0]);

			if (qdist < collmax)
				polygonList.push_back(curPrim);

			if (vertnum == 4) {

				primitive<> curPrim2;

				mxc[0] = pCMyApp->src_v[i + 1].x;
				myc[0] = pCMyApp->src_v[i + 1].y;
				mzc[0] = pCMyApp->src_v[i + 1].z;

				mxc[1] = pCMyApp->src_v[i + 3].x;
				myc[1] = pCMyApp->src_v[i + 3].y;
				mzc[1] = pCMyApp->src_v[i + 3].z;

				mxc[2] = pCMyApp->src_v[i + 2].x;
				myc[2] = pCMyApp->src_v[i + 2].y;
				mzc[2] = pCMyApp->src_v[i + 2].z;
				vert<> va0;
				va0.world().x() = mxc[0];
				va0.world().y() = myc[0];
				va0.world().z() = mzc[0];
				vert<> va1;
				va1.world().x() = mxc[1];
				va1.world().y() = myc[1];
				va1.world().z() = mzc[1];
				//
				vert<> va2;
				va2.world().x() = mxc[2];
				va2.world().y() = myc[2];
				va2.world().z() = mzc[2];

				curPrim2 += va0;
				curPrim2 += va1;
				curPrim2 += va2;

				curPrim2.calcPlane(false);

				qdist = FastDistance(eyeball.x - mxc[0],
				                     eyeball.y - myc[0],
				                     eyeball.z - mzc[0]);

				if (qdist < collmax)
					polygonList.push_back(curPrim2);
			}
		}

		count++;
		if (count > vertnum - 1) {
			count = 0;
			vertcount++;
			vertnum = pCMyApp->verts_per_poly[vertcount];
		}
	}

	count = 0;
	vertcount = 0;
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
			primitive<> curPrim3;
			vert<> va0;
			va0.world().x() = mxc[0];
			va0.world().y() = myc[0];
			va0.world().z() = mzc[0];
			vert<> va1;
			va1.world().x() = mxc[1];
			va1.world().y() = myc[1];
			va1.world().z() = mzc[1];

			vert<> va2;
			va2.world().x() = mxc[2];
			va2.world().y() = myc[2];
			va2.world().z() = mzc[2];

			curPrim3 += va0;
			curPrim3 += va1;
			curPrim3 += va2;

			curPrim3.calcPlane(false);
			curPrim3.setWorldTexture(0.4f, 0.4f);

			qdist = FastDistance(eyeball.x - mxc[0],
			                     eyeball.y - myc[0],
			                     eyeball.z - mzc[0]);

			if (qdist < collmax + 200.0f)

				polygonList.push_back(curPrim3);

			if (vertnum == 4) {

				// 132
				primitive<> curPrim4;
				mxc[0] = boundingbox[i + 1].x;
				myc[0] = boundingbox[i + 1].y;
				mzc[0] = boundingbox[i + 1].z;

				mxc[1] = boundingbox[i + 3].x;
				myc[1] = boundingbox[i + 3].y;
				mzc[1] = boundingbox[i + 3].z;

				mxc[2] = boundingbox[i + 2].x;
				myc[2] = boundingbox[i + 2].y;
				mzc[2] = boundingbox[i + 2].z;

				vert<> va0;
				va0.world().x() = mxc[0];
				va0.world().y() = myc[0];
				va0.world().z() = mzc[0];
				vert<> va1;
				va1.world().x() = mxc[1];
				va1.world().y() = myc[1];
				va1.world().z() = mzc[1];

				vert<> va2;
				va2.world().x() = mxc[2];
				va2.world().y() = myc[2];
				va2.world().z() = mzc[2];

				curPrim4 += va0;
				curPrim4 += va1;
				curPrim4 += va2;

				curPrim4.calcPlane(false);

				qdist = FastDistance(eyeball.x - mxc[0],
				                     eyeball.y - myc[0],
				                     eyeball.z - mzc[0]);

				if (qdist < collmax + 200.0f)
					polygonList.push_back(curPrim4);
			}
		}
		count++;
		if (count > vertnum - 1) {
			count = 0;
			vertcount++;
			vertnum = 4;
		}
	}

	vector3 lastDir;

	point3 xx(eyeball.x, eyeball.y, eyeball.z);

	ray3 test(xx, vector3(v.x, v.y, v.z));

	point3 cameraPosition;

	foundcollisiontrue = 0;
	vector3 ssize(spheresize.x, spheresize.y, spheresize.z);
	cameraPosition = collision::collideAndSlide(ssize, polygonList, test, lastDir, 1);

	finalv.x = cameraPosition._data[0];
	finalv.y = cameraPosition._data[1];
	finalv.z = cameraPosition._data[2];

	vVector = lastDir;
}

void CMyD3DApplication::display_message(float x, float y, char text[2048], D3DVIEWPORT7 vp, int r, int g, int b, float fontx, float fonty, int fonttype) {

	float fontsizex = 14;
	float fontsizey = 16;

	float adjust = 0;
	float tuad = .0625f;
	float tvad = .0625f;

	int abort = 0;

	x = pCMyApp->RoundFloat(x);
	y = pCMyApp->RoundFloat(y);

	int intTextLength = 0;
	intTextLength = strlen(text);
	int i = 0;
	int truecounter = 0;

	float tu = 1.0f, tv = 1.0f;

	fontsizex = fontx;
	fontsizey = fonty;

	fontsizex = 15.0f;

	for (i = 0; i < intTextLength; i++) {

		abort = 0;
		switch (text[i]) {

		case '@':

			tu = 1.0f;
			tv = 5.0f;
			break;

		case '!':

			tu = 2.0f;
			tv = 3.0f;
			break;
		case '#':

			tu = 4.0f;
			tv = 3.0f;
			break;
		case '$':

			tu = 5.0f;
			tv = 3.0f;
			break;
		case '%':

			tu = 6.0f;
			tv = 3.0f;
			break;
		case '&':

			tu = 7.0f;
			tv = 3.0f;

			break;

		case '\"':

			tu = 3.0f;
			tv = 3.0f;
			break;

		case '\'':

			tu = 8.0f;
			tv = 3.0f;
			break;

		case '(':

			tu = 9.0f;
			tv = 3.0f;
			break;

		case ')':

			tu = 10.0f;
			tv = 3.0f;
			break;

		case '*':

			tu = 11.0f;
			tv = 3.0f;
			break;

		case '^':

			tu = 15.0f;
			tv = 6.0f;
			break;

		case ';':

			tu = 12.0f;
			tv = 4.0f;
			break;

		case '<':

			tu = 13.0f;
			tv = 4.0f;
			break;

		case '=':

			tu = 14.0f;
			tv = 4.0f;
			break;

		case '>':

			tu = 15.0f;
			tv = 4.0f;
			break;

		case '?':

			tu = 16.0f;
			tv = 4.0f;
			break;

		case '\\':

			tu = 13.0f;
			tv = 6.0f;
			break;

		case ']':

			tu = 14.0f;
			tv = 6.0f;
			break;

		case '[':

			tu = 12.0f;
			tv = 6.0f;
			break;

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
			abort = 1;
			break;
		}

		if (abort == 0) {
			if (truecounter == 0)
				adjust = 0;
			else
				adjust += 4.0f;

			// 13
			// 02

			m_DisplayMessage[countmessage] =
			    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * (tu - 1.0f), tvad * tv);
			m_DisplayMessage[countmessage].sx = (x + ((float)truecounter * fontsizex)) - (adjust);
			m_DisplayMessage[countmessage].sy = y;
			m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);
			m_DisplayMessageFont[countmessage] = fonttype;

			countmessage++;

			m_DisplayMessage[countmessage] =
			    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * (tu - 1.0f), tvad * (tv - 1.0f));
			m_DisplayMessage[countmessage].sx = (x + ((float)truecounter * fontsizex)) - (adjust);
			m_DisplayMessage[countmessage].sy = (y - fontsizey);

			m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);
			m_DisplayMessageFont[countmessage] = fonttype;
			countmessage++;

			m_DisplayMessage[countmessage] =
			    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * tu, tvad * tv);
			m_DisplayMessage[countmessage].sx = (x + fontsizex + ((float)truecounter * fontsizex)) - (adjust);
			m_DisplayMessage[countmessage].sy = y;

			m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);
			m_DisplayMessageFont[countmessage] = fonttype;
			countmessage++;

			m_DisplayMessage[countmessage] =
			    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, tuad * tu, tvad * (tv - 1.0f));
			m_DisplayMessage[countmessage].sx = ((x + fontsizex + ((float)truecounter * fontsizex)) - (adjust));
			m_DisplayMessage[countmessage].sy = (y - fontsizey);

			m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);

			m_DisplayMessageFont[countmessage] = fonttype;
			countmessage++;
			truecounter++;
		}
	}
}

void CMyD3DApplication::display_box_calc(float x, float y, char text[2048], D3DVIEWPORT7 vp, int r, int g, int b, float fontx, float fonty, int fonttype) {

	int i = 0;
	float x1 = 0;
	float y1 = 0;
	int len = 0;
	int county = 2;
	int countx = 0;
	int counter = 0;

	len = strlen(text);

	for (i = 0; i < len; i++) {

		counter++;

		if (text[i] == '<' || i == len - 1) {

			if (counter > countx)
				countx = counter;

			county++;
			counter = 0;
		}
	}

	x = pCMyApp->RoundFloat(x);
	y = pCMyApp->RoundFloat(y);
	fontx = pCMyApp->RoundFloat(fontx);
	fonty = pCMyApp->RoundFloat(fonty);

	x = (FLOAT)((vp.dwWidth) / 2.0f) - ((countx * 15.0f) / 2.0f);
	y = (FLOAT)((vp.dwHeight) / 2.0f) - ((county * 16.0f) / 2.0f);

	fontx = (FLOAT)((vp.dwWidth) / 2.0f) + ((countx * 15.0f) / 2.0f);
	fonty = (FLOAT)((vp.dwHeight) / 2.0f) + ((county * 16.0f) / 2.0f);

	display_box(x, y, text, vp, r, g, b, fontx, fonty, fonttype);
}

void CMyD3DApplication::display_box(float x, float y, char text[2048], D3DVIEWPORT7 vp, int r, int g, int b, float fontx, float fonty, int fonttype) {

	// 13
	// 02

	float adjust = 0;
	float tuad = .0625f;
	float tvad = .0625f;

	int abort = 0;

	int intTextLength = 0;
	intTextLength = strlen(text);
	int i = 0;
	int truecounter = 0;

	float tu = 1.0f, tv = 1.0f;

	float fontsizex = 14;
	float fontsizey = 16;

	fontsizex = fontx;
	fontsizey = fonty;

	// 13
	// 02

	m_DisplayMessage[countmessage] =
	    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 1);
	m_DisplayMessage[countmessage].sx = x;
	m_DisplayMessage[countmessage].sy = fonty;
	m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);
	m_DisplayMessageFont[countmessage] = fonttype;
	countmessage++;

	m_DisplayMessage[countmessage] =
	    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 0, 0);
	m_DisplayMessage[countmessage].sx = x;
	m_DisplayMessage[countmessage].sy = y;
	m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);
	m_DisplayMessageFont[countmessage] = fonttype;
	countmessage++;

	m_DisplayMessage[countmessage] =
	    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 1);
	m_DisplayMessage[countmessage].sx = fontx;
	m_DisplayMessage[countmessage].sy = fonty;
	m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);
	m_DisplayMessageFont[countmessage] = fonttype;
	countmessage++;

	m_DisplayMessage[countmessage] =
	    D3DTLVERTEX(D3DVECTOR(0, 0, 0.99f), 0.5f, -1, 0, 1, 0);
	m_DisplayMessage[countmessage].sx = fontx;
	m_DisplayMessage[countmessage].sy = y;
	m_DisplayMessage[countmessage].color = RGBA_MAKE(r, g, b, 0);
	m_DisplayMessageFont[countmessage] = fonttype;
	countmessage++;
}

void CMyD3DApplication::AddTreasure(float x, float y, float z, int gold) {

	int i = 0;

	// fix this

	int raction;

	for (i = 0; i < MAX_NUM_ITEMS; i++) {

		if (item_list[i].bIsPlayerValid == FALSE) {

			if (gold == 0) {
				raction = 0;
			} else {
				raction = random_num(10);
			}

			if (raction == 0) {
				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y - 10.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("POTION");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 85;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = 0;

				strcpy_s(item_list[i].rname, "POTION");
			} else if (raction == 1) {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y - 10.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("diamond");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 85;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = gold;

				strcpy_s(item_list[i].rname, "diamond");
			} else if (raction == 2) {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y - 10.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("SCROLL-MAGICMISSLE-");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 0;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = gold;

				strcpy_s(item_list[i].rname, "SCROLL-MAGICMISSLE-");
			} else if (raction == 3) {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y - 10.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("SCROLL-FIREBALL-");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 0;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = gold;

				strcpy_s(item_list[i].rname, "SCROLL-FIREBALL-");
			} else if (raction == 4) {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y - 10.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("SCROLL-LIGHTNING-");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 0;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = gold;

				strcpy_s(item_list[i].rname, "SCROLL-LIGHTNING-");
			}

			else {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y - 10.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("COIN");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 85;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = gold;

				strcpy_s(item_list[i].rname, "COIN");
			}

			if (i >= itemlistcount)
				itemlistcount++;

			break;
		}
	}
}

void CMyD3DApplication::AddTreasureDrop(float x, float y, float z, int raction) {
	int i = 0;

	// fix this
	for (i = 0; i < MAX_NUM_ITEMS; i++) {

		if (item_list[i].bIsPlayerValid == FALSE) {

			if (raction == 0) {
				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y + 40.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("POTION");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 85;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = 0;

				strcpy_s(item_list[i].rname, "POTION");
			} else if (raction == 1) {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y + 40.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("diamond");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 85;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = 10;

				strcpy_s(item_list[i].rname, "diamond");
			} else if (raction == 2) {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y + 40.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("COIN");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 85;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = 5;

				strcpy_s(item_list[i].rname, "COIN");
			} else if (raction == 3) {

				item_list[i].bIsPlayerValid = TRUE;
				item_list[i].x = x;
				item_list[i].y = y + 40.0f;
				item_list[i].z = z;
				item_list[i].rot_angle = 0;
				item_list[i].model_id = pCMyApp->FindModelID("KEY2");
				item_list[i].skin_tex_id = (int)-1;
				item_list[i].current_sequence = 0;
				item_list[i].current_frame = 85;
				item_list[i].draw_external_wep = FALSE;
				item_list[i].monsterid = (int)9999;
				item_list[i].bIsPlayerAlive = TRUE;
				item_list[i].gold = 5;

				strcpy_s(item_list[i].rname, "KEY2");
			}

			if (i >= itemlistcount)
				itemlistcount++;

			break;
		}
	}
}

void CMyD3DApplication::statusbardisplay(float x, float length, int type) {

	int bar = 8;
	int truelength = 0;
	int i = 0;
	strcpy_s(statusbar, "");

	truelength = (int)((x / length) * bar);

	for (i = 0; i < truelength; i++) {

		if (type == 0)
			strcat_s(statusbar, "|");
		else
			strcat_s(statusbar, "`");
	}

	if (strlen(statusbar) == 0 && type == 0) {
		if (player_list[trueplayernum].health > 0)
			strcat_s(statusbar, "|");
	}
}

int CMyD3DApplication::save_game(char *filename) {
	FILE *fp;
	int montry;
	Pause(TRUE);
	UINT resultclick = MessageBox(main_window_handle, "Save Game?", "Save Game", MB_ICONQUESTION | MB_YESNO);
	Pause(FALSE);
	if (resultclick == IDYES) {

		if (strlen(filename) > 0) {
			if (fopen_s(&fp, filename, "wb+") != 0) {
				return 0;
			}
		} else {

			if (fopen_s(&fp, "ds.sav", "wb+") != 0) {
				return 0;
			}
		}

		fwrite(&pCMyApp->levelname, sizeof(char), 50, fp);
		fwrite(&pCMyApp->current_level, sizeof(int), 1, fp);
		fwrite(&pCMyApp->angy, sizeof(int), 1, fp);
		fwrite(&pCMyApp->look_up_ang, sizeof(int), 1, fp);
		fwrite(&perspectiveview, sizeof(int), 1, fp);
		fwrite(&current_gun, sizeof(int), 1, fp);

		fwrite(&pCMyApp->player_list[pCMyApp->trueplayernum], sizeof(struct player_typ), 1, fp);

		fwrite(&pCMyApp->num_players2, sizeof(int), 1, fp);
		for (montry = 0; montry < num_players2; montry++) {
			fwrite(&pCMyApp->player_list2[montry], sizeof(struct player_typ), 1, fp);
		}

		fwrite(&pCMyApp->num_monsters, sizeof(int), 1, fp);
		for (montry = 0; montry < num_monsters; montry++) {
			fwrite(&pCMyApp->monster_list[montry], sizeof(struct player_typ), 1, fp);
		}
		fwrite(&pCMyApp->itemlistcount, sizeof(int), 1, fp);
		for (montry = 0; montry < itemlistcount; montry++) {
			fwrite(&pCMyApp->item_list[montry], sizeof(struct player_typ), 1, fp);
		}
		fwrite(&doorcounter, sizeof(int), 1, fp);
		for (montry = 0; montry < doorcounter; montry++) {
			fwrite(&door[montry], sizeof(struct doors), 1, fp);
		}

		fwrite(&num_your_guns, sizeof(int), 1, fp);
		for (montry = 0; montry < num_your_guns; montry++) {
			fwrite(&your_gun[montry], sizeof(struct gunlist_typ), 1, fp);
		}

		fwrite(&countswitches, sizeof(int), 1, fp);
		for (montry = 0; montry < countswitches; montry++) {
			fwrite(&switchmodify[montry], sizeof(struct SwitchMod), 1, fp);
		}
		fwrite(&totalmod, sizeof(int), 1, fp);
		for (montry = 0; montry < totalmod; montry++) {
			fwrite(&levelmodify[montry], sizeof(struct LevelMod), 1, fp);
		}
		fwrite(&textcounter, sizeof(int), 1, fp);
		for (montry = 0; montry < textcounter; montry++) {
			fwrite(&gtext[montry], sizeof(struct gametext), 1, fp);
		}

		fclose(fp);
		return 1;
	}

	return 0;
}

int CMyD3DApplication::load_game(char *filename) {
	FILE *fp;
	int montry;
	Pause(TRUE);
	UINT resultclick = MessageBox(main_window_handle, "Load Game?", "Load Game", MB_ICONQUESTION | MB_YESNO);
	Pause(FALSE);
	if (resultclick == IDYES) {

		if (strlen(filename) > 0) {

			if (fopen_s(&fp, filename, "rb") != 0) {
				return 0;
			}
		} else {

			if (fopen_s(&fp, "ds.sav", "rb") != 0) {
				return 0;
			}
		}

		fread(&pCMyApp->levelname, sizeof(char), 50, fp);
		fread(&pCMyApp->current_level, sizeof(int), 1, fp);
		fread(&pCMyApp->angy, sizeof(int), 1, fp);
		fread(&pCMyApp->look_up_ang, sizeof(int), 1, fp);
		fread(&perspectiveview, sizeof(int), 1, fp);
		fread(&current_gun, sizeof(int), 1, fp);
		load_level(pCMyApp->levelname);
		fread(&pCMyApp->player_list[pCMyApp->trueplayernum], sizeof(struct player_typ), 1, fp);

		player_list[trueplayernum].y += 100.0f;
		m_vEyePt.x = player_list[trueplayernum].x;
		m_vEyePt.y = player_list[trueplayernum].y;
		m_vEyePt.z = player_list[trueplayernum].z;

		m_vLookatPt.x = player_list[trueplayernum].x;
		m_vLookatPt.y = player_list[trueplayernum].y + 10.0f;
		m_vLookatPt.z = player_list[trueplayernum].z;

		fread(&pCMyApp->num_players2, sizeof(int), 1, fp);

		for (montry = 0; montry < num_players2; montry++) {
			fread(&pCMyApp->player_list2[montry], sizeof(struct player_typ), 1, fp);
		}

		fread(&pCMyApp->num_monsters, sizeof(int), 1, fp);

		for (montry = 0; montry < num_monsters; montry++) {
			fread(&pCMyApp->monster_list[montry], sizeof(struct player_typ), 1, fp);
		}

		fread(&pCMyApp->itemlistcount, sizeof(int), 1, fp);

		for (montry = 0; montry < itemlistcount; montry++) {
			fread(&pCMyApp->item_list[montry], sizeof(struct player_typ), 1, fp);
		}

		fread(&doorcounter, sizeof(int), 1, fp);

		for (montry = 0; montry < doorcounter; montry++) {
			fread(&door[montry], sizeof(struct doors), 1, fp);
			oblist[door[montry].doornum].rot_angle = (float)door[montry].angle;

			if (door[montry].y != 0)
				oblist[door[montry].doornum].y = (float)door[montry].y;
		}

		fread(&num_your_guns, sizeof(int), 1, fp);
		for (montry = 0; montry < num_your_guns; montry++) {
			fread(&your_gun[montry], sizeof(struct gunlist_typ), 1, fp);
		}

		SwitchGun(current_gun);
		player_list[trueplayernum].gunid = your_gun[current_gun].model_id;
		player_list[trueplayernum].guntex = your_gun[current_gun].skin_tex_id;
		player_list[trueplayernum].damage1 = your_gun[current_gun].damage1;
		player_list[trueplayernum].damage2 = your_gun[current_gun].damage2;

		MakeDamageDice();

		fread(&countswitches, sizeof(int), 1, fp);
		for (montry = 0; montry < countswitches; montry++) {
			fread(&switchmodify[montry], sizeof(struct SwitchMod), 1, fp);
		}
		fread(&totalmod, sizeof(int), 1, fp);
		for (montry = 0; montry < totalmod; montry++) {
			fread(&levelmodify[montry], sizeof(struct LevelMod), 1, fp);
		}
		fread(&textcounter, sizeof(int), 1, fp);
		for (montry = 0; montry < textcounter; montry++) {
			fread(&gtext[montry], sizeof(struct gametext), 1, fp);
		}

		fclose(fp);

		return 1;
	}
	return 0;
}

int CMyD3DApplication::load_level(char *filename) {

	char level[255];

	/*
	if (strcmp(pCMyApp->ds_reg->registered,"0") ==0 && pCMyApp->current_level>=3) {

	    Pause(TRUE);
	    UINT resultclick = MessageBox(main_window_handle,"Please register at http://www.murk.on.ca","Register Game",MB_OK);
	    Pause(FALSE);
	    PostQuitMessage(0);
	}
	*/

	if (strlen(filename) > 0) {
		strcpy_s(level, filename);
		strcat_s(level, ".map");
		strcpy_s(levelname, filename);
	} else {
		sprintf_s(levelname, "level%d", pCMyApp->current_level);
		strcpy_s(level, levelname);
		strcat_s(level, ".map");
	}

	if (!pCWorld->LoadWorldMap(m_hWnd, level)) {
	}

	pCMyApp->num_players2 = 0;
	pCMyApp->itemlistcount = 0;
	pCMyApp->num_monsters = 0;

	pCMyApp->ClearObjectList();
	ResetSound();
	pCWorld->LoadSoundFiles(m_hWnd, "sounds.dat");

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

	for (int i = 0; i < MAX_NUM_GUNS; i++) {

		if (i == 0) {
			your_gun[i].active = 1;
			your_gun[i].x_offset = 0;
		} else if (i == 18) {
			your_gun[i].active = 1;
			your_gun[i].x_offset = 2;
		} else {
			your_gun[i].active = 0;

			your_gun[i].x_offset = 0;
		}
	}

	player_list[trueplayernum].gunid = your_gun[current_gun].model_id;
	player_list[trueplayernum].guntex = your_gun[current_gun].skin_tex_id;
	player_list[trueplayernum].damage1 = your_gun[current_gun].damage1;
	player_list[trueplayernum].damage2 = your_gun[current_gun].damage2;

	MakeDamageDice();

	for (int i = 0; i < MAX_MISSLE; i++) {

		if (your_missle[i].sexplode != 0)
			DSound_Delete_Sound(your_missle[i].sexplode);

		if (your_missle[i].smove != 0)
			DSound_Delete_Sound(your_missle[i].smove);

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

	strcpy_s(level, levelname);
	strcat_s(level, ".mod");

	pCWorld->LoadMod(m_hWnd, level);

	return 1;
}
void CMyD3DApplication::ScanMod() {
	int i = 0;
	int j = 0;
	int gotone = 0;

	int counter = 0;
	float qdist = 0;
	LevelModTime = timeGetTime() * 0.001f;

	for (i = 0; i < pCMyApp->totalmod; i++) {

		for (j = 0; j < pCMyApp->num_monsters; j++) {
			if (monster_list[j].monsterid == pCMyApp->levelmodify[counter].objectid &&
			    pCMyApp->levelmodify[counter].active == 1) {
				// DropTreasure
				if (strstr(pCMyApp->levelmodify[counter].Function, "DropTreasure") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {
					if (monster_list[j].bIsPlayerAlive == FALSE) {
						pCMyApp->levelmodify[counter].active = 2;
						AddTreasureDrop(monster_list[j].x, monster_list[j].y, monster_list[j].z, atoi(pCMyApp->levelmodify[counter].Text1));
					}
				}
				// MonsterActive
				if (strstr(pCMyApp->levelmodify[counter].Function, "MonsterActive") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {
					if (monster_list[j].ability == 0) {
						if (gotone == 0) {
							pCMyApp->DisplayDialogText(pCMyApp->levelmodify[counter].Text1, -20.0f);
						}
						gotone = 1;

						ScanModJump(pCMyApp->levelmodify[counter].jump);
						if (countmodtime == 0) {
							LevelModLastTime = timeGetTime() * 0.001f;
							countmodtime = 1;
						}

						if (LevelModTime - LevelModLastTime >= 5.0f) {
							pCMyApp->levelmodify[counter].active = 0;
							countmodtime = 0;
						}
					}
				}
				// XP
				if (strstr(pCMyApp->levelmodify[counter].Function, "XPPoints") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {
					if (monster_list[j].bIsPlayerAlive == FALSE) {
						pCMyApp->levelmodify[counter].active = 0;
						int xp = atoi(pCMyApp->levelmodify[counter].Text1);
						sprintf_s(gActionMessage, "You got %d XP!.", xp);
						UpdateScrollList(0, 245, 255);
						player_list[trueplayernum].xp += xp;
						LevelUp(player_list[trueplayernum].xp);
					}
				}

				// SetHitPoints

				if (strstr(pCMyApp->levelmodify[counter].Function, "SetHitPoints") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {

					pCMyApp->levelmodify[counter].active = 0;
					monster_list[j].health = atoi(pCMyApp->levelmodify[counter].Text1);
					monster_list[j].hp = atoi(pCMyApp->levelmodify[counter].Text1);
				}

				// IsDeadText
				if (strstr(pCMyApp->levelmodify[counter].Function, "IsDeadText") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {
					if (monster_list[j].bIsPlayerAlive == FALSE) {
						if (gotone == 0) {
							pCMyApp->DisplayDialogText(pCMyApp->levelmodify[counter].Text1, -20.0f);
						}
						gotone = 1;

						ScanModJump(pCMyApp->levelmodify[counter].jump);
						if (countmodtime == 0) {
							LevelModLastTime = timeGetTime() * 0.001f;
							countmodtime = 1;
						}

						if (LevelModTime - LevelModLastTime >= 5.0f) {
							pCMyApp->levelmodify[counter].active = 0;
							countmodtime = 0;
						}
					}
				}
				// isNear
				if (strstr(pCMyApp->levelmodify[counter].Function, "IsNear") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {
					qdist = FastDistance(
					    player_list[trueplayernum].x - monster_list[j].x,
					    player_list[trueplayernum].y - monster_list[j].y,
					    player_list[trueplayernum].z - monster_list[j].z);

					if (qdist < 300.0f) {
						if (gotone == 0) {
							pCMyApp->DisplayDialogText(pCMyApp->levelmodify[counter].Text1, -20.0f);
						}
						gotone = 1;

						ScanModJump(pCMyApp->levelmodify[counter].jump);
						if (countmodtime == 0) {
							LevelModLastTime = timeGetTime() * 0.001f;
							countmodtime = 1;
						}

						if (LevelModTime - LevelModLastTime >= 5.0f) {
							pCMyApp->levelmodify[counter].active = 0;
							countmodtime = 0;
						}
					}
				}
			}
			// 	break;
		}

		for (j = 0; j < pCMyApp->num_players2; j++) {
			if (player_list2[j].monsterid == pCMyApp->levelmodify[counter].objectid &&
			    pCMyApp->levelmodify[counter].active == 1) {
				// Moveup
				if (strstr(pCMyApp->levelmodify[counter].Function, "MoveUp") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {

					if (levelmodify[counter].currentheight == -9999)
						levelmodify[counter].currentheight = player_list2[j].y;

					ScanModJump(pCMyApp->levelmodify[counter].jump);
					if (player_list2[j].y - levelmodify[counter].currentheight <= atoi(levelmodify[counter].Text1)) {
						// portcullis speed
						player_list2[j].y = player_list2[j].y + (50.0f * elapsegametimersave);
						qdist = FastDistance(
						    player_list[trueplayernum].x - player_list2[j].x,
						    player_list[trueplayernum].y - player_list2[j].y,
						    player_list[trueplayernum].z - player_list2[j].z);
						closesoundid[3] = qdist;
					} else {
						levelmodify[counter].active = 0;
					}
				}
				// isswitch
				if (strstr(pCMyApp->levelmodify[counter].Function, "IsSwitch") != NULL) {
					for (int t = 0; t < pCMyApp->countswitches; t++) {
						if (j == switchmodify[t].num) {
							if (switchmodify[t].active == 2) {
								levelmodify[levelmodify[counter].jump - 1].active = 1;
							}
						}
					}
				}

				// isNear
				if (strstr(pCMyApp->levelmodify[counter].Function, "IsNear") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {
					qdist = FastDistance(
					    player_list[trueplayernum].x - player_list2[j].x,
					    player_list[trueplayernum].y - player_list2[j].y,
					    player_list[trueplayernum].z - player_list2[j].z);

					if (qdist < 300.0f) {
						if (gotone == 0) {
							pCMyApp->DisplayDialogText(pCMyApp->levelmodify[counter].Text1, -20.0f);
						}
						gotone = 1;

						ScanModJump(pCMyApp->levelmodify[counter].jump);
						if (countmodtime == 0) {
							LevelModLastTime = timeGetTime() * 0.001f;
							countmodtime = 1;
						}

						if (LevelModTime - LevelModLastTime >= 5.0f) {
							pCMyApp->levelmodify[counter].active = 0;
							countmodtime = 0;
						}
					}
				}
			}
		}

		for (j = 0; j < pCMyApp->itemlistcount; j++) {
			if (item_list[j].monsterid == pCMyApp->levelmodify[counter].objectid &&
			    pCMyApp->levelmodify[counter].active == 1) {
				// Moveup
				if (strstr(pCMyApp->levelmodify[counter].Function, "MoveUp") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {

					if (levelmodify[counter].currentheight == -9999)
						levelmodify[counter].currentheight = item_list[j].y;

					ScanModJump(pCMyApp->levelmodify[counter].jump);
					if (item_list[j].y - levelmodify[counter].currentheight <= atoi(levelmodify[counter].Text1)) {
						item_list[j].y++;
						qdist = FastDistance(
						    player_list[trueplayernum].x - item_list[j].x,
						    player_list[trueplayernum].y - item_list[j].y,
						    player_list[trueplayernum].z - item_list[j].z);
						closesoundid[3] = qdist;
					} else {
						levelmodify[counter].active = 0;
					}
				}

				// TreasueAmount
				if (strstr(pCMyApp->levelmodify[counter].Function, "TreasureAmount") != NULL &&
				    pCMyApp->levelmodify[counter].active == 1) {
					pCMyApp->levelmodify[counter].active = 0;

					item_list[j].gold = atoi(pCMyApp->levelmodify[counter].Text1);
				}
			}
		}
		counter++;
	}
}

void CMyD3DApplication::ScanModJump(int jump) {

	int flag = 1;
	int newjump = 0;

	while (flag == 1) {
		if (levelmodify[jump - 1].active == 0) {

			levelmodify[jump - 1].active = 1;
			jump = levelmodify[jump - 1].jump;
			if (jump == 0)
				flag = 0;
		} else
			flag = 0;
	}
}

void CMyD3DApplication::ActivateSwitch() {

	int i = 0;
	int j = 0;
	float qdist;

	for (i = 0; i < countswitches; i++) {

		if (switchmodify[i].active != 2) {
			qdist = FastDistance(
			    player_list[trueplayernum].x - switchmodify[i].x,
			    player_list[trueplayernum].y - switchmodify[i].y,
			    player_list[trueplayernum].z - switchmodify[i].z);

			if (qdist < 200.0f && switchmodify[i].active == 0 && pressopendoor == 1) {
				switchmodify[i].active = 1;
				PlayWavSound(SoundID("stone"), 100);
			}

			if (switchmodify[i].active == 1) {

				j = switchmodify[i].num;

				if (switchmodify[i].direction == 1)
					player_list2[j].x = player_list2[j].x + 5.0f;
				else if (switchmodify[i].direction == 2)
					player_list2[j].z = player_list2[j].z + 5.0f;
				else if (switchmodify[i].direction == 3)
					player_list2[j].x = player_list2[j].x - 5.0f;
				else if (switchmodify[i].direction == 4)
					player_list2[j].z = player_list2[j].z - 5.0f;

				if (switchmodify[i].count > 2) {
					switchmodify[i].active = 2;
				}
				switchmodify[i].count++;
			}
		}
	}
}

void CMyD3DApplication::RegenrateHP() {

	startregen = timeGetTime() * 0.001f; // Get current time in seconds

	if (startregen - startregenlast > 10.0f &&
	    player_list[trueplayernum].health > 0) {
		startregenlast = startregen;
		if (player_list[trueplayernum].health < player_list[trueplayernum].hp) {
			player_list[trueplayernum].health = player_list[trueplayernum].health + 1;
		}
	}
}

void CMyD3DApplication::ApplyMissleDamage(int playernum) {
	int misslecount = 0, i = 0;
	D3DVECTOR MissleMove;
	float qdist;

	// type 0=monster 1=player

	for (misslecount = 0; misslecount < MAX_MISSLE; misslecount++) {
		if (your_missle[misslecount].active == 1) {
			// deal with your missles on monster
			for (i = 0; i < num_monsters; i++) {
				if (monster_list[i].bIsPlayerValid == TRUE &&
				    monster_list[i].bIsPlayerAlive == TRUE &&
				    monster_list[i].health > 0 &&
				    strcmp(monster_list[i].rname, "SLAVE") != 0 &&
				    strcmp(monster_list[i].rname, "CENTAUR") != 0) {

					qdist = FastDistance(your_missle[misslecount].x - monster_list[i].x,
					                     your_missle[misslecount].y - monster_list[i].y,
					                     your_missle[misslecount].z - monster_list[i].z);

					if (qdist < 60.0f) {

						if (your_missle[misslecount].playertype == 0) {
							// missle is a monsters
							// dont explode it
						} else if (your_missle[misslecount].owner == trueplayernum) {
							// it is your missle
							your_missle[misslecount].active = 2;

							int volume;
							volume = 100 - (int)((100 * your_missle[misslecount].qdist) / ((numberofsquares * monsterdist) / 2));
							if (volume < 10)
								volume = 10;

							if (volume > 100)
								volume = 100;

							PlayWavSound(your_missle[misslecount].sexplode, volume);

							int attackbonus = your_gun[your_missle[misslecount].guntype].sattack;
							int damagebonus = your_gun[your_missle[misslecount].guntype].sdamage;
							int weapondamage = your_gun[your_missle[misslecount].guntype].damage2;
							int action;

							int lvl;
							lvl = player_list[trueplayernum].hd;
							if (lvl < 1)
								lvl = 1;

							action = your_missle[misslecount].dmg * lvl;

							action = action + damagebonus;

							int result = pCMyApp->SavingThrow(action, 1, your_missle[misslecount].owner, misslecount, 0, i);

							if (result != action) {
								sprintf_s(gActionMessage, "%s made a saving throw!", monster_list[i].rname);
								UpdateScrollList(0, 245, 255);
								sprintf_s(gActionMessage, "Your spell hit a %s for %d hp (%d/2).", monster_list[i].rname, result, action);
								UpdateScrollList(0, 245, 255);
								action = result;
							} else {
								sprintf_s(gActionMessage, "Your spell hit a %s for %d hp.", monster_list[i].rname, action);
								UpdateScrollList(0, 245, 255);
							}

							monster_list[i].health = monster_list[i].health - action;
							int painaction = random_num(3);
							SetMonsterAnimationSequence(i, 3 + painaction);
							monster_list[i].ability = 0;

							for (int t = 0; t < num_monsters; t++) {
								if (monster_list[t].ability == 2 && monster_list[t].dist < 600.0f) {
									monster_list[t].ability = 0;
								}
							}

							if (monster_list[i].health <= 0) {

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
						}
					}
				}
			}

			// deal with missles hitting you

			i = trueplayernum;
			if (player_list[i].bIsPlayerValid == TRUE &&
			    player_list[i].bIsPlayerAlive == TRUE && player_list[i].health > 0) {

				qdist = FastDistance(your_missle[misslecount].x - player_list[i].x,
				                     your_missle[misslecount].y - player_list[i].y,
				                     your_missle[misslecount].z - player_list[i].z);

				if (qdist < 60.0f) {

					if (your_missle[misslecount].owner == i &&
					    your_missle[misslecount].playertype == 1) {
						// its current players missle missle dont explode it
					} else if (your_missle[misslecount].owner != trueplayernum &&
					           your_missle[misslecount].playertype == 1)

					{
						// its other players missle

						your_missle[misslecount].active = 2;

						int volume;
						volume = 100 - (int)((100 * your_missle[misslecount].qdist) / ((numberofsquares * monsterdist) / 2));

						if (volume < 10)
							volume = 10;

						if (volume > 100)
							volume = 100;

						PlayWavSound(your_missle[misslecount].sexplode, volume);

						int dmg = 0;
						int raction = 0;
						if (your_missle[misslecount].y_offset == 1.0f) {
							raction = random_num(4) + 1;
						} else if (your_missle[misslecount].y_offset == 2.0f) {
							raction = random_num(6) + 1;
						} else if (your_missle[misslecount].y_offset == 3.0f) {
							raction = random_num(8) + 2;
						}

						int lvl;

						lvl = your_missle[misslecount].dmg;

						lvl = lvl / 2;
						if (lvl <= 1)
							lvl = 1;

						raction = raction * lvl;

						int result = pCMyApp->SavingThrow(raction, 1, your_missle[misslecount].owner, misslecount, 1, trueplayernum);

						if (result != raction) {
							sprintf_s(gActionMessage, "You made your saving throw!");
							UpdateScrollList(255, 0, 0);

							if (result == 0) {
								sprintf_s(gActionMessage, "A natural 20! No spell damage.");
							} else {
								sprintf_s(gActionMessage, "A spell hit you for %d hp (%d/2).", result, raction);
							}

							UpdateScrollList(255, 0, 0);
							raction = result;
						} else {
							sprintf_s(gActionMessage, "A spell hit you for %d hp", raction);
							UpdateScrollList(255, 0, 0);
						}

						ApplyPlayerDamage(trueplayernum, raction);
					}

					else if (
					    your_missle[misslecount].playertype == 0) {
						// its a monsters missle
						your_missle[misslecount].active = 2;

						int volume;
						volume = 100 - (int)((100 * your_missle[misslecount].qdist) / ((numberofsquares * monsterdist) / 2));

						if (volume < 10)
							volume = 10;

						if (volume > 100)
							volume = 100;

						PlayWavSound(your_missle[misslecount].sexplode, volume);

						int dmg = 0;
						int raction = 0;
						if (your_missle[misslecount].y_offset == 1.0f) {
							raction = random_num(4) + 1;
						} else if (your_missle[misslecount].y_offset == 2.0f) {
							raction = random_num(6) + 1;
						} else if (your_missle[misslecount].y_offset == 3.0f) {
							raction = random_num(8) + 2;
						}

						int lvl;

						lvl = monster_list[your_missle[misslecount].owner].hd;

						lvl = lvl / 2;
						if (lvl <= 1)
							lvl = 1;

						raction = raction * lvl;

						int result = pCMyApp->SavingThrow(raction, 0, your_missle[misslecount].owner, misslecount, 1, trueplayernum);

						if (result != raction) {
							sprintf_s(gActionMessage, "You made your saving throw!");
							UpdateScrollList(255, 0, 0);

							if (result == 0) {
								sprintf_s(gActionMessage, "A natural 20! No spell damage.");
							} else {
								sprintf_s(gActionMessage, "A spell hit you for %d hp (%d/2).", result, raction);
							}

							UpdateScrollList(255, 0, 0);
							raction = result;

						} else {
							sprintf_s(gActionMessage, "A spell hit you for %d hp", raction);
							UpdateScrollList(255, 0, 0);
						}

						ApplyPlayerDamage(trueplayernum, raction);
					}
				}
			}

			// missle near monster
			for (i = 0; i < num_monsters; i++) {

				if (monster_list[i].bIsPlayerValid == TRUE &&
				    monster_list[i].bIsPlayerAlive == TRUE && monster_list[i].health > 0

				) {

					qdist = FastDistance(your_missle[misslecount].x - monster_list[i].x,
					                     your_missle[misslecount].y - monster_list[i].y,
					                     your_missle[misslecount].z - monster_list[i].z);

					if (qdist < 60.0f) {
						if (your_missle[misslecount].playertype == 0) {

							// yours does nothing
						} else
							your_missle[misslecount].active = 2;
					}
				}
			}

			// missle near player (not you)
			for (i = 0; i < num_players; i++) {

				if (player_list[i].bIsPlayerValid == TRUE &&
				    player_list[i].bIsPlayerAlive == TRUE && i != trueplayernum &&
				    player_list[i].health > 0) {

					qdist = FastDistance(your_missle[misslecount].x - player_list[i].x,
					                     your_missle[misslecount].y - player_list[i].y,
					                     your_missle[misslecount].z - player_list[i].z);

					if (qdist < 60.0f) {

						// explode it

						if (your_missle[misslecount].owner == i &&
						    your_missle[misslecount].playertype == 1) {
							// yours does nothing
						} else {
							your_missle[misslecount].active = 2;
						}
					}
				}
			}
		}
	}
}

void CMyD3DApplication::GiveWeapons() {

	int i = 0;

	for (i = 0; i < MAX_NUM_GUNS; i++) {

		if (i < 10)
			your_gun[i].active = 1;

		your_gun[i].x_offset = 999.0f;
	}

	player_list[trueplayernum].gold = 99999;

	your_gun[18].active = 1;

	your_gun[19].active = 1;
	your_gun[20].active = 1;
	your_gun[21].active = 1;
}

void CMyD3DApplication::SwitchGun(int gun) {
	current_gun = gun;

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

	if (strstr(your_gun[current_gun].gunname, "FLAME") != NULL ||
	    strstr(your_gun[current_gun].gunname, "ICE") != NULL ||
	    strstr(your_gun[current_gun].gunname, "LIGHTNINGSWORD") != NULL) {
		bIsFlashlightOn = TRUE;
		lighttype = 2;
	} else {
		lighttype = 0;
		bIsFlashlightOn = FALSE;
	}
}

int CMyD3DApplication::PostServer() {
	return ((DWORD)1);
}

int CMyD3DApplication::DSOnlineInit() {
	strcpy_s(ds_online[0].ds_name, player_list[trueplayernum].name);
	strcpy_s(ds_online[0].ds_numplayers, "1");
	strcpy_s(ds_online[0].ds_ipaddress, ipaddress);
	strcpy_s(ds_online[0].ds_ipproxy, "");
	strcpy_s(ds_online[0].ds_webipaddress, "");
	strcpy_s(ds_online[0].ds_type, "SINGLEPLAYER");
	strcpy_s(ds_online[0].ds_action, "");
	strcpy_s(ds_online[0].ds_message, "STARTGAME");
	strcpy_s(ds_online[0].ds_version, "1.80");
	strcpy_s(ds_online[0].ds_game, "GAME1");
	return 1;
}

int CMyD3DApplication::DSPostQuit() {
	DSLevel();
	strcpy_s(ds_online[0].ds_message, "QUITGAME");
	strcpy_s(ds_online[0].ds_action, "UPDATE");
	PostServer();

	return 1;
}

int CMyD3DApplication::DSLevel() {

	char junk[255];
	sprintf_s(junk, "SINGLEPLAYER lvl:%d reg:%s", current_level, ds_reg->registered);
	strcpy_s(ds_online[0].ds_type, junk);
	strcpy_s(ds_online[0].ds_action, "UPDATE");
	strcpy_s(ds_online[0].ds_message, "UPDATE");
	PostServer();
	return 1;
}

int CMyD3DApplication::GetServer() {
	return ((DWORD)1);
}

int CMyD3DApplication::initDSTimer() {

	LONGLONG perf_cnt;
	LONGLONG count;
	int a = 0;


	if (QueryPerformanceFrequency((LARGE_INTEGER *)&perf_cnt) && highperftimer == 1) {
		// yes, timer choice flag
		perf_flag = TRUE;

		// set scaling factor
		time_factor = (double)1.0 / (double)perf_cnt;

		QueryPerformanceCounter((LARGE_INTEGER *)&count);

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
	} else {

		// no performance counter, read in using timeGetTime

		LONGLONG last_time = timeGetTime();

		// clear timer selection flag

		perf_flag = FALSE;

		// set timer scaling factor

		time_factor = (float)0.001;

		elapsegametimer = last_time;
		elapsegametimersave = 0;
		elapsegametimerlast = last_time;

		gametimerlast = last_time;
		gametimerlastpost = last_time;
		gametimerlast2 = last_time;
		gametimerlastdoor = last_time;
	}

	return 1;
}

LONGLONG CMyD3DApplication::DSTimer() {

	LONGLONG cur_time;
	// current timer value

	FLOAT a = 0;

	// read appropriate counter

	if (perf_flag) {
		QueryPerformanceCounter((LARGE_INTEGER *)&cur_time);
	} else {
		cur_time = timeGetTime();
	}

	return cur_time;
}

FLOAT CMyD3DApplication::LoadDSini() {

	FILE *fp;

	char command[256];

	if (fopen_s(&fp, "ds.ini", "r") != 0) {
	}

	if (fp == NULL) {
		pCMyApp->dsinix = 640;
		pCMyApp->dsiniy = 480;
		return 0;
	}

	fscanf_s(fp, "%s", &command, 256);
	pCMyApp->dsinix = atoi(command);

	fscanf_s(fp, "%s", &command, 256);
	pCMyApp->dsiniy = atoi(command);

	fscanf_s(fp, "%s", &command, 256);
	pCMyApp->dsiniwindowed = atoi(command);

	fscanf_s(fp, "%s", &command, 256);

	int inputtype = 1;

	fscanf_s(fp, "%s", &command, 256);
	inputtype = (int)atof(command);

	pCMyApp->currentinputtype = inputtype;

	fscanf_s(fp, "%s", &command, 256);
	musicon = atoi(command);

	fclose(fp);

	return 0.0f;
}

FLOAT CMyD3DApplication::SaveDSini() {

	FILE *fp;

	if (fopen_s(&fp, "ds.ini", "w") != 0) {
	}

	if (fp == NULL) {
		return 0;
	}

	fprintf(fp, "%d\n", pCMyApp->dsinix);
	fprintf(fp, "%d\n", pCMyApp->dsiniy);

	if (m_pDeviceInfo->bWindowed == 1) {
		fprintf(fp, "1\n");
	} else {
		fprintf(fp, "0\n");
	}
	fprintf(fp, "%2.4f\n", pCMyApp->gammasetting);

	if (pCMyApp->currentinputtype == 1) {
		fprintf(fp, "1\n");
	}

	if (pCMyApp->currentinputtype == 2) {
		fprintf(fp, "2\n");
	}

	if (pCMyApp->currentinputtype == 3) {
		fprintf(fp, "3\n");
	}

	if (musicon == 1)
		fprintf(fp, "1\n");
	else
		fprintf(fp, "0\n");

	fclose(fp);

	return 0.0f;
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice(DDCAPS *pddDriverCaps,
                                         D3DDEVICEDESC7 *pd3dDeviceDesc) {
	// Get device's stencil caps
	pCMyApp->hasgamma = 0;

	if (pddDriverCaps->dwCaps2 & DDCAPS2_PRIMARYGAMMA) {
		pCMyApp->hasgamma = 1;
	}

	DWORD dwStencilCaps = pd3dDeviceDesc->dwStencilCaps;

	return S_OK;
}

HRESULT CMyD3DApplication::FadeOut() {

	if (pCMyApp->hasgamma == 0)
		return 0;

	if (m_pDeviceInfo->bWindowed == 1)
		return 0;

	LPDIRECTDRAWGAMMACONTROL lpDDGammaControl = NULL;
	DDGAMMARAMP DDGammaRamp;
	DDGAMMARAMP DDGammaOld;

	(GetFramework())->GetFrontBuffer()->QueryInterface(IID_IDirectDrawGammaControl, (void **)&lpDDGammaControl);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaOld);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaRamp);

	float gamma = .5f;

	for (int i = 0; i < 256; i++) {
		DDGammaRamp.red[i] = CLAMP(0, (int)(255 * powf((float)((i + 0.5) / 255.5), gamma) + 0.5), 255) << 8;
		DDGammaRamp.green[i] = CLAMP(0, (int)(255 * powf((float)((i + 0.5) / 255.5), gamma) + 0.5), 255) << 8;
		DDGammaRamp.blue[i] = CLAMP(0, (int)(255 * powf((float)((i + 0.5) / 255.5), gamma) + 0.5), 255) << 8;
	}

	lpDDGammaControl->SetGammaRamp(0, &DDGammaRamp);

	return 1;
}

HRESULT CMyD3DApplication::FadeIn() {

	LPDIRECTDRAWGAMMACONTROL lpDDGammaControl = NULL;
	DDGAMMARAMP DDGammaRamp;
	DDGAMMARAMP DDGammaOld;

	if (pCMyApp->hasgamma == 0)
		return 0;

	if (m_pDeviceInfo->bWindowed == 1)
		return 0;

	(GetFramework())->GetFrontBuffer()->QueryInterface(IID_IDirectDrawGammaControl, (void **)&lpDDGammaControl);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaOld);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaRamp);
	/* Fade OUT */
	for (float fade = 1.f; fade >= 0.f; fade -= 0.01f) {
		for (int i = 0; i < 256; i++) {
			DDGammaRamp.red[i] = (int)((float)DDGammaOld.red[i] * fade);
			DDGammaRamp.green[i] = (int)((float)DDGammaOld.green[i] * fade);
			DDGammaRamp.blue[i] = (int)((float)DDGammaOld.blue[i] * fade);
		}
		Sleep(1);
		lpDDGammaControl->SetGammaRamp(0, &DDGammaRamp);
	}

	Sleep(10);

	return 1;
}

HRESULT CMyD3DApplication::SetGamma() {
	/*
	if (pCMyApp->gammasetting <= 0.1f)
	{
	    pCMyApp->gammasetting = 0.1f;
	}
	if (pCMyApp->hasgamma == 0)
	    return 0;

	if (m_pDeviceInfo->bWindowed == 1)
	    return 0;

	LPDIRECTDRAWGAMMACONTROL lpDDGammaControl = NULL;
	DDGAMMARAMP DDGammaRamp;
	DDGAMMARAMP DDGammaOld;

	(GetFramework())->GetFrontBuffer()->QueryInterface(IID_IDirectDrawGammaControl, (void**)&lpDDGammaControl);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaOld);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaRamp);

	for (int i = 0; i < 256; i++)
	{
	    DDGammaRamp.red[i] = CLAMP(0, (int)(255 * powf((float)((i + 0.5) / 255.5), pCMyApp->gammasetting) + 0.5), 255) << 8;
	    DDGammaRamp.green[i] = CLAMP(0, (int)(255 * powf((float)((i + 0.5) / 255.5), pCMyApp->gammasetting) + 0.5), 255) << 8;
	    DDGammaRamp.blue[i] = CLAMP(0, (int)(255 * powf((float)((i + 0.5) / 255.5), pCMyApp->gammasetting) + 0.5), 255) << 8;
	}

	lpDDGammaControl->SetGammaRamp(0, &DDGammaRamp);
	*/

	return 1;
}

HRESULT CMyD3DApplication::SetGammaFadeOut() {

	if (pCMyApp->gammasetting <= 0.1f) {
		pCMyApp->gammasetting = 0.1f;
	}
	if (pCMyApp->hasgamma == 0)
		return 0;

	if (m_pDeviceInfo->bWindowed == 1)
		return 0;

	LPDIRECTDRAWGAMMACONTROL lpDDGammaControl = NULL;
	DDGAMMARAMP DDGammaRamp;
	DDGAMMARAMP DDGammaOld;

	(GetFramework())->GetFrontBuffer()->QueryInterface(IID_IDirectDrawGammaControl, (void **)&lpDDGammaControl);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaOld);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaRamp);

	for (float fade = 1.f; fade >= 0.0f; fade -= 0.01f) {
		for (int i = 0; i < 256; i++) {
			DDGammaRamp.red[i] = (int)((float)DDGammaOld.red[i] * fade);
			DDGammaRamp.green[i] = (int)((float)DDGammaOld.green[i] * fade);
			DDGammaRamp.blue[i] = (int)((float)DDGammaOld.blue[i] * fade);
		}
		Sleep(1);
		lpDDGammaControl->SetGammaRamp(0, &DDGammaRamp);
	}

	return 1;
}

HRESULT CMyD3DApplication::SetGammaFadeIn() {

	if (pCMyApp->gammasetting <= 0.1f) {
		pCMyApp->gammasetting = 0.1f;
	}
	if (pCMyApp->hasgamma == 0)
		return 0;

	if (m_pDeviceInfo->bWindowed == 1)
		return 0;

	LPDIRECTDRAWGAMMACONTROL lpDDGammaControl = NULL;

	(GetFramework())->GetFrontBuffer()->QueryInterface(IID_IDirectDrawGammaControl, (void **)&lpDDGammaControl);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaOldsave);
	lpDDGammaControl->GetGammaRamp(0, &DDGammaRampsave);

	fadeoutcount += 0.01f;

	float fade = fadeoutcount;

	if (fadeoutcount >= 1.0f) {

		gammafadeout = 0;
		fadeoutcount = 0.0f;
	}

	for (int i = 0; i < 256; i++) {
		DDGammaRampsave.red[i] = (int)fade;
		DDGammaRampsave.green[i] = (int)fade;
		DDGammaRampsave.blue[i] = (int)fade;
	}
	Sleep(1);
	lpDDGammaControl->SetGammaRamp(0, &DDGammaRampsave);

	return 1;
}

HRESULT CMyD3DApplication::menumouseselect() {

	int price = 0;
	int q = 0;
	int k = 0;
	char junk[255];
	//	int oldcount=0;

	buttonselect = -1;

	// buttonselect=1;

	POINT ptCurPos;

	::GetCursorPos(&ptCurPos);

	if (pCMyApp->dsiniwindowed)
		ScreenToClient(main_window_handle, &ptCurPos);

	float x1 = (float)ptCurPos.x;
	float y1 = (float)ptCurPos.y;
	// 24
	// 13

	int i = 0;
	for (int j = 0; j < 7; j++) {
		if (x1 < m_DisplayMessage[6 + i].sx && y1 > m_DisplayMessage[5 + i].sy && x1 > m_DisplayMessage[4 + i].sx && y1 < m_DisplayMessage[4 + i].sy) {
			buttonselect = j + 1;
			// new game
			if (::GetAsyncKeyState(VK_LBUTTON) == 0 && merchantmousebutton != 0) {
				merchantmousebutton = 0;
			}
			if (::GetAsyncKeyState(VK_LBUTTON) < 0 && merchantmousebutton == 0) {

				merchantmousebutton = 1;
				PlayWavSound(SoundID("buttonselect"), 100);
				if (openingscreen == 3) {
					if (buttonselect == 1) {
						merchantnextitem();
					} else if (buttonselect == 2) {
						merchantprevitem();
					} else if (buttonselect == 3) {

						char desc[255];

						if (merchantmode == 0) {
							sprintf_s(desc, "Buy %s for %d ?", merchantlist[merchantcurrent].Text1, merchantlist[merchantcurrent].price);

							if (player_list[trueplayernum].gold < merchantlist[merchantcurrent].price) {
								Pause(TRUE);
								UINT resultclick = MessageBox(main_window_handle, "You have not enough coin!", "Buy Item", MB_OK);
								Pause(FALSE);
							} else {
								Pause(TRUE);
								UINT resultclick = MessageBox(main_window_handle, desc, "Buy Item", MB_ICONQUESTION | MB_YESNO);
								Pause(FALSE);
								if (resultclick == IDYES) {
									// add the weapon
									if (strstr(merchantlist[merchantcurrent].Text1, "armour") != NULL) {
										StartFlare(2);
										PlayWavSound(SoundID("potion"), 100);
										PlayWavSound(SoundID("goal4"), 100);
										player_list[trueplayernum].ac--;
									} else if (strstr(merchantlist[merchantcurrent].Text1, "POTION") != NULL) {
										// fix this generic
										int hp = random_num(8) + 1;
										player_list[trueplayernum].health = player_list[trueplayernum].health + hp;

										if (player_list[trueplayernum].health > player_list[trueplayernum].hp)
											player_list[trueplayernum].health = player_list[trueplayernum].hp;

										PlayWavSound(SoundID("potion"), 100);
										StartFlare(3);
										sprintf_s(gActionMessage, "You drank a potion worth %d health", hp);
										UpdateScrollList(0, 255, 255);
										LevelUp(player_list[trueplayernum].xp);
									} else {

										for (q = 0; q <= num_your_guns; q++) {
											char junk[255];
											strcpy_s(junk, merchantlist[merchantcurrent].Text1);
											if (your_gun[q].model_id == FindModelID(junk) ||
											    strstr(your_gun[q].gunname, junk) != NULL &&
											        strstr(your_gun[q].gunname, "SCROLL") != NULL) {

												if (your_gun[q].active == 1 && strstr(your_gun[q].gunname, "SCROLL") == NULL) {

													Pause(TRUE);
													UINT resultclick = MessageBox(main_window_handle, "You already have that item.", "Buy Item", MB_OK);
													Pause(FALSE);
												} else {
													your_gun[q].active = 1;

													if (strstr(your_gun[q].gunname, "SCROLL") != NULL) {
														your_gun[q].x_offset++;
													}

													// Pay for the new weapon or spell.
													PlayWavSound(SoundID("coin"), 100);
													player_list[trueplayernum].gold = player_list[trueplayernum].gold - merchantlist[merchantcurrent].price;
												}
											}
										}
									}
								}
							}
						} else {

							if (merchantlist[merchantcurrent].price <= 0) {

								Pause(TRUE);
								UINT resultclick = MessageBox(main_window_handle, "You cannot sell this item!", "Buy Item", MB_OK);
								Pause(FALSE);
							}

							else {

								sprintf_s(desc, "Sell %s for %d ?", merchantlist[merchantcurrent].Text1, merchantlist[merchantcurrent].price);

								Pause(TRUE);
								UINT resultclick = MessageBox(main_window_handle, desc, "Sell Item", MB_ICONQUESTION | MB_YESNO);
								Pause(FALSE);

								if (resultclick == IDYES) {

									PlayWavSound(SoundID("coin"), 100);
									player_list[trueplayernum].gold = player_list[trueplayernum].gold + merchantlist[merchantcurrent].price;
									// take away the weapon
									currentmodellist = oldcount;

									if (merchantlist[currentmodellist].object == 1) {
										currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
										currentskinid = FindGunTexture(merchantlist[currentmodellist].Text1);
									} else {
										currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
										currentskinid = model_list[currentmodelid].modeltexture;
									}

									for (q = 0; q <= num_your_guns; q++) {
										char junk[255];
										strcpy_s(junk, merchantlist[merchantcurrent].Text1);
										if (your_gun[q].model_id == FindModelID(junk) ||
										    strstr(your_gun[q].gunname, junk) != NULL &&
										        strstr(your_gun[q].gunname, "SCROLL") != NULL) {
											your_gun[q].active = 0;
											if (strstr(your_gun[q].gunname, "SCROLL") != NULL) {
												your_gun[q].x_offset--;
												if (your_gun[q].x_offset <= 0) {
													your_gun[q].x_offset = 0;
													your_gun[q].active = 0;
												} else {
													your_gun[q].active = 1;
												}
											}
										}
									}
									// load your stuff
									if (!pCWorld->LoadMerchantFiles(m_hWnd, "merchantsell0.txt")) {
									}
									oldcount = merchantlistcount;
									price = 0;

									// append your stuff to end to get price
									for (q = 0; q <= num_your_guns; q++) {
										if (your_gun[q].active == 1) {
											strcpy_s(junk, your_gun[q].gunname);
											// find price
											for (k = 0; k < oldcount; k++) {
												if (strcmp(merchantlist[k].Text1, junk) == 0) {
													price = merchantlist[k].price;
												}
											}
											merchantlist[merchantlistcount].object = 1;
											merchantlist[merchantlistcount].price = price;
											merchantlist[merchantlistcount].qty = 1;

											strcpy_s(merchantlist[merchantlistcount].Text1, junk);
											strcpy_s(merchantlist[merchantlistcount].Text2, junk);

											merchantlist[merchantlistcount].active = 1;
											merchantlist[merchantlistcount].show = 1;
											merchantlistcount++;
										}
									}
									currentmodellist = oldcount;
									if (merchantlist[currentmodellist].object == 1) {
										currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
										currentskinid = FindGunTexture(merchantlist[currentmodellist].Text1);
									} else {
										currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
										currentskinid = model_list[currentmodelid].modeltexture;
									}

									merchantcurrent = 0;
								}
							}
						}
					} else if (buttonselect == 4) {
						if (merchantmode == 0) {
							// load your stuff
							if (!pCWorld->LoadMerchantFiles(m_hWnd, "merchantsell0.txt")) {
							}
							oldcount = merchantlistcount;
							price = 0;

							merchantcurrent = 0;
							// append your stuff to end to get price
							for (q = 0; q <= num_your_guns; q++) {
								if (your_gun[q].active == 1) {
									strcpy_s(junk, your_gun[q].gunname);
									// find price
									for (k = 0; k < oldcount; k++) {

										if (strcmp(merchantlist[k].Text1, junk) == 0) {
											price = merchantlist[k].price;
										}
									}
									merchantlist[merchantlistcount].object = 1;
									merchantlist[merchantlistcount].price = price;
									merchantlist[merchantlistcount].qty = 1;

									strcpy_s(merchantlist[merchantlistcount].Text1, junk);
									strcpy_s(merchantlist[merchantlistcount].Text2, junk);

									merchantlist[merchantlistcount].active = 1;
									merchantlist[merchantlistcount].show = 1;
									merchantlistcount++;
								}
							}
							currentmodellist = oldcount;
							if (merchantlist[currentmodellist].object == 1) {
								currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
								currentskinid = FindGunTexture(merchantlist[currentmodellist].Text1);
							} else {
								currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
								currentskinid = model_list[currentmodelid].modeltexture;
							}

							merchantmode = 1;
						} else if (merchantmode == 1) {
							// load merchant files

							merchantcurrent = 0;
							if (!pCWorld->LoadMerchantFiles(m_hWnd, "merchantbuy0.txt")) {
							}
							currentmodellist = 0;

							price = 99999;
							if (merchantlist[currentmodellist].object == 1) {
								currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
								currentskinid = FindGunTexture(merchantlist[currentmodellist].Text1);
							} else {
								currentmodelid = FindModelID(merchantlist[currentmodellist].Text1);
								currentskinid = model_list[currentmodelid].modeltexture;
							}

							merchantmode = 0;
						}
					} else if (buttonselect == 5) {
						// Hack to fix mouse click problem
						Sleep(200);
						angy = merchantangy;
						m_vEyePt = saveplocation;
						m_vLookatPt = saveplocation;
						modellocation = saveplocation;
						currentmodelid = merchantmodelid;
						currentskinid = merchanttextureid;
						m_vEyePt = merchanteye;
						m_vLookatPt = merchantLook;
						perspectiveview = 1;
						currentmodellist = 0;
						openingscreen = 0;

						m_vEyePt.y = m_vEyePt.y + 100.0f;
					}
				} else {

					if (buttonselect == 1)
						NewGame();
					else if (buttonselect == 2)
						LoadGame();
					else if (buttonselect == 3)
						ChangeVideo();
					else if (buttonselect == 4)
						GameControls();
					else if (buttonselect == 5)
						ExitGame();

					/*
					else if (buttonselect == 5)
					    PlayOnline();
					else if (buttonselect == 6)
					    Register();
					else if (buttonselect == 7)
					    ExitGame();
					*/
				}
			}
		}
		i += 4;
	}
	if (buttonselectlast != buttonselect)
		PlayWavSound(SoundID("click"), 100);

	buttonselectlast = buttonselect;

	return 1;
}

void CMyD3DApplication::CopySurface(LPDIRECTDRAWSURFACE7 pDDSurface, int quitemode)

{

	/*

	HDC hdc, hmemdc;

	HBITMAP hbitmap, hprevbitmap;

	DDSURFACEDESC2 ddsd;

	pDDSurface->GetDC(&hdc);

	hmemdc = CreateCompatibleDC(hdc);

	ZeroMemory(&ddsd, sizeof(ddsd)); // better to clear before using

	ddsd.dwSize = sizeof(ddsd); //initialize with size

	pDDSurface->GetSurfaceDesc(&ddsd);

	hbitmap = CreateCompatibleBitmap(hdc, ddsd.dwWidth, ddsd.dwHeight);

	hprevbitmap = (HBITMAP)SelectObject(hmemdc, hbitmap);

	BitBlt(hmemdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdc, 0, 0, SRCCOPY);

	//hWnd = Handle to App Window

	if (OpenClipboard(main_window_handle))

	{

	    EmptyClipboard();

	    SetClipboardData(CF_BITMAP, hbitmap);

	    CloseClipboard();
	}

	else
	{

	    Pause(TRUE);
	    MessageBox(main_window_handle, "Clipboard not opened", "Error", MB_OK);
	    Pause(FALSE);
	}

	char buf[100];
	time_t current_time = time(NULL);
	struct tm* t = localtime(&current_time);

	//    if (strftime (buf, 100, "%a%b%d%H:%M:%S%Y", t) <= 0)

	if (strftime(buf, 100, "%b-%d-%Y-%H-%M-%S.BMP", t) <= 0)
	    strcpy_s(buf, "SCREENSHOT");

	//	sprintf_s(buf,"%f",(int)fTimeKeysave);

	if (quitemode == 0)
	{
	    sprintf_s(gActionMessage, "Wrote screenshot %s", buf);
	    UpdateScrollList(0, 245, 255);
	}

	SaveBitmap(buf, hbitmap);

	SelectObject(hmemdc, hprevbitmap); // restore the old bitmap

	DeleteDC(hmemdc);

	pDDSurface->ReleaseDC(hdc);

	*/

	return;
}

void CMyD3DApplication::SaveBitmap(char *szFilename, HBITMAP hBitmap)

{

	HDC hdc = NULL;

	FILE *fp = NULL;

	LPVOID pBuf = NULL;

	BITMAPINFO bmpInfo;

	BITMAPFILEHEADER bmpFileHeader;

	BITMAPFILEHEADER bmiHeader;
	do {

		hdc = GetDC(NULL);

		ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));

		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

		if (bmpInfo.bmiHeader.biSizeImage <= 0)
			bmiHeader.bfSize = bmpInfo.bmiHeader.biWidth * abs(bmpInfo.bmiHeader.biHeight) * (bmpInfo.bmiHeader.biBitCount + 7) / 8;
		if ((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage)) == NULL)

		{

			MessageBox(NULL, "Unable to Allocate Bitmap Memory", "Error", MB_OK | MB_ICONERROR);

			break;
		}

		bmpInfo.bmiHeader.biCompression = BI_RGB;

		GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, pBuf, &bmpInfo, DIB_RGB_COLORS);

		if (fopen_s(&fp, szFilename, "wb") != 0) {
			MessageBox(NULL, "Unable to Create Bitmap File", "Error", MB_OK | MB_ICONERROR);
			break;
		}

		bmpFileHeader.bfReserved1 = 0;

		bmpFileHeader.bfReserved2 = 0;

		bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfo.bmiHeader.biSizeImage;

		bmpFileHeader.bfType = 'MB';

		bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);

		fwrite(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);

		fwrite(pBuf, bmpInfo.bmiHeader.biSizeImage, 1, fp);

	} while (false);

	if (hdc)
		ReleaseDC(NULL, hdc);

	if (pBuf)
		free(pBuf);

	if (fp)
		fclose(fp);
}

void CMyD3DApplication::merchantprevitem() {

	currentmodellist--;

	if (merchantmode == 1) {
		if (currentmodellist < oldcount) {
			currentmodellist = merchantlistcount - 1;
		}
	} else {
		if (currentmodellist < 0) {
			currentmodellist = merchantlistcount - 1;
		}
	}
	int a = FindModelID(merchantlist[currentmodellist].Text1);
	int type = merchantlist[currentmodellist].object;

	merchantcurrent = currentmodellist;

	if (type == 1) {
		currentmodelid = a;
		currentskinid = FindGunTexture(merchantlist[currentmodellist].Text1);
	} else {

		currentmodelid = a;
		currentskinid = model_list[a].modeltexture;
	}
}

void CMyD3DApplication::merchantnextitem() {

	currentmodellist++;

	if (currentmodellist > merchantlistcount - 1) {
		currentmodellist = 0;
		if (merchantmode == 1) {
			// go to your items at end
			currentmodellist = oldcount;
		}
	}

	int a = FindModelID(merchantlist[currentmodellist].Text1);
	int type = merchantlist[currentmodellist].object;

	merchantcurrent = currentmodellist;

	if (type == 1) {
		currentmodelid = a;
		currentskinid = FindGunTexture(merchantlist[currentmodellist].Text1);
	} else {

		currentmodelid = a;
		currentskinid = model_list[a].modeltexture;
	}
}

float CMyD3DApplication::RoundFloat(float x) {
	// if there are floating point errors the textures will look squished
	int xx = (int)x;
	x = (float)xx;
	return x;
}
