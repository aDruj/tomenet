/* $Id$ */
/* File: skills.c */

/* Purpose: player skills */

/*
 * Copyright (c) 2001 DarkGod
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

static int get_idx(int i);

/*
 * Given the name of a skill, returns skill index or -1 if no
 * such skill is found
 */
#if 0 /* not used - mikaelh */
static s16b find_skill(cptr name)
{
	u16b i;

	/* Scan skill list */
	for (i = 1; i < MAX_SKILLS; i++)
	{
		/* The name matches */
		if (streq((char*)s_info[i].name, name)) return (i);
	}

	/* No match found */
	return (-1);
}
#endif


/*
 *
 */
s16b get_skill(int skill)
{
	return (p_ptr->s_info[skill].value / SKILL_STEP);
}


/*
 *
 */
s16b get_skill_scale(player_type *pfft, int skill, u32b scale)
{
	/* XXX XXX XXX */
	return (((p_ptr->s_info[skill].value / 10) * (scale * (SKILL_STEP / 10)) /
	         (SKILL_MAX / 10)) /
	        (SKILL_STEP / 10));
}

/*
 *
 */
static int get_idx(int i)
{
	int j;

	for (j = 1; j < MAX_SKILLS; j++)
	{
		if (s_info[j].order == i)
			return (j);
                        }
	return (0);
}


static bool has_child(int sel)
{
	int i;

	for (i = 1; i < MAX_SKILLS; i++)
	{
		if (s_info[i].father == sel)
			return (TRUE);
	}
	return (FALSE);
}

static bool has_active_child(int sel)
{
	int i;

	for (i = 1; i < MAX_SKILLS; i++)
	{
		if ((s_info[i].father == sel) &&
		    ((p_ptr->s_info[i].mod) || (p_ptr->s_info[i].value) ||
		    has_active_child(i)))
			return (TRUE);
	}
	return (FALSE);
}

/*
 *
 */
static void init_table_aux(int table[MAX_SKILLS][2], int *idx, int father, int lev, bool full)
{
	int j, i;

	for (j = 1; j < MAX_SKILLS; j++)
	{
		i = get_idx(j);

		if (s_info[i].father != father) continue;
		if (p_ptr->s_info[i].flags1 & SKF1_HIDDEN) continue;

		/* new option: hide all completely unusable skill branches - C. Blue */
		if (c_cfg.hide_unusable_skills &&
		    !p_ptr->s_info[i].mod && !p_ptr->s_info[i].value &&
		    !has_active_child(i))
			continue;

		table[*idx][0] = i;
		table[*idx][1] = lev;
		(*idx)++;
		if (p_ptr->s_info[i].dev || full) init_table_aux(table, idx, i, lev + 1, full);
	}
}


static void init_table(int table[MAX_SKILLS][2], int *max, bool full)
{
	*max = 0;
	init_table_aux(table, max, -1, 0, full);
}


static void print_desc_aux(cptr txt, int y, int xx)
{
	int i = -1, x = xx;


	while (txt[++i] != 0)
	{
		if (txt[i] == '\n')
		{
			x = xx;
			y++;
		}
		else
		{
			Term_putch(x++, y, TERM_YELLOW, txt[i]);
		}
	}
}

/*
 * Dump the skill tree
 */
