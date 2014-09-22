/* ==============================================================
bstone-ios: Blake Stone Planet Strike for iOS

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013 Boris Bendovsky (bibendovsky@hotmail.com)
Copyright (c) 2014 Ignacio Sanchez (ignacio.sanchez@geardome.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the
Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
============================================================== */


#include <map>
#include "3d_def.h"
#include "jm_lzh.h"

#ifdef MSVC
#pragma hdrstop
#endif


void CA_CacheScreen (Sint16 chunk);
void VH_UpdateScreen();
void	DrawHighScores(void);
void ClearMemory (void);
void DrawTopInfo(sp_type type);
void PreloadUpdate(Uint16 current, Uint16 total);
void INL_GetJoyDelta(Uint16 joy,Sint16 *dx,Sint16 *dy);
boolean LoadTheGame(int handle);
boolean IN_CheckAck();

// BBi
bool LoadTheGame(
    bstone::IStream* stream);

// As is, this switch will not work ... the data associated with this
// is not saved out correctly.
//
//#define CACHE_KEY_DATA


//
// End game message
//

char EndGameStr[] = {"    End current game?\n"
								 " Are you sure (Y or N)?"
};


#define ENDGAMESTR	(EndGameStr)

char QuitToDosStr[] = {"      Quit to DOS?\n"
									" Are you sure (Y or N)?"
};

//#define FREEFONT(fontnum)	{MM_SetPurge (&(memptr)grsegs[fontnum],3);}
#define FREEFONT(fontnum) {if (fontnum != STARTFONT+2 && grsegs[fontnum]) UNCACHEGRCHUNK(fontnum);}


boolean EscPressed = false;

Sint16	lastmenumusic;


//===========================================================================
//
// 							    PRIVATE PROTOTYPES
//
//===========================================================================

void CP_ReadThis(Sint16 temp1);
void CP_OrderingInfo(Sint16 temp1);
void DrawEpisodePic(Sint16 w);
void DrawAllSoundLights(Sint16 which);
void ReadGameNames(void);
void FreeMusic(void);
void CP_GameOptions(Sint16 temp1);
void DrawGopMenu(void);
void CalibrateJoystick(void);
void ExitGame(void);
void CP_Switches(Sint16 temp1);
void DrawSwitchMenu(void);
void DrawAllSwitchLights(Sint16 which);
void DrawSwitchDescription(Sint16 which);

// BBi
void cp_sound_volume(
    Sint16);


extern boolean refresh_screen;


//===========================================================================
//
// 							       LOCAL DATA...
//
//===========================================================================

CP_iteminfo
	MainItems=	{MENU_X,MENU_Y,12,MM_READ_THIS,0,9,{77, 1,154,9,1}},
	GopItems=	{MENU_X,MENU_Y+30,4,0,0,9,{77, 1,154,9,1}},
	SndItems=	{SM_X,SM_Y,6,0,0,7,		{87,-1,144,7,1}},
	LSItems=	{LSM_X,LSM_Y,10,0,0,8,	{86,-1,144,8,1}},
	CtlItems=	{CTL_X,CTL_Y,7,-1,0,9,	{87,1,174,9,1}},
	CusItems=	{CST_X,CST_Y+7,6,-1,0,15,{54,-1,203,7,1}},
	NewEitems=	{NE_X,NE_Y,11,0,0,16,	{43,-2,119,16,1}},
	NewItems=	{NM_X,NM_Y,4,1,0,16,		{60,-2,105,16,1}},
	SwitchItems=	{MENU_X,MENU_Y+15,7,0,0,9,{87,-1,132,7,1}};



CP_itemtype MainMenu[]=
{
	{AT_ENABLED,"NEW MISSION",CP_NewGame,COAL_FONT},
	{AT_READIT,"ORDERING INFO",CP_OrderingInfo},
	{AT_READIT,"INSTRUCTIONS",CP_ReadThis},
	{AT_ENABLED,"STORY",CP_BlakeStoneSaga},
	{AT_DISABLED,"",0},
	{AT_ENABLED,"GAME OPTIONS",CP_GameOptions},
	{AT_ENABLED,"HIGH SCORES",CP_ViewScores},
	{AT_ENABLED,"LOAD MISSION",reinterpret_cast<void (*)(Sint16)>(CP_LoadGame)},
	{AT_DISABLED,"SAVE MISSION",reinterpret_cast<void (*)(Sint16)>(CP_SaveGame)},
	{AT_DISABLED,"",0},
	{AT_ENABLED,"BACK TO DEMO",CP_ExitOptions},
	{AT_ENABLED,"LOGOFF",0}
},

GopMenu[]=
{
	{AT_ENABLED,"SOUND",CP_Sound},

    // BBi
    {AT_ENABLED, "SOUND VOLUME", cp_sound_volume},

	{AT_ENABLED,"CONTROLS",CP_Control},
	{AT_ENABLED,"SWITCHES",CP_Switches}
},

SndMenu[]=
{
 {AT_ENABLED,"NONE",0},
 {AT_ENABLED,"ADLIB/SOUND BLASTER",0},
 {AT_DISABLED,"",0},
 {AT_DISABLED,"",0},
 {AT_ENABLED,"NONE",0},
 {AT_ENABLED,"ADLIB/SOUND BLASTER",0}
},

CtlMenu[]=
{
 {AT_DISABLED,"MOUSE ENABLED",0},
 {AT_DISABLED,"JOYSTICK ENABLED",0},
 {AT_DISABLED,"USE JOYSTICK PORT 2",0},
 {AT_DISABLED,"GRAVIS GAMEPAD ENABLED",0},
 {AT_DISABLED,"CALIBRATE JOYSTICK",0},
 {AT_DISABLED,"MOUSE SENSITIVITY",MouseSensitivity},
 {AT_ENABLED,"CUSTOMIZE CONTROLS",CustomControls}
},

SwitchMenu[] = {
     { AT_ENABLED, "LIGHTING", 0 },
     { AT_ENABLED, "REBA ATTACK INFO", 0 },
     { AT_ENABLED, "SHOW CEILINGS", 0 },
     { AT_ENABLED, "SHOW FLOORS", 0 },

     // BBi
     { AT_ENABLED, "NO WALL HIT SOUND", 0 },
     { AT_ENABLED, "MODERN CONTROLS", 0 },
     { AT_ENABLED, "ALWAYS RUN", 0 }
 },


#if 0
NewEmenu[]=
{
 {AT_ENABLED,"MISSION 1:\n"
				 "STAR INSTITUTE",0},

 {AT_NON_SELECTABLE,"MISSION 2:\n"
				  "FLOATING FORTRESS",0},

 {AT_NON_SELECTABLE,"MISSION 3:\n"
				  "UNDERGROUND NETWORK",0},

 {AT_NON_SELECTABLE,"MISSION 4:\n"
				  "STAR PORT",0},

 {AT_NON_SELECTABLE,"MISSION 5:\n"
				  "HABITAT II",0},

 {AT_NON_SELECTABLE,"MISSION 6:\n"
				  "SATELLITE DEFENSE",0},
},
#endif

NewMenu[]=
{
 {AT_ENABLED,"LEVEL 1:\n"
				 "NOVICE AGENT",0},
 {AT_ENABLED,"LEVEL 2:\n"
				 "SKILLED AGENT",0},
 {AT_ENABLED,"LEVEL 3:\n"
				 "EXPERT AGENT",0},
 {AT_ENABLED,"LEVEL 4:\n"
				 "VETERAN AGENT",0}
},

LSMenu[]=
{
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0}
},

CusMenu[]=
{
 {AT_ENABLED,"",0},
 {AT_DISABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_DISABLED,"",0},
 {AT_ENABLED,"",0},
 {AT_ENABLED,"",0}
}
;


Sint16 color_hlite[]=
{
	HIGHLIGHT_DISABLED_COLOR,
	HIGHLIGHT_TEXT_COLOR,
	READHCOLOR,
	HIGHLIGHT_DEACTIAVED_COLOR,
};

Sint16 color_norml[]=
{
	DISABLED_TEXT_COLOR,
	ENABLED_TEXT_COLOR,
	READCOLOR,
	DEACTIAVED_TEXT_COLOR,
};

Sint16 EpisodeSelect[6]={1};

Sint16 SaveGamesAvail[10],StartGame,SoundStatus=1,pickquick;
char SaveGameNames[10][GAME_DESCRIPTION_LEN+1];
static const std::string SAVE_BASE_NAME = "bstone_ps_save_";



////////////////////////////////////////////////////////////////////
//
// INPUT MANAGER SCANCODE TABLES
//
////////////////////////////////////////////////////////////////////

#ifndef CACHE_KEY_DATA

#define type_cast(x,y) (reinterpret_cast<x>(const_cast<char*>(y)))

static Uint8* ScanNames[] = { // Scan code names with single chars
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "1"), type_cast(Uint8*, "2"),
    type_cast(Uint8*, "3"), type_cast(Uint8*, "4"),
    type_cast(Uint8*, "5"), type_cast(Uint8*, "6"),
    type_cast(Uint8*, "7"), type_cast(Uint8*, "8"),
    type_cast(Uint8*, "9"), type_cast(Uint8*, "0"),
    type_cast(Uint8*, "-"), type_cast(Uint8*, "+"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "Q"), type_cast(Uint8*, "W"),
    type_cast(Uint8*, "E"), type_cast(Uint8*, "R"),
    type_cast(Uint8*, "T"), type_cast(Uint8*, "Y"),
    type_cast(Uint8*, "U"), type_cast(Uint8*, "I"),
    type_cast(Uint8*, "O"), type_cast(Uint8*, "P"),
    type_cast(Uint8*, "["), type_cast(Uint8*, "]"),
    type_cast(Uint8*, "|"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "A"), type_cast(Uint8*, "S"),
    type_cast(Uint8*, "D"), type_cast(Uint8*, "F"),
    type_cast(Uint8*, "G"), type_cast(Uint8*, "H"),
    type_cast(Uint8*, "J"), type_cast(Uint8*, "K"),
    type_cast(Uint8*, "L"), type_cast(Uint8*, ";"),
    type_cast(Uint8*, "\""), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "Z"), type_cast(Uint8*, "X"),
    type_cast(Uint8*, "C"), type_cast(Uint8*, "V"),
    type_cast(Uint8*, "B"), type_cast(Uint8*, "N"),
    type_cast(Uint8*, "M"), type_cast(Uint8*, ","),
    type_cast(Uint8*, "."), type_cast(Uint8*, "/"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "\xF"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "-"), type_cast(Uint8*, "\x15"),
    type_cast(Uint8*, "5"), type_cast(Uint8*, "\x11"),
    type_cast(Uint8*, "+"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "\x13"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?"),
    type_cast(Uint8*, "?"), type_cast(Uint8*, "?")
}; // DEBUG - consolidate these

// Scan codes with >1 char names
static ScanCode ExtScanCodes[] = {
    sc_escape,
    sc_backspace,
    sc_tab,
    sc_control,
    sc_left_shift,
    sc_space,
    sc_caps_lock,
    sc_f1,
    sc_f2,
    sc_f3,
    sc_f4,
    sc_f5,
    sc_f6,
    sc_f7,
    sc_f8,
    sc_f9,
    sc_f10,
    sc_f11,
    sc_f12,
    sc_scroll_lock,
    sc_return,
    sc_right_shift,
    sc_print_screen,
    sc_alt,
    sc_home,
    sc_page_up,
    sc_end,
    sc_page_down,
    sc_insert,
    sc_delete,
    sc_num_lock,
    sc_up_arrow,
    sc_down_arrow,
    sc_left_arrow,
    sc_right_arrow,
    sc_none
}; // ExtScanCodes

static Uint8* ExtScanNames[] = { // Names corresponding to ExtScanCodes
    type_cast(Uint8*, "ESC"), type_cast(Uint8*, "BKSP"),
    type_cast(Uint8*, "TAB"), type_cast(Uint8*, "CTRL"),
    type_cast(Uint8*, "LSHFT"), type_cast(Uint8*, "SPACE"),
    type_cast(Uint8*, "CAPSLK"), type_cast(Uint8*, "F1"),
    type_cast(Uint8*, "F2"), type_cast(Uint8*, "F3"),
    type_cast(Uint8*, "F4"), type_cast(Uint8*, "F5"),
    type_cast(Uint8*, "F6"), type_cast(Uint8*, "F7"),
    type_cast(Uint8*, "F8"), type_cast(Uint8*, "F9"),
    type_cast(Uint8*, "F10"), type_cast(Uint8*, "F11"),
    type_cast(Uint8*, "F12"), type_cast(Uint8*, "SCRLK"),
    type_cast(Uint8*, "ENTER"), type_cast(Uint8*, "RSHFT"),
    type_cast(Uint8*, "PRTSC"), type_cast(Uint8*, "ALT"),
    type_cast(Uint8*, "HOME"), type_cast(Uint8*, "PGUP"),
    type_cast(Uint8*, "END"), type_cast(Uint8*, "PGDN"),
    type_cast(Uint8*, "INS"), type_cast(Uint8*, "DEL"),
    type_cast(Uint8*, "NUMLK"), type_cast(Uint8*, "UP"),
    type_cast(Uint8*, "DOWN"), type_cast(Uint8*, "LEFT"),
    type_cast(Uint8*, "RIGHT"), type_cast(Uint8*, "")
};

#else

Uint8 *ScanNames, * ExtScanNames;
ScanCode *ExtScanCodes;

#endif

static Uint8 special_keys[] =
{
	sc_back_quote,sc_equals,sc_minus,sc_l,sc_p,sc_m,sc_s,sc_i,sc_q,sc_w,sc_e,sc_return,sc_1,sc_2,sc_3,sc_4,sc_5,sc_tab
};


