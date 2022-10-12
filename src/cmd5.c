/* File: cmd5.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/*
 * 2.7.9v3 ���{��Ő���: ���Ƃ���
 * 2.7.9v6 �Ή�        : �ݍN�i, FIRST, ���Ƃ���
 * 2.8.0   �Ή�        : sayu, ���Ƃ���
 * 2.8.1   �Ή�        : FIRST
 * 2.8.3   �Ή�        : FIRST, ���Ƃ���
 * 2.9.0   �Ή�        : �퐣
 *
 * ���{��ŋ@�\�ǉ� : �����̈ꗗ����ɕ\������
 *                    �p���؂�ւ��@�\
 */

#include "angband.h"

#include "script.h"


/*
 * Returns chance of failure for a spell
 */
s16b spell_chance(int spell)
{
	int chance, minfail;

	const magic_type *s_ptr;


	/* Paranoia -- must be literate */
	if (!cp_ptr->spell_book) return (100);

	/* Get the spell */
	s_ptr = &mp_ptr->info[spell];

	/* Extract the base spell failure rate */
	chance = s_ptr->sfail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (p_ptr->lev - s_ptr->slevel);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]];

	/* Not enough mana to cast */
	if (s_ptr->smana > p_ptr->csp)
	{
		chance += 5 * (s_ptr->smana - p_ptr->csp);
	}

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]];

	/* Non mage/priest characters never get better than 5 percent */
	if (!(cp_ptr->flags & CF_ZERO_FAIL))
	{
		if (minfail < 5) minfail = 5;
	}

	/* Priest prayer penalty for "edged" weapons (before minfail) */
	if (p_ptr->icky_wield)
	{
		chance += 25;
	}

	/* Minimum failure rate */
	if (chance < minfail) chance = minfail;

	/* Stunning makes spells harder (after minfail) */
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;

	/* Always a 5 percent chance of working */
	if (chance > 95) chance = 95;

	/* Return the chance */
	return (chance);
}



/*
 * Determine if a spell is "okay" for the player to cast or study
 * The spell must be legible, not forgotten, and also, to cast,
 * it must be known, and to study, it must not be known.
 */
bool spell_okay(int spell, bool known)
{
	const magic_type *s_ptr;

	/* Get the spell */
	s_ptr = &mp_ptr->info[spell];

	/* Spell is illegal */
	if (s_ptr->slevel > p_ptr->lev) return (FALSE);

	/* Spell is forgotten */
	if (p_ptr->spell_flags[spell] & PY_SPELL_FORGOTTEN)
	{
		/* Never okay */
		return (FALSE);
	}

	/* Spell is learned */
	if (p_ptr->spell_flags[spell] & PY_SPELL_LEARNED)
	{
		/* Okay to cast, not to study */
		return (known);
	}

	/* Okay to study, not to cast */
	return (!known);
}


/*
 * Print a list of spells (for browsing or casting or viewing).
 */
