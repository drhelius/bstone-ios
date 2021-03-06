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


// 3D_PLAY.C

#include "3d_def.h"
#include "bstone_globals.h"

#ifdef MSVC
#pragma hdrstop
#endif


void INL_GetJoyDelta(Uint16 joy,Sint16 *dx,Sint16 *dy);
void UpdateRadarGuage(void);
void ClearMemory (void);
void GiveWeapon (Sint16 weapon);
void DrawWeapon (void);
void DrawHealth (void);
void DrawKeys (void);
void DrawScore(void);
void ForceUpdateStatusBar(void);
void ClearSplitVWB (void);
void RedrawStatusAreas();
void PreloadGraphics(void);
void TryDropPlasmaDetonator(void);
void IN_StartAck(void);
boolean IN_CheckAck (void);
void MoveDoors (void);
void MovePWalls (void);
void ConnectAreas (void);
void UpdateSoundLoc(void);


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define sc_Question	0x35

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/


Uint8 music_num=0;

#if LOOK_FOR_DEAD_GUYS
objtype *DeadGuys[MAXACTORS];
Uint8 NumDeadGuys;
#endif

boolean		madenoise;					// true when shooting or screaming
Uint8 alerted = 0,alerted_areanum;


exit_t		playstate;

boolean PowerBall = false;

#if TECH_SUPPORT_VERSION
Sint16			bordertime,DebugOk = true,InstantWin = 0,InstantQuit = 0;
#else
Sint16			bordertime,DebugOk = false,InstantWin = 0,InstantQuit = 0;
#endif

Uint16 ExtraRadarFlags	= 0;



#if IN_DEVELOPMENT

Sint16 TestQuickSave = 0, TestAutoMapper = 0;

#endif

objtype 	objlist[MAXACTORS],*new_actor,*player,*lastobj,
			*objfreelist,*killerobj;

Uint16	farmapylookup[MAPSIZE];
Uint8		*nearmapylookup[MAPSIZE];

boolean		singlestep=false,godmode;	//,noclip;
Sint16			extravbls = 0;

Uint8		tilemap[MAPSIZE][MAPSIZE];	// wall values only
Uint8		spotvis[MAPSIZE][MAPSIZE];
objtype		*actorat[MAPSIZE][MAPSIZE];

//
// replacing refresh manager
//
Uint16	mapwidth,mapheight,tics,realtics;
boolean		compatability,usedummy=false, nevermark = false;
Uint8		*updateptr;
Uint16	mapwidthtable[64];
Uint16	uwidthtable[UPDATEHIGH];
Uint16	blockstarts[UPDATEWIDE*UPDATEHIGH];
Uint8		update[UPDATESIZE];

//
// control info
//
boolean		mouseenabled,joystickenabled,joypadenabled,joystickprogressive;
Sint16			joystickport;
Sint16			dirscan[4] = {sc_up_arrow,sc_right_arrow,sc_down_arrow,sc_left_arrow};
Sint16			buttonscan[NUMBUTTONS] =
				{sc_control,sc_alt,sc_right_shift,sc_space,sc_1,sc_2,sc_3,sc_4,sc_5,sc_6,sc_7};
Sint16			buttonmouse[4]={bt_attack,bt_strafe,bt_use,bt_nobutton};
Sint16			buttonjoy[4]={bt_attack,bt_strafe,bt_use,bt_run};

const int viewsize = 20;

bool buttonheld[NUMBUTTONS];

boolean		demorecord,demoplayback;
char		*demoptr, *lastdemoptr;
void*		demobuffer;

// Light sourcing flag

Uint8 lightson;

//
// curent user input
//
int controlx;
int controly; // range from -100 to 100 per tic
bool buttonstate[NUMBUTTONS];
int strafe_value = 0;


//===========================================================================


void	CenterWindow(Uint16 w,Uint16 h);
void 	InitObjList (void);
void 	RemoveObj (objtype *gone);
void 	PollControls (void);
void 	StopMusic(void);
void StartMusic(boolean preload);
void	PlayLoop (void);
void SpaceEntryExit(boolean entry);
void FinishPaletteShifts (void);
void ShowQuickInstructions(void);
void CleanDrawPlayBorder(void);
void PopupAutoMap(void);


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/


objtype dummyobj;

//
// LIST OF SONGS FOR EACH LEVEL
//

Sint16 songs[]=
{
	MAJMIN_MUS,              // 0
	STICKS_MUS,              // 1
	MOURNING_MUS,            // 2
	LURKING_MUS,             // 3
	CIRCLES_MUS,             // 4
	TIME_MUS,                // 5
	TOHELL_MUS,              // 6
	FORTRESS_MUS,            // 7
	GIVING_MUS,              // 8
	HARTBEAT_MUS,            // 9
	MOURNING_MUS,            // 10
	MAJMIN_MUS,              // 11
	VACCINAP_MUS,            // 12
	LURKING_MUS,             // 13
	MONASTRY_MUS,            // 14
	TOMBP_MUS,               // 15
	DARKNESS_MUS,            // 16
	MOURNING_MUS,            // 17
	SERPENT_MUS,             // 18
	TIME_MUS,                // 19
	CATACOMB_MUS,            // 20
	PLOT_MUS,					 // 21
	GIVING_MUS,              // 22
	VACCINAP_MUS,            // 23
};

/*
=============================================================================

						  USER CONTROL

=============================================================================
*/


const int BASEMOVE = 35;
const int RUNMOVE = 70;

// ISG --> unused:
//const int BASETURN = 35;
//const int RUNTURN = 70;

