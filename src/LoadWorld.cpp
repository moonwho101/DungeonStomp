#define DIRECTINPUT_VERSION 0x0700
//-----------------------------------------------------------------------------
// File: LoadWorld.cpp
//
// Desc: Code for loading the DungeonStomp 3D world
//
// Copyright (c) 2001, Mark Longo, All rights reserved
//-----------------------------------------------------------------------------
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

#include "t3dlib3.h"

#define MAX_NUM_OBJECTS_PER_CELL 250
#define MD2_MODEL 0
#define k3DS_MODEL 1

extern int monsterenable;

extern CMD2 *pCMD2;
extern C3DS *pC3DS;
extern CMyD3DApplication *pCMyApp;

int player_count = 0;
int op_gun_count = 0;
int your_gun_count = 0;
int car_count = 0;
int type_num;
int num_imported_models_loaded = 0;
int save_dat_scale;
int mtype;

char g_model_filename[256];

int addanewdoor = 0;
int addanewkey = 0;
int addanewtext = 0;
int addanewstartpos = 0;
int cell_length[2000];
int cell_xlist[2000][20];
int cell_zlist[2000][20];
int cell_list_debug[2000];
int addanewplayer = 0;
float monx, mony, monz;

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
struct startposition {
	float x;
	float y;
	float z;
	float angle;
};

struct gametext {

	int textnum;
	int type;
	char text[2048];
	int shown;
};

struct dialogtext {

	int textnum;
	int type;
	char text[2048];
};

extern struct startposition startpos[200];
extern struct doors door[200];
extern struct gametext gtext[200];
extern int startposcounter;

extern int doorcounter;
extern int textcounter;

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

} MSOUNDLIST, slist[500];
int slistcount = 0;

void InitWorldMap();

CLoadWorld *pCWorld;

CLoadWorld::CLoadWorld() {
	pCWorld = this;
}