// BBi
namespace {


typedef std::map<ScanCode,const char*> BindsNames;
typedef BindsNames::const_iterator BindsNamesCIt;


enum BindsFindDirection {
    e_bfd_forward,
    e_bfd_backward
}; // enum BindsFindDirection

enum BindsFindFrom {
    e_bff_current,
    e_bff_not_current
}; // enum BindingFindFrom


class BindsItem {
public:
    const char* name;
    int name_width;
    Binding* binding;
}; // class BindsItem

static BindsItem binds[] = {
    { "MOVEMENT", 0, NULL },
    { "FORWARD", 0, &in_bindings[e_bi_forward] },
    { "BACKWARD", 0, &in_bindings[e_bi_backward] },
    { "LEFT", 0, &in_bindings[e_bi_left] },
    { "RIGHT", 0, &in_bindings[e_bi_right] },
    { "STRAFE", 0, &in_bindings[e_bi_strafe] },
    { "STRAFE LEFT", 0, &in_bindings[e_bi_strafe_left] },
    { "STRAFE RIGHT", 0, &in_bindings[e_bi_strafe_right] },
    { "QUICK LEFT", 0, &in_bindings[e_bi_quick_left] },
    { "QUICK RIGHT", 0, &in_bindings[e_bi_quick_right] },
    { "TURN AROUND", 0, &in_bindings[e_bi_turn_around] },
    { "RUN", 0, &in_bindings[e_bi_run] },
    { "", 0, NULL },

    { "WEAPONS", 0, NULL },
    { "ATTACK", 0, &in_bindings[e_bi_attack] },
    { "AUTO CHARGE PISTOL", 0, &in_bindings[e_bi_weapon_1] },
    { "SLOW FIRE PROTECTOR", 0, &in_bindings[e_bi_weapon_2] },
    { "RAPID ASSAULT WEAPON", 0, &in_bindings[e_bi_weapon_3] },
    { "DUAL NEUTRON DISRUPTOR", 0, &in_bindings[e_bi_weapon_4] },
    { "PLASMA DISCHARGE UNIT", 0, &in_bindings[e_bi_weapon_5] },
    { "ANTI-PLASMA CANNON", 0, &in_bindings[e_bi_weapon_6] },
    { "FISSION DETONATOR", 0, &in_bindings[e_bi_weapon_7] },
    { "", 0, NULL },

    { "INTERACTION", 0, NULL },
    { "USE", 0, &in_bindings[e_bi_use] },
    { "", 0, NULL },

    { "HUD", 0, NULL },
    { "STATS", 0, &in_bindings[e_bi_stats] },
    { "MAGNIFY RADAR", 0, &in_bindings[e_bi_radar_magnify] },
    { "MINIFY RADAR", 0, &in_bindings[e_bi_radar_minify] },
    { "", 0, NULL },

    { "MENU", 0, NULL },
    { "HELP", 0, &in_bindings[e_bi_help] },
    { "SAVE", 0, &in_bindings[e_bi_save] },
    { "LOAD", 0, &in_bindings[e_bi_load] },
    { "SOUND OPTIONS", 0, &in_bindings[e_bi_sound] },
    { "CONTROLS", 0, &in_bindings[e_bi_controls] },
    { "END GAME", 0, &in_bindings[e_bi_end_game] },
    { "QUICK SAVE", 0, &in_bindings[e_bi_quick_save] },
    { "QUICK LOAD", 0, &in_bindings[e_bi_quick_load] },
    { "QUICK EXIT", 0, &in_bindings[e_bi_quick_exit] },
    { "", 0, NULL },

    { "OPTIONS", 0, NULL },
    { "ATTACK INFO", 0, &in_bindings[e_bi_attack_info] },
    { "LIGHTNING", 0, &in_bindings[e_bi_lightning] },
    { "SOUND", 0, &in_bindings[e_bi_sfx] },
    { "MUSIC", 0, &in_bindings[e_bi_music] },
    { "CEILING", 0, &in_bindings[e_bi_ceiling] },
    { "FLOORING", 0, &in_bindings[e_bi_flooring] },
    { "", 0, NULL },

    { "MISC", 0, NULL },
    { "PAUSE", 0, &in_bindings[e_bi_pause] },
    { NULL, 0, NULL }
}; // binds


const int k_binds_max_per_window = 14;
const int k_binds_text_keys_gap = 3;
const int k_binds_line_spacing = 1;
const int k_binds_top = 28;

const Uint8 k_binds_category_color = 0x4A;
const Uint8 k_binds_text_color = 0x56;
const Uint8 k_binds_key_text_color = 0x7F;
const Uint8 k_binds_key_bar_default_color = 0x03;
const Uint8 k_binds_key_bar_active_color = 0x31;
const Uint8 k_binds_key_bar_assign_color = 0x14;

int binds_count;
int binds_window;
int binds_window_offset;
int binds_key_index;
int binds_key_width;
int binds_max_text_width;
int binds_text_height;
int binds_text_x;
int binds_key_x[k_max_binding_keys];
bool binds_is_assigning = false;

BindsNames binds_names;


void binds_initialize_menu()
{
    static bool is_initialized = false;

    if (is_initialized)
        return;

    binds_count = 0;
    binds_window = 0;
    binds_window_offset = 0;
    binds_key_index = 0;
    binds_key_width = 0;
    binds_max_text_width = 0;
    binds_text_height = 0;
    binds_is_assigning = false;

    bool has_bindings = false;

    fontnumber = 2;

    for (BindsItem* i = binds; i->name; ++i) {
        ++binds_count;

        if (i->name != NULL && i->name[0] != '\0') {
            int width = 0;
            int height = 0;
            VW_MeasurePropString(i->name, &width, &height);

            i->name_width = width;

            if (width > binds_max_text_width)
                binds_max_text_width = width;

            if (height > binds_text_height)
                binds_text_height = height;

            has_bindings = true;
        }
    }

    if (!has_bindings) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No bindings.\n");
        Quit("no bindings");
    }

    binds_names.clear();
    binds_names[sc_return] = "ENTER";
    binds_names[sc_space] = "SPACE";
    binds_names[sc_minus] = "-";
    binds_names[sc_equals] = "=";
    binds_names[sc_backspace] = "BACKSPACE";
    binds_names[sc_tab] = "TAB";
    binds_names[sc_alt] = "ALT";
    binds_names[sc_left_bracket] = "[";
    binds_names[sc_right_bracket] = "]";
    binds_names[sc_control] = "CTRL";
    binds_names[sc_caps_lock] = "CAPS LOCK";
    binds_names[sc_num_lock] = "NUM LOCK";
    binds_names[sc_scroll_lock] = "SCROLL LOCK";
    binds_names[sc_left_shift] = "L-SHIFT";
    binds_names[sc_right_shift] = "R-SHIFT";
    binds_names[sc_up_arrow] = "UP";
    binds_names[sc_down_arrow] = "DOWN";
    binds_names[sc_left_arrow] = "LEFT";
    binds_names[sc_right_arrow] = "RIGHT";
    binds_names[sc_insert] = "INS";
    binds_names[sc_delete] = "DEL";
    binds_names[sc_home] = "HOME";
    binds_names[sc_end] = "END";
    binds_names[sc_page_up] = "PGUP";
    binds_names[sc_page_down] = "PGDN";
    binds_names[sc_slash] = "/";
    binds_names[sc_f1] = "F1";
    binds_names[sc_f2] = "F2";
    binds_names[sc_f3] = "F3";
    binds_names[sc_f4] = "F4";
    binds_names[sc_f5] = "F5";
    binds_names[sc_f6] = "F6";
    binds_names[sc_f7] = "F7";
    binds_names[sc_f8] = "F8";
    binds_names[sc_f9] = "F9";
    binds_names[sc_f10] = "F10";
    binds_names[sc_f11] = "F11";
    binds_names[sc_f12] = "F12";
    binds_names[sc_print_screen] = "PRT SCR";
    binds_names[sc_pause] = "PAUSE";
    binds_names[sc_back_quote] = "`";
    binds_names[sc_semicolon] = ";";
    binds_names[sc_quote] = "'";
    binds_names[sc_backslash] = "\\";
    binds_names[sc_comma] = ",";
    binds_names[sc_period] = ".";

    binds_names[sc_1] = "1";
    binds_names[sc_2] = "2";
    binds_names[sc_3] = "3";
    binds_names[sc_4] = "4";
    binds_names[sc_5] = "5";
    binds_names[sc_6] = "6";
    binds_names[sc_7] = "7";
    binds_names[sc_8] = "8";
    binds_names[sc_9] = "9";
    binds_names[sc_0] = "0";

    binds_names[sc_a] = "A";
    binds_names[sc_b] = "B";
    binds_names[sc_c] = "C";
    binds_names[sc_d] = "D";
    binds_names[sc_e] = "E";
    binds_names[sc_f] = "F";
    binds_names[sc_g] = "G";
    binds_names[sc_h] = "H";
    binds_names[sc_i] = "I";
    binds_names[sc_j] = "J";
    binds_names[sc_k] = "K";
    binds_names[sc_l] = "L";
    binds_names[sc_m] = "M";
    binds_names[sc_n] = "N";
    binds_names[sc_o] = "O";
    binds_names[sc_p] = "P";
    binds_names[sc_q] = "Q";
    binds_names[sc_r] = "R";
    binds_names[sc_s] = "S";
    binds_names[sc_t] = "T";
    binds_names[sc_u] = "U";
    binds_names[sc_v] = "V";
    binds_names[sc_w] = "W";
    binds_names[sc_x] = "X";
    binds_names[sc_y] = "Y";
    binds_names[sc_z] = "Z";

    binds_names[sc_kp_minus] = "KP MINUS";
    binds_names[sc_kp_plus] = "KP PLUS";

    binds_names[sc_mouse_left] = "MOUSE 1";
    binds_names[sc_mouse_middle] = "MOUSE 2";
    binds_names[sc_mouse_right] = "MOUSE 3";
    binds_names[sc_mouse_x1] = "MOUSE 4";
    binds_names[sc_mouse_x2] = "MOUSE 5";

    for (BindsNamesCIt i = binds_names.begin();
        i != binds_names.end();
        ++i)
    {
        int width = 0;
        int height = 0;
        VW_MeasurePropString(i->second, &width, &height);

        if (width > binds_key_width)
            binds_key_width = width;
    }

    int max_keys_width = k_max_binding_keys * (binds_key_width + 1);
    int max_text_width = 2 + binds_max_text_width;
    int max_width = max_keys_width + k_binds_text_keys_gap + max_text_width;

    int text_x = (SCREEN_W - max_width) / 2;

    int base_key_x = text_x + max_text_width + k_binds_text_keys_gap;

    binds_text_x = text_x;

    for (int i = 0; i < k_max_binding_keys; ++i)
        binds_key_x[i] = base_key_x + (i * (binds_key_width + 1));

    is_initialized = true;
}

bool binds_advance_to_item(
    BindsFindDirection direction)
{
    switch (direction) {
    case e_bfd_forward:
        if ((binds_window + binds_window_offset + 1) < binds_count) {
            ++binds_window_offset;

            if (binds_window_offset == k_binds_max_per_window) {
                ++binds_window;
                --binds_window_offset;
            }

            return true;
        } else
            return false;

    case e_bfd_backward:
        if ((binds_window + binds_window_offset) > 0) {
            --binds_window_offset;

            if (binds_window_offset < 0) {
                --binds_window;
                binds_window_offset = 0;
            }

            return true;
        } else
            return false;

    default:
        return false;
    }

    return false;
}

bool binds_find_item(
    BindsFindDirection direction,
    BindsFindFrom from)
{
    if (from == e_bff_not_current) {
        if (!binds_advance_to_item(direction))
            return false;
    }

    while (true) {
        if (binds[binds_window + binds_window_offset].binding)
            return true;

        if (!binds_advance_to_item(direction))
            return false;
    }

    return false;
}

bool binds_assign_new_key(
    ScanCode key,
    Binding& binding)
{
    BindsNamesCIt it =
        binds_names.find(LastScan);

    if (it == binds_names.end())
        return false;

    for (int b = 0; b < k_max_bindings; ++b) {
        for (int k = 0; k < k_max_binding_keys; ++k) {
            if (in_bindings[b][k] == key)
                in_bindings[b][k] = sc_none;
        }
    }

    binding[binds_key_index] = key;

    return true;
}

void binds_remove_binding()
{
    BindsItem& item = binds[binds_window + binds_window_offset];
    (*item.binding)[binds_key_index] = sc_none;
}

void binds_draw_item_text(
    int item_index)
{
    BindsItem& item = binds[binds_window + item_index];

    if (item.name[0] == '\0')
        return;

    int x = SCREEN_X + binds_text_x;
    int y = SCREEN_Y + k_binds_top +
        (item_index * (binds_text_height + k_binds_line_spacing));

    int text_left_offset = 0;
    int text_width = 0;

    if (item.binding) {
        text_width = item.name_width;
        text_left_offset = binds_max_text_width - item.name_width;

        if (text_left_offset < 0) {
            text_left_offset = 0;
            text_width = binds_max_text_width;
        }
    } else
        text_width = SCREEN_W;

    PrintX = x + text_left_offset;
    PrintY = y;
    WindowX = PrintX;
    WindowY = PrintY;
    WindowW = text_width;
    WindowH = binds_text_height;

    if (item.binding) {
        fontcolor = k_binds_text_color;
        US_Print(item.name);
    } else {
        fontcolor = k_binds_category_color;
        US_PrintCentered(item.name);
    }
}

void binds_draw_keys(
    int item_index)
{
    const BindsItem& item =
        binds[binds_window + item_index];

    if (!item.binding)
        return;

    int y = SCREEN_Y + k_binds_top +
        (item_index * (binds_text_height + k_binds_line_spacing));

    bool is_current = (item_index == binds_window_offset);

    for (int k = 0; k < k_max_binding_keys; ++k) {
        Uint8 color;
        ScanCode key;
        const char* key_name;

        bool is_active = is_current && (binds_key_index == k);

        if (is_active) {
            color =
                binds_is_assigning ?
                    k_binds_key_bar_assign_color :
                    k_binds_key_bar_active_color;
        } else
            color = k_binds_key_bar_default_color;

        int x = SCREEN_X + binds_key_x[k];

        VWB_Bar(
            x,
            y,
            binds_key_width,
            binds_text_height,
            color);

        PrintX = x;
        PrintY = y;
        WindowX = PrintX;
        WindowY = PrintY;
        WindowW = binds_key_width;
        WindowH = binds_text_height;

        if (!(is_active && binds_is_assigning)) {
            key = (*item.binding)[k];

            if (key != sc_none) {
                key_name = "???";

                BindsNamesCIt name_it = binds_names.find(key);

                if (name_it != binds_names.end())
                    key_name = name_it->second;

                fontcolor = k_binds_key_text_color;
                US_PrintCentered(key_name);
            }
        }
    }
}

void binds_draw()
{
    bool found_item = false;

    found_item = binds_find_item(e_bfd_forward, e_bff_current);

    if (!found_item)
        found_item = binds_find_item(e_bfd_backward, e_bff_current);

    ClearMScreen();
    DrawMenuTitle("CUSTOMIZE CONTROLS");

    DrawInstructions(
        binds_is_assigning ? IT_CONTROLS_ASSIGNING_KEY : IT_CONTROLS);

    fontnumber = 2;

    for (int i = 0; i < k_binds_max_per_window; ++i) {
        int item_index = binds_window + i;

        if (item_index == binds_count)
            break;

        binds_draw_item_text(i);
        binds_draw_keys(i);
    }
}

void binds_draw_menu()
{
    bool is_up_pressed = false;
    bool is_down_pressed = false;
    bool is_left_pressed = false;
    bool is_right_pressed = false;
    bool is_pgdn_pressed = false;
    bool is_pgup_pressed = false;
    bool is_enter_pressed = false;
    bool is_delete_pressed = false;
    bool is_escape_pressed = false;

    bool handle_up = false;
    bool handle_down = false;
    bool handle_left = false;
    bool handle_right = false;
    bool handle_pgdn = false;
    bool handle_pgup = false;
    bool handle_enter = false;
    bool handle_delete = false;
    bool handle_escape = false;

    bool fade_in = true;

    CA_CacheGrChunk(STARTFONT + 2);
    CA_CacheGrChunk(STARTFONT + 4);
    binds_initialize_menu();

    binds_is_assigning = false;

    while (true) {
        binds_draw();
        VW_UpdateScreen();

        if (fade_in) {
            fade_in = false;
            MenuFadeIn();
        }

        in_handle_events();

        if (binds_is_assigning) {
            LastScan = sc_none;
            bool quit = false;

            while (!quit) {
                LastScan = sc_none;
                in_handle_events();

                if (Keyboard[sc_escape]) {
                    quit = true;
                    sd_play_player_sound(ESCPRESSEDSND, bstone::AC_ITEM);
                } else if (LastScan != sc_none) {
                    BindsItem& item =
                        binds[binds_window + binds_window_offset];

                    if (binds_assign_new_key(LastScan, *item.binding)) {
                        ShootSnd();
                        quit = true;
                    } else
                        sd_play_player_sound(NOWAYSND, bstone::AC_ITEM);
                }
            }

            is_escape_pressed = true;
            binds_is_assigning = false;
        } else {
            if (Keyboard[sc_up_arrow]) {
                if (!is_up_pressed) {
                    handle_up = true;
                    is_up_pressed = true;
                }
            } else
                is_up_pressed = false;

            if (Keyboard[sc_down_arrow]) {
                if (!is_down_pressed) {
                    handle_down = true;
                    is_down_pressed = true;
                }
            } else
                is_down_pressed = false;

            if (Keyboard[sc_left_arrow]) {
                if (!is_left_pressed) {
                    handle_left = true;
                    is_left_pressed = true;
                }
            } else
                is_left_pressed = false;

            if (Keyboard[sc_right_arrow]) {
                if (!is_right_pressed) {
                    handle_right = true;
                    is_right_pressed = true;
                }
            } else
                is_right_pressed = false;

            if (Keyboard[sc_page_down]) {
                if (!is_pgdn_pressed) {
                    handle_pgdn = true;
                    is_pgdn_pressed = true;
                }
            } else
                is_pgdn_pressed = false;

            if (Keyboard[sc_page_up]) {
                if (!is_pgup_pressed) {
                    handle_pgup = true;
                    is_pgup_pressed = true;
                }
            } else
                is_pgup_pressed = false;

            if (Keyboard[sc_return]) {
                if (!is_enter_pressed) {
                    handle_enter = true;
                    is_enter_pressed = true;
                }
            } else
                is_enter_pressed = false;

            if (Keyboard[sc_delete]) {
                if (!is_delete_pressed) {
                    handle_delete = true;
                    is_delete_pressed = true;
                }
            } else
                is_delete_pressed = false;

            if (Keyboard[sc_escape]) {
                if (!is_escape_pressed) {
                    handle_escape = true;
                    is_escape_pressed = true;
                }
            } else
                is_escape_pressed = false;

            if (handle_up) {
                handle_up = false;
                binds_find_item(e_bfd_backward, e_bff_not_current);
            }

            if (handle_down) {
                handle_down = false;
                binds_find_item(e_bfd_forward, e_bff_not_current);
            }

            if (handle_left) {
                handle_left = false;

                if (binds_key_index == 1)
                    binds_key_index = 0;
            }

            if (handle_right) {
                handle_right = false;

                if (binds_key_index == 0)
                    binds_key_index = 1;
            }

            if (handle_pgdn) {
                handle_pgdn = false;

                for (int i = 0; i < k_binds_max_per_window; ++i)
                    binds_find_item(e_bfd_forward, e_bff_not_current);
            }

            if (handle_pgup) {
                handle_pgup = false;

                for (int i = 0; i < k_binds_max_per_window; ++i)
                    binds_find_item(e_bfd_backward, e_bff_not_current);
            }

            if (handle_enter) {
                handle_enter = false;
                binds_is_assigning = true;
            }

            if (handle_delete) {
                handle_delete = false;
                binds_remove_binding();
                ShootSnd();
            }

            if (handle_escape) {
                handle_escape = false;
                sd_play_player_sound(ESCPRESSEDSND, bstone::AC_ITEM);
                break;
            }
        }
    }

    MenuFadeOut();
}


} // namespace
// BBi