void print_spells(const byte *spells, int num, int y, int x)
{
	int i, spell;

	const magic_type *s_ptr;

	cptr comment;

	char out_val[160];

	byte line_attr;

	/* Title the list */
	prt("", y, x);
#ifdef JP
	put_str("���O", y, x + 5);
	put_str("Lv   MP ���� ����", y, x + 35);
#else /* JP */
	put_str("Name", y, x + 5);
	put_str("Lv Mana Fail Info", y, x + 35);
#endif /* JP */

	/* Dump the spells */
	for (i = 0; i < num; i++)
	{
		/* Get the spell index */
		spell = spells[i];

		/* Get the spell info */
		s_ptr = &mp_ptr->info[spell];

		/* Skip illegible spells */
		if (s_ptr->slevel >= 99)
		{
#ifdef JP
			strnfmt(out_val, sizeof(out_val), "  %c) %-30s", I2A(i), "(���Ǖs�\)");
#else /* JP */
			strnfmt(out_val, sizeof(out_val), "  %c) %-30s", I2A(i), "(illegible)");
#endif /* JP */
			c_prt(TERM_L_DARK, out_val, y + i + 1, x);
			continue;
		}

		/* Get extra info */
		comment = get_spell_info(cp_ptr->spell_book, spell);

		/* Assume spell is known and tried */
		line_attr = TERM_WHITE;

		/* Analyze the spell */
		if (p_ptr->spell_flags[spell] & PY_SPELL_FORGOTTEN)
		{
#ifdef JP
			comment = " �Y�p";
#else /* JP */
			comment = " forgotten";
#endif /* JP */
			line_attr = TERM_YELLOW;
		}
		else if (!(p_ptr->spell_flags[spell] & PY_SPELL_LEARNED))
		{
			if (s_ptr->slevel <= p_ptr->lev)
			{
#ifdef JP
				comment = " ���m";
#else /* JP */
				comment = " unknown";
#endif /* JP */
				line_attr = TERM_L_BLUE;
			}
			else
			{
#ifdef JP
				comment = " ����";
#else /* JP */
				comment = " difficult";
#endif /* JP */
				line_attr = TERM_RED;
			}
		}
		else if (!(p_ptr->spell_flags[spell] & PY_SPELL_WORKED))
		{
#ifdef JP
			comment = " ���o��";
#else /* JP */
			comment = " untried";
#endif /* JP */
			line_attr = TERM_L_GREEN;
		}

		/* Dump the spell --(-- */
		strnfmt(out_val, sizeof(out_val), "  %c) %-30s%2d %4d %3d%%%s",
		        I2A(i), get_spell_name(cp_ptr->spell_book, spell),
		        s_ptr->slevel, s_ptr->smana, spell_chance(spell), comment);
		c_prt(line_attr, out_val, y + i + 1, x);
	}

	/* Clear the bottom line */
	prt("", y + i + 1, x);
}



/*
 * Hack -- display an object kind in the current window
 *
 * Include list of usable spells for readible books
 */
void display_koff(int k_idx)
{
	int y;

	object_type *i_ptr;
	object_type object_type_body;

	char o_name[80];


	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* No info */
	if (!k_idx) return;


	/* Get local object */
	i_ptr = &object_type_body;

	/* Prepare the object */
	object_wipe(i_ptr);

	/* Prepare the object */
	object_prep(i_ptr, k_idx);


	/* Describe */
	object_desc_spoil(o_name, sizeof(o_name), i_ptr, FALSE, 0);

	/* Mention the object name */
	Term_putstr(0, 0, -1, TERM_WHITE, o_name);


	/* Warriors are illiterate */
	if (!cp_ptr->spell_book) return;

	/* Display spells in readible books */
	if (i_ptr->tval == cp_ptr->spell_book)
	{
		int i;
		int spell;
		int num = 0;

		byte spells[PY_MAX_SPELLS];


		/* Extract spells */
		for (i = 0; i < SPELLS_PER_BOOK; i++)
		{
			spell = get_spell_index(i_ptr, i);

			/* Collect this spell */
			if (spell >= 0) spells[num++] = spell;
		}

		/* Print spells */
		print_spells(spells, num, 2, 0);
	}
}



/*
 * Allow user to choose a spell/prayer from the given book.
 *
 * Returns -1 if the user hits escape.
 * Returns -2 if there are no legal choices.
 * Returns a valid spell otherwise.
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 */
static int get_spell(const object_type *o_ptr, cptr prompt, bool known)
{
	int i;

	int spell;
	int num = 0;

	byte spells[PY_MAX_SPELLS];

	bool verify;

	bool flag, redraw, okay;
	char choice;

	const magic_type *s_ptr;

	char out_val[160];

#ifdef JP
	char        jverb_buf[80];
	cptr p = ((cp_ptr->spell_book == TV_MAGIC_BOOK) ? "����" : "�F��");
#else
	cptr p = ((cp_ptr->spell_book == TV_MAGIC_BOOK) ? "spell" : "prayer");
#endif

#ifdef ALLOW_REPEAT

	int result;

	/* Get the spell, if available */
	if (repeat_pull(&result))
	{
		/* Verify the spell */
		if (spell_okay(result, known))
		{
			/* Success */
			return (result);
		}
		else
		{
			/* Invalid repeat - reset it */
			repeat_clear();
		}
	}

#endif /* ALLOW_REPEAT */

	/* Extract spells */
	for (i = 0; i < SPELLS_PER_BOOK; i++)
	{
		spell = get_spell_index(o_ptr, i);

		/* Collect this spell */
		if (spell != -1) spells[num++] = spell;
	}

	/* Assume no usable spells */
	okay = FALSE;

	/* Check for "okay" spells */
	for (i = 0; i < num; i++)
	{
		/* Look for "okay" spells */
		if (spell_okay(spells[i], known)) okay = TRUE;
	}

	/* No available spells */
	if (!okay) return (-2);


	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt (accept all spells) */
#ifdef JP
	jverb(prompt, jverb_buf, JVERB_AND);
	strnfmt(out_val, 78, "(%^s %c-%c, '*'�ňꗗ, ESC�Œ��f) �ǂ�%s��%^s�܂���? ",
	        p, I2A(0), I2A(num - 1), p, jverb_buf );
#else
	strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
	        p, I2A(0), I2A(num - 1), prompt, p);
