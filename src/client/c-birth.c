/* $Id$ */
/* File: birth.c */

/* Purpose: create a player character */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#define CLIENT

#include "angband.h"

/* Cut down on some of the walls of text hitting the player during login process? */
#define SIMPLE_LOGIN

/* Choose class before race */
#define CLASS_BEFORE_RACE

/* Don't display 'Trait' if traits aren't available */
#define HIDE_UNAVAILABLE_TRAIT

/* For race/class/trait descriptions */
#define DIZ_ROW 3
#define DIZ_COL 29

/*
 * Choose the character's name
 */
static void choose_name(void)
{
	char tmp[23];

	/* Prompt and ask */
#ifndef SIMPLE_LOGIN
	c_put_str(TERM_SLATE, "If you are new to TomeNET, read this:", 7, 2);
	prt("http://www.tomenet.net/guide.php", 8, 2);
	c_put_str(TERM_SLATE, "*** Logging in with an account ***", 12, 2);
	prt("In order to play, you need to create an account.", 14, 2);
	prt("Your account can hold a maximum of 7 different characters to play with!", 15, 2);
	prt("If you don't have an account yet, just enter one of your choice and make sure", 16, 2);
	prt("that you remember its name and password. Each player should have not more", 17, 2);
	prt("than 1 account. Ask a server administrator to 'validate' your account!", 18, 2);
	prt("If an account is not validated, it has certain restrictions to prevent abuse.", 19, 2);
#else
	c_put_str(TERM_SLATE, "Welcome! In order to play, you need to create an account.", 1, 2);
	c_put_str(TERM_SLATE, "If you don't have an account yet, just enter one of your choice, and don't", 2, 2);
	c_put_str(TERM_SLATE, "forget name and password. Players may only own one account each at a time.", 3, 2);
	c_put_str(TERM_SLATE, "If you are new to TomeNET, this guide may prove useful:", 14, 2);
	prt("http://www.tomenet.net/guide.php", 15, 2);
#endif
#ifndef SIMPLE_LOGIN
	prt("Enter your account name above.", 21, 2);
#endif

	/* Ask until happy */
	while (1)
	{
		/* Go to the "name" area */
#ifndef SIMPLE_LOGIN
		move_cursor(2, 15);
#else
		move_cursor(5, 15);
#endif
		/* Save the player name */
		strcpy(tmp, nick);

		/* Get an input, ignore "Escape" */
		if (askfor_aux(tmp, 15, 0)) strcpy(nick, tmp);

		/* All done */
		break;
	}

	/* Pad the name (to clear junk) */
	sprintf(tmp, "%-15.15s", nick);

	/* Re-Draw the name (in light blue) */
#ifndef SIMPLE_LOGIN
	c_put_str(TERM_L_BLUE, tmp, 2, 15);
#else
	c_put_str(TERM_L_BLUE, tmp, 5, 15);
#endif

	/* Erase the prompt, etc */
	clear_from(20);
}


/*
 * Choose the character's name
 */
static void enter_password(void)
{
	size_t c;
	char tmp[23];

#ifndef SIMPLE_LOGIN
	/* Prompt and ask */
	prt("Enter your password above.", 21, 2);
#endif

	/* Default */
	strcpy(tmp, pass);

	/* Ask until happy */
	while (1)
	{
		/* Go to the "name" area */
#ifndef SIMPLE_LOGIN
		move_cursor(3, 15);
#else
		move_cursor(6, 15);
#endif

		/* Get an input, ignore "Escape" */
		if (askfor_aux(tmp, 15, ASKFOR_PRIVATE)) strcpy(pass, tmp);

		/* All done */
		break;
	}

	/* Pad the name (to clear junk)
	sprintf(tmp, "%-15.15s", pass); */

	 /* Re-Draw the password as 'x's (in light blue) */
	for (c = 0; c < strlen(pass); c++)
#ifndef SIMPLE_LOGIN
		Term_putch(15+c, 3, TERM_L_BLUE, 'x');
#else
		Term_putch(15+c, 6, TERM_L_BLUE, 'x');
#endif

	/* Erase the prompt, etc */
	clear_from(20);
}


/*
 * Choose the character's sex				-JWT-
 */
static bool choose_sex(void)
{
	char        c='\0';		/* pfft redesign while(1) */
	bool hazard = FALSE;
	bool parity = magik(50);

	put_str("m) Male", 21, parity ? 2 : 17);
	put_str("f) Female", 21, parity ? 17 : 2);
	put_str("      ", 4, 15);

	while (1)
	{
		c_put_str(TERM_SLATE, "Choose a sex (* for random, Q to Quit): ", 20, 2);
		if (!hazard) c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == 'm')
		{
			sex = 1;
			c_put_str(TERM_L_BLUE, "Male", 4, 15);
			break;
		}
		else if (c == 'f')
		{
			sex = 0;
			c_put_str(TERM_L_BLUE, "Female", 4, 15);
			break;
		}
		else if (c == '?')
		{
			/*do_cmd_help("help.hlp");*/
		}
		else if (c == '*')
		{
			switch (rand_int(2))
			{
				case 0:
					c = 'f';
					break;
				case 1:
					c = 'm';
					break;
			}
			hazard = TRUE;
		}
		else
		{
			bell();
		}
	}

	clear_from(19);
	return TRUE;
}


static void clear_diz(void) {
	int i;
//	c_put_str(TERM_UMBER, "                              ", DIZ_ROW, DIZ_COL);
	for (i = 0; i < 12; i++)
		c_put_str(TERM_L_UMBER, "                                                  ", DIZ_ROW + i, DIZ_COL);
}

static void display_race_diz(int r) {
	int i = 0;

	clear_diz();
	if (!race_diz[r][i]) return; /* server !newer_than 4.5.1.2 */

//	c_put_str(TERM_UMBER, format("--- %s ---", race_info[r].title), DIZ_ROW, DIZ_COL);
	while (i < 12 && race_diz[r][i][0]) {
		c_put_str(TERM_L_UMBER, race_diz[r][i], DIZ_ROW + i, DIZ_COL);
		i++;
	}
}

/*
 * Allows player to select a race			-JWT-
 */