const int JOYSCALE = 2;


void PollKeyboardButtons()
{
    if (in_use_modern_bindings) {
        if (in_is_binding_pressed(e_bi_attack))
            buttonstate[bt_attack] = true;

        if (in_is_binding_pressed(e_bi_strafe))
            buttonstate[bt_strafe] = true;

        if (in_is_binding_pressed(e_bi_run))
            buttonstate[bt_run] = true;

        if (in_is_binding_pressed(e_bi_use))
            buttonstate[bt_use] = true;

        if (in_is_binding_pressed(e_bi_weapon_1))
            buttonstate[bt_ready_autocharge] = true;

        if (in_is_binding_pressed(e_bi_weapon_2))
            buttonstate[bt_ready_pistol] = true;

        if (in_is_binding_pressed(e_bi_weapon_3))
            buttonstate[bt_ready_burst_rifle] = true;

        if (in_is_binding_pressed(e_bi_weapon_4))
            buttonstate[bt_ready_ion_cannon] = true;

        if (in_is_binding_pressed(e_bi_weapon_5))
            buttonstate[bt_ready_grenade] = true;

        if (in_is_binding_pressed(e_bi_weapon_6))
            buttonstate[bt_ready_bfg_cannon] = true;

        if (in_is_binding_pressed(e_bi_weapon_7))
            buttonstate[bt_ready_plasma_detonators] = true;
    } else {
        for (int i = 0; i < NUMBUTTONS; ++i)
            if (Keyboard[buttonscan[i]])
                buttonstate[i] = true;
    }
}


/*
===================
=
= PollMouseButtons
=
===================
*/

void PollMouseButtons()
{
    if (in_use_modern_bindings)
        return;

    Uint8 buttons = IN_MouseButtons();

    if ((buttons & 1) != 0)
        buttonstate[buttonmouse[0]] = true;

    if ((buttons & 2) != 0)
        buttonstate[buttonmouse[1]] = true;

    if ((buttons & 4) != 0)
        buttonstate[buttonmouse[2]] = true;
}



/*
===================
=
= PollJoystickButtons
=
===================
*/

void PollJoystickButtons (void)
{
	Sint16	buttons;

	buttons = IN_JoyButtons ();

	if (joystickport && !joypadenabled)
	{
		if (buttons&4)
			buttonstate[buttonjoy[0]] = true;
		if (buttons&8)
			buttonstate[buttonjoy[1]] = true;
	}
	else
	{
		if (buttons&1)
			buttonstate[buttonjoy[0]] = true;
		if (buttons&2)
			buttonstate[buttonjoy[1]] = true;
		if (joypadenabled)
		{
			if (buttons&4)
				buttonstate[buttonjoy[2]] = true;
			if (buttons&8)
				buttonstate[buttonjoy[3]] = true;
		}
	}
}

void PollKeyboardMove()
{
    bool is_running = in_is_binding_pressed(e_bi_run);

    if (g_always_run)
        is_running = !is_running;

    // ISG --> touch pad:
    int value = tics * (is_running ? RUNMOVE : BASEMOVE);
    int valuex = tics * delta_pad_x;
    int valuey = tics * delta_pad_y;
    
    if (in_is_binding_pressed(e_bi_forward))
        controly -= valuey;

    if (in_is_binding_pressed(e_bi_backward))
        controly += valuey;

    if (in_is_binding_pressed(e_bi_left))
        controlx -= valuex;

    if (in_is_binding_pressed(e_bi_right))
        controlx += valuex;

    strafe_value = 0;

    if (in_is_binding_pressed(e_bi_strafe)) {
        if (in_is_binding_pressed(e_bi_left))
            strafe_value = -value;
        else if (in_is_binding_pressed(e_bi_right))
            strafe_value = value;
    } else if (in_is_binding_pressed(e_bi_strafe_left))
        strafe_value = -value;
    else if (in_is_binding_pressed(e_bi_strafe_right))
        strafe_value = value;
}

void PollMouseMove()
{
    int mousexmove;
    int mouseymove;
    ::in_get_mouse_deltas(mousexmove, mouseymove);
    ::in_clear_mouse_deltas();

    bool is_running = in_is_binding_pressed(e_bi_run);

    if (g_always_run)
        is_running = !is_running;

    if (is_running) {
        if (!in_use_modern_bindings)
            controly += (mouseymove * 20 / (13 - mouseadjustment)) * 4;

        controlx += (mousexmove * 10 / (13 - mouseadjustment)) / 2;
    }

    controlx += mousexmove * 10 / (13 - mouseadjustment);

    if (!in_use_modern_bindings)
        controly += mouseymove * 20 / (13 - mouseadjustment);
}


/*
===================
=
= PollJoystickMove
=
===================
*/

void PollJoystickMove (void)
{
	Sint16	joyx,joyy;

	INL_GetJoyDelta(joystickport,&joyx,&joyy);

	if (joystickprogressive)
	{
		if (joyx > 64)
			controlx += (joyx-64)*JOYSCALE*tics;
		else if (joyx < -64)
			controlx -= (-joyx-64)*JOYSCALE*tics;
		if (joyy > 64)
			controlx += (joyy-64)*JOYSCALE*tics;
		else if (joyy < -64)
			controly -= (-joyy-64)*JOYSCALE*tics;
	}
	else if (buttonstate[bt_run])
	{
		if (joyx > 64)
			controlx += RUNMOVE*tics;
		else if (joyx < -64)
			controlx -= RUNMOVE*tics;
		if (joyy > 64)
			controly += RUNMOVE*tics;
		else if (joyy < -64)
			controly -= RUNMOVE*tics;
	}
	else
	{
		if (joyx > 64)
			controlx += BASEMOVE*tics;
		else if (joyx < -64)
			controlx -= BASEMOVE*tics;
		if (joyy > 64)
			controly += BASEMOVE*tics;
		else if (joyy < -64)
			controly -= BASEMOVE*tics;
	}
}