#endif

#ifdef JP
	/* Show the list */
	if (always_show_list)
	{
		/* Show list */
		redraw = TRUE;

		/* Save screen */
		screen_save();

		/* Display a list of spells */
		print_spells(spells, num, 1, 20);
	}
#endif

	/* Get a spell from the user */
	while (!flag && get_com(out_val, &choice))
	{
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Hide the list */
			if (redraw)
			{
				/* Load screen */
				screen_load();

				/* Hide list */
				redraw = FALSE;
			}

			/* Show the list */
			else
			{
				/* Show list */
				redraw = TRUE;

				/* Save screen */
				screen_save();

				/* Display a list of spells */
				print_spells(spells, num, 1, 20);
			}

			/* Ask again */
			continue;
		}


		/* Note verify */
		verify = (isupper((unsigned char)choice) ? TRUE : FALSE);

		/* Lowercase */
		choice = tolower((unsigned char)choice);

		/* Extract request */
		i = (islower((unsigned char)choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
#ifdef JP
			bell("�����ȃL�[�ł��I");
#else
			bell("Illegal spell choice!");
#endif
			continue;
		}

		/* Save the spell index */
		spell = spells[i];

		/* Require "okay" spells */
		if (!spell_okay(spell, known))
		{
#ifdef JP
			bell("�����ȃL�[�ł��I");
			msg_format("����%s��%s���Ƃ͂ł��܂���B", p, prompt);
#else /* JP */
			bell("Illegal spell choice!");
			msg_format("You may not %s that %s.", prompt, p);
#endif /* JP */
			continue;
		}

		/* Verify it */
		if (verify)
		{
			char tmp_val[160];

			/* Get the spell */
			s_ptr = &mp_ptr->info[spell];

			/* Prompt */
#ifdef JP
			jverb(prompt, jverb_buf, JVERB_AND);
			strnfmt(tmp_val, 78, "%s(MP%d, ���s��%d%%)��%s�܂���? ",
			        get_spell_name(cp_ptr->spell_book, spell),
			        s_ptr->smana, spell_chance(spell), jverb_buf);
#else /* JP */
			strnfmt(tmp_val, 78, "%^s %s (%d mana, %d%% fail)? ",
			        prompt, get_spell_name(cp_ptr->spell_book, spell),
			        s_ptr->smana, spell_chance(spell));
#endif /* JP */

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}


	/* Restore the screen */
	if (redraw)
	{
		/* Load screen */
		screen_load();
	}


	/* Abort if needed */
	if (!flag) return (-1);

#ifdef ALLOW_REPEAT

	repeat_push(spell);

#endif /* ALLOW_REPEAT */

	/* Success */
	return (spell);
}


void do_cmd_browse_aux(const object_type *o_ptr)
{
	int i;
	int spell;
	int num = 0;

	byte spells[PY_MAX_SPELLS];


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Extract spells */
	for (i = 0; i < SPELLS_PER_BOOK; i++)
	{
		spell = get_spell_index(o_ptr, i);

		/* Collect this spell */
		if (spell != -1) spells[num++] = spell;
	}


	/* Save screen */
	screen_save();

	/* Display the spells */
	print_spells(spells, num, 1, 20);

	/* Prompt for a command */
#ifdef JP
	put_str("(�Ǐ�) �R�}���h: ", 0, 0);
#else /* JP */
	put_str("(Browsing) Command: ", 0, 0);
#endif /* JP */

	/* Hack -- Get a new command */
	p_ptr->command_new = inkey();

	/* Load screen */
	screen_load();


	/* Hack -- Process "Escape" */
	if (p_ptr->command_new == ESCAPE)
	{
		/* Reset stuff */
		p_ptr->command_new = 0;
	}
}