static bool choose_race(void)
{
	player_race *rp_ptr;
	int i, j, l, m, n, sel = 0;
	char c = '\0';
	char out_val[160];
	bool hazard = FALSE;

	for (i = 18; i < 24; i++) Term_erase(1, i, 255);

race_redraw:
	l = 2;
	m = 22 - (Setup.max_race - 1) / 5;
	n = m - 1;

	for (j = 0; j < Setup.max_race; j++) {
		rp_ptr = &race_info[j];
		sprintf(out_val, "%c) %s", I2A(j), rp_ptr->title);

#ifdef CLASS_BEFORE_RACE
		if (!(rp_ptr->choice & BITS(class))) {
			c_put_str(TERM_L_DARK, out_val, m, l);
		} else
#endif
		{
			if (j == sel) c_put_str(TERM_YELLOW, out_val, m, l);
			else c_put_str(TERM_WHITE, out_val, m, l);
		}
		l += 15;
		if (l > 70) {
			l = 2;
			m++;
		}
	}
#ifndef CLASS_BEFORE_RACE
	c_put_str(TERM_L_BLUE, "                    ", 5, 15);
#else
	c_put_str(TERM_L_BLUE, "                    ", 6, 15);
#endif

	while (1) {
		c_put_str(TERM_SLATE, "Choose a race (* for random, Q to Quit, BACKSPACE to go back, 2/4/6/8): ", n, 2);
		display_race_diz(sel);

		if (!hazard) c = inkey();

		if (c == 'Q') quit(NULL);
		if (c == '\b') {
			clear_diz();
			clear_from(n);
			return FALSE;
		}

		/* Allow 'navigating', to highlight and display the descriptive text */
		if (c == '4' || c == 'j') {
			sel = (Setup.max_race + sel - 1) % Setup.max_race;
			while (!(race_info[sel].choice & BITS(class)))
			    sel = (Setup.max_race + sel - 1) % Setup.max_race;
			goto race_redraw;
		}
		if (c == '6' || c == 'k') {
			sel = (sel + 1) % Setup.max_race;
			while (!(race_info[sel].choice & BITS(class)))
			    sel = (sel + 1) % Setup.max_race;
			goto race_redraw;
		}
		if (c == '8' || c == 'h') {
			if (sel - 5 < 0) sel = sel + ((Setup.max_race - sel) / 5) * 5;
			else sel -= 5;
			while (!(race_info[sel].choice & BITS(class))) {
				if (sel - 5 < 0) sel = sel + ((Setup.max_race - sel) / 5) * 5;
				else sel -= 5;
			}
			goto race_redraw;
		}
		if (c == '2' || c == 'l') {
			if (sel + 5 >= Setup.max_race) sel = sel % 5;
			else sel += 5;
			while (!(race_info[sel].choice & BITS(class))) {
				if (sel + 5 >= Setup.max_race) sel = sel % 5;
				else sel += 5;
			}
			goto race_redraw;
		}
		if (c == '\r' || c == '\n') c = 'a' + sel;

		if (c == '*') hazard = TRUE;
		if (hazard) j = rand_int(Setup.max_race);
		else j = (islower(c) ? A2I(c) : -1);

		if ((j < Setup.max_race) && (j >= 0)) {
			rp_ptr = &race_info[j];
#ifdef CLASS_BEFORE_RACE
			if (!(rp_ptr->choice & BITS(class))) continue;
#endif

			race = j;
#ifndef CLASS_BEFORE_RACE
			c_put_str(TERM_L_BLUE, (char*)rp_ptr->title, 5, 15);
#else
			c_put_str(TERM_L_BLUE, (char*)rp_ptr->title, 6, 15);
#endif
			break;
		} else if (c == '?') {
			/*do_cmd_help("help.hlp");*/
		} else bell();
	}

	clear_diz();
	clear_from(n);
	return TRUE;
}


static void display_trait_diz(int r) {
	int i = 0;

	clear_diz();
	if (!trait_diz[r][i]) return; /* server !newer_than 4.5.1.2 */

//	c_put_str(TERM_UMBER, format("--- %s ---", trait_info[r].title), DIZ_ROW, DIZ_COL);
	while (i < 12 && trait_diz[r][i][0]) {
		c_put_str(TERM_L_UMBER, trait_diz[r][i], DIZ_ROW + i, DIZ_COL);
		i++;
	}
}

/*
 * Allows player to select a racial trait (introduced for Draconians) - C. Blue
 */
