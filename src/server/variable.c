/* $Id$ */
/* File: variable.c */

/* Purpose: Angband variables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#define SERVER

#include "angband.h"


/*
 * Hack -- Link a copyright message into the executable
 */
cptr copyright[6] =
{
	"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
	"also Copyright (c) 1997 Keldon L. Jones",
	"",
	"This software may be copied and distributed for educational, research,",
	"and not for profit purposes provided that this copyright and statement",
	"are included in all such copies."
};


/*
 * Hack -- Link the "version" into the executable
 */
byte version_major = VERSION_MAJOR;
byte version_minor = VERSION_MINOR;
byte version_patch = VERSION_PATCH;
byte version_extra = VERSION_EXTRA;


/*
 * Hack -- Savefile version
 */
byte sf_major;                  /* Savefile's "version_major" */
byte sf_minor;                  /* Savefile's "version_minor" */
byte sf_patch;                  /* Savefile's "version_patch" */
byte sf_extra;                  /* Savefile's "version_extra" */


/*
 * Hack -- Savefile information
 */
u32b sf_xtra;                   /* Operating system info */
u32b sf_when;                   /* Time when savefile created */
u16b sf_lives;                  /* Number of past "lives" with this file */
u16b sf_saves;                  /* Number of "saves" during this life */

/*
 * Hack -- Run-time arguments
 */
bool server_generated;          /* The character exists */
bool server_dungeon;            /* The character has a dungeon */
//bool central_town_loaded = FALSE;
bool server_state_loaded;       /* The server state was loaded from a savefile */
bool server_saved;              /* The character was just saved to a savefile */

bool character_loaded;          /* The character was loaded from a savefile */

u32b seed_flavor;               /* Hack -- consistent object colors */
u32b seed_town;                 /* Hack -- consistent town layout */

s16b command_new;               /* Command chaining from inven/equip view */

bool create_up_stair;           /* Auto-create "up stairs" */
bool create_down_stair;         /* Auto-create "down stairs" */

s16b num_repro;                 /* Current reproducer count */
s16b object_level;              /* Current object creation level */
s16b monster_level;             /* Current monster creation level */

s32b turn;                      /* Current game turn */

s32b player_id;                 /* Current player ID */

u16b panic_save;                /* Track some special "conditions" */

s16b signal_count = 0;          /* Hack -- Count interupts */

s16b coin_type;                 /* Hack -- force coin type */

bool opening_chest;             /* Hack -- prevent chest generation */

bool scan_monsters;             /* Hack -- optimize multi-hued code, etc */
bool scan_objects;              /* Hack -- optimize multi-hued code, etc */

s32b o_nxt = 1;                 /* Object free scanner */
s32b m_nxt = 1;                 /* Monster free scanner */

s32b o_max = 1;                 /* Object heap size */
s32b m_max = 1;                 /* Monster heap size */

s32b o_top = 0;                 /* Object top size */
s32b m_top = 0;                 /* Monster top size */


/*
 * Server options, set in tomenet.cfg
 */


server_opts cfg =
{
	/* others */
	6, 0,		// runlevel, closetime (NOT config options)

	/* char * */
	"mangband.org",		// meta_address
	"",		// bind_name
	"changeme",		// console_password
	"Serverchez",		// admin_wizard
	"DungeonMaster",	// dungeon_master

	/* s32b */
	201,		// preserve_death_level
	300,50000,	// unique_respawn_time, unique_max_respawn_time
	6, 8,		// level_unstatic_chance, min_unstatic_level
	-1,18348,18349,	// retire_timer, game_port, console_port
	0,200,		// spell_interfere, spell_stack_limit

	/* s16b */
	60,5,5,		// fps, newbies_cannot_drop, running_speed,
	25, 150,	// anti_scum, dun_unusual,
	32,32,		// town_x, town_y
	0, 1,		// town_base, dun_base
	127, 200,	// dun_max, store_turns

	/* char */
	3, 2,		// resting_rate, party_xp_boost
	0,			// use_pk_rules

	10, 100, 100,	// zang_monsters, pern_monsters, cth_monsters
	1, 100, 0,		// joke_monsters, vanilla_monsters, pet_monsters

	/* bool */
	TRUE,TRUE,	// report_to_meta, secret_dungeon_master
	TRUE,FALSE, // anti_arts_horde, mage_hp_bonus
	TRUE,FALSE,	// door_bump_open, no_ghost
	TRUE,TRUE,	// maximize, kings_etiquette
	FALSE,FALSE,	// public_rfe, auto_purge
};