/*
 * Peruse the spells/prayers in a Book
 *
 * Note that *all* spells in the book are listed
 *
 * Note that browsing is allowed while confused or blind,
 * and in the dark, primarily to allow browsing in stores.
 */
void do_cmd_browse(void)
{
	int item;

	object_type *o_ptr;

	cptr q, s;


	/* Warriors are illiterate */
	if (!cp_ptr->spell_book)
	{
#ifdef JP
		msg_print("�{��ǂނ��Ƃ��ł��Ȃ��I");
#else
		msg_print("You cannot read books!");
#endif
		return;
	}

#if 0

	/* No lite */
	if (p_ptr->blind || no_lite())
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��I");
#else
		msg_print("You cannot see!");
#endif
		return;
	}

	/* Confused */
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������ēǂ߂Ȃ��I");
#else
		msg_print("You are too confused!");
#endif
		return;
	}

#endif

	/* Restrict choices to "useful" books */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
#ifdef JP
	q = "�ǂ̖{��ǂ݂܂���? ";
	s = "�ǂ߂�{���Ȃ��B";
#else
	q = "Browse which book? ";
	s = "You have no books that you can read.";
#endif
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Browse the book */
	do_cmd_browse_aux(o_ptr);
}




/*
 * Study a book to gain a new spell/prayer
 */
void do_cmd_study(void)
{
	int i, item;

	int spell;

#ifdef JP
	cptr p = ((cp_ptr->spell_book == TV_MAGIC_BOOK) ? "����" : "�F��");
#else
	cptr p = ((cp_ptr->spell_book == TV_MAGIC_BOOK) ? "spell" : "prayer");
#endif

	cptr q, s;

	object_type *o_ptr;


	if (!cp_ptr->spell_book)
	{
#ifdef JP
		msg_print("�{��ǂނ��Ƃ��ł��Ȃ��I");
#else
		msg_print("You cannot read books!");
#endif
		return;
	}

	if (p_ptr->blind || no_lite())
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��I");
#else
		msg_print("You cannot see!");
#endif
		return;
	}

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������ēǂ߂Ȃ��I");
#else
		msg_print("You are too confused!");
#endif
		return;
	}

	if (!(p_ptr->new_spells))
	{
#ifdef JP
		msg_format("�V����%s���o���邱�Ƃ͂ł��Ȃ��I", p);
#else
		msg_format("You cannot learn any new %ss!", p);
#endif
		return;
	}


	/* Restrict choices to "useful" books */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
#ifdef JP
	q = "�ǂ̖{����w�т܂���? ";
	s = "�ǂ߂�{���Ȃ��B";
#else
	q = "Study which book? ";
	s = "You have no books that you can read.";
#endif
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Mage -- Learn a selected spell */
	if (cp_ptr->flags & CF_CHOOSE_SPELLS)
	{
		/* Ask for a spell */
#ifdef JP
		spell = get_spell(o_ptr, "�w��", FALSE);
#else /* JP */
		spell = get_spell(o_ptr, "study", FALSE);
#endif /* JP */

		/* Allow cancel */
		if (spell == -1) return;
	}
	else
	{
		int k = 0;

		int gift = -1;

		/* Extract spells */
		for (i = 0; i < SPELLS_PER_BOOK; i++)
		{
			spell = get_spell_index(o_ptr, i);

			/* Skip empty spells */
			if (spell == -1) continue;

			/* Skip non "okay" prayers */
			if (!spell_okay(spell, FALSE)) continue;

			/* Apply the randomizer */
			if ((++k > 1) && (rand_int(k) != 0)) continue;

			/* Track it */
			gift = spell;
		}

		/* Accept gift */
		spell = gift;
	}

	/* Nothing to study */
	if (spell < 0)
	{
		/* Message */
#ifdef JP
		msg_format("���̖{�ɂ͊w�Ԃׂ�%s���Ȃ��B", p);
#else
		msg_format("You cannot learn any %ss in that book.", p);
#endif

		/* Abort */
		return;
	}


	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Learn the spell */
	p_ptr->spell_flags[spell] |= PY_SPELL_LEARNED;

	/* Find the next open entry in "spell_order[]" */
	for (i = 0; i < PY_MAX_SPELLS; i++)
	{
		/* Stop at the first empty space */
		if (p_ptr->spell_order[i] == 99) break;
	}

	/* Add the spell to the known list */
	p_ptr->spell_order[i] = spell;

	/* Mention the result */