void dump_skills(FILE *fff)
{
	int i, j, max = 0;
	int table[MAX_SKILLS][2];
	char buf[80];

	init_table(table, &max, TRUE);

	Term_clear();

	fprintf(fff, "\nSkills (points left: %d)", p_ptr->skill_points);

	for (j = 0; j < max; j++)
	{
		int z;

		i = table[j][0];

		/* XXX this causes strange dump when one has SKILL_DEVICE and not
		 * SKILL_MAGIC, for example.
		 * We should make sure the skill doesn't have 'valid' children!
		 */
		if ((p_ptr->s_info[i].value == 0) && (i != SKILL_MISC))
		{
			if (p_ptr->s_info[i].mod == 0) continue;
		}

/*		sprintf(buf, "\n");		*/
		fprintf(fff, "\n");

/*		sprintf(buf, "");		*/
		buf[0]='\0';

		for (z = 0; z < table[j][1]; z++) strcat(buf, "    ");

		if (!has_child(i))
		{
			strcat(buf, format(" . %s", s_info[i].name));
		}
		else if (!has_active_child(i))
		{
			strcat(buf, format(" o %s", s_info[i].name));
		}
		else
		{
			strcat(buf, format(" - %s", s_info[i].name));
		}

		if (!(p_ptr->s_info[i].flags1 & SKF1_DUMMY))
			fprintf(fff, "%-50s%02d.%03d [%0d.%03d]",
		    	    buf, p_ptr->s_info[i].value / SKILL_STEP, p_ptr->s_info[i].value % SKILL_STEP,
		    	    p_ptr->s_info[i].mod / 1000, p_ptr->s_info[i].mod % 1000);
	}
	fprintf(fff, "\n");
}

/*
 * Draw the skill tree
 */
static void print_skills(int table[MAX_SKILLS][2], int max, int sel, int start)
{
	int i, j;
	int wid, hgt;

	Term_clear();
	Term_get_size(&wid, &hgt);

	if (c_cfg.rogue_like_commands)
	{
		c_prt(TERM_WHITE, " === TomeNET Skills Screen ===  [move:j,k,g,G,#  fold:<CR>,c,o  advance:l]", 0, 0);
	}
	else
	{
		c_prt(TERM_WHITE, " === TomeNET Skills Screen ===  [move:2,8,g,G,#  fold:<CR>,c,o  advance:6]", 0, 0);
	}

	c_prt((p_ptr->skill_points) ? TERM_L_BLUE : TERM_L_RED,
	      format("Skill points left: %d", p_ptr->skill_points), 1, 0);
	print_desc_aux((char*)s_info[table[sel][0]].desc, 2, 0);

	for (j = start; j < start + (hgt - 4); j++)
	{
		byte color = TERM_WHITE;
		char deb = ' ', end = ' ';

		if (j >= max) break;

		i = table[j][0];

		if ((p_ptr->s_info[i].value == 0))
		{
			if (p_ptr->s_info[i].mod == 0) color = TERM_L_DARK;
			else color = TERM_ORANGE;
		}
		else if ((p_ptr->s_info[i].value == SKILL_MAX) ||
			((p_ptr->s_info[i].flags1 & SKF1_MAX_1) && (p_ptr->s_info[i].value == 1000)))
			color = TERM_L_BLUE;
		if (p_ptr->s_info[i].flags1 & SKF1_HIDDEN) color = TERM_L_RED;
		if (p_ptr->s_info[i].flags1 & SKF1_DUMMY) color = TERM_SLATE;

		if (j == sel)
		{
			color = TERM_L_GREEN;
			deb = '[';
			end = ']';
		}
		if (!has_child(i))
		{
			c_prt(color, format("%c.%c%s", deb, end, s_info[i].name),
			      j + 4 - start, table[j][1] * 4);
		}
		else if (!has_active_child(i))
		{
			c_prt(color, format("%co%c%s", deb, end, s_info[i].name),
			      j + 4 - start, table[j][1] * 4);
		}
		else if (p_ptr->s_info[i].dev)
		{
			c_prt(color, format("%c-%c%s", deb, end, s_info[i].name),
			      j + 4 - start, table[j][1] * 4);
		}
		else
		{
			c_prt(color, format("%c+%c%s", deb, end, s_info[i].name),
			      j + 4 - start, table[j][1] * 4);
		}
		
		if (!(p_ptr->s_info[i].flags1 & SKF1_DUMMY))
			c_prt(color,
			      format("%02ld.%03ld [%01d.%03d]",
			         p_ptr->s_info[i].value / SKILL_STEP, p_ptr->s_info[i].value % SKILL_STEP,
			         p_ptr->s_info[i].mod / 1000, p_ptr->s_info[i].mod % 1000),
				 j + 4 - start, 60);
	}

	/* Hack - Get rid of the cursor - mikaelh */
	Term->scr->cx = Term->wid;
	Term->scr->cu = 1;
}

