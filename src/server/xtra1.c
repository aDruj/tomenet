/* $Id$ */
/* File: misc.c */

/* Purpose: misc code */

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
 * Modifier for martial-arts AC bonus; it's needed to balance martial-arts
 * and dodging skills. in percent. [50]
 */
#define MARTIAL_ARTS_AC_ADJUST	50


/*
 * Converts stat num into a six-char (right justified) string
 */
void cnv_stat(int val, char *out_val)
{
	/* Above 18 */
	if (val > 18)
	{
		int bonus = (val - 18);

		if (bonus >= 220)
		{
			sprintf(out_val, "18/%3s", "***");
		}
		else if (bonus >= 100)
		{
			sprintf(out_val, "18/%03d", bonus);
		}
		else
		{
			sprintf(out_val, " 18/%02d", bonus);
		}
	}

	/* From 3 to 18 */
	else
	{
		sprintf(out_val, "    %2d", val);
	}
}



/*
 * Modify a stat value by a "modifier", return new value
 *
 * Stats go up: 3,4,...,17,18,18/10,18/20,...,18/220
 * Or even: 18/13, 18/23, 18/33, ..., 18/220
 *
 * Stats go down: 18/220, 18/210,..., 18/10, 18, 17, ..., 3
 * Or even: 18/13, 18/03, 18, 17, ..., 3
 */
s16b modify_stat_value(int value, int amount)
{
	int i;

	/* Reward */
	if (amount > 0)
	{
		/* Apply each point */
		for (i = 0; i < amount; i++)
		{
			/* One point at a time */
			if (value < 18) value++;

			/* Ten "points" at a time */
			else value += 10;
		}
	}

	/* Penalty */
	else if (amount < 0)
	{
		/* Apply each point */
		for (i = 0; i < (0 - amount); i++)
		{
			/* Ten points at a time */
			if (value >= 18+10) value -= 10;

			/* Hack -- prevent weirdness */
			else if (value > 18) value = 18;

			/* One point at a time */
			else if (value > 3) value--;
		}
	}

	/* Return new value */
	return (value);
}





/*
 * Print character stat in given row, column
 */
static void prt_stat(int Ind, int stat)
{	
	player_type *p_ptr = Players[Ind];

	Send_stat(Ind, stat, p_ptr->stat_top[stat], p_ptr->stat_use[stat], p_ptr->stat_ind[stat]);
}




/*
 * Prints "title", including "wizard" or "winner" as needed.
 */
static void prt_title(int Ind)
{
	player_type *p_ptr = Players[Ind];

	cptr p = "";

	/* Winner */
	if (p_ptr->total_winner || (p_ptr->lev > PY_MAX_LEVEL))
	{
		p = (p_ptr->male ? "**KING**" : "**QUEEN**");
	}

	/* Normal */
	else
	{
		p = player_title[p_ptr->pclass][(p_ptr->lev-1)/10];
	}

	/* Ghost */
	if (p_ptr->ghost)
		p = "Ghost";

	Send_title(Ind, p);
}


/*
 * Prints level
 */
static void prt_level(int Ind)
{
	player_type *p_ptr = Players[Ind];

	s64b adv_exp;

	if (p_ptr->lev >= PY_MAX_LEVEL)
		adv_exp = 0;
	else adv_exp = (s64b)((s64b)player_exp[p_ptr->lev - 1] * (s64b)p_ptr->expfact / 100L);

	Send_experience(Ind, p_ptr->lev, p_ptr->max_exp, p_ptr->exp, adv_exp);
}


/*
 * Display the experience
 */
static void prt_exp(int Ind)
{
	player_type *p_ptr = Players[Ind];

	s64b adv_exp;

	if (p_ptr->lev >= PY_MAX_LEVEL)
		adv_exp = 0;
	else adv_exp = (s64b)((s64b)player_exp[p_ptr->lev - 1] * (s64b)p_ptr->expfact / 100L);

	Send_experience(Ind, p_ptr->lev, p_ptr->max_exp, p_ptr->exp, adv_exp);
}


/*
 * Prints current gold
 */
static void prt_gold(int Ind)
{
	player_type *p_ptr = Players[Ind];

	Send_gold(Ind, p_ptr->au);
}



/*
 * Prints current AC
 */
static void prt_ac(int Ind)
{
	player_type *p_ptr = Players[Ind];

	Send_ac(Ind, p_ptr->dis_ac, p_ptr->dis_to_a);
}

static void prt_sanity(int Ind)
{
#ifdef SHOW_SANITY	// No.
	player_type *p_ptr=Players[Ind];
#if 0
	Send_sanity(Ind, p_ptr->msane, p_ptr->csane);
#else	// 0
	char buf[20];
	byte attr = TERM_L_GREEN;
	int skill = get_skill(p_ptr, SKILL_HEALTH);
	int ratio;
	ratio = p_ptr->msane ? (p_ptr->csane * 100) / p_ptr->msane : 100;

	/* Vague */
	if (ratio < 0)
	{
		/* This guy should be dead - for tombstone */
		attr = TERM_RED;
		strcpy(buf, "Vegetable");
	}
	else if (ratio < 10)
	{
//		attr = TERM_RED;
		attr = TERM_MULTI;
		strcpy(buf, "      Mad");
	}
	else if (ratio < 25)
	{
		attr = TERM_ORANGE;
		strcpy(buf, "   Insane");
	}
	else if (ratio < 50)
	{
		attr = TERM_YELLOW;
		strcpy(buf, "    Weird");
	}
	else if (ratio < 75)
	{
		attr = TERM_GREEN;
		strcpy(buf, "     Sane");
	}
	else
	{
		attr = TERM_L_GREEN;
		strcpy(buf, "    Sound");
	}

	/* Full */
	if (skill >= 40)
	{
		sprintf(buf, "%4d/%4d", p_ptr->csane, p_ptr->msane);
	}
	/* Percentile */
	else if (skill >= 20)
	{
		sprintf(buf, "     %3d%%", ratio);
	}
	/* Sanity Bar */
	else if (skill >= 10)
	{
		int tmp = ratio / 11;
		strcpy(buf, "---------");
		if (tmp > 0) strncpy(buf, "*********", tmp);
	}

	/* Terminate */
	buf[9] = '\0';

	/* Send it */
	Send_sanity(Ind, attr, &buf);

#endif	// 0
#endif	// SHOW_SANITY
}

/*
 * Prints Cur/Max hit points
 */
static void prt_hp(int Ind)
{
	player_type *p_ptr = Players[Ind];

	Send_hp(Ind, p_ptr->mhp, p_ptr->chp);
}

/*
 * Prints players max/cur spell points
 */
static void prt_sp(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Do not show mana unless it matters */
	Send_sp(Ind, p_ptr->msp, p_ptr->csp);
}


/*
 * Prints depth in stat area
 */
static void prt_depth(int Ind)
{
	player_type *p_ptr = Players[Ind];

	Send_depth(Ind, &p_ptr->wpos);
}


/*
 * Prints status of hunger
 */
static void prt_hunger(int Ind)
{
	player_type *p_ptr = Players[Ind];

	Send_food(Ind, p_ptr->food);
}


/*
 * Prints Blind status
 */
static void prt_blind(int Ind)
{
	player_type *p_ptr = Players[Ind];

	if (p_ptr->blind)
	{
		Send_blind(Ind, TRUE);
	}
	else
	{
		Send_blind(Ind, FALSE);
	}
}


/*
 * Prints Confusion status
 */
static void prt_confused(int Ind)
{
	player_type *p_ptr = Players[Ind];

	if (p_ptr->confused)
	{
		Send_confused(Ind, TRUE);
	}
	else
	{
		Send_confused(Ind, FALSE);
	}
}


/*
 * Prints Fear status
 */
static void prt_afraid(int Ind)
{
	player_type *p_ptr = Players[Ind];

	if (p_ptr->afraid)
	{
		Send_fear(Ind, TRUE);
	}
	else
	{
		Send_fear(Ind, FALSE);
	}
}


/*
 * Prints Poisoned status
 */
static void prt_poisoned(int Ind)
{
	player_type *p_ptr = Players[Ind];

	if (p_ptr->poisoned)
	{
		Send_poison(Ind, TRUE);
	}
	else
	{
		Send_poison(Ind, FALSE);
	}
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
static void prt_state(int Ind)
{
	player_type *p_ptr = Players[Ind];

	bool p, s, r;

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		p = TRUE;
	}
	else
	{
		p = FALSE;
	}

	/* Searching */
	if (p_ptr->searching)
	{
		s = TRUE;
	}
	else
	{
		s = FALSE;
	}

	/* Resting */
	if (p_ptr->resting)
	{
		r = TRUE;
	}
	else
	{
		r = FALSE;
	}

	Send_state(Ind, p, s, r);
}


/*
 * Prints the speed of a character.			-CJS-
 */
static void prt_speed(int Ind)
{
	player_type *p_ptr = Players[Ind];

	int i = p_ptr->pspeed;

#if 0	/* methinks we'd better tell it to players.. - Jir - */
	/* Hack -- Visually "undo" the Search Mode Slowdown */
	/* And this formula can be wrong for hellish */
//	if (p_ptr->searching) i+=(p_ptr->mode&MODE_HELL ? 5 : 10);
	if (p_ptr->searching) i += 10;
#endif	// 0

	Send_speed(Ind, i - 110);
}


static void prt_study(int Ind)
{
	player_type *p_ptr = Players[Ind];

	if (p_ptr->skill_points)
	{
		Send_study(Ind, TRUE);
	}
	else
	{
		Send_study(Ind, FALSE);
	}
}


static void prt_cut(int Ind)
{
	player_type *p_ptr = Players[Ind];

	int c = p_ptr->cut;

	Send_cut(Ind, c);
}



static void prt_stun(int Ind)
{
	player_type *p_ptr = Players[Ind];

	int s = p_ptr->stun;

	Send_stun(Ind, s);
}

static void prt_history(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int i;

	for (i = 0; i < 4; i++)
	{
		Send_history(Ind, i, p_ptr->history[i]);
	}
}

static void prt_various(int Ind)
{
	player_type *p_ptr = Players[Ind];

	Send_various(Ind, p_ptr->ht, p_ptr->wt, p_ptr->age, p_ptr->sc, r_name + r_info[p_ptr->body_monster].name);
}

static void prt_plusses(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int show_tohit = p_ptr->dis_to_h;
	int show_todam = p_ptr->dis_to_d;

	object_type *o_ptr = &p_ptr->inventory[INVEN_WIELD];

	if (object_known_p(Ind, o_ptr)) show_tohit += o_ptr->to_h;
	if (object_known_p(Ind, o_ptr)) show_todam += o_ptr->to_d;

	Send_plusses(Ind, show_tohit, show_todam, p_ptr->to_h_ranged, p_ptr->to_d_ranged, p_ptr->to_h_melee, p_ptr->to_d_melee);
}

static void prt_skills(int Ind)
{
	Send_skills(Ind);
}


/*
 * Redraw the "monster health bar"	-DRS-
 * Rather extensive modifications by	-BEN-
 *
 * The "monster health bar" provides visual feedback on the "health"
 * of the monster currently being "tracked".  There are several ways
 * to "track" a monster, including targetting it, attacking it, and
 * affecting it (and nobody else) with a ranged attack.
 *
 * Display the monster health bar (affectionately known as the
 * "health-o-meter").  Clear health bar if nothing is being tracked.
 * Auto-track current target monster when bored.  Note that the
 * health-bar stops tracking any monster that "disappears".
 */
 
 
static void health_redraw(int Ind)
{
	player_type *p_ptr = Players[Ind];

#ifdef DRS_SHOW_HEALTH_BAR

	/* Not tracking */
	if (p_ptr->health_who == 0)
	{
		/* Erase the health bar */
		Send_monster_health(Ind, 0, 0);
	}

	/* Tracking a hallucinatory monster */
	else if (p_ptr->image)
	{
		/* Indicate that the monster health is "unknown" */
		Send_monster_health(Ind, 0, TERM_WHITE);
	}

	/* Tracking a player */
	else if (p_ptr->health_who < 0)
	{
		player_type *q_ptr = Players[0 - p_ptr->health_who];

		if(Players[Ind]->conn == NOT_CONNECTED ) {
			/* Send_monster_health(Ind, 0, 0); */
			return;
		};

		if(0-p_ptr->health_who < NumPlayers) {
			if(Players[0-p_ptr->health_who]->conn == NOT_CONNECTED ) {
				Send_monster_health(Ind, 0, 0); 
				return;
			};
		} else {
			Send_monster_health(Ind, 0, 0); 
			return;
		}
			

		/* Tracking a bad player (?) */
		if (!q_ptr)
		{
			/* Erase the health bar */
			Send_monster_health(Ind, 0, 0);
		}

		/* Tracking an unseen player */
		else if (!p_ptr->play_vis[0 - p_ptr->health_who])
		{
			/* Indicate that the player health is "unknown" */
			Send_monster_health(Ind, 0, TERM_WHITE);
		}

		/* Tracking a visible player */
		else
		{
			int pct, len;

			/* Default to almost dead */
			byte attr = TERM_RED;

			/* Extract the "percent" of health */
			pct = 100L * q_ptr->chp / q_ptr->mhp;

			/* Badly wounded */
			if (pct >= 10) attr = TERM_L_RED;

			/* Wounded */
			if (pct >= 25) attr = TERM_ORANGE;

			/* Somewhat Wounded */
			if (pct >= 60) attr = TERM_YELLOW;

			/* Healthy */
			if (pct >= 100) attr = TERM_L_GREEN;

			/* Afraid */
			if (q_ptr->afraid) attr = TERM_VIOLET;

			/* Asleep (?) */
			if (q_ptr->paralyzed) attr = TERM_BLUE;

			/* Convert percent into "health" */
			len = (pct < 10) ? 1 : (pct < 90) ? (pct / 10 + 1) : 10;

			/* Send the health */
			Send_monster_health(Ind, len, attr);
		}
	}

	/* Tracking a bad monster (?) */
	else if (!m_list[p_ptr->health_who].r_idx)
	{
		/* Erase the health bar */
		Send_monster_health(Ind, 0, 0);
	}

	/* Tracking an unseen monster */
	else if (!p_ptr->mon_vis[p_ptr->health_who])
	{
		/* Indicate that the monster health is "unknown" */
		Send_monster_health(Ind, 0, TERM_WHITE);
	}

	/* Tracking a dead monster (???) */
	else if (!m_list[p_ptr->health_who].hp < 0)
	{
		/* Indicate that the monster health is "unknown" */
		Send_monster_health(Ind, 0, TERM_WHITE);
	}

	/* Tracking a visible monster */
	else
	{
		int pct, len;

		monster_type *m_ptr = &m_list[p_ptr->health_who];

		/* Default to almost dead */
		byte attr = TERM_RED;

		/* Extract the "percent" of health */
		pct = 100L * m_ptr->hp / m_ptr->maxhp;

		/* Badly wounded */
		if (pct >= 10) attr = TERM_L_RED;

		/* Wounded */
		if (pct >= 25) attr = TERM_ORANGE;

		/* Somewhat Wounded */
		if (pct >= 60) attr = TERM_YELLOW;

		/* Healthy */
		if (pct >= 100) attr = TERM_L_GREEN;

		/* Afraid */
		if (m_ptr->monfear) attr = TERM_VIOLET;

		/* Asleep */
		if (m_ptr->csleep) attr = TERM_BLUE;

		/* Convert percent into "health" */
		len = (pct < 10) ? 1 : (pct < 90) ? (pct / 10 + 1) : 10;

		/* Send the health */
		Send_monster_health(Ind, len, attr);
	}

#endif

}



