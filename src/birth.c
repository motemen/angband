/* File: birth.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/*
 * 2.7.9v3-v6 ���{��Ő���: ���Ƃ���
 * 2.8.0      �Ή�        : sayu, ���Ƃ���
 * 2.8.1      �Ή�        : FIRST, ���Ƃ���
 * 2.8.3      �Ή�        : FIRST, ���Ƃ���
 * 2.9.0      �Ή�        : �퐣
 */

#include "angband.h"

#include "script.h"


/*
 * Forward declare
 */
typedef struct birther birther;

/*
 * A structure to hold "rolled" information
 */
struct birther
{
	s16b age;
	s16b wt;
	s16b ht;
	s16b sc;

	s32b au;

	s16b stat[A_MAX];

	char history[250];
};



/*
 * The last character displayed
 */
static birther prev;


/*
 * Current stats (when rolling a character).
 */
static s16b stat_use[A_MAX];



/*
 * Save the currently rolled data for later.
 */
static void save_prev_data(void)
{
	int i;


	/*** Save the current data ***/

	/* Save the data */
	prev.age = p_ptr->age;
	prev.wt = p_ptr->wt;
	prev.ht = p_ptr->ht;
	prev.sc = p_ptr->sc;
	prev.au = p_ptr->au;

	/* Save the stats */
	for (i = 0; i < A_MAX; i++)
	{
		prev.stat[i] = p_ptr->stat_max[i];
	}

	/* Save the history */
	my_strcpy(prev.history, p_ptr->history, sizeof(prev.history));
}


/*
 * Load the previously rolled data.
 */
static void load_prev_data(void)
{
	int i;

	birther temp;


	/*** Save the current data ***/

	/* Save the data */
	temp.age = p_ptr->age;
	temp.wt = p_ptr->wt;
	temp.ht = p_ptr->ht;
	temp.sc = p_ptr->sc;
	temp.au = p_ptr->au;

	/* Save the stats */
	for (i = 0; i < A_MAX; i++)
	{
		temp.stat[i] = p_ptr->stat_max[i];
	}

	/* Save the history */
	my_strcpy(temp.history, p_ptr->history, sizeof(temp.history));


	/*** Load the previous data ***/

	/* Load the data */
	p_ptr->age = prev.age;
	p_ptr->wt = prev.wt;
	p_ptr->ht = prev.ht;
	p_ptr->sc = prev.sc;
	p_ptr->au = prev.au;

	/* Load the stats */
	for (i = 0; i < A_MAX; i++)
	{
		p_ptr->stat_max[i] = prev.stat[i];
		p_ptr->stat_cur[i] = prev.stat[i];
	}

	/* Load the history */
	my_strcpy(p_ptr->history, prev.history, sizeof(p_ptr->history));


	/*** Save the current data ***/

	/* Save the data */
	prev.age = temp.age;
	prev.wt = temp.wt;
	prev.ht = temp.ht;
	prev.sc = temp.sc;
	prev.au = temp.au;

	/* Save the stats */
	for (i = 0; i < A_MAX; i++)
	{
		prev.stat[i] = temp.stat[i];
	}

	/* Save the history */
	my_strcpy(prev.history, temp.history, sizeof(prev.history));
}




/*
 * Adjust a stat by an amount.
 *
 * This just uses "modify_stat_value()" unless "maximize" mode is false,
 * and a positive bonus is being applied, in which case, a special hack
 * is used, with the "auto_roll" flag affecting the result.
 *
 * The "auto_roll" flag selects "maximal" changes for use with the
 * auto-roller initialization code.  Otherwise, if "maximize" mode
 * is being used, the changes are fixed.  Otherwise, semi-random
 * changes will occur, with larger changes at lower values.
 */
static int adjust_stat(int value, int amount, int auto_roll)
{
	/* Negative amounts or maximize mode */
	if ((amount < 0) || adult_maximize)
	{
		return (modify_stat_value(value, amount));
	}

	/* Special hack */
	else
	{
		int i;

		/* Apply reward */
		for (i = 0; i < amount; i++)
		{
			if (value < 18)
			{
				value++;
			}
			else if (value < 18+70)
			{
				value += ((auto_roll ? 15 : randint(15)) + 5);
			}
			else if (value < 18+90)
			{
				value += ((auto_roll ? 6 : randint(6)) + 2);
			}
			else if (value < 18+100)
			{
				value++;
			}
		}
	}

	/* Return the result */
	return (value);
}




/*
 * Roll for a characters stats
 *
 * For efficiency, we include a chunk of "calc_bonuses()".
 */
static void get_stats(void)
{
	int i, j;

	int bonus;

	int dice[18];


	/* Roll and verify some stats */
	while (TRUE)
	{
		/* Roll some dice */
		for (j = i = 0; i < 18; i++)
		{
			/* Roll the dice */
			dice[i] = randint(3 + i % 3);

			/* Collect the maximum */
			j += dice[i];
		}

		/* Verify totals */
		if ((j > 42) && (j < 54)) break;
	}

	/* Roll the stats */
	for (i = 0; i < A_MAX; i++)
	{
		/* Extract 5 + 1d3 + 1d4 + 1d5 */
		j = 5 + dice[3*i] + dice[3*i+1] + dice[3*i+2];

		/* Save that value */
		p_ptr->stat_max[i] = j;

		/* Obtain a "bonus" for "race" and "class" */
		bonus = rp_ptr->r_adj[i] + cp_ptr->c_adj[i];

		/* Variable stat maxes */
		if (adult_maximize)
		{
			/* Start fully healed */
			p_ptr->stat_cur[i] = p_ptr->stat_max[i];

			/* Efficiency -- Apply the racial/class bonuses */
			stat_use[i] = modify_stat_value(p_ptr->stat_max[i], bonus);
		}

		/* Fixed stat maxes */
		else
		{
			/* Apply the bonus to the stat (somewhat randomly) */
			stat_use[i] = adjust_stat(p_ptr->stat_max[i], bonus, FALSE);

			/* Save the resulting stat maximum */
			p_ptr->stat_cur[i] = p_ptr->stat_max[i] = stat_use[i];
		}
	}
}