//-------------------------------------------------------------------------
// HelpScreens()
//-------------------------------------------------------------------------
void HelpScreens()
{
#ifndef ID_CACHE_HELP
	HelpPresenter("HELP.TXT",false,0,true);
#else
	HelpPresenter(NULL,false,HELPTEXT,true);
#endif
}


//-------------------------------------------------------------------------
// HelpPresenter()
//-------------------------------------------------------------------------
void HelpPresenter(const char *fname,boolean continue_keys, Uint16 id_cache, boolean startmusic)
{
	#define FULL_VIEW_WIDTH			19

	PresenterInfo pi;
	Sint16 oldwidth;

	memset(&pi,0,sizeof(pi));

	pi.flags = TPF_SHOW_PAGES;
	if (continue_keys)
		pi.flags |= TPF_CONTINUE;

	VW_FadeOut();

// Change view size to MAX! (scaler clips shapes on smaller views)
//
	oldwidth = viewwidth/16;
	if (oldwidth != FULL_VIEW_WIDTH)
		NewViewSize();

// Draw help border
//
	CacheLump(H_TOPWINDOWPIC,H_BOTTOMINFOPIC);
	VWB_DrawPic (0,0,H_TOPWINDOWPIC);
	VWB_DrawPic (0,8,H_LEFTWINDOWPIC);
	VWB_DrawPic (312,8,H_RIGHTWINDOWPIC);
	VWB_DrawPic (8,176,H_BOTTOMINFOPIC);
	UnCacheLump(H_TOPWINDOWPIC,H_BOTTOMINFOPIC);

// Setup for text presenter
//
	pi.xl=8;
	pi.yl=8;
	pi.xh=311;
	pi.yh=175;
	pi.ltcolor=0x7b;
	pi.bgcolor=0x7d;
	pi.dkcolor=0x7f;
	pi.shcolor=0x00;
	pi.fontnumber=4;

	if (continue_keys)
		pi.infoline = (char *)" UP / DN - PAGES       ENTER - CONTINUES         ESC - EXITS";
	else
		pi.infoline = (char *)"           UP / DN - PAGES            ESC - EXITS";

	if (startmusic)
		StartCPMusic(TEXTSONG);

// Load, present, and free help text.
//
	TP_LoadScript(fname,&pi,id_cache);
	TP_Presenter(&pi);
	TP_FreeScript(&pi,id_cache);

	MenuFadeOut();

// Reset view size
//
	if (oldwidth != FULL_VIEW_WIDTH)
		NewViewSize();

	if (startmusic && TPscan==sc_escape)
		StartCPMusic(MENUSONG);
	IN_ClearKeysDown();
}

//--------------------------------------------------------------------------
// US_ControlPanel() - Control Panel!  Ta Da!
//--------------------------------------------------------------------------
void US_ControlPanel(Uint8 scancode)
{
	Sint16 which;

#ifdef CACHE_KEY_DATA
 CA_CacheGrChunk(SCANNAMES_DATA);
 CA_CacheGrChunk(EXTSCANNAMES_DATA);
 CA_CacheGrChunk(EXTSCANCODES_DATA);

 ScanNames = grsegs[SCANNAMES_DATA];
 ExtScanNames = grsegs[EXTSCANNAMES_DATA];
 ExtScanCodes = grsegs[EXTSCANCODES_DATA];
#else

//
// This code doesn't correctly save the table data -- it saves garbage
// for SCANNAMES and EXTSCANCODES...
//

//	IO_WriteFile("SCANNAME.BIN",ScanNames,sizeof(ScanNames));
//	IO_WriteFile("EXTSCNAM.BIN",ExtScanNames,sizeof(ExtScanNames));
//	IO_WriteFile("EXTSCCOD.BIN",ExtScanCodes,sizeof(ExtScanCodes));

#endif

	if (ingame)
		if (CP_CheckQuick(scancode))
			return;

	SetupControlPanel();
	StartCPMusic(MENUSONG);

 //
 // F-KEYS FROM WITHIN GAME
 //
 switch(scancode)
 {
  case sc_f1:
	 CleanupControlPanel();
	 HelpScreens();
	 return;

  case sc_f2:
	 CP_SaveGame(0);
    goto finishup;

  case sc_f3:
	 CP_LoadGame(0);
//	 refresh_screen=false;
    goto finishup;

  case sc_f4:
    CP_Sound(0);
	 goto finishup;

// BBi
#if 0
  case sc_f5:
    CP_ChangeView(0);
    goto finishup;
#endif // 0

  case sc_f6:
    CP_Control(0);
    goto finishup;

  finishup:
	 CleanupControlPanel();
	 return;
 }


 DrawMainMenu();
 MenuFadeIn();
 StartGame=0;

 //
 // MAIN MENU LOOP
 //
 do
 {
  which=HandleMenu(&MainItems,&MainMenu[0],NULL);

  switch(which)
  {
	case MM_VIEW_SCORES:
	  if (MainMenu[MM_VIEW_SCORES].routine == NULL)
		 if (CP_EndGame())
			 StartGame=1;

	  DrawMainMenu();
	  MenuFadeIn();
	  break;

	case -1:
	case MM_LOGOFF:
	  CP_Quit();
	  break;

	default:
	  if (!StartGame)
	  {
		DrawMainMenu();
		MenuFadeIn();
	  }
  }

 //
 // "EXIT OPTIONS" OR "NEW GAME" EXITS
 //
 } while(!StartGame);

 //
 // DEALLOCATE EVERYTHING
 //
 CleanupControlPanel();
 if (!loadedgame)
	 StopMusic();


 //
 // CHANGE MAINMENU ITEM
 //
 if (startgame || loadedgame)
 {
  MainMenu[MM_VIEW_SCORES].routine = NULL;
  strcpy(MainMenu[MM_VIEW_SCORES].string,"END GAME");
 }

 if (ingame && loadedgame)
	refresh_screen=false;


#ifdef CACHE_KEY_DATA
 FREEFONT(SCANNAMES_DATA);
 FREEFONT(EXTSCANNAMES_DATA);
 FREEFONT(EXTSCANCODES_DATA);
#endif

 // RETURN/START GAME EXECUTION
}

//--------------------------------------------------------------------------
// DrawMainMenu(void)
//--------------------------------------------------------------------------
void DrawMainMenu(void)
{
	ControlPanelFree();
	CA_CacheScreen (BACKGROUND_SCREENPIC);
	ControlPanelAlloc();

	ClearMScreen();
	DrawMenuTitle("MAIN OPTIONS");
	DrawInstructions(IT_STANDARD);

 //
 // CHANGE "MISSION" AND "DEMO"
 //
	if (ingame)
	{
		strcpy(&MainMenu[MM_BACK_TO_DEMO].string[8],"MISSION");
		MainMenu[MM_BACK_TO_DEMO].active=AT_READIT;
	}
	else
	{
		strcpy(&MainMenu[MM_BACK_TO_DEMO].string[8],"DEMO");
		MainMenu[MM_BACK_TO_DEMO].active=AT_ENABLED;
	}

	fontnumber = 4;				// COAL

	DrawMenu(&MainItems,&MainMenu[0]);

	VW_UpdateScreen();
}

//--------------------------------------------------------------------------
// READ THIS!
//--------------------------------------------------------------------------
void CP_ReadThis(Sint16)
{
	ControlPanelFree();
	HelpScreens();
	ControlPanelAlloc();
}


//--------------------------------------------------------------------------
// CP_OrderingInfo()
//--------------------------------------------------------------------------
void CP_OrderingInfo(Sint16)
{
	ControlPanelFree();
#ifndef ID_CACHE_HELP
	HelpPresenter("ORDER.TXT",false,0,true);
#else
	HelpPresenter("",false,ORDERTEXT,true);
#endif
	ControlPanelAlloc();
}


//-------------------------------------------------------------------------
// CP_BlakeStoneSaga()
//-------------------------------------------------------------------------
void CP_BlakeStoneSaga(Sint16)
{
	ControlPanelFree();
#ifndef ID_CACHE_HELP
	HelpPresenter("SAGA.TXT",false,0,true);
#else
	HelpPresenter("",false,SAGATEXT,true);
#endif
	ControlPanelAlloc();
}