BOOL CLoadWorld::LoadWorldMap(HWND hwnd, char *filename) {
	FILE *fp;
	char s[256];
	char p[256];
	char buffer[100];
	int y_count = 30;
	int done = 0;
	int object_count = 0;
	int vert_count = 0;
	int pv_count = 0;
	int poly_count = 0;
	int object_id;
	BOOL lwm_start_flag = TRUE;
	int num_lverts;
	int mem_counter = 0;
	int lit_vert;
	char monsterid[256];
	char monstertexture[256];
	char mnum[256];
	int ability = 0;
	char abil[256];
	char globaltext[2048];
	char bigbuf[2048];
	char bigbuf2[2048];
	int bufc = 0;
	int bufc2 = 0;
	int loop1 = 0;

	doorcounter = 0;
	textcounter = 0;
	startposcounter = 0;
	pCMyApp->countswitches = 0;
	BYTE red, green, blue;

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "Error can't load file ", filename, SCN_AND_FILE);
		MessageBox(hwnd, "Error can't load file", NULL, MB_OK);
		return FALSE;
	}

	PrintMessage(hwnd, "Loading map ", filename, SCN_AND_FILE);
	pCMyApp->num_light_sources_in_map = 0;
	pCMyApp->num_light_sources = 0;
	pCMyApp->outside = 0;
	while (done == 0) {
		fscanf_s(fp, "%s", &s, 256);

		if (strcmp(s, "OBJECT") == 0) {
			fscanf_s(fp, "%s", &p, 256);

			object_id = CheckObjectId(hwnd, (char *)&p);

			if (strstr(p, "door") != NULL) {
				addanewdoor = 1;
			} else if (strstr(p, "text") != NULL) {
				addanewtext = 1;
			} else if (strstr(p, "startpos") != NULL) {
				addanewstartpos = 1;
			} else if (strstr(p, "!") != NULL) {
				if (object_id == 35)
					addanewplayer = 2;
				else
					addanewplayer = 1;
			}

			if (object_id == -1) {
				PrintMessage(hwnd, "Error Bad Object ID in: LoadWorld ", p, SCN_AND_FILE);
				MessageBox(hwnd, "Error Bad Object ID in: LoadWorld", NULL, MB_OK);
				return FALSE;
			}
			if (lwm_start_flag == FALSE)
				object_count++;

			pCMyApp->oblist[object_count].type = object_id;
			strcpy_s(pCMyApp->oblist[object_count].name, p);

			num_lverts = pCMyApp->num_vert_per_object[object_id];
			mem_counter += sizeof(LIGHT) * num_lverts;

			pCMyApp->oblist[object_count].light_source = new LIGHTSOURCE;
			pCMyApp->oblist[object_count].light_source->command = 0;

			lwm_start_flag = FALSE;
		}

		if (strcmp(s, "CO_ORDINATES") == 0) {
			fscanf_s(fp, "%s", &p, 256);
			pCMyApp->oblist[object_count].x = (float)atof(p);

			fscanf_s(fp, "%s", &p, 256);
			pCMyApp->oblist[object_count].y = (float)atof(p) + 28.0f;

			fscanf_s(fp, "%s", &p, 256);
			pCMyApp->oblist[object_count].z = (float)atof(p);

			if (addanewplayer > 0) {

				monx = pCMyApp->oblist[object_count].x;
				mony = pCMyApp->oblist[object_count].y + 44.0f;
				monz = pCMyApp->oblist[object_count].z;
			}

			if (addanewstartpos == 1) {

				startpos[startposcounter].x = pCMyApp->oblist[object_count].x;
				startpos[startposcounter].y = pCMyApp->oblist[object_count].y + 100.0f;
				startpos[startposcounter].z = pCMyApp->oblist[object_count].z;
			}
		}

		if (strcmp(s, "ROT_ANGLE") == 0) {
			float mid;
			float mtex;
			float monnum;

			int truemonsterid = 0;

			if (addanewplayer > 0) {
				fscanf_s(fp, "%s", &p, 256);
				fscanf_s(fp, "%s", &monsterid, 256);
				fscanf_s(fp, "%s", &monstertexture, 256);
				fscanf_s(fp, "%s", &mnum, 256);
				fscanf_s(fp, "%s", &abil, 256);

				truemonsterid = (int)atof(monsterid);

				if (addanewplayer == 2) {
					mid = (float)pCMyApp->FindModelID(monsterid);

					if (strcmp(monstertexture, "0") == 0)
						mtex = 0;
					else if (strcmp(monstertexture, "-1") == 0)
						mtex = -1;
					else
						mtex = (float)pCMyApp->FindTextureAlias(monstertexture);
					monnum = (float)atof(mnum);
				} else {
					mid = (float)atof(monsterid);
					mtex = (float)pCMyApp->FindTextureAlias(monstertexture);
					monnum = (float)atof(mnum);
				}

				if (mtex == 94) {
					// 94-101
					int raction = pCMyApp->random_num(7); // 94 + 7 = 101
					mtex += raction;
				}

				pCMyApp->oblist[object_count].rot_angle = (float)atof(p);
				ability = (int)atof(abil);
				pCMyApp->oblist[object_count].ability = (int)atof(abil);
			} else {

				if (addanewtext == 1) {

					fscanf_s(fp, "%s", &abil, 256);
					fgets(bigbuf, 2048, fp);

					bufc2 = 0;

					for (loop1 = 1; loop1 < (int)strlen(bigbuf); loop1++) {

						if (bigbuf[loop1] != 13 && bigbuf[loop1] != 10)
							bigbuf2[bufc2++] = bigbuf[loop1];
					}

					bigbuf2[bufc2] = '\0';

					strcpy_s(globaltext, bigbuf2);
					if (strstr(bigbuf2, "outside") != NULL) {
						pCMyApp->outside = 1;
					}

					pCMyApp->oblist[object_count].rot_angle = (float)0;
					pCMyApp->oblist[object_count].ability = (int)atof(abil);
				} else {

					if (addanewdoor == 1) {
						fscanf_s(fp, "%s", &p, 256);
						fscanf_s(fp, "%s", &abil, 256);

						pCMyApp->oblist[object_count].rot_angle = (float)atof(p);
						ability = (int)atof(abil);
						pCMyApp->oblist[object_count].ability = (int)atof(abil);
						addanewkey = 0;
					} else if (addanewstartpos == 1) {

						fscanf_s(fp, "%s", &p, 256);
						pCMyApp->oblist[object_count].rot_angle = (float)atof(p);
						startpos[startposcounter].angle = (float)atof(p);
						startposcounter++;
						addanewstartpos = 0;
					}

					else {

						fscanf_s(fp, "%s", &p, 256);
						pCMyApp->oblist[object_count].rot_angle = (float)atof(p);
					}
				}
			}

			if (addanewplayer > 0) {

				if (addanewplayer == 2) {
					if (mtex == 0) {
						pCMyApp->AddModel(monx, mony, monz, (float)atof(p), mid, mtex, monnum, monsterid, monstertexture, ability);
					} else if (mtex == -1)
						pCMyApp->AddItem(monx, mony - 10.0f, monz, (float)atof(p), mid, mtex, monnum, monsterid, monstertexture, ability);
					else if (mtex == -2) {
						pCMyApp->AddItem(monx, mony - 10.0f, monz, (float)atof(p), mid, mtex, monnum, monsterid, monstertexture, ability);
					} else {

						int sc = 0;

						int s1, s2, s3, s4, s5, s6, s7;
						float speed;
						char name[80];
						char dm[80];

						for (sc = 0; sc < slistcount; sc++) {
							if (slist[sc].id == mid) {
								s1 = slist[sc].attack;
								s2 = slist[sc].die;
								s3 = slist[sc].weapon;
								s4 = slist[sc].yell;
								s5 = slist[sc].ac;
								s6 = slist[sc].hd;
								s7 = slist[sc].thaco;
								strcpy_s(name, slist[sc].name);
								speed = slist[sc].speed;
								strcpy_s(dm, slist[sc].damage);
								break;
							}
						}

						pCMyApp->AddMonster(monx, mony, monz, (float)atof(p), mid, mtex, monnum, s1, s2, s3, s4, s5, s6, dm, s7, name, speed, ability);
					}
				}

				addanewplayer = 0;

				pCMyApp->oblist[object_count].monstertexture = (int)mtex;

				pCMyApp->oblist[object_count].monsterid = (int)monnum;
			}

			if (addanewdoor == 1) {

				door[doorcounter].angle = (float)atof(p);
				door[doorcounter].saveangle = (float)atof(p);
				door[doorcounter].doornum = object_count;
				door[doorcounter].key = (int)atof(abil);
				door[doorcounter].open = 0;
				door[doorcounter].swing = 0;
				door[doorcounter].type = 0;
				door[doorcounter].listen = 0;
				door[doorcounter].y = 0;
				door[doorcounter].up = 0;
				addanewdoor = 0;
				doorcounter++;
			}

			if (addanewtext == 1) {
				strcpy_s(gtext[textcounter].text, globaltext);
				gtext[textcounter].textnum = object_count;
				gtext[textcounter].type = (int)atof(abil);
				gtext[textcounter].shown = 0;
				addanewtext = 0;
				textcounter++;
			}
		}

		if (strcmp(s, "LIGHT_ON_VERT") == 0) {
			fscanf_s(fp, "%s", &p, 256);
			lit_vert = atoi(p);

			fscanf_s(fp, "%s", &p, 256);
			red = atoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].r = red;

			fscanf_s(fp, "%s", &p, 256);
			green = atoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].g = green;

			fscanf_s(fp, "%s", &p, 256);
			blue = atoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].b = blue;
		}

		if (strcmp(s, "LIGHT_SOURCE") == 0) {
			fscanf_s(fp, "%s", &p, 256);

			if (strcmp(p, "Spotlight") == 0)
				pCMyApp->oblist[object_count].light_source->command = SPOT_LIGHT_SOURCE;

			if (strcmp(p, "directional") == 0)
				pCMyApp->oblist[object_count].light_source->command = DIRECTIONAL_LIGHT_SOURCE;

			if (strcmp(p, "point") == 0)
				pCMyApp->oblist[object_count].light_source->command = POINT_LIGHT_SOURCE;

			if (strcmp(p, "flicker") == 0)
				pCMyApp->oblist[object_count].light_source->command = 900;

			fscanf_s(fp, "%s", &p, 256);

			if (strcmp(p, "POS") == 0) {

				pCMyApp->oblist[object_count].light_source->flickerrangedir = 0;
				int raction = pCMyApp->random_num(10);
				pCMyApp->oblist[object_count].light_source->flickeratt = (float)raction * 0.1f;
				raction = pCMyApp->random_num(15);
				pCMyApp->oblist[object_count].light_source->flickerrange = (float)raction;
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->position_x = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->position_y = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->position_z = (float)atof(p);
			}

			fscanf_s(fp, "%s", &p, 256);

			if (strcmp(p, "DIR") == 0) {
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->direction_x = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->direction_y = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->direction_z = (float)atof(p);
			}

			fscanf_s(fp, "%s", &p, 256);

			if (strcmp(p, "COLOUR") == 0) {
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->rcolour = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->gcolour = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->oblist[object_count].light_source->bcolour = (float)atof(p);
			}
			pCMyApp->num_light_sources_in_map++;
		}

		if (strcmp(s, "END_FILE") == 0) {
			fscanf_s(fp, "%s", &p, 256);
			pCMyApp->oblist_length = object_count + 1;
			done = 1;
		}
	}
	fclose(fp);

	_itoa_s(pCMyApp->oblist_length, buffer, 100, 10);
	PrintMessage(hwnd, buffer, " map objects loaded (oblist_length)", SCN_AND_FILE);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);

	return TRUE;
}

BOOL CLoadWorld::LoadMod(HWND hwnd, char *filename) {
	FILE *fp;
	char s[256];
	//	char p[256];
	int counter = 0;
	int done = 0;
	char bigbuf[2048];

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "Error can't load file ", filename, SCN_AND_FILE);
		MessageBox(hwnd, "Error can't load file", NULL, MB_OK);
		return FALSE;
	}

	PrintMessage(hwnd, "Loading map ", filename, SCN_AND_FILE);

	while (done == 0) {

		fscanf_s(fp, "%s", &s, 256);
		// objectid

		if (strcmp(s, "END_FILE") == 0) {
			done = 1;
		} else {
			// num
			pCMyApp->levelmodify[counter].num = atoi(s);
			// objectid
			fscanf_s(fp, "%s", &s, 256);
			pCMyApp->levelmodify[counter].objectid = atoi(s);
			// active
			fscanf_s(fp, "%s", &s, 256);
			pCMyApp->levelmodify[counter].active = atoi(s);
			// Function
			fscanf_s(fp, "%s", &s, 256);
			strcpy_s(pCMyApp->levelmodify[counter].Function, s);
			// jump
			fscanf_s(fp, "%s", &s, 256);
			pCMyApp->levelmodify[counter].jump = atoi(s);
			// text1
			fgets(bigbuf, 2048, fp);
			strcpy_s(pCMyApp->levelmodify[counter].Text1, bigbuf);
			// text2
			strcpy_s(pCMyApp->levelmodify[counter].Text2, "");
			pCMyApp->levelmodify[counter].currentheight = -9999;
		}

		counter++;
	}
	fclose(fp);
	pCMyApp->totalmod = counter - 1;
	return TRUE;
}

