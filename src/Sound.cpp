#define DIRECTINPUT_VERSION 0x0700

#include "prag.h"
#define STRICT
#define D3D_OVERLOADS
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

#include "t3dlib3.h"

extern float closesoundid[100];
extern int musicon;

int sound_ids[4]; // storage for 4 sound fx ids

int skipmusic = 0;

void CMyD3DApplication::PlayWavSound(int id, int volume) {
	int i = 0;

	DSound_Play(id, 0, volume);
}

void CMyD3DApplication::LoadSound() {
	currentmidi = 0;

	DSound_Init();
	DMusic_Init();
}

void CMyD3DApplication::CheckMidiMusic() {
	int raction;
	static FLOAT fLastTimeMusic = 0;
	int i = 0;

	FLOAT fTime = timeGetTime() * 0.01f; // Get current time in seconds

	if (fTime - fLastTimeMusic > 280.0f && skipmusic == 1) {
		skipmusic = 0;
		fLastTimeMusic = fTime;

		raction = random_num(4);

		if (raction == 1)
			PlayWavSound(SoundID("effect1"), 100);
		else if (raction == 2)
			PlayWavSound(SoundID("effect2"), 100);
		else if (raction == 3)
			PlayWavSound(SoundID("effect3"), 100);
		else
			PlayWavSound(SoundID("effect4"), 100);
	}

	if (DMusic_Status_MIDI(currentmidi) != MIDI_STOPPED && musicon == 0) {

		DMusic_Stop(currentmidi);
	}

	if (DMusic_Status_MIDI(currentmidi) == MIDI_STOPPED && skipmusic == 0 && musicon == 1) {
		raction = random_num(2);

		if (raction == 0) {
			skipmusic = 1;
		} else {
			skipmusic = 0;
			raction = random_num(nummidi);
			DMusic_Play(raction);
			currentmidi = raction;
		}
	}

	if (closesoundid[0] < 9000.0f) {
		for (i = 0; i < numsounds; i++) {
			if (strcmp("water", sound_list[i].name) == 0) {

				if (sound_list[i].playing == 0)
					sound_list[i].playing = 1;

				break;
			}
		}
	}

	if (closesoundid[1] < 9000.0f) {
		for (i = 0; i < numsounds; i++) {
			if (strcmp("fire", sound_list[i].name) == 0) {

				if (sound_list[i].playing == 0)
					sound_list[i].playing = 1;

				break;
			}
		}
	}

	if (closesoundid[2] < 9000.0f) {
		for (i = 0; i < numsounds; i++) {
			if (strcmp("lava", sound_list[i].name) == 0) {

				if (sound_list[i].playing == 0)
					sound_list[i].playing = 1;

				break;
			}
		}
	}

	if (closesoundid[3] < 9000.0f) {
		for (i = 0; i < numsounds; i++) {
			if (strcmp("stone", sound_list[i].name) == 0) {

				if (sound_list[i].playing == 0)
					sound_list[i].playing = 1;

				break;
			}
		}
	}

	if (closesoundid[4] < 9000.0f) {
		for (i = 0; i < numsounds; i++) {
			if (strcmp("teleport", sound_list[i].name) == 0) {

				if (sound_list[i].playing == 0)
					sound_list[i].playing = 1;

				break;
			}
		}
	}

	for (i = 0; i < numsounds; i++) {
		if (sound_list[i].playing > 0) {
			if (sound_list[i].playing == 1) {
				if (strcmp("water", sound_list[i].name) == 0) {
					PlayWavSound(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[0]) / ((400))));
				}
				if (strcmp("fire", sound_list[i].name) == 0) {
					PlayWavSound(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[1]) / ((400))));
				}
				if (strcmp("lava", sound_list[i].name) == 0) {
					PlayWavSound(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[2]) / ((400))));
				}
				if (strcmp("stone", sound_list[i].name) == 0) {
					PlayWavSound(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[3]) / ((1400))));
				}

				if (strcmp("teleport", sound_list[i].name) == 0) {
					PlayWavSound(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[4]) / ((1400))));
				}

				sound_list[i].playing = 2;
			} else if (sound_list[i].playing == 2) {
				if (DSound_Status_Sound(SoundID(sound_list[i].name))) {

					if (strcmp("water", sound_list[i].name) == 0) {
						DSound_Set_Volume(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[0]) / ((400))));
					}
					if (strcmp("fire", sound_list[i].name) == 0) {
						DSound_Set_Volume(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[1]) / ((400))));
					}
					if (strcmp("lava", sound_list[i].name) == 0) {
						DSound_Set_Volume(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[2]) / ((400))));
					}
					if (strcmp("stone", sound_list[i].name) == 0) {
						DSound_Set_Volume(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[3]) / ((1400))));
					}
					if (strcmp("teleport", sound_list[i].name) == 0) {
						DSound_Set_Volume(SoundID(sound_list[i].name), 100 - (int)((100 * closesoundid[4]) / ((1400))));
					}
				} else {
					sound_list[i].playing = 0;
				}
			}
		}
	}
}

int CMyD3DApplication::SoundID(char *name) {
	int i = 0;
	int id = 0;

	for (i = 0; i < numsounds; i++) {

		if (strcmp(name, sound_list[i].name) == 0) {
			id = sound_list[i].id;
			break;
		}
	}
	return id;
}