#ifdef JP
	message_format(MSG_STUDY, 0, "%s��%s���w�񂾁B",
	           get_spell_name(cp_ptr->spell_book, spell), p);
#else /* JP */
	message_format(MSG_STUDY, 0, "You have learned the %s of %s.",
	           p, get_spell_name(cp_ptr->spell_book, spell));
#endif /* JP */

	/* One less spell available */
	p_ptr->new_spells--;

	/* Message if needed */
	if (p_ptr->new_spells)
	{
		/* Message */
#ifdef JP
		msg_format("�܂� %d ��%s���w�ׂ�B", p_ptr->new_spells, p);
#else
		msg_format("You can learn %d more %s%s.",
		           p_ptr->new_spells, p,
		           (p_ptr->new_spells != 1) ? "s" : "");
#endif
	}

	/* Redraw Study Status */
	p_ptr->redraw |= (PR_STUDY);

	/* Redraw object recall */
	p_ptr->window |= (PW_OBJECT);
}



/*
 * Cast a spell
 */
void do_cmd_cast(void)
{
	int item, spell;
	int chance;

	object_type *o_ptr;

	const magic_type *s_ptr;

	cptr q, s;


	/* Require spell ability */
	if (cp_ptr->spell_book != TV_MAGIC_BOOK)
	{
#ifdef JP
		msg_print("�������������Ȃ��I");
#else
		msg_print("You cannot cast spells!");
#endif
		return;
	}

	/* Require lite */
	if (p_ptr->blind || no_lite())
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��I");
#else
		msg_print("You cannot see!");
#endif
		return;
	}

	/* Not when confused */
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������ď������Ȃ��I");
#else
		msg_print("You are too confused!");
#endif
		return;
	}


	/* Restrict choices to spell books */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
#ifdef JP
	q = "�ǂ̎��������g���܂���? ";
	s = "���������Ȃ��I";
#else
	q = "Use which book? ";
	s = "You have no spell books!";
#endif
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Ask for a spell */
#ifdef JP
	spell = get_spell(o_ptr, "������", TRUE);
#else
	spell = get_spell(o_ptr, "cast", TRUE);
#endif

	if (spell < 0)
	{
#ifdef JP
		if (spell == -2) msg_print("���̎������ɂ͒m���Ă���������Ȃ��B");
#else /* JP */
		if (spell == -2) msg_print("You don't know any spells in that book.");
#endif /* JP */
		return;
	}


	/* Get the spell */
	s_ptr = &mp_ptr->info[spell];


	/* Verify "dangerous" spells */
	if (s_ptr->smana > p_ptr->csp)
	{
		/* Warning */
#ifdef JP
		msg_print("���̎�����������̂ɏ\���ȃ}�W�b�N�|�C���g���Ȃ��B");
#else
		msg_print("You do not have enough mana to cast this spell.");
#endif

		/* Flush input */
		flush();

		/* Verify */
#ifdef JP
		if (!get_check("����ł����킵�܂���? ")) return;
#else
		if (!get_check("Attempt it anyway? ")) return;
#endif
	}


	/* Spell failure chance */
	chance = spell_chance(spell);

	/* Failed spell */
	if (rand_int(100) < chance)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("������������̂Ɏ��s�����I");
#else
		msg_print("You failed to get the spell off!");