BOOL CLoadWorld::LoadSoundFiles(HWND hwnd, char *filename) {
	FILE *fp;

	int soundid = 0;

	char name[256];
	char type[256];
	char file[256];

	int y_count = 30;
	int done = 0;
	int object_count = 0;
	int vert_count = 0;
	int pv_count = 0;
	int poly_count = 0;

	BOOL lwm_start_flag = TRUE;

	pCMyApp->nummidi = 0;
	pCMyApp->numsounds = 0;

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "Error can't load file ", filename, SCN_AND_FILE);
		MessageBox(hwnd, "Error can't load file", NULL, MB_OK);
		return FALSE;
	}

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);
	pCMyApp->numsounds = 0;
	pCMyApp->nummidi = 0;

	while (done == 0) {
		fscanf_s(fp, "%s", &type, 256);
		fscanf_s(fp, "%s", &name, 256);
		fscanf_s(fp, "%s", &file, 256);

		if (strcmp(type, "WAV") == 0) {
			strcpy_s(pCMyApp->sound_list[pCMyApp->numsounds].file, file);
			strcpy_s(pCMyApp->sound_list[pCMyApp->numsounds].name, name);

			soundid = DSound_Load_WAV(file);

			if (soundid == -1) {
				MessageBox(hwnd, "sounds.dat wave file not found", "sounds.dat wave file not found", MB_OK);
			}
			pCMyApp->sound_list[pCMyApp->numsounds].id = soundid;
			pCMyApp->sound_list[pCMyApp->numsounds].type = 0;
			pCMyApp->sound_list[pCMyApp->numsounds].playing = 0;

			pCMyApp->numsounds++;
		}
		if (strcmp(type, "MID") == 0) {
			strcpy_s(pCMyApp->midi_list[pCMyApp->nummidi].file, file);
			strcpy_s(pCMyApp->midi_list[pCMyApp->nummidi].name, name);

			soundid = DMusic_Load_MIDI(file);
			if (soundid == -1)
				MessageBox(hwnd, "sounds.dat midi file not found", "sounds.dat midi file not found", MB_OK);

			pCMyApp->midi_list[pCMyApp->nummidi].id = soundid;
			pCMyApp->midi_list[pCMyApp->nummidi].type = 1;
			pCMyApp->sound_list[pCMyApp->numsounds].playing = 0;
			pCMyApp->nummidi++;
		}

		if (strcmp(type, "LOP") == 0) {
			strcpy_s(pCMyApp->sound_list[pCMyApp->numsounds].file, file);
			strcpy_s(pCMyApp->sound_list[pCMyApp->numsounds].name, name);

			soundid = DSound_Load_WAV(file);

			if (soundid == -1) {
				MessageBox(hwnd, "sounds.dat wave file not found", "sounds.dat wave file not found", MB_OK);
			}
			pCMyApp->sound_list[pCMyApp->numsounds].id = soundid;
			pCMyApp->sound_list[pCMyApp->numsounds].type = 2;
			pCMyApp->sound_list[pCMyApp->numsounds].playing = 0;
			pCMyApp->numsounds++;
		}

		if (strcmp(type, "END_FILE") == 0) {

			done = 1;
		}
	}
	fclose(fp);

	return TRUE;
}

BOOL CLoadWorld::LoadMerchantFiles(HWND hwnd, char *filename) {
	FILE *fp;

	int soundid = 0;

	char name[256];
	char type[256];
	char price[256];
	char qty[256];

	int y_count = 30;
	int done = 0;
	int object_count = 0;
	int vert_count = 0;
	int pv_count = 0;
	int poly_count = 0;

	BOOL lwm_start_flag = TRUE;

	int count = 0;

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "Error can't load file ", filename, SCN_AND_FILE);
		MessageBox(hwnd, "Error can't load file", NULL, MB_OK);
		return FALSE;
	}

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);
	pCMyApp->merchantlistcount = 0;
	while (done == 0) {

		fscanf_s(fp, "%s", &name, 256);
		fscanf_s(fp, "%s", &type, 256);
		fscanf_s(fp, "%s", &price, 256);
		fscanf_s(fp, "%s", &qty, 256);

		if (strcmp(name, "END_FILE") == 0) {
			done = 1;
		} else {

			if (strstr(type, "1") != NULL) {
				pCMyApp->merchantlist[count].object = 1;
			} else {
				pCMyApp->merchantlist[count].object = 0;
			}

			pCMyApp->merchantlist[count].price = atoi(price);
			strcpy_s(pCMyApp->merchantlist[count].Text1, name);
			strcpy_s(pCMyApp->merchantlist[count].Text2, name);
			pCMyApp->merchantlist[count].active = 1;
			pCMyApp->merchantlist[count].show = 1;

			pCMyApp->merchantlistcount++;
			count++;
		}
	}
	fclose(fp);

	return TRUE;
}

int CLoadWorld::CheckObjectId(HWND hwnd, char *p) {
	int i;
	char *buffer2;

	for (i = 0; i < pCMyApp->obdata_length; i++) {
		buffer2 = pCMyApp->obdata[i].name;

		if (strcmp(buffer2, p) == 0) {
			return i;
		}
	}
	PrintMessage(hwnd, buffer2, "ERROR bad ID in : CheckObjectId ", SCN_AND_FILE);
	MessageBox(hwnd, buffer2, "Bad ID in :CheckObjectId", MB_OK);

	return -1; // error
}

BOOL CLoadWorld::InitPreCompiledWorldMap(HWND hwnd, char *filename) {
	FILE *fp;
	char s[256];
	int w;
	int done = 0;
	int count;
	int cell_x, cell_z;
	int mem_count;

	PrintMessage(hwnd, "InitPreCompiledWorldMap: starting\n", NULL, LOGFILE_ONLY);

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "Error can't load ", filename, SCN_AND_FILE);
		MessageBox(hwnd, "Error can't load file", NULL, MB_OK);
		return FALSE;
	}

	for (cell_z = 0; cell_z < 200; cell_z++) {
		for (cell_x = 0; cell_x < 200; cell_x++) {
			pCMyApp->cell[cell_x][cell_z] = NULL;
			pCMyApp->cell_length[cell_x][cell_z] = 0;
		}
	}

	mem_count = 0;

	while (done == 0) {
		fscanf_s(fp, "%s", &s, 256);

		if (strcmp(s, "END_FILE") == 0) {
			done = 1;
			fclose(fp);
			PrintMessage(hwnd, "InitPreCompiledWorldMap: sucess\n\n", NULL, LOGFILE_ONLY);
			done = 1;
			return TRUE;
		}

		cell_x = atoi(s);

		if ((cell_x >= 0) && (cell_x <= 200)) {
			fscanf_s(fp, "%s", &s, 256);
			cell_z = atoi(s);

			if ((cell_z >= 0) && (cell_z <= 200)) {
				fscanf_s(fp, "%s", &s, 256);
				count = atoi(s);

				pCMyApp->cell[cell_x][cell_z] = new int[count];
				mem_count = mem_count + (count * sizeof(int));

				for (w = 0; w < count; w++) {
					fscanf_s(fp, "%s", &s, 256);
					pCMyApp->cell[cell_x][cell_z][w] = atoi(s);
				}

				pCMyApp->cell_length[cell_x][cell_z] = count;
			}
		}

	} // end while

	return FALSE;
}