static bool choose_trait(void) {
	player_trait *tp_ptr;

	int i, j, l, m, n, sel = 0;
	char c = '\0';
	char out_val[160];
	bool hazard = FALSE;

	/* Prepare to list */
	l = 2;
	m = 20 - (Setup.max_trait - 1) / 5;
	n = m - 1;

#ifdef HIDE_UNAVAILABLE_TRAIT
	/* If server doesn't support traits, or we only have the
	   dummy 'N/A' trait available in general, skip trait choice */
	if (Setup.max_trait <= 1) return TRUE;

	/* Slaughter compiler warning */
	tp_ptr = &trait_info[0];

	/* If we have no traits available for the race chosen, skip trait choice */
	if (trait_info[0].choice & BITS(race)) return TRUE;

	/* If we have traits available for the race chose, prepare to display them */
 #ifdef CLASS_BEFORE_RACE
	put_str("Trait       :                               ", 7, 1);
 #else
	put_str("Trait       :                               ", 6, 1);
 #endif
#endif

	/* Outdated server? */
	if (Setup.max_trait == 0) {
		c_put_str(TERM_L_BLUE, trait_info[0].title, 7, 15);
		return TRUE;
	}

	/* No traits available for this race? Skip forward then.
	   Note: trait #0 is "N/A", which only traitless classes are supposed to 'have'. */
	if (trait_info[0].choice & BITS(race)) {
		c_put_str(TERM_L_BLUE, trait_info[0].title, 7, 15);
		return TRUE;
	}

	for (i = 18; i < 24; i++) Term_erase(1, i, 255);

trait_redraw:
	l = 2;
	m = 20 - (Setup.max_trait - 1) / 5;
	n = m - 1;

	/* Display the legal choices */
	i = 0;
	for (j = 0; j < Setup.max_trait; j++) {
                tp_ptr = &trait_info[j];
                if (!(tp_ptr->choice & BITS(race))) continue;

		sprintf(out_val, "%c) %s", I2A(i), tp_ptr->title);
		if (j == sel) c_put_str(TERM_YELLOW, out_val, m, l);
		else c_put_str(TERM_WHITE, out_val, m, l);
		i++;

		l += 25;
		if (l > 70) {
			l = 2;
			m++;
		}
	}

	/* Get a trait */
	while (1) {
		c_put_str(TERM_SLATE, "Choose a trait (* for random, Q to Quit, BACKSPACE to go back, 2/4/6/8):  ", n, 2);
		display_trait_diz(sel);

		if (!hazard) c = inkey();

		if (c == 'Q') quit(NULL);
		if (c == '\b') {
			clear_diz();
			clear_from(n);
			return FALSE;
		}

		/* Allow 'navigating', to highlight and display the descriptive text */
		if (c == '4' || c == 'j') {
			sel = (Setup.max_trait + sel - 1) % Setup.max_trait;
			while (!(trait_info[sel].choice & BITS(race)))
			    sel = (Setup.max_trait + sel - 1) % Setup.max_trait;
			goto trait_redraw;
		}
		if (c == '6' || c == 'k') {
			sel = (sel + 1) % Setup.max_trait;
			while (!(trait_info[sel].choice & BITS(race)))
			    sel = (sel + 1) % Setup.max_trait;
			goto trait_redraw;
		}
		if (c == '8' || c == 'h') {
			if (sel - 5 < 0) sel = sel + ((Setup.max_trait - sel) / 5) * 5;
			else sel -= 5;
			while (!(trait_info[sel].choice & BITS(race))) {
				if (sel - 5 < 0) sel = sel + ((Setup.max_trait - sel) / 5) * 5;
				else sel -= 5;
			}
			goto trait_redraw;
		}
		if (c == '2' || c == 'l') {
			if (sel + 5 >= Setup.max_trait) sel = sel % 5;
			else sel += 5;
			while (!(trait_info[sel].choice & BITS(race))) {
				if (sel + 5 >= Setup.max_trait) sel = sel % 5;
				else sel += 5;
			}
			goto trait_redraw;
		}
		if (c == '\r' || c == '\n') c = 'a' + sel;

		if (c == '*') hazard = TRUE;
		if (hazard) j = rand_int(Setup.max_trait);
		else j = (islower(c) ? A2I(c) : -1);

		/* Paranoia */
		if (j > Setup.max_trait) continue;

		/* Transform visible index back to real index */
		for (i = 0; i <= j; i++) {
			tp_ptr = &trait_info[i];
			if (!(tp_ptr->choice & BITS(race))) j++;
		}

		/* Verify if legal */
		if ((j < Setup.max_trait) && (j >= 0)) {
			if (!(tp_ptr->choice & BITS(race))) continue;

			trait = j;
			tp_ptr = &trait_info[j];
#ifndef CLASS_BEFORE_RACE
			c_put_str(TERM_L_BLUE, (char*)tp_ptr->title, 6, 15);
#else
			c_put_str(TERM_L_BLUE, (char*)tp_ptr->title, 7, 15);
#endif
			break;
		} else if (c == '?') {
			/*do_cmd_help("help.hlp");*/
		} else {
			bell();
		}
	}

	clear_diz();
	clear_from(n);
	return TRUE;
}


static void display_class_diz(int r) {
	int i = 0;

	clear_diz();
	if (!class_diz[r][i]) return; /* server !newer_than 4.5.1.2 */

//	c_put_str(TERM_UMBER, format("--- %s ---", class_info[r].title), DIZ_ROW, DIZ_COL);
	while (i < 12 && class_diz[r][i][0]) {
		c_put_str(TERM_L_UMBER, class_diz[r][i], DIZ_ROW + i, DIZ_COL);
		i++;
	}
}

/*
 * Gets a character class				-JWT-
 */
static bool choose_class(void)
{
	player_class *cp_ptr;
#ifndef CLASS_BEFORE_RACE
	player_race *rp_ptr = &race_info[race];
#endif
	int i, j, l, m, n, sel = 0;
	char c = '\0';
	char out_val[160];
	bool hazard = FALSE;


	/* Prepare to list */
	l = 2;
	m = 22 - (Setup.max_class - 1) / 5;
	n = m - 1;

	c_put_str(TERM_SLATE, "--- Important: For beginners, classes easy to play are... ---", n - 3, 2);
	c_put_str(TERM_YELLOW, "Important", n - 3, 6);
	c_put_str(TERM_SLATE, "Warrior, Rogue, Paladin, Druid, and possibly Mimic, Archer, Ranger.", n - 2, 2);

	for (i = 18; i < 24; i++) {
		Term_erase(1, i, 255);
	}

class_redraw:
	l = 2;
	m = 22 - (Setup.max_class - 1) / 5;
	n = m - 1;

	/* Display the legal choices */
	for (j = 0; j < Setup.max_class; j++) {
                cp_ptr = &class_info[j];
		sprintf(out_val, "%c) %s", I2A(j), cp_ptr->title);

#ifndef CLASS_BEFORE_RACE
		if (!(rp_ptr->choice & BITS(j))) {
			c_put_str(TERM_L_DARK, out_val, m, l);
		} else
#endif
		{
			if (j == sel) c_put_str(TERM_YELLOW, out_val, m, l);
			else c_put_str(TERM_WHITE, out_val, m, l);
		}

		l += 15;
		if (l > 70) {
			l = 2;
			m++;
		}
	}
#ifndef CLASS_BEFORE_RACE
	c_put_str(TERM_L_BLUE, "                    ", 7, 15);
#else
	c_put_str(TERM_L_BLUE, "                    ", 5, 15);
#endif

	/* Get a class */
	while (1) {
		c_put_str(TERM_SLATE, "Choose a class (* for random, Q to Quit, BACKSPACE to go back, 2/4/6/8):  ", n, 2);
		display_class_diz(sel);

		if (!hazard) c = inkey();

		if (c == 'Q') quit(NULL);
		if (c == '\b') {
			clear_diz();
			clear_from(n - 3);
			return FALSE;
		}

		/* Allow 'navigating', to highlight and display the descriptive text */
		if (c == '4' || c == 'j') {
			sel = (Setup.max_class + sel - 1) % Setup.max_class;
#ifndef CLASS_BEFORE_RACE
			while (!(race_info[race].choice & BITS(sel)))
			    sel = (Setup.max_class + sel - 1) % Setup.max_class;
#endif
			goto class_redraw;
		}
		if (c == '6' || c == 'k') {
			sel = (sel + 1) % Setup.max_class;
#ifndef CLASS_BEFORE_RACE
			while (!(race_info[race].choice & BITS(sel)))
			    sel = (sel + 1) % Setup.max_class;
#endif
			goto class_redraw;
		}
		if (c == '8' || c == 'h') {
			if (sel - 5 < 0) sel = sel + ((Setup.max_class - sel) / 5) * 5;
			else sel -= 5;
#ifndef CLASS_BEFORE_RACE
			while (!(race_info[race].choice & BITS(sel))) {
				if (sel - 5 < 0) sel = sel + ((Setup.max_class - sel) / 5) * 5;
				else sel -= 5;
			}
#endif
			goto class_redraw;
		}
		if (c == '2' || c == 'l') {
			if (sel + 5 >= Setup.max_class) sel = sel % 5;
			else sel += 5;
#ifndef CLASS_BEFORE_RACE
			while (!(race_info[race].choice & BITS(sel))) {
				if (sel + 5 >= Setup.max_class) sel = sel % 5;
				else sel += 5;
			}
#endif
			goto class_redraw;
		}
		if (c == '\r' || c == '\n') c = 'a' + sel;

		if (c == '*') hazard = TRUE;
		if (hazard) j = rand_int(Setup.max_class);
		else j = (islower(c) ? A2I(c) : -1);

		if ((j < Setup.max_class) && (j >= 0)) {
#ifndef CLASS_BEFORE_RACE
			if (!(rp_ptr->choice & BITS(j))) continue;
#endif

			class = j;
			cp_ptr = &class_info[j];
#ifndef CLASS_BEFORE_RACE
			c_put_str(TERM_L_BLUE, (char*)cp_ptr->title, 7, 15);
#else
			c_put_str(TERM_L_BLUE, (char*)cp_ptr->title, 5, 15);
#endif
			break;
		} else if (c == '?') {
			/*do_cmd_help("help.hlp");*/
		} else bell();
	}

	clear_diz();
	clear_from(n - 3); /* -3 so beginner-warnings are also cleared */
	return TRUE;
}