//--------------------------------------------------------------------------
// CP_CheckQuick() - CHECK QUICK-KEYS & QUIT (WHILE IN A GAME)
//--------------------------------------------------------------------------
Sint16 CP_CheckQuick(Uint16 scancode)
{
	switch(scancode)
	{
	// END GAME
	//
		case sc_f7:
			VW_ScreenToScreen (static_cast<Uint16>(displayofs),static_cast<Uint16>(bufferofs),80,160);
			CA_CacheGrChunk(STARTFONT+1);

			WindowH=160;
			if (Confirm(ENDGAMESTR))
			{
				playstate = ex_died;
				pickquick = gamestate.lives = 0;
			}

			WindowH=200;
			fontnumber=4;
			return(1);

	// QUICKSAVE
	//
		case sc_f8:
			if (SaveGamesAvail[static_cast<int>(LSItems.curpos)] && pickquick)
			{
				char string[100]="Quick Save will overwrite:\n\"";

				CA_CacheGrChunk(STARTFONT+1);

				strcat(string,SaveGameNames[static_cast<int>(LSItems.curpos)]);
				strcat(string,"\"?");
				VW_ScreenToScreen (static_cast<Uint16>(displayofs),static_cast<Uint16>(bufferofs),80,160);

#if IN_DEVELOPMENT
				if (TestQuickSave || Confirm(string))
				{
            	if (TestQuickSave)
               	TestQuickSave--;
#else
				if (Confirm(string))
            {
#endif
					CA_CacheGrChunk(STARTFONT+1);
					CP_SaveGame(1);
					fontnumber=4;
				}
				else
					refresh_screen=false;
			}
			else
			{
				CA_CacheGrChunk(STARTFONT+1);

				VW_FadeOut ();

				StartCPMusic(MENUSONG);
				pickquick=CP_SaveGame(0);

				lasttimecount = TimeCount;
                ::in_clear_mouse_deltas();
			}

			return(1);

	// QUICKLOAD
	//
		case sc_f9:
			if (SaveGamesAvail[static_cast<int>(LSItems.curpos)] && pickquick)
			{
				char string[100]="Quick Load:\n\"";

				CA_CacheGrChunk(STARTFONT+1);

				strcat(string,SaveGameNames[static_cast<int>(LSItems.curpos)]);
				strcat(string,"\"?");
				VW_ScreenToScreen (static_cast<Uint16>(displayofs),static_cast<Uint16>(bufferofs),80,160); 

				if (Confirm(string))
					CP_LoadGame(1);
				else
				{
					refresh_screen=false;
					return(1);
				}

				fontnumber=4;
			}
			else
			{
				CA_CacheGrChunk(STARTFONT+1);

				VW_FadeOut ();

				StartCPMusic(MENUSONG);
				pickquick=CP_LoadGame(0);

				lasttimecount = TimeCount;
                ::in_clear_mouse_deltas();
			}

			if (pickquick)
				refresh_screen=false;
			return(1);

	// QUIT
	//
		case sc_f10:
			CA_CacheGrChunk(STARTFONT+1);
			VW_ScreenToScreen (static_cast<Uint16>(displayofs),static_cast<Uint16>(bufferofs),80,160);

			WindowX=WindowY=0;
			WindowW=320;
			WindowH=160;
			if (Confirm(QuitToDosStr))
				ExitGame();

			refresh_screen=false;
			WindowH=200;
			fontnumber=4;

			return(1);
	}

	return(0);
}

//--------------------------------------------------------------------------
// END THE CURRENT GAME
//--------------------------------------------------------------------------
Sint16 CP_EndGame(void)
{
	if (!Confirm(ENDGAMESTR))
		return 0;

	pickquick = gamestate.lives = 0;
	playstate = ex_died;
   InstantQuit = 1;

#if 0
#pragma warn -sus
	 MainMenu[MM_VIEW_SCORES].routine=&CP_ViewScores;
	_fstrcpy(MainMenu[MM_VIEW_SCORES].string,"HIGH SCORES");
#pragma warn +sus
#endif

	return 1;
}

//--------------------------------------------------------------------------
// CP_ViewScores() - VIEW THE HIGH SCORES
//--------------------------------------------------------------------------
void CP_ViewScores(Sint16)
{
	fontnumber=4;
	StartCPMusic(ROSTER_MUS);
	DrawHighScores ();
	VW_UpdateScreen ();
	MenuFadeIn();
	fontnumber=1;

	IN_Ack();

	StartCPMusic(MENUSONG);
	MenuFadeOut();
}

//--------------------------------------------------------------------------
// CP_NewGame() - START A NEW GAME
//--------------------------------------------------------------------------
void CP_NewGame(Sint16)
{
	Sint16 which,episode;

	DrawMenuTitle("Difficulty Level");
	DrawInstructions(IT_STANDARD);


#if 0

firstpart:

 DrawNewEpisode();
 do
 {
  which=HandleMenu(&NewEitems,&NewEmenu[0],DrawEpisodePic);
  switch(which)
  {
	case -1:
	 MenuFadeOut();
	 return;

	default:
	 if (!EpisodeSelect[which])
	 {
	  SD_PlaySound (NOWAYSND);
	  CacheMessage(READTHIS_TEXT);
	  IN_ClearKeysDown();
	  IN_Ack();
	  VL_Bar(35,69,250,62,TERM_BACK_COLOR);
	  DrawNewEpisode();
	  which = 0;
	 }
	 else
	 {
	  episode = which;
	  which = 1;
	 }
	 break;
  }

 } while (!which);

 ShootSnd();
#else
 episode = 0;
#endif

#if 0
 //
 // ALREADY IN A GAME?
 //
	if (ingame)
		if (!Confirm(CURGAME))
		{
			MenuFadeOut();
			return;
		}
#endif

secondpart:

	MenuFadeOut();
	if (ingame)
		CA_CacheScreen(BACKGROUND_SCREENPIC);
	DrawNewGame();
	which=HandleMenu(&NewItems,&NewMenu[0],DrawNewGameDiff);

 if (which<0)
 {
  MenuFadeOut();
#if 0
  goto firstpart;
#else
  return;
#endif
 }

 ShootSnd();
 MenuFadeOut();
 ControlPanelFree();

 if (Breifing(BT_INTRO,episode))
 {
	CA_CacheScreen (BACKGROUND_SCREENPIC);
	ControlPanelAlloc();
	goto secondpart;
 }

 StartGame=1;
 NewGame(which,episode);

 //
 // CHANGE "READ THIS!" TO NORMAL COLOR
 //
 MainMenu[MM_READ_THIS].active=AT_ENABLED;

}

//---------------------------------------------------------------------------
// DrawMenuTitle() - Draws the menu title
//---------------------------------------------------------------------------
void DrawMenuTitle(const char *title)
{

	fontnumber = 3;
	CA_CacheGrChunk(STARTFONT+3);

	PrintX = WindowX = 32;
	PrintY = WindowY = 32;
	WindowW = 244;
	WindowH = 20;

	SETFONTCOLOR(TERM_SHADOW_COLOR,TERM_BACK_COLOR);
	US_PrintCentered(title);

	WindowX = 32-1;
	WindowY = 32-1;

	SETFONTCOLOR(ENABLED_TEXT_COLOR,TERM_BACK_COLOR);
	US_PrintCentered(title);

	FREEFONT(STARTFONT+3);

}

//---------------------------------------------------------------------------
// DrawInstructions() - Draws instructions centered at the bottom of
//								the view screen.
//
// NOTES: Orginal font number or font color is not maintained.
//---------------------------------------------------------------------------
void DrawInstructions(inst_type Type)
{
	#define INSTRUCTIONS_Y_POS		154+10

    const char* instr[MAX_INSTRUCTIONS] = {
        "UP/DN SELECTS - ENTER CHOOSES - ESC EXITS",
        "PRESS ANY KEY TO CONTINUE",
        "ENTER YOUR NAME AND PRESS ENTER",
        "RT/LF ARROW SELECTS - ENTER CHOOSES",

        // BBi
        "UP/DN SELECTS - LF/RT CHANGES - ESC EXITS",
        "ARROWS SELECTS - ENTER CHOOSES - DEL REMOVES",
        "ESC EXITS"
    };

	fontnumber = 2;

	WindowX = 48;
   WindowY = INSTRUCTIONS_Y_POS;
	WindowW = 236;
   WindowH = 8;

   VWB_Bar(WindowX,WindowY-1,WindowW,WindowH,TERM_BACK_COLOR);

	SETFONTCOLOR(TERM_SHADOW_COLOR,TERM_BACK_COLOR);
	US_PrintCentered(instr[Type]);

   WindowX--;
   WindowY--;

	SETFONTCOLOR(INSTRUCTIONS_TEXT_COLOR,TERM_BACK_COLOR);
	US_PrintCentered(instr[Type]);
}

#if 0

//--------------------------------------------------------------------------
// DrawNewEpisode() - DRAW NEW EPISODE MENU
//--------------------------------------------------------------------------
void DrawNewEpisode(void)
{
	ClearMScreen();

	DrawMenuTitle("CHOOSE A MISSION");
	DrawInstructions(IT_STANDARD);

	PrintY=51;
	WindowX=58;

	fontnumber = 2;							// six point font
	DrawMenu(&NewEitems,&NewEmenu7[0]);

	DrawEpisodePic(NewEitems.curpos);

	VW_UpdateScreen();
	MenuFadeIn();
	WaitKeyUp();

}

#endif

//--------------------------------------------------------------------------
// DrawNewGame() - DRAW NEW GAME MENU
//--------------------------------------------------------------------------
void DrawNewGame(void)
{
	ClearMScreen();
	DrawMenuTitle("DIFFICULTY LEVEL");
	DrawInstructions(IT_STANDARD);

	fontnumber = 2;							// six point font
	DrawMenu(&NewItems,&NewMenu[0]);

	DrawNewGameDiff(NewItems.curpos);

	px=48;
	py=INSTRUCTIONS_Y_POS-24;
	ShPrint("        HIGHER DIFFICULTY LEVELS CONTAIN",TERM_SHADOW_COLOR,false);

	px=48;
	py+=6;
	ShPrint("            MORE, STRONGER ENEMIES",TERM_SHADOW_COLOR,false);


	VW_UpdateScreen();

	MenuFadeIn();
	WaitKeyUp();
}

//--------------------------------------------------------------------------
// DRAW NEW GAME GRAPHIC
//--------------------------------------------------------------------------
void DrawNewGameDiff(Sint16 w)
{
	VWB_DrawPic(192,77,w+C_BABYMODEPIC);
}

//--------------------------------------------------------------------------
// DRAW NEW GAME GRAPHIC
//--------------------------------------------------------------------------
void DrawEpisodePic(Sint16 w)
{
	VWB_DrawPic(176,72,w+C_EPISODE1PIC);
}

//--------------------------------------------------------------------------
// CP_GameOptions() - DRAW THE GAME OPTIONS MENU
//--------------------------------------------------------------------------
void CP_GameOptions(Sint16)
{
	Sint16 which;

	CA_CacheScreen (BACKGROUND_SCREENPIC);
	DrawGopMenu();
	MenuFadeIn();
	WaitKeyUp();

	do
	{
		which=HandleMenu(&GopItems,&GopMenu[0],NULL);

		if (which != -1)
		{
			DrawGopMenu();
			MenuFadeIn();
		}

	} while(which>=0);

	MenuFadeOut();
}

//--------------------------------------------------------------------------
// DrawGopMenu(void)
//--------------------------------------------------------------------------
void DrawGopMenu(void)
{
	CA_CacheScreen (BACKGROUND_SCREENPIC);

	ClearMScreen();
	DrawMenuTitle("GAME OPTIONS");
	DrawInstructions(IT_STANDARD);

	fontnumber = 4;				// COAL

	DrawMenu(&GopItems,&GopMenu[0]);

	VW_UpdateScreen();
}


void ChangeSwaps(void)
{
	WindowX=WindowY=0;
	WindowW=320;
	WindowH=200;
   Message(Computing);

	PM_Shutdown();
	PM_Startup ();
	ClearMemory();
	ControlPanelAlloc();

   IN_UserInput(50);
	IN_ClearKeysDown();

}

//--------------------------------------------------------------------------
// GAME SWITCHES MENU
//--------------------------------------------------------------------------
void CP_Switches(Sint16)
{
	Sint16 which;

	CA_CacheScreen (BACKGROUND_SCREENPIC);
	DrawSwitchMenu();
	MenuFadeIn();
	WaitKeyUp();

	do
	{
		which=HandleMenu(&SwitchItems,&SwitchMenu[0],DrawAllSwitchLights);

		switch (which)
		{
			case	SW_LIGHTING:
				gamestate.flags ^= GS_LIGHTING;
				ShootSnd();
				DrawSwitchMenu();
			break;

			case	SW_REBA_ATTACK_INFO:
				gamestate.flags ^= GS_ATTACK_INFOAREA;
				ShootSnd();
				DrawSwitchMenu();
			break;

			case	SW_CEILING:
				gamestate.flags ^= GS_DRAW_CEILING;
				ShootSnd();
				DrawSwitchMenu();
			break;

			case	SW_FLOORS:
				gamestate.flags ^= GS_DRAW_FLOOR;
				ShootSnd();
				DrawSwitchMenu();
			break;

            // BBi
            case SW_NO_WALL_HIT_SOUND:
                g_no_wall_hit_sound = !g_no_wall_hit_sound;
                ShootSnd();
                DrawSwitchMenu();
                break;

            case SW_MODERN_CONTROLS:
                in_use_modern_bindings = !in_use_modern_bindings;
                ShootSnd();
                DrawSwitchMenu();
                break;

            case SW_ALWAYS_RUN:
                g_always_run = !g_always_run;
                ShootSnd();
                DrawSwitchMenu();
                break;
		}

	} while(which>=0);

	MenuFadeOut();
}

//--------------------------------------------------------------------------
// DrawSwitchMenu(void)
//--------------------------------------------------------------------------
void DrawSwitchMenu(void)
{
	CA_CacheScreen (BACKGROUND_SCREENPIC);

	ClearMScreen();
	DrawMenuTitle("GAME SWITCHES");
	DrawInstructions(IT_STANDARD);

	fontnumber = 2;

	DrawMenu(&SwitchItems,&SwitchMenu[0]);
	DrawAllSwitchLights(SwitchItems.curpos);

	VW_UpdateScreen();
}

//--------------------------------------------------------------------------
// DrawAllSwitchLights()
//--------------------------------------------------------------------------
void DrawAllSwitchLights(Sint16 which)
{
	Sint16 i;
	Uint16 Shape;

	for (i=0;i<SwitchItems.amount;i++)
		if (SwitchMenu[i].string[0])
		{
			Shape = C_NOTSELECTEDPIC;

			//
			// DRAW SELECTED/NOT SELECTED GRAPHIC BUTTONS
			//

			if (SwitchItems.cursor.on)
				if (i == which)			// Is the cursor sitting on this pic?
					Shape +=2;

			switch(i)
			{
				case SW_LIGHTING:
					if (gamestate.flags & GS_LIGHTING)
						Shape++;
				break;

				case SW_REBA_ATTACK_INFO:
					if (gamestate.flags & GS_ATTACK_INFOAREA)
						Shape++;
				break;

				case SW_CEILING:
					if (gamestate.flags & GS_DRAW_CEILING)
						Shape++;
				break;

				case SW_FLOORS:
					if (gamestate.flags & GS_DRAW_FLOOR)
						Shape++;
				break;

                // BBi
                case SW_NO_WALL_HIT_SOUND:
                    if (g_no_wall_hit_sound)
                        ++Shape;
                    break;

                case SW_MODERN_CONTROLS:
                    if (in_use_modern_bindings)
                        ++Shape;
                    break;

                case SW_ALWAYS_RUN:
                    if (g_always_run)
                        ++Shape;
                    break;
			}

			VWB_DrawPic(SwitchItems.x-16,SwitchItems.y+i*SwitchItems.y_spacing-1,Shape);
		}

	DrawSwitchDescription(which);

}


//--------------------------------------------------------------------------
//  DrawSwitchDescription()
//--------------------------------------------------------------------------
void DrawSwitchDescription(Sint16 which)
{
	#define DESCRIPTIONS_Y_POS		134

    const char *instr[] = {
        "TOGGLES LIGHT SOURCING IN HALLWAYS",
        "TOGGLES DETAILED ATTACKER INFO",
        "TOGGLES CEILING MAPPING",
        "TOGGLES FLOOR MAPPING",

        // BBi
        "TOGGLES WALL HIT SOUND",
        "TOGGLES BETWEEN CLASSIC AND MODERN CONTROLS",
        "TOGGLES ALWAYS RUN MODE"
    };

	fontnumber = 2;

	WindowX = 48;
	WindowY = DESCRIPTIONS_Y_POS;
	WindowW = 236;
	WindowH = 8;

	VWB_Bar(WindowX,WindowY-1,WindowW,WindowH,TERM_BACK_COLOR);

	SETFONTCOLOR(TERM_SHADOW_COLOR,TERM_BACK_COLOR);
	US_PrintCentered(instr[which]);

	WindowX--;
	WindowY--;

	SETFONTCOLOR(INSTRUCTIONS_TEXT_COLOR,TERM_BACK_COLOR);
	US_PrintCentered(instr[which]);
}


//--------------------------------------------------------------------------
// HANDLE SOUND MENU
//--------------------------------------------------------------------------
void CP_Sound(Sint16)
{
	Sint16 which;

	CA_CacheScreen (BACKGROUND_SCREENPIC);
 DrawSoundMenu();
 MenuFadeIn();
 WaitKeyUp();

 do
 {
  which=HandleMenu(&SndItems,&SndMenu[0],DrawAllSoundLights);
  //
  // HANDLE MENU CHOICES
  //
  switch(which)
  {
	//
	// SOUND EFFECTS / DIGITIZED SOUND
	//
	case 0:
	 if (SoundMode!=sdm_Off)
	 {
	  SD_WaitSoundDone();
	  SD_SetSoundMode(sdm_Off);
	  SD_SetDigiDevice(sds_Off);
	  DrawSoundMenu();
	 }
	 break;

	case 1:
	  if (SoundMode!=sdm_AdLib)
	  {
		SD_WaitSoundDone();
		SD_SetSoundMode(sdm_AdLib);
        SD_SetDigiDevice(sds_SoundBlaster);
		CA_LoadAllSounds();
		DrawSoundMenu();
		ShootSnd();
	  }
	  break;

	//
	// MUSIC
	//
	case 4:
	  if (MusicMode!=smm_Off)
	  {
		SD_SetMusicMode(smm_Off);
		DrawSoundMenu();
		ShootSnd();
	  }
	  break;

	case 5:
	  if (MusicMode!=smm_AdLib)
	  {
		SD_SetMusicMode(smm_AdLib);
		DrawSoundMenu();
		ShootSnd();
		StartCPMusic(MENUSONG);
	  }
	  break;
  }
 } while(which>=0);

 MenuFadeOut();
}

//--------------------------------------------------------------------------
// DrawSoundMenu() - DRAW THE SOUND MENU
//--------------------------------------------------------------------------
void DrawSoundMenu(void)
{
	//
	// DRAW SOUND MENU
	//

	ClearMScreen();
	DrawMenuTitle("SOUND SETTINGS");
	DrawInstructions(IT_STANDARD);

	//
	// IF NO ADLIB, NON-CHOOSENESS!
	//

	if (!AdLibPresent && !SoundBlasterPresent)
	{
		SndMenu[1].active=SndMenu[5].active=AT_DISABLED;
	}

	fontnumber = 4;

	SETFONTCOLOR(DISABLED_TEXT_COLOR,TERM_BACK_COLOR);
	ShadowPrint("SOUND EFFECTS",105,72);
	ShadowPrint("BACKGROUND MUSIC",105,100);

	fontnumber = 2;
	DrawMenu(&SndItems,&SndMenu[0]);


	DrawAllSoundLights(SndItems.curpos);

	VW_UpdateScreen();

}

//--------------------------------------------------------------------------
// DrawAllSoundLights()
//--------------------------------------------------------------------------
void DrawAllSoundLights(Sint16 which)
{
	Sint16 i;
	Uint16 Shape;

	for (i=0;i<SndItems.amount;i++)
		if (SndMenu[i].string[0])
		{
			Shape = C_NOTSELECTEDPIC;

	 		//
	 		// DRAW SELECTED/NOT SELECTED GRAPHIC BUTTONS
	 		//

			if (SndItems.cursor.on)
			   if (i == which)			// Is the cursor sitting on this pic?
			   	Shape +=2;

			switch(i)
			{
				//
			  	// SOUND EFFECTS / DIGITIZED SOUND
				//
				case 0: if (SoundMode==sdm_Off) Shape++; break;
				case 1: if (SoundMode==sdm_AdLib) Shape++; break;

				//
				// MUSIC
				//
				case 4: if (MusicMode==smm_Off) Shape++; break;
				case 5: if (MusicMode==smm_AdLib) Shape++; break;
			}

			VWB_DrawPic(SndItems.x-16,SndItems.y+i*SndItems.y_spacing-1,Shape);
		}
}

char LOADSAVE_GAME_MSG[2][25]={"^ST1^CELoading Game\r^XX",
											  "^ST1^CESaving Game\r^XX"};

extern char LS_current,LS_total;

//--------------------------------------------------------------------------
// DrawLSAction() - DRAW LOAD/SAVE IN PROGRESS
//--------------------------------------------------------------------------
void DrawLSAction(Sint16 which)
{
	char total[]={19,19};

   VW_FadeOut();
   screenfaded = true;
	DrawTopInfo(static_cast<sp_type>(sp_loading+which));
	DrawPlayBorder();
	DisplayPrepingMsg(LOADSAVE_GAME_MSG[which]);

	if (which)
		PreloadUpdate(1,1);	// GFX: bar is full when saving...

	LS_current=1;
	LS_total=total[which];
	WindowY = 181;
}

//--------------------------------------------------------------------------
// CP_LoadGame() - LOAD SAVED GAMES
//--------------------------------------------------------------------------
Sint16 CP_LoadGame(
    Sint16 quick)
{
    Sint16 which;
    Sint16 exit = 0;

    //
    // QUICKLOAD?
    //
    if (quick) {
        which = LSItems.curpos;

        if (SaveGamesAvail[which]) {
            std::string name = SAVE_BASE_NAME;
            name += static_cast<char>('0' + which);
            MakeDestPath(name.c_str());
            bstone::FileStream handle(tempPath);
            DrawLSAction(0); // Testing...
            if ((loadedgame = ::LoadTheGame(&handle)) == 0)
                LS_current = -1; // clean up

            return loadedgame;
        }
    }

restart:

    DrawLoadSaveScreen(0);

    do {
        which = HandleMenu(&LSItems, &LSMenu[0], TrackWhichGame);

        if (which >= 0 && SaveGamesAvail[which]) {
            ShootSnd();

            std::string name = SAVE_BASE_NAME;
            name += static_cast<char>('0' + which);

            MakeDestPath(name.c_str());

            bstone::FileStream handle(tempPath);

            DrawLSAction(0);

            if (!::LoadTheGame(&handle)) {
                exit = 0;
                StartGame = 0;
                loadedgame = 0;
                LS_current = -1; // Clean up
                goto restart;
            }

            loadedgame = true;
            StartGame= true;

            ::ShootSnd();

            //
            // CHANGE "READ THIS!" TO NORMAL COLOR
            //
            MainMenu[MM_READ_THIS].active = AT_ENABLED;
            exit = 1;
            break;
        }
    } while (which >= 0);

    if (which == -1)
        MenuFadeOut();

    if (loadedgame)
        refresh_screen = false;

    return exit;
}

///////////////////////////////////
//
// HIGHLIGHT CURRENT SELECTED ENTRY
//
void TrackWhichGame(Sint16 w)
{
	static Sint16 lastgameon=0;

	PrintLSEntry(lastgameon,ENABLED_TEXT_COLOR);
	PrintLSEntry(w,HIGHLIGHT_TEXT_COLOR);

	lastgameon=w;
}

//--------------------------------------------------------------------------
// DRAW THE LOAD/SAVE SCREEN
//--------------------------------------------------------------------------
void DrawLoadSaveScreen(Sint16 loadsave)
{
	#define DISKX	100
	#define DISKY	0

	Sint16 i;

	CA_CacheScreen (BACKGROUND_SCREENPIC);
	ClearMScreen();

	fontnumber=1;

	if (!loadsave)
		DrawMenuTitle("Load Mission");
	else
		DrawMenuTitle("Save Mission");

	DrawInstructions(IT_STANDARD);

	for (i=0;i<10;i++)
		PrintLSEntry(i,ENABLED_TEXT_COLOR);

	fontnumber = 4;
	DrawMenu(&LSItems,&LSMenu[0]);

	VW_UpdateScreen();
	MenuFadeIn();
	WaitKeyUp();
}

//--------------------------------------------------------------------------
// PRINT LOAD/SAVE GAME ENTRY W/BOX OUTLINE
//--------------------------------------------------------------------------
void PrintLSEntry(Sint16 w,Sint16 color)
{
	SETFONTCOLOR(color,BKGDCOLOR);
	DrawOutline(LSM_X+LSItems.indent,LSM_Y+w*LSItems.y_spacing-2,LSM_W-LSItems.indent,8,color,color);

	fontnumber=2;

	PrintX=LSM_X+LSItems.indent+2;
	PrintY=LSM_Y+w*LSItems.y_spacing;

	if (SaveGamesAvail[w])
		US_Print(SaveGameNames[w]);
	else
		US_Print("       ----- EMPTY -----");

	fontnumber=1;
}

//--------------------------------------------------------------------------
// SAVE CURRENT GAME
//--------------------------------------------------------------------------
Sint16 CP_SaveGame(
    Sint16 quick)
{
    Sint16 which,exit=0;
    char input[GAME_DESCRIPTION_LEN+1];
    bool temp_caps = allcaps;
    US_CursorStruct TermCursor = {'@',0,HIGHLIGHT_TEXT_COLOR,2};

    allcaps = true;
    use_custom_cursor = true;
    US_CustomCursor = TermCursor;

    //
    // QUICKSAVE?
    //
    if (quick)
    {
        which=LSItems.curpos;

        if (SaveGamesAvail[which])
        {
            DrawLSAction(1); // Testing...
            std::string name = SAVE_BASE_NAME;
            name += static_cast<char>('0' + which);
            MakeDestPath(name.c_str());

            bstone::FileStream stream(tempPath, bstone::STREAM_OPEN_WRITE);

            SaveTheGame(&stream, &SaveGameNames[which][0]);

            return 1;
        }
    }

    DrawLoadSaveScreen(1);

    do
    {
        which=HandleMenu(&LSItems,&LSMenu[0],TrackWhichGame);
        if (which>=0)
        {
            //
            // OVERWRITE EXISTING SAVEGAME?
            //
            if (SaveGamesAvail[which])
            {
                if (!Confirm(GAMESVD))
                {
                    DrawLoadSaveScreen(1);
                    continue;
                }
                else
                {
                    DrawLoadSaveScreen(1);
                    PrintLSEntry(which,HIGHLIGHT_TEXT_COLOR);
                    VW_UpdateScreen();
                }
            }

            ShootSnd();

            strcpy(input,&SaveGameNames[which][0]);

            std::string name = SAVE_BASE_NAME;
            name += static_cast<char>('0' + which);

            fontnumber=2;
            VWB_Bar(LSM_X+LSItems.indent+1,LSM_Y+which*LSItems.y_spacing-1,LSM_W-LSItems.indent-1,7,HIGHLIGHT_BOX_COLOR);
            SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,HIGHLIGHT_BOX_COLOR);
            VW_UpdateScreen();


            if (US_LineInput(LSM_X+LSItems.indent+2,LSM_Y+which*LSItems.y_spacing,input,input,true,GAME_DESCRIPTION_LEN,LSM_W-LSItems.indent-10))
            {
                SaveGamesAvail[which]=1;
                strcpy(&SaveGameNames[which][0],input);

                MakeDestPath(name.c_str());
                bstone::FileStream stream(tempPath, bstone::STREAM_OPEN_WRITE);

                DrawLSAction(1);
                SaveTheGame(&stream,input);

                ShootSnd();
                exit=1;
            }
            else
            {
                VWB_Bar(LSM_X+LSItems.indent+1,LSM_Y+which*LSItems.y_spacing-1,LSM_W-LSItems.indent-1,7,TERM_BACK_COLOR);
                PrintLSEntry(which,HIGHLIGHT_TEXT_COLOR);
                VW_UpdateScreen();
                ::sd_play_player_sound(ESCPRESSEDSND, bstone::AC_ITEM);
                continue;
            }

            fontnumber=1;
            break;
        }

    } while(which>=0);

    MenuFadeOut();
    use_custom_cursor = false;
    allcaps = temp_caps;
    return exit;
}

