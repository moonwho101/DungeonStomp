#ifndef __DUNGEONSTOMP_H
#define __DUNGEONSTOMP_H
#define _CRT_NONSTDC_NO_DEPRECATE

// #pragma inline_depth( 255 )
// #pragma inline_recursion( on )
// #pragma auto_inline( on )

#include <dinput.h>
#include "D3DFrame.h"
#include "D3DEnum.h"
#include "D3DUtil.h"
#include "D3DRes.h"
#include "world.hpp"
#include "controls.hpp"
#include "GlobalSettings.hpp"

#define RRnetID_ERROR -1

#define USE_DEFAULT_MODEL_TEX 0
#define USE_PLAYERS_SKIN 1

#define USE_INDEXED_DP 0
#define USE_NON_INDEXED_DP 1

#define KEY_DELAY_TIMER 1
#define ANIMATION_TIMER 2
#define RESPAWN_TIMER 3
#define DP_UPDATE_POSITION_TIMER 5
#define GUN_TIMER 6

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------

class CMyD3DApplication : public CD3DApplication {

	D3DLIGHTTYPE m_dltType;
	VOID Cleanup3DEnvironment();

  protected:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT Render();
	void SetTextureStage();
	void DisplayPlayerHUD();
	void DrawFlare();
	void ShowScreenOverlays(LPDIRECTDRAWSURFACE7 &lpDDsurface);
	void DrawCrossHair(LPDIRECTDRAWSURFACE7 &lpDDsurface);
	void DrawPlayerSphere(LPDIRECTDRAWSURFACE7 &lpDDsurface);
	void SetOutside(LPDIRECTDRAWSURFACE7 &lpDDsurface);
	void DrawAlphaIndexed(int i, int &last_texture_number, int j, int &vert_index);
	void DrawAlphaNonIndexed(int i, int &vert_index, int &last_texture_number);
	void SetFlamesMaterial();
	void DrawIndexed(int i, int fakel, int &last_texture_number, int vert_index);
	void DrawNonIndexed(int i, int vert_index, int &last_texture_number);
	void SetTexture(int last_texture_number, const LPDIRECTDRAWSURFACE7 &lpDDsurface);
	void EnableMaterialFlares(int texture_alias_number);
	void DrawMissles();
	void DrawPlayerGun();
	void DrawMonsters();
	void DrawItems();
	void OpenChest();
	void DrawPlayers();
	void MonsterInRange();
	void WakeUpMonsters();
	void DrawMissle();
	void SortLights();
	void DetermineItem(int icnt, int &cell_x, int &cell_z, D3DVIEWPORT7 &vp);
	void ComputeCells(int lookside, int &icnt, int cell_x, int cell_z);
	HRESULT RenderOpeningScreen();
	HRESULT FrameMove(FLOAT fTimeKey);
	HRESULT FrameMove2(FLOAT fTimeKey);
	HRESULT FrameMoveOpeningScreen(FLOAT fTimeKey);
	HRESULT FinalCleanup();

  public:
	CMyD3DApplication();
	HRESULT ChangeVideo();
	BOOL RrBltCrossHair(LPDIRECTDRAWSURFACE7 lpddsTexturePtr);
	BOOL DrawBackground();
	D3DTLVERTEX m_pBackground[4];
	HRESULT PlayOnline();
	HRESULT Register();
	HRESULT ExitGame();
	HRESULT GameControls();
	void InitBackgroundWrap();
	int buttonselectlast;
	RECT rc_dest;
	RECT rc_src;
	int src_backgnd_width;
	int src_backgnd_height;
	int trueplayernum;
	int openingscreen;

	HRESULT FadeOut();
	HRESULT FadeIn();
	static HRESULT ConfirmDevice(DDCAPS *pddDriverCaps, D3DDEVICEDESC7 *pd3dDeviceDesc);

	HRESULT NewGame();
	HRESULT LoadGame();

	int crosshairenabled;
	BOOL perf_flag;     // Timer Selection Flag
	double time_factor; // Time Scaling Factor
	double last_time;