/*
 * Roll for some info that the auto-roller ignores
 */
static void get_extra(void)
{
	int i, j, min_value, max_value;


	/* Level one */
	p_ptr->max_lev = p_ptr->lev = 1;

	/* Experience factor */
	p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;

	/* Hitdice */
	p_ptr->hitdie = rp_ptr->r_mhp + cp_ptr->c_mhp;

	/* Initial hitpoints */
	p_ptr->mhp = p_ptr->hitdie;

	/* Minimum hitpoints at highest level */
	min_value = (PY_MAX_LEVEL * (p_ptr->hitdie - 1) * 3) / 8;
	min_value += PY_MAX_LEVEL;

	/* Maximum hitpoints at highest level */
	max_value = (PY_MAX_LEVEL * (p_ptr->hitdie - 1) * 5) / 8;
	max_value += PY_MAX_LEVEL;

	/* Pre-calculate level 1 hitdice */
	p_ptr->player_hp[0] = p_ptr->hitdie;

	/* Roll out the hitpoints */
	while (TRUE)
	{
		/* Roll the hitpoint values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			j = randint(p_ptr->hitdie);
			p_ptr->player_hp[i] = p_ptr->player_hp[i-1] + j;
		}

		/* XXX Could also require acceptable "mid-level" hitpoints */

		/* Require "valid" hitpoints at highest level */
		if (p_ptr->player_hp[PY_MAX_LEVEL-1] < min_value) continue;
		if (p_ptr->player_hp[PY_MAX_LEVEL-1] > max_value) continue;

		/* Acceptable */
		break;
	}
}


/*
 * Get the racial history, and social class, using the "history charts".
 */
static void get_history(void)
{
	int i, chart, roll, social_class;


	/* Clear the previous history strings */
	p_ptr->history[0] = '\0';


	/* Initial social class */
	social_class = randint(4);

	/* Starting place */
	chart = rp_ptr->hist;


	/* Process the history */
	while (chart)
	{
		/* Start over */
		i = 0;

		/* Roll for nobility */
		roll = randint(100);

		/* Get the proper entry in the table */
		while ((chart != h_info[i].chart) || (roll > h_info[i].roll)) i++;

		/* Get the textual history */
		my_strcat(p_ptr->history, (h_text + h_info[i].text), sizeof(p_ptr->history));

		/* Add in the social class */
		social_class += (int)(h_info[i].bonus) - 50;

		/* Enter the next chart */
		chart = h_info[i].next;
	}



	/* Verify social class */
	if (social_class > 100) social_class = 100;
	else if (social_class < 1) social_class = 1;

	/* Save the social class */
	p_ptr->sc = social_class;
}


/*
 * Computes character's age, height, and weight
 */
static void get_ahw(void)
{
	/* Calculate the age */
	p_ptr->age = rp_ptr->b_age + randint(rp_ptr->m_age);

	/* Calculate the height/weight for males */
	if (p_ptr->psex == SEX_MALE)
	{
		p_ptr->ht = Rand_normal(rp_ptr->m_b_ht, rp_ptr->m_m_ht);
		p_ptr->wt = Rand_normal(rp_ptr->m_b_wt, rp_ptr->m_m_wt);
	}

	/* Calculate the height/weight for females */
	else if (p_ptr->psex == SEX_FEMALE)
	{
		p_ptr->ht = Rand_normal(rp_ptr->f_b_ht, rp_ptr->f_m_ht);
		p_ptr->wt = Rand_normal(rp_ptr->f_b_wt, rp_ptr->f_m_wt);
	}
}




/*
 * Get the player's starting money
 */
static void get_money(void)
{
	int i;

	int gold;

	/* Social Class determines starting gold */
	gold = (p_ptr->sc * 6) + randint(100) + 300;

	/* Process the stats */
	for (i = 0; i < A_MAX; i++)
	{
		/* Mega-Hack -- reduce gold for high stats */
		if (stat_use[i] >= 18+50) gold -= 300;
		else if (stat_use[i] >= 18+20) gold -= 200;
		else if (stat_use[i] > 18) gold -= 150;
		else gold -= (stat_use[i] - 8) * 10;
	}

	/* Minimum 100 gold */
	if (gold < 100) gold = 100;

	/* Save the gold */
	p_ptr->au = gold;
}



/*
 * Clear all the global "character" data
 */
static void player_wipe(void)
{
	int i;

	/* Backup the player choices */
	byte psex = p_ptr->psex;
	byte prace = p_ptr->prace;
	byte pclass = p_ptr->pclass;


	/* Wipe the player */
	(void)WIPE(p_ptr, player_type);

	/* Restore the choices */
	p_ptr->psex = psex;
	p_ptr->prace = prace;
	p_ptr->pclass = pclass;

	/* Clear the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&inventory[i]);
	}


	/* Start with no artifacts made yet */
	for (i = 0; i < z_info->a_max; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		a_ptr->cur_num = 0;
	}


	/* Start with no quests */
	for (i = 0; i < MAX_Q_IDX; i++)
	{
		q_list[i].level = 0;
	}

	/* Add a special quest */
	q_list[0].level = 99;

	/* Add a second quest */
	q_list[1].level = 100;


	/* Reset the "objects" */
	for (i = 1; i < z_info->k_max; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Reset "tried" */
		k_ptr->tried = FALSE;

		/* Reset "aware" */
		k_ptr->aware = FALSE;
	}


	/* Reset the "monsters" */
	for (i = 1; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];
		monster_lore *l_ptr = &l_list[i];

		/* Hack -- Reset the counter */
		r_ptr->cur_num = 0;

		/* Hack -- Reset the max counter */
		r_ptr->max_num = 100;

		/* Hack -- Reset the max counter */
		if (r_ptr->flags1 & (RF1_UNIQUE)) r_ptr->max_num = 1;

		/* Clear player kills */
		l_ptr->pkills = 0;
	}


	/* Hack -- no ghosts */
	r_info[z_info->r_max-1].max_num = 0;


	/* Hack -- Well fed player */
	p_ptr->food = PY_FOOD_FULL - 1;


	/* None of the spells have been learned yet */
	for (i = 0; i < PY_MAX_SPELLS; i++) p_ptr->spell_order[i] = 99;
}