/*
===================
=
= PollControls
=
= Gets user or demo input, call once each frame
=
= controlx		set between -100 and 100 per tic
= controly
= buttonheld[]	the state of the buttons LAST frame
= buttonstate[]	the state of the buttons THIS frame
=
===================
*/

void PollControls (void)
{
	Sint16		max,min,i;
	Uint8	buttonbits;

	controlx = 0;
	controly = 0;
	memcpy (buttonheld,buttonstate,sizeof(buttonstate));
	memset (buttonstate,0,sizeof(buttonstate));

#ifdef MYPROFILE
	controlx = 100;			// just spin in place
	return;
#endif

	if (demoplayback)
	{
	//
	// read commands from demo buffer
	//
		buttonbits = *demoptr++;
		for (i=0;i<NUMBUTTONS;i++)
		{
			buttonstate[i] = buttonbits&1;
			buttonbits >>= 1;
		}

		controlx = *demoptr++;
		controly = *demoptr++;
		tics = *demoptr++;

		while (TimeCount-lasttimecount < tics)
		;
		lasttimecount = TimeCount;

		if (demoptr == lastdemoptr)
			playstate = ex_completed;		// demo is done

		controlx *= tics;
		controly *= tics;


		return;
	}

//
// get timing info for last frame
//
	CalcTics ();
    


    // BBi
    ::in_handle_events();
//
// get button states
//
	PollKeyboardButtons ();

	if (mouseenabled)
		PollMouseButtons ();

	if (joystickenabled)
		PollJoystickButtons ();

#if 0
if (buttonstate[bt_run])
	VL_ColorBorder (1);
else
	VL_ColorBorder (0);
#endif

//
// get movements
//
	PollKeyboardMove ();

	if (mouseenabled)
		PollMouseMove ();

	if (joystickenabled)
		PollJoystickMove ();

//
// bound movement to a maximum
//
	max = 100*tics;
	min = -max;
	if (controlx > max)
		controlx = max;
	else if (controlx < min)
		controlx = min;

	if (controly > max)
		controly = max;
	else if (controly < min)
		controly = min;

#ifdef DEMOS_EXTERN

	if (demorecord)
	{
	//
	// save info out to demo buffer
	//
		controlx /= (Sint16)tics;
		controly /= (Sint16)tics;

		buttonbits = 0;

		for (i=NUMBUTTONS-1;i>=0;i--)
		{
			buttonbits <<= 1;
			if (buttonstate[i])
				buttonbits |= 1;
		}

		*demoptr++ = buttonbits;
		*demoptr++ = controlx;
		*demoptr++ = controly;
		*demoptr++ = tics;

		if (demoptr >= lastdemoptr)
			PLAY_ERROR(POLLCONTROLS_DEMO_OV);

		controlx *= (Sint16)tics;
		controly *= (Sint16)tics;
	}

#endif			

}



//==========================================================================



///////////////////////////////////////////////////////////////////////////
//
//	CenterWindow() - Generates a window of a given width & height in the
//		middle of the screen
//
///////////////////////////////////////////////////////////////////////////

#define MAXX	320
#define MAXY	160

void	CenterWindow(Uint16 w,Uint16 h)
{
	FixOfs ();
	US_DrawWindow(((MAXX / 8) - w) / 2,((MAXY / 8) - h) / 2,w,h);
}

//===========================================================================


/*
=====================
=
= CheckKeys
=
=====================
*/

extern boolean PP_step,sqActive;	
extern Sint16 pickquick;

boolean refresh_screen;
#if (GAME_VERSION != SHAREWARE_VERSION) || GEORGE_CHEAT
Uint8 jam_buff_cmp[]={sc_j,sc_a,sc_m};
Uint8 jam_buff[sizeof(jam_buff_cmp)];
#endif

char PAUSED_MSG[]="^ST1^CEGame Paused\r^CEPress any key to resume.^XX";