/*
 * Get the desired stat order.
 */
static bool choose_stat_order(void)
{
	int i, j, k, avail[6], crb, maxed_stats = 0;
	char c='\0';
	char out_val[160], stats[6][4], buf[8], buf2[8];
	bool hazard = FALSE;
	s16b stat_order_tmp[6];

        player_class *cp_ptr = &class_info[class];
        player_race *rp_ptr = &race_info[race];

	for (i = 0; i < 6; i++) stat_order_tmp[i] = stat_order[i];


        /* Character stats are randomly rolled (1 time): */
        if (char_creation_flags == 0) {

		put_str("Stat order  :", 11, 1);

		/* All stats are initially available */
		for (i = 0; i < 6; i++) {
			strncpy(stats[i], stat_names[i], 3);
			stats[i][3] = '\0';
			avail[i] = 1;
		}

		/* Find the ordering of all 6 stats */
		for (i = 0; i < 6; i++) {
			/* Clear bottom of screen */
			clear_from(20);

			/* Print available stats at bottom */
			for (k = 0; k < 6; k++) {
				/* Check for availability */
				if (avail[k]) {
					sprintf(out_val, "%c) %s", I2A(k), stats[k]);
					put_str(out_val, 21, k * 9);
				}
			}

			/* Hack -- it's obvious */
			/* if (i > 4) hazard = TRUE;
			It confused too many noobiez. Taking it out for now. */

			/* Get a stat */
			while (1) {
				put_str("Choose your stat order (* for random, Q to Quit): ", 20, 2);
				if (hazard) {
					j = rand_int(6);
				} else {
					c = inkey();
					if (c == 'Q') quit(NULL);
					if (c == '*') hazard = TRUE;

					j = (islower(c) ? A2I(c) : -1);
				}

				if ((j < 6) && (j >= 0) && (avail[j])) {
					stat_order[i] = j;
					c_put_str(TERM_L_BLUE, stats[j], 8, 15 + i * 5);
					avail[j] = 0;
					break;
				} else if (c == '?') {
					/*do_cmd_help("help.hlp");*/
				} else bell();
			}
		}

		clear_from(20);
	}

        /* player can define his stats completely manually: */
        else if (char_creation_flags == 1) {
		int col1 = 3, col2 = 35, col3 = 54, tmp_stat;

                j = 0; /* current stat to be modified */
                k = 30; /* free points left */

                clear_from(14);

                c_put_str(TERM_SLATE, "Distribute your attribute points (use them all!):", 13, col1);
                c_put_str(TERM_L_GREEN, format("%2d", k), 13, col3);
                c_put_str(TERM_SLATE, "                         Min. recommended,", 14, col2);
                c_put_str(TERM_SLATE, "Current:      (Base)     if possible:", 15, col2);

                put_str("Use keys '+', '-', 'RETURN'", 16, col1);
                put_str("or 8/2/4/6 or arrow keys to", 17, col1);
                put_str("modify and navigate.", 18, col1);
                put_str("Press ESC to proceed, after", 19, col1);
                put_str("you distributed all points.", 20, col1);
                put_str("'Q' = quit, BACKSPACE = back.", 21, col1);
                c_put_str(TERM_SLATE, "No more than 1 attribute out of the 6 is allowed to be maximised.", 23, col1);

		c_put_str(TERM_L_UMBER, "Strength -    ", 3, 30);
		c_put_str(TERM_YELLOW, "  How quickly you can strike.", 4, 30);
		c_put_str(TERM_YELLOW, "  How much you can carry and wield.", 5, 30);
		c_put_str(TERM_YELLOW, "  How much damage your strikes inflict.", 6, 30);
		c_put_str(TERM_YELLOW, "  How easily you can bash, throw and dig.", 7, 30);
		c_put_str(TERM_YELLOW, "  Slightly helps you swimming.", 8, 30);

                for (i = 0; i < 6; i++) {
                        stat_order[i] = 10;

			strncpy(stats[i], stat_names[i], 3);
			stats[i][3] = '\0';
		}

		while (1)
		{
			c_put_str(TERM_L_GREEN, format("%2d", k), 13, col3);

			for (i = 0; i < 6; i++) {
				crb = stat_order[i] + cp_ptr->c_adj[i] + rp_ptr->r_adj[i];
				if (crb > 18) crb = 18 + (crb - 18) * 10;
				cnv_stat(crb, buf);
				sprintf(buf2, "%2d", stat_order[i]);
                        	sprintf(out_val, "%s: %s    (%s)", stats[i], buf, buf2);

				tmp_stat = cp_ptr->min_recommend[i];
				if (tmp_stat >= 100) {
					/* indicate it's a main stat of this class */
					tmp_stat -= 100;
					c_put_str(TERM_GREEN, "(main)", 16 + i, col2 + 26 + 8);
				}
				if (tmp_stat) {
					if (tmp_stat > 18) tmp_stat = 18 + (tmp_stat - 18) * 10;
					cnv_stat(tmp_stat, buf);
					if (crb >= tmp_stat)
						c_put_str(TERM_L_GREEN, buf, 16 + i, col2 + 26);
					else
						c_put_str(TERM_GREEN, buf, 16 + i, col2 + 26);
				}

                        	if (j == i) {
                        		if (stat_order[i] == 10-2)
                        			c_put_str(TERM_L_RED, out_val, 16 + i, col2);
                        		else if (stat_order[i] == 17)
                        			c_put_str(TERM_L_BLUE, out_val, 16 + i, col2);
                        		else
                        			c_put_str(TERM_ORANGE, out_val, 16 + i, col2);
                        	} else {
                        		if (stat_order[i] == 10-2)
		                        	c_put_str(TERM_RED, out_val, 16 + i, col2);
                        		else if (stat_order[i] == 17)
                        			c_put_str(TERM_VIOLET, out_val, 16 + i, col2);
		                        else
		                        	c_put_str(TERM_L_UMBER, out_val, 16 + i, col2);
		                }
                	}

			c = inkey();
			crb = cp_ptr->c_adj[j] + rp_ptr->r_adj[j];
			if (c == '-' || c == '4' || c == 'h') {
				if (stat_order[j] > 10-2 &&
				    /* exception: allow going below 3 if we initially were below 3 too */
				    (stat_order[j] > 10 || stat_order[j]+crb > 3)) {
					if (stat_order[j] <= 12) {
						/* intermediate */
						stat_order[j]--;
						k++;
					} else if (stat_order[j] <= 14) {
						/* high */
						stat_order[j]--;
						k+=2;
					} else if (stat_order[j] <= 16) {
						/* nearly max */
						stat_order[j]--;
						k+=3;
					} else {
						/* max! */
						stat_order[j]--;
						k+=4;
						maxed_stats--;
					}
				}
			}
			if (c == '+' || c == '6' || c == 'l') {
				if (stat_order[j] < 17) {
					if (stat_order[j] < 12 && k >= 1) {
						/* intermediate */
						stat_order[j]++;
						k--;
					} else if (stat_order[j] < 14 && k >= 2) {
						/* high */
						stat_order[j]++;
						k-=2;
					} else if (stat_order[j] < 16 && k >= 3) {
						/* nearly max */
						stat_order[j]++;
						k-=3;
					} else if (k >= 4 && !maxed_stats) { /* only 1 maxed stat is allowed */
						/* max! */
						stat_order[j]++;
						k-=4;
						maxed_stats++;
					}
				}
			}
			if (c == '\r' || c == '2' || c == 'j') j = (j+1) % 6;
			if (c == '8' || c == 'k') j = (j+5) % 6;
			if (c == '\r' || c == '2' || c == '8' || c == 'j' || c == 'k') {
				switch (j) {
				case 0:	c_put_str(TERM_L_UMBER, "Strength -    ", 3, 30);
					c_put_str(TERM_YELLOW, "  How quickly you can strike.                  ", 4, 30);
					c_put_str(TERM_YELLOW, "  How much you can carry and wield.            ", 5, 30);
					c_put_str(TERM_YELLOW, "  How much damage your strikes inflict.        ", 6, 30);
					c_put_str(TERM_YELLOW, "  How easily you can bash, throw and dig.      ", 7, 30);
					c_put_str(TERM_YELLOW, "  Slightly helps you swimming.                 ", 8, 30);
					c_put_str(TERM_YELLOW, "                                               ", 9, 30);
					c_put_str(TERM_YELLOW, "                                               ", 10, 30);
					c_put_str(TERM_YELLOW, "                                               ", 11, 30);
					break;
				case 1:	c_put_str(TERM_L_UMBER, "Intelligence -", 3, 30);
					c_put_str(TERM_YELLOW, "  How well you can use magic                   ", 4, 30);
					c_put_str(TERM_YELLOW, "    (depending on your class and spells).      ", 5, 30);
					c_put_str(TERM_YELLOW, "  How well you can use magic devices.          ", 6, 30);
					c_put_str(TERM_YELLOW, "  Helps your disarming ability.                ", 7, 30);
					c_put_str(TERM_YELLOW, "  Helps noticing attempts to steal from you.   ", 8, 30);
					c_put_str(TERM_YELLOW, "                                               ", 9, 30);
					c_put_str(TERM_YELLOW, "                                               ", 10, 30);
					c_put_str(TERM_YELLOW, "                                               ", 11, 30);
					break;
				case 2:	c_put_str(TERM_L_UMBER, "Wisdom -      ", 3, 30);
					c_put_str(TERM_YELLOW, "  How well you can use prayers and magic       ", 4, 30);
					c_put_str(TERM_YELLOW, "    (depending on your class and spells).      ", 5, 30);
					c_put_str(TERM_YELLOW, "  How well can you resist malicious effects    ", 6, 30);
					c_put_str(TERM_YELLOW, "    and influences on both body and mind.      ", 7, 30);
					c_put_str(TERM_YELLOW, "                                               ", 8, 30);
					c_put_str(TERM_YELLOW, "                                               ", 9, 30);
					c_put_str(TERM_YELLOW, "                                               ", 10, 30);
					c_put_str(TERM_YELLOW, "                                               ", 11, 30);
					break;
				case 3:	c_put_str(TERM_L_UMBER, "Dexterity -   ", 3, 30);
					c_put_str(TERM_YELLOW, "  How quickly you can strike.                  ", 4, 30);
					c_put_str(TERM_YELLOW, "  Increases your chance to not miss.           ", 5, 30);
					c_put_str(TERM_YELLOW, "  Opponents will miss very slightly more often.", 6, 30);
					c_put_str(TERM_YELLOW, "  Helps your stealing skills (if any).         ", 7, 30);
					c_put_str(TERM_YELLOW, "  Helps to prevent foes stealing from you.     ", 8, 30);
					c_put_str(TERM_YELLOW, "  Helps keeping your balance after bashing.    ", 9, 30);
					c_put_str(TERM_YELLOW, "  Helps your disarming ability.                ", 10, 30);
					c_put_str(TERM_YELLOW, "  Slightly helps you swimming.                 ", 11, 30);
					break;
				case 4:	c_put_str(TERM_L_UMBER, "Constitution -", 3, 30);
					c_put_str(TERM_YELLOW, "  Determines your amout of HP                  ", 4, 30);
					c_put_str(TERM_YELLOW, "    (hit points, ie how much damage you can    ", 5, 30);
					c_put_str(TERM_YELLOW, "    take without dieing.                       ", 6, 30);
					c_put_str(TERM_YELLOW, "    Very high constitution might not show much ", 7, 30);
					c_put_str(TERM_YELLOW, "    effect until your character also reaches   ", 8, 30);
					c_put_str(TERM_YELLOW, "    an appropriate level.)                     ", 9, 30);
					c_put_str(TERM_YELLOW, "  Reduces the duration of poisonous effects.   ", 10, 30);
					c_put_str(TERM_YELLOW, "  Helps your character not to drown easily.    ", 11, 30);
					break;
				case 5:	c_put_str(TERM_L_UMBER, "Charisma -    ", 3, 30);
					c_put_str(TERM_YELLOW, "  Shops will offer you wares at better prices. ", 4, 30);
					c_put_str(TERM_YELLOW, "    (Note that shop keepers are also influenced", 5, 30);
					c_put_str(TERM_YELLOW, "    by your character's race.)                 ", 6, 30);
					c_put_str(TERM_YELLOW, "  Helps you to resist seducing attacks.        ", 7, 30);
					c_put_str(TERM_YELLOW, "                                               ", 8, 30);
					c_put_str(TERM_YELLOW, "                                               ", 9, 30);
					c_put_str(TERM_YELLOW, "                                               ", 10, 30);
					c_put_str(TERM_YELLOW, "                                               ", 11, 30);
					break;
				}
			}
			if (c == '\e') break;
			if (c == 'Q') quit(NULL);
			if (c == '\b') {
				for (i = 0; i < 6; i++) stat_order[i] = stat_order_tmp[i];

				for (i = 3; i < 12; i++) Term_erase(30, i, 255);
				clear_from(13);

				return FALSE;
			}
		}

		for (i = 3; i < 12; i++) Term_erase(30, i, 255);
		clear_from(13);
        }
        return TRUE;
}