/*
 * Init players with some belongings
 *
 * Having an item identifies it and makes the player "aware" of its purpose.
 */
static void player_outfit(void)
{
	int i;
	const start_item *e_ptr;
	object_type *i_ptr;
	object_type object_type_body;


	/* Hack -- Give the player his equipment */
	for (i = 0; i < MAX_START_ITEMS; i++)
	{
		/* Access the item */
		e_ptr = &(cp_ptr->start_items[i]);

		/* Get local object */
		i_ptr = &object_type_body;

		/* Hack	-- Give the player an object */
		if (e_ptr->tval > 0)
		{
			/* Get the object_kind */
			int k_idx = lookup_kind(e_ptr->tval, e_ptr->sval);

			/* Valid item? */
			if (!k_idx) continue;

			/* Prepare the item */
			object_prep(i_ptr, k_idx);
			i_ptr->number = (byte)rand_range(e_ptr->min, e_ptr->max);

			object_aware(i_ptr);
			object_known(i_ptr);
			(void)inven_carry(i_ptr);
		}
	}
}


/* "Failure" codes */
#define BIRTH_SUCCESS             0
#define BIRTH_RESTART            -1
#define BIRTH_BACK               -2


/* Locations of the tables on the screen */
#define HEADER_ROW       1
#define QUESTION_ROW     7
#define TABLE_ROW       10

#define QUESTION_COL     2
#define SEX_COL          2
#define RACE_COL        14
#define RACE_AUX_COL    29
#define CLASS_COL       29
#define CLASS_AUX_COL   50


typedef struct birth_menu birth_menu;


/*
 * A structure to hold the menus
 */
struct birth_menu
{
	bool grayed;
#ifdef JP
	cptr J_name;
#endif /* JP */
	cptr name;
};


/*
 * Clear the previous question
 */
static void clear_question(void)
{
	int i;

	for (i = QUESTION_ROW; i < TABLE_ROW; i++)
	{
		/* Clear line, position cursor */
		Term_erase(0, i, 255);
	}
}


/*
 * Generic "get choice from menu" function
 */
static int get_player_choice(birth_menu *choices, int num, int def,
                             int col, int wid, byte *select,
                             cptr helpfile, void (*hook)(birth_menu))
{
	int top = 0, cur = def;
	int i, dir;
	char c;
	char buf[80];
	bool done = FALSE;
	int hgt;
	byte attr;

	/* Autoselect if able */
	if (num == 1) done = TRUE;

	/* Clear */
	for (i = TABLE_ROW; i < Term->hgt; i++)
	{
		/* Clear */
		Term_erase(col, i, Term->wid - wid);
	}

	/* Choose */
	while (TRUE)
	{
		hgt = Term->hgt - TABLE_ROW - 1;

		/* Redraw the list */
		for (i = 0; ((i + top < num) && (i <= hgt)); i++)
		{
			if (i + top < 26)
			{
#ifdef JP
				strnfmt(buf, sizeof(buf), "%c) %s", I2A(i + top),
				        X_title_name(&choices[i + top]));
#else /* JP */
				strnfmt(buf, sizeof(buf), "%c) %s", I2A(i + top),
				        choices[i + top].name);
#endif /* JP */
			}
			else
			{
				/* ToDo: Fix the ASCII dependency */
#ifdef JP
				strnfmt(buf, sizeof(buf), "%c) %s", 'A' + (i + top - 26),
				        X_title_name(&choices[i + top]));
#else /* JP */
				strnfmt(buf, sizeof(buf), "%c) %s", 'A' + (i + top - 26),
				        choices[i + top].name);
#endif /* JP */
			}

			/* Clear */
			Term_erase(col, i + TABLE_ROW, wid);

			/* Display */
			if (i == (cur - top))
			{
				/* Highlight the current selection */
				if (choices[i + top].grayed) attr = TERM_BLUE;
				else attr = TERM_L_BLUE;
			}
			else
			{
				if (choices[i + top].grayed) attr = TERM_SLATE;
				else attr = TERM_WHITE;
			}

			Term_putstr(col, i + TABLE_ROW, wid, attr, buf);
		}

		if (done)
		{
			/* Set the value */
			*select = cur;

			/* Success */
			return BIRTH_SUCCESS;
		}

		/* Display auxiliary information if any is available. */
		if (hook) hook(choices[cur]);

		/* Move the cursor */
		put_str("", TABLE_ROW + cur - top, col);

		c = inkey();

		/* Exit the game */
		if (c == KTRL('X')) quit(NULL);

		/* Make a choice */
		if ((c == '\n') || (c == '\r'))
		{
			/* Set the value */
			*select = cur;

			/* Success */
			return BIRTH_SUCCESS;
		}

		/* Random choice */
		if (c == '*')
		{
			/* Ensure legal choice */
			do { cur = rand_int(num); } while (choices[cur].grayed);

			/* Done */
			done = TRUE;
		}

		/* Alphabetic choice */
		else if (isalpha((unsigned char)c))
		{
			int choice;

			if (islower((unsigned char)c)) choice = A2I(c);
			else choice = c - 'A' + 26;

			/* Validate input */
			if ((choice > -1) && (choice < num))
			{
				cur = choice;

				/* Done */
				done = TRUE;
			}
			else
			{
#ifdef JP
				bell("�����ȃL�[�ł��I");
#else /* JP */
				bell("Illegal response to question!");
#endif /* JP */
			}
		}

		/* Move */
		else if (isdigit((unsigned char)c))
		{
			/* Get a direction from the key */
			dir = target_dir(c);

			/* Going up? */
			if (dir == 8)
			{
				/* Move selection */
				if (cur != 0) cur--;

				/* Scroll up */
				if ((top > 0) && ((cur - top) < 4)) top--;
			}

			/* Going down? */
			else if (dir == 2)
			{
				/* Move selection */
				if (cur != (num - 1)) cur++;

				/* Scroll down */
				if ((top + hgt < (num - 1)) && ((top + hgt - cur) < 4)) top++;
			}

			/* Going back? */
			else if (dir == 4)
			{
				/* Save the current value (for later) */
				*select = cur;

				/* Return */
				return BIRTH_BACK;
			}

			/* Going forward acts as pressing enter */
			else if (dir == 6)
			{
				/* Set the value */
				*select = cur;

				/* Success */
				return BIRTH_SUCCESS;
			}
		}

		/* Hack - go back */
		else if (c == ESCAPE) return BIRTH_RESTART;

		/* Help */
		else if (c == '?')
		{
			strnfmt(buf, sizeof(buf), "%s#%s", helpfile, choices[cur].name);

			screen_save();
			(void)show_file(buf, NULL, 0, 0);
			screen_load();
		}

		/* Options */
		else if (c == '=')
		{
			do_cmd_options();
		}

		/* Invalid input */
#ifdef JP
		else bell("�����ȃL�[�ł��I");
#else /* JP */
		else bell("Illegal response to question!");
#endif /* JP */

		/* If choice is off screen, move it to the top */
		if ((cur < top) || (cur > top + hgt)) top = cur;
	}
}