struct ip_ban *banlist=NULL;
struct swear swear[30];

/*
 * Software options (set via the '=' command).  See "tables.c"
 */

/* Option Set 1 -- User Interface */

bool use_color;                         /* Use color if possible (slow) */

/* Option Set 2 -- Disturbance */

/* Option Set 3 -- Game-Play */

bool auto_scum;                         /* Auto-scum for good levels */
bool dungeon_align;                     /* Generate dungeons with aligned rooms */

/* Option Set 4 -- Efficiency */

bool avoid_other;                       /* Avoid processing special colors */

/* Special options */

s16b hitpoint_warn;             /* Hitpoint warning (0 to 9) */

/* The array of players */
player_type **Players;

/* The party information */
party_type parties[MAX_PARTIES];
struct guild_type guilds[MAX_GUILDS];

struct quest_type quests[20];	/* server quest data */

/* The information about houses */
house_type *houses;
u32b num_houses = 0;
u32b house_alloc = 0;

/* An array to access a Player's ID */
long GetInd[MAX_ID];

/* Buffer to hold the current savefile name */
//char savefile[MAX_PATH_LENGTH];


/*
 * Array of grids lit by player lite (see "cave.c")
 */
/*s16b lite_n;
byte lite_y[LITE_MAX];
byte lite_x[LITE_MAX];*/

/*
 * Array of grids for use by various functions (see "cave.c")
 */
s16b temp_n;
byte temp_y[TEMP_MAX];
byte temp_x[TEMP_MAX];

/*
 * The number of quarks
 */
s16b quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
cptr *quark__str;

/*
 * The array of indexes of "live" objects
 */
s16b o_fast[MAX_O_IDX];

/*
 * The array of indexes of "live" monsters
 */
s16b m_fast[MAX_M_IDX];

#if 0	// DELETEME
/*
 * The array of indexes of "live" traps
 */
s16b t_fast[MAX_TR_IDX];
#endif	// 0


/*
 * The array of "cave grids" [MAX_WID][MAX_HGT].
 * Not completely allocated, that would be inefficient
 * Not completely hardcoded, that would overflow memory
 */
/* For wilderness, I am hacking this to extend in the negative direction.
   I currently have a huge number (4096?) of wilderness levels allocated.
   -APD-
*/ 

/* I have moved the cave_type stuff to the wilderness and dungeon
   level structures now. So, to reference the cave_type array for
   the level (x,y,z) you would first check z for being zero. If it
   is, return the wilderness cave_type array pointer. Otherwise
   select the correct dungeon pointer and lookup the dungeon level
   you want. Then return its cave_type array pointer. Of course, 
   use (cave_type**)zcave=getcave(struct worldpos *).
    Evileye
 */
wilderness_type wild_info[MAX_WILD_Y][MAX_WILD_X];   /* seems my world may be a bit bigger */
struct town_type *town=NULL;
u16b numtowns=0;

/*
 * The array of dungeon items [MAX_O_IDX]
 */
object_type *o_list;

/*
 * The array of dungeon monsters [MAX_M_IDX]
 */
monster_type *m_list;

/*
 * The array of dungeon traps [MAX_T_IDX]
 * (elsewhere)
 */
// trap_type *t_list;

/*
 * Hack -- Quest array
 */
quest q_list[MAX_Q_IDX];

/*
 * The stores [MAX_STORES]
 */
store_type *store;

/*
 * The size of "alloc_kind_table" (at most MAX_K_IDX * 4)
 */
s16b alloc_kind_size;

/*
 * The entries in the "kind allocator table"
 */
alloc_entry *alloc_kind_table;


/*
 * The size of "alloc_race_table" (at most MAX_R_IDX)
 */
s16b alloc_race_size;

/*
 * The entries in the "race allocator table"
 */
alloc_entry *alloc_race_table;