void InitWorldMap() {
}

BOOL CLoadWorld::LoadObjectData(HWND hwnd, char *filename) {
	FILE *fp;
	int i;
	int done = 0;
	int object_id = 0;
	int object_count = 0;
	int old_object_id = 0;
	int vert_count = 0;
	int pv_count = 0;
	int poly_count = 0;
	int texture = 0;
	int conn_cnt = 0;
	int num_v;
	BOOL command_error;
	float dat_scale;
	char buffer[256];
	char s[256];
	char p[256];

	int maxvertcount = 0;

	int vertcountfinal = 0;
	int polycountfinal = 0;

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, "Error can't load file", MB_OK);
		return FALSE;
	}

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);

	while (done == 0) {
		command_error = TRUE;

		fscanf_s(fp, "%s", &s, 256);

		if ((strcmp(s, "OBJECT") == 0) || (strcmp(s, "Q2M_OBJECT") == 0))

		{
			dat_scale = 1.0;

			old_object_id = object_id;

			fscanf_s(fp, "%s", &p, 256); // read object ID
			object_id = atoi(p);

			// find the highest object_id

			if (object_id > object_count)
				object_count = object_id;

			if ((object_id < 0) || (object_id > 399)) {
				MessageBox(hwnd, "Error Bad Object ID in: LoadObjectData", NULL, MB_OK);
				return FALSE;
			}

			pCMyApp->num_vert_per_object[old_object_id] = vert_count;
			pCMyApp->num_polys_per_object[old_object_id] = poly_count;

			vertcountfinal += vert_count;
			polycountfinal += poly_count;

			_itoa_s(vert_count, buffer, _countof(buffer), 10);

			if (vert_count > maxvertcount)
				maxvertcount = vert_count;

			PrintMessage(hwnd, buffer, " vert_count objects", LOGFILE_ONLY);
			_itoa_s(poly_count, buffer, _countof(buffer), 10);
			PrintMessage(hwnd, buffer, " polycount objects", LOGFILE_ONLY);

			PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);

			vert_count = 0;
			poly_count = 0;
			conn_cnt = 0;

			fscanf_s(fp, "%s", &p, 256); // read object name
			PrintMessage(hwnd, p, " vert_count objects", LOGFILE_ONLY);

			strcpy_s((char *)pCMyApp->obdata[object_id].name, 256, (char *)&p);
			command_error = FALSE;
		}

		if (strcmp(s, "SCALE") == 0) {
			fscanf_s(fp, "%s", &p, 256);
			dat_scale = (float)atof(p);
			command_error = FALSE;
		}

		if (strcmp(s, "TEXTURE") == 0) {
			fscanf_s(fp, "%s", &p, 256);

			texture = pCMyApp->CheckValidTextureAlias(pCMyApp->Get_hWnd(), p);

			if (texture == -1) {
				PrintMessage(hwnd, "Error in objects.dat - Bad Texture ID", p, LOGFILE_ONLY);
				MessageBox(hwnd, "Error in objects.dat", "Bad Texture ID", MB_OK);
				fclose(fp);
				return FALSE;
			}
			command_error = FALSE;
		}

		if (strcmp(s, "TYPE") == 0) {
			fscanf_s(fp, "%s", &p, 256);
			command_error = FALSE;
		}

		if (strcmp(s, "TRI") == 0) {
			for (i = 0; i < 3; i++) {
				ReadObDataVert(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 3;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLELIST; // POLY_CMD_TRI;
			poly_count++;
			command_error = FALSE;
		}

		if (strcmp(s, "QUAD") == 0) {
			ReadObDataVert(fp, object_id, vert_count, dat_scale);
			ReadObDataVert(fp, object_id, vert_count + 1, dat_scale);
			ReadObDataVert(fp, object_id, vert_count + 3, dat_scale);
			ReadObDataVert(fp, object_id, vert_count + 2, dat_scale);

			vert_count += 4;

			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 4;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; // POLY_CMD_QUAD;
			poly_count++;
			command_error = FALSE;
		}

		if (strcmp(s, "TRITEX") == 0) {
			for (i = 0; i < 3; i++) {
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].use_texmap[poly_count] = FALSE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 3;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLELIST; // POLY_CMD_TRI_TEX;
			poly_count++;
			command_error = FALSE;
		}

		if (strcmp(s, "QUADTEX") == 0) {
			ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count + 1, dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count + 3, dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count + 2, dat_scale);

			vert_count += 4;

			pCMyApp->obdata[object_id].use_texmap[poly_count] = FALSE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 4;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; // POLY_CMD_QUAD_TEX;
			poly_count++;
			command_error = FALSE;
		}

		if (strcmp(s, "TRI_STRIP") == 0) {
			// Get numbers of verts in triangle strip
			fscanf_s(fp, "%s", &p, 256);
			num_v = atoi(p);

			for (i = 0; i < num_v; i++) {
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = num_v;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; // POLY_CMD_TRI_STRIP;
			poly_count++;
			command_error = FALSE;
		}

		if (strcmp(s, "TRI_FAN") == 0) {
			// Get numbers of verts in triangle fan
			fscanf_s(fp, "%s", &p, 256);
			num_v = atoi(p);

			for (i = 0; i < num_v; i++) {
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = num_v;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLEFAN; // POLY_CMD_TRI_FAN;
			poly_count++;
			command_error = FALSE;
		}

		if (strcmp(s, "CONNECTION") == 0) {
			if (conn_cnt < 4) {
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->obdata[object_id].connection[conn_cnt].x = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->obdata[object_id].connection[conn_cnt].y = (float)atof(p);
				fscanf_s(fp, "%s", &p, 256);
				pCMyApp->obdata[object_id].connection[conn_cnt].z = (float)atof(p);
				conn_cnt++;
			} else {
				fscanf_s(fp, "%s", &p, 256);
				fscanf_s(fp, "%s", &p, 256);
				fscanf_s(fp, "%s", &p, 256);
			}
			command_error = FALSE;
		}

		if (strcmp(s, "END_FILE") == 0) {
			pCMyApp->num_vert_per_object[object_id] = vert_count;
			pCMyApp->num_polys_per_object[object_id] = poly_count;
			pCMyApp->obdata_length = object_count + 1;
			command_error = FALSE;
			done = 1;
		}

		if (command_error == TRUE) {
			_itoa_s(object_id, buffer, _countof(buffer), 10);
			PrintMessage(NULL, "CLoadWorld::LoadObjectData - ERROR in objects.dat, object : ", buffer, LOGFILE_ONLY);
			MessageBox(hwnd, s, "Unrecognised command", MB_OK);
			fclose(fp);
			return FALSE;
		}
	}
	fclose(fp);

	_itoa_s(pCMyApp->obdata_length, buffer, _countof(buffer), 10);

	PrintMessage(hwnd, buffer, " DAT objects loaded", SCN_AND_FILE);

	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);

	_itoa_s(vertcountfinal, buffer, _countof(buffer), 10);
	PrintMessage(hwnd, buffer, " vert_count objects", SCN_AND_FILE);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
	_itoa_s(polycountfinal, buffer, _countof(buffer), 10);
	PrintMessage(hwnd, buffer, " polycount objects", SCN_AND_FILE);

	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);

	//	char buf2[100];

	sprintf_s(buffer, "MAXVERTCOUNT %d", maxvertcount);
	PrintMessage(hwnd, buffer, "", LOGFILE_ONLY);

	for (i = 0; i < pCMyApp->obdata_length; i++) {

		sprintf_s(buffer, "%d %s", i, pCMyApp->obdata[i].name);
		PrintMessage(hwnd, buffer, "", LOGFILE_ONLY);
	}

	return TRUE;
}

