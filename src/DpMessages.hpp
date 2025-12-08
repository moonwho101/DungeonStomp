
#ifndef __DP_MESSAGES_H__
#define __DP_MESSAGES_H__

#include <dplobby.h>
#include <dplay.h>

#include "D3DApp.h"

const DWORD MAXPLAYERS = 10; // max no. players in the session

const DWORD APPMSG_CHATSTRING = 10;
const DWORD APPMSG_POSITION = 1;
const DWORD APPMSG_FIRE = 2;
const DWORD APPMSG_WALK_OR_DRIVE = 3;
const DWORD APPMSG_GESTURE = 4;
const DWORD APPMSG_JUMP = 5;
const DWORD APPMSG_CROUCH = 6;
const DWORD APPMSG_CHANGE_WEAPON_TO = 7;
const DWORD APPMSG_HIT = 8;
const DWORD APPMSG_PLAYER_LIST = 9;
const DWORD APPMSG_PING_TO_CLIENT = 11;
const DWORD APPMSG_PING_TO_SERVER = 12;
const DWORD APPMSG_PING_TO_ALL = 13;
const DWORD APPMSG_RESPAWN = 14;
const DWORD APPMSG_SEQUENCE = 15;
const DWORD APPMSG_FIREWEAPON = 16;
const DWORD APPMSG_PTYPE = 17;
const DWORD APPMSG_GUN = 18;
const DWORD APPMSG_DOOR = 19;
const DWORD APPMSG_PSTAT1 = 20;
const DWORD APPMSG_PSTAT2 = 21;
const DWORD APPMSG_PDAMAGE = 22;
const DWORD APPMSG_PLAYER2 = 23;
const DWORD APPMSG_ITEM = 24;
const DWORD APPMSG_TREASURE = 25;
const DWORD APPMSG_POSITIONMONSTER = 26;
const DWORD APPMSG_SEQUENCEMONSTER = 27;
const DWORD APPMSG_LOADLEVEL = 28;
const DWORD APPMSG_SWITCH = 29;
typedef struct
{
	DWORD dwType;  // message type (APPMSG_CHATSTRING)
	char szMsg[1]; // message string (variable length)
} MSG_CHATSTRING, *LPMSG_CHATSTRING;

typedef struct
{
	DWORD dwType;
	BYTE player_number;
	BYTE num_players;
	DWORD RRnetID;
	int model_id;
	int skinid;
	char name[1];

} MSG_PLAYER_INFO, *LPMSG_PLAYER_INFO;

typedef struct
{
	DWORD dwType;
	int seq; // current player seq;
	int id;
} MSG_SEQ, *LPMSG_SEQ;

typedef struct
{
	DWORD dwType;
	BOOL IsRunning;
	float x_pos;
	float y_pos;
	float z_pos;
	//	int seq;  //current player seq;
	WORD view_angle;
	int id;
	int alive;
	int hp;
	int health;
	int hd;
	// DWORD RRnetID;

} MSG_POS, *LPMSG_POS;

typedef struct
{
	DWORD dwType;
	int gunid;

} MSG_GUN, *LPMSG_GUN;

typedef struct
{
	DWORD dwType;
	int doorid;
	int open;
	float saveangle;
	float angle;
} MSG_DOOR, *LPMSG_DOOR;

typedef struct
{
	DWORD dwType;
	int switchid;
	int open;
	float saveangle;
	float angle;
} MSG_SWITCH, *LPMSG_SWITCH;

typedef struct
{
	DWORD dwType;
	int item;
	int itemlistcount;
} MSG_ITEM, *LPMSG_ITEM;

typedef struct
{
	DWORD dwType;
	int level;
	char name[80];
} MSG_LOADLEVEL, *LPMSG_LOADLEVEL;

typedef struct
{
	DWORD dwType;
	float x;
	float y;
	float z;
	int gold;
	int model_id;
	int texture;
	int itemid;
	int itemcount;
	char name[80];

} MSG_TREASURE, *LPMSG_TREASURE;

typedef struct
{
	DWORD dwType;
	int hd;
	int armour;
	int lvl;
	int hp;
	int health;
	int xp;
	int dead;
	int frags;

} MSG_PSTAT1, *LPMSG_PSTAT1;

typedef struct
{
	DWORD dwType;
	int player_num;
	int damage;
} MSG_PDAMAGE, *LPMSG_PDAMAGE;

typedef struct
{
	DWORD dwType;
	int model_id;
	int skinid;

} MSG_PTYPE, *LPMSG_PTYPE;

typedef struct
{
	DWORD dwType;
	int model_id;
	int playernum;

} MSG_PLAYER2, *LPMSG_PLAYER2;