	int initDSTimer();
	VOID UpdateTalk();
	VOID ResetChatString();
	VOID HandleTalkMode(BYTE *diks);
	VOID DriveMode(CONTROLS *Controls);
	VOID WalkMode(CONTROLS *Controls);
	VOID SelectCharacterMode(CONTROLS *Controls);
	VOID MovePlayer(CONTROLS *Controls);
	BOOL m_bBufferPaused;
	BOOL m_bWindowed;
	BOOL m_bToggleFullscreen;
	BOOL m_toggleFog;
	BOOL IsRenderingOk;
	void IsPlayerHit();
	void DisplayScores();
	void DisplayCredits(HWND hwnd);
	void DisplayControls(HWND hwnd);
	void DisplayRRStats(HWND hwnd);
	void DisplayLegalInfo(HWND hwnd);
	void OnCharDown(WPARAM wParam);
	void OnKeyDown(WPARAM wParam);
	void SwitchView();
	void CyclePreviousWeapon();
	void CycleNextWeapon();
	int FindTextureAlias(char *alias);
	VOID OutputText(DWORD x, DWORD y, TCHAR *str);
	HRESULT Render3DEnvironment();
	void SaveBitmap(char *szFilename, HBITMAP hBitmap);

	void CopySurface(LPDIRECTDRAWSURFACE7 pDDSurface, int quietmode);

	void CycleFlare();
	void StartFlare(int type);
	HRESULT menumouseselect();

	HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT pddpf,
	                                            LPVOID lpContext);
	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	                LPARAM lParam);
	// Miscellaneous functions
	VOID ShowStats();
	BOOL m_bShowStats;
	int SceneInBox(D3DVECTOR point);
	VOID UpdateControls();
	BOOL LoadRR_Resources();
	void InitRRvariables();
	HRESULT AnimateCharacters();
	HRESULT AnimateGuns();
	int GetServer();
	HRESULT SetGamma();
	int hitmonster;
	int criticalhiton;
	int spellhiton;

	HRESULT SetGammaFadeOut();
	HRESULT SetGammaFadeIn();
	VOID MouseFilter(float deltaMouseX, float deltaMouseY);
	void smooth_mouse(float time_d, float realx, float realy);
	void ApplyMissleDamage(int playernum);
	LONGLONG DSTimer();
	void SwitchGun(int gun);
	int DSPostQuit();
	int DSLevel();
	void ObjectCollision();
	int LevelUpXPNeeded(int xp);
	int LevelUp(int xp);
	void StartMPGame();
	void CalculateNormals();
	int DisplayDialogText(char *text, float yloc);
	void MakeDamageDice();
	int ResetPlayer();
	int currentinputtype;
	HRESULT SelectInputDevice();

	void AddTreasureDrop(float x, float y, float z, int raction);
	void ResetMainPlayer();
	void ResetDeadPlayer();
	void FirePlayerMissle(float x, float y, float z, float angy, int owner, int shoot, D3DVECTOR velocity, float lookangy);
	void PlayerToD3DVertList(int pmodel_id, int curr_frame, float angle, int texture_alias, int tex_flag, int nextFrame = -1);
	void PlayerToD3DIndexedVertList(int pmodel_id, int curr_frame, float angle, int texture_alias, int tex_flag);
	void ObjectToD3DVertList(int ob_type, float angle, int oblist_index);
	void SetMapLights(int ob_type, float angle, int oblist_index);
	void calculate_block_location();
	void AddPlayer(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, int ability);
	void AddMonster(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, int s1, int s2, int s3, int s4, int ac, int hd, char damage[80], int thaco, char name[80], float speed, int ability);
	void AddItem(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, char modelid[80], char modeltexture[80], int ability);
	void AddModel(float x, float y, float z, float rot_angle, float monsterid, float monstertexture, float monnum, char modelid[80], char modeltexture[80], int ability);
	void UpdateMainPlayer();
	void PlayerHit(int actionnum);
	void MonsterControl();
	void ActivateSwitch();
	int CalculateView(D3DVECTOR EyeBall, D3DVECTOR LookPoint, float angle);
	int CalculateViewMonster(D3DVECTOR EyeBall, D3DVECTOR LookPoint, float angle, float angy);
	int ResetSound();
	void ApplyPlayerDamage(int playerid, int damage);
	void MonsterHit();
	void GiveWeapons();
	void GetItem();
	void PlayerIndexedBox(int pmodel_id, int curr_frame, float angle);
	void PlayerNonIndexedBox(int pmodel_id, int curr_frame, float angle, int monsterid);
	void MakeBoundingBox();
	void PlayWavSound(int id, int volume);
	void LoadSound();
	void SetDiceTexture();
	int MakeDice();
	int random_num(int num);
	void ScanModJump(int jump);
	void CheckMidiMusic();
	int SoundID(char *name);
	void display_font(float x, float y, char text[1000], D3DVIEWPORT7 vp, int r, int g, int b);
	void DisplayPlayerCaption();
	void getaddr();
	D3DVECTOR collideWithWorld(D3DVECTOR position, D3DVECTOR velocity);
	void MoveMonsters();
	void CMyD3DApplication::MoveCamera();
	BOOL LoadRRTextures(HWND hwnd, char *filename);
	int CheckValidTextureAlias(HWND hwnd, char *alias);
	void AddPlayerLightSource(int player_num, float x, float y, float z);
	setupinfo_ptr setupinfo;
	int num_monsters;
	void AddTreasure(float x, float y, float z, int gold);
	int pnpoly(int npol, float *xp, float *yp, float x, float y);
	int collisiondetection(int i);
	int CycleBitMap(int i);
	void calculate_y_location();
	void SetPlayerAnimationSequence(int player_number, int sequence_number);
	int DSOnlineInit();
	int UpdateScrollList(int r, int g, int b);
	int OpenDoor(int doornum, float dist);
	int FreeSlave();
	int save_game(char *filename);
	int load_game(char *filename);
	int load_level(char *filename);
	void RegenrateHP();
	void LoadFile();
	void ScanMod();
	void LoadFileSpot();
	void SaveFileSpot();
	int PostServer();
	bool ReadReg(char key[255], char &result);
	bool WriteReg(char key[255]);
	bool IsShareware();

	void merchantnextitem();
	void merchantprevitem();

	
	int GetNextFrame(int monsterId);
	int GetNextFramePlayer();

	D3DVALUE merchantangy;
	int merchantlistcount;
	int merchantmode;
	int merchantmodelid;
	int merchanttextureid;
	D3DVECTOR merchanteye;
	D3DVECTOR merchantLook;
	int merchantview;

	int SavingThrow(int damage, int player, int level, int missleid, int isplayer, int id);
	void FireMonsterMissle(int monsterid, int type);
	void SetMonsterAnimationSequence(int player_number, int sequence_number);
	HRESULT CreateInputDevice(HWND hWnd,
	                          LPDIRECTINPUT7 pDI,
	                          LPDIRECTINPUTDEVICE2 pDIdDevice,
	                          GUID guidDevice,
	                          const DIDATAFORMAT *pdidDataFormat, DWORD dwFlags);
	void display_message(float x, float y, char text[1000], D3DVIEWPORT7 vp, int r, int g, int b, float fontx, float fonty, int fonttype);
	void display_box(float x, float y, char text[2048], D3DVIEWPORT7 vp, int r, int g, int b, float fontx, float fonty, int fonttype);
	void display_box_calc(float x, float y, char text[2048], D3DVIEWPORT7 vp, int r, int g, int b, float fontx, float fonty, int fonttype);
	int DisplayDamage(float x, float y, float z, int owner, int monsterid, bool cirticalhit);

	int ListenAtDoor();
	float RoundFloat(float x);
	int XpPoints(int hd, int hp);
	void ClearObjectList();
	int thaco(int ac, int thaco);
	void debug_me2(char *u, char *s, int a, int b);
	FLOAT LoadDSini();
	bool ReadDSReg();
	bool WriteDSReg();
	bool SaveDSReg();
	void statusbardisplay(float x, float length, int type);
	void dump2(char *u, char *s, float a, float b, float c, float tu, float tv);
	int FindGunTexture(char *p);
	VOID OpenSoundFile(TCHAR *strFileName, int bufferNum);
	HRESULT PlaySound(int bufferNum, BOOL bLooped);
	HRESULT InitDirectSound(HWND hDlg);
	HRESULT FreeDirectSound();
	VOID LoadWaveFile(TCHAR *strFileName, int bufferNum);
	void SetStartSpot();
	VOID DestroyInputDevice();
	HRESULT CreateDInput(HWND hWnd);
	VOID DestroyDInput();

	LPDIRECTDRAWSURFACE7 lpds_crosshair;
	LPDIRECTDRAWSURFACE7 lpds_background;
	RECT rc_crosshair;
	HRESULT CreateStaticBuffer(TCHAR *strFileName);
	HRESULT FillBuffer(int bufferNum);
	BOOL IsBufferPlaying(int bufferNum);
	HRESULT PlayBuffer(BOOL bLooped, int bufferNum);
	VOID StopBuffer(BOOL bResetPosition, int bufferNum);
	BOOL IsSoundPlaying(int bufferNum);
	HRESULT RestoreBuffers();
	HRESULT SetFrequency(int bufferNum, int frequency);
	int FindModelID(char *p);
	float fadeoutcount;

	int merchantmousebutton;
	int gammafadeout;
	int showchar;
	int showdisplay;
	int showdice;

	int switchcontrols;
	int usespell;
	int buttonselect;
	int hasgamma;
	int outside;
	float k;
	char buffer[256];
	char buffer2[256];
	char buffer3[256];
	char buffer4[256];
	char levelname[80];
	char ipaddress[256];
	char currentdir[255];
	int specularon;
	BOOL DelayKey2[256];
	int DelayKey2Pause[256];
	BOOL RRAppActive;
	BOOL firing_gun_flag;
	BOOL have_i_moved_flag;
	BOOL delay_key_press_flag;
	BOOL multiplay_flag;
	BOOL bInWalkMode;
	BOOL bInTalkMode;
	int num_packets_sent;
	int num_packets_received;
	int num_packets_sent_ps;
	int num_packets_received_ps;
	int m_num_sounds;
	char statusbar[255];
	static HRESULT hr;
	int num_light_sources;
	D3DVALUE angx, angy, angz;
	D3DVALUE look_up_ang;
	D3DVECTOR m_vEyePt;
	D3DVECTOR m_vLookatPt;
	D3DVECTOR m_vUpVec;
	FLOAT m_fEyeDistance; // Offset of each eye from m_vEyePt
	BOOL m_bUseViewPoints;
	int debug_test;
	int keycode;
	int car_gear;
	int editor_insert_mode;
	int MouseX, MouseY;
	int xPos, yPos;
	int current_mode;
	int object_rot_angle;
	int old_xPos, old_yPos;
	int toggle_delay;
	int current_screen_status; // default, in case rrsetup.ini
	int current_level;
	char serveripaddress[255];
	int jump;
	float jumpcount;
	D3DVECTOR jumpv;
	int iswindowed;
	float gammasetting;
	float gravitydropcount;
	D3DVECTOR gravitydrop;

	int mod_id;
	int curr_frame;
	int stop_frame;

	float temp_view_angle;

	int curr_seq;
	char szDevice_debug[256];

	OBJECTLIST *oblist;
	int oblist_length;
	char gActionMessage[2048];
	GUNLIST *your_gun;
	GUNLIST *your_missle;
	GUNLIST *other_players_guns;
	int gunlist_length;
	int current_gun;
	int current_gun_model_id;
	int current_car;
	int num_your_guns;
	int num_op_guns;

	int num_your_missles;
	int pressopendoor;
	int countmodellist;
	int currentmodellist;
	int currentskinid;
	int currentmodelid;

	int currentgunid;
	int currentguntex;
	int debugtimer;
	int debugscreensize;

	PLAYER *item_list;
	MODELLIST *model_list;
	PLAYER *player_list;
	PLAYER *player_list2;
	PLAYER *monster_list;
	PLAYER *car_list;
	PLAYER *debug;
	LEVELMOD *levelmodify;
	SWITCHMOD *switchmodify;
	MERCHANT *merchantlist;
	DSREGISTRY *ds_reg;
	DSONLINE *ds_online;
	DSSERVERINFO *ds_serverinfo;

	int countswitches;

	SOUNDLIST *sound_list;
	DSINI *ds_inilist;

	SOUNDLIST *midi_list;
	int itemlistcount;
	int dsinix;
	int dsiniy;
	int dsiniwindowed;

	FLOAT SaveDSini();

	int turnoffscreentext;
	int src_on[MAX_NUM_VERTICES];
	int src_collide[MAX_NUM_VERTICES];

	int currentmidi;
	int num_players;
	int num_players2;
	int num_cars;
	int num_debug_models;
	int current_frame;
	int current_sequence;

	PLAYERMODELDATA *pmdata;

	OBJECTDATA *obdata;

	char current_levelname[255];
	int *num_vert_per_object;
	int num_polys_per_object[500];
	int obdata_length;
	int model_format;

	int *cell[200][200];
	int **cell_length;

	BOOL **draw_flags;
	BOOL m_tableFog;
	int dsservercount;
	TEXTUREMAPPING TexMap[MAX_NUM_TEXTURES];
	int number_of_tex_aliases;

	int num_polys_per_frame;
	int num_verts_per_frame;

	BOOL walk_mode_enabled;
	BOOL rendering_first_frame;

	D3DVERTEX *src_v;

	LPDIRECT3DMATERIAL3 lpMat[MAX_NUM_TEXTURES];

	D3DMATERIALHANDLE hMat[MAX_NUM_TEXTURES];

	BOOL *dp_command_index_mode;
	D3DPRIMITIVETYPE *dp_commands;

	int *verts_per_poly;
	int *faces_per_poly;
	int *src_f;

	float pi;
	float piover2;

	D3DMATRIX matWorld, matProj;

	D3DMATRIX trans, roty;
	D3DMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	D3DVALUE posx, posy, posz; // Player position
	// D3DVALUE angx, angz; //Player angles
	D3DVECTOR from, at, up; // Used to set view matrix
	D3DVALUE Z_NEAR;
	D3DVALUE Z_FAR;
	D3DVALUE FOV;
	D3DVALUE ASPECT_RATIO;

	int cell_flag;
	int NUM_TRIANGLES;
	int NUM_VERTICES;
	int Num_Quads;
	int Num_Triangles;
	int Num_Vertices;
	int sx, sy, sz;
	int display_poly_flag;
	int clip_display_poly_flag;
	int num_clipped_polys;
	int color;
	int texture_number;
	int RampSize;
	int clength;
	int poly_cnt;
	int cnt_f;

	int cnt;
	float sine, cosine;
	float wx, wy, wz;
	float x_off, y_off, z_off;

	int number_of_polys_per_frame;

	float sin_table[361];
	float cos_table[361];
	float gradient;
	float c;
	float inv_gradient;
	float ncp_angle;

	float px[100], py[100], pz[100], pw[100];
	float mx[100], my[100], mz[100], mw[100];

	float cx[100], cy[100], cz[100], cw[100];
	float tx[100], ty[100];

	int flareon;
	D3DTLVERTEX flare[4];
	float mxc[100], myc[100], mzc[100], mwc[100];

	int mlr[100], mlg[100], mlb[100];

	int *texture_list_buffer;

	char ImageFile[500][256];
	LPDIRECTDRAWSURFACE7 lpddsImagePtr[MAX_NUM_TEXTURES];

	int ctext;
	int NumTextures;

	int *poly_clip_flags;
	int *poly_clip;

	BOOL *oblist_overdraw_flags;
	BOOL *oblist_overlite_flags;

	float player_x, player_z, player_y;
	int numsounds;
	int nummidi;
	int num_light_sources_in_map;
	int num_triangles_in_scene;
	int num_verts_in_scene;
	int num_dp_commands_in_scene;

	int tx_rate_ps;
	int rx_rate_ps;

	DWORD intensity;
	BOOL display_scores;
	BOOL bEnableAlphaTransparency;
	BOOL bEnableLighting;
	D3DSHADEMODE ShadeMode;
	HINSTANCE hInstApp;
	DWORD MyRRnetID;
	DWORD FireWeaponTimer;
	DWORD UpdatePosTimer;
	int MaxTextureSize;
	int MyHealth;
	int MyPing;

	int drawsphere;

	int displaycap;
	int displaychat;

	int forcemainserver;
	int wireframe;
	int solid;
	int point;

	int totalmod;
	int highperftimer;
	int antialias;
};

BOOL CALLBACK FireWeapon(UINT uTimerID,
                         UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

#endif //__DUNGEONSTOMP_H