void CheckKeys (void)
{
	boolean one_eighty=false;
	Uint8	scan;
	static boolean Plus_KeyReleased;
	static boolean Minus_KeyReleased;
	static boolean I_KeyReleased;
	static boolean S_KeyReleased;

#if IN_DEVELOPMENT || BETA_TEST
//	if (DebugOk && (Keyboard[sc_p] || PP_step))
//		PicturePause ();
#endif


	if (screenfaded || demoplayback)	// don't do anything with a faded screen
		return;

	scan = LastScan;


#if IN_DEVELOPMENT
#ifdef ACTIVATE_TERMINAL
	if (Keyboard[sc_9] && Keyboard[sc_0])
		ActivateTerminal(true);
#endif
#endif

	//
	// SECRET CHEAT CODE: 'JAM'
	//

#if GAME_VERSION != SHAREWARE_VERSION
	if (Keyboard[sc_j] || Keyboard[sc_a] || Keyboard[sc_m])
	{
		if (jam_buff[sizeof(jam_buff_cmp)-1] != LastScan)
		{
			memcpy(jam_buff,jam_buff+1,sizeof(jam_buff_cmp)-1);
			jam_buff[sizeof(jam_buff_cmp)-1] = LastScan;
		}
	}
#endif

	CheckMusicToggle();

	if (gamestate.rpower)
	{
		if (in_is_binding_pressed(e_bi_radar_magnify))
		{
			if (Plus_KeyReleased && gamestate.rzoom<2)
			{
				UpdateRadarGuage();
				gamestate.rzoom++;
				Plus_KeyReleased=false;
			}
		}
		else
			Plus_KeyReleased=true;

		if (in_is_binding_pressed(e_bi_radar_minify))
		{
			if (Minus_KeyReleased && gamestate.rzoom)
			{
				UpdateRadarGuage();
				gamestate.rzoom--;
				Minus_KeyReleased=false;
			}
		}
		else
			Minus_KeyReleased=true;
	}

	if (in_is_binding_pressed(e_bi_sfx)) {
		if (S_KeyReleased)
		{
			if ((SoundMode != sdm_Off) || (DigiMode!=sds_Off))
			{
				if (SoundMode != sdm_Off)
				{
					SD_WaitSoundDone();
					SD_SetSoundMode(sdm_Off);
				}

				if (DigiMode!=sds_Off)
					SD_SetDigiDevice(sds_Off);

				memcpy((char *)&SoundOn[55],"OFF.",4);
			}
			else
			{
				ClearMemory();
				if (SoundBlasterPresent || AdLibPresent)
					SD_SetSoundMode(sdm_AdLib);
				else
					SD_SetSoundMode(sdm_Off);

				if (SoundBlasterPresent)
					SD_SetDigiDevice(sds_SoundBlaster);
				else
				if (SoundSourcePresent)
					SD_SetDigiDevice(sds_SoundSource);
				else
					SD_SetDigiDevice(sds_Off);

				CA_LoadAllSounds();

				memcpy((char *)&SoundOn[55],"ON. ",4);
			}

			DISPLAY_TIMED_MSG(SoundOn,MP_BONUS,MT_GENERAL);
			S_KeyReleased=false;
		}
	}
	else
		S_KeyReleased=true;

	if (Keyboard[sc_return])
	{
#if (GAME_VERSION != SHAREWARE_VERSION) || GEORGE_CHEAT
		char loop;

		if ((!memcmp(jam_buff,jam_buff_cmp,sizeof(jam_buff_cmp))))
		{
			jam_buff[0]=0;

			for (loop=0; loop<NUMKEYS; loop++)
				if (gamestate.numkeys[static_cast<int>(loop)] < MAXKEYS)
					gamestate.numkeys[static_cast<int>(loop)]=1;

			gamestate.health = 100;
			gamestate.ammo = MAX_AMMO;
			gamestate.rpower = MAX_RADAR_ENERGY;

			if (!DebugOk)
			{
				gamestate.score = 0;
				gamestate.nextextra = EXTRAPOINTS;
			}

			gamestate.TimeCount += 42000L;

			for (loop=0; loop<NUMWEAPONS-1; loop++)
				GiveWeapon(loop);

			DrawWeapon();
			DrawHealth();
			DrawKeys();
			DrawScore();
			DISPLAY_TIMED_MSG("\r\r     YOU CHEATER!",MP_INTERROGATE,MT_GENERAL);
			ForceUpdateStatusBar();

			ClearMemory ();
			ClearSplitVWB ();
			VW_ScreenToScreen (static_cast<Uint16>(displayofs),static_cast<Uint16>(bufferofs),80,160);

			Message("\n NOW you're jammin'!! \n");

			IN_ClearKeysDown();
			IN_Ack();

			CleanDrawPlayBorder();
		}
		else if (!in_use_modern_bindings)
#endif
			one_eighty=true;
	}

// Handle quick turning!
//
	if (!gamestate.turn_around)
	{
	// 90 degrees left
	//
		if (in_is_binding_pressed(e_bi_quick_left))
		{
			gamestate.turn_around = -90;
			gamestate.turn_angle = player->angle + 90;
			if (gamestate.turn_angle > 359)
				gamestate.turn_angle -= ANGLES;
		}

	// 180 degrees right
	//
		if (in_is_binding_pressed(e_bi_turn_around) || one_eighty)
		{
			gamestate.turn_around = 180;
			gamestate.turn_angle = player->angle + 180;
			if (gamestate.turn_angle > 359)
				gamestate.turn_angle -= ANGLES;
		}

	// 90 degrees right
	//
		if (in_is_binding_pressed(e_bi_quick_right))
		{
			gamestate.turn_around = 90;
			gamestate.turn_angle = player->angle - 90;
			if (gamestate.turn_angle < 0)
				gamestate.turn_angle += ANGLES;
		}
	}

//
// pause key weirdness can't be checked as a scan code
//
    if (in_is_binding_pressed(e_bi_pause)) {
        SD_MusicOff();
        fontnumber = 4;
        BMAmsg(PAUSED_MSG);
        IN_Ack();
        IN_ClearKeysDown();
        fontnumber = 2;
        RedrawStatusAreas();
        SD_MusicOn();
        Paused = false;
        ::in_clear_mouse_deltas();
        return;
    }

#if IN_DEVELOPMENT
	if (TestQuickSave)
	{
//   	TestQuickSave--;
		scan = sc_f8;
	}

	if (TestAutoMapper)
		PopupAutoMap();

#endif

    scan = sc_none;

    if (Keyboard[sc_escape])
        scan = sc_escape;
    else if (in_is_binding_pressed(e_bi_help))
        scan = sc_f1;
    if (in_is_binding_pressed(e_bi_save))
        scan = sc_f2;
    else if (in_is_binding_pressed(e_bi_load))
        scan = sc_f3;
    else if (in_is_binding_pressed(e_bi_sound))
        scan = sc_f4;
    else if (in_is_binding_pressed(e_bi_controls))
        scan = sc_f6;
    else if (in_is_binding_pressed(e_bi_end_game))
        scan = sc_f7;
    else if (in_is_binding_pressed(e_bi_quick_save))
        scan = sc_f8;
    else if (in_is_binding_pressed(e_bi_quick_load))
        scan = sc_f9;
    else if (in_is_binding_pressed(e_bi_quick_exit))
        scan = sc_f10;

    switch (scan) {
    case sc_f7:							// END GAME
    case sc_f10:						// QUIT TO DOS
        FinishPaletteShifts();
        ClearMemory();
        US_ControlPanel(scan);
        CleanDrawPlayBorder();
        return;

    case sc_f2:							// SAVE MISSION
    case sc_f8:							// QUICK SAVE
        // Make sure there's room to save...
        //
        ClearMemory();
        FinishPaletteShifts();
        if (!CheckDiskSpace(DISK_SPACE_NEEDED, CANT_SAVE_GAME_TXT, cds_id_print)) {
            CleanDrawPlayBorder();
            break;
        }

    case sc_f1:							// HELP
    case sc_f3:							// LOAD MISSION
    case sc_f4:							// SOUND MENU
    case sc_f5:							//	RESIZE VIEW
    case sc_f6:							// CONTROLS MENU
    case sc_f9:							// QUICK LOAD
    case sc_escape:					// MAIN MENU
        refresh_screen = true;
        if (scan < sc_f8)
            VW_FadeOut();
        StopMusic();
        ClearMemory();
        ClearSplitVWB();
        US_ControlPanel(scan);
        if (refresh_screen) {
            boolean old = loadedgame;

            loadedgame = false;
            DrawPlayScreen(false);
            loadedgame = old;
        }
        ClearMemory();
        if (!sqActive || !loadedgame)
            StartMusic(false);
        IN_ClearKeysDown();
        if (loadedgame) {
            PreloadGraphics();
            loadedgame = false;
            DrawPlayScreen(false);
        } else
            if (!refresh_screen)
                CleanDrawPlayBorder();
        if (!sqActive)
            StartMusic(false);
        return;
    }

    scan = sc_none;

	if (in_is_binding_pressed(e_bi_stats))
		PopupAutoMap();

  	if (Keyboard[sc_back_quote])
   {
      Keyboard[sc_back_quote] = 0;
   	TryDropPlasmaDetonator();
   }


	if ((DebugOk || gamestate.flags & GS_MUSIC_TEST) && (Keyboard[sc_backspace]))
	{
		Uint8 old_num=music_num;

		if (gamestate.flags & GS_MUSIC_TEST)
		{
			if (Keyboard[sc_left_arrow])
			{
				if (music_num)
					music_num--;
				Keyboard[sc_left_arrow]=false;
			}
			else
			if (Keyboard[sc_right_arrow])
			{
				if (music_num < LASTMUSIC-1)
					music_num++;
				Keyboard[sc_right_arrow]=false;
			}

			if (old_num != music_num)
			{
				ClearMemory();

                delete [] audiosegs[STARTMUSIC + old_num];
                audiosegs[STARTMUSIC + old_num] = NULL;

				StartMusic(false);
				DrawScore();
			}
		}

		if (old_num == music_num)
		{
			fontnumber=4;
			SETFONTCOLOR(0,15);
			if (DebugKeys())
         {
				CleanDrawPlayBorder();
         }

            ::in_clear_mouse_deltas();

			lasttimecount = TimeCount;
			return;
		}
	}

	if (in_is_binding_pressed(e_bi_attack_info))
	{
		if (I_KeyReleased)
		{
			gamestate.flags ^= GS_ATTACK_INFOAREA;
			if (gamestate.flags & GS_ATTACK_INFOAREA)
				DISPLAY_TIMED_MSG(attacker_info_enabled,MP_ATTACK_INFO,MT_GENERAL);
			else
				DISPLAY_TIMED_MSG(attacker_info_disabled,MP_ATTACK_INFO,MT_GENERAL);
			I_KeyReleased = false;
		}
	}
	else
		I_KeyReleased = true;


#ifdef CEILING_FLOOR_COLORS
	if (in_is_binding_pressed(e_bi_ceiling))
	{
		gamestate.flags ^= GS_DRAW_CEILING;
		in_reset_binding_state(e_bi_ceiling);
	}

	if (in_is_binding_pressed(e_bi_flooring))
	{
		ThreeDRefresh();
		ThreeDRefresh();

		gamestate.flags ^= GS_DRAW_FLOOR;

		in_reset_binding_state(e_bi_flooring);
#if DUAL_SWAP_FILES
		ChangeSwapFiles(true);
#endif
	}
#endif

	if (in_is_binding_pressed(e_bi_lightning))
	{
		in_reset_binding_state(e_bi_lightning);
		gamestate.flags ^= GS_LIGHTING;
	}
}