/* Quick hack!		- Jir -
 * TODO: remove hard-coded things. */
static bool choose_mode(void)
{
	char c = '\0';
	bool hazard = FALSE;

	put_str("n) Normal", 16, 2);
	c_put_str(TERM_SLATE, "(3 lifes)", 16, 12);
	put_str("g) No Ghost", 17, 2);
	c_put_str(TERM_SLATE, "('Unworldly' - One life only. The traditional rogue-like way)", 17, 14);
	put_str("e) Everlasting", 18, 2);
	c_put_str(TERM_SLATE, "(You may resurrect infinite times, but cannot enter highscore)", 18, 17);
#if 0
	put_str("h) Hard", 19, 2);
	c_put_str(TERM_SLATE, "('Purgatorial' - like normal, with nasty additional penalties)", 19, 10);
#endif
	put_str("H) Hellish", 20 - 1, 2);
	c_put_str(TERM_SLATE, "(Combination of Hard + No Ghost)", 20 - 1, 13);
	put_str("p) PvP", 21 - 1, 2);
	c_put_str(TERM_SLATE, "(Can't beat the game, instead special 'player vs player' rules apply)", 21 - 1, 9);

	c_put_str(TERM_L_BLUE, "                    ", 9, 15);

	while (1) {
		c_put_str(TERM_SLATE, "Choose a mode (* for random, Q to Quit, BACKSPACE to go back): ", 15, 2);
		if (!hazard) c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == '\b') {
			clear_from(15);
			return FALSE;
		}

		if (c == 'p') {
			sex += MODE_PVP;
			c_put_str(TERM_L_BLUE, "PvP", 9, 15);
			break;
		} else if (c == 'n') {
			c_put_str(TERM_L_BLUE, "Normal", 9, 15);
			break;
		} else if (c == 'g') {
			sex += MODE_NO_GHOST;
			c_put_str(TERM_L_BLUE, "No Ghost", 9, 15);
			break;
		}
#if 0
		else if (c == 'h') {
			sex += (MODE_HARD);
			c_put_str(TERM_L_BLUE, "Hard", 9, 15);
			break;
		}
#endif
		else if (c == 'H') {
			sex += (MODE_NO_GHOST + MODE_HARD);
			c_put_str(TERM_L_BLUE, "Hellish", 9, 15);
			break;
		} else if (c == 'e') {
			sex += MODE_EVERLASTING;
			c_put_str(TERM_L_BLUE, "Everlasting", 9, 15);
			break;
		} else if (c == '?') {
			/*do_cmd_help("help.hlp");*/
		} else if (c == '*') {
			switch (rand_int(5 - 1)) {
				case 0:
					c = 'n';
					break;
				case 1:
					c = 'p';
					break;
				case 2:
					c = 'g';
					break;
#if 0
				case 3:
					c = 'h';
					break;
#endif
				case 4 - 1:
					c = 'H';
					break;
				case 5 - 1:
					c = 'e';
					break;
			}
			hazard = TRUE;
		} else bell();
	}

	clear_from(15);
	return TRUE;
}