BOOL CLoadWorld::ReadObDataVertEx(FILE *fp, int object_id, int vert_count, float dat_scale) {
	float x, y, z;
	char p[256];

	fscanf_s(fp, "%s", &p, 256);
	x = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].x = x;

	fscanf_s(fp, "%s", &p, 256);
	y = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].y = y;

	fscanf_s(fp, "%s", &p, 256);
	z = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].z = z;

	fscanf_s(fp, "%s", &p, 256);
	pCMyApp->obdata[object_id].t[vert_count].x = (float)atof(p);

	fscanf_s(fp, "%s", &p, 256);
	pCMyApp->obdata[object_id].t[vert_count].y = (float)atof(p);

	return TRUE;
}

BOOL CLoadWorld::ReadObDataVert(FILE *fp, int object_id, int vert_count, float dat_scale) {
	float x, y, z;
	char p[256];

	fscanf_s(fp, "%s", &p, 256);
	x = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].x = x;

	fscanf_s(fp, "%s", &p, 256);
	y = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].y = y;

	fscanf_s(fp, "%s", &p, 256);
	z = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].z = z;

	return TRUE;
}

BOOL CLoadWorld::LoadRRTextures(HWND hwnd, char *filename) {
	FILE *fp;
	char s[256];
	char p[256];

	int y_count = 30;
	int done = 0;
	int object_count = 0;
	int vert_count = 0;
	int pv_count = 0;
	int poly_count = 0;
	int tex_alias_counter = 0;
	int tex_counter = 0;
	int i;
	BOOL start_flag = TRUE;
	BOOL found;
	LPDIRECTDRAWSURFACE7 lpDDsurface;

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "ERROR can't open ", filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, "Error can't open", MB_OK);
		return FALSE;
	}

	D3DTextr_InvalidateAllTextures();

	for (i = 0; i < MAX_NUM_TEXTURES; i++)
		pCMyApp->TexMap[i].is_alpha_texture = FALSE;

	for (i = 0; i < MAX_NUM_TEXTURES; i++)
		pCMyApp->lpddsImagePtr[i] = NULL;

	pCMyApp->NumTextures = 0;

	while (done == 0) {
		found = FALSE;
		fscanf_s(fp, "%s", &s, 256);

		if (strcmp(s, "AddTexture") == 0) {
			fscanf_s(fp, "%s", &p, 256);
			PrintMessage(hwnd, "Loading ", p, LOGFILE_ONLY);
			strcpy_s(pCMyApp->ImageFile[tex_counter], p);

			if (strstr(p, "dungeont.bmp") != NULL) {

				int aaa = 1;
			}

			if (D3DTextr_CreateTextureFromFile(p, 0, 0) != S_OK)
				PrintMessage(NULL, "CLoadWorld::LoadRRTextures() - Can't load texture ", p, LOGFILE_ONLY);

			tex_counter++;
			found = TRUE;
		}

		if (strcmp(s, "Alias") == 0) {
			fscanf_s(fp, "%s", &p, 256);

			fscanf_s(fp, "%s", &p, 256);

			strcpy_s((char *)pCMyApp->TexMap[tex_alias_counter].tex_alias_name, 100, (char *)&p);

			pCMyApp->TexMap[tex_alias_counter].texture = tex_counter - 1;

			fscanf_s(fp, "%s", &p, 256);
			if (strcmp(p, "AlphaTransparent") == 0)
				pCMyApp->TexMap[tex_alias_counter].is_alpha_texture = TRUE;

			i = tex_alias_counter;
			fscanf_s(fp, "%s", &p, 256);

			if (strcmp(p, "WHOLE") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}

			if (strcmp(p, "TL_QUAD") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)0.5;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)0.5;
				pCMyApp->TexMap[i].tv[2] = (float)0.5;
				pCMyApp->TexMap[i].tu[3] = (float)0.5;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}

			if (strcmp(p, "TR_QUAD") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.5;
				pCMyApp->TexMap[i].tv[0] = (float)0.5;
				pCMyApp->TexMap[i].tu[1] = (float)0.5;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)0.5;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if (strcmp(p, "LL_QUAD") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.5;
				pCMyApp->TexMap[i].tu[2] = (float)0.5;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)0.5;
				pCMyApp->TexMap[i].tv[3] = (float)0.5;
			}
			if (strcmp(p, "LR_QUAD") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.5;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.5;
				pCMyApp->TexMap[i].tv[1] = (float)0.5;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.5;
			}
			if (strcmp(p, "TOP_HALF") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)0.5;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)0.5;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if (strcmp(p, "BOT_HALF") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.5;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.5;
			}
			if (strcmp(p, "LEFT_HALF") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)0.5;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)0.5;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if (strcmp(p, "RIGHT_HALF") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.5;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.5;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if (strcmp(p, "TL_TRI") == 0) {
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)0.0;
				pCMyApp->TexMap[i].tu[1] = (float)1.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)0.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
			}
			if (strcmp(p, "BR_TRI") == 0) {
			}

			tex_alias_counter++;
			found = TRUE;
		}

		if (strcmp(s, "END_FILE") == 0) {
			PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
			pCMyApp->number_of_tex_aliases = tex_alias_counter;
			pCMyApp->NumTextures = tex_counter;
			found = TRUE;
			done = 1;
		}

		if (found == FALSE) {
			PrintMessage(hwnd, "File Error: Syntax problem :", p, SCN_AND_FILE);
			MessageBox(hwnd, "p", "File Error: Syntax problem ", MB_OK);
			return FALSE;
		}
	}
	fclose(fp);

	D3DTextr_RestoreAllTextures(pCMyApp->GetDevice());
	DDCOLORKEY ckey;
	// set color key to black, for crosshair texture.
	// so any pixels in crosshair texture with color RGB 0,0,0 will be transparent
	ckey.dwColorSpaceLowValue = RGB_MAKE(0, 0, 0);
	ckey.dwColorSpaceHighValue = 0L;

	for (i = 0; i < pCMyApp->NumTextures; i++) {
		lpDDsurface = D3DTextr_GetSurface(pCMyApp->ImageFile[i]);
		pCMyApp->lpddsImagePtr[i] = lpDDsurface;

		if (strstr(pCMyApp->ImageFile[i], "@") != NULL || strstr(pCMyApp->ImageFile[i], "fontA") != NULL || strstr(pCMyApp->ImageFile[i], "die") != NULL || strstr(pCMyApp->ImageFile[i], "dungeont") != NULL || strstr(pCMyApp->ImageFile[i], "button") != NULL || strstr(pCMyApp->ImageFile[i], "lightmap") != NULL || strstr(pCMyApp->ImageFile[i], "flare") != NULL || strstr(pCMyApp->ImageFile[i], "pb8") != NULL || strstr(pCMyApp->ImageFile[i], "pb0") != NULL || strstr(pCMyApp->ImageFile[i], "crosshair") != NULL || strstr(pCMyApp->ImageFile[i], "pbm") != NULL || strstr(pCMyApp->ImageFile[i], "box1") != NULL) {

			if (pCMyApp->lpddsImagePtr[i])
				pCMyApp->lpddsImagePtr[i]->SetColorKey(DDCKEY_SRCBLT, &ckey);
		} else {

			DDCOLORKEY ckeyfix;
			ckeyfix.dwColorSpaceLowValue = RGB_MAKE(9, 99, 99);
			ckeyfix.dwColorSpaceHighValue = 0L;

			if (pCMyApp->lpddsImagePtr[i])
				pCMyApp->lpddsImagePtr[i]->SetColorKey(DDCKEY_SRCBLT, &ckeyfix);
		}
	}
	PrintMessage(hwnd, "CMyD3DApplication::LoadRRTextures - suceeded", NULL, LOGFILE_ONLY);

	return TRUE;
}