#endif
	}

	/* Process spell */
	else
	{
		/* Cast the spell */
		if (!cast_spell(cp_ptr->spell_book, spell)) return;

		/* A spell was cast */
		sound(MSG_SPELL);
		if (!(p_ptr->spell_flags[spell] & PY_SPELL_WORKED))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			p_ptr->spell_flags[spell] |= PY_SPELL_WORKED;

			/* Gain experience */
			gain_exp(e * s_ptr->slevel);

			/* Redraw object recall */
			p_ptr->window |= (PW_OBJECT);
		}
	}

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Sufficient mana */
	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
#ifdef JP
		msg_print("���_���W���������ċC�������Ă��܂����I");
#else
		msg_print("You faint from the effort!");
#endif

		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
#ifdef JP
			msg_print("�̂��������Ă��܂����I");
#else
			msg_print("You have damaged your health!");
#endif

			/* Reduce constitution */
			(void)dec_stat(A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
}


/*
 * Pray a prayer
 */
void do_cmd_pray(void)
{
	int item, spell, chance;

	object_type *o_ptr;

	const magic_type *s_ptr;

	cptr q, s;


	/* Must use prayer books */
	if (cp_ptr->spell_book != TV_PRAYER_BOOK)
	{
#ifdef JP
		msg_print("�S���珥����΁A�F�肪�_�ɓ͂����Ƃ�����B");
#else
		msg_print("Pray hard enough and your prayers may be answered.");
#endif
		return;
	}

	/* Must have lite */
	if (p_ptr->blind || no_lite())
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��I");
#else
		msg_print("You cannot see!");
#endif
		return;
	}

	/* Must not be confused */
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������ď������Ȃ��I");
#else
		msg_print("You are too confused!");
#endif
		return;
	}


	/* Restrict choices */
	item_tester_tval = cp_ptr->spell_book;

	/* Get an item */
#ifdef JP
	q = "�ǂ̋F�������g���܂���? ";
	s = "�F�������Ȃ��I";
#else
	q = "Use which book? ";
	s = "You have no prayer books!";
#endif
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Choose a spell */
#ifdef JP
	spell = get_spell(o_ptr, "������", TRUE);
#else /* JP */
	spell = get_spell(o_ptr, "recite", TRUE);
#endif

	if (spell < 0)
	{
#ifdef JP
		if (spell == -2) msg_print("���̋F�����ɂ͏�������F�肪�Ȃ��B");
#else /* JP */
		if (spell == -2) msg_print("You don't know any prayers in that book.");
#endif
		return;
	}


	/* Get the spell */
	s_ptr = &mp_ptr->info[spell];


	/* Verify "dangerous" prayers */
	if (s_ptr->smana > p_ptr->csp)
	{
		/* Warning */
#ifdef JP
		msg_print("���̋F���������̂ɏ\���ȃ}�W�b�N�|�C���g���Ȃ��B");
#else
		msg_print("You do not have enough mana to recite this prayer.");
#endif

		/* Flush input */
		flush();

		/* Verify */
#ifdef JP
		if (!get_check("����ł����킵�܂���? ")) return;
#else
		if (!get_check("Attempt it anyway? ")) return;
#endif
	}


	/* Spell failure chance */
	chance = spell_chance(spell);

	/* Check for failure */
	if (rand_int(100) < chance)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���_�̏W���Ɏ��s�����I");
#else
		msg_print("You failed to concentrate hard enough!");
#endif
	}

	/* Success */
	else
	{
		/* Cast the spell */
		if (!cast_spell(cp_ptr->spell_book, spell)) return;

		/* A prayer was prayed */
		sound(MSG_PRAYER);
		if (!(p_ptr->spell_flags[spell] & PY_SPELL_WORKED))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			p_ptr->spell_flags[spell] |= PY_SPELL_WORKED;

			/* Gain experience */
			gain_exp(e * s_ptr->slevel);

			/* Redraw object recall */
			p_ptr->window |= (PW_OBJECT);
		}
	}

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Sufficient mana */
	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
#ifdef JP
		msg_print("���_���W���������ċC�������Ă��܂����I");
#else
		msg_print("You faint from the effort!");
#endif

		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
#ifdef JP
			msg_print("�̂��������Ă��܂����I");
#else
			msg_print("You have damaged your health!");
#endif

			/* Reduce constitution */
			(void)dec_stat(A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
}