/* Fruit bat is now a "body modification" that can be applied to all "modes" - C. Blue */
static bool choose_body_modification(void)
{
	char c = '\0';
	bool hazard = FALSE;

	put_str("n) Normal body", 20, 2);
	put_str("f) Fruit bat", 21, 2);
	c_put_str(TERM_SLATE, "(Bats are faster and vampiric, but can't wear certain items)", 21, 15);

	c_put_str(TERM_L_BLUE, "                    ", 8, 15);

	while (1) {
		c_put_str(TERM_SLATE, "Choose a body modification (* for random, Q to Quit, BACKSPACE to go back): ", 19, 2);
		if (!hazard) c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == '\b') {
			clear_from(19);
			return FALSE;
		}

		if (c == 'f') {
			sex += MODE_FRUIT_BAT;
			c_put_str(TERM_L_BLUE, "Fruit bat", 8, 15);
			break;
		} else if (c == 'n') {
			c_put_str(TERM_L_BLUE, "Normal body", 8, 15);
			break;
		} else if (c == '?') {
			/*do_cmd_help("help.hlp");*/
		} else if (c == '*') {
			switch (rand_int(2)) {
				case 0:
					c = 'n';
					break;
				case 1:
					c = 'f';
					break;
			}
			hazard = TRUE;
		} else bell();
	}

	clear_from(19);
	return TRUE;
}

/*
 * Get the name/pass for this character.
 */