/*
 * Display additional information about each race during the selection.
 */
static void race_aux_hook(birth_menu r_str)
{
	int race, i;
	char s[50];

	/* Extract the proper race index from the string. */
	for (race = 0; race < z_info->p_max; race++)
	{
		if (!strcmp(r_str.name, p_name + p_info[race].name)) break;
	}

	if (race == z_info->p_max) return;

	/* Display relevant details. */
	for (i = 0; i < A_MAX; i++)
	{
#ifdef JP
		strnfmt(s, sizeof(s), "%s%+d", stat_names[i],
		        p_info[race].r_adj[i]);
#else /* JP */
		strnfmt(s, sizeof(s), "%s%+d", stat_names_reduced[i],
		        p_info[race].r_adj[i]);
#endif /* JP */
		Term_putstr(RACE_AUX_COL, TABLE_ROW + i, -1, TERM_WHITE, s);
	}

#ifdef JP
	strnfmt(s, sizeof(s), "�̗̓T�C: %d ", p_info[race].r_mhp);
#else /* JP */
	strnfmt(s, sizeof(s), "Hit die: %d ", p_info[race].r_mhp);
#endif /* JP */
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX, -1, TERM_WHITE, s);
#ifdef JP
	strnfmt(s, sizeof(s), "�o���l: %d%% ", p_info[race].r_exp);
#else /* JP */
	strnfmt(s, sizeof(s), "Experience: %d%% ", p_info[race].r_exp);
#endif /* JP */
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 1, -1, TERM_WHITE, s);
#ifdef JP
	strnfmt(s, sizeof(s), "�ԊO������: %d ft ", p_info[race].infra * 10);
#else /* JP */
	strnfmt(s, sizeof(s), "Infravision: %d ft ", p_info[race].infra * 10);
#endif /* JP */
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 2, -1, TERM_WHITE, s);
}


/*
 * Player race
 */
static bool get_player_race(void)
{
	int i, res;
	birth_menu *races;

	C_MAKE(races, z_info->p_max, birth_menu);

	/* Extra info */
#ifdef JP
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
	            "�s�푰�t�ɂ���ăL�����N�^�[�̐�V�I�Ȏ�����{�[�i�X���ω����܂��B");
#else /* JP */
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
	            "Your 'race' determines various intrinsic factors and bonuses.");
#endif /* JP */

	/* Tabulate races */
	for (i = 0; i < z_info->p_max; i++)
	{
		races[i].name = p_name + p_info[i].name;
#ifdef JP
		races[i].J_name = p_name + p_info[i].J_name;
#endif /* JP */
		races[i].grayed = FALSE;
	}

	res = get_player_choice(races, z_info->p_max, p_ptr->prace,
	                                 RACE_COL, 15, &p_ptr->prace,
	                                 "birth.txt", race_aux_hook);

	/* Free memory */
	FREE(races);

	/* No selection? */
	if (res < 0) return (res);

	/* Save the race pointer */
	rp_ptr = &p_info[p_ptr->prace];

	/* Success */
	return (TRUE);
}


/*
 * Display additional information about each class during the selection.
 */
static void class_aux_hook(birth_menu c_str)
{
	int class_idx, i;
	char s[128];

	/* Extract the proper class index from the string. */
	for (class_idx = 0; class_idx < z_info->c_max; class_idx++)
	{
		if (!strcmp(c_str.name, c_name + c_info[class_idx].name)) break;
	}

	if (class_idx == z_info->c_max) return;

	/* Display relevant details. */
	for (i = 0; i < A_MAX; i++)
	{
#ifdef JP
		strnfmt(s, sizeof(s), "%s%+d", stat_names[i],
		        c_info[class_idx].c_adj[i]);
#else /* JP */
		strnfmt(s, sizeof(s), "%s%+d", stat_names_reduced[i],
		        c_info[class_idx].c_adj[i]);
#endif /* JP */
		Term_putstr(CLASS_AUX_COL, TABLE_ROW + i, -1, TERM_WHITE, s);
	}

#ifdef JP
	strnfmt(s, sizeof(s), "�̗̓T�C: %d ", c_info[class_idx].c_mhp);
#else /* JP */
	strnfmt(s, sizeof(s), "Hit die: %d ", c_info[class_idx].c_mhp);
#endif /* JP */
	Term_putstr(CLASS_AUX_COL, TABLE_ROW + A_MAX, -1, TERM_WHITE, s);
#ifdef JP
	strnfmt(s, sizeof(s), "�o���l: %d%% ", c_info[class_idx].c_exp);
#else /* JP */
	strnfmt(s, sizeof(s), "Experience: %d%% ", c_info[class_idx].c_exp);
#endif /* JP */
	Term_putstr(CLASS_AUX_COL, TABLE_ROW + A_MAX + 1, -1, TERM_WHITE, s);
}


