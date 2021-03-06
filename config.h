/* See LICENSE file for copyright and license details. */

/* appearance */
static const char font[]	     = "-*-Consola-medium-r-*-*-9-*-*-*-*-*-*-*";
static const char normbordercolor[] = "#101010";
static const char normbgcolor[]     = "#101010";
static const char normfgcolor[]     = "#d0d0d0";
static const char selbordercolor[]  = "#232526";
static const char selbgcolor[]      = "#232526";
static const char selfgcolor[]      = "#d0d0d0";
static const unsigned int borderpx  = 1;	 /* border pixel of windows */
static const unsigned int snap      = 1;	/* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to moni */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const Bool systraypinningfailfirst = True;   /* True: if pinning fails, display systray on the first monitor, F */
static const Bool showsystray = True;     /* False means no systray */
static const Bool showbar = True;     /* False means no bar */
static const Bool topbar = True;     /* False means bottom bar */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class		instance	title	tag	isfloating	monitor */
	{ "Gimp",		NULL,		NULL,	0,	True,		-1 },

	{ "UXTerm",		NULL,		NULL,	1,	False,		0 },
	{ "Steam",		NULL,		NULL,	2,	False,		0 },
	{ "Thunar",		NULL,		NULL,	4,	False,		0 },

	{ "Firefox",		NULL,		NULL,	1,	False,		1 },
	{ "Skype",		NULL,		NULL,	2,	False,		1 },
	{ "TeamSpeak 3",		NULL,		NULL,	2,	False,		1 },
	{ "Mutt",		NULL,		"Mutt",	4,	False,		1 },
};

/* layout(s) */
static const float mfact      = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const Bool resizehints = False; /* True means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "<><",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,			KEY,	toggleview,	{.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,		KEY,	view,		{.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,		KEY,	tag,		{.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,	toggletag,	{.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/bash", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *mailcmd[]  = { "uxterm", "-title", "Mutt", "-class", "Mutt", "-e", "mutt", NULL };
static const char *sleepcmd[]  = { "sudo", "pm-suspend", NULL };
static const char *termcmd[]  = { "uxterm", NULL };

static Key keys[] = {
	/* modifier		key		function		argument */
	{ MODKEY,		XK_r,		spawn,			{.v = dmenucmd } },
	{ MODKEY,		XK_p,		spawn,			{.v = mailcmd } },
	{ MODKEY,		XK_Return,	spawn,			{.v = termcmd } },
	{ MODKEY,		XK_b,		togglebar,		{0} },
	{ MODKEY,		XK_j,		focusstack,		{.i = +1 } },
	{ MODKEY,		XK_k,		focusstack,		{.i = -1 } },
	{ MODKEY,		XK_i,		incnmaster,		{.i = +1 } },
	{ MODKEY,		XK_d,		incnmaster,		{.i = -1 } },
	{ MODKEY,		XK_h,		setmfact,		{.f = -0.05} },
	{ MODKEY,		XK_l,		setmfact,		{.f = +0.05} },
	{ MODKEY|ShiftMask,	XK_Return,	zoom,			{0} },
	{ MODKEY,		XK_Tab,		view,			{0} },
	{ MODKEY,		XK_c,		killclient,		{0} },
	{ MODKEY,		XK_t,		setlayout,		{.v = &layouts[0]} },
	{ MODKEY,		XK_f,		setlayout,		{.v = &layouts[1]} },
	{ MODKEY,		XK_m,		setlayout,		{.v = &layouts[2]} },
	{ MODKEY|ShiftMask,	XK_f,		togglefullscreen,	{0} },
	{ MODKEY,		XK_space,	setlayout,		{0} },
	{ MODKEY|ShiftMask,	XK_space,	togglefloating,		{0} },
	{ MODKEY,		XK_0,		view,			{.ui = ~0 } },
	{ MODKEY|ShiftMask,	XK_0,		tag,			{.ui = ~0 } },
	{ MODKEY,		XK_comma,	focusmon,		{.i = -1 } },
	{ MODKEY,		XK_period,	focusmon,		{.i = +1 } },
	{ MODKEY|ShiftMask,	XK_comma,	tagmon,			{.i = -1 } },
	{ MODKEY|ShiftMask,	XK_period,	tagmon,			{.i = +1 } },
	{ MODKEY,		XK_Left,	shiftview,		{.i = -1 } },
	{ MODKEY,		XK_Right,	shiftview,		{.i = +1 } },
	{ MODKEY|ShiftMask,	XK_q,		quit,			{0} },
	// Multimedia keys
	{ 0,			0x1008ff2f,	spawn,			{.v = sleepcmd } },
	{ 0,			0x1008ff14,	spawn,			{.v = (const char*[]){"mpc", "toggle", NULL} } },
	{ 0,			0x1008ff17,	spawn,			{.v = (const char*[]){"mpc", "next", NULL} } },
	{ 0,			0x1008ff16,	spawn,			{.v = (const char*[]){"mpc", "prev", NULL} } },
	TAGKEYS( XK_1,	 0 )
	TAGKEYS( XK_2,	 1 )
	TAGKEYS( XK_3,	 2 )
	TAGKEYS( XK_4,	 3 )
	TAGKEYS( XK_5,	 4 )
	TAGKEYS( XK_6,	 5 )
	TAGKEYS( XK_7,	 6 )
	TAGKEYS( XK_8,	 7 )
	TAGKEYS( XK_9,	 8 )
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click		event mask	button		function	argument */
	{ ClkLtSymbol,		0,		Button1,	setlayout,	{0} },
	{ ClkLtSymbol,		0,		Button3,	setlayout,	{.v = &layouts[2]} },
	{ ClkWinTitle,		0,		Button2,	zoom,		{0} },
	{ ClkStatusText,	0,		Button2,	spawn,		{.v = termcmd } },
	{ ClkClientWin,		MODKEY,		Button1,	movemouse,	{0} },
	{ ClkClientWin,		MODKEY,		Button2,	togglefloating,	{0} },
	{ ClkClientWin,		MODKEY,		Button3,	resizemouse,	{0} },
	{ ClkTagBar,		0,		Button1,	view,		{0} },
	{ ClkTagBar,		0,		Button3,	toggleview,	{0} },
	{ ClkTagBar,		MODKEY,		Button1,	tag,		{0} },
	{ ClkTagBar,		MODKEY,		Button3,	toggletag,	{0} },
};