void get_char_name(void)
{
	/* Clear screen */
	Term_clear();

	/* Title everything */
#ifndef SIMPLE_LOGIN
	put_str("Name        :", 2, 1);
	put_str("Password    :", 3, 1);
	c_put_str(TERM_SLATE, "If TomeNET quits with a login failure, check account name and password", 4, 3);
	c_put_str(TERM_SLATE, "for upper/lower case! If you are a new player, try a different name.", 5, 3);
#else
	put_str("Name        :", 5, 1);
	put_str("Password    :", 6, 1);
	c_put_str(TERM_SLATE, "If TomeNET quits with a login failure and you are a new player, it means", 8, 3);
	c_put_str(TERM_SLATE, "the account name you picked is already in use. Try a different name!", 9, 3);
	c_put_str(TERM_SLATE, "If you're not a new player, check name and password for upper/lower case!", 10, 3);
#endif
	/* Dump the default name */
#ifndef SIMPLE_LOGIN
	c_put_str(TERM_L_BLUE, nick, 2, 15);
#else
	c_put_str(TERM_L_BLUE, nick, 5, 15);
#endif


	/* Display some helpful information XXX XXX XXX */

	/* Choose a name */
	choose_name();

	/* Enter password */
	enter_password();

	/* Message */
	put_str("Connecting to server....", 21, 1);

	/* Make sure the message is shown */
	Term_fresh();

	/* Note player birth in the message recall */
	c_message_add(" ");
	c_message_add("  ");
	c_message_add("====================");
	c_message_add("  ");
	c_message_add(" ");
}

/*
 * Get the other info for this character.
 */
void get_char_info(void)
{
	int i, j;
	char out_val[160];
	bool ded = sex & (MODE_DED_PVP | MODE_DED_IDDC);
	sex &= ~(MODE_DED_PVP | MODE_DED_IDDC);

	/* Load tables from LUA into memory --
	   could just request it from LUA on demand instead. - C. Blue */
	memset(race_diz, 0, sizeof(char) * MAX_RACE * 12 * 61);
	memset(class_diz, 0, sizeof(char) * MAX_CLASS * 12 * 61);
	memset(trait_diz, 0, sizeof(char) * MAX_TRAIT * 12 * 61);
	if (is_newer_than(&server_version, 4, 5, 1, 1, 0, 0)) {
		for (j = 0; j < 12; j++) {
			for (i = 0; i < MAX_RACE; i++) {
				if (!race_info[i].title) continue;
				sprintf(out_val, "return get_race_diz(\"%s\", %d)", race_info[i].title, j);
				strcpy(race_diz[i][j], string_exec_lua(0, out_val));
			}
			for (i = 0; i < MAX_CLASS; i++) {
				if (!class_info[i].title) continue;
				sprintf(out_val, "return get_class_diz(\"%s\", %d)", class_info[i].title, j);
				strcpy(class_diz[i][j], string_exec_lua(0, out_val));
			}
			for (i = 0; i < MAX_TRAIT; i++) {
				if (!trait_info[i].title) continue;
				sprintf(out_val, "return get_trait_diz(\"%s\", %d)", trait_info[i].title, j);
				strcpy(trait_diz[i][j], string_exec_lua(0, out_val));
			}
		}
	}

	/* Title everything */
	put_str("Sex         :", 4, 1);
#ifndef CLASS_BEFORE_RACE
	put_str("Race        :", 5, 1);
 #ifndef HIDE_UNAVAILABLE_TRAIT
	/* If server doesn't support traits, or we only have the dummy
	   'N/A' trait available, we don't want to display traits at all */
	put_str("Trait       :", 6, 1);
 #endif
	put_str("Class       :", 7, 1);
	put_str("Body        :", 8, 1);
#else
	put_str("Class       :", 5, 1);
	put_str("Race        :", 6, 1);
 #ifndef HIDE_UNAVAILABLE_TRAIT
	/* If server doesn't support traits, or we only have the dummy
	   'N/A' trait available, we don't want to display traits at all */
	put_str("Trait       :", 7, 1);
 #endif
	put_str("Body        :", 8, 1);
#endif
	put_str("Mode        :", 9, 1);

	/* Clear bottom of screen */
	clear_from(20);

	/* Display some helpful information XXX XXX XXX */

csex:
	/* Choose a sex */
	choose_sex();

#if 0
	/* Clean up the selections */
	Term_erase(15, 5, 255);
	Term_erase(15, 6, 255);
	Term_erase(15, 7, 255);
#endif

#ifndef CLASS_BEFORE_RACE
crace:
	/* Choose a race */
	if (!choose_race()) goto csex;
	/* Choose a trait */
	if (!choose_trait()) goto crace;
cbody:
	/* Choose character's body modification */
	if (!choose_body_modification()) goto crace;
cclass:
	/* Choose a class */
	if (!choose_class()) goto cbody;


cstats:
	class_extra = 0;

	/* Choose stat order */
	if (!choose_stat_order()) goto cclass;
#else
cclass:
	/* Choose a class */
	if (!choose_class()) goto csex;
crace:
	/* Choose a race */
	if (!choose_race()) goto cclass;
	/* Choose a trait */
	if (!choose_trait()) goto crace;
cbody:
	/* Choose character's body modification */
	if (!choose_body_modification()) goto crace;


cstats:
	class_extra = 0;

	/* Choose stat order */
	if (!choose_stat_order()) goto cbody;
#endif


	/* Choose character mode */
	if (!s_RPG || s_RPG_ADMIN) {
		if (!choose_mode()) goto cstats;
	}
	else c_put_str(TERM_L_BLUE, "No Ghost", 9, 15);

	/* Clear */
	clear_from(15);

	/* Message */
	put_str("Entering game...  [Hit any key]", 21, 1);

	/* Wait for key */
	inkey();

	/* Clear */
	clear_from(20);

	/* Hack: Apply slot-exclusive mode on user demand */
	if (ded) {
		if (sex & MODE_PVP) sex |= MODE_DED_PVP;
		else sex |= MODE_DED_IDDC;
	}
}

static bool enter_server_name(void)
{
	/* Clear screen */
	Term_clear();

	/* Message */
	prt("Enter the server name you want to connect to (ESCAPE to quit): ", 3, 1);

	/* Move cursor */
	move_cursor(5, 1);

	/* Default */
        strcpy(server_name, "europe.tomenet.net");

	/* Ask for server name */
	return askfor_aux(server_name, 80, 0);
}

/*
 * Have the player choose a server from the list given by the
 * metaserver.
 */
/* TODO:
 * - wrap the words using strtok
 * - allow to scroll the screen in case
 * */