BOOL CLoadWorld::LoadImportedModelList(HWND hwnd, char *filename) {
	FILE *fp;
	FILE *fp2;
	char p[256];
	int done = 0;
	//	int i;
	char command[256];
	float f;
	int model_id;
	// char model_name[256];
	char model_filename[256];
	int model_tex_alias;

	float scale;
	int Q2M_Anim_Counter = 0;
	BOOL command_recognised;
	BOOL Model_loaded_flags[1000] = { FALSE };
	slistcount = 0;
	//	pCMyApp->num_players   = 0;
	pCMyApp->num_your_guns = 0;
	pCMyApp->num_op_guns = 0;

	if (fopen_s(&fp, filename, "r") != 0) {
		PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, "Error can't load file", MB_OK);
		return FALSE;
	}

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);

	fscanf_s(fp, "%s", &command, 256);

	if (strcmp(command, "FILENAME") == 0)
		fscanf_s(fp, "%s", &p, 256); // ignore comment
	else
		return FALSE;

	while (done == 0) {
		command_recognised = FALSE;
		scale = (float)0.4;

		fscanf_s(fp, "%s", &command, 256); // get next command

		fscanf_s(fp, "%s", &p, 256); // read player number
		type_num = atoi(p);

		fscanf_s(fp, "%s", &p, 256); // find model file foramt, MD2 or 3DS ?

		if (strcmp(p, "MD2") == 0) {
			mtype = 0;
			pCMyApp->model_format = MD2_MODEL;
		}

		if (strcmp(p, "3DS") == 0) {
			pCMyApp->model_format = k3DS_MODEL;
			mtype = 1;
		}

		fscanf_s(fp, "%s", &p, 256); //  ignore comment "model_id"

		fscanf_s(fp, "%s", &p, 256); // read model id
		model_id = atoi(p);

		fscanf_s(fp, "%s", &model_filename, 256); // read filename for object

		fscanf_s(fp, "%s", &p, 256); //  ignore comment "tex"

		fscanf_s(fp, "%s", &p, 256); // read texture alias id

		model_tex_alias = pCMyApp->FindTextureAlias(p) + 1;

		if (strcmp(command, "PLAYER") == 0) {
			fscanf_s(fp, "%s", &p, 256); // ignore comment pos
			fscanf_s(fp, "%s", &p, 256); // x pos
			fscanf_s(fp, "%s", &p, 256); // y pos
			fscanf_s(fp, "%s", &p, 256); // z pos
			fscanf_s(fp, "%s", &p, 256); // ignore comment angle
			fscanf_s(fp, "%s", &p, 256); // angle
			fscanf_s(fp, "%s", &p, 256); // ignore comment scale
			fscanf_s(fp, "%f", &f);
			scale = f;

			fscanf_s(fp, "%s", &p, 256); // Don't draw players external weapon
			                             //			if(strcmp(p, "NO_EXTERNAL_WEP") == 0)
			                             //				pCMyApp->player_list[type_num].draw_external_wep = FALSE;

			fscanf_s(fp, "%s", &p, 256);

			if (fopen_s(&fp2, p, "r") != 0) {
				PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_FILE);
				MessageBox(hwnd, filename, "Error can't load file", MB_OK);
				return FALSE;
			}

			// load .snd file

			fscanf_s(fp2, "%s", &p, 256);
			slist[slistcount].attack = pCMyApp->SoundID(p);
			fscanf_s(fp2, "%s", &p, 256);
			slist[slistcount].die = pCMyApp->SoundID(p);
			fscanf_s(fp2, "%s", &p, 256);
			slist[slistcount].weapon = pCMyApp->SoundID(p);
			fscanf_s(fp2, "%s", &p, 256);
			slist[slistcount].yell = pCMyApp->SoundID(p);
			player_count++;
			slist[slistcount].id = model_id;

			fclose(fp2);

			// ac
			fscanf_s(fp, "%s", &p, 256);
			fscanf_s(fp, "%s", &p, 256);
			slist[slistcount].ac = atoi(p);
			// hd
			fscanf_s(fp, "%s", &p, 256);
			fscanf_s(fp, "%s", &p, 256);
			slist[slistcount].hd = atoi(p);
			// damage
			fscanf_s(fp, "%s", &p, 256);
			fscanf_s(fp, "%s", &p, 256);
			strcpy_s(slist[slistcount].damage, p);
			// thaco
			fscanf_s(fp, "%s", &p, 256);
			fscanf_s(fp, "%s", &p, 256);
			slist[slistcount].thaco = atoi(p);
			// name
			fscanf_s(fp, "%s", &p, 256); //
			strcpy_s(slist[slistcount].name, p);
			strcpy_s(pCMyApp->model_list[pCMyApp->countmodellist].name, p);
			// speed
			fscanf_s(fp, "%s", &p, 256); //
			slist[slistcount].speed = (float)atof(p);

			fscanf_s(fp, "%s", &p, 256); //
			strcpy_s(pCMyApp->model_list[pCMyApp->countmodellist].monsterweapon, p);

			slistcount++;
			pCMyApp->model_list[pCMyApp->countmodellist].model_id = model_id;
			pCMyApp->model_list[pCMyApp->countmodellist].mtype = mtype;

			if (model_tex_alias == -1)
				pCMyApp->model_list[pCMyApp->countmodellist].modeltexture = model_tex_alias;
			else if (model_tex_alias == -2)
				pCMyApp->model_list[pCMyApp->countmodellist].modeltexture = model_tex_alias;
			else
				pCMyApp->model_list[pCMyApp->countmodellist].modeltexture = model_tex_alias - 1;
			pCMyApp->countmodellist++;

			LoadPlayerAnimationSequenceList(model_id);
			//			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id);
			command_recognised = TRUE;
		}

		if (strcmp(command, "CAR") == 0) {
			/*
			fscanf_s( fp, "%s", &p ); // ignore comment pos

			fscanf_s( fp, "%s", &p ); // x pos
			pCMyApp->car_list[type_num].x = (float)atoi(p);

			fscanf_s( fp, "%s", &p ); // y pos
			pCMyApp->car_list[type_num].y = (float)atoi(p);

			fscanf_s( fp, "%s", &p ); // z pos
			pCMyApp->car_list[type_num].z = (float)atoi(p);

			fscanf_s( fp, "%s", &p ); // ignore comment angle
			fscanf_s( fp, "%s", &p ); // angle
			pCMyApp->car_list[type_num].rot_angle = (float)atoi(p);

			pCMyApp->car_list[type_num].model_id = model_id;
			pCMyApp->car_list[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_cars++;

			fscanf_s( fp, "%s", &p ); // ignore comment scale
			fscanf_s( fp, "%f", &f );
			scale = f;

			car_count++;
			command_recognised = TRUE;
			*/
		}

		if (strcmp(command, "YOURGUN") == 0) {
			fscanf_s(fp, "%s", &p, 256); // ignore comment pos

			fscanf_s(fp, "%s", &p, 256); // x pos
			pCMyApp->your_gun[type_num].x_offset = (float)atoi(p);

			fscanf_s(fp, "%s", &p, 256); // y pos
			pCMyApp->your_gun[type_num].z_offset = (float)atoi(p);

			fscanf_s(fp, "%s", &p, 256); // z pos
			pCMyApp->your_gun[type_num].y_offset = (float)atoi(p);

			fscanf_s(fp, "%s", &p, 256); // ignore comment angle
			fscanf_s(fp, "%s", &p, 256); // angle
			pCMyApp->your_gun[type_num].rot_angle = (float)atoi(p);

			pCMyApp->your_gun[type_num].model_id = model_id;
			pCMyApp->your_gun[type_num].skin_tex_id = model_tex_alias - 1;
			pCMyApp->num_your_guns++;

			fscanf_s(fp, "%s", &p, 256); // ignore comment scale
			fscanf_s(fp, "%f", &f);
			scale = f;

			fscanf_s(fp, "%s", &p, 256); // ignore comment scale
			fscanf_s(fp, "%s", &p, 256); // ignore comment scale

			char d[80];
			char build[80];
			char build2[80];
			strcpy_s(d, p);
			int i = 0;
			int flag = 0, count = 0, count2 = 0;
			for (i = 0; i < (int)strlen(d); i++) {

				if (d[i] == 'd') {
					flag = 1;
				} else {

					if (flag == 0) {
						build[count++] = d[i];
					} else {
						build2[count2++] = d[i];
					}
				}
			}

			build[count] = '\0';
			build2[count2] = '\0';

			pCMyApp->your_gun[type_num].damage1 = atoi(build);
			pCMyApp->your_gun[type_num].damage2 = atoi(build2);
			;

			fscanf_s(fp, "%s", &p, 256); // ignore comment scale
			fscanf_s(fp, "%s", &p, 256); // ignore comment scale

			pCMyApp->your_gun[type_num].sattack = atoi(p);
			;

			fscanf_s(fp, "%s", &p, 256); // ignore comment scale
			fscanf_s(fp, "%s", &p, 256); // ignore comment scale

			pCMyApp->your_gun[type_num].sdamage = atoi(p);
			;

			fscanf_s(fp, "%s", &p, 256); // ignore comment scale
			strcpy_s(pCMyApp->your_gun[type_num].gunname, p);

			fscanf_s(fp, "%s", &p, 256); // ignore comment scale
			pCMyApp->your_gun[type_num].guntype = atoi(p);

			your_gun_count++;
			LoadYourGunAnimationSequenceList(model_id);
			command_recognised = TRUE;
		}

		if (strcmp(command, "DEBUG") == 0) {

			/*
			fscanf_s( fp, "%s", &p ); // ignore comment pos

			fscanf_s( fp, "%s", &p ); // x pos
			pCMyApp->debug[type_num].x = (float)atoi(p);

			fscanf_s( fp, "%s", &p ); // y pos
			pCMyApp->debug[type_num].y = (float)atoi(p);

			fscanf_s( fp, "%s", &p ); // z pos
			pCMyApp->debug[type_num].z = (float)atoi(p);

			fscanf_s( fp, "%s", &p ); // ignore comment angle
			fscanf_s( fp, "%s", &p ); // angle
			pCMyApp->debug[type_num].rot_angle = (float)atoi(p);

			pCMyApp->debug[type_num].model_id = model_id;
			pCMyApp->debug[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_debug_models++;

			fscanf_s( fp, "%s", &p ); // ignore comment scale
			fscanf_s( fp, "%f", &f );
			scale = f;

			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id);

			command_recognised = TRUE;
			*/
		}

		if (strcmp(command, "OPGUN") == 0) {
			/*
			pCMyApp->other_players_guns[type_num].model_id = model_id;
			pCMyApp->other_players_guns[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_op_guns++;

			fscanf_s( fp, "%s", &p ); // ignore comment scale
			fscanf_s( fp, "%f", &f );
			scale = f;

			op_gun_count++;
			command_recognised = TRUE;
			*/
		}

		if (strcmp(command, "END_FILE") == 0) {
			done = 1;
			command_recognised = TRUE;
		}

		if (command_recognised == TRUE) {
			if (Model_loaded_flags[model_id] == FALSE) {
				PrintMessage(hwnd, "loading  ", model_filename, LOGFILE_ONLY);

				if (pCMyApp->model_format == MD2_MODEL)
					pCMD2->ImportMD2_GLCMD(hwnd, model_filename, model_tex_alias, model_id, scale);

				if (pCMyApp->model_format == k3DS_MODEL)
					pC3DS->Import3DS(hwnd, model_filename, model_id, scale);

				Model_loaded_flags[model_id] = TRUE;
				Q2M_Anim_Counter++;
			}
		} else {
			PrintMessage(hwnd, "command unrecognised ", command, SCN_AND_FILE);
			MessageBox(hwnd, command, "command unrecognised", MB_OK);
			return FALSE;
		}

	} // end while loop

	num_imported_models_loaded = player_count + your_gun_count + op_gun_count;
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
	fclose(fp);
	return TRUE;
}