typedef struct
{
	DWORD dwType;
	float x_pos;
	float y_pos;
	float z_pos;
	int owner;
	int gunid;
	float gun_angle;
	int angle;
	D3DVECTOR velocity;
	float lookangy;
	int model_id;
	int skin_tex_id;
	int dmg;

	//	int seq;  //current player seq;
	//	WORD	view_angle;
	// DWORD RRnetID;

} MSG_FIRE, *LPMSG_FIRE;

typedef struct
{
	DWORD dwType;
	WORD action_word;
	DWORD RRnetID;

} MSG_ACTIONWORDINFO, *LPMSG_ACTIONWORDINFO;

typedef struct
{
	DWORD dwType;
	DWORD action_dword;
	DWORD RRnetID;

} MSG_ACTIONDWORDINFO, *LPMSG_ACTIONDWORDINFO;

typedef struct
{
	DWORD dwType;
	float action_float;
	DWORD RRnetID;

} MSG_ACTIONFLOATINFO, *LPMSG_ACTIONFLOATINFO;

typedef struct
{
	DWORD dwType;
	BOOL action_flag;
	DWORD RRnetID;

} MSG_ACTIONFLAGINFO, *LPMSG_ACTIONFLAGINFO;

typedef struct
{
	DWORD dwType;
	DWORD RRnetID;

} MSG_ACTIONNOINFO, *LPMSG_ACTIONNOINFO;

struct GAMEMSG_GENERIC {
	DWORD dwType;
};
HRESULT SendTreasure(DWORD RRnetID, float x, float y, float z, int gold, int modelid, int texture, int itemid, int itemcount, char name[80]);
HRESULT SendPlayer2(DWORD RRnetID, int modelid, int playernum);
HRESULT SendItemInfo(DWORD RRnetID, int item, int itemlistcount);
HRESULT SendSequenceMessageMonster(DWORD RRnetID, int seq, int id);
HRESULT HandleAppMessages(HWND hDlg, GAMEMSG_GENERIC *pMsg, DWORD dwMsgSize,
                          DPID idFrom, DPID idTo);

HRESULT SendLoadLevel(DWORD RRnetID, int level, char levelname[80]);
HRESULT HandleSystemMessages(HWND hDlg, DPMSG_GENERIC *pMsg, DWORD dwMsgSize,
                             DPID idFrom, DPID idTo);

HRESULT SendPositionMessage(DWORD RRnetID, float x, float y, float z, float view_angle, BOOL IsRunning);
HRESULT SendGunInfo(DWORD RRnetID, int gunid);
HRESULT SendDoorInfo(DWORD RRnetID, int doorid, int open, float saveangle, float angle);

HRESULT SendSwitchInfo(DWORD RRnetID, int switchid, int open, float saveangle, float angle);
HRESULT SendPlayerStat1(DWORD RRnetID, int player_num);
HRESULT SendSequenceMessage(DWORD RRnetID, int seq);
HRESULT SendFireMessage(float x, float y, float z, float angle, DWORD owner, D3DVECTOR velocity, int lookangy, int gunid, int mowner, int missle);
HRESULT SendPtypeMessage(DWORD RRnetID, int model_id, int skinid);
HRESULT SendPlayerDamage(DWORD RRnetID, int player_num, int damage);

HRESULT SendPositionMessageMonster(DWORD RRnetID, float x, float y, float z, float view_angle, BOOL IsRunning, int id, int alive);

HRESULT SendActionFlagMessage(DWORD RRnetID, BOOL action_flag, DWORD dwType);
HRESULT SendActionWordMessage(DWORD RRnetID, int action_word, DWORD dwType);
HRESULT SendChatMessage(DWORD RRnetID, char *pChatStr);
HRESULT SendPlayerInfoMessage(DWORD player_id, BYTE player_num,
                              char *player_name, BYTE num_players, DWORD dwType, int model_id, int skinid);

HRESULT SendActionDWordMessage(DWORD RRnetID, DWORD action_dword, DWORD dwType, DWORD SendTo);
HRESULT SendActionFloatMessage(DWORD RRnetID, float action_float, DWORD dwType, DWORD SendTo);

VOID DisplayNumberPlayersInGame(HWND hDlg);
void ReadDpInfoFromRegistry();
void WriteRegisteryInfo();
BOOL CALLBACK UpdatePosCallBack(UINT uTimerID,
                                UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

int GetPlayerNumber(DWORD dpid);
void AddDpChatMessageToDisplay(char *msg);
void DisplayIncomingDpChatMessages();

void RrDestroyPlayer();

#endif //__DP_MESSAGES_H__