/*
 * Redraw the skill menu if possible.
 */
static bool hack_do_cmd_skill = FALSE;
static int table[MAX_SKILLS][2];
static int sel = 0;
static int start = 0;
static int max;
void do_redraw_skills() {
	redraw_skills = FALSE;

	if (hack_do_cmd_skill) {
		print_skills(table, max, sel, start);
	}
}

/*
 * Interact with skills.
 */
void do_cmd_skill()
{
	char c;
	int i;
	int wid, hgt;

	/* Initialize global variables */
	sel = 0;
	start = 0;

	/* Save the screen */
	Term_save();

	/* Clear the screen */
	Term_clear();

	/* Skill menu open */
	hack_do_cmd_skill = TRUE;

	/* Initialise the skill list */
	init_table(table, &max, FALSE);

	while (TRUE)
	{
		Term_get_size(&wid, &hgt);

		/* Display list of skills */
		print_skills(table, max, sel, start);

		c = inkey();

		/* Leave the skill screen */
		if (c == ESCAPE || c == KTRL('X')) break;

		/* Take a screenshot */
		else if (c == KTRL('T'))
		{
			xhtml_screenshot("screenshot????");
		}

		/* Expand / collapse list of skills */
		else if (c == '\r')
		{
			if (p_ptr->s_info[table[sel][0]].dev) p_ptr->s_info[table[sel][0]].dev = FALSE;
			else p_ptr->s_info[table[sel][0]].dev = TRUE;
			init_table(table, &max, FALSE);
		}
		else if (c == 'c')
		{
			for (i = 0; i < max; i++)
				p_ptr->s_info[table[i][0]].dev = FALSE;

			init_table(table, &max, FALSE);
			start = sel = 0;
		}
		else if (c == 'o')
		{
			for (i = 0; i < max; i++)
				p_ptr->s_info[table[i][0]].dev = TRUE;

			init_table(table, &max, FALSE);
			/* TODO: memorize and recover the cursor position */
		}

		else if (c == 'g')
		{
			start = sel = 0;
		}
		else if (c == 'G')
		{
			sel = max - 1;
			start = sel - (hgt - 4);
			if (sel >= start + (hgt - 4)) start = sel - (hgt - 4) + 1;
		}
		/* Hack -- go to a specific line */
		else if (c == '#')
		{
			char tmp[80];
			prt(format("Goto Line(max %d): ", max), 23, 0);
			strcpy(tmp, "1");
			if (askfor_aux(tmp, 10, 0))
			{
				sel = start = atoi(tmp) - 1;
				if (sel >= max) sel = start = max - 1;
				if (sel < 0) sel = start = 0;
			}
		}

		/* Next page */
		else if (c == 'n' || c == ' ')
		{
			sel += (hgt - 4);
			start += (hgt - 4);
			if (sel >= max) sel = max - 1;
			if (start >= max) start = max - 1;
		}

		/* Previous page */
		else if (c == 'p' || c == 'b')
		{
			sel -= (hgt - 4);
			start -= (hgt - 4);
			if (sel < 0) sel = 0;
			if (start < 0) start = 0;
		}

		/* Select / increase a skill */
		else
		{
			int dir = c;

			/* Move cursor down */
			if (dir == '2' || dir == 'j') sel++;

			/* Move cursor up */
			if (dir == '8' || dir == 'k') sel--;

			/* Increase the current skill */
			if (dir == '6' || dir == 'l')
			{
				/* Send a packet */
				Send_skill_mod(table[sel][0]);
			}

			/* XXX XXX XXX Wizard mode commands outside of wizard2.c */

			/* Handle boundaries and scrolling */
			if (sel < 0) sel = max - 1;
			if (sel >= max) sel = 0;
			if (sel < start) start = sel;
			if (sel >= start + (hgt - 4)) start = sel - (hgt - 4) + 1;
		}
	}

	/* Load the screen */
	Term_load();

	/* Skill menu not open anymore */
	hack_do_cmd_skill = FALSE;

	/* Flush the queue */
	Flush_queue();
}