//-------------------------------------------------------------------------
// CheckMusicToggle()
//-------------------------------------------------------------------------
void CheckMusicToggle(void)
{
	static boolean M_KeyReleased;

	if (in_is_binding_pressed(e_bi_music))
	{
		if (M_KeyReleased
#if GAME_VERSION != SHAREWARE_VERSION
			 && ((jam_buff[0] != sc_j) || (jam_buff[1] != sc_a))
#endif
			)
		{
			if (!AdLibPresent)
			{
				DISPLAY_TIMED_MSG(NoAdLibCard,MP_BONUS,MT_GENERAL);

                ::sd_play_player_sound(NOWAYSND, bstone::AC_NO_WAY);
            return;
			}
			else
			if (MusicMode != smm_Off)
			{
				SD_SetMusicMode(smm_Off);
				memcpy((char *)&MusicOn[58],"OFF.",4);
			}
			else
			{
				SD_SetMusicMode(smm_AdLib);
				StartMusic(false);
				memcpy((char *)&MusicOn[58],"ON. ",4);
			}

			DISPLAY_TIMED_MSG(MusicOn,MP_BONUS,MT_GENERAL);
			M_KeyReleased=false;
		}
	}
	else
		M_KeyReleased=true;
}


char Computing[] = {"Computing..."};