/*
 * Player class
 */
static bool get_player_class(void)
{
	int i, res;
	birth_menu *classes;

	C_MAKE(classes, z_info->c_max, birth_menu);

	/* Extra info */
#ifdef JP
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW, /* mada */
	            "�s�E�Ɓt�ɂ���ăL�����N�^�[�̐�V�I�Ȕ\�͂�{�[�i�X���ω����܂��B");
	Term_putstr(QUESTION_COL, QUESTION_ROW + 1, -1, TERM_YELLOW,
	            "�D�F�ŕ\�����ꂽ�I�����͏㋉�Ҍ����̐E�Ƃł��B");
#else /* JP */
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
	            "Your 'class' determines various intrinsic abilities and bonuses.");
	Term_putstr(QUESTION_COL, QUESTION_ROW + 1, -1, TERM_YELLOW,
	            "Any greyed-out entries should only be used by advanced players.");
#endif /* JP */

	/* Tabulate classes */
	for (i = 0; i < z_info->c_max; i++)
	{
		/* Analyze */
		if (!(rp_ptr->choice & (1L << i))) classes[i].grayed = TRUE;
		else classes[i].grayed = FALSE;

		/* Save the string */
		classes[i].name = c_name + c_info[i].name;
#ifdef JP
		classes[i].J_name = c_name + c_info[i].J_name;
#endif /* JP */
	}

	res = get_player_choice(classes, z_info->c_max, p_ptr->pclass,
	                                  CLASS_COL, 20, &p_ptr->pclass,
	                                  "birth.txt", class_aux_hook);

	/* Free memory */
	FREE(classes);

	/* No selection? */
	if (res < 0) return (res);

	/* Set class */
	cp_ptr = &c_info[p_ptr->pclass];
	mp_ptr = &cp_ptr->spells;

	return (TRUE);
}


/*
 * Player sex
 */
static bool get_player_sex(void)
{
	int i, res;
	birth_menu genders[MAX_SEXES];

	/* Extra info */
#ifdef JP
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
	            "�s���ʁt�̈Ⴂ�̓Q�[������ɉe�����y�ڂ��܂���B");
#else /* JP */
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
	            "Your 'sex' does not have any significant gameplay effects.");
#endif /* JP */

	/* Tabulate genders */
	for (i = 0; i < MAX_SEXES; i++)
	{
		genders[i].name = sex_info[i].title;
#ifdef JP
		genders[i].J_name = sex_info[i].J_title;
#endif /* JP */
		genders[i].grayed = FALSE;
	}

	res = get_player_choice(genders, MAX_SEXES, p_ptr->psex,
	                                SEX_COL, 15, &p_ptr->psex,
	                                "birth.txt", NULL);

	/* No selection? */
	if (res < 0) return (res);

	/* Save the sex pointer */
	sp_ptr = &sex_info[p_ptr->psex];

	return (TRUE);
}


/*
 * Helper function for 'player_birth()'.
 *
 * This function allows the player to select a sex, race, and class, and
 * modify options (including the birth options).
 */
static bool player_birth_aux_1(void)
{
	int i, res, cur;

	/*** Instructions ***/

	/* Clear screen */
	Term_clear();

	/* Output to the screen */
	text_out_hook = text_out_to_screen;

	/* Indent output */
	text_out_indent = QUESTION_COL;
	Term_gotoxy(QUESTION_COL, HEADER_ROW);

	/* Display some helpful information */
#ifdef JP
	text_out_c(TERM_L_BLUE,
	           "�ȉ��̑I��������L�����N�^�[��I�����Ă�������:\n\n");
	text_out_c(TERM_L_GREEN, "�ړ��L�[");
	text_out("�Ń��j���[���X�N���[���B");
	text_out_c(TERM_L_GREEN, "���^�[���L�[");
	text_out("�Ō��݂̑I������I���B");
	text_out_c(TERM_L_GREEN, "���^�[���L�[");
	text_out("�Ō��݂̑I������I���B'");
	text_out_c(TERM_L_GREEN, "*");
	text_out("'�őI�����̒����烉���_���ɑI���B'");
	text_out_c(TERM_L_GREEN, "ESC");
	text_out("'�ŃL�����N�^�[�̐�������蒼���B'");
	text_out_c(TERM_L_GREEN, "=");
	text_out("'�ŏ����I�v�V�����B'");
	text_out_c(TERM_L_GREEN, "?");
	text_out("'�Ńw���v�B'");
	text_out_c(TERM_L_GREEN, "Ctrl-X");
	text_out("'�ŏI���B");
#else /* JP */
	text_out_c(TERM_L_BLUE,
	           "Please select your character from the menu below:\n\n");
	text_out("Use the ");
	text_out_c(TERM_L_GREEN, "movement keys");
	text_out(" to scroll the menu, ");
	text_out_c(TERM_L_GREEN, "Enter");
	text_out(" to select the current menu item, '");
	text_out_c(TERM_L_GREEN, "*");
	text_out("' for a random menu item, '");
	text_out_c(TERM_L_GREEN, "ESC");
	text_out("' to restart the character selection, '");
	text_out_c(TERM_L_GREEN, "=");
	text_out("' for the birth options, '");
	text_out_c(TERM_L_GREEN, "?");
	text_out("' for help, or '");
	text_out_c(TERM_L_GREEN, "Ctrl-X");
	text_out("' to quit.");
#endif /* JP */

	/* Reset text_out() indentation */
	text_out_indent = 0;

	/* Prepare for user selection */
	cur = 0;
	res = 0;

	/* Get user choices */
	while (cur < 3)
	{
		/* Get the player's choice */
		switch (cur)
		{
			case 0: res = get_player_sex(); break;
			case 1: res = get_player_race(); break;
			case 2: res = get_player_class(); break;
		}

		/* Clean up */
		clear_question();

		/* Act on the result */
		if (res == BIRTH_RESTART) return (FALSE);

		/* Work out where we are on the menu system now */
		if ((res == BIRTH_BACK) && (cur > 0)) cur--;
		else cur++;
	}

	/* Set adult options from birth options */
	for (i = OPT_BIRTH; i < OPT_CHEAT; i++)
	{
		op_ptr->opt[OPT_ADULT + (i - OPT_BIRTH)] = op_ptr->opt[i];
	}

	/* Reset score options from cheat options */
	for (i = OPT_CHEAT; i < OPT_ADULT; i++)
	{
		op_ptr->opt[OPT_SCORE + (i - OPT_CHEAT)] = op_ptr->opt[i];
	}

	/* Clear */
	Term_clear();

	/* Done */
	return (TRUE);
}


