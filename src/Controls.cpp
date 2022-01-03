#define DIRECTINPUT_VERSION 0x0700

//-----------------------------------------------------------------------------
// File: Controls.cpp
//
// Desc: Code for handling player input
//
// Copyright (c) 2001, Mark Longo, All rights reserved
//-----------------------------------------------------------------------------

#include "prag.h"
#include "dpmessages.hpp"
#include "d3dapp.h"
#include "DungeonStomp.hpp"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <windows.h>
extern CMyD3DApplication* pCMyApp;
#define FORWARD_GEAR 0
#define REVERSE_GEAR 1

#define SND_ENGINE_REV 1

VOID WalkMode(CONTROLS* Controls);
VOID DriveMode(CONTROLS* Controls);
VOID SelectCharacterMode(CONTROLS* Controls);
BOOL set_firing_timer_flag = FALSE;

extern FLOAT elapsegametimersave;
int gear = FORWARD_GEAR;
extern int mousefilter;
int chat_msg_cnt = 0;
int frequency;
float car_speed = 0;
char chat_message[1024];
char dp_chat_msg[1024];
char rr_multiplay_chat_string[1024];
extern int playermove;
extern int playermovestrife;
extern int maingameloopdoor;
extern HMENU gmenuhandle;
extern HMENU gmenuhandle2;
extern int firemissle;

extern FLOAT fLastGunFireTime;
extern int firepause;
extern int trueplayernum;

extern float saveangy;
extern BOOL g_bUseMouse;

extern int pressopendoor;
extern float rotate_camera;
int runflag = 0;
extern int jumpvdir;
extern int nojumpallow;
extern int perspectiveview;
extern int maingameloop;
extern int jumpstart;
extern int betamode;

int delayon = -1;

float historyBufferX[10];
float historyBufferY[10];
int historycount = 0;
float finalX = 0;
float finalY = 0;
float filterx = 0;
float filtery = 0;


struct gametext
{

	int textnum;
	int type;
	char text[2048];
	int shown;
};

extern struct gametext gtext[200];


extern int dialogpause;
extern int dialognum;

struct diceroll
{

	char name[40];
	char monster[50];
	char prefix[40];
	int roll;
	int rollnum;
	int sides;
	int rollmax;

	D3DTLVERTEX dicebox[4];
};

extern struct diceroll dice[50];
extern int numdice;

float mousediv = 5.0f;

FLOAT talkgametimer = 0;
FLOAT talkgametimerlast = 0;

float springiness = 10.0f;
float use_x = 0;
float use_y = 0;