/*
 * Specify attr/char pairs for inventory items (by tval)
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte tval_to_attr[128];
char tval_to_char[128];

/*
 * Simple keymap method, see "init.c" and "cmd6.c".
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte keymap_cmds[128];
byte keymap_dirs[128];


/*
 * Global table of color definitions
 * Be sure to use "index & 0xFF" to avoid illegal access
 */
byte color_table[256][4];

/*
 * The vault generation arrays
 */
header *v_head;
vault_type *v_info;
char *v_name;
char spacer;
const char spacer2;
char *v_text;

/*
 * The terrain feature arrays
 */
header *f_head;
feature_type *f_info;
char *f_name;
char *f_text;

/*
 * The object kind arrays
 */
header *k_head;
object_kind *k_info;
char *k_name;
char *k_text;

/*
 * The artifact arrays
 */
header *a_head;
artifact_type *a_info;
char *a_name;
char *a_text;

/*
 * The ego-item arrays
 */
header *e_head;
ego_item_type *e_info;
char *e_name;
char *e_text;

/* jk / Jir */
/* the trap-arrays/variables */
header *t_head;
trap_kind *t_info;
char *t_name;
char *t_text;
#if 0	// DELETEME
trap_type *t_list;
s32b t_nxt = 1;
s32b t_max = 1;
s32b t_top = 0;
#endif	// 0


/*
 * The monster race arrays
 */
header *r_head;
monster_race *r_info;
char *r_name;
char *r_text;

/*
 * The monster ego race arrays
 */
#ifdef RANDUNIS
header *re_head;
monster_ego *re_info;
char *re_name;

#endif


/*
 * Hack -- The special Angband "System Suffix"
 * This variable is used to choose an appropriate "pref-xxx" file
 */
cptr ANGBAND_SYS = "xxx";

/*
 * Path name: The main "lib" directory
 * This variable is not actually used anywhere in the code
 */
cptr ANGBAND_DIR;


/*
 * Scripts
 */
cptr ANGBAND_DIR_SCPT;

/*
 * Various data files for the game, such as the high score and
 * the mangband logs.
 */
cptr ANGBAND_DIR_DATA;

/*
 * Textual template files for the "*_info" arrays (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_GAME;

/*
 * Various user editable text files (ascii), such as the help and greeting
 * screen. These files may be portable between platforms
 */
cptr ANGBAND_DIR_TEXT;

/*
 * Savefiles for current characters (binary)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_SAVE;

/*
 * User "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_USER;

/*
 * Mangband configuration file
 * Usually it's 'tomenet.cfg'
 */
cptr MANGBAND_CFG;

/*
 * Total Hack -- allow all items to be listed (even empty ones)
 * This is only used by "do_cmd_inven_e()" and is cleared there.
 */
bool item_tester_full;


/*
 * Here is a "pseudo-hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
byte item_tester_tval;


/*
 * Here is a "hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
bool (*item_tester_hook)(object_type*);



/*
 * Current "comp" function for ang_sort()
 */
bool (*ang_sort_comp)(int Ind, vptr u, vptr v, int a, int b);


/*
 * Current "swap" function for ang_sort()
 */
void (*ang_sort_swap)(int Ind, vptr u, vptr v, int a, int b);



/*
 * Hack -- function hook to restrict "get_mon_num_prep()" function
 */
bool (*get_mon_num_hook)(int r_idx);



/*
 * Hack -- function hook to restrict "get_obj_num_prep()" function
 */
bool (*get_obj_num_hook)(int k_idx);

/* the dungeon master movement hook, is called whenever he moves
 * (to make building large buildings / summoning hoards of mosnters 
 * easier)
 */
//bool (*master_move_hook)(int Ind, char * args) = master_acquire;
//bool (*master_move_hook)(int Ind, char * args) = NULL; /*evileye - multi DMs! */


/* new Hacks */
int artifact_bias;
char summon_kin_type;

/*
 * radius tables
 */
char tdy[662];
char tdx[662];
s32b tdi[18];	// PREPARE_RADIUS + 2

/* How many times project() is called in this turn? */
#ifdef PROJECTION_FLUSH_LIMIT
s16b count_project = 0;
#endif	// PROJECTION_FLUSH_LIMIT

/* Hack -- 'default' values for obj_theme.	- Jir -
 * Only a makeshift till d_info thingie will be implemented. */
obj_theme default_obj_theme = {20, 20, 20, 20};