/*
 * Initial stat costs (initial stats always range from 10 to 18 inclusive).
 */
static const int birth_stat_costs[(18-10)+1] = { 0, 1, 2, 4, 7, 11, 16, 22, 30 };


/*
 * Helper function for 'player_birth()'.
 *
 * This function handles "point-based" character creation.
 *
 * The player selects, for each stat, a value from 10 to 18 (inclusive),
 * each costing a certain amount of points (as above), from a pool of 48
 * available points, to which race/class modifiers are then applied.
 *
 * Each unused point is converted into 100 gold pieces.
 */
static bool player_birth_aux_2(void)
{
	int i;

	int row = 3;
	int col = 42;

	int stat = 0;

	int stats[A_MAX];

	int cost;

	char ch;

	char buf[80];


	/* Initialize stats */
	for (i = 0; i < A_MAX; i++)
	{
		/* Initial stats */
		stats[i] = 10;
	}


	/* Roll for base hitpoints */
	get_extra();

	/* Roll for age/height/weight */
	get_ahw();

	/* Roll for social class */
	get_history();


	/* Interact */
	while (1)
	{
		/* Reset cost */
		cost = 0;

		/* Process stats */
		for (i = 0; i < A_MAX; i++)
		{
			/* Variable stat maxes */
			if (adult_maximize)
			{
				/* Reset stats */
				p_ptr->stat_cur[i] = p_ptr->stat_max[i] = stats[i];

			}

			/* Fixed stat maxes */
			else
			{
				/* Obtain a "bonus" for "race" and "class" */
				int bonus = rp_ptr->r_adj[i] + cp_ptr->c_adj[i];

				/* Apply the racial/class bonuses */
				p_ptr->stat_cur[i] = p_ptr->stat_max[i] =
					modify_stat_value(stats[i], bonus);
			}

			/* Total cost */
			cost += birth_stat_costs[stats[i] - 10];
		}

		/* Restrict cost */
		if (cost > 48)
		{
			/* Warning */
			bell("Excessive stats!");

			/* Reduce stat */
			stats[stat]--;

			/* Recompute costs */
			continue;
		}

		/* Gold is inversely proportional to cost */
		p_ptr->au = (100 * (48 - cost)) + 100;

		/* Calculate the bonuses and hitpoints */
		p_ptr->update |= (PU_BONUS | PU_HP);

		/* Update stuff */
		update_stuff();

		/* Fully healed */
		p_ptr->chp = p_ptr->mhp;

		/* Fully rested */
		p_ptr->csp = p_ptr->msp;

		/* Display the player */
		display_player(0);

		/* Display the costs header */
#ifdef JP
		put_str("�R�X�g", row - 1, col + 30);
#else /* JP */
		put_str("Cost", row - 1, col + 32);
#endif /* JP */

		/* Display the costs */
		for (i = 0; i < A_MAX; i++)
		{
			/* Display cost */
			strnfmt(buf, sizeof(buf), "%4d", birth_stat_costs[stats[i] - 10]);
			put_str(buf, row + i, col + 32);
		}


		/* Prompt XXX XXX XXX */
#ifdef JP
		strnfmt(buf, sizeof(buf), "���R�X�g %2d/48:  2/8 �ňړ�, 4/6 �ŕύX, ���^�[���L�[�Ō���", cost);
#else /* JP */
		strnfmt(buf, sizeof(buf), "Total Cost %2d/48.  Use 2/8 to move, 4/6 to modify, 'Enter' to accept.", cost);
#endif /* JP */
		prt(buf, 0, 0);

		/* Place cursor just after cost of current stat */
		Term_gotoxy(col + 36, row + stat);

		/* Get key */
		ch = inkey();

		/* Quit */
		if (ch == 'Q') quit(NULL);

		/* Start over */
		if (ch == 'S') return (FALSE);

		/* Done */
		if ((ch == '\r') || (ch == '\n')) break;

		/* Prev stat */
		if (ch == '8')
		{
			stat = (stat + A_MAX - 1) % A_MAX;
		}

		/* Next stat */
		if (ch == '2')
		{
			stat = (stat + 1) % A_MAX;
		}

		/* Decrease stat */
		if ((ch == '4') && (stats[stat] > 10))
		{
			stats[stat]--;
		}

		/* Increase stat */
		if ((ch == '6') && (stats[stat] < 18))
		{
			stats[stat]++;
		}
	}


	/* Done */
	return (TRUE);
}


/*
 * Helper function for 'player_birth()'.
 *
 * This function handles "auto-rolling" and "random-rolling".
 */