bool get_server_name(void)
{
        s32b i;
        cptr tmp;

#ifdef EXPERIMENTAL_META
	int j, bytes, socket = -1;
	char buf[80192], c;
#else
	int j, k, l, bytes, socket = -1, offsets[20], lines = 0;
	char buf[80192], *ptr, c, out_val[260];
#endif

        /* We NEED lua here, so if it aint initialized yet, do it */
        init_lua();

	/* Message */
	prt("Connecting to metaserver for server list....", 1, 1);

	/* Make sure message is shown */
	Term_fresh();

	if (strlen(meta_address) > 0) {
		/* Metaserver in config file */
		socket = CreateClientSocket(meta_address, 8801);
	}

	/* Failed to connect to metaserver in config file, connect to hard-coded address */
	if (socket == -1) {
		prt("Failed to connect to used-specified meta server.", 2, 1);
		prt("Trying to connect to default metaserver instead....", 3, 1);

	        /* Connect to metaserver */
		socket = CreateClientSocket(META_ADDRESS, 8801);
	}

#ifdef META_ADDRESS_2
	/* Check for failure */
	if (socket == -1)
	{
		prt("Failed to connect to meta server.", 2, 1);
		prt("Trying to connect to default alternate metaserver instead....", 3, 1);
		/* Hack -- Connect to metaserver #2 */
		socket = CreateClientSocket(META_ADDRESS_2, 8801);
	}
#endif

	/* Check for failure */
	if (socket == -1)
	{
		prt("Failed to connect to meta server.", 2, 1);
		return enter_server_name();
	}

	/* Wipe the buffer so valgrind doesn't complain - mikaelh */
	C_WIPE(buf, 80192, char);

	/* Read */
	bytes = SocketRead(socket, buf, 80192);

	/* Close the socket */
	SocketClose(socket);

	/* Check for error while reading */
	if (bytes <= 0)
	{
		return enter_server_name();
	}

        Term_clear();

#ifdef EXPERIMENTAL_META
        call_lua(0, "meta_display", "(s)", "d", buf, &i);
#else

	/* Start at the beginning */
	ptr = buf;
	i = 0;

	/* Print each server */
	while (ptr - buf < bytes)
	{
		/* Check for no entry */
		if (strlen(ptr) <= 1)
		{
			/* Increment */
			ptr++;

			/* Next */
			continue;
		}

		/* Save offset */
		offsets[i] = ptr - buf;

		/* Format entry */
		sprintf(out_val, "%c) %s", I2A(i), ptr);

		j = strlen(out_val);

		/* Strip off offending characters */
		out_val[j - 1] = '\0';
		out_val[j]='\0';

		k=0;
		while(j){
			l=strlen(&out_val[k]);
			if(j > 75){
				l=75;
				while(out_val[k+l]!=' ') l--;
				out_val[l]='\0';
			}
			prt(out_val+k, lines++, (k ? 4 : 1));
			k+=(l+1);
			j=strlen(&out_val[k]);
		}

		/* Go to next metaserver entry */
		ptr += strlen(ptr) + 1;

		/* One more entry */
		i++;

		/* We can't handle more than 20 entries -- BAD */
		if (lines > 20) break;
	}

	/* Message */
	prt(longVersion , lines + 1, 1);

	/* Prompt */
	prt("-- Choose a server to connect to (Q for manual entry): ", lines + 2, 1);
#endif

	/* Ask until happy */
	while (1)
	{
		/* Get a key */
		c = inkey();

		/* Check for quit */
		if (c == 'Q')
		{
			return enter_server_name();
		}
		else if (c == ESCAPE)
		{
			quit(NULL);
			return FALSE;
		}

		/* Index */
		j = (islower(c) ? A2I(c) : -1);

		/* Check for legality */
		if (j >= 0 && j < i)
			break;
	}

#ifdef EXPERIMENTAL_META
        call_lua(0, "meta_get", "(s,d)", "sd", buf, j, &tmp, &server_port);
        strcpy(server_name, tmp);
#else
	/* Extract server name */
        sscanf(buf + offsets[j], "%s", server_name);
#endif

	/* Success */
	return TRUE;
}

/* Human */
static char *human_syllable1[] =
{
	"Ab", "Ac", "Ad", "Af", "Agr", "Ast", "As", "Al", "Adw", "Adr", "Ar",
	"B", "Br", "C", "Cr", "Ch", "Cad", "D", "Dr", "Dw", "Ed", "Eth", "Et",
	"Er", "El", "Eow", "F", "Fr", "G", "Gr", "Gw", "Gal", "Gl", "H", "Ha",
	"Ib", "Jer", "K", "Ka", "Ked", "L", "Loth", "Lar", "Leg", "M", "Mir",
	"N", "Nyd", "Ol", "Oc", "On", "P", "Pr", "R", "Rh", "S", "Sev", "T",
	"Tr", "Th", "V", "Y", "Z", "W", "Wic",
};

static char *human_syllable2[] =
{
	"a", "ae", "au", "ao", "are", "ale", "ali", "ay", "ardo", "e", "ei",
	"ea", "eri", "era", "ela", "eli", "enda", "erra", "i", "ia", "ie",
	"ire", "ira", "ila", "ili", "ira", "igo", "o", "oa", "oi", "oe",
	"ore", "u", "y",
};

static char *human_syllable3[] =
{
	"a", "and", "b", "bwyn", "baen", "bard", "c", "ctred", "cred", "ch",
	"can", "d", "dan", "don", "der", "dric", "dfrid", "dus", "f", "g",
	"gord", "gan", "l", "li", "lgrin", "lin", "lith", "lath", "loth",
	"ld", "ldric", "ldan", "m", "mas", "mos", "mar", "mond", "n",
	"nydd", "nidd", "nnon", "nwan", "nyth", "nad", "nn", "nnor", "nd",
	"p", "r", "ron", "rd", "s", "sh", "seth", "sean", "t", "th", "tha",
	"tlan", "trem", "tram", "v", "vudd", "w", "wan", "win", "wyn", "wyr",
	"wyr", "wyth",
};

/*
 * Random Name Generator
 * based on a Javascript by Michael Hensley
 * "http://geocities.com/timessquare/castle/6274/"
 */
void create_random_name(int race, char *name)
{
	char *syl1, *syl2, *syl3;

	int idx;

	/* Paranoia */
	if (!name) return;

	idx = rand_int(sizeof(human_syllable1) / sizeof(char *));
	syl1 = human_syllable1[idx];
	idx = rand_int(sizeof(human_syllable2) / sizeof(char *));
	syl2 = human_syllable2[idx];
	idx = rand_int(sizeof(human_syllable3) / sizeof(char *));
	syl3 = human_syllable3[idx];

	/* Concatenate selected syllables */
	strnfmt(name, 32, "%s%s%s", syl1, syl2, syl3);
}