#if DUAL_SWAP_FILES
//--------------------------------------------------------------------------
// ChangeSwapFiles()
//
// PURPOSE: To chance out swap files durring game play -
//
// ASSUMES: PageManager is installed.
//
//--------------------------------------------------------------------------

void ChangeSwapFiles(boolean display)
{
	ClearMemory();

   if (display)
   {
		WindowX=WindowY=0;
		WindowW=320;
		WindowH=200;
	   Message(Computing);
   }

	PM_Shutdown();
	PM_Startup ();

	PM_CheckMainMem();

   if (display)
   {
	   IN_UserInput(50);
		CleanDrawPlayBorder();
		IN_ClearKeysDown();
   }
}
#endif


//--------------------------------------------------------------------------
// PopupAutoMap()
//--------------------------------------------------------------------------
void PopupAutoMap()
{
	#define BASE_X	64
	#define BASE_Y	44

	ThreeDRefresh();
	ThreeDRefresh();

	SD_StopSound();
	ClearMemory();
	CacheDrawPic(BASE_X,BASE_Y,AUTOMAPPIC);

	ShowStats(BASE_X+101,BASE_Y+22,ss_quick,&gamestuff.level[gamestate.mapon].stats);

	while (Keyboard[sc_back_quote])
		CalcTics();

#if GAME_VERSION != SHAREWARE_VERSION && IN_DEVELOPMENT
//	if (DebugOk && PP_step)
//		PicturePause();
#endif

	IN_StartAck ();
	while (!IN_CheckAck ())
		CalcTics();

	CleanDrawPlayBorder();
	IN_ClearKeysDown();
}


//===========================================================================

/*
#############################################################################

				  The objlist data structure

#############################################################################

objlist containt structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawn another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitActorList
=
= Call to clear out the actor object lists returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

int objcount;

void InitActorList (void)
{
	Sint16	i;

//
// init the actor lists
//
#if LOOK_FOR_DEAD_GUYS
	NumDeadGuys=0;
	memset(DeadGuys,0,sizeof(DeadGuys));
#endif

	memset(statobjlist,0,sizeof(statobjlist));
	for (i=0;i<MAXACTORS;i++)
	{
		objlist[i].prev = &objlist[i+1];
		objlist[i].next = NULL;
	}

	objlist[MAXACTORS-1].prev = NULL;

	objfreelist = &objlist[0];
	lastobj = NULL;

	objcount = 0;

//
// give the player the first free spots
//
	GetNewActor ();
	player = new_actor;
}

//===========================================================================

/*
=========================
=
= GetNewActor
=
= Sets the global variable new to point to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

void GetNewActor (void)
{
	if (objcount >= MAXACTORS-1)
	{
		objtype *obj=player->next;

		while (obj)
		{
			if ((obj->flags & (FL_DEADGUY|FL_VISABLE)) == FL_DEADGUY)
			{
				RemoveObj(obj);
				obj = NULL;
			}
			else
				obj = obj->next;
		}
	}

	if (!objfreelist)
		if (usedummy)
      {
			new_actor = &dummyobj;
			memset (new_actor,0,sizeof(*new_actor));
      }
		else
			PLAY_ERROR(GETNEWACTOR_NO_FREE_SPOTS);
   else
   {
		new_actor = objfreelist;
		objfreelist = new_actor->prev;

		memset (new_actor,0,sizeof(*new_actor));

		if (lastobj)
			lastobj->next = new_actor;

		new_actor->prev = lastobj;	// new_actor->next is allready NULL from memset

//		new_actor->active = false;
		lastobj = new_actor;

		objcount++;
	}
}


//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype *gone)
{
   if (gone == &dummyobj)
   	return;

	if (gone == player)
		PLAY_ERROR(REMOVEOBJ_REMOVED_PLAYER);

	gone->state = NULL;

//
// fix the next object's back link
//
	if (gone == lastobj)
		lastobj = (objtype *)gone->prev;
	else
		gone->next->prev = gone->prev;

//
// fix the previous object's forward link
//
	gone->prev->next = gone->next;

//
// add it back in to the free list
//
	gone->prev = objfreelist;
	objfreelist = gone;

	objcount--;
}

/*
=============================================================================

						MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/

void StopMusic(void)
{
	SD_MusicOff();
}

//==========================================================================

//-------------------------------------------------------------------------
// StartMusic()
//		o preload = true, music is cached but not started
//-------------------------------------------------------------------------
void StartMusic(boolean preload)
{
	musicnames	musicchunk;

	SD_MusicOff();
#if IN_DEVELOPMENT || GAME_VERSION != SHAREWARE_VERSION || TECH_SUPPORT_VERSION
	if (gamestate.flags & GS_MUSIC_TEST)
		musicchunk=static_cast<musicnames>(music_num);
	else
#endif
	if (playstate==ex_victorious)
		musicchunk = FORTRESS_MUS;
	else
		musicchunk = static_cast<musicnames>(songs[gamestate.mapon+gamestate.episode*MAPS_PER_EPISODE]);

	if (!audiosegs[STARTMUSIC+musicchunk])
	{
		CA_CacheAudioChunk(static_cast<Sint16>(STARTMUSIC + musicchunk));
	}

	{
		if (!preload)
        ::SD_StartMusic(musicchunk);
	}
}

/*
=============================================================================

					PALETTE SHIFTING STUFF

=============================================================================
*/