static bool player_birth_aux_3(void)
{
	int i, j, m, v;

	bool flag;
	bool prev = FALSE;

	char ch;

	char b1 = '[';
	char b2 = ']';

	char buf[80];


#ifdef ALLOW_AUTOROLLER

	s16b stat_limit[A_MAX];

	s32b stat_match[A_MAX];

	s32b auto_round = 0L;

	s32b last_round;


	/*** Autoroll ***/

	/* Initialize */
	if (adult_auto_roller)
	{
		int mval[A_MAX];

		char inp[80];


		/* Extra info */
#ifdef JP
		Term_putstr(5, 10, -1, TERM_WHITE,
		            "�s�I�[�g���[���[�t���g�p����ƃX�e�[�^�X���ݒ肵���ŏ��l���");
		Term_putstr(5, 11, -1, TERM_WHITE,
		            "�Ⴂ�L�����N�^�[�������I�ɖ������邱�Ƃ��ł��܂��B");
		Term_putstr(5, 12, -1, TERM_WHITE,
		            "�X�e�[�^�X�݂͌��Ɋ֘A���Ă���̂ŁA�S�ẴX�e�[�^�X���ő�l");
		Term_putstr(5, 13, -1, TERM_WHITE,
		            "(���邢�͔��ɍ����l)�ɐݒ肷�邱�Ƃ͕s�\�ł��B");
#else
		Term_putstr(5, 10, -1, TERM_WHITE,
		            "The auto-roller will automatically ignore characters which do");
		Term_putstr(5, 11, -1, TERM_WHITE,
		            "not meet the minimum values for any stats specified below.");
		Term_putstr(5, 12, -1, TERM_WHITE,
		            "Note that stats are not independant, so it is not possible to");
		Term_putstr(5, 13, -1, TERM_WHITE,
		            "get perfect (or even high) values for all your stats.");
#endif

		/* Prompt for the minimum stats */
#ifdef JP
		put_str("�ŏ��l����͂��ĉ�����: ", 15, 2);
#else
		put_str("Enter minimum value for: ", 15, 2);
#endif

		/* Output the maximum stats */
		for (i = 0; i < A_MAX; i++)
		{
			/* Reset the "success" counter */
			stat_match[i] = 0;

			/* Race/Class bonus */
			j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i];

			/* Obtain the "maximal" stat */
			m = adjust_stat(17, j, TRUE);

			/* Save the maximum */
			mval[i] = m;

			/* Extract a textual format */
			/* cnv_stat(m, inp); */

			/* Above 18 */
			if (m > 18)
			{
#ifdef JP
				strnfmt(inp, sizeof(inp), "(�ő�l 18/%02d):", (m - 18));
#else /* JP */
				strnfmt(inp, sizeof(inp), "(Max of 18/%02d):", (m - 18));
#endif /* JP */
			}

			/* From 3 to 18 */
			else
			{
#ifdef JP
				strnfmt(inp, sizeof(inp), "(�ő�l %2d):", m);
#else /* JP */
				strnfmt(inp, sizeof(inp), "(Max of %2d):", m);
#endif /* JP */
			}

			/* Prepare a prompt */
			strnfmt(buf, sizeof(buf), "%-5s%-20s", stat_names[i], inp);

			/* Dump the prompt */
			put_str(buf, 16 + i, 5);
		}

		/* Input the minimum stats */
		for (i = 0; i < A_MAX; i++)
		{
			/* Get a minimum stat */
			while (TRUE)
			{
				char *s;

				/* Move the cursor */
				put_str("", 16 + i, 30);

				/* Default */
				strcpy(inp, "");

				/* Get a response (or escape) */
				if (!askfor_aux(inp, 9)) inp[0] = '\0';

				/* Hack -- add a fake slash */
				strcat(inp, "/");

				/* Hack -- look for the "slash" */
				s = strchr(inp, '/');

				/* Hack -- Nuke the slash */
				*s++ = '\0';

				/* Hack -- Extract an input */
				v = atoi(inp) + atoi(s);

				/* Break on valid input */
				if (v <= mval[i]) break;
			}

			/* Save the minimum stat */
			stat_limit[i] = (v > 0) ? v : 0;
		}
	}