//--------------------------------------------------------------------------
// EXIT OPTIONS
//--------------------------------------------------------------------------
void CP_ExitOptions(Sint16)
{
	StartGame=1;
}

//--------------------------------------------------------------------------
// DEFINE CONTROLS
//--------------------------------------------------------------------------
void CP_Control(Sint16)
{

	enum {MOUSEENABLE,JOYENABLE,USEPORT2,PADENABLE,CALIBRATEJOY,MOUSESENS,CUSTOMIZE};	 

 	Sint16 which;

	CA_CacheScreen (BACKGROUND_SCREENPIC);

 DrawCtlScreen();
 MenuFadeIn();
 WaitKeyUp();

 do
 {
  which=HandleMenu(&CtlItems,&CtlMenu[0],NULL);
  switch(which)
  {
   case MOUSEENABLE:
     mouseenabled^=1;

     DrawCtlScreen();
     CusItems.curpos=-1;
	 ShootSnd();
     break;

   case JOYENABLE:
     joystickenabled^=1;
	  if (joystickenabled)
		  CalibrateJoystick();		 
     DrawCtlScreen();
     CusItems.curpos=-1;
     ShootSnd();
     break;

	case USEPORT2:
	  joystickport^=1;
	  DrawCtlScreen();
	  ShootSnd();
	  break;

	case PADENABLE:
	  joypadenabled^=1;
	  DrawCtlScreen();
	  ShootSnd();
	  break;

	case CALIBRATEJOY:
		CalibrateJoystick();
		DrawCtlScreen();
	break;


	case MOUSESENS:
	case CUSTOMIZE:
	  DrawCtlScreen();
	  MenuFadeIn();
	  WaitKeyUp();
	  break;
  }
 } while(which>=0);

 MenuFadeOut();
}

//--------------------------------------------------------------------------
// DRAW MOUSE SENSITIVITY SCREEN
//--------------------------------------------------------------------------
void DrawMousePos(void)
{
	VWB_Bar(74,92,160,8,HIGHLIGHT_BOX_COLOR);
	DrawOutline(73,91,161,9,ENABLED_TEXT_COLOR,ENABLED_TEXT_COLOR);
	VWB_Bar(74+160/10*mouseadjustment,92,16,8,HIGHLIGHT_TEXT_COLOR);
}

void DrawMouseSens(void)
{
	ClearMScreen();
	DrawMenuTitle("MOUSE SENSITIVITY");
	DrawInstructions(IT_MOUSE_SEN);

	fontnumber = 4;

	SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);
	PrintX=36;
	PrintY=91;
	US_Print("SLOW");
	PrintX=242;
	US_Print("FAST");

	DrawMousePos();

	VW_UpdateScreen();
	MenuFadeIn();
}

//--------------------------------------------------------------------------
// CalibrateJoystick()
//--------------------------------------------------------------------------
void CalibrateJoystick(void)
{
	Uint16 minx,maxx,miny,maxy;

	CacheMessage(CALJOY1_TEXT);		 
	VW_UpdateScreen();

	while (IN_GetJoyButtonsDB(joystickport));
	while ((LastScan != sc_escape) && !IN_GetJoyButtonsDB(joystickport));
	if (LastScan == sc_escape)
		return;

	IN_GetJoyAbs(joystickport,&minx,&miny);
	while (IN_GetJoyButtonsDB(joystickport));

	CacheMessage(CALJOY2_TEXT);			  
	VW_UpdateScreen();

	while ((LastScan != sc_escape) && !IN_GetJoyButtonsDB(joystickport));
	if (LastScan == sc_escape)
		return;

	IN_GetJoyAbs(joystickport,&maxx,&maxy);
	if ((minx == maxx) || (miny == maxy))
		return;

	IN_SetupJoy(joystickport,minx,maxx,miny,maxy);
	while (IN_GetJoyButtonsDB(joystickport));

	IN_ClearKeysDown();
	JoystickCalibrated = true;
}


//--------------------------------------------------------------------------
// ADJUST MOUSE SENSITIVITY
//--------------------------------------------------------------------------
void MouseSensitivity(Sint16)
{
	ControlInfo ci;
	Sint16 exit=0,oldMA;

	oldMA=mouseadjustment;
	DrawMouseSens();
	do
	{
		ReadAnyControl(&ci);
		switch(ci.dir)
		{
			case dir_North:
   		case dir_West:
     			if (mouseadjustment)
				{
					mouseadjustment--;
	   			DrawMousePos();
      			VW_UpdateScreen();
                ::sd_play_player_sound(MOVEGUN1SND, bstone::AC_ITEM);

                while (Keyboard[sc_left_arrow])
                    ::in_handle_events();

      			WaitKeyUp();
     			}
     		break;

   		case dir_South:
   		case dir_East:
				if (mouseadjustment<9)
				{
					mouseadjustment++;
					DrawMousePos();
					VW_UpdateScreen();
                    ::sd_play_player_sound(MOVEGUN1SND, bstone::AC_ITEM);

                    while (Keyboard[sc_right_arrow])
                        ::in_handle_events();

					WaitKeyUp();
				}
			break;

        default:
            break;
		}

		if (ci.button0 || Keyboard[sc_space] || Keyboard[sc_return])
			exit=1;
		else
		if (ci.button1 || Keyboard[sc_escape])
			exit=2;

	} while(!exit);

	if (exit==2)
	{
		mouseadjustment=oldMA;
        ::sd_play_player_sound(ESCPRESSEDSND, bstone::AC_ITEM);
	}
	else
        ::sd_play_player_sound(SHOOTSND, bstone::AC_ITEM);

	WaitKeyUp();
	MenuFadeOut();
}

//--------------------------------------------------------------------------
// DrawCtlScreen() - DRAW CONTROL MENU SCREEN
//--------------------------------------------------------------------------
void DrawCtlScreen(void)
{
	#define Y_CTL_PIC_OFS	(3)

	Sint16 i,x,y;

	ClearMScreen();
	DrawMenuTitle("CONTROL");
	DrawInstructions(IT_STANDARD);

	WindowX=0;
	WindowW=320;
	SETFONTCOLOR(TEXTCOLOR,BKGDCOLOR);

	if (JoysPresent[0])
		CtlMenu[1].active=
		CtlMenu[2].active=
		CtlMenu[3].active=
		CtlMenu[4].active=AT_ENABLED;

	CtlMenu[2].active=CtlMenu[3].active=CtlMenu[4].active=static_cast<activetypes>(joystickenabled);

	if (MousePresent)
	{
		CtlMenu[5].active=
		CtlMenu[0].active=AT_ENABLED;
	}

	CtlMenu[5].active=static_cast<activetypes>(mouseenabled);

	fontnumber = 4;
	DrawMenu(&CtlItems,&CtlMenu[0]);

	x=CTL_X+CtlItems.indent-24;
	y=CTL_Y+Y_CTL_PIC_OFS;
	if (mouseenabled)
		VWB_DrawPic(x,y,C_SELECTEDPIC);
	else
		VWB_DrawPic(x,y,C_NOTSELECTEDPIC);

	y=CTL_Y+9+Y_CTL_PIC_OFS;
	if (joystickenabled)
		VWB_DrawPic(x,y,C_SELECTEDPIC);
	else
		VWB_DrawPic(x,y,C_NOTSELECTEDPIC);

	y=CTL_Y+9*2+Y_CTL_PIC_OFS;
	if (joystickport)
		VWB_DrawPic(x,y,C_SELECTEDPIC);
	else
		VWB_DrawPic(x,y,C_NOTSELECTEDPIC);

	y=CTL_Y+9*3+Y_CTL_PIC_OFS;
	if (joypadenabled)
		VWB_DrawPic(x,y,C_SELECTEDPIC);
	else
		VWB_DrawPic(x,y,C_NOTSELECTEDPIC);

	//
	// PICK FIRST AVAILABLE SPOT
	//

	if (CtlItems.curpos<0 || !CtlMenu[static_cast<int>(CtlItems.curpos)].active)

        // BBi
		//for (i=0;i<6;i++)
        for (i = 0; i < CtlItems.amount; ++i)
			if (CtlMenu[i].active)
			{
				CtlItems.curpos=static_cast<char>(i);
				break;
			}

	DrawMenuGun(&CtlItems);
	VW_UpdateScreen();
}

enum ControlButton1 {
    FIRE,
    STRAFE,
    RUN,
    OPEN
}; // enum ControlButton1

char mbarray[4][3]={"B0","B1","B2","B3"},
		order[4]={RUN,OPEN,FIRE,STRAFE};

//--------------------------------------------------------------------------
// CustomControls() CUSTOMIZE CONTROLS
//--------------------------------------------------------------------------
void CustomControls(Sint16)
{
    if (in_use_modern_bindings) {
        binds_draw_menu();
        return;
    }

	Sint16 which;

 DrawCustomScreen();

 do
 {
  which=HandleMenu(&CusItems,&CusMenu[0],FixupCustom);

  switch(which)
  {
	case 0:
	  DefineMouseBtns();
	  DrawCustMouse(1);
	  break;

	case 2:
	  DefineJoyBtns();
	  DrawCustJoy(0);
	  break;

	case 4:
	  DefineKeyBtns();
	  DrawCustKeybd(0);
	  break;

	case 5:
	  DefineKeyMove();
	  DrawCustKeys(0);
  }
 } while(which>=0);



 MenuFadeOut();
}

//--------------------------------------------------------------------------
// DEFINE THE MOUSE BUTTONS
//--------------------------------------------------------------------------
void DefineMouseBtns(void)
{
	CustomCtrls mouseallowed={1,1,1,1};
	EnterCtrlData(2,&mouseallowed,DrawCustMouse,PrintCustMouse,MOUSE);
}