const int NUMREDSHIFTS = 6;
const int REDSTEPS = 8;

const int NUMWHITESHIFTS = 3;
const int WHITESTEPS = 20;
const int WHITETICS = 6;


Uint8 redshifts[NUMREDSHIFTS][768];
Uint8 whiteshifts[NUMREDSHIFTS][768];

int damagecount;
int bonuscount;
bool palshifted;


void InitRedShifts()
{
    //
    // fade through intermediate frames
    //
    for (int i = 1; i <= NUMREDSHIFTS; i++) {
        Uint8* workptr = &redshifts[i - 1][0];
        const Uint8* baseptr = vgapal;

        for (int j = 0; j <= 255; ++j) {
            int delta = 64 - baseptr[0];

            workptr[0] = static_cast<Uint8>(
                baseptr[0] + ((delta * i) / REDSTEPS));

            delta = -baseptr[1];

            workptr[1] = static_cast<Uint8>(
                baseptr[1] + ((delta * i) / REDSTEPS));

            delta = -baseptr[2];

            workptr[2] = static_cast<Uint8>(
                baseptr[2] + ((delta * i) / REDSTEPS));

            baseptr += 3;
            workptr += 3;
        }
    }

    for (int i = 1; i <= NUMWHITESHIFTS; i++) {
        Uint8* workptr = &whiteshifts[i - 1][0];
        const Uint8* baseptr = vgapal;

        for (int j = 0; j <= 255; ++j) {
            int delta = 64 - baseptr[0];

            workptr[0] = static_cast<Uint8>(
                baseptr[0] + ((delta * i) / WHITESTEPS));

            delta = 62 - baseptr[1];

            workptr[1] = static_cast<Uint8>(
                baseptr[1] + ((delta * i) / WHITESTEPS));

            delta = 0 - baseptr[2];

            workptr[2] = static_cast<Uint8>(
                baseptr[2] + ((delta * i) / WHITESTEPS));

            baseptr += 3;
            workptr += 3;
        }
    }
}

void ClearPaletteShifts()
{
    bonuscount = 0;
    damagecount = 0;
}

void StartBonusFlash()
{
    // white shift palette
    bonuscount = NUMWHITESHIFTS * WHITETICS;
}

void StartDamageFlash(
    int damage)
{
    damagecount += damage;
}

void UpdatePaletteShifts()
{
    int red = 0;
    int white = 0;

    if (bonuscount > 0) {
        white = (bonuscount / WHITETICS) + 1;

        if (white > NUMWHITESHIFTS)
            white = NUMWHITESHIFTS;

        bonuscount -= tics;

        if (bonuscount < 0)
            bonuscount = 0;
    } else
        white = 0;

    if (damagecount > 0) {
        red = (damagecount / 10) + 1;

        if (red > NUMREDSHIFTS)
            red = NUMREDSHIFTS;

        damagecount -= tics;

        if (damagecount < 0)
            damagecount = 0;
    } else
        red = 0;

    if (red > 0) {
        VW_WaitVBL(1);
        VL_SetPalette(0, 256, redshifts[red - 1], false);
        palshifted = true;
    } else if (white > 0) {
        VW_WaitVBL(1);
        VL_SetPalette(0, 256, whiteshifts[white - 1], false);
        palshifted = true;
    } else if (palshifted) {
        VW_WaitVBL(1);
        VL_SetPalette(0, 256, vgapal, false); // back to normal
        palshifted = false;
    }
}

void FinishPaletteShifts()
{
    if (palshifted) {
        palshifted = false;
        VW_WaitVBL(1);
        VL_SetPalette(0, 256, vgapal);
    }
}


/*
=============================================================================

						CORE PLAYLOOP

=============================================================================
*/


/*
=====================
=
= DoActor
=
=====================
*/