#endif /* ALLOW_AUTOROLLER */

	/* Clean up */
	clear_from(10);


	/*** Generate ***/

	/* Roll */
	while (TRUE)
	{
		int col = 42;

#ifdef ALLOW_AUTOROLLER

		/* Feedback */
		if (adult_auto_roller)
		{
			Term_clear();

			/* Label */
#ifdef JP
			put_str("�ŏ��l", 2, col+5);
#else
			put_str(" Limit", 2, col+5);
#endif

			/* Label */
#ifdef JP
			put_str("������", 2, col+13);
#else
			put_str("  Freq", 2, col+13);
#endif

			/* Label */
#ifdef JP
			put_str("���ݒl", 2, col+24);
#else
			put_str("  Roll", 2, col+24);
#endif

			/* Put the minimal stats */
			for (i = 0; i < A_MAX; i++)
			{
				/* Label stats */
				put_str(stat_names[i], 3+i, col);

				/* Put the stat */
				cnv_stat(stat_limit[i], buf);
				c_put_str(TERM_L_BLUE, buf, 3+i, col+5);
			}

			/* Note when we started */
			last_round = auto_round;

			/* Label count */
#ifdef JP
			put_str("�� :", 10, col+13);
#else
			put_str("Round:", 10, col+13);
#endif

			/* Indicate the state */
#ifdef JP
			put_str("(ESC�Œ�~)", 12, col+13);
#else
			put_str("(Hit ESC to stop)", 12, col+13);
#endif

			/* Auto-roll */
			while (1)
			{
				bool accept = TRUE;

				/* Get a new character */
				get_stats();

				/* Advance the round */
				auto_round++;

				/* Hack -- Prevent overflow */
				if (auto_round >= 1000000L) break;

				/* Check and count acceptable stats */
				for (i = 0; i < A_MAX; i++)
				{
					/* This stat is okay */
					if (stat_use[i] >= stat_limit[i])
					{
						stat_match[i]++;
					}

					/* This stat is not okay */
					else
					{
						accept = FALSE;
					}
				}

				/* Break if "happy" */
				if (accept) break;

				/* Take note every 25 rolls */
				flag = (!(auto_round % 25L));

				/* Update display occasionally */
				if (flag || (auto_round < last_round + 100))
				{
					/* Put the stats (and percents) */
					for (i = 0; i < A_MAX; i++)
					{
						/* Put the stat */
						cnv_stat(stat_use[i], buf);
						c_put_str(TERM_L_GREEN, buf, 3+i, col+24);

						/* Put the percent */
						if (stat_match[i])
						{
							int p = 1000L * stat_match[i] / auto_round;
							byte attr = (p < 100) ? TERM_YELLOW : TERM_L_GREEN;
							strnfmt(buf, sizeof(buf), "%3d.%d%%", p/10, p%10);
							c_put_str(attr, buf, 3+i, col+13);
						}

						/* Never happened */
						else
						{
#ifdef JP
							c_put_str(TERM_RED, "(�Ȃ�)", 3+i, col+13);
#else
							c_put_str(TERM_RED, "(NONE)", 3+i, col+13);
#endif
						}
					}

					/* Dump round */
					put_str(format("%10ld", auto_round), 10, col+20);

					/* Make sure they see everything */
					Term_fresh();

					/* Do not wait for a key */
					inkey_scan = TRUE;

					/* Check for a keypress */
					if (inkey()) break;
				}
			}
		}

		/* Otherwise just get a character */
		else

#endif /* ALLOW_AUTOROLLER */

		{
			/* Get a new character */
			get_stats();
		}

		/* Flush input */
		flush();


		/*** Display ***/

		/* Roll for base hitpoints */
		get_extra();

		/* Roll for age/height/weight */
		get_ahw();

		/* Roll for social class */
		get_history();

		/* Roll for gold */
		get_money();

		/* Input loop */
		while (TRUE)
		{
			/* Calculate the bonuses and hitpoints */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Update stuff */
			update_stuff();

			/* Fully healed */
			p_ptr->chp = p_ptr->mhp;

			/* Fully rested */
			p_ptr->csp = p_ptr->msp;

			/* Display the player */
			display_player(0);

			/* Prepare a prompt (must squeeze everything in) */
#ifdef JP
			/* �I���W�i���̂܂܂��Ə������Ɋ���Ă��Č��h���������̂ŏC�� */
			Term_gotoxy(9, 23);
#else
			Term_gotoxy(2, 23);
#endif
			Term_addch(TERM_WHITE, b1);
#ifdef JP
			Term_addstr(-1, TERM_WHITE, "'r'�Ŏ��̐��l");
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p'�őO�̐��l");
			Term_addstr(-1, TERM_WHITE, ", ���^�[���L�[�ł��̐��l�Ɍ���");
#else
			Term_addstr(-1, TERM_WHITE, "'r' to reroll");
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p' for prev");
			Term_addstr(-1, TERM_WHITE, ", or 'Enter' to accept");
#endif
			Term_addch(TERM_WHITE, b2);

			/* Prompt and get a command */
			ch = inkey();

			/* Quit */
			if (ch == 'Q') quit(NULL);

			/* Start over */
			if (ch == 'S') return (FALSE);

			/* 'Enter' accepts the roll */
			if ((ch == '\r') || (ch == '\n')) break;

			/* Reroll this character */
			if ((ch == ' ') || (ch == 'r')) break;

			/* Previous character */
			if (prev && (ch == 'p'))
			{
				load_prev_data();
				continue;
			}

			/* Help */
			if (ch == '?')
			{
				do_cmd_help();
				continue;
			}

			/* Warning */
#ifdef JP
			bell("�����ȃL�[�ł��I");
#else
			bell("Illegal auto-roller command!");
#endif
		}

		/* Are we done? */
		if ((ch == '\r') || (ch == '\n')) break;

		/* Save this for the "previous" character */
		save_prev_data();

		/* Note that a previous roll exists */
		prev = TRUE;
	}

	/* Clear prompt */
	clear_from(23);

	/* Done */
	return (TRUE);
}


/*
 * Helper function for 'player_birth()'.
 *
 * See "display_player" for screen layout code.
 */
static bool player_birth_aux(void)
{
	char ch;
#ifdef JP
	cptr prompt = "[ 'Q' �Œ��f, 'S' �ŏ��߂���, ���̑��̃L�[�ŃQ�[���J�n ]";
#else
	cptr prompt = "['Q' to suicide, 'S' to start over, or any other key to continue]";
#endif

	/* Ask questions */
	if (!player_birth_aux_1()) return (FALSE);

	/* Point-based */
	if (adult_point_based)
	{
		/* Point based */
		if (!player_birth_aux_2()) return (FALSE);
	}

	/* Random */
	else
	{
		/* Auto-roll */
		if (!player_birth_aux_3()) return (FALSE);
	}

	/* Get a name, prepare savefile */
	get_name();

	/* Display the player */
	display_player(0);

	/* Prompt for it */
	prt(prompt, Term->hgt - 1, Term->wid / 2 - strlen(prompt) / 2);

	/* Get a key */
	ch = inkey();

	/* Quit */
	if (ch == 'Q') quit(NULL);

	/* Start over */
	if (ch == 'S') return (FALSE);

	/* Accept */
	return (TRUE);
}


/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
void player_birth(void)
{
	int i, n;


	/* Create a new character */
	while (1)
	{
		/* Wipe the player */
		player_wipe();

		/* Roll up a new character */
		if (player_birth_aux()) break;
	}


	/* Note player birth in the message recall */
	message_add(" ", MSG_GENERIC);
	message_add("  ", MSG_GENERIC);
	message_add("====================", MSG_GENERIC);
	message_add("  ", MSG_GENERIC);
	message_add(" ", MSG_GENERIC);


	/* Hack -- outfit the player */
	player_outfit();

	/* Event -- player birth done */
	player_birth_done_hook();

	/* Shops */
	for (n = 0; n < MAX_STORES; n++)
	{
		/* Initialize */
		store_init(n);

		/* Ignore home */
		if (n == STORE_HOME) continue;

		/* Maintain the shop (ten times) */
		for (i = 0; i < 10; i++) store_maint(n);
	}
}
