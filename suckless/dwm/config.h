/* See LICENSE file for copyright and license details. */
#include "dwm.h"
#include "themes/nord.h"

/* appearance */
#define ICONSIZE 16   /* icon size */
#define ICONSPACING 5 /* space between icon and title */

static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const int gappx              = 5;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 8;        /* 2 is the default spacing around the bar's font */
static const int vertpad            = 10;       /* vertical padding of bar */
static const int sidepad            = 10;       /* horizontal padding of bar */
static const char *fonts[]          = {"MesloLGS Nerd Font:size=14"};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };
static const char *tagsalt[] = { "", "", "", "", "", "", "", "", "" };
static const int momentaryalttags = 0; /* 1 means alttags will show only when key is held down*/

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int attachbelow = 1;    /* 1 means attach after the currently active window */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "|  0 |",    tile },    /* first entry is default */
	{ "|  |",      NULL },    /* no layout function means floating behavior */
	{ "|  0 |",    monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *upvol[] = { "amixer", "set", "Master", "4%+", NULL };
static const char *downvol[] = { "amixer", "set", "Master", "4%-", NULL };
static const char *mutevol[] = { "amixer", "set", "Master", "toggle", NULL };
static const char *upbr[] = { "brightnessctl", "set", "5%+" , NULL };
static const char *downbr[] = { "brightnessctl", "set", "5%-" , NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",               STRING,  &fonts },
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "urgbgcolor",         STRING,  &urgbgcolor },
		{ "urgbordercolor",     STRING,  &urgbordercolor },
		{ "urgfgcolor",         STRING,  &urgfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		  INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	  INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	    FLOAT,   &mfact },
};


#include "shift-tools.c"

static Key keys[] = {
	/* modifier                     key        function           argument */
	{ 0,                            XF86XK_AudioRaiseVolume,    		spawn,          {.v = upvol } },
	{ 0,                            XF86XK_AudioLowerVolume,    		spawn,          {.v = downvol } },
	{ 0,                            XF86XK_AudioMute,    			      spawn,          {.v = mutevol } },
	{ 0,                            XF86XK_MonBrightnessUp,     		spawn,          {.v = upbr } },
	{ 0,                            XF86XK_MonBrightnessDown,   		spawn,          {.v = downbr } },

	{ MODKEY,                       XK_o,      shiftviewclients,  { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,	     shiftview,         { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,	     shiftview,         { .i = -1 } },
	{ MODKEY,	                      XK_i,      shiftviewclients,  { .i = -1 } },

	{ MODKEY,                       XK_p,      spawn,             { .v = dmenucmd } },
	{ MODKEY,                       XK_n,      spawn,             { .v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,         { 0 } },

	{ MODKEY,                       XK_j,      focusstack,        { .i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,        { .i = -1 } },

  { MODKEY|ShiftMask,             XK_s,      spawn,             SHCMD("~/.dwm/screensnap.sh") }, 
  { MODKEY,                       XK_s,      spawn,             SHCMD("~/.dwm/screenshot.sh") }, 
  {MODKEY,                        XK_z,      spawn,             SHCMD("slock")},

	// { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	// { MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
  //
	{ MODKEY,                       XK_h,      setmfact,          { .f = -0.05} },
	{ MODKEY|ControlMask,		        XK_i,      shiftboth,         { .i = -1 }	},
	{ MODKEY|ShiftMask,		          XK_h,      shiftswaptags,     { .i = -1 }	},
	{ MODKEY|ShiftMask,		          XK_l,      shiftswaptags,     { .i = +1 }	},
	{ MODKEY|ControlMask,           XK_o,      shiftboth,         { .i = +1 }	},
	{ MODKEY,                       XK_l,      setmfact,          { .f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,              { 0 } },
	{ MODKEY,                       XK_Tab,    view,              { 0 } },
	{ MODKEY,                       XK_q,      killclient,        { 0 } },

	{ MODKEY,                       XK_t,      setlayout,         { .v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,         { .v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,         { .v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,         { 0 } },

	{ MODKEY|ControlMask,           XK_space,  togglefloating,    { 0 } },
	{ MODKEY,                       XK_0,      view,              { .ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,               { .ui = ~0 } },

	{ MODKEY,                       XK_comma,  focusmon,          { .i = -1 } },
	{ MODKEY,                       XK_period, focusmon,          { .i = +1 } },
	{ MODKEY|ControlMask,           XK_comma,  tagandfocusmon,    { .i = -1 } },
	{ MODKEY|ControlMask,           XK_period, tagandfocusmon,    { .i = +1 } },
	{ MODKEY,                       XK_a,      togglealttag,      { 0 } },

	{ MODKEY,                       XK_minus,  setgaps,           { .i = -5 } },
	{ MODKEY,                       XK_equal,  setgaps,           { .i = +5 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,           { .i = 0  } },

	{ MODKEY|ShiftMask,             XK_q,      quit,              { 0 } },

	TAGKEYS(                        XK_1,                         0)
	TAGKEYS(                        XK_2,                         1)
	TAGKEYS(                        XK_3,                         2)
	TAGKEYS(                        XK_4,                         3)
	TAGKEYS(                        XK_5,                         4)
	TAGKEYS(                        XK_6,                         5)
	TAGKEYS(                        XK_7,                         6)
	TAGKEYS(                        XK_8,                         7)
	TAGKEYS(                        XK_9,                         8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
 	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
 	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