/*
 * Print a batch of skills.
 */
static void print_skill_batch(int *p, int start, int max, bool mode)
{
	char buff[80];
	int i = start, j = 0;

	if (mode) prt(format("         %-31s", "Name"), 1, 20);

	for (i = start; i < (start + 20); i++)
	{
		if (i >= max) break;

		if (p[i] > 0)
			sprintf(buff, "  %c-%3d) %-30s", I2A(j), s_info[p[i]].action_mkey, (char*)s_info[p[i]].action_desc);
		else
			sprintf(buff, "  %c-%3d) %-30s", I2A(j), 1, "Change melee style");

		if (mode) prt(buff, 2 + j, 20);
		j++;
	}
	if (mode) prt("", 2 + j, 20);
	prt(format("Select a skill (a-%c), * to list, @ to select by name/No., +/- to scroll:", I2A(j - 1)), 0, 0);
}

static int do_cmd_activate_skill_aux()
{
	char which;
	int max = 0, i, start = 0;
	int ret;
	bool mode = c_cfg.always_show_lists;
	int *p;
	bool term_saved = FALSE;

	C_MAKE(p, MAX_SKILLS, int);

	for (i = 1; i < MAX_SKILLS; i++)
	{
		if (s_info[i].action_mkey && p_ptr->s_info[i].value)
		{
			int j;
			bool next = FALSE;

			/* Already got it ? */
			for (j = 0; j < max; j++)
			{
				if (s_info[i].action_mkey == s_info[p[j]].action_mkey)
				{
					next = TRUE;
					break;
				}
			}
			if (next) continue;

			p[max++] = i;
		}
	}

	if (!max)
	{
		c_msg_print("You don't have any activable skills.");
		return -1;
	}
/*	if (max == 1 && c_cfg.quick_messages)
	{
		return p[0];
	}
*/

	/* Save the terminal if c_cfg.always_show_lists was on */
	if (mode && !term_saved)
	{
		Term_save();
		term_saved = TRUE;
	}

	topline_icky = TRUE;

	while (1)
	{
		print_skill_batch(p, start, max, mode);
		which = inkey();

		if (which == ESCAPE)
		{
			ret = -1;
			break;
		}
		else if (which == KTRL('T'))
		{
			/* Take a screenshot */
			xhtml_screenshot("screenshot????");
		}
		else if (which == '*' || which == '?' || which == ' ')
		{
			mode = (mode)?FALSE:TRUE;
			if (!mode && term_saved)
			{
				Term_load();
				term_saved = FALSE;
			}
			else if (mode && !term_saved)
			{
				Term_save();
				term_saved = TRUE;
			}
		}
		else if (which == '+')
		{
			start += 20;
			if (start >= max) start -= 20;
			if (term_saved)
			{
				/* Draw the new list on a fresh copy of the screen */
				Term_load();
				Term_save();
			}
		}
		else if (which == '-')
		{
			start -= 20;
			if (start < 0) start += 20;
			if (term_saved)
			{
				/* Draw the new list on a fresh copy of the screen */
				Term_load();
				Term_save();
			}
		}
		else if (which == '@')
		{
			char buf[80];
			int nb;

			strcpy(buf, "Cast a spell");
			if (!get_string("Skill action? ", buf, 79)) {
				if (term_saved) Term_load();
				return FALSE;
			}

			/* Can we convert to a number? */
			nb = atoi(buf) - 1;

			/* Find the skill it is related to */
			for (i = 1; i < MAX_SKILLS; i++)
			{
				if (s_info[i].action_desc && (!strcmp(buf, (char*)s_info[i].action_desc) && get_skill(i)))
					break;
				if ((s_info[i].action_mkey == nb + 1) && (nb != -1) && get_skill(i))
					break;
			}
			if ((i < MAX_SKILLS))
			{
				ret = i;
				break;
			}

		}
		else
		{
			which = tolower(which);
			if (start + A2I(which) >= max)
			{
				bell();
				continue;
			}
			if (start + A2I(which) < 0)
			{
				bell();
				continue;
			}

			ret = p[start + A2I(which)];
			break;
		}
	}

	if (term_saved) Term_load();
	topline_icky = FALSE;

	/* Clear the prompt line */
	prt("", 0, 0);

	C_FREE(p, MAX_SKILLS, int);

	return ret;
}