void DoActor (objtype *ob)
{
	void (*think)(objtype *);
	objtype *actor;


	if (ob->flags & FL_FREEZE)
		return;

	if (ob->flags & FL_BARRIER)
	{
		actor = actorat[ob->tilex][ob->tiley];
		if (BARRIER_STATE(ob) == bt_ON)
		{
			if (actor)
			{
				Sint16 damage = 0;

				actor->flags |= FL_BARRIER_DAMAGE;
				if ((US_RndT() < 0x7f) && (actor->flags & FL_SHOOTABLE))
				{
					switch (ob->obclass)
					{
						case arc_barrierobj:			// arc barrier - Mild Damage
							damage = 500;			// 100
						break;

						case post_barrierobj:		// post barrier - Butt kicker
							damage = 500;
						break;

                        default:
                            break;
					}
					DamageActor(actor,damage,ob);
				}
			}
		}
		else
			if (actor)
				actor->flags &= ~FL_BARRIER_DAMAGE;
	}

	if (!ob->active && !areabyplayer[ob->areanumber])
		return;

	if (!(ob->flags&(FL_NONMARK|FL_NEVERMARK)) )
		actorat[ob->tilex][ob->tiley] = NULL;

//
// non transitional object
//

	if (!ob->ticcount)
	{
		think =	ob->state->think;
		if (think)
		{
			think (ob);
			if (!ob->state)
			{
				RemoveObj (ob);
				return;
			}
		}

		if (!(ob->flags&FL_NEVERMARK) )
		{
			if (ob->flags&FL_NONMARK)
			{
				if (actorat[ob->tilex][ob->tiley])
				{
					return;
				}
			}
			actorat[ob->tilex][ob->tiley] = ob;
		}
		return;
	}

//
// transitional object
//
	ob->ticcount-=tics;
	while ( ob->ticcount <= 0)
	{
		think = ob->state->action;			// end of state action
		if (think)
		{
			think (ob);
			if (!ob->state)
			{
				RemoveObj (ob);
				return;
			}
		}

		ob->state = ob->state->next;

		if (!ob->state)
		{
			RemoveObj (ob);
			return;
		}

		if (!ob->state->tictime)
		{
			ob->ticcount = 0;
			goto think;
		}

		ob->ticcount += ob->state->tictime;
	}

think:
	//
	// think
	//
	think =	ob->state->think;
	if (think)
	{
		think (ob);
		if (!ob->state)
		{
			RemoveObj (ob);
			return;
		}
	}

	if ( !(ob->flags&FL_NEVERMARK) )
	{
		if (ob->flags&FL_NONMARK)
		{
			if (actorat[ob->tilex][ob->tiley])
			{
				return;
			}
		}
		actorat[ob->tilex][ob->tiley] = ob;
	}
	return;
}

//==========================================================================


/*
===================
=
= PlayLoop
=
===================
*/

extern boolean ShowQuickMsg;


void PlayLoop (void)
{
	boolean reset_areas=false;
	objtype *obj;

    lasttimecount = 0;
    TimeCount = 0;
	playstate = ex_stillplaying;

	framecount = frameon = 0;
	pwallstate = anglefrac = 0;
	memset (buttonstate,0,sizeof(buttonstate));
	ClearPaletteShifts ();
   ForceUpdateStatusBar();

    ::in_clear_mouse_deltas();

	tics = 1;			// for first time through
	if (demoplayback)
		IN_StartAck ();

	do
	{
		PollControls();

//
// actor thinking
//
		madenoise = false;

		if (alerted)
			alerted--;

		MoveDoors ();
		MovePWalls ();

		for (obj = player;obj;obj = obj->next)
		{
			if ((obj != player) && (Keyboard[sc_6] || Keyboard[sc_7]) && Keyboard[sc_8] && DebugOk)
			{
				if (!reset_areas)
					memset(areabyplayer,1,sizeof(areabyplayer));
				reset_areas=true;

				if ((((!(obj->flags & FL_INFORMANT)) && (obj->flags & FL_SHOOTABLE))) ||
					 (obj->obclass == liquidobj && !(obj->flags & FL_DEADGUY)))
					DamageActor(obj,1000,player);
			}
			else
				if (reset_areas)
				{
					ConnectAreas();
					reset_areas=false;
				}
			DoActor (obj);
		}

		if (NumEAWalls)
			CheckSpawnEA();

		if ((!GoldsternInfo.GoldSpawned) && GoldsternInfo.SpawnCnt)
			CheckSpawnGoldstern();

		UpdatePaletteShifts ();


		ThreeDRefresh ();

		gamestate.TimeCount+=tics;

		UpdateSoundLoc();	// JAB

		if (screenfaded & !playstate)
			VW_FadeIn();

	// Display first-time instructions.
	//
		if (ShowQuickMsg)
			ShowQuickInstructions();

		CheckKeys();

		if (demoplayback && demoptr == lastdemoptr)
			playstate = ex_title;

//
// debug aids
//
		if (singlestep)
		{
			VW_WaitVBL(14);
			lasttimecount = TimeCount;
		}
		if (extravbls)
			VW_WaitVBL(extravbls);

		if ((demoplayback) && (IN_CheckAck()))
		{
			IN_ClearKeysDown ();
			playstate = ex_abort;
		}


	}while (!playstate && !startgame);

	if (playstate != ex_died)
		FinishPaletteShifts ();

	gamestate.flags &= ~GS_VIRGIN_LEVEL;
}

//--------------------------------------------------------------------------
// ShowQuickInstructions()
//--------------------------------------------------------------------------
void ShowQuickInstructions()
{
	ShowQuickMsg=false;

	if ((demoplayback) || (gamestate.mapon) || (gamestate.flags & GS_QUICKRUN))
		return;

	ThreeDRefresh();
	ThreeDRefresh();
	ClearMemory();
	WindowX=0; WindowY=16; WindowW=320; WindowH=168;
	CacheMessage(QUICK_INFO1_TEXT);
	VW_WaitVBL(120);
	CacheMessage(QUICK_INFO2_TEXT);
	IN_Ack();
	IN_ClearKeysDown();
	CleanDrawPlayBorder();
}

//--------------------------------------------------------------------------
// CleanDrawPlayBorder()
//--------------------------------------------------------------------------
void CleanDrawPlayBorder()
{
	DrawPlayBorder();
	ThreeDRefresh();
	DrawPlayBorder();
	ThreeDRefresh();
	DrawPlayBorder();
}