/*
 * Display basic info (mostly left of map)
 */
static void prt_frame_basic(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int i;

	/* Race and Class */
	Send_char_info(Ind, p_ptr->prace, p_ptr->pclass, p_ptr->male);

	/* Title */
	prt_title(Ind);

	/* Level/Experience */
	prt_level(Ind);
	prt_exp(Ind);

	/* All Stats */
	for (i = 0; i < 6; i++) prt_stat(Ind, i);

	/* Armor */
	prt_ac(Ind);

	/* Hitpoints */
	prt_hp(Ind);

	/* Sanity */
#ifdef SHOW_SANITY
	prt_sanity(Ind);
#endif

	/* Spellpoints */
	prt_sp(Ind);

	/* Gold */
	prt_gold(Ind);

	/* Current depth */
	prt_depth(Ind);

	/* Special */
	health_redraw(Ind);
}


/*
 * Display extra info (mostly below map)
 */
static void prt_frame_extra(int Ind)
{
	/* Cut/Stun */
	prt_cut(Ind);
	prt_stun(Ind);

	/* Food */
	prt_hunger(Ind);

	/* Various */
	prt_blind(Ind);
	prt_confused(Ind);
	prt_afraid(Ind);
	prt_poisoned(Ind);

	/* State */
	prt_state(Ind);

	/* Speed */
	prt_speed(Ind);

	/* Study spells */
	prt_study(Ind);
}



/*
 * Hack -- display inventory in sub-windows
 */
static void fix_inven(int Ind)
{
	/* Resend the inventory */
	display_inven(Ind);
}



/*
 * Hack -- display equipment in sub-windows
 */
static void fix_equip(int Ind)
{
	/* Resend the equipment */
	display_equip(Ind);
}

static void fix_spell_aux(int Ind, int realm, int sval)
{
	int i;
	byte		spell[64], num = 0;

	/* Extract spells */
	for (i = 0; i < 64; i++)
	{
		/* Check for this spell */
		if ((i < 32) ?
		    (spell_flags[realm][sval][0] & (1L << i)) :
		    (spell_flags[realm][sval][1] & (1L << (i - 32))))
		{
			/* Collect this spell */
			spell[num++] = i;
		}
	}


	/* Display the spells */
	print_spells(Ind, realm, sval, spell, num);
}

/*
 * Hack -- display equipment in sub-windows
 */
/*
 * NOTE: Seemingly, this function has different meanings than Vanilla -
 * It simply send the spell informations to the client.
 *
 * Probably we'd better call this function only once when a player logs on.
 * XXX XXX target of spell-reform
 */
void fix_spell(int Ind, bool full)
{
	player_type *p_ptr = Players[Ind];
	int i, j;
	object_type forge;

	/* Ghosts get a different set */
	if (p_ptr->ghost)
	{
		show_ghost_spells(Ind);
		return;
	}

#if 0	// this should be done elsewhere (tho I'm not sure where)
	/* Check for blindness and no lite and confusion */
	if (p_ptr->blind || no_lite(Ind) || p_ptr->confused)
	{
		return;
	}
#endif	// 0

	/* Scan for appropriate books */
#if 0	// pfft, visit the magic shop and eXamine books; pls fix more nicely
	/* sending all spells ?*/
	/* This is *still* far from adequate 
	   we should only send individual spells that
	   are changed - evileye */
	/* idea is to send (like mentioned above) all at start
	   and only updates after that */
	if(!full){
		for (i = 0; i < INVEN_WIELD; i++)
		{
			if (is_book((&p_ptr->inventory[i])))
			{
				do_cmd_browse(Ind, &p_ptr->inventory[i]);
			}
		}
	}
	else{
		for (i = TV_PSI_BOOK; i < TV_HUNT_BOOK + 1; i++)
		{
			for (j = 0; j < 9; j++)
			{
				if (!lookup_kind(i, j)) continue;
				forge.tval = i;
				forge.sval = j;
				do_cmd_browse(Ind, &forge);
			}
		}
	}
#else
	for (i = 0; i < MAX_REALM; i++)
		for (j = 0; j < 9; j++)
			fix_spell_aux(Ind, i, j);
#endif
}


/*
 * Hack -- display character in sub-windows
 */
static void fix_player(int Ind)
{
}



/*
 * Hack -- display recent messages in sub-windows
 *
 * XXX XXX XXX Adjust for width and split messages
 */
static void fix_message(int Ind)
{
}


/*
 * Hack -- display overhead view in sub-windows
 *
 * Note that the "player" symbol does NOT appear on the map.
 */
static void fix_overhead(int Ind)
{
}


/*
 * Hack -- display monster recall in sub-windows
 */
static void fix_monster(int Ind)
{
}


/*
 * Calculate number of spells player should have, and forget,
 * or remember, spells until that number is properly reflected.
 *
 * Note that this function induces various "status" messages,
 * which must be bypasses until the character is created.
 */