//--------------------------------------------------------------------------
// DEFINE THE JOYSTICK BUTTONS
//--------------------------------------------------------------------------
void DefineJoyBtns(void)
{
	CustomCtrls joyallowed={1,1,1,1};
	EnterCtrlData(5,&joyallowed,DrawCustJoy,PrintCustJoy,JOYSTICK);
}

//--------------------------------------------------------------------------
// DEFINE THE KEYBOARD BUTTONS
//--------------------------------------------------------------------------
void DefineKeyBtns(void)
{
	CustomCtrls keyallowed={1,1,1,1};
	EnterCtrlData(8,&keyallowed,DrawCustKeybd,PrintCustKeybd,KEYBOARDBTNS);
}

//--------------------------------------------------------------------------
// DEFINE THE KEYBOARD BUTTONS
//--------------------------------------------------------------------------
void DefineKeyMove(void)
{
	CustomCtrls keyallowed={1,1,1,1};
	EnterCtrlData(10,&keyallowed,DrawCustKeys,PrintCustKeys,KEYBOARDMOVE);
}

//--------------------------------------------------------------------------
// TestForValidKey
//--------------------------------------------------------------------------
bool TestForValidKey(ScanCode Scan)
{
	char *pos;

    pos = static_cast<char*>(memchr(buttonscan,Scan,sizeof(buttonscan)));

	if (!pos)
  		pos = static_cast<char*>(memchr(dirscan,Scan,sizeof(dirscan)));

	if (pos)
   {
   	*pos = sc_none;
        ::sd_play_player_sound(SHOOTDOORSND, bstone::AC_ITEM);

		DrawCustomScreen();
   }

	return(!((pos > 0) ? true : false));
}


//--------------------------------------------------------------------------
// EnterCtrlData() - ENTER CONTROL DATA FOR ANY TYPE OF CONTROL
//--------------------------------------------------------------------------

enum ControlButton2 {
    FWRD,
    RIGHT,
    BKWD,
    LEFT
}; // enum ControlButton2

Sint16 moveorder[4]={LEFT,RIGHT,FWRD,BKWD};

void EnterCtrlData(Sint16 index,CustomCtrls *cust,void (*DrawRtn)(Sint16),void (*PrintRtn)(Sint16),Sint16 type)
{
	Sint16 j,exit,tick,redraw,which = 0,x = 0,picked;
	ControlInfo ci;
  	bool clean_display = true;

	ShootSnd();
	PrintY=CST_Y+13*index;
	IN_ClearKeysDown();
	exit=0;
	redraw=1;

	CA_CacheGrChunk(STARTFONT+fontnumber);

	//
	// FIND FIRST SPOT IN ALLOWED ARRAY
	//
	for (j=0;j<4;j++)
		if (cust->allowed[j])
		{
			 which=j;
			 break;
		}

	do
	{
	  if (redraw)
	  {
	  		x=CST_START+CST_SPC*which;
	  		DrawRtn(1);

			VWB_Bar(x-1,PrintY-1,CST_SPC,7,HIGHLIGHT_BOX_COLOR);
			SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,HIGHLIGHT_BOX_COLOR);
			PrintRtn(which);
   		PrintX=x;
   		SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);
			VW_UpdateScreen();
   		WaitKeyUp();
   		redraw=0;
		}

		ReadAnyControl(&ci);

  		if (type==MOUSE || type==JOYSTICK)
	 		if (IN_KeyDown(sc_return)||IN_KeyDown(sc_control)||IN_KeyDown(sc_alt))
			{
		   	 IN_ClearKeysDown();
				 ci.button0=ci.button1=false;
		   }

	  //
	  // CHANGE BUTTON VALUE?
	  //

	  if ((ci.button0|ci.button1|ci.button2|ci.button3)||
   	  ((type==KEYBOARDBTNS||type==KEYBOARDMOVE) && LastScan==sc_return))
	  {
   		tick=0;
        TimeCount=0;
        picked=0;
			SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,HIGHLIGHT_BOX_COLOR);

		   do
		   {
			   Sint16 button,result=0;

			   if (type==KEYBOARDBTNS||type==KEYBOARDMOVE)
			      IN_ClearKeysDown();

               // BBi
               ::in_handle_events();

			   //
			   // FLASH CURSOR
			   //

				if (TimeCount>10)
			   {
			   	switch(tick)
			     	{
				   	case 0:
							VWB_Bar(x-1,PrintY-1,CST_SPC,7,HIGHLIGHT_BOX_COLOR);
						break;

				      case 1:
							PrintX=x;
							US_Print("?");

                            ::sd_play_player_sound(
                                HITWALLSND, bstone::AC_ITEM);
				  	}

			     	tick^=1;
			     	TimeCount=0;
			     	VW_UpdateScreen();
			   }

		    //
		    // WHICH TYPE OF INPUT DO WE PROCESS?
			 //

		    switch(type)
		    {
				 case MOUSE:
                     button = ::IN_MouseButtons();

		   	    switch(button)
			       {
						case 1: result=1; break;
						case 2: result=2; break;
						case 4: result=3; break;
			       }

			       if (result)
			       {
						Sint16 z;

						for (z=0;z<4;z++)
						  if (order[which]==buttonmouse[z])
						  {
							   buttonmouse[z]=bt_nobutton;
	   						break;
						  }

						buttonmouse[result-1]=order[which];
						picked=1;

                        ::sd_play_player_sound(
                            SHOOTDOORSND, bstone::AC_ITEM);

						clean_display = false;
		 	       }
   	    	 break;

	     case JOYSTICK:
   	    if (ci.button0) result=1;
      	 else
	       if (ci.button1) result=2;
   	    else
	       if (ci.button2) result=3;
			 else
      	 if (ci.button3) result=4;

	       if (result)
			 {
				Sint16 z;

				for (z=0;z<4;z++)
				  if (order[which]==buttonjoy[z])
				  {
					   buttonjoy[z]=bt_nobutton;
					   break;
				  }

				buttonjoy[result-1]=order[which];
				picked=1;

                ::sd_play_player_sound(SHOOTDOORSND, bstone::AC_ITEM);

				clean_display = false;
				}
	  	  break;

   	  case KEYBOARDBTNS:
	       if (LastScan)
   	    {
          	if (LastScan == sc_escape)
            	break;

	   	  	if (memchr(special_keys,LastScan,sizeof(special_keys)))
                ::sd_play_player_sound(NOWAYSND, bstone::AC_ITEM);

				else
   	      {
              clean_display = TestForValidKey(LastScan);

      	   	if (clean_display)
						ShootSnd();

					buttonscan[static_cast<int>(order[which])]=LastScan;
					picked=1;
      	   }
				IN_ClearKeysDown();
   	    }
	     break;


	     case KEYBOARDMOVE:
	       if (LastScan)
	       {
	        	if (LastScan == sc_escape)
   	        	break;

		     	if (memchr(special_keys,LastScan,sizeof(special_keys)))
                ::sd_play_player_sound(NOWAYSND, bstone::AC_ITEM);
				else
	         {
                 clean_display = TestForValidKey(LastScan);

   	      	if (clean_display)
						ShootSnd();

					dirscan[moveorder[which]]=LastScan;
					picked=1;
      	   }
				IN_ClearKeysDown();
			 }
	     break;
	    }


	    //
   	 // EXIT INPUT?
	    //

		 if (IN_KeyDown(sc_escape))
   	 {
	   	  picked=1;
		     continue;
		 }

   } while(!picked);

   if (!clean_display)
     	DrawCustomScreen();

	SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);
   redraw=1;
   WaitKeyUp();
	continue;
  }

  if (ci.button1 || IN_KeyDown(sc_escape))
    exit=1;

  //
  // MOVE TO ANOTHER SPOT?
  //
  switch(ci.dir)
  {

   case dir_West:
		VWB_Bar(x-1,PrintY-1,CST_SPC,7,TERM_BACK_COLOR);
		SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);
  		PrintRtn(which);
     	do
     	{
      	which--;
	      if (which<0)
				which=3;
     	} while(!cust->allowed[which]);

     redraw=1;

     ::sd_play_player_sound(MOVEGUN1SND, bstone::AC_ITEM);

	  while(ReadAnyControl(&ci),ci.dir!=dir_None);
	  IN_ClearKeysDown();
	break;



	case dir_East:
		VWB_Bar(x-1,PrintY-1,CST_SPC,7,TERM_BACK_COLOR);
		SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);
		PrintRtn(which);
		do
		{
			which++;
			if (which>3)
				which=0;
		} while(!cust->allowed[which]);

		redraw=1;

        ::sd_play_player_sound(MOVEGUN1SND, bstone::AC_ITEM);

        // ISG --> while loop with empty body:
		//while(ReadAnyControl(&ci),ci.dir!=dir_None);

	   IN_ClearKeysDown();
   break;

	case dir_North:
   case dir_South:
	  exit=1;

    default:
        break;
  }

 } while(!exit);

	FREEFONT(STARTFONT+fontnumber);

    ::sd_play_player_sound(ESCPRESSEDSND, bstone::AC_ITEM);

 WaitKeyUp();
}


//--------------------------------------------------------------------------
// FixupCustom() - FIXUP GUN CURSOR OVERDRAW SHIT
//--------------------------------------------------------------------------
void FixupCustom(Sint16 w)
{
	static Sint16 lastwhich=-1;

	switch(w)
	{
		case 0: DrawCustMouse(1); break;
  		case 2: DrawCustJoy(1); break;
  		case 4: DrawCustKeybd(1); break;
  		case 5: DrawCustKeys(1);
	}


	if (lastwhich>=0)
	{
		if (lastwhich!=w)
			switch(lastwhich)
    		{
	  			case 0: DrawCustMouse(0); break;
     			case 2: DrawCustJoy(0); break;
     			case 4: DrawCustKeybd(0); break;
     			case 5: DrawCustKeys(0);
    		}
	}

	lastwhich=w;
}


//---------------------------------------------------------------------------
// DrawCustomScreen() - DRAW CUSTOMIZE SCREEN
//---------------------------------------------------------------------------
void DrawCustomScreen(void)
{
	Sint16 i;

	ClearMScreen();
	DrawMenuTitle("CUSTOMIZE");
	DrawInstructions(IT_STANDARD);

	//
	// MOUSE
	//

	WindowX=32;
	WindowW=244;

	fontnumber = 4;

	SETFONTCOLOR(0x0C,TERM_BACK_COLOR);


	PrintY=49;
	US_CPrint("MOUSE\n");
	PrintY=79;
	US_CPrint("JOYSTICK/GRAVIS GAMEPAD\n");
	PrintY=109;
	US_CPrint("KEYBOARD\n");

	fontnumber = 2;

	SETFONTCOLOR(DISABLED_TEXT_COLOR,TERM_BACK_COLOR);

   for (i=60;i<=120;i+=30)
   {
		ShadowPrint("RUN",CST_START,i);
		ShadowPrint("OPEN",CST_START+CST_SPC*1,i);
		ShadowPrint("FIRE",CST_START+CST_SPC*2,i);
		ShadowPrint("STRAFE",CST_START+CST_SPC*3,i);
	}

	ShadowPrint("LEFT",CST_START,135);
	ShadowPrint("RIGHT",CST_START+CST_SPC*1,135);
	ShadowPrint("FWRD",CST_START+CST_SPC*2,135);
	ShadowPrint("BKWRD",CST_START+CST_SPC*3,135);


	DrawCustMouse(0);
	DrawCustJoy(0);
	DrawCustKeybd(0);
	DrawCustKeys(0);

	//
	// PICK STARTING POINT IN MENU
	//
	if (CusItems.curpos<0)
		for (i=0;i<CusItems.amount;i++)
		  if (CusMenu[i].active)
		  {
				CusItems.curpos=static_cast<char>(i);
				break;
		  }

	VW_UpdateScreen();
	MenuFadeIn();
}


//---------------------------------------------------------------------------
// PrintCustMouse
//---------------------------------------------------------------------------
void PrintCustMouse(Sint16 i)
{
	Sint16 j;

	for (j=0;j<4;j++)
		if (order[i]==buttonmouse[j])
		{
			PrintX=CST_START+CST_SPC*i;
			US_Print(mbarray[j]);
			break;
		}
}



//---------------------------------------------------------------------------
// DrawCustMouse
//---------------------------------------------------------------------------
void DrawCustMouse(Sint16 hilight)
{
	Sint16 i,color;

	color=ENABLED_TEXT_COLOR;

	if (hilight)
		color=HIGHLIGHT_TEXT_COLOR;

	SETFONTCOLOR(color,TERM_BACK_COLOR);

	if (!mouseenabled)
	{
		SETFONTCOLOR(DISABLED_TEXT_COLOR,TERM_BACK_COLOR);
		CusMenu[0].active=AT_DISABLED;
	}
	else
		CusMenu[0].active=AT_ENABLED;

	PrintY=CST_Y+7;
	for (i=0;i<4;i++)
		PrintCustMouse(i);
}



//---------------------------------------------------------------------------
// PrintCustJoy
//---------------------------------------------------------------------------
void PrintCustJoy(Sint16 i)
{
	Sint16 j;

	for (j=0;j<4;j++)
		if (order[i]==buttonjoy[j])
		{
			PrintX=CST_START+CST_SPC*i;
			US_Print(mbarray[j]);
			break;
		}
}


//---------------------------------------------------------------------------
// DrawCustJoy
//---------------------------------------------------------------------------
void DrawCustJoy(Sint16 hilight)
{
	Sint16 i,color;


	color=ENABLED_TEXT_COLOR;
	if (hilight)
		color=HIGHLIGHT_TEXT_COLOR;

	SETFONTCOLOR(color,TERM_BACK_COLOR);

	if (!joystickenabled)
	{
		SETFONTCOLOR(DISABLED_TEXT_COLOR,TERM_BACK_COLOR);
		CusMenu[2].active=AT_DISABLED;
	}
	else
		CusMenu[2].active=AT_ENABLED;

	PrintY=CST_Y+37;
	for (i=0;i<4;i++)
		PrintCustJoy(i);
}



//---------------------------------------------------------------------------
// PrintCustKeybd
//---------------------------------------------------------------------------
void PrintCustKeybd(Sint16 i)
{
	PrintX=CST_START+CST_SPC*i;
	US_Print(reinterpret_cast<char*>(IN_GetScanName(static_cast<ScanCode>(buttonscan[static_cast<int>(order[i])]))));
}




//---------------------------------------------------------------------------
// DrawCustKeybd
//---------------------------------------------------------------------------
void DrawCustKeybd(Sint16 hilight)
{
	Sint16 i,color;

   if (hilight)
		color=HIGHLIGHT_TEXT_COLOR;
   else
 		color=ENABLED_TEXT_COLOR;

	SETFONTCOLOR(color,TERM_BACK_COLOR);

	PrintY=CST_Y+67;

	for (i=0;i<4;i++)
		PrintCustKeybd(i);
}




//---------------------------------------------------------------------------
// PrintCustKeys()
//---------------------------------------------------------------------------
void PrintCustKeys(Sint16 i)
{
	PrintX=CST_START+CST_SPC*i;
	US_Print(reinterpret_cast<char*>(IN_GetScanName(static_cast<ScanCode>(dirscan[moveorder[i]]))));
}




//---------------------------------------------------------------------------
// DrawCustKeys()
//---------------------------------------------------------------------------
void DrawCustKeys(Sint16 hilight)
{
	Sint16 i,color;

	color=ENABLED_TEXT_COLOR;

	if (hilight)
		color=HIGHLIGHT_TEXT_COLOR;

	SETFONTCOLOR(color,TERM_BACK_COLOR);

	PrintY=CST_Y+82;
	for (i=0;i<4;i++)
		PrintCustKeys(i);
}

//---------------------------------------------------------------------------
// CP_Quit() - QUIT THIS INFERNAL GAME!
//---------------------------------------------------------------------------
void CP_Quit(void)
{
	if (Confirm(QuitToDosStr))
		ExitGame();

	DrawMainMenu();
}


////////////////////////////////////////////////////////////////////
//
// SUPPORT ROUTINES
//
////////////////////////////////////////////////////////////////////