/*
 * Hook to determine if an object is a device
 */
static bool item_tester_hook_device(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_ROD) ||
	    (o_ptr->tval == TV_STAFF) ||
	    (o_ptr->tval == TV_WAND)) return (TRUE);

	/* Assume not */
	return (FALSE);
}

/*
 * Hook to determine if an object is a potion
 */
static bool item_tester_hook_potion(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_POTION) ||
	    (o_ptr->tval == TV_POTION2) ||
	    (o_ptr->tval == TV_FLASK)) return (TRUE);

	/* Assume not */
	return (FALSE);
}

static bool item_tester_hook_scroll_rune(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_SCROLL) ||
	    (o_ptr->tval == TV_RUNE2)) return (TRUE);

	/* Assume not */
	return (FALSE);
}

/*
 * set a trap .. it's out of place somewhat.	- Jir -
 */
static void do_trap(int item_kit)
{
	int item_load;
	object_type *o_ptr;

	if (item_kit < 0)
	{
		item_tester_tval = TV_TRAPKIT;
		if (!c_get_item(&item_kit, "Use which trapping kit? ", (USE_INVEN)))
		{
			if (item_kit == -2)
				c_msg_print("You have no trapping kits.");
			return;
		}
	}

	o_ptr = &inventory[item_kit];

	/* Trap kits need a second object */
	switch (o_ptr->sval)
	{
		case SV_TRAPKIT_BOW:
			item_tester_tval = TV_ARROW;
			break;
		case SV_TRAPKIT_XBOW:
			item_tester_tval = TV_BOLT;
			break;
		case SV_TRAPKIT_SLING:
			item_tester_tval = TV_SHOT;
			break;
		case SV_TRAPKIT_POTION:
			item_tester_hook = item_tester_hook_potion;
			break;
		case SV_TRAPKIT_SCROLL_RUNE:
			item_tester_hook = item_tester_hook_scroll_rune;
			break;
		case SV_TRAPKIT_DEVICE:
			item_tester_hook = item_tester_hook_device;
			break;
		default:
			c_msg_print("Unknown trapping kit type!");
			break;
	}

	if (!c_get_item(&item_load, "Load with what? ", (USE_EQUIP | USE_INVEN)))
	{
		if (item_load == -2)
			c_msg_print("You have nothing to load that trap with.");
		return;
	}


	/* Send it */
	Send_activate_skill(MKEY_TRAP, item_kit, item_load, 0, 0, 0);
}

/*
 * cast a runic spell
 */