void CLoadWorld::LoadDebugAnimationSequenceList(HWND hwnd, char *filename, int model_id) {
	FILE *fp;
	int done = 0;
	int start_frame;
	int stop_frame;
	int sequence_number;
	int file_ex_start;
	int i;
	char command[256];
	char p[256];
	char name[256];

	BOOL command_recognised;

	strcpy_s(g_model_filename, filename);

	for (i = 0; i < 255; i++) {
		if (g_model_filename[i] == '.') {
			if (g_model_filename[i + 1] == '.') {
				i++;
			} else {
				file_ex_start = i;
				break;
			}
		}
	}

	strcpy_s(&g_model_filename[file_ex_start + 1], 256, "seq");

	if (fopen_s(&fp, g_model_filename, "r") != 0) {
		PrintMessage(hwnd, "ERROR can't load ", g_model_filename, SCN_AND_FILE);
		MessageBox(hwnd, g_model_filename, "Error can't load file", MB_OK);
		return;
	}

	PrintMessage(hwnd, "Loading ", g_model_filename, LOGFILE_ONLY);

	fscanf_s(fp, "%s", &command, 256);

	if (strcmp(command, "FILENAME") == 0)
		fscanf_s(fp, "%s", &p, 256); // ignore comment
	else
		return;

	// SEQUENCE 0  START_FRAME   0  STOP_FRAME  39 NAME Stand
	// SEQUENCE 1  START_FRAME  40  STOP_FRAME  45 NAME Run
	// SEQUENCE 2  START_FRAME  46  STOP_FRAME  53 NAME Attack

	while (done == 0) {
		command_recognised = FALSE;

		// read sequence number
		fscanf_s(fp, "%s", &command, 256);
		if (_strcmpi(command, "SEQUENCE") == 0) {
			fscanf_s(fp, "%d", &sequence_number);

			// read start frame
			fscanf_s(fp, "%s", &p, 256);
			if (_strcmpi(p, "START_FRAME") != 0)
				return;

			fscanf_s(fp, "%d", &start_frame);
			pCMyApp->pmdata[model_id].sequence_start_frame[sequence_number] = start_frame;

			// read stop frame
			fscanf_s(fp, "%s", &p, 256);
			if (_strcmpi(p, "STOP_FRAME") != 0)
				return;

			fscanf_s(fp, "%d", &stop_frame);
			pCMyApp->pmdata[model_id].sequence_stop_frame[sequence_number] = stop_frame;

			// read name
			fscanf_s(fp, "%s", &p, 256);
			if (_strcmpi(p, "NAME") != 0)
				return;

			fscanf_s(fp, "%s", &name, 256);
		}

		if (_strcmpi(command, "END_FILE") == 0)
			done = 1;

	} // end while loop

	fclose(fp);
}