//---------------------------------------------------------------------------
// Clear Menu screens to dark red
//---------------------------------------------------------------------------
void ClearMScreen(void)
{
	VWB_Bar(SCREEN_X,SCREEN_Y,SCREEN_W,SCREEN_H,TERM_BACK_COLOR);
}



//---------------------------------------------------------------------------
// Un/Cache a LUMP of graphics
//---------------------------------------------------------------------------
void CacheLump(Sint16 lumpstart,Sint16 lumpend)
{
	Sint16 i;

	for (i=lumpstart;i<=lumpend;i++)
		CA_CacheGrChunk(i);
}



//---------------------------------------------------------------------------
// UnCacheLump
//---------------------------------------------------------------------------
void UnCacheLump(Sint16 lumpstart,Sint16 lumpend)
{
	Sint16 i;

	for (i=lumpstart;i<=lumpend;i++)
		FREEFONT(i);
}


//---------------------------------------------------------------------------
// Draw a window for a menu
//---------------------------------------------------------------------------
void DrawWindow(Sint16 x,Sint16 y,Sint16 w,Sint16 h,Sint16 wcolor)
{
	VWB_Bar(x,y,w,h,wcolor);
	DrawOutline(x,y,w,h,BORD2COLOR,DEACTIVE);
}



//---------------------------------------------------------------------------
// DrawOutline
//---------------------------------------------------------------------------
void DrawOutline(Sint16 x,Sint16 y,Sint16 w,Sint16 h,Sint16 color1,Sint16 color2)
{
	VWB_Hlin(x,x+w,y,color2);
	VWB_Vlin(y,y+h,x,color2);
	VWB_Hlin(x,x+w,y+h,color1);
	VWB_Vlin(y,y+h,x+w,color1);
}


//---------------------------------------------------------------------------
// SetupControlPanel() - Setup Control Panel stuff - graphics, etc.
//---------------------------------------------------------------------------
void SetupControlPanel(void)
{

	//
	// CACHE GRAPHICS & SOUNDS
	//
//	CA_CacheScreen (BACKGROUND_SCREENPIC);

	ControlPanelAlloc();

//	SETFONTCOLOR(TEXTCOLOR,BKGDCOLOR);
	fontnumber=2;

	WindowH=200;

	if (!ingame)
		CA_LoadAllSounds();
	else
		MainMenu[MM_SAVE_MISSION].active=AT_ENABLED;

	ReadGameNames();
}


//---------------------------------------------------------------------------
// ReadGameNames()
//---------------------------------------------------------------------------
void ReadGameNames()
{
    for (int i = 0; i < 10; ++i) {
        std::string name = SAVE_BASE_NAME;
        name += static_cast<char>('0' + i);

        ::MakeDestPath(name.c_str());

        char temp[GAME_DESCRIPTION_LEN+1];

        bstone::FileStream stream(tempPath);

        if (!stream.is_open())
            continue;

        SaveGamesAvail[i] = 1;

        int chunk_size = ::FindChunk(&stream, "DESC");

        if (chunk_size > 0) {
            stream.read(temp, chunk_size);
            ::strcpy(&SaveGameNames[i][0], temp);
        } else
            ::strcpy(&SaveGameNames[i][0], "DESCRIPTION LOST");
    }
}

//---------------------------------------------------------------------------
// CleanupControlPanel() - Clean up all the Control Panel stuff
//---------------------------------------------------------------------------
void CleanupControlPanel(void)
{
	if (!loadedgame)
		FreeMusic();
	ControlPanelFree();
	fontnumber = 4;
}

//---------------------------------------------------------------------------
// ControlPanelFree() - This FREES the control panel lump from memory
//								 and REALLOCS the ScaledDirectory
//---------------------------------------------------------------------------
void ControlPanelFree(void)
{
	UnCacheLump(CONTROLS_LUMP_START,CONTROLS_LUMP_END);
	NewViewSize();
}

//---------------------------------------------------------------------------
// ControlPanelAlloc() - This CACHEs the control panel lump into memory
//							    and FREEs the ScaledDirectory.
//---------------------------------------------------------------------------
void ControlPanelAlloc(void)
{
	CacheLump(CONTROLS_LUMP_START,CONTROLS_LUMP_END);
}

//---------------------------------------------------------------------------
// ShadowPrint() - Shadow Prints given text @ a given x & y in default font
//
// NOTE: Font MUST already be loaded
//---------------------------------------------------------------------------
void ShadowPrint(const char *strng,Sint16 x, Sint16 y)
{
	Sint16 old_bc,old_fc;

	old_fc = fontcolor;
	old_bc = backcolor;

	PrintX = x+1;
	PrintY = y+1;

	SETFONTCOLOR(TERM_SHADOW_COLOR,TERM_BACK_COLOR);
	US_Print(strng);

	PrintX = x;
	PrintY = y;
	SETFONTCOLOR(old_fc,old_bc);
	US_Print(strng);
}

//---------------------------------------------------------------------------
// HandleMenu() - Handle moving gun around a menu
//---------------------------------------------------------------------------
Sint16 HandleMenu(CP_iteminfo *item_i,CP_itemtype *items,void (*routine)(Sint16 w))
{
	#define box_on 	item_i->cursor.on
	char key;
	static Sint16 redrawitem=1;

	Sint16 i,x,y,basey,exit,which,flash_tics;
	ControlInfo ci;

	which=item_i->curpos;
	x=item_i->x;
	basey=item_i->y;
	y=basey+which*item_i->y_spacing;
	box_on = 1;
	DrawGun(item_i,items,x,&y,which,basey,routine);

	SetTextColor(items+which,1);

	if (redrawitem)
	{
		ShadowPrint((items+which)->string,item_i->x+item_i->indent,item_i->y+which*item_i->y_spacing);
	}

	//
	// CALL CUSTOM ROUTINE IF IT IS NEEDED
	//

	if (routine)
		routine(which);

	VW_UpdateScreen();

	flash_tics=40;
	exit=0;
	TimeCount=0;
	IN_ClearKeysDown();

	do
	{
		CalcTics();
		flash_tics -= tics;

		CycleColors();

		//
		// CHANGE GUN SHAPE
		//

		if (flash_tics <= 0)
		{
			flash_tics = 40;

			box_on ^= 1;

			if (box_on)
				DrawGun(item_i,items,x,&y,which,basey,routine);
			else
			{
				EraseGun(item_i,items,x,y,which);
				if (routine)
					routine(which);
			}


			VW_UpdateScreen();
		}

		CheckPause();


		//
		// SEE IF ANY KEYS ARE PRESSED FOR INITIAL CHAR FINDING
		//

		key=LastASCII;
		if (key)
		{
			Sint16 ok=0;

			if (key>='a')
				key-='a'-'A';

			for (i=which+1;i<item_i->amount;i++)
				if ((items+i)->active && (items+i)->string[0]==key)
				{
					EraseGun(item_i,items,x,y,which);
					which=i;
					item_i->curpos=static_cast<char>(which);			// jtr -testing
					box_on = 1;
					DrawGun(item_i,items,x,&y,which,basey,routine);
					VW_UpdateScreen();

					ok=1;
					IN_ClearKeysDown();
					break;
				}

			//
			// DIDN'T FIND A MATCH FIRST TIME THRU. CHECK AGAIN.
			//

			if (!ok)
			{
				for (i=0;i<which;i++)
					if ((items+i)->active && (items+i)->string[0]==key)
					{
						EraseGun(item_i,items,x,y,which);
						which=i;
						item_i->curpos=static_cast<char>(which);			// jtr -testing
						box_on = 1;
						DrawGun(item_i,items,x,&y,which,basey,routine);
						VW_UpdateScreen();

						IN_ClearKeysDown();
						break;
					}
			}
		}

		//
		// GET INPUT
		//

		ReadAnyControl(&ci);

		switch(ci.dir)
		{
			//------------------------
			// MOVE UP
			//
			case dir_North:
				EraseGun(item_i,items,x,y,which);

				do
				{
					if (!which)
						which=item_i->amount-1;
					else
						which--;

				} while(!(items+which)->active);

				item_i->curpos=static_cast<char>(which);			// jtr -testing

				box_on = 1;
				DrawGun(item_i,items,x,&y,which,basey,routine);

				VW_UpdateScreen();

				TicDelay(20);
			break;

			//--------------------------
			// MOVE DOWN
			//
			case dir_South:
				EraseGun(item_i,items,x,y,which);

				do
				{
					if (which==item_i->amount-1)
						which=0;
					else
						which++;
				} while(!(items+which)->active);

				item_i->curpos=static_cast<char>(which);			// jtr -testing

				box_on = 1;
				DrawGun(item_i,items,x,&y,which,basey,routine);

				VW_UpdateScreen();

				TicDelay(20);
			break;

			default:
                break;
		}

		if (ci.button0 ||	Keyboard[sc_space] || Keyboard[sc_return])
			exit=1;

		if (ci.button1 || Keyboard[sc_escape])
			exit=2;

	} while(!exit);

	IN_ClearKeysDown();

	//
	// ERASE EVERYTHING
	//

//	if (lastitem!=which)
//	{
		box_on = 0;
		redrawitem=1;
		EraseGun(item_i,items,x,y,which);
//	}
//	else
//		redrawitem=0;

	if (routine)
	{
		routine(which);
	}

	VW_UpdateScreen();

	item_i->curpos=static_cast<char>(which);

	switch(exit)
	{
		case 1:
			//
			// CALL THE ROUTINE
			//
			if ((items+which)->routine!=NULL)
			{
			// Make sure there's room to save when CP_SaveGame() is called.
			//
				if (reinterpret_cast<size_t>(items[which].routine) == reinterpret_cast<size_t>(CP_SaveGame))
					if (!CheckDiskSpace(DISK_SPACE_NEEDED,CANT_SAVE_GAME_TXT,cds_menu_print))
						return(which);

			 //
			 // ALREADY IN A GAME?
			 //
				if (ingame && ((items+which)->routine == CP_NewGame))
					if (!Confirm(CURGAME))
					{
						MenuFadeOut();
						return 0;
					}

				ShootSnd();
				MenuFadeOut();
				(items+which)->routine(0);
			}
			return which;

		case 2:
            ::sd_play_player_sound(ESCPRESSEDSND, bstone::AC_ITEM);

			return -1;
	}

	return 0; // JUST TO SHUT UP THE ERROR MESSAGES!
}

//---------------------------------------------------------------------------
// EraseGun() - ERASE GUN & DE-HIGHLIGHT STRING
//---------------------------------------------------------------------------
void EraseGun(CP_iteminfo *item_i,CP_itemtype *items,Sint16 x,Sint16 y,Sint16 which)
{
	VWB_Bar(item_i->cursor.x,y+item_i->cursor.y_ofs,item_i->cursor.width,item_i->cursor.height,TERM_BACK_COLOR);
	SetTextColor(items+which,0);

   ShadowPrint((items+which)->string,item_i->x+item_i->indent,y);

//	VW_UpdateScreen();

 	x++;				// Shut up compiler
}


//---------------------------------------------------------------------------
// DrawGun() - DRAW GUN AT NEW POSITION
//---------------------------------------------------------------------------
void DrawGun(CP_iteminfo *item_i,CP_itemtype *items,Sint16 x,Sint16 *y,Sint16 which,Sint16 basey,void (*routine)(Sint16 w))
{
	*y=basey+which*item_i->y_spacing;

   VWB_Bar(item_i->cursor.x,*y+item_i->cursor.y_ofs,item_i->cursor.width,item_i->cursor.height,HIGHLIGHT_BOX_COLOR);
	SetTextColor(items+which,1);

   ShadowPrint((items+which)->string,item_i->x+item_i->indent,item_i->y+which*item_i->y_spacing);

	//
	// CALL CUSTOM ROUTINE IF IT IS NEEDED
	//

	if (routine)
		routine(which);

//	VW_UpdateScreen();
//	SD_PlaySound(MOVEGUN2SND);

	x++;				// Shutup compiler
}

//---------------------------------------------------------------------------
// TicDelay() - DELAY FOR AN AMOUNT OF TICS OR UNTIL CONTROLS ARE INACTIVE
//---------------------------------------------------------------------------
void TicDelay(Sint16 count)
{
	ControlInfo ci;

	TimeCount=0;

	do	{
		ReadAnyControl(&ci);
	} while(TimeCount<static_cast<Uint32>(count) && ci.dir!=dir_None);
}

//---------------------------------------------------------------------------
// DrawMenu() - Draw a menu
//
//       This also calculates the Y position of the current items in the
//			CP_itemtype structures.
//---------------------------------------------------------------------------
void DrawMenu(CP_iteminfo *item_i,CP_itemtype *items)	 
{
	Sint16 i,which=item_i->curpos;

	WindowX=PrintX=item_i->x+item_i->indent;
	WindowY=PrintY=item_i->y;

	WindowW=320;
	WindowH=200;

	for (i=0;i<item_i->amount;i++)
	{
		SetTextColor(items+i,which==i);
		ShadowPrint((items+i)->string,WindowX,item_i->y+i*item_i->y_spacing);
	}
}

//---------------------------------------------------------------------------
// SetTextColor() - SET TEXT COLOR (HIGHLIGHT OR NO)
//---------------------------------------------------------------------------
void SetTextColor(CP_itemtype *items,Sint16 hlight)
{
	if (hlight)
	{
		SETFONTCOLOR(color_hlite[items->active],TERM_BACK_COLOR);
	}
	else
	{
		SETFONTCOLOR(color_norml[items->active],TERM_BACK_COLOR);
	}
}

//---------------------------------------------------------------------------
// WaitKeyUp() - WAIT FOR CTRLKEY-UP OR BUTTON-UP
//---------------------------------------------------------------------------
void WaitKeyUp(void)
{
	ControlInfo ci;

	while(ReadAnyControl(&ci),ci.button0 |
									  ci.button1 |
									  ci.button2 |
									  ci.button3|
									  Keyboard[sc_space]|
									  Keyboard[sc_return]|
									  Keyboard[sc_escape]);
}