static void do_rune() {
	int basic_rune, mod_rune;
	object_type *o_ptr;

        //Ask for a basic rune
	item_tester_tval = TV_RUNE1;
	if (!c_get_item(&basic_rune, "Use which basic rune? ", (USE_INVEN)))
	{
		if (basic_rune == -2)
			c_msg_print("You have no runes.");
		return;
	}

	o_ptr = &inventory[basic_rune];

	//Ask for a modifier rune
	item_tester_tval = TV_RUNE2;

	if (!c_get_item(&mod_rune, "Use which modifier rune? ", (USE_EQUIP | USE_INVEN)))
	{
		if (mod_rune == -2)
			c_msg_print("You have nothing to use that with.");
		return;
	}

	/* Send it */
	Send_activate_skill(MKEY_RUNE, basic_rune, mod_rune, 0, 0, 0);
}

/*
 * Handle the mkey according to the types.
 * if item is less than zero, ask for an item if needed.
 */
void do_activate_skill(int x_idx, int item)
{
	int dir=0;
	s32b spell=0L;
	if (s_info[x_idx].flags1 & SKF1_MKEY_HARDCODE)
	{
		switch (s_info[x_idx].action_mkey)
		{
			case MKEY_MIMICRY:
				do_mimic();
				break;
			case MKEY_TRAP:
				do_trap(item);
				break;
			case MKEY_RUNE:
				do_rune();
				break;
#ifdef ENABLE_RCRAFT
			case MKEY_RCRAFT:
				do_runespell();
				break;
#endif
			case MKEY_STANCE:
				do_stance();
				break;
			case MKEY_MELEE:
				do_melee_technique();
				break;
			case MKEY_RANGED:
				do_ranged_technique();
				break;
			default:
				c_msg_print("Very sorry, you need more recent client.");
				break;
		}
		return;
	}
	else if (s_info[x_idx].action_mkey == MKEY_SCHOOL)
	{
		int item_obj = -1, aux=0;

		/* Ask for a spell, allow cancel */
		if ((spell = get_school_spell("cast", &item)) == -1) return;

		/* Ask for a direction? */
		dir = -1;
		if (exec_lua(0, format("return pre_exec_spell_dir(%d)", spell)))
			if (!get_dir(&dir))
				return;

                /* Ask for something? */
                if (exec_lua(0, format("return pre_exec_spell_extra(%d)", spell)))
                {
                        aux = exec_lua(0, "return __pre_exec_extra");
                }

                /* Ask for an item? */
                if (exec_lua(0, format("return pre_exec_spell_item(%d)", spell)))
                {
                        item_obj = exec_lua(0, "return __pre_exec_item");
                }

		/* Send it */
		Send_activate_skill(MKEY_SCHOOL, item, spell, dir, item_obj, aux);

		return;
	}
	else if (s_info[x_idx].flags1 & SKF1_MKEY_SPELL)
	{
		if (item < 0)
		{
			item_tester_tval = s_info[x_idx].tval;
			if (!c_get_item(&item, "Cast from which book? ", (USE_INVEN)))
			{
				if (item == -2)
					c_msg_print("You have no books that you can cast from.");
				return;
			}
		}

		/* Ask for a spell, allow cancel */
		if (!get_spell(&spell, "cast", item, FALSE)) return;

		/* Send it */
		Send_activate_skill(s_info[x_idx].action_mkey, item, spell, dir, 0, 0);

		return;
	}

	if (item < 0 && s_info[x_idx].flags1 & SKF1_MKEY_ITEM)
	{
		item_tester_tval = s_info[x_idx].tval;
		if (!c_get_item(&item, "Which item? ", (USE_EQUIP | USE_INVEN)))
		{
			return;
		}
	}

	if (s_info[x_idx].flags1 & SKF1_MKEY_DIRECTION)
	{
		if (!get_dir(&dir))
			return;
	}

	/* Send it */
	Send_activate_skill(s_info[x_idx].action_mkey, item, spell, dir, 0, 0);
}

/* Ask & execute a skill */
void do_cmd_activate_skill()
{
	int x_idx = -1;

	/* Get the skill, if available */
	x_idx = do_cmd_activate_skill_aux();
	if (x_idx == -1) return;

	do_activate_skill(x_idx, -1);
}