//-----------------------------------------------------------------------------
// Name: MovePlayer
// Desc: This method handles showing the scores for our game.
//		 It then moves the player as long as we are still alive.
//		 sends a DirectPlay message to notify all other players that
//		 we are moving.
//		 Finally, it handles input controls for driving or walking.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::MovePlayer(CONTROLS* Controls)
{
	// Update the variables for the player

	if ((Controls->bScores == TRUE) && (DelayKey2[DIK_S] == FALSE))
	{
		display_scores = !display_scores;
		DelayKey2[DIK_S] = TRUE;
	}

	if (MyHealth > 0)
	{
		if (openingscreen == 1)
		{
			SelectCharacterMode(Controls);
		}
		else
		{
			WalkMode(Controls);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::ResetChatString
// Desc: This method resets the chat count to 0
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::ResetChatString()
{
	chat_msg_cnt = 0;
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::HandleTalkMode
// Desc: This method handles chat messaging for multi-player games.
//		 The user can type anything at the 'say' prompt.
//		 If the user type 'disconnect', they will exit from the gaming
//		 session.  If they type 'kill', they will be re-started at the
//		 starting position.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::HandleTalkMode(BYTE* diks)
{
	int i;
	int player_num;
	char ch;
	BOOL shift = FALSE;
	//	char buildmessage[200];

	int sz = 0;
	int caption = 0;
	char buf[256];
	char buf2[256];
	FILE* fp;

	if (!bInTalkMode)
		return;

	if ((diks[DIK_LSHIFT] && 0x80) || (diks[DIK_RSHIFT] && 0x80))
		shift = TRUE;

	int hitkey = 0;
	for (i = 0; i < 256; i++)
	{

		if ((diks[i] && 0x80))
		{
			hitkey = 1;
		}
		if (i == delayon && maingameloop)
		{

			if (DelayKey2Pause[i] > 0)
			{
				DelayKey2Pause[i]++;
			}

			if (DelayKey2Pause[i] > 15)
			{
				DelayKey2[i] = FALSE;
				delayon = -1;

				if ((diks[i] && 0x80))
				{
					DelayKey2Pause[i] = 99;
				}
				else
				{
					DelayKey2Pause[i] = 0;
				}
			}
		}
	}

	if (hitkey == 0)
		delayon = -1;

	for (i = 0; i < 256; i++)
	{

		if ((diks[i] && 0x80) && (DelayKey2[i] == FALSE) && delayon == -1)
		{
			if ((i <= 58) && (i != DIK_RETURN))
			{
				DelayKey2[i] = TRUE;

				if (DelayKey2Pause[i] == 99)
					DelayKey2Pause[i] = 99;
				else
					DelayKey2Pause[i] = 1;
				if (shift == TRUE)
					ch = dik_to_char_upper_case[i];
				else
					ch = dik_to_char_lower_case[i];

				if ((diks[DIK_BACK] && 0x80) == FALSE)
				{
					if (ch != 0)
					{
						if (chat_msg_cnt < 1000)
						{
							chat_message[chat_msg_cnt] = ch;
							chat_msg_cnt++;
						}
					}
				}
				else
				{
					delayon = i;
					chat_msg_cnt--;
					if (chat_msg_cnt < 0)
						chat_msg_cnt = 0;
				}
			}
		}
	}
	chat_message[chat_msg_cnt] = 0;
	strcpy_s(rr_multiplay_chat_string, "SAY: ");
	strcat_s(rr_multiplay_chat_string, chat_message);

	player_num = trueplayernum;

	sz = strlen(chat_message);

	if (sz == 0)
		return;

	strcpy_s(dp_chat_msg, player_list[player_num].name);
	strcat_s(dp_chat_msg, " : ");
	strcat_s(dp_chat_msg, chat_message);
	caption = 0;

	int secret = 0;

	if ((diks[DIK_RETURN] && 0x80) && (DelayKey2[DIK_RETURN] == FALSE))
	{
		DelayKey2[DIK_RETURN] = TRUE;

		if (chat_message[0] == '-')
		{

			if (strstr(chat_message, "debugt") != NULL)
			{

				if (debugtimer == 1)
					debugtimer = 0;
				else
					debugtimer = 1;
			}

			if (strstr(chat_message, "debugs") != NULL)
			{

				if (debugscreensize == 1)
					debugscreensize = 0;
				else
					debugscreensize = 1;
			}

			if (strstr(chat_message, "bigmoney") != NULL)
			{

				player_list[0].gold = 100000000;
			}

			if (strstr(chat_message, "breeyark") != NULL)
			{

				if (betamode == 0)
				{
					betamode = 1;
					player_list[0].keys = 99;
					SetMenu(m_hWnd, gmenuhandle);
					sprintf_s(gActionMessage, "Debug mode enabled!");
					UpdateScrollList(0, 245, 255);

					secret = 1;
				}
				else
				{

					betamode = 0;

					SetMenu(m_hWnd, gmenuhandle2);
					sprintf_s(gActionMessage, "Debug mode disabled!");
					UpdateScrollList(0, 245, 255);
					secret = 1;
				}
			}

			if (strstr(chat_message, "yyz") != NULL)
			{

				sprintf_s(gActionMessage, "Giving all weapons...");
				UpdateScrollList(0, 245, 255);
				GiveWeapons();
				secret = 1;
			}
			if (strstr(chat_message, "dsload") != NULL)
			{

				char* getnamelen;
				getnamelen = (strstr(chat_message, "dsload"));
				int getnamelenend = (strlen(chat_message));

				int count = 0;
				getnamelen = getnamelen + 7;
				int flag = 0;

				while (flag == 0)
				{

					if (*getnamelen == '\0' || *getnamelen == ' ' || *getnamelen == 13)
						flag = 1;
					else
					{
						buf[count++] = *getnamelen;

						getnamelen++;
					}
				}
				buf[count] = '\0';

				strcpy_s(buf2, buf);
				strcat_s(buf2, ".map");

				if (fopen_s(&fp, buf2, "r") != 0)
				{
				}

				if (fp == NULL)
				{
					strcpy_s(gActionMessage, "File not found");
					UpdateScrollList(0, 245, 255);
					return;
				}
				else
				{

					strcpy_s(levelname, buf);

					fclose(fp);
					look_up_ang = 0.0f;
					rotate_camera = 0;
					m_vEyePt = m_vLookatPt;
					perspectiveview = 1;
					angy = 0.0f;
					ResetMainPlayer();

					char response[10];
					response[0] = chat_message[7];
					response[1] = '\0';
					current_level = atoi(response);

					strcpy_s(levelname, buf);

					load_level(buf);

					SetStartSpot();

					secret = 1;
				}
			}

			if (strstr(chat_message, "load") != NULL)
			{

				char* getnamelen;
				getnamelen = (strstr(chat_message, "load"));
				int getnamelenend = (strlen(chat_message));

				int count = 0;
				getnamelen = getnamelen + 5;
				int flag = 0;

				while (flag == 0)
				{

					if (*getnamelen == '\0' || *getnamelen == ' ' || *getnamelen == 13)
						flag = 1;
					else
					{
						buf[count++] = *getnamelen;

						getnamelen++;
					}
				}
				buf[count] = '\0';

				strcpy_s(buf2, buf);
				strcat_s(buf2, ".sav");

				//fp = fopen(buf2,"r");

				if (fopen_s(&fp, buf2, "r") != 0)
				{
				}

				if (fp == NULL)
				{
					strcpy_s(gActionMessage, "File not found");
					UpdateScrollList(0, 245, 255);
					return;
				}
				else
				{

					load_game(buf2);

					fclose(fp);
				}
			}

			if (strstr(chat_message, "save") != NULL)
			{

				char* getnamelen;
				getnamelen = (strstr(chat_message, "save"));
				int getnamelenend = (strlen(chat_message));

				int count = 0;
				getnamelen = getnamelen + 5;
				int flag = 0;

				while (flag == 0)
				{

					if (*getnamelen == '\0' || *getnamelen == ' ' || *getnamelen == 13)
						flag = 1;
					else
					{
						buf[count++] = *getnamelen;

						getnamelen++;
					}
				}
				buf[count] = '\0';

				strcpy_s(buf2, buf);
				strcat_s(buf2, ".sav");
				save_game(buf2);
			}
		}

		if (caption == 0 && secret == 0)
		{
			strcpy_s(gActionMessage, dp_chat_msg);
			UpdateScrollList(0, 245, 255);
		}
		DelayKey2[DIK_RETURN] = TRUE;
		chat_msg_cnt = 0;
	}
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::WalkMode()
// Desc: This method checks all controls when the user is in 'Walking'
//		 mode and adjusts the associated variables accordingly - e.g.,
//		 switching weapons, turning, moving, etc.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::WalkMode(CONTROLS* Controls)
{
	int i = 0;
	float speed = 8.0f;

	//	float step_left_angy;
	//	float step_right_angy;

	filterx = 0;
	filtery = 0;

	have_i_moved_flag = FALSE;

	playermove = 0;

	FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds

	// Update the frame rate once per second
	if (fTime - fLastGunFireTime > .3f)
	{
		firepause = 0;
	}

	if (Controls->spell && player_list[trueplayernum].firespeed == 0 && usespell == 1)
	{
		int pspeed = (18 - player_list[trueplayernum].hd);
		if (pspeed < 6)
			pspeed = 6;

		player_list[trueplayernum].firespeed = pspeed;
		firemissle = 1;
	}

	if (Controls->bCameraleft)
	{

		//rotate_camera+=5.0f;
		rotate_camera += 105 * elapsegametimersave;
	}

	if (Controls->bCameraright)
	{

		//rotate_camera-=5.0f;
		rotate_camera -= 105 * elapsegametimersave;
	}

	if (Controls->missle && jump == 0 && nojumpallow == 0 && !bInTalkMode)
	{

		jump = 1;
		if (perspectiveview == 1)
			jumpv = D3DVECTOR(0, 30, 0);
		else
			jumpv = D3DVECTOR(0, 30, 0);
		jumpvdir = 0;
		jumpcount = 0;
		SetPlayerAnimationSequence(trueplayernum, 6);
		PlayWavSound(SoundID("jump1"), 80);
		jumpstart = 1;
	}

	// turn left
	if (Controls->bLeft && !bInTalkMode)
	{
		playermove = 2;
		if (g_bUseMouse)
		{
			filterx = (float)Controls->bLeft / (float)mousediv;
		}
		else
		{
			angy -= 105 * elapsegametimersave;
			have_i_moved_flag = TRUE;
		}
	}

	// turn right
	if (Controls->bRight && !bInTalkMode)
	{
		playermove = 3;
		if (g_bUseMouse)
		{
			filterx = (float)Controls->bRight / (float)mousediv;
		}
		else
		{
			angy += 105 * elapsegametimersave;
			have_i_moved_flag = TRUE;
		}
	}
	// move forward
	runflag = 0;

	if ( (Controls->bForward || Controls->bForwardButton) && !bInTalkMode)
	{
		playermove = 1;
		have_i_moved_flag = TRUE;
		runflag = 1;
	}

	// move backward
	if (Controls->bBackward && !bInTalkMode)
	{
		runflag = 1;
		playermove = 4;
		have_i_moved_flag = TRUE;
	}

	// Move vertically up towards sky
	if (Controls->bUp)
		m_vEyePt.y += 2;

	// Move vertically down towards ground
	if (Controls->bDown)
		m_vEyePt.y -= 2;

	// fire gun
	if ((Controls->bFire == TRUE) && (MyHealth > 0 && jump == 0) && (player_list[trueplayernum].current_sequence != 2) && player_list[trueplayernum].bIsPlayerAlive == TRUE && usespell == 0 ||
		(Controls->bFire2 == TRUE) && (MyHealth > 0) && jump == 0 && (player_list[trueplayernum].current_sequence != 2) && player_list[trueplayernum].bIsPlayerAlive == TRUE &&
		usespell == 0)
	{


			hitmonster = 0;
			numdice = 2;
			firing_gun_flag = TRUE;
			SetPlayerAnimationSequence(trueplayernum, 2);
			PlayWavSound(SoundID("knightattack"), 100);


			set_firing_timer_flag = TRUE;
			fLastGunFireTime = timeGetTime() * 0.001f;

			playermove = 5;
			firepause = 1;

			dice[0].roll = 1;
			dice[1].roll = 1;

			criticalhiton = 0;

	}
	else
	{
		firing_gun_flag = FALSE;
	}

	if (Controls->opendoor)
	{
		if (!bInTalkMode)
			pressopendoor = 1;
	}
	else
	{
		pressopendoor = 0;
	}

	// tilt head upwards
	if (Controls->bHeadDown)
	{

		if (g_bUseMouse)
		{
			filtery = (float)Controls->bHeadDown / (float)mousediv;
		}
		else
		{
			//	look_up_ang += 4;
			look_up_ang += 105 * elapsegametimersave;
			if (look_up_ang >= 90)
				look_up_ang = 89;
		}
	}

	// tilt head downward
	if (Controls->bHeadUp)
	{

		if (g_bUseMouse)
		{
			//look_up_ang += (float)Controls->bHeadUp / (float)5;
			filtery = (float)Controls->bHeadUp / (float)mousediv;
		}
		else
		{

			look_up_ang -= 105 * elapsegametimersave;
			//	look_up_ang -= 4;
			if (look_up_ang <= -90)
				look_up_ang = -89;
		}

		//look_up_ang -= 4;
	}

	// side step left

	if (!bInTalkMode)
		if (Controls->bStepLeft)
		{
			playermovestrife = 6;
		}

	// side step right
	if (!bInTalkMode)
		if (Controls->bStepRight)
		{
			playermovestrife = 7;
		}

	if (g_bUseMouse)
	{
		if (mousefilter)
		{
			//MouseFilter(filterx, filtery);
			smooth_mouse(elapsegametimersave, filterx, filtery);
			angy += filterx;
			look_up_ang += filtery;
		}
		else
		{
			angy += filterx;

			look_up_ang += filtery;
		}
		if (look_up_ang <= -90.0f)
			look_up_ang = -89.0f;

		if (look_up_ang >= 90.0f)
			look_up_ang = 89.0f;
	}
}


void CMyD3DApplication::smooth_mouse(float time_d, float realx, float realy) {
	
	double d = 1 - exp(log(0.5) * springiness * time_d);

	use_x += (realx - use_x) * d;
	use_y += (realy - use_y) * d;

	filterx = use_x;
	filtery = use_y;
}




VOID CMyD3DApplication::SelectCharacterMode(CONTROLS* Controls)
{
	int i = 0;
	float speed = 8.0f;

	return;

	// turn left
	if (Controls->bLeft)
	{

		currentmodellist--;

		if (currentmodellist < 0)
			currentmodellist = countmodellist - 1;

		currentmodelid = model_list[currentmodellist].model_id;
		currentskinid = model_list[currentmodellist].modeltexture;
	}

	// turn right
	if (Controls->bRight)
	{

		currentmodellist++;

		if (currentmodellist >= countmodellist)
			currentmodellist = 0;

		currentmodelid = model_list[currentmodellist].model_id;
		currentskinid = model_list[currentmodellist].modeltexture;
	}
	if ((Controls->bFire == TRUE))
	{
	}
}

VOID CMyD3DApplication::MouseFilter(float deltaMouseX, float deltaMouseY)
{

	// Reposition the mouse to the center of our window

	// Shift the buffer around. If you want performance from this, be sure
	// to use a circular buffer than these slow memmove()s.

	memmove(historyBufferX + 1, historyBufferX, sizeof(historyBufferX) - sizeof(float));
	memmove(historyBufferY + 1, historyBufferY, sizeof(historyBufferY) - sizeof(float));

	// Put the current values at the front of the history buffer

	*historyBufferX = deltaMouseX;
	*historyBufferY = deltaMouseY;

	// Filter the mouse
	int historyBufferLength = 10;

	float curAverageX = 0;
	float curAverageY = 0;
	float averageTot = 0;
	float currentWeight = 1.0f;
	float weightModifier = 0.3f;
	for (int i = 0; i < historyBufferLength; ++i)
	{
		curAverageX += historyBufferX[i] * currentWeight;
		curAverageY += historyBufferY[i] * currentWeight;

		// Note! Our total is also weighted

		averageTot += 1.0f * currentWeight;

		// The weight for the next entry in the history buffer

		currentWeight *= weightModifier;
	}

	// Calculate the final weighted value

	finalX = curAverageX / averageTot;
	finalY = curAverageY / averageTot;
}