//---------------------------------------------------------------------------
// ReadAnyControl() - READ KEYBOARD, JOYSTICK AND MOUSE FOR INPUT
//---------------------------------------------------------------------------
void ReadAnyControl(
    ControlInfo* ci)
{
    bool mouseactive = false;

    ::IN_ReadControl(0, ci);

    //
    // UNDO some of the ControlInfo vars that were init
    // with IN_ReadControl() for the mouse...
    //
    if (ControlTypeUsed == ctrl_Mouse) {
        //
        // Clear directions & buttons (if enabled or not)
        //
        ci->dir = dir_None;
        ci->button0 = 0;
        ci->button1 = 0;
        ci->button2 = 0;
        ci->button3 = 0;
    }

    if (mouseenabled) {
        int mousex;
        int mousey;

        // READ MOUSE MOTION COUNTERS
        // RETURN DIRECTION
        // HOME MOUSE
        // CHECK MOUSE BUTTONS

        ::in_get_mouse_deltas(mousex, mousey);
        ::in_clear_mouse_deltas();

#if 0
        if (mousey < (CENTER - SENSITIVE)) {
            mouseactive = true;
            ci->dir = dir_North;
        } else if (mousey > (CENTER + SENSITIVE)) {
            mouseactive = true;
            ci->dir = dir_South;
        }

        if (mousex < (CENTER - SENSITIVE)) {
            mouseactive = true;
            ci->dir = dir_West;
        } else if (mousex > (CENTER + SENSITIVE)) {
            mouseactive = true;
            ci->dir = dir_East;
        }

        if (mouseactive)
            ::in_set_mouse_cursor_position(CENTER, CENTER);
#endif

        const int DELTA_THRESHOLD = 10;

        if (mousey < -DELTA_THRESHOLD) {
            ci->dir = dir_North;
            mouseactive = true;
        } else if (mousey > DELTA_THRESHOLD) {
            ci->dir = dir_South;
            mouseactive = true;
        }

        if (mousex < -DELTA_THRESHOLD) {
            ci->dir = dir_West;
            mouseactive = true;
        } else if (mousex > DELTA_THRESHOLD) {
            ci->dir = dir_East;
            mouseactive = true;
        }

        int buttons = ::IN_MouseButtons();

        if (buttons != 0) {
            ci->button0 = buttons & 1;
            ci->button1 = buttons & 2;
            ci->button2 = buttons & 4;
            ci->button3 = false;
            mouseactive = true;
        }
    }

    if (joystickenabled && !mouseactive) {
        Sint16 jx;
        Sint16 jy;
        Sint16 jb;

        ::INL_GetJoyDelta(joystickport, &jx, &jy);

        if (jy < -SENSITIVE)
            ci->dir = dir_North;
        else if (jy > SENSITIVE)
            ci->dir = dir_South;

        if (jx < -SENSITIVE)
            ci->dir = dir_West;
        else if (jx > SENSITIVE)
            ci->dir = dir_East;

        jb = ::IN_JoyButtons();

        if (jb != 0) {
            ci->button0 =jb & 1;
            ci->button1 =jb & 2;

            if (joypadenabled) {
                ci->button2 =jb & 4;
                ci->button3 =jb & 8;
            } else {
                ci->button2 = false;
                ci->button3 = false;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////
//
// DRAW DIALOG AND CONFIRM YES OR NO TO QUESTION
//
////////////////////////////////////////////////////////////////////
Sint16 Confirm(const char *string)
{
	Sint16 xit=0,x,y,tick=0,whichsnd[2]={ESCPRESSEDSND,SHOOTSND};


	Message(string);

// Next two lines needed for flashing cursor ...
//
	SETFONTCOLOR(BORDER_TEXT_COLOR,BORDER_MED_COLOR);
	CA_CacheGrChunk(STARTFONT+fontnumber);

	IN_ClearKeysDown();

	//
	// BLINK CURSOR
	//
	x=PrintX;
	y=PrintY;
	TimeCount=0;
	do
	{
		if (TimeCount>=10)
		{
			switch(tick)
			{
				case 0:
					VWB_Bar(x,y,8,13,BORDER_MED_COLOR);
					break;

				case 1:
					PrintX=x;
					PrintY=y;
					US_Print("_");
			}

			VW_UpdateScreen();
			tick^=1;
			TimeCount=0;
		}

        // BBi
        IN_CheckAck();
	} while(!Keyboard[sc_y] && !Keyboard[sc_n] && !Keyboard[sc_escape]);


	if (Keyboard[sc_y])
	{
		xit=1;
		ShootSnd();
	}

// BBi
#if 0
	while(Keyboard[sc_y] || Keyboard[sc_n] || Keyboard[sc_Escape]);
#endif

    while(Keyboard[sc_y] || Keyboard[sc_n] || Keyboard[sc_escape])
        IN_CheckAck();
// BBi

	IN_ClearKeysDown();

    ::sd_play_player_sound(
        static_cast<soundnames>(whichsnd[xit]),
        bstone::AC_ITEM);

	FREEFONT(STARTFONT+fontnumber);		

	return xit;
}

//---------------------------------------------------------------------------
// Message() - PRINT A MESSAGE IN A WINDOW
//---------------------------------------------------------------------------
void Message(const char *string)
{
	Sint16 h=0,w=0,mw=0;
    size_t i;
	fontstruct *font;
	Uint16 OldPrintX,OldPrintY;

	fontnumber=1;
	CA_CacheGrChunk(STARTFONT+1);

	font=static_cast<fontstruct*>(grsegs[STARTFONT+fontnumber]);

	h=font->height;
	for (i=0;i<strlen(string);i++)
		if (string[i]=='\n')
		{
			if (w>mw)
				mw=w;
			w=0;
			h+=font->height;
		}
		else
			w+=font->width[static_cast<int>(string[i])];

	if (w+10>mw)
	mw=w+10;

	OldPrintY = PrintY=(WindowH/2)-h/2;
	OldPrintX = PrintX=WindowX=160-mw/2;

	// bump down and to right for shadow

	PrintX++;
	PrintY++;
	WindowX++;

	BevelBox(WindowX-6,PrintY-6,mw+10,h+10,BORDER_HI_COLOR,BORDER_MED_COLOR,BORDER_LO_COLOR);

	SETFONTCOLOR(BORDER_LO_COLOR,BORDER_MED_COLOR);
	US_Print(string);

	PrintY=OldPrintY;
	WindowX=PrintX=OldPrintX;

	SETFONTCOLOR(BORDER_TEXT_COLOR,BORDER_MED_COLOR);
	US_Print(string);

	FREEFONT(STARTFONT+1);

	VW_UpdateScreen();
}



//--------------------------------------------------------------------------
// TerminateStr - Searches for an "^XX" and replaces with a 0 (NULL)
//--------------------------------------------------------------------------
void TerminateStr(char *pos)
{
   pos = strstr(pos,"^XX");

#if IN_DEVELOPMENT
   if (!pos)
   	MENU_ERROR(CACHE_MESSAGE_NO_END_MARKER);
#endif

	*pos = 0;
}

//---------------------------------------------------------------------------
// CacheMessage() - Caches and prints a message in a window.
//---------------------------------------------------------------------------
void CacheMessage(Uint16 MessageNum)
{
	char *string;

	CA_CacheGrChunk(MessageNum);
   string = (char*)grsegs[MessageNum];

   TerminateStr(string);

   Message(string);

	FREEFONT(MessageNum);
}


//---------------------------------------------------------------------------
// CacheCompData() - Caches and Decompresses data from the VGAGRAPH
//
// NOTE: - User is responsible for freeing loaded data
//       - Returns the size of the data
//       - Does not call TerminateStr() for loaded TEXT data
//
// RETURNS: Lenght of loaded (decompressed) data
//
//---------------------------------------------------------------------------

Uint32 CacheCompData(Uint16 item_number, void** dst_ptr)
{
   char* chunk;
   char* dst;
    CompHeader_t CompHeader;
   Uint32 data_length;

    // Load compressed data
    CA_CacheGrChunk(item_number);
    chunk = (char*)grsegs[item_number];

    memcpy(CompHeader.NameId, &chunk[0], 4);
    CompHeader.OriginalLen = ((Uint32*)&chunk[4])[0];
    CompHeader.CompType = (ct_TYPES)((Sint16*)&chunk[8])[0];
    CompHeader.CompressLen = ((Uint32*)&chunk[10])[0];

    data_length = CompHeader.OriginalLen;

   chunk += 14;

   // Allocate Dest Memory

    dst = new char[data_length];
    *dst_ptr = dst;

   // Decompress and terminate string

    if (!LZH_Startup())
        Quit("out of memory");

    ::LZH_Decompress(
        chunk,
        dst,
        data_length,
        CompHeader.CompressLen);

    LZH_Shutdown();

    // Free compressed data
    UNCACHEGRCHUNK(item_number);

   // Return loaded size
   return data_length;
}

//-------------------------------------------------------------------------
// CheckForSpecialCode() - Scans the Command Line parameters for
//									special code word and returns true if found.
//
// NOTE: - Requires that the MEMORY and CACHE manager be started up.
//       - The chunk being checked MUST be JAMPAKd - (this may change)
//
//-------------------------------------------------------------------------
boolean CheckForSpecialCode(Uint16 ItemNum)
{
   void* code;
   boolean return_val = false;
   char *code_ptr;

   // Allocate, Cache & Decomp into ram

   CacheCompData(ItemNum, &code);
   code_ptr = (char*)code;
   TerminateStr(code_ptr);

   // Check for code
   if (::g_args.find_argument(code_ptr) >= 0)
       return_val = true;

	// free allocated memory

    delete [] static_cast<char*>(code);

   return(return_val);
}


////////////////////////////////////////////////////////////////////
//
// THIS MAY BE FIXED A LITTLE LATER...
//
////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// StartCPMusic()
//---------------------------------------------------------------------------
void StartCPMusic(Sint16 song)
{
	musicnames	chunk;

    if (audiosegs[STARTMUSIC + lastmenumusic]) { // JDC
        delete [] audiosegs[STARTMUSIC + lastmenumusic];
        audiosegs[STARTMUSIC + lastmenumusic] = NULL;
    }

	lastmenumusic = song;

	SD_MusicOff();
	chunk = static_cast<musicnames>(song);
	CA_CacheAudioChunk(static_cast<Sint16>(STARTMUSIC + chunk));
    ::SD_StartMusic(chunk);
}

//---------------------------------------------------------------------------
// FreeMusic ()
//---------------------------------------------------------------------------
void FreeMusic (void)
{
	SD_MusicOff();
	if (audiosegs[STARTMUSIC + lastmenumusic]) { // JDC
        delete [] audiosegs[STARTMUSIC + lastmenumusic];
        audiosegs[STARTMUSIC + lastmenumusic] = NULL;
    }
}


#ifdef CACHE_KEY_DATA

//---------------------------------------------------------------------------
//	IN_GetScanName() - 	Returns a string containing the name of the
//								specified scan code
//---------------------------------------------------------------------------
Uint8 far* IN_GetScanName(ScanCode scan)
{
	Uint8		*p;
	ScanCode	*s;

	for (s = ExtScanCodes,p = ExtScanNames;*s;p+=7,s++)
		if (*s == scan)
			return((Uint8 *)p);

	return((Uint8 *)(ScanNames+(scan<<1)));
}

#else

//---------------------------------------------------------------------------
//	IN_GetScanName() - 	Returns a string containing the name of the
//								specified scan code
//---------------------------------------------------------------------------
Uint8* IN_GetScanName(ScanCode scan)
{
	Uint8		**p;
	ScanCode	*s;

	for (s = ExtScanCodes,p = ExtScanNames;*s;p++,s++)
		if (*s == scan)
			return(*p);

	return(ScanNames[scan]);
}

#endif


//---------------------------------------------------------------------------
// CheckPause() - CHECK FOR PAUSE KEY (FOR MUSIC ONLY)
//---------------------------------------------------------------------------
void CheckPause(void)
{
	if (Paused)
	{
		switch(SoundStatus)
		{
			case 0:
				SD_MusicOn();
				break;

			case 1:
				SD_MusicOff();
				break;
		}

		SoundStatus^=1;
		VW_WaitVBL(3);
		IN_ClearKeysDown();
		Paused=false;
	}
}

//-------------------------------------------------------------------------
// DrawMenuGun() - DRAW GUN CURSOR AT CORRECT POSITION IN MENU
//-------------------------------------------------------------------------
void DrawMenuGun(CP_iteminfo *iteminfo)
{
	Sint16 x,y;

	x=iteminfo->cursor.x;
	y=iteminfo->y+iteminfo->curpos*iteminfo->y_spacing+iteminfo->cursor.y_ofs;

	VWB_Bar(x,y,iteminfo->cursor.width,iteminfo->cursor.height,HIGHLIGHT_BOX_COLOR);
}

//-------------------------------------------------------------------------
// ShootSnd()
//-------------------------------------------------------------------------
void ShootSnd(void)
{
    ::sd_play_player_sound(SHOOTSND, bstone::AC_ITEM);
}

#if GAME_VERSION == SHAREWARE_VERSION


//-------------------------------------------------------------------------
// ShowPromo()
//-------------------------------------------------------------------------
void ShowPromo()
{
	#define PROMO_MUSIC HIDINGA_MUS

// Load and start music
//
	CA_CacheAudioChunk(STARTMUSIC+PROMO_MUSIC);
	SD_StartMusic((MusicGroup *)audiosegs[STARTMUSIC+PROMO_MUSIC]);

// Show promo screen 1
//
	MenuFadeOut();
	CA_CacheScreen(PROMO1PIC);
	VW_UpdateScreen();
	MenuFadeIn();
	IN_UserInput(TickBase*20);

// Show promo screen 2
//
	MenuFadeOut();
	CA_CacheScreen(PROMO2PIC);
	VW_UpdateScreen();
	MenuFadeIn();
	IN_UserInput(TickBase*20);

// Music off and freed!
//
	StopMusic();
}

#endif

//-------------------------------------------------------------------------
// ExitGame()
//-------------------------------------------------------------------------
void ExitGame()
{
   VW_FadeOut();
#if GAME_VERSION == SHAREWARE_VERSION
	ShowPromo();
#endif

	SD_MusicOff();
	SD_StopSound();
	Quit("");
}

// BBi
int volume_index = 0;
int* const volumes[2] = {&g_sfx_volume, &g_music_volume};

void draw_volume_control(
    int index,
    int volume,
    bool is_enabled)
{
    int16_t slider_color =
        is_enabled ? ENABLED_TEXT_COLOR : DISABLED_TEXT_COLOR;

    int16_t outline_color =
        is_enabled ? HIGHLIGHT_TEXT_COLOR : DEACTIAVED_TEXT_COLOR;

    int y = 82 + (index * 40);

    VWB_Bar(74, static_cast<int16_t>(y), 160, 8, HIGHLIGHT_BOX_COLOR);
    DrawOutline(73, static_cast<int16_t>(y - 1), 161, 9,
        outline_color, outline_color);
    VWB_Bar(static_cast<int16_t>(74 + ((160 * volume) / (MAX_VOLUME + 1))),
        static_cast<int16_t>(y), 16, 8, slider_color);
}

void draw_volume_controls()
{
    for (int i = 0; i < 2; ++i)
        draw_volume_control(i, *(volumes[i]), i == volume_index);
}

void cp_sound_volume(
    Sint16)
{
    ClearMScreen();
    DrawMenuTitle("SOUND VOLUME");
    DrawInstructions(IT_SOUND_VOLUME);

    fontnumber = 4;

    SETFONTCOLOR(HIGHLIGHT_TEXT_COLOR, TERM_BACK_COLOR);

    PrintX = 150;
    PrintY = 60;
    US_Print("SFX");

    PrintX = 145;
    PrintY = 105;
    US_Print("MUSIC");

    for (int i = 0; i < 2; ++i) {
        PrintX = 36;
        PrintY = static_cast<uint16_t>(81 + (i * 40));
        US_Print("MUTE");

        PrintX = 242;
        US_Print("LOUD");
    }

    draw_volume_controls();

    VW_UpdateScreen();
    MenuFadeIn();

    ControlInfo ci;

    int old_volumes[2];
    for (int i = 0; i < 2; ++i)
        old_volumes[i] = -1;

    for (bool quit = false; !quit; ) {
        bool update_volumes = false;
        bool redraw_controls = false;

        ReadAnyControl(&ci);

        switch (ci.dir) {
        case dir_North:
            if (volume_index == 1) {
                redraw_controls = true;
                volume_index = 0;
                draw_volume_controls();
                VW_UpdateScreen();
            }

            while (Keyboard[sc_up_arrow])
                ::in_handle_events();
            break;

        case dir_South:
            if (volume_index == 0) {
                redraw_controls = true;
                volume_index = 1;
                draw_volume_controls();
                VW_UpdateScreen();
            }

            while (Keyboard[sc_down_arrow])
                ::in_handle_events();
            break;

        case dir_West:
            if (*volumes[volume_index] > MIN_VOLUME) {
                redraw_controls = true;
                update_volumes = true;
                --(*volumes[volume_index]);
                draw_volume_controls();
                VW_UpdateScreen();
            }

            while (Keyboard[sc_left_arrow])
                ::in_handle_events();
            break;

        case dir_East:
            if (*volumes[volume_index] < MAX_VOLUME) {
                redraw_controls = true;
                update_volumes = true;
                ++(*volumes[volume_index]);
            }

            while (Keyboard[sc_right_arrow])
                ::in_handle_events();
            break;

        default:
            break;
        }

        if (update_volumes) {
            update_volumes = false;

            if (old_volumes[0] != *volumes[0]) {
                sd_set_sfx_volume(g_sfx_volume);
                sd_play_player_sound(MOVEGUN1SND, bstone::AC_ITEM);
            }

            if (old_volumes[1] != *volumes[1])
                sd_set_music_volume(g_music_volume);
        }

        if (redraw_controls) {
            redraw_controls = false;
            draw_volume_controls();
            VW_UpdateScreen();
        }

        quit = (ci.button1 || Keyboard[sc_escape]);
    }

    sd_play_player_sound(ESCPRESSEDSND, bstone::AC_ITEM);

    WaitKeyUp();
    MenuFadeIn();
}
// BBi