static void calc_spells(int Ind)
{
	player_type *p_ptr = Players[Ind];

	int			i, j, k, levels;
	int			num_allowed, num_known;

	magic_type		*s_ptr;

	cptr p = "spell";

#if 0 // DGDGDGDG -- no more spell learning

	/* Determine the number of spells allowed */
	levels = p_ptr->lev;

	/* Hack -- no negative spells */
	if (levels < 0) levels = 0;

	/* Extract total allowed spells */
	num_allowed = (adj_mag_study[p_ptr->stat_ind[p_ptr->mp_ptr->spell_stat]] *
		levels / 2);

	/* Assume none known */
	num_known = 0;

	/* Count the number of spells we know */
        for (j = 0; j < 64; j++)
	{
		/* Count known spells */
		if ((j < 32) ?
			(p_ptr->spell_learned1 & (1L << j)) :
			(p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			num_known++;
		}
	}

	/* See how many spells we must forget or may learn */
	p_ptr->new_spells = num_allowed - num_known;



	/* Forget spells which are too hard */
	for (i = 63; i >= 0; i--)
	{
		/* Efficiency -- all done */
		if (!p_ptr->spell_learned1 && !p_ptr->spell_learned2) break;

		/* Access the spell */
		j = p_ptr->spell_order[i];

		/* Skip non-spells */
		if (j >= 99) continue;

		/* Get the spell */
		s_ptr = &p_ptr->mp_ptr->info[j];

		/* Skip spells we are allowed to know */
		if (s_ptr->slevel <= p_ptr->lev) continue;

		/* Is it known? */
		if ((j < 32) ?
			(p_ptr->spell_learned1 & (1L << j)) :
			(p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 |= (1L << j);
			}
			else
			{
				p_ptr->spell_forgotten2 |= (1L << (j - 32));
			}

			/* No longer known */
			if (j < 32)
			{
				p_ptr->spell_learned1 &= ~(1L << j);
			}
			else
			{
				p_ptr->spell_learned2 &= ~(1L << (j - 32));
			}

			/* Message */
			msg_format(Ind, "You have forgotten the %s of %s.", p,
				spell_names[p_ptr->mp_ptr->spell_type][j]);

			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Forget spells if we know too many spells */
	for (i = 63; i >= 0; i--)
	{
		/* Stop when possible */
		if (p_ptr->new_spells >= 0) break;

		/* Efficiency -- all done */
		if (!p_ptr->spell_learned1 && !p_ptr->spell_learned2) break;

		/* Get the (i+1)th spell learned */
		j = p_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) continue;

		/* Forget it (if learned) */
		if ((j < 32) ?
			(p_ptr->spell_learned1 & (1L << j)) :
			(p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 |= (1L << j);
			}
			else
			{
				p_ptr->spell_forgotten2 |= (1L << (j - 32));
			}

			/* No longer known */
			if (j < 32)
			{
				p_ptr->spell_learned1 &= ~(1L << j);
			}
			else
			{
				p_ptr->spell_learned2 &= ~(1L << (j - 32));
			}

			/* Message */
			msg_format(Ind, "You have forgotten the %s of %s.", p,
				spell_names[p_ptr->mp_ptr->spell_type][j]);

			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Check for spells to remember */
	for (i = 0; i < 64; i++)
	{
		/* None left to remember */
		if (p_ptr->new_spells <= 0) break;

		/* Efficiency -- all done */
		if (!p_ptr->spell_forgotten1 && !p_ptr->spell_forgotten2) break;

		/* Get the next spell we learned */
		j = p_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) break;

		/* Access the spell */
		s_ptr = &p_ptr->mp_ptr->info[j];

		/* Skip spells we cannot remember */
		if (s_ptr->slevel > p_ptr->lev) continue;

		/* First set of spells */
		if ((j < 32) ?
			(p_ptr->spell_forgotten1 & (1L << j)) :
			(p_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			/* No longer forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 &= ~(1L << j);
			}
			else
			{
				p_ptr->spell_forgotten2 &= ~(1L << (j - 32));
			}

			/* Known once more */
			if (j < 32)
			{
				p_ptr->spell_learned1 |= (1L << j);
			}
			else
			{
				p_ptr->spell_learned2 |= (1L << (j - 32));
			}

			/* Message */
			msg_format(Ind, "You have remembered the %s of %s.",
				p, spell_names[p_ptr->mp_ptr->spell_type][j]);

			/* One less can be learned */
			p_ptr->new_spells--;
		}
	}


	/* Assume no spells available */
	k = 0;

	/* Count spells that can be learned */
	for (j = 0; j < 64; j++)
	{
		/* Access the spell */
		s_ptr = &p_ptr->mp_ptr->info[j];

		/* Skip spells we cannot remember */
		if (s_ptr->slevel > p_ptr->lev) continue;

		/* Skip spells we already know */
		if ((j < 32) ?
			(p_ptr->spell_learned1 & (1L << j)) :
			(p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			continue;
		}

		/* Count it */
		k++;
	}

	/* Cannot learn more spells than exist */
	if (p_ptr->new_spells > k) p_ptr->new_spells = k;

	/* Learn new spells */
	if (p_ptr->new_spells && !p_ptr->old_spells)
	{
		/* Message */
		msg_format(Ind, "You can learn some new %ss now.", p);

		/* Display "study state" later */
		p_ptr->redraw |= (PR_STUDY);
	}

	/* No more spells */
	else if (!p_ptr->new_spells && p_ptr->old_spells)
	{
		/* Display "study state" later */
		p_ptr->redraw |= (PR_STUDY);
	}

	/* Save the new_spells value */
	p_ptr->old_spells = p_ptr->new_spells;
#endif
}


/*
 * Calculate the player's sanity
 */

void calc_sanity(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int bonus, msane;
	/* Don't make the capacity too large */
	int lev = p_ptr->lev > 50 ? 50 : p_ptr->lev;

	/* Hack -- use the con/hp table for sanity/wis */
	bonus = ((int)(adj_con_mhp[p_ptr->stat_ind[A_WIS]]) - 128);

	/* Hack -- assume 5 sanity points per level. */
	msane = 5*(lev+1) + (bonus * lev / 2);

	if (msane < lev + 1) msane = lev + 1;

	if (p_ptr->msane != msane) {

		/* Sanity carries over between levels. */
		p_ptr->csane += (msane - p_ptr->msane);

		p_ptr->msane = msane;

		if (p_ptr->csane >= msane) {
			p_ptr->csane = msane;
			p_ptr->csane_frac = 0;
		}

		p_ptr->redraw |= (PR_SANITY);
//		p_ptr->window |= (PW_SPELL | PW_PLAYER);
		p_ptr->window |= (PW_PLAYER);
	}
}


/*
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
static void calc_mana(int Ind)
{
	player_type *p_ptr = Players[Ind], *p_ptr2;

	int levels, cur_wgt, max_wgt;
	s32b new_mana;

	object_type	*o_ptr;

	u32b f1, f2, f3, f4, f5, esp;

	int Ind2 = 0;


	if (p_ptr->esp_link_type && p_ptr->esp_link && (p_ptr->esp_link_flags & LINKF_PAIN))
	  {
		Ind2 = find_player(p_ptr->esp_link);

		if (!Ind2)
			end_mind(Ind, FALSE);
		else
		{
			p_ptr2 = Players[Ind2];
		}
	}

	/* Extract "effective" player level */
	levels = p_ptr->lev;

	/* Hack -- no negative mana */
	if (levels < 0) levels = 0;

	/* Extract total mana */
	new_mana = get_skill_scale(p_ptr, SKILL_MAGIC, 200) + (adj_mag_mana[p_ptr->stat_ind[A_INT]] * levels / 5) + (adj_mag_mana[p_ptr->stat_ind[A_WIS]] * levels / 5);

	/* Hack -- usually add one mana */
	if (new_mana) new_mana++;

	/* Get the gloves */
	o_ptr = &p_ptr->inventory[INVEN_HANDS];

	/* Examine the gloves */
	object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

	/* Only Sorcery/Magery users are affected */
	if (get_skill(p_ptr, SKILL_SORCERY) || get_skill(p_ptr, SKILL_MAGERY))
	{
		/* Assume player is not encumbered by gloves */
		p_ptr->cumber_glove = FALSE;

		/* Normal gloves hurt mage-type spells */
		if (o_ptr->k_idx &&
		    !(f2 & TR2_FREE_ACT) &&
		    !((f1 & TR1_DEX) && (o_ptr->pval > 0)))
		{
			/* Encumbered */
			p_ptr->cumber_glove = TRUE;

			/* Reduce mana */
			new_mana = (3 * new_mana) / 4;
		}
	}

	if (new_mana <= 0) new_mana = 1;

	/* Sorcery helps mana */
	if (get_skill(p_ptr, SKILL_SORCERY))
	{
		new_mana += (new_mana * get_skill(p_ptr, SKILL_SORCERY)) / 100;
	}
#if 0 // DGDGDGDG
	/* Mimic really need that */
	if (p_ptr->pclass == CLASS_MIMIC)
	{
		new_mana = (new_mana * 7) / 10;
		if (new_mana < 1) new_mana = 1;
        }
#endif

	if (p_ptr->to_m) new_mana += new_mana * p_ptr->to_m / 10;
	
	/* Meditation increase mana at the cost of hp */
	if (p_ptr->tim_meditation)
	{
		new_mana += (new_mana * get_skill(p_ptr, SKILL_SORCERY)) / 100;
	}

	/* Disruption Shield now increases hp at the cost of mana */
	if (p_ptr->tim_manashield)
	{
	/* commented out (evileye for power) */
	/*	new_mana -= new_mana / 2; */
	}
#if 0 // DGDGDG
	/* Warrior dont get much */
	if (p_ptr->pclass == CLASS_WARRIOR)
	{
		new_mana /= 8;
		if (!new_mana) new_mana++;
	}
#endif
	/* Assume player not encumbered by armor */
	p_ptr->cumber_armor = FALSE;

	/* Weigh the armor */
	cur_wgt = armour_weight(p_ptr);

	/* Determine the weight allowance */
	max_wgt = 200 + get_skill_scale(p_ptr, SKILL_COMBAT, 500);

	/* Heavy armor penalizes mana */
	if (((cur_wgt - max_wgt) / 10) > 0)
	{
		/* Encumbered */
		p_ptr->cumber_armor = TRUE;

		/* Reduce mana */
		new_mana -= ((cur_wgt - max_wgt) * 2 / 3);
	}

	if (Ind2)
	{
		new_mana += p_ptr2->msp / 2;
	}

	/* Mana can never be negative */
	if (new_mana < 0) new_mana = 0;


	/* Maximum mana has changed */
	if (p_ptr->msp != new_mana)
	{
		/* Player has no mana now */
		if (!new_mana)
		{
			/* No mana left */
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}

		/* Player had no mana, has some now */
		else if (!p_ptr->msp)
		{
			/* Reset mana */
#if 0 /* completely cheezable restoration */
			p_ptr->csp = new_mana;
#endif
			p_ptr->csp_frac = 0;
		}

		/* Player had some mana, adjust current mana */
		else
		{
			s32b value;

			/* change current mana proportionately to change of max mana, */
			/* divide first to avoid overflow, little loss of accuracy */
			value = ((((long)p_ptr->csp << 16) + p_ptr->csp_frac) /
				p_ptr->msp * new_mana);

			/* Extract mana components */
			p_ptr->csp = (value >> 16);
			p_ptr->csp_frac = (value & 0xFFFF);
		}

		/* Save new mana */
		p_ptr->msp = new_mana;

		/* Display mana later */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}


	/* Take note when "glove state" changes */
	if (p_ptr->old_cumber_glove != p_ptr->cumber_glove)
	{
		/* Message */
		if (p_ptr->cumber_glove)
		{
			msg_print(Ind, "Your covered hands feel unsuitable for spellcasting.");
		}
		else
		{
			msg_print(Ind, "Your hands feel more suitable for spellcasting.");
		}

		/* Save it */
		p_ptr->old_cumber_glove = p_ptr->cumber_glove;
	}


	/* Take note when "armor state" changes */
	if (p_ptr->old_cumber_armor != p_ptr->cumber_armor)
	{
		/* Message */
		if (p_ptr->cumber_armor)
		{
			msg_print(Ind, "The weight of your armor encumbers your movement.");
		}
		else
		{
			msg_print(Ind, "You feel able to move more freely.");
		}

		/* Save it */
		p_ptr->old_cumber_armor = p_ptr->cumber_armor;
	}
}



/*
 * Calculate the players (maximal) hit points
 * Adjust current hitpoints if necessary
 */
 
/* An option of giving mages an extra hit point per level has been added,
 * to hopefully facilitate them making it down to 1600ish and finding  
 * Constitution potions.  This should probably be changed to stop after level
 * 30.
 */

static void calc_hitpoints(int Ind)
{
	player_type *p_ptr = Players[Ind], *p_ptr2;

//	object_type *o_ptr;
//	u32b f1, f2, f3, f4, f5, esp;

	int bonus, mhp, Ind2 = 0;

	if (p_ptr->esp_link_type && p_ptr->esp_link && (p_ptr->esp_link_flags & LINKF_PAIN))
	  {
		Ind2 = find_player(p_ptr->esp_link);

		if (!Ind2)
	      end_mind(Ind, FALSE);
	    else
	      {
		p_ptr2 = Players[Ind2];
	      }
	  }


	/* Un-inflate "half-hitpoint bonus per level" value */
	bonus = ((int)(adj_con_mhp[p_ptr->stat_ind[A_CON]]) - 128);

	/* Calculate hitpoints */
	if (p_ptr->fruit_bat) mhp = (p_ptr->player_hp[p_ptr->lev-1] / 4) + (bonus * p_ptr->lev);
	else mhp = p_ptr->player_hp[p_ptr->lev-1] + (bonus * p_ptr->lev / 2);

	if (p_ptr->body_monster)
	  {
	    int rhp = r_info[p_ptr->body_monster].hdice * r_info[p_ptr->body_monster].hside;

	    mhp = (mhp * 6 / 10) + (rhp * 3 / 22);
	  }

	/* Always have at least one hitpoint per level */
	if (mhp < p_ptr->lev + 1) mhp = p_ptr->lev + 1;

#if 0 // DGDGDGDG why ?
	/* Option : give mages a bonus hitpoint / lvl */
	if (cfg.mage_hp_bonus)
		if (p_ptr->pclass == CLASS_MAGE) mhp += p_ptr->lev;
#endif

	/* Factor in the hero / superhero settings */
	if (p_ptr->hero) mhp += 10;
	if (p_ptr->shero) mhp += 30;
	
	if (p_ptr->fury) mhp += 40;
	
	/* Meditation increase mana at the cost of hp */
	if (p_ptr->tim_meditation)
	{
		mhp = mhp * 3 / 5;
	}

        /* Sorcery reduces hp */
	if (get_skill(p_ptr, SKILL_SORCERY))
	{
		mhp -= (mhp * get_skill_scale(p_ptr, SKILL_SORCERY, 20)) / 100;
	}

	/* Disruption Shield */
	if (p_ptr->tim_manashield)
	{
	/* commented out (evileye for power) */
	/*	mhp += p_ptr->msp * 2 / 3; */
	}

	mhp += mhp * p_ptr->to_l / 10;

	/* New maximum hitpoints */
	if (mhp != p_ptr->mhp)
	{
		s32b value;

		/* change current hit points proportionately to change of mhp */
		/* divide first to avoid overflow, little loss of accuracy */
		value = (((long)p_ptr->chp << 16) + p_ptr->chp_frac) / p_ptr->mhp;
		value = value * mhp;
		p_ptr->chp = (value >> 16);
		p_ptr->chp_frac = (value & 0xFFFF);

		/* Save the new max-hitpoints */
		p_ptr->mhp = mhp;

		/* Display hitpoints (later) */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}



/*
 * Extract and set the current "lite radius"
 */
/*
 * XXX currently, this function does almost nothing; if lite radius
 * should be changed, call calc_bonuses too.
 */
static void calc_torch(int Ind)
{
	player_type *p_ptr = Players[Ind];
	
	object_type *o_ptr = &p_ptr->inventory[INVEN_LITE];

#if 0
	/* Base light radius */
	p_ptr->cur_lite = p_ptr->lite;

	/* Examine actual lites */
	if (o_ptr->tval == TV_LITE)
	{
		/* Torches (with fuel) provide some lite */
		if ((o_ptr->sval == SV_LITE_TORCH) && (o_ptr->pval > 0))
		{
			p_ptr->cur_lite += 1;
		}

		/* Lanterns (with fuel) provide more lite */
		if ((o_ptr->sval == SV_LITE_LANTERN) && (o_ptr->pval > 0))
		{
			p_ptr->cur_lite += 2;
		}

		/* Dwarven lanterns provide permanent radius 2 lite */
		if (o_ptr->sval == SV_LITE_DWARVEN)
		{
			p_ptr->cur_lite += 2;
		}

		/* Feanorian lanterns provide permanent, bright, lite */
		if (o_ptr->sval == SV_LITE_FEANOR)
		{
			p_ptr->cur_lite += 3;
		}

		/* Artifact Lites provide permanent, bright, lite */
		if (artifact_p(o_ptr)) p_ptr->cur_lite += 3;
	}
#endif	// 0

	/* Reduce lite when running if requested */
	if (p_ptr->running && p_ptr->view_reduce_lite)
	{
		/* Reduce the lite radius if needed */
		if (p_ptr->cur_lite > 1) p_ptr->cur_lite = 1;
	}

	/* Notice changes in the "lite radius" */
	if (p_ptr->old_lite != p_ptr->cur_lite)
	{
		/* Update the lite */
		p_ptr->update |= (PU_LITE);

		/* Update the monsters */
		p_ptr->update |= (PU_MONSTERS);

		/* Remember the old lite */
		p_ptr->old_lite = p_ptr->cur_lite;
	}
}



/*
 * Computes current weight limit.
 */
static int weight_limit(int Ind)
{
	player_type *p_ptr = Players[Ind];

	int i;

	/* Weight limit based only on strength */
	i = adj_str_wgt[p_ptr->stat_ind[A_STR]] * 100;

	/* Return the result */
	return (i);
}


/* Should be called by every calc_bonus call */
/* TODO: allow ego form */
void calc_body_bonus(int Ind)
{
	player_type *p_ptr = Players[Ind];

	int d, i, j, toac = 0, body = 0;
	bool wepless = FALSE;
	monster_race *r_ptr = &r_info[p_ptr->body_monster];

	/* If in the player body nothing have to be done */
	if (!p_ptr->body_monster) return;

	if (!r_ptr->body_parts[BODY_WEAPON])
	{
		wepless = TRUE;
		p_ptr->num_blow = 0;
	}

	d = 0;
	for (i = 0; i < 4; i++)
	{
		j = (r_ptr->blow[i].d_dice * r_ptr->blow[i].d_side);

		/* Hack -- weaponless combat */
		if (wepless && j)
		{
			p_ptr->num_blow++;
			j *= 2;
		}

		d += j;
	}
	d /= 8;		// 7
	p_ptr->to_d += d;
	p_ptr->dis_to_d += d;

	/* Evaluate monster AC (if skin or armor etc) */
	body = (r_ptr->body_parts[BODY_HEAD] ? 1 : 0)
		+ (r_ptr->body_parts[BODY_TORSO] ? 3 : 0)
		+ (r_ptr->body_parts[BODY_ARMS] ? 2 : 0)
		+ (r_ptr->body_parts[BODY_LEGS] ? 1 : 0);

	toac = r_ptr->ac * 7 / (4 + body);
	p_ptr->ac += toac;
	p_ptr->dis_ac += toac;
	p_ptr->pspeed = r_ptr->speed;

	/* Base skill -- searching ability */
	p_ptr->skill_srh /= 2;
	p_ptr->skill_srh += r_ptr->aaf / 10;

	/* Base skill -- searching frequency */
	p_ptr->skill_fos /= 2;
	p_ptr->skill_fos += r_ptr->aaf / 10;

	/* Extra fire if good archer */
	if (r_ptr->flags4 & RF4_ARROW_1)
	{
		p_ptr->num_fire++;	// Free
		if (r_ptr->freq_inate > 30) p_ptr->num_fire++;	// 1_IN_3
		if (r_ptr->freq_inate > 60) p_ptr->num_fire++;	// 1_IN_1
	}
	else
	/* Extra casting if good spellcaster */
	{
		if (r_ptr->freq_inate > 30) p_ptr->num_spell++;	// 1_IN_3
		if (r_ptr->freq_inate > 60) p_ptr->num_spell++;	// 1_IN_1
	}


	//        if(r_ptr->flags1 & RF1_NEVER_MOVE) p_ptr->immovable = TRUE;
	if(r_ptr->flags2 & RF2_STUPID) p_ptr->stat_add[A_INT] -= 1;
	if(r_ptr->flags2 & RF2_SMART) p_ptr->stat_add[A_INT] += 1;
	if(r_ptr->flags2 & RF2_REFLECTING) p_ptr->reflect = TRUE;
	if(r_ptr->flags2 & RF2_INVISIBLE){
		//		p_ptr->tim_invisibility = 100;
		p_ptr->tim_invis_power = p_ptr->lev * 4 / 5;
	}
	if(r_ptr->flags2 & RF2_REGENERATE) p_ptr->regenerate = TRUE;
	if(r_ptr->flags2 & RF2_PASS_WALL) p_ptr->tim_wraith = 30000;
	//        if(r_ptr->flags2 & RF2_KILL_WALL) p_ptr->auto_tunnel = 100;
	/* quick hack */
	if(r_ptr->flags2 & RF2_KILL_WALL) p_ptr->skill_dig = 20000;
	if(r_ptr->flags2 & RF2_AURA_FIRE) p_ptr->sh_fire = TRUE;
	if(r_ptr->flags2 & RF2_AURA_ELEC) p_ptr->sh_elec = TRUE;

	if(r_ptr->flags3 & RF3_SUSCEP_FIRE) p_ptr->sensible_fire = TRUE;
//	if(r_ptr->flags3 & RF3_SUSCEP_COLD) p_ptr->sensible_cold = TRUE;
	if(r_ptr->flags3 & RF3_IM_ACID) p_ptr->resist_acid = TRUE;
	if(r_ptr->flags3 & RF3_IM_ELEC) p_ptr->resist_elec = TRUE;
	if(r_ptr->flags3 & RF3_IM_FIRE) p_ptr->resist_fire = TRUE;
	if(r_ptr->flags3 & RF3_IM_POIS) p_ptr->resist_pois = TRUE;
	if(r_ptr->flags3 & RF3_IM_COLD) p_ptr->resist_cold = TRUE;
	if(r_ptr->flags3 & RF3_RES_TELE) p_ptr->anti_tele = TRUE;
	if(r_ptr->flags3 & RF3_RES_NETH) p_ptr->resist_neth = TRUE;
//#define RF3_RES_WATE		0x00800000	/* Resist water */
	if(r_ptr->flags3 & RF3_RES_NEXU) p_ptr->resist_nexus = TRUE;
	if(r_ptr->flags3 & RF3_RES_DISE) p_ptr->resist_disen = TRUE;
	if(r_ptr->flags3 & RF3_NO_FEAR) p_ptr->resist_fear = TRUE;
	if(r_ptr->flags3 & RF3_NO_SLEEP) p_ptr->free_act = TRUE;
	if(r_ptr->flags3 & RF3_NO_CONF) p_ptr->resist_conf = TRUE;
	if(r_ptr->flags7 & RF7_CAN_FLY) p_ptr->fly = TRUE;
	if(r_ptr->flags7 & RF7_DISBELIEVE)
	{
		p_ptr->antimagic += r_ptr->level / 2 + 20;
		p_ptr->antimagic_dis += r_ptr->level / 15 + 3;
	}

	/* If not changed, spells didnt changed too, no need to send them */
	if (!p_ptr->body_changed) return;
	p_ptr->body_changed = FALSE;

#if 0	/* moved so that 2 handed weapons etc can be checked for */
	/* Take off what is no more usable */
	do_takeoff_impossible(Ind);
#endif

	/* Hack -- cancel wraithform upon form change  */
	if(!(r_ptr->flags2 & RF2_PASS_WALL) && p_ptr->tim_wraith)
		p_ptr->tim_wraith = 1;

	/* Update the innate spells */
	p_ptr->innate_spells[0] = r_ptr->flags4 & RF4_PLAYER_SPELLS;
	p_ptr->innate_spells[1] = r_ptr->flags5 & RF5_PLAYER_SPELLS;
	p_ptr->innate_spells[2] = r_ptr->flags6 & RF6_PLAYER_SPELLS;
	Send_spell_info(Ind, 0, 0, 0, "nothing");
}

#if 0	// moved to defines.h
bool monk_heavy_armor(int Ind)
{
#if 1 // DGDGDGDG -- no more monks for the time being
	player_type *p_ptr = Players[Ind];
	u16b monk_arm_wgt = 0;

//	if (!(p_ptr->pclass == CLASS_MONK)) return FALSE;
	if (!get_skill(p_ptr, SKILL_MARTIAL_ARTS)) return FALSE;

	/* Weight the armor */
	monk_arm_wgt = armour_weight(p_ptr);
#if 0
	monk_arm_wgt += p_ptr->inventory[INVEN_BODY].weight;
	monk_arm_wgt += p_ptr->inventory[INVEN_HEAD].weight;
	monk_arm_wgt += p_ptr->inventory[INVEN_ARM].weight;
	monk_arm_wgt += p_ptr->inventory[INVEN_OUTER].weight;
	monk_arm_wgt += p_ptr->inventory[INVEN_HANDS].weight;
	monk_arm_wgt += p_ptr->inventory[INVEN_FEET].weight;
#endif	// 0

//	return (monk_arm_wgt > ( 100 + (p_ptr->lev * 4))) ;
	return (monk_arm_wgt > 100 + get_skill_scale(p_ptr, SKILL_MARTIAL_ARTS, 100));
#endif
}
#endif	// 0

/* Are all the weapons wielded of the right type ? */
int get_weaponmastery_skill(player_type *p_ptr)
{
	int i, skill = 0;
	object_type *o_ptr = &p_ptr->inventory[INVEN_WIELD];

	i = 0;

        if (!o_ptr->k_idx)
        {
                return 0;
        }
        switch (o_ptr->tval)
        {
        case TV_SWORD:
                if ((!skill) || (skill == SKILL_SWORD)) skill = SKILL_SWORD;
                else skill = -1;
                break;
		case TV_AXE:
				if ((!skill) || (skill == SKILL_AXE)) skill = SKILL_AXE;
				else skill = -1;
				break;
        case TV_HAFTED:
                if ((!skill) || (skill == SKILL_HAFTED)) skill = SKILL_HAFTED;
                else skill = -1;
                break;
//        case SKILL_POLEARM:
        case TV_POLEARM:
                if ((!skill) || (skill == SKILL_POLEARM)) skill = SKILL_POLEARM;
                else skill = -1;
                break;
	}

	/* Everything is ok */
	return skill;
}

/* Are all the ranged weapons wielded of the right type ? */
int get_archery_skill(player_type *p_ptr)
{
	int i, skill = 0;
	object_type *o_ptr;

        o_ptr = &p_ptr->inventory[INVEN_BOW];

		if (!o_ptr->k_idx) return -1;

		/* Hack -- Boomerang skill */
		if (o_ptr->tval == TV_BOOMERANG) return SKILL_BOOMERANG;

        switch (o_ptr->sval / 10)
        {
        case 0:
                if ((!skill) || (skill == SKILL_SLING)) skill = SKILL_SLING;
                else skill = -1;
                break;
        case 1:
                if ((!skill) || (skill == SKILL_BOW)) skill = SKILL_BOW;
                else skill = -1;
                break;
        case 2:
                if ((!skill) || (skill == SKILL_XBOW)) skill = SKILL_XBOW;
                else skill = -1;
                break;
        }

	/* Everything is ok */
	return skill;
}


int calc_blows(int Ind, object_type *o_ptr)
{
	player_type *p_ptr = Players[Ind];
	int str_index, dex_index;

	int num = 0, wgt = 0, mul = 0, div = 0, num_blow = 0;

	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		/* Adevnturer */
		case CLASS_ADVENTURER: num = 5; wgt = 35; mul = 6; break;
		case CLASS_BARD: num = 5; wgt = 35; mul = 6; break;

							   /* Warrior */
		case CLASS_WARRIOR: num = 6; wgt = 30; mul = 5; break;

							/* Mage */
		case CLASS_MAGE:    num = 4; wgt = 40; mul = 2; break;

							/* Priest */
		case CLASS_PRIEST:  num = 5; wgt = 35; mul = 3; break;

							/* Rogue */
		case CLASS_ROGUE:   num = 5; wgt = 30; mul = 3; break;

							/* Mimic */
		case CLASS_MIMIC:   num = 4; wgt = 30; mul = 3; break;

							/* Archer */
		case CLASS_ARCHER:   num = 3; wgt = 30; mul = 3; break;
	}

	/* Enforce a minimum "weight" (tenth pounds) */
	div = ((o_ptr->weight < wgt) ? wgt : o_ptr->weight);

	/* Access the strength vs weight */
	str_index = (adj_str_blow[p_ptr->stat_ind[A_STR]] * mul / div);

	/* Maximal value */
	if (str_index > 11) str_index = 11;

	/* Index by dexterity */
	dex_index = (adj_dex_blow[p_ptr->stat_ind[A_DEX]]);

	/* Maximal value */
	if (dex_index > 11) dex_index = 11;

	/* Use the blows table */
	num_blow = blows_table[str_index][dex_index];

	/* Maximal value */
	if (num_blow > num) num_blow = num;

	/* Add in the "bonus blows" */
//	num_blow += p_ptr->extra_blows;

	/* Require at least one blow */
	if (num_blow < 1) num_blow = 1;

	/* Boost blows with masteries */
	if (get_weaponmastery_skill(p_ptr) != -1)
	{
		num_blow += get_skill_scale(p_ptr, get_weaponmastery_skill(p_ptr), 2);
	}

	return (num_blow);
}

/*
 * Calculate the players current "state", taking into account
 * not only race/class intrinsics, but also objects being worn
 * and temporary spell effects.
 *
 * See also calc_mana() and calc_hitpoints().
 *
 * Take note of the new "speed code", in particular, a very strong
 * player will start slowing down as soon as he reaches 150 pounds,
 * but not until he reaches 450 pounds will he be half as fast as
 * a normal kobold.  This both hurts and helps the player, hurts
 * because in the old days a player could just avoid 300 pounds,
 * and helps because now carrying 300 pounds is not very painful.
 *
 * The "weapon" and "bow" do *not* add to the bonuses to hit or to
 * damage, since that would affect non-combat things.  These values
 * are actually added in later, at the appropriate place.
 *
 * This function induces various "status" messages.
 */
static void calc_bonuses(int Ind)
{
	player_type *p_ptr = Players[Ind];

	int			i, j, hold, minus;

	int			old_speed;

	int			old_telepathy;
	int			old_see_inv;

	int			old_dis_ac;
	int			old_dis_to_a;

	int			old_dis_to_h;
	int			old_dis_to_d;

//	int			extra_blows;
	int			extra_shots;
	int			extra_spells;

	object_type		*o_ptr;
	object_kind		*k_ptr;

	    u32b f1, f2, f3, f4, f5, esp;
		s16b pval;


	/* Save the old speed */
	old_speed = p_ptr->pspeed;

	/* Save the old vision stuff */
	old_telepathy = p_ptr->telepathy;
	old_see_inv = p_ptr->see_inv;

	/* Save the old armor class */
	old_dis_ac = p_ptr->dis_ac;
	old_dis_to_a = p_ptr->dis_to_a;

	/* Save the old hit/damage bonuses */
	old_dis_to_h = p_ptr->dis_to_h;
	old_dis_to_d = p_ptr->dis_to_d;

	/* Clear extra blows/shots */
//	extra_blows = extra_shots = extra_spells = 0;
	extra_shots = extra_spells = 0;

	/* Clear the stat modifiers */
	for (i = 0; i < 6; i++) p_ptr->stat_add[i] = 0;


	/* Clear the Displayed/Real armor class */
	p_ptr->dis_ac = p_ptr->ac = 0;

	/* Clear the Displayed/Real Bonuses */
	p_ptr->dis_to_h = p_ptr->to_h = p_ptr->to_h_melee = p_ptr->to_h_ranged = 0;
	p_ptr->dis_to_d = p_ptr->to_d = p_ptr->to_d_melee = p_ptr->to_d_ranged = 0;
	p_ptr->dis_to_a = p_ptr->to_a = 0;


	/* Clear all the flags */
	p_ptr->aggravate = FALSE;
	p_ptr->teleport = FALSE;
	p_ptr->exp_drain = FALSE;
        p_ptr->drain_mana = 0;
        p_ptr->drain_life = 0;
	p_ptr->bless_blade = FALSE;
	p_ptr->xtra_might = 0;
	p_ptr->impact = FALSE;
	p_ptr->see_inv = FALSE;
	p_ptr->free_act = FALSE;
	p_ptr->slow_digest = FALSE;
	p_ptr->regenerate = FALSE;
	p_ptr->regen_mana = FALSE;
	p_ptr->feather_fall = FALSE;
	p_ptr->hold_life = FALSE;
	p_ptr->telepathy = 0;
	p_ptr->lite = FALSE;
			p_ptr->cur_lite = 0;
	p_ptr->sustain_str = FALSE;
	p_ptr->sustain_int = FALSE;
	p_ptr->sustain_wis = FALSE;
	p_ptr->sustain_con = FALSE;
	p_ptr->sustain_dex = FALSE;
	p_ptr->sustain_chr = FALSE;
	p_ptr->resist_acid = FALSE;
	p_ptr->resist_elec = FALSE;
	p_ptr->resist_fire = FALSE;
	p_ptr->resist_cold = FALSE;
	p_ptr->resist_pois = FALSE;
	p_ptr->resist_conf = FALSE;
	p_ptr->resist_sound = FALSE;
	p_ptr->resist_lite = FALSE;
	p_ptr->resist_dark = FALSE;
	p_ptr->resist_chaos = FALSE;
	p_ptr->resist_disen = FALSE;
	p_ptr->resist_shard = FALSE;
	p_ptr->resist_nexus = FALSE;
	p_ptr->resist_blind = FALSE;
	p_ptr->resist_neth = FALSE;
	p_ptr->resist_fear = FALSE;
	p_ptr->immune_acid = FALSE;
	p_ptr->immune_elec = FALSE;
	p_ptr->immune_fire = FALSE;
	p_ptr->immune_cold = FALSE;
	p_ptr->sh_fire = FALSE;
	p_ptr->sh_elec = FALSE;
	p_ptr->fly = FALSE;
        p_ptr->reduc_fire = 0;
        p_ptr->reduc_cold = 0;
        p_ptr->reduc_elec = 0;
        p_ptr->reduc_acid = 0;
	p_ptr->anti_magic = FALSE;
	p_ptr->auto_id = FALSE;
//		p_ptr->to_s = 0;
		p_ptr->to_m = 0;
		p_ptr->to_l = 0;
		p_ptr->black_breath_tmp = FALSE;
		p_ptr->stormbringer = FALSE;

	/* Invisibility */
	p_ptr->invis = 0;
	if (!p_ptr->tim_invisibility) p_ptr->tim_invis_power = 0;

	p_ptr->immune_neth = FALSE;
	p_ptr->anti_tele = FALSE;
	p_ptr->antimagic = 0;
	p_ptr->antimagic_dis = 0;
	p_ptr->xtra_crit = 0;
	p_ptr->dodge_chance = 0;

	p_ptr->sensible_fire = FALSE;
	p_ptr->resist_continuum = FALSE;

	/* Start with a single blow per turn */
	p_ptr->num_blow = 1;
	p_ptr->extra_blows = 0;

	/* Start with a single shot per turn */
	p_ptr->num_fire = 1;

	/* Start with a single spell per turn */
	p_ptr->num_spell = 1;

	/* Reset the "xtra" tval */
	p_ptr->tval_xtra = 0;

	/* Reset the "ammo" tval */
	p_ptr->tval_ammo = 0;

	/* Base infravision (purely racial) */
	p_ptr->see_infra = p_ptr->rp_ptr->infra;


	/* Base skill -- disarming */
	p_ptr->skill_dis = p_ptr->rp_ptr->r_dis + p_ptr->cp_ptr->c_dis;

	/* Base skill -- magic devices */
	p_ptr->skill_dev = p_ptr->rp_ptr->r_dev + p_ptr->cp_ptr->c_dev;

	/* Base skill -- saving throw */
	p_ptr->skill_sav = p_ptr->rp_ptr->r_sav + p_ptr->cp_ptr->c_sav;

	/* Base skill -- stealth */
	p_ptr->skill_stl = p_ptr->rp_ptr->r_stl + p_ptr->cp_ptr->c_stl;

	/* Base skill -- searching ability */
	p_ptr->skill_srh = p_ptr->rp_ptr->r_srh + p_ptr->cp_ptr->c_srh;

	/* Base skill -- searching frequency */
	p_ptr->skill_fos = p_ptr->rp_ptr->r_fos + p_ptr->cp_ptr->c_fos;

	/* Base skill -- combat (normal) */
	p_ptr->skill_thn = p_ptr->rp_ptr->r_thn + p_ptr->cp_ptr->c_thn;

	/* Base skill -- combat (shooting) */
	p_ptr->skill_thb = p_ptr->rp_ptr->r_thb + p_ptr->cp_ptr->c_thb;

	/* Base skill -- combat (throwing) */
	p_ptr->skill_tht = p_ptr->rp_ptr->r_thb + p_ptr->cp_ptr->c_thb;

	/* Base skill -- digging */
	p_ptr->skill_dig = 0;


	/* Calc bonus body */
	if(p_ptr->body_monster)
	{
		calc_body_bonus(Ind);
	}
	else	// if if or switch to switch, that is the problem :)
			/* I vote for p_info ;) */
	{
		/* Start with "normal" speed */
		p_ptr->pspeed = 110;

		/* Bats get +10 speed ... they need it!*/
		if (p_ptr->fruit_bat){
			p_ptr->pspeed += 10;
			p_ptr->fly=TRUE;
		}
		if (p_ptr->fruit_bat) p_ptr->feather_fall = TRUE;

		/* Elf */
		if (p_ptr->prace == RACE_ELF) p_ptr->resist_lite = TRUE;

		/* Hobbit */
		else if (p_ptr->prace == RACE_HOBBIT)
		{
			p_ptr->sustain_dex = TRUE;

			/* DEX bonus for NOT wearing shoes */
			if (!p_ptr->inventory[INVEN_FEET].k_idx)
				p_ptr->stat_add[A_DEX] += 2;
		}

		/* Gnome */
		else if (p_ptr->prace == RACE_GNOME) p_ptr->free_act = TRUE;

		/* Dwarf */
		else if (p_ptr->prace == RACE_DWARF) p_ptr->resist_blind = TRUE;

		/* Half-Orc */
		else if (p_ptr->prace == RACE_HALF_ORC) p_ptr->resist_dark = TRUE;

		/* Half-Troll */
		else if (p_ptr->prace == RACE_HALF_TROLL) p_ptr->sustain_str = TRUE;

		/* Dunadan */
		else if (p_ptr->prace == RACE_DUNADAN) p_ptr->sustain_con = TRUE;

		/* High Elf */
		else if (p_ptr->prace == RACE_HIGH_ELF)
		{
			p_ptr->resist_lite = TRUE;
			p_ptr->see_inv = TRUE;
		}

		/* Yeek */
		else if (p_ptr->prace == RACE_YEEK) p_ptr->feather_fall = TRUE;

		/* Goblin */
		else if (p_ptr->prace == RACE_GOBLIN)
		{
			p_ptr->resist_dark = TRUE;
			p_ptr->feather_fall = TRUE;
		}

		/* Ent */
		else if (p_ptr->prace == RACE_ENT)
		{
			p_ptr->slow_digest = TRUE;
			p_ptr->pspeed -= 2;
			p_ptr->sensible_fire = TRUE;

			if (p_ptr->lev >= 4) p_ptr->see_inv = TRUE;
//			if (p_ptr->lev >= 40) p_ptr->telepathy |= ESP_ALL;
			if (p_ptr->lev >= 20) p_ptr->telepathy |= ESP_ANIMAL;
			if (p_ptr->lev >= 25) p_ptr->telepathy |= ESP_TROLL;
			if (p_ptr->lev >= 30) p_ptr->telepathy |= ESP_GIANT;
			if (p_ptr->lev >= 40) p_ptr->telepathy |= ESP_EVIL;
			if (p_ptr->lev >= 50) p_ptr->telepathy = ESP_ALL;
		}

		/* DragonRider */
		else if (p_ptr->prace == RACE_DRIDER)
		{
			p_ptr->feather_fall = TRUE;

			if (p_ptr->lev >= 5) p_ptr->telepathy |= ESP_DRAGON;
			if (p_ptr->lev >= 10) p_ptr->resist_fire = TRUE;
			if (p_ptr->lev >= 15) p_ptr->resist_cold = TRUE;
			if (p_ptr->lev >= 20) p_ptr->resist_acid = TRUE;
			if (p_ptr->lev >= 25) p_ptr->resist_elec = TRUE;
			if (p_ptr->lev >= 30) p_ptr->fly = TRUE;
		}
	}

	/* Compute antimagic */
	if (get_skill(p_ptr, SKILL_ANTIMAGIC))
	{
//		p_ptr->anti_magic = TRUE;	/* it means 95% saving-throw!! */
		p_ptr->antimagic += get_skill(p_ptr, SKILL_ANTIMAGIC);
		p_ptr->antimagic_dis += 1 + (get_skill(p_ptr, SKILL_ANTIMAGIC) / 11);
	}

	/* Take off what is no more usable */
	do_takeoff_impossible(Ind);

	/* Ghost */
	if (p_ptr->ghost)
	{
		p_ptr->see_inv = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->hold_life = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->see_infra += 2;
		p_ptr->resist_pois = TRUE;
//		p_ptr->invis += 5; */ /* No. */
	}


	/* Hack -- apply racial/class stat maxes */
	if (p_ptr->maximize)
	{
		/* Apply the racial modifiers */
		for (i = 0; i < 6; i++)
		{
			/* Modify the stats for "race" */
			/* yeek mimic no longer rocks too much */
//			if (!p_ptr->body_monster) p_ptr->stat_add[i] += (p_ptr->rp_ptr->r_adj[i]);
			p_ptr->stat_add[i] += (p_ptr->rp_ptr->r_adj[i]) * 3 / (p_ptr->body_monster ? 4 : 3);
			p_ptr->stat_add[i] += (p_ptr->cp_ptr->c_adj[i]);
		}
	}

       	/* Apply the racial modifiers */
	if (p_ptr->mode & MODE_HELL)
	{
		for (i = 0; i < 6; i++)
		{
			/* Modify the stats for "race" */
			p_ptr->stat_add[i]--;
		}
	}
	

	/* Hack -- the dungeon master gets +50 speed. */
	if (p_ptr->admin_dm) 
	{
		p_ptr->pspeed += 50;
		p_ptr->telepathy |= ESP_ALL;
	}

	/* Hack -- recalculate inventory weight and count */
	p_ptr->total_weight = 0;
	p_ptr->inven_cnt = 0;

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &p_ptr->inventory[i];

		/* Skip missing items */
		if (!o_ptr->k_idx) break;

		p_ptr->inven_cnt++;
		p_ptr->total_weight += o_ptr->weight * o_ptr->number;
	}

	/* Scan the usable inventory */
	for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		o_ptr = &p_ptr->inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];
		pval = o_ptr->pval;

		/* Skip missing items */
		if (!o_ptr->k_idx) continue;

		p_ptr->total_weight += o_ptr->weight * o_ptr->number;

		/* Extract the item flags */
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

		/* Not-burning light source does nothing, good or bad */
		if ((f4 & TR4_FUEL_LITE) && (o_ptr->timeout < 1)) continue;

		/* MEGA ugly hack -- set spacetime distortion resistance */
		if(o_ptr->name1 == ART_ANCHOR)
		{
			p_ptr->resist_continuum = TRUE;
		}

		/* Hack -- first add any "base bonuses" of the item.  A new
		 * feature in MAngband 0.7.0 is that the magnitude of the
		 * base bonuses is stored in bpval instead of pval, making the
		 * magnitude of "base bonuses" and "ego bonuses" independent 
		 * from each other.
		 * An example of an item that uses this independency is an
		 * Orcish Shield of the Avari that gives +1 to STR and +3 to
		 * CON. (base bonus from the shield +1 STR,CON, ego bonus from
		 * the Avari +2 CON).  
		 * Of course, the proper fix would be to redesign the object
		 * type so that each of the ego bonuses has its own independent
		 * parameter.
		 */
		/* If we have any base bonuses to add, add them */
		if (k_ptr->flags1 & TR1_PVAL_MASK)
		{
			/* Affect stats */
			if (k_ptr->flags1 & TR1_STR) p_ptr->stat_add[A_STR] += o_ptr->bpval;
			if (k_ptr->flags1 & TR1_INT) p_ptr->stat_add[A_INT] += o_ptr->bpval;
			if (k_ptr->flags1 & TR1_WIS) p_ptr->stat_add[A_WIS] += o_ptr->bpval;
			if (k_ptr->flags1 & TR1_DEX) p_ptr->stat_add[A_DEX] += o_ptr->bpval;
			if (k_ptr->flags1 & TR1_CON) p_ptr->stat_add[A_CON] += o_ptr->bpval;
			if (k_ptr->flags1 & TR1_CHR) p_ptr->stat_add[A_CHR] += o_ptr->bpval;

			/* Affect stealth */
			if (k_ptr->flags1 & TR1_STEALTH) p_ptr->skill_stl += o_ptr->bpval;

			/* Affect searching ability (factor of five) */
			if (k_ptr->flags1 & TR1_SEARCH) p_ptr->skill_srh += (o_ptr->bpval * 5);

			/* Affect searching frequency (factor of five) */
			if (k_ptr->flags1 & TR1_SEARCH) p_ptr->skill_fos += (o_ptr->bpval * 5);

			/* Affect infravision */
			if (k_ptr->flags1 & TR1_INFRA) p_ptr->see_infra += o_ptr->bpval;

			/* Affect digging (factor of 20) */
			if (k_ptr->flags1 & TR1_TUNNEL) p_ptr->skill_dig += (o_ptr->bpval * 20);

			/* Affect speed */
			if (k_ptr->flags1 & TR1_SPEED) p_ptr->pspeed += o_ptr->bpval;

			/* Affect blows */
			if (k_ptr->flags1 & TR1_BLOWS) p_ptr->extra_blows += o_ptr->bpval;

			/* Affect spells */
			if (k_ptr->flags1 & TR1_SPELL) extra_spells += o_ptr->bpval;
//			if (k_ptr->flags1 & TR1_SPELL_SPEED) extra_spells += o_ptr->bpval;

                /* Affect mana capacity */
                if (f1 & (TR1_MANA)) p_ptr->to_m += o_ptr->bpval;

                /* Affect life capacity */
                if (f1 & (TR1_LIFE)) p_ptr->to_l += o_ptr->bpval;

		}

		if (k_ptr->flags5 & TR5_PVAL_MASK)
		{
			if (f5 & (TR5_CRIT)) p_ptr->xtra_crit += o_ptr->bpval;
			if (f5 & (TR5_DISARM)) p_ptr->skill_dis += (o_ptr->bpval) * 10;
//			if (f5 & (TR5_LUCK)) p_ptr->luck_cur += o_ptr->bpval;
		}

		/* Next, add our ego bonuses */
		/* Hack -- clear out any pval bonuses that are in the base item
		 * bonus but not the ego bonus so we don't add them twice.
		*/
#if 1
//		if (o_ptr->name2 && o_ptr->tval!=TV_RING) // pls see apply_magic ;)
		if (o_ptr->name2)
		{
			artifact_type *a_ptr;
	 	
			a_ptr =	ego_make(o_ptr);
			f1 &= ~(k_ptr->flags1 & TR1_PVAL_MASK & ~a_ptr->flags1);
			f5 &= ~(k_ptr->flags5 & TR5_PVAL_MASK & ~a_ptr->flags5);

			/* Hack: Stormbringer! */
			if (o_ptr->name2 == EGO_STORMBRINGER)
			{
				p_ptr->stormbringer = TRUE;
				if (cfg.use_pk_rules == PK_RULES_DECLARE)
				{
					p_ptr->pkill|=PKILL_KILLABLE;
					if (!(p_ptr->pkill & PKILL_KILLER) &&
							!(p_ptr->pkill & PKILL_SET))
						set_pkill(Ind, 50);
				}
			}
		}

		if (o_ptr->name1 == ART_RANDART)
		{
			artifact_type *a_ptr;
	 	
			a_ptr =	randart_make(o_ptr);
			f1 &= ~(k_ptr->flags1 & TR1_PVAL_MASK & ~a_ptr->flags1);
			f5 &= ~(k_ptr->flags5 & TR5_PVAL_MASK & ~a_ptr->flags5);
		}
#endif


		/* Affect stats */
		if (f1 & TR1_STR) p_ptr->stat_add[A_STR] += pval;
		if (f1 & TR1_INT) p_ptr->stat_add[A_INT] += pval;
		if (f1 & TR1_WIS) p_ptr->stat_add[A_WIS] += pval;
		if (f1 & TR1_DEX) p_ptr->stat_add[A_DEX] += pval;
		if (f1 & TR1_CON) p_ptr->stat_add[A_CON] += pval;
		if (f1 & TR1_CHR) p_ptr->stat_add[A_CHR] += pval;


//                if (f5 & (TR5_LUCK)) p_ptr->luck_cur += pval;

                /* Affect spell power */
//                if (f1 & (TR1_SPELL)) p_ptr->to_s += pval;

                /* Affect mana capacity */
                if (f1 & (TR1_MANA)) p_ptr->to_m += pval;

                /* Affect life capacity */
                if (f1 & (TR1_LIFE)) p_ptr->to_l += pval;

		/* Affect stealth */
		if (f1 & TR1_STEALTH) p_ptr->skill_stl += pval;

		/* Affect searching ability (factor of five) */
		if (f1 & TR1_SEARCH) p_ptr->skill_srh += (pval * 5);

		/* Affect searching frequency (factor of five) */
		if (f1 & TR1_SEARCH) p_ptr->skill_fos += (pval * 5);

		/* Affect infravision */
		if (f1 & TR1_INFRA) p_ptr->see_infra += pval;

		/* Affect digging (factor of 20) */
		if (f1 & TR1_TUNNEL) p_ptr->skill_dig += (pval * 20);

		/* Affect speed */
		if (f1 & TR1_SPEED) p_ptr->pspeed += pval;

		/* Affect blows */
		if (f1 & TR1_BLOWS) p_ptr->extra_blows += pval;
                if (f5 & (TR5_CRIT)) p_ptr->xtra_crit += pval;

		/* Affect spellss */
//		if (f1 & TR1_SPELL_SPEED) extra_spells += pval;
		if (f1 & TR1_SPELL) extra_spells += pval;

		/* Affect disarming (factor of 20) */
		if (f5 & (TR5_DISARM)) p_ptr->skill_dis += pval * 10;

		/* Hack -- Sensible fire */
		if (f5 & (TR5_SENS_FIRE)) p_ptr->sensible_fire = TRUE;

		/* Hack -- cause earthquakes */
		if (f1 & TR1_IMPACT) p_ptr->impact = TRUE;

		/* Boost shots */
//		if (f3 & TR3_KNOWLEDGE) p_ptr->auto_id = TRUE;
		if (f4 & TR4_AUTO_ID) p_ptr->auto_id = TRUE;

		/* Boost shots */
		if (f3 & TR3_XTRA_SHOTS) extra_shots++;

		/* Various flags */
		if (f3 & TR3_AGGRAVATE) p_ptr->aggravate = TRUE;
		if (f3 & TR3_TELEPORT) p_ptr->teleport = TRUE;
		if (f3 & TR3_DRAIN_EXP) p_ptr->exp_drain = TRUE;
                if (f5 & (TR5_DRAIN_MANA)) p_ptr->drain_mana++;
                if (f5 & (TR5_DRAIN_HP)) p_ptr->drain_life++;
		if (f5 & (TR5_INVIS)){
//			p_ptr->tim_invisibility = 100;
			p_ptr->tim_invis_power = p_ptr->lev * 4 / 5;
//			p_ptr->invis = p_ptr->tim_invis_power;
		}
		if (f3 & TR3_BLESSED) p_ptr->bless_blade = TRUE;
		if (f3 & TR3_XTRA_MIGHT) p_ptr->xtra_might++;
		if (f3 & TR3_SLOW_DIGEST) p_ptr->slow_digest = TRUE;
		if (f3 & TR3_REGEN) p_ptr->regenerate = TRUE;
		if (f5 & TR5_REGEN_MANA) p_ptr->regen_mana = TRUE;
                if (esp) p_ptr->telepathy |= esp;
//		if (f3 & TR3_TELEPATHY) p_ptr->telepathy = TRUE;
//		if (f3 & TR3_LITE1) p_ptr->lite += 1;
		if (f3 & TR3_SEE_INVIS) p_ptr->see_inv = TRUE;
		if (f3 & TR3_FEATHER) p_ptr->feather_fall = TRUE;
		if (f2 & TR2_FREE_ACT) p_ptr->free_act = TRUE;
		if (f2 & TR2_HOLD_LIFE) p_ptr->hold_life = TRUE;

		/* Light(consider doing it on calc_torch) */
//		if (((f4 & TR4_FUEL_LITE) && (o_ptr->timeout > 0)) || (!(f4 & TR4_FUEL_LITE)))
		{
			j = 0;
			if (f3 & TR3_LITE1) j++;
			if (f4 & TR4_LITE2) j += 2;
			if (f4 & TR4_LITE3) j += 3;

			p_ptr->cur_lite += j;
			if (j && !(f4 & TR4_FUEL_LITE)) p_ptr->lite = TRUE;
		}

		/* Immunity flags */
		if (f2 & TR2_IM_FIRE) p_ptr->immune_fire = TRUE;
		if (f2 & TR2_IM_ACID) p_ptr->immune_acid = TRUE;
		if (f2 & TR2_IM_COLD) p_ptr->immune_cold = TRUE;
		if (f2 & TR2_IM_ELEC) p_ptr->immune_elec = TRUE;

                if (f2 & TR2_REDUC_FIRE) p_ptr->reduc_fire += 5 * o_ptr->to_a;
                if (f2 & TR2_REDUC_COLD) p_ptr->reduc_cold += 5 * o_ptr->to_a;
                if (f2 & TR2_REDUC_ELEC) p_ptr->reduc_elec += 5 * o_ptr->to_a;
                if (f2 & TR2_REDUC_ACID) p_ptr->reduc_acid += 5 * o_ptr->to_a;

		/* Resistance flags */
		if (f2 & TR2_RES_ACID) p_ptr->resist_acid = TRUE;
		if (f2 & TR2_RES_ELEC) p_ptr->resist_elec = TRUE;
		if (f2 & TR2_RES_FIRE) p_ptr->resist_fire = TRUE;
		if (f2 & TR2_RES_COLD) p_ptr->resist_cold = TRUE;
		if (f2 & TR2_RES_POIS) p_ptr->resist_pois = TRUE;
		if (f2 & TR2_RES_FEAR) p_ptr->resist_fear = TRUE;
		if (f2 & TR2_RES_CONF) p_ptr->resist_conf = TRUE;
		if (f2 & TR2_RES_SOUND) p_ptr->resist_sound = TRUE;
		if (f2 & TR2_RES_LITE) p_ptr->resist_lite = TRUE;
		if (f2 & TR2_RES_DARK) p_ptr->resist_dark = TRUE;
		if (f2 & TR2_RES_CHAOS) p_ptr->resist_chaos = TRUE;
		if (f2 & TR2_RES_DISEN) p_ptr->resist_disen = TRUE;
		if (f2 & TR2_RES_SHARDS) p_ptr->resist_shard = TRUE;
		if (f2 & TR2_RES_NEXUS) p_ptr->resist_nexus = TRUE;
		if (f2 & TR2_RES_BLIND) p_ptr->resist_blind = TRUE;
		if (f2 & TR2_RES_NETHER) p_ptr->resist_neth = TRUE;
//		if (f2 & TR2_ANTI_MAGIC) p_ptr->anti_magic = TRUE;

		/* Sustain flags */
		if (f2 & TR2_SUST_STR) p_ptr->sustain_str = TRUE;
		if (f2 & TR2_SUST_INT) p_ptr->sustain_int = TRUE;
		if (f2 & TR2_SUST_WIS) p_ptr->sustain_wis = TRUE;
		if (f2 & TR2_SUST_DEX) p_ptr->sustain_dex = TRUE;
		if (f2 & TR2_SUST_CON) p_ptr->sustain_con = TRUE;
		if (f2 & TR2_SUST_CHR) p_ptr->sustain_chr = TRUE;

		/* PernA flags */
//                if (f3 & (TR3_WRAITH)) p_ptr->wraith_form = TRUE;
                if (f4 & (TR4_FLY)) p_ptr->fly = TRUE;
                if (f4 & (TR4_CLIMB)) p_ptr->climb = TRUE;
                if (f4 & (TR4_IM_NETHER)) p_ptr->immune_neth = TRUE;
		if (f5 & (TR5_REFLECT)) p_ptr->reflect = TRUE;
		if (f3 & (TR3_SH_FIRE)) p_ptr->sh_fire = TRUE;
		if (f3 & (TR3_SH_ELEC)) p_ptr->sh_elec = TRUE;
		if (f3 & (TR3_NO_MAGIC)) p_ptr->anti_magic = TRUE;
		if (f3 & (TR3_NO_TELE)) p_ptr->anti_tele = TRUE;

		/* Additional flags from PernAngband */

		if (f4 & (TR4_IM_NETHER)) p_ptr->immune_neth = TRUE;

		/* Limit use of disenchanted DarkSword for non-unbe */
		minus = o_ptr->to_h + o_ptr->to_d + pval + o_ptr->to_a;
		if (minus < 0) minus = 0;

		if (f4 & (TR4_ANTIMAGIC_50) && minus < 50)
		{
			p_ptr->antimagic += 50 - minus;
			p_ptr->antimagic_dis += 5 - (minus / 15);
		}
		if (f4 & (TR4_ANTIMAGIC_30) && minus < 30)
		{
			p_ptr->antimagic += 30 - minus;
			p_ptr->antimagic_dis += 3 - (minus / 15);
		}
		if (f4 & (TR4_ANTIMAGIC_20) && minus < 20)
		{
			p_ptr->antimagic += 20 - minus;
			p_ptr->antimagic_dis += 2 - (minus / 15);
		}
		if (f4 & (TR4_ANTIMAGIC_10) && minus < 10)
		{
			p_ptr->antimagic += 10 - minus;
			p_ptr->antimagic_dis += 1 - (minus / 15);
		}

		if (f4 & (TR4_BLACK_BREATH)) p_ptr->black_breath_tmp = TRUE;

//		if (f5 & (TR5_IMMOVABLE)) p_ptr->immovable = TRUE;



		/* Modify the base armor class */
		p_ptr->ac += o_ptr->ac;

		/* The base armor class is always known */
		p_ptr->dis_ac += o_ptr->ac;

		/* Apply the bonuses to armor class */
		p_ptr->to_a += o_ptr->to_a;

		/* Apply the mental bonuses to armor class, if known */
		if (object_known_p(Ind, o_ptr)) p_ptr->dis_to_a += o_ptr->to_a;

		/* Hack -- do not apply "weapon" bonuses */
		if (i == INVEN_WIELD) continue;

		/* Hack -- do not apply "bow" bonuses */
		if (i == INVEN_BOW) continue;

		if (i == INVEN_AMMO || i == INVEN_TOOL) continue;

		/* Apply the bonuses to hit/damage */
		p_ptr->to_h += o_ptr->to_h;
		p_ptr->to_d += o_ptr->to_d;

		/* Apply the mental bonuses tp hit/damage, if known */
		if (object_known_p(Ind, o_ptr)) p_ptr->dis_to_h += o_ptr->to_h;
		if (object_known_p(Ind, o_ptr)) p_ptr->dis_to_d += o_ptr->to_d;

	}


	/* Calculate stats */
	for (i = 0; i < 6; i++)
	{
		int top, use, ind;


		/* Extract the new "stat_use" value for the stat */
		top = modify_stat_value(p_ptr->stat_max[i], p_ptr->stat_add[i]);

		/* Notice changes */
		if (p_ptr->stat_top[i] != top)
		{
			/* Save the new value */
			p_ptr->stat_top[i] = top;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}


		/* Extract the new "stat_use" value for the stat */
		use = modify_stat_value(p_ptr->stat_cur[i], p_ptr->stat_add[i]);

		/* Notice changes */
		if (p_ptr->stat_use[i] != use)
		{
			/* Save the new value */
			p_ptr->stat_use[i] = use;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}


		/* Values: 3, 4, ..., 17 */
		if (use <= 18) ind = (use - 3);

		/* Ranges: 18/00-18/09, ..., 18/210-18/219 */
		else if (use <= 18+219) ind = (15 + (use - 18) / 10);

		/* Range: 18/220+ */
		else ind = (37);

		/* Notice changes */
		if (p_ptr->stat_ind[i] != ind)
		{
			/* Save the new index */
			p_ptr->stat_ind[i] = ind;

			/* Change in CON affects Hitpoints */
			if (i == A_CON)
			{
				p_ptr->update |= (PU_HP);
			}

			/* Change in INT may affect Mana/Spells */
			else if (i == A_INT)
			{
                                p_ptr->update |= (PU_MANA | PU_SPELLS);
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == A_WIS)
			{
                                p_ptr->update |= (PU_MANA | PU_SPELLS);
			}

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}
	}

#if 0 // DGDGDGDG -- skill powa !
	switch(p_ptr->pclass)
	{
		case CLASS_MONK:
			/* Unencumbered Monks become faster every 10 levels */
			if (!(monk_heavy_armor(Ind)))
				p_ptr->pspeed += (p_ptr->lev) / 10;

			/* Free action if unencumbered at level 25 */
			if  ((p_ptr->lev > 24) && !(monk_heavy_armor(Ind)))
				p_ptr->free_act = TRUE;
			break;

			/* -APD- Hack -- rogues +1 speed at 5,20,35,50.
			 * this may be out of place, but.... */
	
		case CLASS_ROGUE:
			p_ptr->pspeed += p_ptr->lev / 6;
			break;
	}
#else	// 0
	if (get_skill(p_ptr, SKILL_MARTIAL_ARTS) && !monk_heavy_armor(p_ptr))
	{
		int k = get_skill_scale(p_ptr, SKILL_MARTIAL_ARTS, 8);

		if (k)
		{
			/* Extract the current weight (in tenth pounds) */
			j = p_ptr->total_weight;

			/* Extract the "weight limit" (in tenth pounds) */
			i = weight_limit(Ind);

			/* XXX XXX XXX Apply "encumbrance" from weight */
			if (j > i/5) k -= ((j - (i/5)) / (i / 10));

			if (k > 0)
			{
				p_ptr->pspeed += k;

				/* Free action if unencumbered at level 25 */
				if  (get_skill(p_ptr, SKILL_MARTIAL_ARTS) > 24)
					p_ptr->free_act = TRUE;

				/* give a stealth bonus */
				p_ptr->skill_stl += k;
			}
		}

		/* Monks get extra ac for armour _not worn_ */
		if (!p_ptr->inventory[INVEN_BOW].k_idx &&
			!p_ptr->inventory[INVEN_WIELD].k_idx)
		{
			int marts = get_skill_scale(p_ptr, SKILL_MARTIAL_ARTS, 60);
			if (!(p_ptr->inventory[INVEN_BODY].k_idx))
			{
				p_ptr->to_a += (marts * 3) / 2 * MARTIAL_ARTS_AC_ADJUST / 100;
				p_ptr->dis_to_a += (marts * 3) / 2 * MARTIAL_ARTS_AC_ADJUST / 100;
			}
			if (!(p_ptr->inventory[INVEN_OUTER].k_idx) && (marts > 15))
			{
				p_ptr->to_a += ((marts - 13) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
				p_ptr->dis_to_a += ((marts - 13) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
			}
			if (!(p_ptr->inventory[INVEN_ARM].k_idx) && (marts > 10))
			{
				p_ptr->to_a += ((marts - 8) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
				p_ptr->dis_to_a += ((marts - 8) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
			}
			if (!(p_ptr->inventory[INVEN_HEAD].k_idx)&& (marts > 4))
			{
				p_ptr->to_a += (marts - 2) / 3 * MARTIAL_ARTS_AC_ADJUST / 100;
				p_ptr->dis_to_a += (marts -2) / 3 * MARTIAL_ARTS_AC_ADJUST / 100;
			}
			if (!(p_ptr->inventory[INVEN_HANDS].k_idx))
			{
				p_ptr->to_a += (marts / 2) * MARTIAL_ARTS_AC_ADJUST / 100;
				p_ptr->dis_to_a += (marts / 2) * MARTIAL_ARTS_AC_ADJUST / 100;
			}
			if (!(p_ptr->inventory[INVEN_FEET].k_idx))
			{
				p_ptr->to_a += (marts / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
				p_ptr->dis_to_a += (marts / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
			}
		}
	}

	p_ptr->pspeed += get_skill_scale(p_ptr, SKILL_AGILITY, 10);
	p_ptr->pspeed += get_skill_scale(p_ptr, SKILL_SNEAKINESS, 5);
#endif	// 0

#if 0 // DGDGDGDGDG - no monks ffor the time being
	/* Monks get extra ac for armour _not worn_ */
//	if ((p_ptr->pclass == CLASS_MONK) && !(monk_heavy_armor(Ind)))
	if (get_skill(p_ptr, SKILL_MARTIAL_ARTS) && !(monk_heavy_armor(p_ptr)) &&
		!(p_ptr->inventory[INVEN_BOW].k_idx))
	{
		int marts = get_skill_scale(p_ptr, SKILL_MARTIAL_ARTS, 60);
		if (!(p_ptr->inventory[INVEN_BODY].k_idx))
		{
			p_ptr->to_a += (marts * 3) / 2 * MARTIAL_ARTS_AC_ADJUST / 100;
			p_ptr->dis_to_a += (marts * 3) / 2 * MARTIAL_ARTS_AC_ADJUST / 100;
		}
		if (!(p_ptr->inventory[INVEN_OUTER].k_idx) && (marts > 15))
		{
			p_ptr->to_a += ((marts - 13) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
			p_ptr->dis_to_a += ((marts - 13) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
		}
		if (!(p_ptr->inventory[INVEN_ARM].k_idx) && (marts > 10))
		{
			p_ptr->to_a += ((marts - 8) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
			p_ptr->dis_to_a += ((marts - 8) / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
		}
		if (!(p_ptr->inventory[INVEN_HEAD].k_idx)&& (marts > 4))
		{
			p_ptr->to_a += (marts - 2) / 3 * MARTIAL_ARTS_AC_ADJUST / 100;
			p_ptr->dis_to_a += (marts -2) / 3 * MARTIAL_ARTS_AC_ADJUST / 100;
		}
		if (!(p_ptr->inventory[INVEN_HANDS].k_idx))
		{
			p_ptr->to_a += (marts / 2) * MARTIAL_ARTS_AC_ADJUST / 100;
			p_ptr->dis_to_a += (marts / 2) * MARTIAL_ARTS_AC_ADJUST / 100;
		}
		if (!(p_ptr->inventory[INVEN_FEET].k_idx))
		{
			p_ptr->to_a += (marts / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
			p_ptr->dis_to_a += (marts / 3) * MARTIAL_ARTS_AC_ADJUST / 100;
		}
	}
#endif

	/* Apply temporary "stun" */
	if (p_ptr->stun > 50)
	{
		p_ptr->to_h -= 20;
		p_ptr->dis_to_h -= 20;
		p_ptr->to_d -= 20;
		p_ptr->dis_to_d -= 20;
	}
	else if (p_ptr->stun)
	{
		p_ptr->to_h -= 5;
		p_ptr->dis_to_h -= 5;
		p_ptr->to_d -= 5;
		p_ptr->dis_to_d -= 5;
	}


        /* Adrenaline effects */
        if (p_ptr->adrenaline)
	{
	        int i;

		i = p_ptr->lev / 7;
		if (i > 5) i = 5;
		p_ptr->stat_add[A_CON] += i;
		p_ptr->stat_add[A_STR] += i;
		p_ptr->stat_add[A_DEX] += (i + 1) / 2;
		p_ptr->to_h += 12;
		p_ptr->dis_to_h += 12;
		if (p_ptr->adrenaline & 1)
		{
			p_ptr->to_d += 8;
			p_ptr->dis_to_d += 8;
		}
		if (p_ptr->adrenaline & 2) p_ptr->extra_blows++;
		p_ptr->to_a -= 20;
		p_ptr->dis_to_a -= 10;
	}

	/* Invulnerability */
	if (p_ptr->invuln)
	{
		p_ptr->to_a += 100;
		p_ptr->dis_to_a += 100;
	}

	/* Temp ESP */
	if (p_ptr->tim_esp)
	{
//		p_ptr->telepathy = TRUE;
                p_ptr->telepathy |= ESP_ALL;
	}

	/* Temporary blessing */
	if (p_ptr->blessed)
	{
		p_ptr->to_a += p_ptr->blessed_power;
		p_ptr->dis_to_a += p_ptr->blessed_power;
		p_ptr->to_h += p_ptr->blessed_power;
		p_ptr->dis_to_h += p_ptr->blessed_power;
	}

	/* Temprory invisibility */
//	if (p_ptr->tim_invisibility)
	{
		p_ptr->invis = p_ptr->tim_invis_power;
	}

	/* Temprory shield */
	if (p_ptr->shield)
	{
		p_ptr->to_a += 50;
		p_ptr->dis_to_a += 50;
	}

	/* Temporary "Hero" */
	if (p_ptr->hero)
	{
		p_ptr->to_h += 12;
		p_ptr->dis_to_h += 12;
	}

	/* Temporary "Berserk" */
	if (p_ptr->shero)
	{
		p_ptr->to_h += 24;
		p_ptr->dis_to_h += 24;
		p_ptr->to_a -= 10;
		p_ptr->dis_to_a -= 10;
	}

	/* Temporary "Fury" */
	if (p_ptr->fury)
	{
		p_ptr->to_h += 10;
		p_ptr->dis_to_h += 10;
                p_ptr->to_d += 10;
                p_ptr->dis_to_d += 10;
		p_ptr->pspeed += 10;
                p_ptr->to_a -= 50;
                p_ptr->dis_to_a -= 50;
	}

	/* Temporary "fast" */
	if (p_ptr->fast)
	{
		p_ptr->pspeed += 10;
	}

	/* Temporary "slow" */
	if (p_ptr->slow)
	{
		p_ptr->pspeed -= 10;
	}

	/* Temporary see invisible */
	if (p_ptr->tim_invis)
	{
		p_ptr->see_inv = TRUE;
	}

	/* Temporary infravision boost */
	if (p_ptr->tim_infra)
	{
//		p_ptr->see_infra++;
		p_ptr->see_infra += 5;
	}

	/* Temporary st-anchor */
	if (p_ptr->st_anchor)
	{
		p_ptr->resist_continuum = TRUE;
	}

	/* Hack -- Res Chaos -> Res Conf */
	if (p_ptr->resist_chaos)
	{
		p_ptr->resist_conf = TRUE;
	}

	/* Hack -- Hero/Shero -> Res fear */
	if (p_ptr->hero || p_ptr->shero)
	{
		p_ptr->resist_fear = TRUE;
	}


	/* Hack -- Telepathy Change */
	if (p_ptr->telepathy != old_telepathy)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Hack -- See Invis Change */
	if (p_ptr->see_inv != old_see_inv)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Temporary space-time anchor */
	if (p_ptr->st_anchor)
	{
		p_ptr->anti_tele = TRUE;
	}


	/* Extract the current weight (in tenth pounds) */
	j = p_ptr->total_weight;

	/* Extract the "weight limit" (in tenth pounds) */
	i = weight_limit(Ind);

	/* XXX XXX XXX Apply "encumbrance" from weight */
	if (j > i/2) p_ptr->pspeed -= ((j - (i/2)) / (i / 10));

	/* Bloating slows the player down (a little) */
	if (p_ptr->food >= PY_FOOD_MAX) p_ptr->pspeed -= 10;

	/* Searching slows the player down */
	/* -APD- adding "stealth mode" for rogues... will probably need to tweek this */
	if (p_ptr->searching) 
	{
		int stealth = get_skill(p_ptr, SKILL_STEALTH);
		p_ptr->pspeed -= 10;

		if (stealth >= 10)
		{
//			p_ptr->skill_stl *= 3;
			p_ptr->skill_stl *=
				(stealth >= 40) ? 4 : ((stealth > 25) ? 3 : 2);
		}
	}

	if (p_ptr->mode & MODE_HELL && p_ptr->pspeed > 110)
	{
		int speed = p_ptr->pspeed - 110;

		speed /= 2;

		p_ptr->pspeed = speed + 110;
	}

	/* Display the speed (if needed) */
	if (p_ptr->pspeed != old_speed) p_ptr->redraw |= (PR_SPEED);


	/* Actual Modifier Bonuses (Un-inflate stat bonuses) */
	p_ptr->to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);

	/* Displayed Modifier Bonuses (Un-inflate stat bonuses) */
	p_ptr->dis_to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);

	if (p_ptr->mode & MODE_HELL)
	{
		if (p_ptr->dis_to_a > 0) p_ptr->dis_to_a /= 2;
		if (p_ptr->to_a > 0) p_ptr->to_a /= 2;
	}


	/* Redraw armor (if needed) */
	if ((p_ptr->dis_ac != old_dis_ac) || (p_ptr->dis_to_a != old_dis_to_a))
	{
		/* Redraw */
		p_ptr->redraw |= (PR_ARMOR);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}


	/* Obtain the "hold" value */
	hold = adj_str_hold[p_ptr->stat_ind[A_STR]];


	/* Examine the "current bow" */
	o_ptr = &p_ptr->inventory[INVEN_BOW];


	/* Assume not heavy */
	p_ptr->heavy_shoot = FALSE;

	/* It is hard to carholdry a heavy bow */
	if (hold < o_ptr->weight / 10)
	{
		/* Hard to wield a heavy bow */
		p_ptr->to_h += 2 * (hold - o_ptr->weight / 10);
		p_ptr->dis_to_h += 2 * (hold - o_ptr->weight / 10);

		/* Heavy Bow */
		p_ptr->heavy_shoot = TRUE;
	}


	/* Compute "extra shots" if needed */
	if (o_ptr->k_idx && !p_ptr->heavy_shoot)
	{
		int archery = get_archery_skill(p_ptr);

		p_ptr->tval_ammo = 0;

		/* Take note of required "tval" for missiles */
		switch (o_ptr->sval)
		{
			case SV_SLING:
			{
				p_ptr->tval_ammo = TV_SHOT;
				break;
			}

			case SV_SHORT_BOW:
			case SV_LONG_BOW:
			{
				p_ptr->tval_ammo = TV_ARROW;
				break;
			}

			case SV_LIGHT_XBOW:
			case SV_HEAVY_XBOW:
			{
				p_ptr->tval_ammo = TV_BOLT;
				break;
			}
		}

		if (archery != -1)
		{
			p_ptr->to_h_ranged += get_skill_scale(p_ptr, archery, 25);
			/* Isn't 4 shots/turn too small? */
			p_ptr->num_fire += (get_skill(p_ptr, archery) / 16)
				+ get_skill_scale(p_ptr, SKILL_ARCHERY, 1);
			p_ptr->xtra_might += (get_skill(p_ptr, archery) / 25);
#if 0	// not so meaningful (25,30,50)
			switch (archery)
			{
				case SKILL_SLING:
					if (p_ptr->tval_ammo == TV_SHOT) p_ptr->xtra_might += get_skill(p_ptr, archery) / 30;
					break;
				case SKILL_BOW:
					if (p_ptr->tval_ammo == TV_ARROW) p_ptr->xtra_might += get_skill(p_ptr, archery) / 30;
					break;
				case SKILL_XBOW:
					if (p_ptr->tval_ammo == TV_BOLT) p_ptr->xtra_might += get_skill(p_ptr, archery) / 30;
					break;
				case SKILL_BOOMERANG:
					if (!p_ptr->tval_ammo) p_ptr->xtra_might += get_skill(p_ptr, archery) / 30;
					break;
			}
#endif	// 0
		}

		/* Add in the "bonus shots" */
		p_ptr->num_fire += extra_shots;

		/* Require at least one shot */
		if (p_ptr->num_fire < 1) p_ptr->num_fire = 1;
	}

	/* Add in the "bonus spells" */
	p_ptr->num_spell += extra_spells;


	/* Examine the "tool" */
	o_ptr = &p_ptr->inventory[INVEN_TOOL];

	/* Boost digging skill by tool weight */
	if(o_ptr->k_idx && o_ptr->tval == TV_DIGGING)
	{
		p_ptr->skill_dig += (o_ptr->weight / 10);

		/* Hack -- to_h/to_d added to digging (otherwise meanless) */
		p_ptr->skill_dig += o_ptr->to_h;
		p_ptr->skill_dig += o_ptr->to_d;

		p_ptr->skill_dig += p_ptr->skill_dig *
			get_skill_scale(p_ptr, SKILL_DIG, 300) / 100;
	}


	/* Examine the "main weapon" */
	o_ptr = &p_ptr->inventory[INVEN_WIELD];

	/* Assume not heavy */
	p_ptr->heavy_wield = FALSE;

	/* It is hard to hold a heavy weapon */
	if (hold < o_ptr->weight / 10)
	{
		/* Hard to wield a heavy weapon */
		p_ptr->to_h += 2 * (hold - o_ptr->weight / 10);
		p_ptr->dis_to_h += 2 * (hold - o_ptr->weight / 10);

		/* Heavy weapon */
		p_ptr->heavy_wield = TRUE;
	}


	/* Normal weapons */
	if (o_ptr->k_idx && !p_ptr->heavy_wield)
	{
		p_ptr->num_blow = calc_blows(Ind, o_ptr);
		p_ptr->num_blow += p_ptr->extra_blows;
		/* Boost blows with masteries */
		if (get_weaponmastery_skill(p_ptr) != -1)
		{
			int lev = get_skill(p_ptr, get_weaponmastery_skill(p_ptr));

			p_ptr->to_h_melee += lev;
			p_ptr->to_d_melee += lev / 2;
//			p_ptr->num_blow += get_skill_scale(p_ptr, get_weaponmastery_skill(p_ptr), 2);
		}
	}

	/* Different calculation for monks with empty hands */
#if 1 // DGDGDGDG -- no more monks for the time being
//	if (p_ptr->pclass == CLASS_MONK)
	if (get_skill(p_ptr, SKILL_MARTIAL_ARTS) && !o_ptr->k_idx &&
		!(p_ptr->inventory[INVEN_BOW].k_idx))
	{
		int marts = get_skill_scale(p_ptr, SKILL_MARTIAL_ARTS, 50);
		p_ptr->num_blow = 0;

		if (marts >  9) p_ptr->num_blow++;
		if (marts > 19) p_ptr->num_blow++;
		if (marts > 29) p_ptr->num_blow++;
		if (marts > 34) p_ptr->num_blow++;
		if (marts > 39) p_ptr->num_blow++;
		if (marts > 44) p_ptr->num_blow++;
		if (marts > 49) p_ptr->num_blow++;

		if (monk_heavy_armor(p_ptr)) p_ptr->num_blow /= 2;

		p_ptr->num_blow += 1 + p_ptr->extra_blows;

		if (!monk_heavy_armor(p_ptr))
		{
			p_ptr->to_h += (marts / 3);
			p_ptr->to_d += (marts / 3);

			p_ptr->dis_to_h += (marts / 3);
			p_ptr->dis_to_d += (marts / 3);
		}
	}
#endif

	/* Hell mode is HARD */
	if ((p_ptr->mode & MODE_HELL) && (p_ptr->num_blow > 1)) p_ptr->num_blow--;

	/* Combat bonus to damage */
	if (get_skill(p_ptr, SKILL_COMBAT))
	{
		int lev = get_skill_scale(p_ptr, SKILL_COMBAT, 10);

		p_ptr->to_d += lev;
		p_ptr->dis_to_d += lev;
	}

	if (get_skill(p_ptr, SKILL_DODGE))
//	if (!(r_ptr->flags1 & RF1_NEVER_MOVE));		// not for now
	{
		/* Get the armor weight */
		int cur_wgt = armour_weight(p_ptr);

		/* Base dodge chance */
		p_ptr->dodge_chance = get_skill_scale(p_ptr, SKILL_DODGE, 150);

		/* Armor weight bonus/penalty */
//		p_ptr->dodge_chance -= cur_wgt * 2;
		p_ptr->dodge_chance -= cur_wgt;		/* XXX adjust me */

		/* Encumberance bonus/penalty */
		p_ptr->dodge_chance -= p_ptr->total_weight / 100;

		/* Penalty for bad conditions */
		p_ptr->dodge_chance -= UNAWARENESS(p_ptr);

		/* Never below 0 */
		if (p_ptr->dodge_chance < 0) p_ptr->dodge_chance = 0;
	}

	/* Assume okay */
	p_ptr->icky_wield = FALSE;
	p_ptr->awkward_wield = FALSE;

	/* 2handed weapon and shield = less damage */
//	if (inventory[INVEN_WIELD + i].k_idx && inventory[INVEN_ARM + i].k_idx)
	if (p_ptr->inventory[INVEN_WIELD].k_idx && p_ptr->inventory[INVEN_ARM].k_idx)
	{
		/* Extract the item flags */
		object_flags(&p_ptr->inventory[INVEN_WIELD], &f1, &f2, &f3, &f4, &f5, &esp);

		if (f4 & TR4_COULD2H)
		{                
			/* Reduce the real bonuses */
			if (p_ptr->to_h > 0) p_ptr->to_h = (3 * p_ptr->to_h) / 4;
			if (p_ptr->to_d > 0) p_ptr->to_d = (3 * p_ptr->to_d) / 4;

			/* Reduce the mental bonuses */
			if (p_ptr->dis_to_h > 0) p_ptr->dis_to_h = (3 * p_ptr->dis_to_h) / 4;
			if (p_ptr->dis_to_d > 0) p_ptr->dis_to_d = (3 * p_ptr->dis_to_d) / 4;

			p_ptr->awkward_wield = TRUE;
		}
	}

	/* Priest weapon penalty for non-blessed edged weapons */
//	if ((p_ptr->pclass == 2) && (!p_ptr->bless_blade) &&
	if ((get_skill(p_ptr, SKILL_PRAY)) && (!p_ptr->bless_blade) &&
	    ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM)))
	{
		/* Reduce the real bonuses */
		p_ptr->to_h -= 2;
		p_ptr->to_d -= 2;

		/* Reduce the mental bonuses */
		p_ptr->dis_to_h -= 2;
		p_ptr->dis_to_d -= 2;

		/* Icky weapon */
		p_ptr->icky_wield = TRUE;
	}

	/* Redraw plusses to hit/damage if necessary */
	if ((p_ptr->dis_to_h != old_dis_to_h) || (p_ptr->dis_to_d != old_dis_to_d))
	{
		/* Redraw plusses */
		p_ptr->redraw |= (PR_PLUSSES);
	}

	/* Affect Skill -- stealth (bonus one) */
	p_ptr->skill_stl += 1;

	/* Affect Skill -- disarming (DEX and INT) */
	p_ptr->skill_dis += adj_dex_dis[p_ptr->stat_ind[A_DEX]];
	p_ptr->skill_dis += adj_int_dis[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- magic devices (INT) */
	p_ptr->skill_dev += get_skill_scale(p_ptr, SKILL_DEVICE, 20);

	/* Affect Skill -- saving throw (WIS) */
	p_ptr->skill_sav += adj_wis_sav[p_ptr->stat_ind[A_WIS]];

	/* Affect Skill -- digging (STR) */
	p_ptr->skill_dig += adj_str_dig[p_ptr->stat_ind[A_STR]];

	/* Affect Skill -- disarming (Level, by Class) */
	p_ptr->skill_dis += (p_ptr->cp_ptr->x_dis * get_skill(p_ptr, SKILL_DISARM) / 10);

	/* Affect Skill -- magic devices (Level, by Class) */
	p_ptr->skill_dev += (p_ptr->cp_ptr->x_dev * get_skill(p_ptr, SKILL_DEVICE) / 10);

	/* Affect Skill -- saving throw (Level, by Class) */
	p_ptr->skill_sav += (p_ptr->cp_ptr->x_sav * p_ptr->lev / 10);

#if 0	// doh, it's all zero!!
	/* Affect Skill -- stealth (Level, by Class) */
	p_ptr->skill_stl += (p_ptr->cp_ptr->x_stl * get_skill(p_ptr, SKILL_STEALTH) / 10);

	/* Affect Skill -- search ability (Level, by Class) */
	p_ptr->skill_srh += (p_ptr->cp_ptr->x_srh * get_skill(p_ptr, SKILL_SNEAKINESS) / 10);

	/* Affect Skill -- search frequency (Level, by Class) */
	p_ptr->skill_fos += (p_ptr->cp_ptr->x_fos * get_skill(p_ptr, SKILL_SNEAKINESS) / 10);
#else
	/* Affect Skill -- stealth (Level, by Class) */
	p_ptr->skill_stl += (get_skill_scale(p_ptr, SKILL_STEALTH, p_ptr->cp_ptr->x_stl * 5)) + get_skill_scale(p_ptr, SKILL_STEALTH, 25);

	/* Affect Skill -- search ability (Level, by Class) */
	p_ptr->skill_srh += (get_skill_scale(p_ptr, SKILL_SNEAKINESS, p_ptr->cp_ptr->x_srh * 5)) + get_skill(p_ptr, SKILL_SNEAKINESS);

	/* Affect Skill -- search frequency (Level, by Class) */
	p_ptr->skill_fos += (get_skill_scale(p_ptr, SKILL_SNEAKINESS, p_ptr->cp_ptr->x_fos * 5)) + get_skill(p_ptr, SKILL_SNEAKINESS);


#endif	// 0

	/* Affect Skill -- combat (normal) (Level, by Class) */
        p_ptr->skill_thn += (p_ptr->cp_ptr->x_thn * (((7 * get_skill(p_ptr, SKILL_MASTERY)) + (3 * get_skill(p_ptr, SKILL_COMBAT))) / 10) / 10);

	/* Affect Skill -- combat (shooting) (Level, by Class) */
	p_ptr->skill_thb += (p_ptr->cp_ptr->x_thb * (((7 * get_skill(p_ptr, SKILL_ARCHERY)) + (3 * get_skill(p_ptr, SKILL_COMBAT))) / 10) / 10);

	/* Affect Skill -- combat (throwing) (Level, by Class) */
	p_ptr->skill_tht += (p_ptr->cp_ptr->x_thb * get_skill_scale(p_ptr, SKILL_COMBAT, 10));

#if 0	// moved to player_invis
	if (p_ptr->aggravate)
	{
		if (p_ptr->tim_invisibility || magik(1))	/* don't be too noisy */
			msg_print(Ind, "You somewhat feel your presence is known.");
		p_ptr->invis = 0;
		p_ptr->tim_invisibility = 0;
		p_ptr->tim_invis_power = 0;
	}
	else
	{
#if 0 // DGDGDGDG -- no more monks for the time being
                if (p_ptr->pclass == CLASS_MONK)
		{
			p_ptr->skill_stl += (p_ptr->lev/10); /* give a stealth bonus */
                }
#endif
	}
#endif	// 0


	/* Limit Skill -- stealth from 0 to 30 */
	if (p_ptr->skill_stl > 30) p_ptr->skill_stl = 30;
	if (p_ptr->skill_stl < 0) p_ptr->skill_stl = 0;

	/* Limit Skill -- digging from 1 up */
	if (p_ptr->skill_dig < 1) p_ptr->skill_dig = 1;

	if ((p_ptr->anti_magic) && (p_ptr->skill_sav < 95)) p_ptr->skill_sav = 95;

	/* Limit Skill -- saving throw upto 95 */
	if (p_ptr->skill_sav > 95) p_ptr->skill_sav = 95;


	/* Take note when "heavy bow" changes */
	if (p_ptr->old_heavy_shoot != p_ptr->heavy_shoot)
	{
		/* Message */
		if (p_ptr->heavy_shoot)
		{
			msg_print(Ind, "You have trouble wielding such a heavy bow.");
		}
		else if (p_ptr->inventory[INVEN_BOW].k_idx)
		{
			msg_print(Ind, "You have no trouble wielding your bow.");
		}
		else
		{
			msg_print(Ind, "You feel relieved to put down your heavy bow.");
		}

		/* Save it */
		p_ptr->old_heavy_shoot = p_ptr->heavy_shoot;
	}


	/* Take note when "heavy weapon" changes */
	if (p_ptr->old_heavy_wield != p_ptr->heavy_wield)
	{
		/* Message */
		if (p_ptr->heavy_wield)
		{
			msg_print(Ind, "You have trouble wielding such a heavy weapon.");
		}
		else if (p_ptr->inventory[INVEN_WIELD].k_idx)
		{
			msg_print(Ind, "You have no trouble wielding your weapon.");
		}
		else
		{
			msg_print(Ind, "You feel relieved to put down your heavy weapon.");
		}

		/* Save it */
		p_ptr->old_heavy_wield = p_ptr->heavy_wield;
	}


	/* Take note when "illegal weapon" changes */
	if (p_ptr->old_icky_wield != p_ptr->icky_wield)
	{
		/* Message */
		if (p_ptr->icky_wield)
		{
			msg_print(Ind, "You do not feel comfortable with your weapon.");
		}
		else if (p_ptr->inventory[INVEN_WIELD].k_idx)
		{
			msg_print(Ind, "You feel comfortable with your weapon.");
		}
		else
		{
			msg_print(Ind, "You feel more comfortable after removing your weapon.");
		}

		/* Save it */
		p_ptr->old_icky_wield = p_ptr->icky_wield;
	}

	/* Take note when "illegal weapon" changes */
	if (p_ptr->old_awkward_wield != p_ptr->awkward_wield)
	{
		/* Message */
		if (p_ptr->awkward_wield)
		{
			msg_print(Ind, "You find it hard to fight with your weapon and shield.");
		}
		else if (p_ptr->inventory[INVEN_WIELD].k_idx)
		{
			msg_print(Ind, "You feel comfortable with your weapon.");
		}
		else if (!p_ptr->inventory[INVEN_ARM].k_idx)
		{
			msg_print(Ind, "You feel more dexterous after removing your shield.");
		}

		/* Save it */
		p_ptr->old_awkward_wield = p_ptr->awkward_wield;
	}

	/* resistance to fire cancel sensibility to fire */
	if(p_ptr->resist_fire || p_ptr->oppose_fire || p_ptr->immune_fire)
		p_ptr->sensible_fire=FALSE;

	/* XXX - Always resend skills */
	p_ptr->redraw |= (PR_SKILLS);
}



/*
 * Handle "p_ptr->notice"
 */
void notice_stuff(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Notice stuff */
	if (!p_ptr->notice) return;


	/* Combine the pack */
	if (p_ptr->notice & PN_COMBINE)
	{
		p_ptr->notice &= ~(PN_COMBINE);
		combine_pack(Ind);
	}

	/* Reorder the pack */
	if (p_ptr->notice & PN_REORDER)
	{
		p_ptr->notice &= ~(PN_REORDER);
		reorder_pack(Ind);
	}
}


/*
 * Handle "p_ptr->update"
 */
void update_stuff(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Update stuff */
	if (!p_ptr->update) return;

	/* This should only be sent once. This data
	   does not change in runtime */
	if (p_ptr->update & PU_SKILL_INFO)
        {
                int i;

		p_ptr->update &= ~(PU_SKILL_INFO);
                for (i = 1; i < MAX_SKILLS; i++)
                {
                        if (s_info[i].name)
                        {
#if 0
                                Send_skill_init(Ind, PKT_SKILL_INIT_NAME, i);
                                Send_skill_init(Ind, PKT_SKILL_INIT_DESC, i);
                                if (s_info[i].action_desc != NULL)
                                        Send_skill_init(Ind, PKT_SKILL_INIT_MKEY, i);
#else
				Send_skill_init(Ind, i);
#endif
                        }
                }
	}

	if (p_ptr->update & PU_SKILL_MOD)
        {
                int i;

		p_ptr->update &= ~(PU_SKILL_MOD);
                for (i = 1; i < MAX_SKILLS; i++)
                {
                        if (s_info[i].name)
                        {
                                Send_skill_info(Ind, i);
                        }
                }
	}

	if (p_ptr->update & PU_BONUS)
	{
		p_ptr->update &= ~(PU_BONUS);
		calc_bonuses(Ind);
	}

	if (p_ptr->update & PU_TORCH)
	{
		p_ptr->update &= ~(PU_TORCH);
		calc_torch(Ind);
	}

	if (p_ptr->update & PU_HP)
	{
		p_ptr->update &= ~(PU_HP);
		calc_hitpoints(Ind);
	}

	if (p_ptr->update & (PU_SANITY))
	{
		p_ptr->update &= ~(PU_SANITY);
		calc_sanity(Ind);
	}

	if (p_ptr->update & PU_MANA)
	{
		p_ptr->update &= ~(PU_MANA);
		calc_mana(Ind);
	}

	if (p_ptr->update & PU_SPELLS)
	{
		p_ptr->update &= ~(PU_SPELLS);
		calc_spells(Ind);
	}


	/* Character is not ready yet, no screen updates */
	/*if (!character_generated) return;*/

	/* Character has changed depth very recently, no screen updates */
	if (p_ptr->new_level_flag) return;

	if (p_ptr->update & PU_UN_LITE)
	{
		p_ptr->update &= ~(PU_UN_LITE);
		forget_lite(Ind);
	}

	if (p_ptr->update & PU_UN_VIEW)
	{
		p_ptr->update &= ~(PU_UN_VIEW);
		forget_view(Ind);
	}


	if (p_ptr->update & PU_VIEW)
	{
		p_ptr->update &= ~(PU_VIEW);
		update_view(Ind);
	}

	if (p_ptr->update & PU_LITE)
	{
		p_ptr->update &= ~(PU_LITE);
		update_lite(Ind);
	}


	if (p_ptr->update & PU_FLOW)
	{
		p_ptr->update &= ~(PU_FLOW);
		update_flow();
	}


	if (p_ptr->update & PU_DISTANCE)
	{
		p_ptr->update &= ~(PU_DISTANCE);
		p_ptr->update &= ~(PU_MONSTERS);
		update_monsters(TRUE);
		update_players();
	}

	if (p_ptr->update & PU_MONSTERS)
	{
		p_ptr->update &= ~(PU_MONSTERS);
		update_monsters(FALSE);
		update_players();
	}

	if(p_ptr->update & PU_LUA){
		/* update the client files */
		p_ptr->update &= ~(PU_LUA);
		remote_update(p_ptr->conn, "scpt/c-init.lua");
		remote_update(p_ptr->conn, "scpt/spells.lua");
		remote_update(p_ptr->conn, "scpt/s_convey.lua");
		remote_update(p_ptr->conn, "scpt/s_aux.lua");
	}
}


/*
 * Handle "p_ptr->redraw"
 */
void redraw_stuff(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Redraw stuff */
	if (!p_ptr->redraw) return;


	/* Character is not ready yet, no screen updates */
	/*if (!character_generated) return;*/

	/* Hack -- clear the screen */
	if (p_ptr->redraw & PR_WIPE)
	{
		p_ptr->redraw &= ~PR_WIPE;
		msg_print(Ind, NULL);
	}


	if (p_ptr->redraw & PR_MAP)
	{
		p_ptr->redraw &= ~(PR_MAP);
		prt_map(Ind);
	}


	if (p_ptr->redraw & PR_BASIC)
	{
		p_ptr->redraw &= ~(PR_BASIC);
		p_ptr->redraw &= ~(PR_MISC | PR_TITLE | PR_STATS);
		p_ptr->redraw &= ~(PR_LEV | PR_EXP | PR_GOLD);
		p_ptr->redraw &= ~(PR_ARMOR | PR_HP | PR_MANA);
		p_ptr->redraw &= ~(PR_DEPTH | PR_HEALTH);
		prt_frame_basic(Ind);
	}

	if (p_ptr->redraw & PR_MISC)
	{
		p_ptr->redraw &= ~(PR_MISC);
		Send_char_info(Ind, p_ptr->prace, p_ptr->pclass, p_ptr->male);
	}

	if (p_ptr->redraw & PR_TITLE)
	{
		p_ptr->redraw &= ~(PR_TITLE);
		prt_title(Ind);
	}

	if (p_ptr->redraw & PR_LEV)
	{
		p_ptr->redraw &= ~(PR_LEV);
		prt_level(Ind);
	}

	if (p_ptr->redraw & PR_EXP)
	{
		p_ptr->redraw &= ~(PR_EXP);
		prt_exp(Ind);
	}

	if (p_ptr->redraw & PR_STATS)
	{
		p_ptr->redraw &= ~(PR_STATS);
		prt_stat(Ind, A_STR);
		prt_stat(Ind, A_INT);
		prt_stat(Ind, A_WIS);
		prt_stat(Ind, A_DEX);
		prt_stat(Ind, A_CON);
		prt_stat(Ind, A_CHR);
	}

	if (p_ptr->redraw & PR_ARMOR)
	{
		p_ptr->redraw &= ~(PR_ARMOR);
		prt_ac(Ind);
	}

	if (p_ptr->redraw & PR_HP)
	{
		p_ptr->redraw &= ~(PR_HP);
		prt_hp(Ind);
	}

	if(p_ptr->redraw & PR_SANITY)
	{
		p_ptr->redraw &= ~(PR_SANITY);
#ifdef SHOW_SANITY
		prt_sanity(Ind);
#endif
	}

	if (p_ptr->redraw & PR_MANA)
	{
		p_ptr->redraw &= ~(PR_MANA);
		prt_sp(Ind);
	}

	if (p_ptr->redraw & PR_GOLD)
	{
		p_ptr->redraw &= ~(PR_GOLD);
		prt_gold(Ind);
	}

	if (p_ptr->redraw & PR_DEPTH)
	{
		p_ptr->redraw &= ~(PR_DEPTH);
		prt_depth(Ind);
	}

	if (p_ptr->redraw & PR_HEALTH)
	{
		p_ptr->redraw &= ~(PR_HEALTH);
		health_redraw(Ind);
	}

	if (p_ptr->redraw & PR_HISTORY)
	{
		p_ptr->redraw &= ~(PR_HISTORY);
		prt_history(Ind);
	}

	if (p_ptr->redraw & PR_VARIOUS)
	{
		p_ptr->redraw &= ~(PR_VARIOUS);
		prt_various(Ind);
	}

	if (p_ptr->redraw & PR_PLUSSES)
	{
		p_ptr->redraw &= ~(PR_PLUSSES);
		prt_plusses(Ind);
	}

	if (p_ptr->redraw & PR_SKILLS)
	{
		p_ptr->redraw &= ~(PR_SKILLS);
		prt_skills(Ind);
	}

	if (p_ptr->redraw & PR_EXTRA)
	{
		p_ptr->redraw &= ~(PR_EXTRA);
		p_ptr->redraw &= ~(PR_CUT | PR_STUN);
		p_ptr->redraw &= ~(PR_HUNGER);
		p_ptr->redraw &= ~(PR_BLIND | PR_CONFUSED);
		p_ptr->redraw &= ~(PR_AFRAID | PR_POISONED);
		p_ptr->redraw &= ~(PR_STATE | PR_SPEED | PR_STUDY);
		prt_frame_extra(Ind);
	}

	if (p_ptr->redraw & PR_CUT)
	{
		p_ptr->redraw &= ~(PR_CUT);
		prt_cut(Ind);
	}

	if (p_ptr->redraw & PR_STUN)
	{
		p_ptr->redraw &= ~(PR_STUN);
		prt_stun(Ind);
	}

	if (p_ptr->redraw & PR_HUNGER)
	{
		p_ptr->redraw &= ~(PR_HUNGER);
		prt_hunger(Ind);
	}

	if (p_ptr->redraw & PR_BLIND)
	{
		p_ptr->redraw &= ~(PR_BLIND);
		prt_blind(Ind);
	}

	if (p_ptr->redraw & PR_CONFUSED)
	{
		p_ptr->redraw &= ~(PR_CONFUSED);
		prt_confused(Ind);
	}

	if (p_ptr->redraw & PR_AFRAID)
	{
		p_ptr->redraw &= ~(PR_AFRAID);
		prt_afraid(Ind);
	}

	if (p_ptr->redraw & PR_POISONED)
	{
		p_ptr->redraw &= ~(PR_POISONED);
		prt_poisoned(Ind);
	}

	if (p_ptr->redraw & PR_STATE)
	{
		p_ptr->redraw &= ~(PR_STATE);
		prt_state(Ind);
	}

	if (p_ptr->redraw & PR_SPEED)
	{
		p_ptr->redraw &= ~(PR_SPEED);
		prt_speed(Ind);
	}

	if (p_ptr->redraw & PR_STUDY)
	{
		p_ptr->redraw &= ~(PR_STUDY);
		prt_study(Ind);
	}
}


/*
 * Handle "p_ptr->window"
 */
void window_stuff(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Window stuff */
	if (!p_ptr->window) return;

	/* Display inventory */
	if (p_ptr->window & PW_INVEN)
	{
		p_ptr->window &= ~(PW_INVEN);
		fix_inven(Ind);
	}

	/* Display equipment */
	if (p_ptr->window & PW_EQUIP)
	{
		p_ptr->window &= ~(PW_EQUIP);
		fix_equip(Ind);
	}

	/* Display spell list */
	if (p_ptr->window & PW_SPELL)
	{
		p_ptr->window &= ~(PW_SPELL);
		fix_spell(Ind, 0);
	}

	/* Display player */
	if (p_ptr->window & PW_PLAYER)
	{
		p_ptr->window &= ~(PW_PLAYER);
		fix_player(Ind);
	}

	/* Display overhead view */
	if (p_ptr->window & PW_MESSAGE)
	{
		p_ptr->window &= ~(PW_MESSAGE);
		fix_message(Ind);
	}

	/* Display overhead view */
	if (p_ptr->window & PW_OVERHEAD)
	{
		p_ptr->window &= ~(PW_OVERHEAD);
		fix_overhead(Ind);
	}

	/* Display monster recall */
	if (p_ptr->window & PW_MONSTER)
	{
		p_ptr->window &= ~(PW_MONSTER);
		fix_monster(Ind);
	}
}


/*
 * Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window"
 */
void handle_stuff(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Hack -- delay updating */
	if (p_ptr->new_level_flag) return;

	/* Update stuff */
	if (p_ptr->update) update_stuff(Ind);

	/* Redraw stuff */
	if (p_ptr->redraw) redraw_stuff(Ind);

	/* Window stuff */
	if (p_ptr->window) window_stuff(Ind);
}