void CLoadWorld::LoadPlayerAnimationSequenceList(int model_id) {
	int i;

	i = model_id;

	pCMyApp->pmdata[i].sequence_start_frame[0] = 0; // Stand
	pCMyApp->pmdata[i].sequence_stop_frame[0] = 39;

	pCMyApp->pmdata[i].sequence_start_frame[1] = 40; // run
	pCMyApp->pmdata[i].sequence_stop_frame[1] = 45;

	pCMyApp->pmdata[i].sequence_start_frame[2] = 46; // attack
	pCMyApp->pmdata[i].sequence_stop_frame[2] = 53;

	pCMyApp->pmdata[i].sequence_start_frame[3] = 54; // pain1
	pCMyApp->pmdata[i].sequence_stop_frame[3] = 57;

	pCMyApp->pmdata[i].sequence_start_frame[4] = 58; // pain2
	pCMyApp->pmdata[i].sequence_stop_frame[4] = 61;

	pCMyApp->pmdata[i].sequence_start_frame[5] = 62; // pain3
	pCMyApp->pmdata[i].sequence_stop_frame[5] = 65;

	pCMyApp->pmdata[i].sequence_start_frame[6] = 66; // jump
	pCMyApp->pmdata[i].sequence_stop_frame[6] = 71;

	pCMyApp->pmdata[i].sequence_start_frame[7] = 72; // flip
	pCMyApp->pmdata[i].sequence_stop_frame[7] = 83;

	pCMyApp->pmdata[i].sequence_start_frame[8] = 84; // Salute
	pCMyApp->pmdata[i].sequence_stop_frame[8] = 94;

	pCMyApp->pmdata[i].sequence_start_frame[9] = 95; // Taunt
	pCMyApp->pmdata[i].sequence_stop_frame[9] = 111;

	pCMyApp->pmdata[i].sequence_start_frame[10] = 112; // Wave
	pCMyApp->pmdata[i].sequence_stop_frame[10] = 122;

	pCMyApp->pmdata[i].sequence_start_frame[11] = 123; // Point
	pCMyApp->pmdata[i].sequence_stop_frame[11] = 134;

	pCMyApp->pmdata[i].sequence_start_frame[12] = 178; // death
	pCMyApp->pmdata[i].sequence_stop_frame[12] = 183;

	pCMyApp->pmdata[i].sequence_start_frame[13] = 184; // death
	pCMyApp->pmdata[i].sequence_stop_frame[13] = 189;

	pCMyApp->pmdata[i].sequence_start_frame[14] = 190; // death
	pCMyApp->pmdata[i].sequence_stop_frame[14] = 197;

	// 178-183
	// 184-189
	// 190-197
}

void CLoadWorld::LoadYourGunAnimationSequenceList(int model_id) {
	int i;

	i = model_id;

	pCMyApp->pmdata[i].sequence_start_frame[0] = 0; // Stand
	pCMyApp->pmdata[i].sequence_stop_frame[0] = 39;

	pCMyApp->pmdata[i].sequence_start_frame[1] = 40; // run
	pCMyApp->pmdata[i].sequence_stop_frame[1] = 45;

	pCMyApp->pmdata[i].sequence_start_frame[2] = 46; // attack
	pCMyApp->pmdata[i].sequence_stop_frame[2] = 53;

	pCMyApp->pmdata[i].sequence_start_frame[3] = 54; // pain1
	pCMyApp->pmdata[i].sequence_stop_frame[3] = 57;

	pCMyApp->pmdata[i].sequence_start_frame[4] = 58; // pain2
	pCMyApp->pmdata[i].sequence_stop_frame[4] = 61;

	pCMyApp->pmdata[i].sequence_start_frame[5] = 62; // pain3
	pCMyApp->pmdata[i].sequence_stop_frame[5] = 65;

	pCMyApp->pmdata[i].sequence_start_frame[6] = 66; // jump
	pCMyApp->pmdata[i].sequence_stop_frame[6] = 71;

	pCMyApp->pmdata[i].sequence_start_frame[7] = 72; // flip
	pCMyApp->pmdata[i].sequence_stop_frame[7] = 83;

	pCMyApp->pmdata[i].sequence_start_frame[8] = 84; // Salute
	pCMyApp->pmdata[i].sequence_stop_frame[8] = 94;

	pCMyApp->pmdata[i].sequence_start_frame[9] = 95; // Taunt
	pCMyApp->pmdata[i].sequence_stop_frame[9] = 111;

	pCMyApp->pmdata[i].sequence_start_frame[10] = 112; // Wave
	pCMyApp->pmdata[i].sequence_stop_frame[10] = 122;

	pCMyApp->pmdata[i].sequence_start_frame[11] = 123; // Point
	pCMyApp->pmdata[i].sequence_stop_frame[11] = 134;

	pCMyApp->pmdata[i].sequence_start_frame[12] = 178; // death
	pCMyApp->pmdata[i].sequence_stop_frame[12] = 183;

	pCMyApp->pmdata[i].sequence_start_frame[13] = 184; // death
	pCMyApp->pmdata[i].sequence_stop_frame[13] = 189;

	pCMyApp->pmdata[i].sequence_start_frame[14] = 190; // death
	pCMyApp->pmdata[i].sequence_stop_frame[14] = 197;
}
