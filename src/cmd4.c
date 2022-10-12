/* File: cmd4.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/*
 * 2.7.9v3 日本語版製作: しとしん
 * 2.7.9v6 対応        : 岸康司, FIRST, しとしん
 * 2.8.0   対応        : sayu, しとしん
 * 2.8.1   対応        : FIRST
 * 2.8.3   対応        : FIRST, しとしん
 * 2.9.0   対応        : 楠瀬
 *
 * 日本語版機能追加 : 英日切り替え機能
 */

#include "angband.h"


/*
 *  Header and footer marker string for pref file dumps
 */
static cptr dump_seperator = "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#";


/*
 * Remove old lines from pref files
 */
static void remove_old_dump(cptr orig_file, cptr mark)
{
	FILE *tmp_fff, *orig_fff;

	char tmp_file[1024];
	char buf[1024];
	bool between_marks = FALSE;
	bool changed = FALSE;
	char expected_line[1024];

	/* Open an old dump file in read-only mode */
	orig_fff = my_fopen(orig_file, "r");

	/* If original file does not exist, nothing to do */
	if (!orig_fff) return;

	/* Open a new temporary file */
	tmp_fff = my_fopen_temp(tmp_file, sizeof(tmp_file));

	if (!tmp_fff)
	{
	    msg_format("Failed to create temporary file %s.", tmp_file);
	    msg_print(NULL);
	    return;
	}

	strnfmt(expected_line, sizeof(expected_line),
	        "%s begin %s", dump_seperator, mark);

	/* Loop for every line */
	while (TRUE)
	{
		/* Read a line */
		if (my_fgets(orig_fff, buf, sizeof(buf)))
		{
			/* End of file but no end marker */
			if (between_marks) changed = FALSE;

			break;
		}

		/* Is this line a header/footer? */
		if (strncmp(buf, dump_seperator, strlen(dump_seperator)) == 0)
		{
			/* Found the expected line? */
			if (strcmp(buf, expected_line) == 0)
			{
				if (!between_marks)
				{
					/* Expect the footer next */
					strnfmt(expected_line, sizeof(expected_line),
					        "%s end %s", dump_seperator, mark);

					between_marks = TRUE;

					/* There are some changes */
					changed = TRUE;
				}
				else
				{
					/* Expect a header next - XXX shouldn't happen */
					strnfmt(expected_line, sizeof(expected_line),
					        "%s begin %s", dump_seperator, mark);

					between_marks = FALSE;

					/* Next line */
					continue;
				}
			}
			/* Found a different line */
			else
			{
				/* Expected a footer and got something different? */
				if (between_marks)
				{
					/* Abort */
					changed = FALSE;
					break;
				}
			}
		}

		if (!between_marks)
		{
			/* Copy orginal line */
			fprintf(tmp_fff, "%s\n", buf);
		}
	}

	/* Close files */
	my_fclose(orig_fff);
	my_fclose(tmp_fff);

	/* If there are changes, overwrite the original file with the new one */
	if (changed)
	{
		/* Copy contents of temporary file */
		tmp_fff = my_fopen(tmp_file, "r");
		orig_fff = my_fopen(orig_file, "w");

		while (!my_fgets(tmp_fff, buf, sizeof(buf)))
		{
			fprintf(orig_fff, "%s\n", buf);
		}

		my_fclose(orig_fff);
		my_fclose(tmp_fff);
	}

	/* Kill the temporary file */
	fd_kill(tmp_file);
}


/*
 * Output the header of a pref-file dump
 */
static void pref_header(FILE *fff, cptr mark)
{
	/* Start of dump */
	fprintf(fff, "%s begin %s\n", dump_seperator, mark);

	fprintf(fff, "# *Warning!*  The lines below are an automatic dump.\n");
	fprintf(fff, "# Don't edit them; changes will be deleted and replaced automatically.\n");
}


/*
 * Output the footer of a pref-file dump
 */
static void pref_footer(FILE *fff, cptr mark)
{
	fprintf(fff, "# *Warning!*  The lines above are an automatic dump.\n");
	fprintf(fff, "# Don't edit them; changes will be deleted and replaced automatically.\n");

	/* End of dump */
	fprintf(fff, "%s end %s\n", dump_seperator, mark);
}


/*
 * Hack -- redraw the screen
 *
 * This command performs various low level updates, clears all the "extra"
 * windows, does a total redraw of the main window, and requests all of the
 * interesting updates and redraws that I can think of.
 *
 * This command is also used to "instantiate" the results of the user
 * selecting various things, such as graphics mode, so it must call
 * the "TERM_XTRA_REACT" hook before redrawing the windows.
 */
void do_cmd_redraw(void)
{
	int j;

	term *old = Term;


	/* Low level flush */
	Term_flush();

	/* Reset "inkey()" */
	flush();


	/* Hack -- React to changes */
	Term_xtra(TERM_XTRA_REACT, 0);


	/* Combine and Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);


	/* Update torch */
	p_ptr->update |= (PU_TORCH);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Redraw everything */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1 |
	                  PW_MESSAGE | PW_OVERHEAD | PW_MONSTER | PW_OBJECT |
	                  PW_MAP | PW_MONLIST);

	/* Clear screen */
	Term_clear();

	/* Hack -- update */
	handle_stuff();


	/* Redraw every window */
	for (j = 0; j < ANGBAND_TERM_MAX; j++)
	{
		/* Dead window */
		if (!angband_term[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw */
		Term_redraw();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- change name
 */
void do_cmd_change_name(void)
{
	char ch;

	int mode = 0;

	cptr p;

	/* Prompt */
#ifdef JP
	p = "['c'で名前変更, 'f'でファイルへ書出, 'h'でモード変更, ESCで終了]";
#else /* JP */
	p = "['c' to change name, 'f' to file, 'h' to change mode, or ESC]";
#endif /* JP */

	/* Save screen */
	screen_save();

	/* Forever */
	while (1)
	{
		/* Display the player */
		display_player(mode);

		/* Prompt */
		Term_putstr(2, 23, -1, TERM_WHITE, p);

		/* Query */
		ch = inkey();

		/* Exit */
		if (ch == ESCAPE) break;

		/* Change name */
		if (ch == 'c')
		{
			get_name();
		}

		/* File dump */
		else if (ch == 'f')
		{
			char ftmp[80];

			strnfmt(ftmp, sizeof(ftmp), "%s.txt", op_ptr->base_name);

#ifdef JP
			if (get_string("ファイル名: ", ftmp, sizeof(ftmp)))
#else /* JP */
			if (get_string("File name: ", ftmp, sizeof(ftmp)))
#endif /* JP */
			{
				if (ftmp[0] && (ftmp[0] != ' '))
				{
					if (file_character(ftmp, FALSE))
					{
#ifdef JP
						msg_print("キャラクタ情報のファイルへの書き出しに失敗しました！");
#else /* JP */
						msg_print("Character dump failed!");
#endif /* JP */
					}
					else
					{
#ifdef JP
						msg_print("キャラクタ情報のファイルへの書き出しに成功しました。");
#else /* JP */
						msg_print("Character dump successful.");
#endif /* JP */
					}
				}
			}
		}

		/* Toggle mode */
		else if (ch == 'h')
		{
			mode = !mode;
		}

		/* Oops */
		else
		{
#ifdef JP
			bell("無効なキーです！");
#else /* JP */
			bell("Illegal command for change name!");
#endif /* JP */
		}

		/* Flush messages */
		message_flush();
	}

	/* Load screen */
	screen_load();
}


/*
 * Recall the most recent message
 */
void do_cmd_message_one(void)
{
	/* Recall one message XXX XXX XXX */
	c_prt(message_color(0), format( "> %s", message_str(0)), 0, 0);
}


/*
 * Show previous messages to the user
 *
 * The screen format uses line 0 and 23 for headers and prompts,
 * skips line 1 and 22, and uses line 2 thru 21 for old messages.
 *
 * This command shows you which commands you are viewing, and allows
 * you to "search" for strings in the recall.
 *
 * Note that messages may be longer than 80 characters, but they are
 * displayed using "infinite" length, with a special sub-command to
 * "slide" the virtual display to the left or right.
 *
 * Attempt to only hilite the matching portions of the string.
 */
void do_cmd_messages(void)
{
	char ch;

	int i, j, n, q;
	int wid, hgt;

	char shower[80];
	char finder[80];


	/* Wipe finder */
	strcpy(finder, "");

	/* Wipe shower */
	strcpy(shower, "");


	/* Total messages */
	n = message_num();

	/* Start on first message */
	i = 0;

	/* Start at leftmost edge */
	q = 0;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Save screen */
	screen_save();

	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Dump messages */
		for (j = 0; (j < hgt - 4) && (i + j < n); j++)
		{
			cptr msg = message_str((s16b)(i+j));
			byte attr = message_color((s16b)(i+j));

			/* Apply horizontal scroll */
			msg = ((int)strlen(msg) >= q) ? (msg + q) : "";

			/* Dump the messages, bottom to top */
			Term_putstr(0, hgt - 3 - j, -1, attr, msg);

			/* Hilite "shower" */
			if (shower[0])
			{
				cptr str = msg;

				/* Display matches */
				while ((str = strstr(str, shower)) != NULL)
				{
					int len = strlen(shower);

					/* Display the match */
					Term_putstr(str-msg, hgt - 3 - j, len, TERM_YELLOW, shower);

					/* Advance */
					str += len;
				}
			}
		}

		/* Display header XXX XXX XXX */
#ifdef JP
		/* translation */
		prt(format("以前のメッセージ %d-%d 全部で(%d) オフセット(%d)",
		           i, i + j - 1, n, q), 0, 0);
#else /* JP */
		prt(format("Message Recall (%d-%d of %d), Offset %d",
		           i, i + j - 1, n, q), 0, 0);
#endif /* JP */

		/* Display prompt (not very informative) */
#ifdef JP
		prt("[ 'p' で更に古いもの, 'n' で更に新しいもの, '/' で検索, ESC で中断 ]", hgt - 1, 0);
#else /* JP */
		prt("[Press 'p' for older, 'n' for newer, ..., or ESCAPE]", hgt - 1, 0);
#endif /* JP */

		/* Get a command */
		ch = inkey();

		/* Exit on Escape */
		if (ch == ESCAPE) break;

		/* Hack -- Save the old index */
		j = i;

		/* Horizontal scroll */
		if (ch == '4')
		{
			/* Scroll left */
			q = (q >= wid / 2) ? (q - wid / 2) : 0;

			/* Success */
			continue;
		}

		/* Horizontal scroll */
		if (ch == '6')
		{
			/* Scroll right */
			q = q + wid / 2;

			/* Success */
			continue;
		}

		/* Hack -- handle show */
		if (ch == '=')
		{
			/* Prompt */
#ifdef JP
			prt("強調: ", hgt - 1, 0);
#else /* JP */
			prt("Show: ", hgt - 1, 0);
#endif /* JP */

			/* Get a "shower" string, or continue */
			if (!askfor_aux(shower, sizeof(shower))) continue;

			/* Okay */
			continue;
		}

		/* Hack -- handle find */
		if (ch == '/')
		{
			s16b z;

			/* Prompt */
#ifdef JP
			prt("検索: ", hgt - 1, 0);
#else /* JP */
			prt("Find: ", hgt - 1, 0);
#endif /* JP */

			/* Get a "finder" string, or continue */
			if (!askfor_aux(finder, sizeof(finder))) continue;

			/* Show it */
			my_strcpy(shower, finder, sizeof(shower));

			/* Scan messages */
			for (z = i + 1; z < n; z++)
			{
				cptr msg = message_str(z);

				/* Search for it */
				if (strstr(msg, finder))
				{
					/* New location */
					i = z;

					/* Done */
					break;
				}
			}
		}

		/* Recall 20 older messages */
		if ((ch == 'p') || (ch == KTRL('P')) || (ch == ' '))
		{
			/* Go older if legal */
			if (i + 20 < n) i += 20;
		}

		/* Recall 10 older messages */
		if (ch == '+')
		{
			/* Go older if legal */
			if (i + 10 < n) i += 10;
		}

		/* Recall 1 older message */
		if ((ch == '8') || (ch == '\n') || (ch == '\r'))
		{
			/* Go newer if legal */
			if (i + 1 < n) i += 1;
		}

		/* Recall 20 newer messages */
		if ((ch == 'n') || (ch == KTRL('N')))
		{
			/* Go newer (if able) */
			i = (i >= 20) ? (i - 20) : 0;
		}

		/* Recall 10 newer messages */
		if (ch == '-')
		{
			/* Go newer (if able) */
			i = (i >= 10) ? (i - 10) : 0;
		}

		/* Recall 1 newer messages */
		if (ch == '2')
		{
			/* Go newer (if able) */
			i = (i >= 1) ? (i - 1) : 0;
		}

		/* Hack -- Error of some kind */
		if (i == j) bell(NULL);
	}

	/* Load screen */
	screen_load();
}



/*
 * Ask for a "user pref line" and process it
 */
void do_cmd_pref(void)
{
	char tmp[80];

	/* Default */
	strcpy(tmp, "");

	/* Ask for a "user pref command" */
#ifdef JP
	if (!get_string("設定変更コマンド: ", tmp, sizeof(tmp))) return;
#else /* JP */
	if (!get_string("Pref: ", tmp, sizeof(tmp))) return;
#endif /* JP */

	/* Process that pref command */
	(void)process_pref_file_command(tmp);
}

#ifdef ALLOW_AUTO_PICKUP

/*
 * Process 'autopick pref file'
 */
void do_cmd_pickpref(void)
{
	char buf[80];

	errr err = -1; 


#ifdef JP
	if(!get_check("自動拾い設定ファイルをロードしますか? ")) return;
#else /* JP */
	if(!get_check("Reload auto-pick preference file? ")) return;
#endif /* JP */

	init_autopicker();

	/* Access character's "autopick" pref file */
	sprintf(buf, "%s.prf", op_ptr->base_name);

	/* Process that file */
	err = process_autopick_pref_file(buf);
	if (err > 0)
	{
#ifdef JP
		msg_format("自動拾い設定ファイル'%s'の読み込みに失敗しました。", buf);
#else /* JP */
		msg_format("Failed to reload autopick pref file '%s'.", buf);
#endif /* JP */
	}
	else if (err == 0)
	{
#ifdef JP
		msg_format("%sを読み込みました。", buf);
#else /* JP */
		msg_format("loaded '%s'.", buf);
#endif /* JP */
		return;
	}

	/* Process "autopick" pref file */
	strcpy(buf, "autopick.prf");
	err = process_autopick_pref_file(buf);
	if (err > 0)
	{
#ifdef JP
		msg_format("自動拾い設定ファイル'%s'の読み込みに失敗しました。", buf);
#else /* JP */
		msg_format("Failed to reload autopick pref file '%s'.", buf);
#endif /* JP */
	}
	else if (err == 0)
	{
#ifdef JP
		msg_format("%sを読み込みました。", buf);
#else /* JP */
		msg_format("loaded '%s'.", buf);
#endif /* JP */
		return;
	}
}

#endif /* ALLOW_AUTO_PICKUP */


/*
 * Ask for a "user pref file" and process it.
 *
 * This function should only be used by standard interaction commands,
 * in which a standard "Command:" prompt is present on the given row.
 *
 * Allow absolute file names?  XXX XXX XXX
 */
static void do_cmd_pref_file_hack(int row)
{
	char ftmp[80];

	/* Prompt */
#ifdef JP
	prt("コマンド: ユーザー設定ファイルをロードします", row, 0);
#else /* JP */
	prt("Command: Load a user pref file", row, 0);
#endif /* JP */

	/* Prompt */
#ifdef JP
	prt("ファイル: ", row + 2, 0);
#else /* JP */
	prt("File: ", row + 2, 0);
#endif /* JP */

	/* Default filename */
	strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

	/* Ask for a file (or cancel) */
	if (!askfor_aux(ftmp, sizeof(ftmp))) return;

	/* Process the given filename */
	if (process_pref_file(ftmp))
	{
		/* Mention failure */
#ifdef JP
		msg_format("'%s'のロードに失敗しました。", ftmp);
#else /* JP */
		msg_format("Failed to load '%s'!", ftmp);
#endif /* JP */
	}
	else
	{
		/* Mention success */
#ifdef JP
		msg_format("'%s'をロードしました", ftmp);
#else /* JP */
		msg_format("Loaded '%s'.", ftmp);
#endif /* JP */
	}
}



/*
 * Interact with some options
 */
static void do_cmd_options_aux(int page, cptr info)
{
	char ch;

	int i, k = 0, n = 0;

	int opt[OPT_PAGE_PER];

	char buf[80];

	int dir;


	/* Scan the options */
	for (i = 0; i < OPT_PAGE_PER; i++)
	{
		/* Collect options on this "page" */
		if (option_page[page][i] != OPT_NONE)
		{
			opt[n++] = option_page[page][i];
		}
	}


	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		strnfmt(buf, sizeof(buf), "%s (リターンで次へ, y/n でセット, ESC で決定, ? でヘルプ) ", info);
#else /* JP */
		strnfmt(buf, sizeof(buf), "%s (RET to advance, y/n to set, ESC to accept, ? for help) ", info);
#endif /* JP */
		prt(buf, 0, 0);

#ifdef JP
		/* 詐欺オプションをうっかりいじってしまう人がいるようなので注意 */
		if (page == 6)
		{
			prt("                                 <<  注意  >>", 11, 0);
			prt("      詐欺オプションを一度でも設定すると、スコア記録が残らなくなります！", 12, 0);
			prt("      後に解除してもダメですので、勝利者を目指す方はここのオプションはい", 13, 0);
			prt("      じらないようにして下さい。", 14, 0);
		}
#endif /* JP */

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			strnfmt(buf, sizeof(buf), "%-48s: %s  (%s)",
			        option_desc[opt[i]],
#ifdef JP
			        op_ptr->opt[opt[i]] ? "Yes" : "No ",
#else /* JP */
			        op_ptr->opt[opt[i]] ? "yes" : "no ",
#endif /* JP */
			        option_text[opt[i]]);
			c_prt(a, buf, i + 2, 0);
		}

		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/*
		 * HACK - Try to translate the key into a direction
		 * to allow using the roguelike keys for navigation.
		 */
		dir = target_dir(ch);
		if ((dir == 2) || (dir == 4) || (dir == 6) || (dir == 8))
			ch = I2D(dir);

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				/* Hack -- Notice use of any "cheat" options */
				for (i = OPT_CHEAT; i < OPT_ADULT; i++)
				{
					if (op_ptr->opt[i])
					{
						/* Set score option */
						op_ptr->opt[OPT_SCORE + (i - OPT_CHEAT)] = TRUE;
					}
				}

				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 't':
			case '5':
			{
				op_ptr->opt[opt[k]] = !op_ptr->opt[opt[k]];
				break;
			}

			case 'y':
			case '6':
			{
				op_ptr->opt[opt[k]] = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case '4':
			{
				op_ptr->opt[opt[k]] = FALSE;
				k = (k + 1) % n;
				break;
			}

			case '?':
			{
				strnfmt(buf, sizeof(buf), "option.txt#%s", option_text[opt[k]]);
				show_file(buf, NULL, 0, 0);
				Term_clear();
				break;
			}

			default:
			{
#ifdef JP
				bell("無効なキーです！");
#else /* JP */
				bell("Illegal command for normal options!");
#endif /* JP */
				break;
			}
		}
	}
}


/*
 * Modify the "window" options
 */
static void do_cmd_options_win(void)
{
	int i, j, d;

	int y = 0;
	int x = 0;

	char ch;

	u32b old_flag[ANGBAND_TERM_MAX];


	/* Memorize old flags */
	for (j = 0; j < ANGBAND_TERM_MAX; j++)
	{
		old_flag[j] = op_ptr->window_flag[j];
	}


	/* Clear screen */
	Term_clear();

	/* Interact */
	while (1)
	{
		/* Prompt */
#ifdef JP
		prt("ウィンドウ・フラグ (<方向>で移動, tでチェンジ, ESC)", 0, 0);
#else /* JP */
		prt("Window flags (<dir> to move, 't' to toggle, or ESC)", 0, 0);
#endif /* JP */

		/* Display the windows */
		for (j = 0; j < ANGBAND_TERM_MAX; j++)
		{
			byte a = TERM_WHITE;

			cptr s = angband_term_name[j];

			/* Use color */
			if (j == x) a = TERM_L_BLUE;

			/* Window name, staggered, centered */
			Term_putstr(35 + j * 5 - strlen(s) / 2, 2 + j % 2, -1, a, s);
		}

		/* Display the options */
		for (i = 0; i < 16; i++)
		{
			byte a = TERM_WHITE;

			cptr str = window_flag_desc[i];

			/* Use color */
			if (i == y) a = TERM_L_BLUE;

			/* Unused option */
#ifdef JP
			if (!str) str = "(未使用)";
#else /* JP */
			if (!str) str = "(Unused option)";
#endif /* JP */

			/* Flag name */
			Term_putstr(0, i + 5, -1, a, str);

			/* Display the windows */
			for (j = 0; j < ANGBAND_TERM_MAX; j++)
			{
				byte a = TERM_WHITE;

				char c = '.';

				/* Use color */
				if ((i == y) && (j == x)) a = TERM_L_BLUE;

				/* Active flag */
				if (op_ptr->window_flag[j] & (1L << i)) c = 'X';

				/* Flag value */
				Term_putch(35 + j * 5, i + 5, a, c);
			}
		}

		/* Place Cursor */
		Term_gotoxy(35 + x * 5, y + 5);

		/* Get key */
		ch = inkey();

		/* Allow escape */
		if ((ch == ESCAPE) || (ch == 'q')) break;

		/* Toggle */
		if ((ch == '5') || (ch == 't'))
		{
			/* Hack -- ignore the main window */
			if (x == 0)
			{
#ifdef JP
				bell("メイン・ウィンドウ・フラグはセットできません！");
#else /* JP */
				bell("Cannot set main window flags!");
#endif /* JP */
			}

			/* Toggle flag (off) */
			else if (op_ptr->window_flag[x] & (1L << y))
			{
				op_ptr->window_flag[x] &= ~(1L << y);
			}

			/* Toggle flag (on) */
			else
			{
				op_ptr->window_flag[x] |= (1L << y);
			}

			/* Continue */
			continue;
		}

		/* Extract direction */
		d = target_dir(ch);

		/* Move */
		if (d != 0)
		{
			x = (x + ddx[d] + 8) % 8;
			y = (y + ddy[d] + 16) % 16;
		}

		/* Oops */
		else
		{
#ifdef JP
			bell("無効なキーです！");
#else /* JP */
			bell("Illegal command for window options!");
#endif /* JP */
		}
	}

	/* Notice changes */
	for (j = 0; j < ANGBAND_TERM_MAX; j++)
	{
		term *old = Term;

		/* Dead window */
		if (!angband_term[j]) continue;

		/* Ignore non-changes */
		if (op_ptr->window_flag[j] == old_flag[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Erase */
		Term_clear();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Write all current options to the given preference file in the
 * lib/user directory. Modified from KAmband 1.8.
 */
static errr option_dump(cptr fname)
{
	static cptr mark = "Options Dump";

	int i, j;

	FILE *fff;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Remove old options */
	remove_old_dump(buf, mark);

	/* Append to the file */
	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff) return (-1);

	/* Output header */
	pref_header(fff, mark);

	/* Skip some lines */
	fprintf(fff, "\n\n");

	/* Start dumping */
#ifdef JP
	fprintf(fff, "# 自動オプションセーブ\n\n");
#else /* JP */
	fprintf(fff, "# Automatic option dump\n\n");
#endif /* JP */

	/* Dump options (skip cheat, adult, score) */
	for (i = 0; i < OPT_CHEAT; i++)
	{
		/* Require a real option */
		if (!option_text[i]) continue;

		/* Comment */
#ifdef JP
		fprintf(fff, "# オプション '%s'\n", option_desc[i]);
#else /* JP */
		fprintf(fff, "# Option '%s'\n", option_desc[i]);
#endif /* JP */

		/* Dump the option */
		if (op_ptr->opt[i])
		{
			fprintf(fff, "Y:%s\n", option_text[i]);
		}
		else
		{
			fprintf(fff, "X:%s\n", option_text[i]);
		}

		/* Skip a line */
		fprintf(fff, "\n");
	}

	/* Dump window flags */
	for (i = 1; i < ANGBAND_TERM_MAX; i++)
	{
		/* Require a real window */
		if (!angband_term[i]) continue;

		/* Check each flag */
		for (j = 0; j < 32; j++)
		{
			/* Require a real flag */
			if (!window_flag_desc[j]) continue;

			/* Comment */
#ifdef JP
			fprintf(fff, "# ウィンドウ '%s', フラグ '%s'\n",
			        angband_term_name[i], window_flag_desc[j]);
#else /* JP */
			fprintf(fff, "# Window '%s', Flag '%s'\n",
			        angband_term_name[i], window_flag_desc[j]);
#endif /* JP */

			/* Dump the flag */
			if (op_ptr->window_flag[i] & (1L << j))
			{
				fprintf(fff, "W:%d:%d:1\n", i, j);
			}
			else
			{
				fprintf(fff, "W:%d:%d:0\n", i, j);
			}

			/* Skip a line */
			fprintf(fff, "\n");
		}
	}

	/* Output footer */
	pref_footer(fff, mark);

	/* Close */
	my_fclose(fff);

	/* Success */
	return (0);
}


/*
 * Set or unset various options.
 *
 * After using this command, a complete redraw should be performed,
 * in case any visual options have been changed.
 */
void do_cmd_options(void)
{
	char ch;


	/* Save screen */
	screen_save();


	/* Interact */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Why are we here */
#ifdef JP
		prt(format("[ %sオプションの設定 ]", JVERSION_NAME), 2, 0);
#else /* JP */
		prt(format("%s options", VERSION_NAME), 2, 0);
#endif /* JP */

		/* Give some choices */
#ifdef JP
		prt("(1) インターフェイス オプション", 4, 5);
		prt("(2)   行動中止関係   オプション", 5, 5);
		prt("(3)   ゲームプレイ   オプション", 6, 5);
		prt("(4)      効率化      オプション", 7, 5);
		prt("(5)       表示       オプション", 8, 5);
		prt("(6)       初期       オプション", 9, 5);
		prt("(7)       詐欺       オプション", 10, 5);
#else /* JP */
		prt("(1) User Interface Options", 4, 5);
		prt("(2) Disturbance Options", 5, 5);
		prt("(3) Game-Play Options", 6, 5);
		prt("(4) Efficiency Options", 7, 5);
		prt("(5) Display Options", 8, 5);
		prt("(6) Birth Options", 9, 5);
		prt("(7) Cheat Options", 10, 5);
#endif /* JP */

#ifdef JP
		/* 日本語版で追加されたオプション */
		prt("(J)   日本語版特別   オプション", 11, 5);
#endif /* JP */

		/* Window flags */
#ifdef JP
		prt("(W) ウインドウフラグ", 12, 5);
#else /* JP */
		prt("(W) Window flags", 12, 5);
#endif /* JP */

		/* Load and Append */
#ifdef JP
		prt("(L) ユーザー設定ファイルのロード", 14, 5);
		prt("(A) オプションをファイルに追加する", 15, 5);
#else /* JP */
		prt("(L) Load a user pref file", 14, 5);
		prt("(A) Append options to a file", 15, 5);
#endif /* JP */

		/* Special choices */
#ifdef JP
		prt("(D) 基本ウェイト量", 17, 5);
		prt("(H) 低ヒットポイント警告", 18, 5);
#else /* JP */
		prt("(D) Base Delay Factor", 17, 5);
		prt("(H) Hitpoint Warning", 18, 5);
#endif /* JP */

		/* Prompt */
#ifdef JP
		prt("コマンド:", 20, 0);
#else /* JP */
		prt("Command: ", 20, 0);
#endif /* JP */

		/* Get command */
		ch = inkey();

		/* Exit */
		if (ch == ESCAPE) break;

		/* General Options */
		else if (ch == '1')
		{
#ifdef JP
			do_cmd_options_aux(0, "インターフェイス・オプション");
#else /* JP */
			do_cmd_options_aux(0, "User Interface Options");
#endif /* JP */
		}

		/* Disturbance Options */
		else if (ch == '2')
		{
#ifdef JP
			do_cmd_options_aux(1, "行動中止関係のオプション");
#else /* JP */
			do_cmd_options_aux(1, "Disturbance Options");
#endif /* JP */
		}

		/* Inventory Options */
		else if (ch == '3')
		{
#ifdef JP
			do_cmd_options_aux(2, "ゲームプレイ・オプション");
#else /* JP */
			do_cmd_options_aux(2, "Game-Play Options");
#endif /* JP */
		}

		/* Efficiency Options */
		else if (ch == '4')
		{
#ifdef JP
			do_cmd_options_aux(3, "効率化オプション");
#else /* JP */
			do_cmd_options_aux(3, "Efficiency Options");
#endif /* JP */
		}

		/* Display Options */
		else if (ch == '5')
		{
#ifdef JP
			do_cmd_options_aux(4, "表示オプション");
#else /* JP */
			do_cmd_options_aux(4, "Display Options");
#endif /* JP */
		}

		/* Birth Options */
		else if (ch == '6')
		{
#ifdef JP
			do_cmd_options_aux(5, "初期オプション");
#else /* JP */
			do_cmd_options_aux(5, "Birth Options");
#endif /* JP */
		}

		/* Cheating Options */
		else if (ch == '7')
		{
#ifdef JP
			do_cmd_options_aux(6, "詐欺オプション");
#else /* JP */
			do_cmd_options_aux(6, "Cheat Options");
#endif /* JP */
		}

#ifdef JP
		/* 日本語版特別オプション */
		else if ((ch == 'J') || (ch == 'j'))
		{
			do_cmd_options_aux(7, "日本語版特別オプション");
		}
#endif /* JP */

		/* Window flags */
		else if ((ch == 'W') || (ch == 'w'))
		{
			do_cmd_options_win();
		}

		/* Load a user pref file */
		else if ((ch == 'L') || (ch == 'l'))
		{
			/* Ask for and load a user pref file */
			do_cmd_pref_file_hack(20);
		}

		/* Append options to a file */
		else if ((ch == 'A') || (ch == 'a'))
		{
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: オプションをファイルに追加する", 20, 0);
#else /* JP */
			prt("Command: Append options to a file", 20, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 21, 0);
#else /* JP */
			prt("File: ", 21, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Ask for a file */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Dump the options */
			if (option_dump(ftmp))
			{
				/* Failure */
#ifdef JP
				msg_print("失敗!");
#else /* JP */
				msg_print("Failed!");
#endif /* JP */
			}
			else
			{
				/* Success */
#ifdef JP
				msg_print("完了。");
#else /* JP */
				msg_print("Done.");
#endif /* JP */
			}
		}

		/* Hack -- Base Delay Factor */
		else if ((ch == 'D') || (ch == 'd'))
		{
			/* Prompt */
#ifdef JP
			prt("コマンド: 基本ウェイト量", 20, 0);
#else /* JP */
			prt("Command: Base Delay Factor", 20, 0);
#endif /* JP */

			/* Get a new value */
			while (1)
			{
				char cx;
				int msec = op_ptr->delay_factor * op_ptr->delay_factor;
#ifdef JP
				prt(format("現在のウェイト: %d (%dミリ秒)",
				           op_ptr->delay_factor, msec), 22, 0);
				prt("ウェイト (0-9) ESCで決定: ", 21, 0);
#else /* JP */
				prt(format("Current base delay factor: %d (%d msec)",
				           op_ptr->delay_factor, msec), 22, 0);
				prt("New base delay factor (0-9 or ESC to accept): ", 21, 0);
#endif /* JP */

				cx = inkey();
				if (cx == ESCAPE) break;
				if (isdigit((unsigned char)cx)) op_ptr->delay_factor = D2I(cx);
#ifdef JP
				else bell("無効なキーです！");
#else /* JP */
				else bell("Illegal delay factor!");
#endif /* JP */
			}
		}

		/* Hack -- hitpoint warning factor */
		else if ((ch == 'H') || (ch == 'h'))
		{
			/* Prompt */
#ifdef JP
			prt("コマンド: 低ヒットポイント警告", 20, 0);
#else /* JP */
			prt("Command: Hitpoint Warning", 20, 0);
#endif /* JP */

			/* Get a new value */
			while (1)
			{
				char cx;
#ifdef JP
				prt(format("現在の低ヒットポイント警告: %2d%%",
  				           op_ptr->hitpoint_warn * 10), 22, 0);
				prt("低ヒットポイント警告 (0-9) ESCで決定: ", 21, 0);
#else /* JP */
				prt(format("Current hitpoint warning: %2d%%",
				           op_ptr->hitpoint_warn * 10), 22, 0);
				prt("New hitpoint warning (0-9 or ESC to accept): ", 21, 0);
#endif /* JP */

				cx = inkey();
				if (cx == ESCAPE) break;
				if (isdigit((unsigned char)cx)) op_ptr->hitpoint_warn = D2I(cx);
#ifdef JP
				else bell("無効なキーです！");
#else /* JP */
				else bell("Illegal hitpoint warning!");
#endif /* JP */
			}
		}

		/* Unknown option */
		else
		{
			/* Oops */
#ifdef JP
			bell("無効なキーです！");
#else /* JP */
			bell("Illegal command for options!");
#endif /* JP */
		}

		/* Flush messages */
		message_flush();
	}


	/* Load screen */
	screen_load();
}



#ifdef ALLOW_MACROS

/*
 * Hack -- append all current macros to the given file
 */
static errr macro_dump(cptr fname)
{
	static cptr mark = "Macro Dump";

	int i;

	FILE *fff;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Remove old macros */
	remove_old_dump(buf, mark);

	/* Append to the file */
	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff) return (-1);

	/* Output header */
	pref_header(fff, mark);

	/* Skip some lines */
	fprintf(fff, "\n\n");

	/* Start dumping */
#ifdef JP
	fprintf(fff, "# 自動マクロセーブ\n\n");
#else /* JP */
	fprintf(fff, "# Automatic macro dump\n\n");
#endif /* JP */

	/* Dump them */
	for (i = 0; i < macro__num; i++)
	{
		/* Start the macro */
#ifdef JP
		fprintf(fff, "# マクロ '%d'\n\n", i);
#else /* JP */
		fprintf(fff, "# Macro '%d'\n\n", i);
#endif /* JP */

		/* Extract the macro action */
		ascii_to_text(buf, sizeof(buf), macro__act[i]);

		/* Dump the macro action */
		fprintf(fff, "A:%s\n", buf);

		/* Extract the macro pattern */
		ascii_to_text(buf, sizeof(buf), macro__pat[i]);

		/* Dump the macro pattern */
		fprintf(fff, "P:%s\n", buf);

		/* End the macro */
		fprintf(fff, "\n\n");
	}

	/* Start dumping */
	fprintf(fff, "\n\n\n\n");

	/* Output footer */
	pref_footer(fff, mark);

	/* Close */
	my_fclose(fff);

	/* Success */
	return (0);
}


/*
 * Hack -- ask for a "trigger" (see below)
 *
 * Note the complex use of the "inkey()" function from "util.c".
 *
 * Note that both "flush()" calls are extremely important.  This may
 * no longer be true, since "util.c" is much simpler now.  XXX XXX XXX
 */
static void do_cmd_macro_aux(char *buf)
{
	char ch;

	int n = 0;

	char tmp[1024];


	/* Flush */
	flush();

	/* Do not process macros */
	inkey_base = TRUE;

	/* First key */
	ch = inkey();

	/* Read the pattern */
	while (ch != '\0')
	{
		/* Save the key */
		buf[n++] = ch;

		/* Do not process macros */
		inkey_base = TRUE;

		/* Do not wait for keys */
		inkey_scan = TRUE;

		/* Attempt to read a key */
		ch = inkey();
	}

	/* Terminate */
	buf[n] = '\0';

	/* Flush */
	flush();


	/* Convert the trigger */
	ascii_to_text(tmp, sizeof(tmp), buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);
}


/*
 * Hack -- ask for a keymap "trigger" (see below)
 *
 * Note that both "flush()" calls are extremely important.  This may
 * no longer be true, since "util.c" is much simpler now.  XXX XXX XXX
 */
static void do_cmd_macro_aux_keymap(char *buf)
{
	char tmp[1024];


	/* Flush */
	flush();


	/* Get a key */
	buf[0] = inkey();
	buf[1] = '\0';


	/* Convert to ascii */
	ascii_to_text(tmp, sizeof(tmp), buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);


	/* Flush */
	flush();
}


/*
 * Hack -- Append all keymaps to the given file.
 *
 * Hack -- We only append the keymaps for the "active" mode.
 */
static errr keymap_dump(cptr fname)
{
	static cptr mark = "Keymap Dump";

	int i;

	FILE *fff;

	char buf[1024];

	int mode;


	/* Roguelike */
	if (rogue_like_commands)
	{
		mode = KEYMAP_MODE_ROGUE;
	}

	/* Original */
	else
	{
		mode = KEYMAP_MODE_ORIG;
	}


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Remove old keymaps */
	remove_old_dump(buf, mark);

	/* Append to the file */
	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff) return (-1);

	/* Output header */
	pref_header(fff, mark);

	/* Skip some lines */
	fprintf(fff, "\n\n");

	/* Start dumping */
#ifdef JP
	fprintf(fff, "# 自動キー配置セーブ\n\n");
#else /* JP */
	fprintf(fff, "# Automatic keymap dump\n\n");
#endif /* JP */

	/* Dump them */
	for (i = 0; i < (int)N_ELEMENTS(keymap_act[mode]); i++)
	{
		char key[2] = "?";

		cptr act;

		/* Loop up the keymap */
		act = keymap_act[mode][i];

		/* Skip empty keymaps */
		if (!act) continue;

		/* Encode the action */
		ascii_to_text(buf, sizeof(buf), act);

		/* Dump the keymap action */
		fprintf(fff, "A:%s\n", buf);

		/* Convert the key into a string */
		key[0] = i;

		/* Encode the key */
		ascii_to_text(buf, sizeof(buf), key);

		/* Dump the keymap pattern */
		fprintf(fff, "C:%d:%s\n", mode, buf);

		/* Skip a line */
		fprintf(fff, "\n");
	}

	/* Skip some lines */
	fprintf(fff, "\n\n\n");

	/* Output footer */
	pref_footer(fff, mark);

	/* Close */
	my_fclose(fff);

	/* Success */
	return (0);
}


#endif


/*
 * Interact with "macros"
 *
 * Could use some helpful instructions on this page.  XXX XXX XXX
 */
void do_cmd_macros(void)
{
	char ch;

	char tmp[1024];

	char pat[1024];

	int mode;


	/* Roguelike */
	if (rogue_like_commands)
	{
		mode = KEYMAP_MODE_ROGUE;
	}

	/* Original */
	else
	{
		mode = KEYMAP_MODE_ORIG;
	}


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save screen */
	screen_save();


	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Describe */
#ifdef JP
		prt("[ マクロの設定 ]", 2, 0);
#else /* JP */
		prt("Interact with Macros", 2, 0);
#endif /* JP */


		/* Describe that action */
#ifdef JP
		prt("(あるなら)現在のマクロ行動が下に表示されています:", 20, 0);
#else /* JP */
		prt("Current action (if any) shown below:", 20, 0);
#endif /* JP */

		/* Analyze the current action */
		ascii_to_text(tmp, sizeof(tmp), macro_buffer);

		/* Display the current action */
		prt(tmp, 22, 0);


		/* Selections */
#ifdef JP
		prt("(1) ユーザー設定ファイルのロード", 4, 5);
#else /* JP */
		prt("(1) Load a user pref file", 4, 5);
#endif /* JP */
#ifdef ALLOW_MACROS
#ifdef JP
		prt("(2) ファイルにマクロを追加", 5, 5);
		prt("(3) マクロの確認", 6, 5);
		prt("(4) マクロの作成", 7, 5);
		prt("(5) マクロの削除", 8, 5);
		prt("(6) ファイルにキー配置を追加", 9, 5);
		prt("(7) キー配置の確認", 10, 5);
		prt("(8) キー配置の作成", 11, 5);
		prt("(9) キー配置の削除", 12, 5);
		prt("(0) マクロ行動の入力", 13, 5);
#else /* JP */
		prt("(2) Append macros to a file", 5, 5);
		prt("(3) Query a macro", 6, 5);
		prt("(4) Create a macro", 7, 5);
		prt("(5) Remove a macro", 8, 5);
		prt("(6) Append keymaps to a file", 9, 5);
		prt("(7) Query a keymap", 10, 5);
		prt("(8) Create a keymap", 11, 5);
		prt("(9) Remove a keymap", 12, 5);
		prt("(0) Enter a new action", 13, 5);
#endif /* JP */
#endif /* ALLOW_MACROS */

		/* Prompt */
#ifdef JP
		prt("コマンド:", 16, 0);
#else /* JP */
		prt("Command: ", 16, 0);
#endif /* JP */

		/* Get a command */
		ch = inkey();

		/* Leave */
		if (ch == ESCAPE) break;

		/* Load a user pref file */
		if (ch == '1')
		{
			/* Ask for and load a user pref file */
			do_cmd_pref_file_hack(16);
		}

#ifdef ALLOW_MACROS

		/* Save macros */
		else if (ch == '2')
		{
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: マクロをファイルに追加する", 16, 0);
#else /* JP */
			prt("Command: Append macros to a file", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 18, 0);
#else /* JP */
			prt("File: ", 18, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Ask for a file */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Dump the macros */
			(void)macro_dump(ftmp);

			/* Prompt */
#ifdef JP
			msg_print("マクロを追加しました。");
#else /* JP */
			msg_print("Appended macros.");
#endif /* JP */
		}

		/* Query a macro */
		else if (ch == '3')
		{
			int k;

			/* Prompt */
#ifdef JP
			prt("コマンド: マクロの確認", 16, 0);
#else /* JP */
			prt("Command: Query a macro", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("トリガーキー: ", 18, 0);
#else /* JP */
			prt("Trigger: ", 18, 0);
#endif /* JP */

			/* Get a macro trigger */
			do_cmd_macro_aux(pat);

			/* Get the action */
			k = macro_find_exact(pat);

			/* Nothing found */
			if (k < 0)
			{
				/* Prompt */
#ifdef JP
				msg_print("そのキーにはマクロは定義されていません。");
#else /* JP */
				msg_print("Found no macro.");
#endif /* JP */
			}

			/* Found one */
			else
			{
				/* Obtain the action */
				my_strcpy(macro_buffer, macro__act[k], sizeof(macro_buffer));

				/* Analyze the current action */
				ascii_to_text(tmp, sizeof(tmp), macro_buffer);

				/* Display the current action */
				prt(tmp, 22, 0);

				/* Prompt */
#ifdef JP
				msg_print("マクロを確認しました。");
#else /* JP */
				msg_print("Found a macro.");
#endif /* JP */
			}
		}

		/* Create a macro */
		else if (ch == '4')
		{
			/* Prompt */
#ifdef JP
			prt("コマンド: マクロの作成", 16, 0);
#else /* JP */
			prt("Command: Create a macro", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("トリガーキー: ", 18, 0);
#else /* JP */
			prt("Trigger: ", 18, 0);
#endif /* JP */

			/* Get a macro trigger */
			do_cmd_macro_aux(pat);

			/* Clear */
			clear_from(20);

			/* Prompt */
#ifdef JP
			prt("マクロ行動: ", 20, 0);
#else /* JP */
			prt("Action: ", 20, 0);
#endif /* JP */

			/* Convert to text */
			ascii_to_text(tmp, sizeof(tmp), macro_buffer);

			/* Get an encoded action */
			if (askfor_aux(tmp, 80))
			{
				/* Convert to ascii */
				text_to_ascii(macro_buffer, sizeof(macro_buffer), tmp);

				/* Link the macro */
				macro_add(pat, macro_buffer);

				/* Prompt */
#ifdef JP
				msg_print("マクロを追加しました。");
#else /* JP */
				msg_print("Added a macro.");
#endif /* JP */
			}
		}

		/* Remove a macro */
		else if (ch == '5')
		{
			/* Prompt */
#ifdef JP
			prt("コマンド: マクロの削除", 16, 0);
#else /* JP */
			prt("Command: Remove a macro", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("トリガーキー: ", 18, 0);
#else /* JP */
			prt("Trigger: ", 18, 0);
#endif /* JP */

			/* Get a macro trigger */
			do_cmd_macro_aux(pat);

			/* Link the macro */
			macro_add(pat, pat);

			/* Prompt */
#ifdef JP
			msg_print("マクロを削除しました。");
#else /* JP */
			msg_print("Removed a macro.");
#endif /* JP */
		}

		/* Save keymaps */
		else if (ch == '6')
		{
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: キー配置をファイルに追加する", 16, 0);
#else /* JP */
			prt("Command: Append keymaps to a file", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 18, 0);
#else /* JP */
			prt("File: ", 18, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Ask for a file */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Dump the macros */
			(void)keymap_dump(ftmp);

			/* Prompt */
#ifdef JP
			msg_print("キー配置を追加しました。");
#else /* JP */
			msg_print("Appended keymaps.");
#endif /* JP */
		}

		/* Query a keymap */
		else if (ch == '7')
		{
			cptr act;

			/* Prompt */
#ifdef JP
			prt("コマンド: キー配置の確認", 16, 0);
#else /* JP */
			prt("Command: Query a keymap", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("押すキー: ", 18, 0);
#else /* JP */
			prt("Keypress: ", 18, 0);
#endif /* JP */

			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(pat);

			/* Look up the keymap */
			act = keymap_act[mode][(byte)(pat[0])];

			/* Nothing found */
			if (!act)
			{
				/* Prompt */
#ifdef JP
				msg_print("キー配置は定義されていません。");
#else /* JP */
				msg_print("Found no keymap.");
#endif /* JP */
			}

			/* Found one */
			else
			{
				/* Obtain the action */
				my_strcpy(macro_buffer, act, sizeof(macro_buffer));

				/* Analyze the current action */
				ascii_to_text(tmp, sizeof(tmp), macro_buffer);

				/* Display the current action */
				prt(tmp, 22, 0);

				/* Prompt */
#ifdef JP
				msg_print("キー配置を確認しました。");
#else /* JP */
				msg_print("Found a keymap.");
#endif /* JP */
			}
		}

		/* Create a keymap */
		else if (ch == '8')
		{
			/* Prompt */
#ifdef JP
			prt("コマンド: キー配置の作成", 16, 0);
#else /* JP */
			prt("Command: Create a keymap", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("押すキー: ", 18, 0);
#else /* JP */
			prt("Keypress: ", 18, 0);
#endif /* JP */

			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(pat);

			/* Clear */
			clear_from(20);

			/* Prompt */
#ifdef JP
			prt("行動: ", 20, 0);
#else /* JP */
			prt("Action: ", 20, 0);
#endif /* JP */

			/* Convert to text */
			ascii_to_text(tmp, sizeof(tmp), macro_buffer);

			/* Get an encoded action */
			if (askfor_aux(tmp, 80))
			{
				/* Convert to ascii */
				text_to_ascii(macro_buffer, sizeof(macro_buffer), tmp);

				/* Free old keymap */
				string_free(keymap_act[mode][(byte)(pat[0])]);

				/* Make new keymap */
				keymap_act[mode][(byte)(pat[0])] = string_make(macro_buffer);

				/* Prompt */
#ifdef JP
				msg_print("キー配置を追加しました。");
#else /* JP */
				msg_print("Added a keymap.");
#endif /* JP */
			}
		}

		/* Remove a keymap */
		else if (ch == '9')
		{
			/* Prompt */
#ifdef JP
			prt("コマンド: キー配置の削除", 16, 0);
#else /* JP */
			prt("Command: Remove a keymap", 16, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("押すキー: ", 18, 0);
#else /* JP */
			prt("Keypress: ", 18, 0);
#endif /* JP */

			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(pat);

			/* Free old keymap */
			string_free(keymap_act[mode][(byte)(pat[0])]);

			/* Make new keymap */
			keymap_act[mode][(byte)(pat[0])] = NULL;

			/* Prompt */
#ifdef JP
			msg_print("キー配置を削除しました。");
#else /* JP */
			msg_print("Removed a keymap.");
#endif /* JP */
		}

		/* Enter a new action */
		else if (ch == '0')
		{
			/* Prompt */
#ifdef JP
			prt("コマンド: マクロ行動の入力", 16, 0);
#else /* JP */
			prt("Command: Enter a new action", 16, 0);
#endif /* JP */

			/* Go to the correct location */
			Term_gotoxy(0, 22);

			/* Analyze the current action */
			ascii_to_text(tmp, sizeof(tmp), macro_buffer);

			/* Get an encoded action */
			if (askfor_aux(tmp, 80))
			{
				/* Extract an action */
				text_to_ascii(macro_buffer, sizeof(macro_buffer), tmp);
			}
		}

#endif /* ALLOW_MACROS */

		/* Oops */
		else
		{
			/* Oops */
#ifdef JP
			bell("無効なキーです！");
#else /* JP */
			bell("Illegal command for macros!");
#endif /* JP */
		}

		/* Flush messages */
		message_flush();
	}


	/* Load screen */
	screen_load();
}



/*
 * Interact with "visuals"
 */
void do_cmd_visuals(void)
{
	int ch;
	int cx;

	int i;

	FILE *fff;

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save screen */
	screen_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("画面表示の設定", 2, 0);
#else /* JP */
		prt("Interact with Visuals", 2, 0);
#endif /* JP */

		/* Give some choices */
#ifdef JP
		prt("(1) ユーザー設定ファイルのロード", 4, 5);
#else /* JP */
		prt("(1) Load a user pref file", 4, 5);
#endif /* JP */
#ifdef ALLOW_VISUALS
#ifdef JP
		prt("(2) モンスターの 色/文字 をファイルに書き出す", 5, 5);
		prt("(3) アイテムの   色/文字 をファイルに書き出す", 6, 5);
		prt("(4) 地形の       色/文字 をファイルに書き出す", 7, 5);
		prt("(5) 外見の       色/文字 をファイルに書き出す", 8, 5);
		prt("(6) モンスターの 色/文字 を変更する", 9, 5);
		prt("(7) アイテムの   色/文字 を変更する", 10, 5);
		prt("(8) 地形の       色/文字 を変更する", 11, 5);
		prt("(9) 外見         色/文字 を変更する", 12, 5);
#else /* JP */
		prt("(2) Dump monster attr/chars", 5, 5);
		prt("(3) Dump object attr/chars", 6, 5);
		prt("(4) Dump feature attr/chars", 7, 5);
		prt("(5) Dump flavor attr/chars", 8, 5);
		prt("(6) Change monster attr/chars", 9, 5);
		prt("(7) Change object attr/chars", 10, 5);
		prt("(8) Change feature attr/chars", 11, 5);
		prt("(9) Change flavor attr/chars", 12, 5);
#endif /* JP */
#endif
#ifdef JP
		prt("(0) 画面表示方法の初期化", 13, 5);
#else /* JP */
		prt("(0) Reset visuals", 13, 5);
#endif /* JP */

		/* Prompt */
#ifdef JP
		prt("コマンド: ", 15, 0);
#else /* JP */
		prt("Command: ", 15, 0);
#endif /* JP */

		/* Prompt */
		ch = inkey();

		/* Done */
		if (ch == ESCAPE) break;

		/* Load a user pref file */
		if (ch == '1')
		{
			/* Ask for and load a user pref file */
			do_cmd_pref_file_hack(15);
		}

#ifdef ALLOW_VISUALS

		/* Dump monster attr/chars */
		else if (ch == '2')
		{
			static cptr mark = "Monster attr/chars";
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: モンスターの[色/文字]をファイルに書き出します", 15, 0);
#else /* JP */
			prt("Command: Dump monster attr/chars", 15, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 17, 0);
#else /* JP */
			prt("File: ", 17, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Get a filename */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, ftmp);

			/* Remove old attr/chars */
			remove_old_dump(buf, mark);

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Failure */
			if (!fff) continue;

			/* Output header */
			pref_header(fff, mark);

			/* Skip some lines */
			fprintf(fff, "\n\n");

			/* Start dumping */
#ifdef JP
			fprintf(fff, "# モンスターの[色/文字]の設定\n\n");
#else /* JP */
			fprintf(fff, "# Monster attr/char definitions\n\n");
#endif /* JP */

			/* Dump monsters */
			for (i = 0; i < z_info->r_max; i++)
			{
				monster_race *r_ptr = &r_info[i];

				/* Skip non-entries */
				if (!r_ptr->name) continue;

				/* Dump a comment */
#ifdef JP
				/* 英日切り替え機能対応 */
				fprintf(fff, "# %s\n", X_r_name(r_ptr));
#else /* JP */
				fprintf(fff, "# %s\n", (r_name + r_ptr->name));
#endif /* JP */

				/* Dump the monster attr/char info */
				fprintf(fff, "R:%d:0x%02X:0x%02X\n\n", i,
				        (byte)(r_ptr->x_attr), (byte)(r_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Output footer */
			pref_footer(fff, mark);

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("モンスターの[色/文字]をファイルに書き出しました。");
#else /* JP */
			msg_print("Dumped monster attr/chars.");
#endif /* JP */
		}

		/* Dump object attr/chars */
		else if (ch == '3')
		{
			static cptr mark = "Object attr/chars";
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: アイテムの[色/文字]をファイルに書き出します", 15, 0);
#else /* JP */
			prt("Command: Dump object attr/chars", 15, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 17, 0);
#else /* JP */
			prt("File: ", 17, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Get a filename */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, ftmp);

			/* Remove old attr/chars */
			remove_old_dump(buf, mark);

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Failure */
			if (!fff) continue;

			/* Output header */
			pref_header(fff, mark);

			/* Skip some lines */
			fprintf(fff, "\n\n");

			/* Start dumping */
#ifdef JP
			fprintf(fff, "# アイテムの[色/文字]の設定\n\n");
#else /* JP */
			fprintf(fff, "# Object attr/char definitions\n\n");
#endif /* JP */

			/* Dump objects */
			for (i = 0; i < z_info->k_max; i++)
			{
				object_kind *k_ptr = &k_info[i];

				/* Skip non-entries */
				if (!k_ptr->name) continue;

				/* Dump a comment */
#ifdef JP
				/* 英日切り替え機能対応 */
				fprintf(fff, "# %s\n", X_k_name(k_ptr));
#else /* JP */
				fprintf(fff, "# %s\n", (k_name + k_ptr->name));
#endif /* JP */

				/* Dump the object attr/char info */
				fprintf(fff, "K:%d:0x%02X:0x%02X\n\n", i,
				        (byte)(k_ptr->x_attr), (byte)(k_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Output footer */
			pref_footer(fff, mark);

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("アイテムの[色/文字]をファイルに書き出しました。");
#else /* JP */
			msg_print("Dumped object attr/chars.");
#endif /* JP */
		}

		/* Dump feature attr/chars */
		else if (ch == '4')
		{
			static cptr mark = "Feature attr/chars";
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: 地形の[色/文字]をファイルに書き出します", 15, 0);
#else /* JP */
			prt("Command: Dump feature attr/chars", 15, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 17, 0);
#else /* JP */
			prt("File: ", 17, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Get a filename */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, ftmp);

			/* Remove old attr/chars */
			remove_old_dump(buf, mark);

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Failure */
			if (!fff) continue;

			/* Output header */
			pref_header(fff, mark);

			/* Skip some lines */
			fprintf(fff, "\n\n");

			/* Start dumping */
#ifdef JP
			fprintf(fff, "# 地形の[色/文字]の設定\n\n");
#else /* JP */
			fprintf(fff, "# Feature attr/char definitions\n\n");
#endif /* JP */

			/* Dump features */
			for (i = 0; i < z_info->f_max; i++)
			{
				feature_type *f_ptr = &f_info[i];

				/* Skip non-entries */
				if (!f_ptr->name) continue;

				/* Dump a comment */
#ifdef JP
				/* 英日切り替え機能対応 */
				fprintf(fff, "# %s\n", X_f_name(f_ptr));
#else /* JP */
				fprintf(fff, "# %s\n", (f_name + f_ptr->name));
#endif /* JP */

				/* Dump the feature attr/char info */
				fprintf(fff, "F:%d:0x%02X:0x%02X\n\n", i,
				        (byte)(f_ptr->x_attr), (byte)(f_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Output footer */
			pref_footer(fff, mark);

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("地形の[色/文字]をファイルに書き出しました。");
#else /* JP */
			msg_print("Dumped feature attr/chars.");
#endif /* JP */
		}

		/* Dump flavor attr/chars */
		else if (ch == '5')
		{
			static cptr mark = "Flavor attr/chars";
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: 外見の[色/文字]をファイルに書き出します", 15, 0);
#else /* JP */
			prt("Command: Dump flavor attr/chars", 15, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 17, 0);
#else /* JP */
			prt("File: ", 17, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Get a filename */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, ftmp);

			/* Remove old attr/chars */
			remove_old_dump(buf, mark);

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Failure */
			if (!fff) continue;

			/* Output header */
			pref_header(fff, mark);

			/* Skip some lines */
			fprintf(fff, "\n\n");

			/* Start dumping */
#ifdef JP
			fprintf(fff, "# 外見の[色/文字]の設定\n\n");
#else /* JP */
			fprintf(fff, "# Flavor attr/char definitions\n\n");
#endif /* JP */

			/* Dump flavors */
			for (i = 0; i < z_info->flavor_max; i++)
			{
				flavor_type *flavor_ptr = &flavor_info[i];

				/* Dump a comment */
#ifdef JP
				fprintf(fff, "# %s\n", X_flavor_name(flavor_ptr));
#else /* JP */
				fprintf(fff, "# %s\n", (flavor_text + flavor_ptr->text));
#endif /* JP */

				/* Dump the flavor attr/char info */
				fprintf(fff, "L:%d:0x%02X:0x%02X\n\n", i,
				        (byte)(flavor_ptr->x_attr), (byte)(flavor_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Output footer */
			pref_footer(fff, mark);

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("外見の[色/文字]をファイルに書き出しました。");
#else /* JP */
			msg_print("Dumped flavor attr/chars.");
#endif /* JP */
		}

		/* Modify monster attr/chars */
		else if (ch == '6')
		{
			static int r = 0;

			/* Prompt */
#ifdef JP
			prt("コマンド: モンスターの[色/文字]を変更します", 15, 0);
#else /* JP */
			prt("Command: Change monster attr/chars", 15, 0);
#endif /* JP */

			/* Hack -- query until done */
			while (1)
			{
				monster_race *r_ptr = &r_info[r];

				byte da = (byte)(r_ptr->d_attr);
				byte dc = (byte)(r_ptr->d_char);
				byte ca = (byte)(r_ptr->x_attr);
				byte cc = (byte)(r_ptr->x_char);
#ifdef JP
				byte ca2, cc2;
#endif /* JP */

				/* Label the object */
#ifdef JP
				/* 英日切り替え機能対応 */
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("モンスター = %d, 名前 = %-40.40s",
				                   r, X_r_name(r_ptr)));
#else /* JP */
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("Monster = %d, Name = %-40.40s",
				                   r, (r_name + r_ptr->name)));
#endif /* JP */

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("初期値  色 / 文字 = %3u / %3u", da, dc));
#else /* JP */
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("Default attr/char = %3u / %3u", da, dc));
#endif /* JP */
				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 19, ca, cc);
				if (use_bigtile) Term_putch(43, 19, ca2, cc2);
#else /* JP */
				Term_putch(43, 19, da, dc);

				if (use_bigtile)
				{
					if (da & 0x80)
						Term_putch(44, 19, 255, -1);
					else
						Term_putch(44, 19, 0, ' ');
				}
#endif /* JP */

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("現在値  色 / 文字 = %3u / %3u", ca, cc));
#else /* JP */
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("Current attr/char = %3u / %3u", ca, cc));
#endif /* JP */
				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 20, ca, cc);
				if (use_bigtile) Term_putch(43, 20, ca2, cc2);
#else /* JP */
				Term_putch(43, 20, ca, cc);

				if (use_bigtile)
				{
					if (ca & 0x80)
						Term_putch(44, 20, 255, -1);
					else
						Term_putch(44, 20, 0, ' ');
				}
#endif /* JP */

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "コマンド (n/N/a/A/c/C): ");
#else /* JP */
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "Command (n/N/a/A/c/C): ");
#endif /* JP */

				/* Get a command */
				cx = inkey();

				/* All done */
				if (cx == ESCAPE) break;

				/* Analyze */
				if (cx == 'n') r = (r + z_info->r_max + 1) % z_info->r_max;
				if (cx == 'N') r = (r + z_info->r_max - 1) % z_info->r_max;
				if (cx == 'a') r_ptr->x_attr = (byte)(ca + 1);
				if (cx == 'A') r_ptr->x_attr = (byte)(ca - 1);
				if (cx == 'c') r_ptr->x_char = (byte)(cc + 1);
				if (cx == 'C') r_ptr->x_char = (byte)(cc - 1);
			}
		}

		/* Modify object attr/chars */
		else if (ch == '7')
		{
			static int k = 0;

			/* Prompt */
#ifdef JP
			prt("コマンド: アイテムの[色/文字]を変更します", 15, 0);
#else /* JP */
			prt("Command: Change object attr/chars", 15, 0);
#endif /* JP */

			/* Hack -- query until done */
			while (1)
			{
				object_kind *k_ptr = &k_info[k];

				byte da = (byte)(k_ptr->d_attr);
				byte dc = (byte)(k_ptr->d_char);
				byte ca = (byte)(k_ptr->x_attr);
				byte cc = (byte)(k_ptr->x_char);
#ifdef JP
				byte ca2, cc2;
#endif /* JP */

				/* Label the object */
#ifdef JP
				/* 英日切り替え機能対応 */
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("アイテム = %d, 名前 = %-40.40s",
				                   k, X_k_name(k_ptr)));
#else /* JP */
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("Object = %d, Name = %-40.40s",
				                   k, (k_name + k_ptr->name)));
#endif /* JP */

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("初期値  色 / 文字 = %3d / %3d", da, dc));
#else /* JP */
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("Default attr/char = %3d / %3d", da, dc));
#endif /* JP */
				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 19, ca, cc);
				if (use_bigtile) Term_putch(43, 19, ca2, cc2);
#else /* JP */
				Term_putch(43, 19, da, dc);

				if (use_bigtile)
				{
					if (da & 0x80)
						Term_putch(44, 19, 255, -1);
					else
						Term_putch(44, 19, 0, ' ');
				}
#endif /* JP */

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("現在値  色 / 文字 = %3d / %3d", ca, cc));
#else /* JP */
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("Current attr/char = %3d / %3d", ca, cc));
#endif /* JP */
				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 20, ca, cc);
				if (use_bigtile) Term_putch(43, 20, ca2, cc2);
#else /* JP */
				Term_putch(43, 20, ca, cc);

				if (use_bigtile)
				{
					if (ca & 0x80)
						Term_putch(44, 20, 255, -1);
					else
						Term_putch(44, 20, 0, ' ');
				}
#endif /* JP */

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "コマンド (n/N/a/A/c/C): ");
#else /* JP */
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "Command (n/N/a/A/c/C): ");
#endif /* JP */

				/* Get a command */
				cx = inkey();

				/* All done */
				if (cx == ESCAPE) break;

				/* Analyze */
				if (cx == 'n') k = (k + z_info->k_max + 1) % z_info->k_max;
				if (cx == 'N') k = (k + z_info->k_max - 1) % z_info->k_max;
				if (cx == 'a') k_info[k].x_attr = (byte)(ca + 1);
				if (cx == 'A') k_info[k].x_attr = (byte)(ca - 1);
				if (cx == 'c') k_info[k].x_char = (byte)(cc + 1);
				if (cx == 'C') k_info[k].x_char = (byte)(cc - 1);
			}
		}

		/* Modify feature attr/chars */
		else if (ch == '8')
		{
			static int f = 0;

			/* Prompt */
#ifdef JP
			prt("コマンド: 地形の[色/文字]を変更します", 15, 0);
#else /* JP */
			prt("Command: Change feature attr/chars", 15, 0);
#endif /* JP */

			/* Hack -- query until done */
			while (1)
			{
				feature_type *f_ptr = &f_info[f];

				byte da = (byte)(f_ptr->d_attr);
				byte dc = (byte)(f_ptr->d_char);
				byte ca = (byte)(f_ptr->x_attr);
				byte cc = (byte)(f_ptr->x_char);
#ifdef JP
				byte ca2, cc2;
#endif /* JP */

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("地形 = %d, 名前 = %-40.40s",
				                   f, X_f_name(f_ptr)));
#else /* JP */
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("Terrain = %d, Name = %-40.40s",
				                   f, (f_name + f_ptr->name)));
#endif /* JP */

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("初期値  色 / 文字 = %3d / %3d", da, dc));
#else /* JP */
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("Default attr/char = %3d / %3d", da, dc));
#endif /* JP */
				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 19, ca, cc);
				if (use_bigtile) Term_putch(43, 19, ca2, cc2);
#else /* JP */
				Term_putch(43, 19, da, dc);

				if (use_bigtile)
				{
					if (da & 0x80)
						Term_putch(44, 19, 255, -1);
					else
						Term_putch(44, 19, 0, ' ');
				}
#endif /* JP */

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("現在値  色 / 文字 = %3d / %3d", ca, cc));
#else /* JP */
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("Current attr/char = %3d / %3d", ca, cc));
#endif /* JP */
				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 20, ca, cc);
				if (use_bigtile) Term_putch(43, 20, ca2, cc2);
#else /* JP */
				Term_putch(43, 20, ca, cc);

				if (use_bigtile)
				{
					if (ca & 0x80)
						Term_putch(44, 20, 255, -1);
					else
						Term_putch(44, 20, 0, ' ');
				}
#endif /* JP */

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "コマンド (n/N/a/A/c/C): ");
#else /* JP */
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "Command (n/N/a/A/c/C): ");
#endif /* JP */

				/* Get a command */
				cx = inkey();

				/* All done */
				if (cx == ESCAPE) break;

				/* Analyze */
				if (cx == 'n') f = (f + z_info->f_max + 1) % z_info->f_max;
				if (cx == 'N') f = (f + z_info->f_max - 1) % z_info->f_max;
				if (cx == 'a') f_info[f].x_attr = (byte)(ca + 1);
				if (cx == 'A') f_info[f].x_attr = (byte)(ca - 1);
				if (cx == 'c') f_info[f].x_char = (byte)(cc + 1);
				if (cx == 'C') f_info[f].x_char = (byte)(cc - 1);
			}
		}

		/* Modify flavor attr/chars */
		else if (ch == '9')
		{
			static int f = 0;

			/* Prompt */
#ifdef JP
			prt("コマンド: 外見の[色/文字]を変更します", 15, 0);
#else /* JP */
			prt("Command: Change flavor attr/chars", 15, 0);
#endif /* JP */

			/* Hack -- query until done */
			while (1)
			{
				flavor_type *flavor_ptr = &flavor_info[f];

				byte da = (byte)(flavor_ptr->d_attr);
				byte dc = (byte)(flavor_ptr->d_char);
				byte ca = (byte)(flavor_ptr->x_attr);
				byte cc = (byte)(flavor_ptr->x_char);
#ifdef JP
				byte ca2, cc2;
#endif /* JP */

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("外見 = %d, 名前 = %-40.40s",
				                   f, X_flavor_name(flavor_ptr)));
#else /* JP */
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("Flavor = %d, Text = %-40.40s",
				                   f, (flavor_text + flavor_ptr->text)));
#endif /* JP */

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("初期値  色 / 文字 = %3d / %3d", da, dc));
#else /* JP */
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("Default attr/char = %3d / %3d", da, dc));
#endif /* JP */
				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 19, da, dc);

#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 19, ca, cc);
				if (use_bigtile) Term_putch(43, 19, ca2, cc2);
#else /* JP */
				if (use_bigtile)
				{
					if (da & 0x80)
						Term_putch(44, 19, 255, -1);
					else
						Term_putch(44, 19, 0, ' ');
				}
#endif /* JP */

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("現在値  色 / 文字 = %3d / %3d", ca, cc));
#else /* JP */
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("Current attr/char = %3d / %3d", ca, cc));
#endif /* JP */
				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
#ifdef JP
				if (use_bigtile) bigtile_attr(&cc, &ca, &cc2, &ca2);
				Term_putch(43, 20, ca, cc);
				if (use_bigtile) Term_putch(43, 20, ca2, cc2);
#else /* JP */
				Term_putch(43, 20, ca, cc);

				if (use_bigtile)
				{
					if (ca & 0x80)
						Term_putch(44, 20, 255, -1);
					else
						Term_putch(44, 20, 0, ' ');
				}
#endif /* JP */

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "コマンド (n/N/a/A/c/C): ");
#else /* JP */
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "Command (n/N/a/A/c/C): ");
#endif /* JP */

				/* Get a command */
				cx = inkey();

				/* All done */
				if (cx == ESCAPE) break;

				/* Analyze */
				if (cx == 'n') f = (f + z_info->flavor_max + 1) % z_info->flavor_max;
				if (cx == 'N') f = (f + z_info->flavor_max - 1) % z_info->flavor_max;
				if (cx == 'a') flavor_info[f].x_attr = (byte)(ca + 1);
				if (cx == 'A') flavor_info[f].x_attr = (byte)(ca - 1);
				if (cx == 'c') flavor_info[f].x_char = (byte)(cc + 1);
				if (cx == 'C') flavor_info[f].x_char = (byte)(cc - 1);
			}
		}

#endif /* ALLOW_VISUALS */

		/* Reset visuals */
		else if (ch == '0')
		{
			/* Reset */
			reset_visuals(TRUE);

			/* Message */
#ifdef JP
			msg_print("画面上の[色/文字]を初期値にリセットしました。");
#else /* JP */
			msg_print("Visual attr/char tables reset.");
#endif /* JP */
		}

		/* Unknown option */
		else
		{
#ifdef JP
			bell("無効なキーです！");
#else /* JP */
			bell("Illegal command for visuals!");
#endif /* JP */
		}

		/* Flush messages */
		message_flush();
	}


	/* Load screen */
	screen_load();
}


/*
 * Interact with "colors"
 */
void do_cmd_colors(void)
{
	int ch;
	int cx;

	int i;

	FILE *fff;

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save screen */
	screen_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("[ カラーの設定 ]", 2, 0);
#else /* JP */
		prt("Interact with Colors", 2, 0);
#endif /* JP */

		/* Give some choices */
#ifdef JP
		prt("(1) ユーザー設定ファイルのロード", 4, 5);
#else /* JP */
		prt("(1) Load a user pref file", 4, 5);
#endif /* JP */
#ifdef ALLOW_COLORS
#ifdef JP
		prt("(2) カラーの設定をファイルに書き出す", 5, 5);
		prt("(3) カラーの設定を変更する", 6, 5);
#else /* JP */
		prt("(2) Dump colors", 5, 5);
		prt("(3) Modify colors", 6, 5);
#endif /* JP */
#endif /* ALLOW_COLORS */

		/* Prompt */
#ifdef JP
		prt("コマンド: ", 8, 0);
#else /* JP */
		prt("Command: ", 8, 0);
#endif /* JP */

		/* Prompt */
		ch = inkey();

		/* Done */
		if (ch == ESCAPE) break;

		/* Load a user pref file */
		if (ch == '1')
		{
			/* Ask for and load a user pref file */
			do_cmd_pref_file_hack(8);

			/* Could skip the following if loading cancelled XXX XXX XXX */

			/* Mega-Hack -- React to color changes */
			Term_xtra(TERM_XTRA_REACT, 0);

			/* Mega-Hack -- Redraw physical windows */
			Term_redraw();
		}

#ifdef ALLOW_COLORS

		/* Dump colors */
		else if (ch == '2')
		{
			static cptr mark = "Colors";
			char ftmp[80];

			/* Prompt */
#ifdef JP
			prt("コマンド: カラーの設定をファイルに書き出します", 8, 0);
#else /* JP */
			prt("Command: Dump colors", 8, 0);
#endif /* JP */

			/* Prompt */
#ifdef JP
			prt("ファイル: ", 10, 0);
#else /* JP */
			prt("File: ", 10, 0);
#endif /* JP */

			/* Default filename */
			strnfmt(ftmp, sizeof(ftmp), "%s.prf", op_ptr->base_name);

			/* Get a filename */
			if (!askfor_aux(ftmp, sizeof(ftmp))) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, ftmp);

			/* Remove old colors */
			remove_old_dump(buf, mark);

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Failure */
			if (!fff) continue;

			/* Output header */
			pref_header(fff, mark);

			/* Skip some lines */
			fprintf(fff, "\n\n");

			/* Start dumping */
#ifdef JP
			fprintf(fff, "# カラーの設定\n\n");
#else /* JP */
			fprintf(fff, "# Color redefinitions\n\n");
#endif /* JP */

			/* Dump colors */
			for (i = 0; i < 256; i++)
			{
				int kv = angband_color_table[i][0];
				int rv = angband_color_table[i][1];
				int gv = angband_color_table[i][2];
				int bv = angband_color_table[i][3];

#ifdef JP
				cptr name = "未知";
#else /* JP */
				cptr name = "unknown";
#endif /* JP */

				/* Skip non-entries */
				if (!kv && !rv && !gv && !bv) continue;

				/* Extract the color name */
				if (i < 16) name = color_names[i];

				/* Dump a comment */
#ifdef JP
				fprintf(fff, "# カラー '%s'\n", name);
#else /* JP */
				fprintf(fff, "# Color '%s'\n", name);
#endif /* JP */

				/* Dump the monster attr/char info */
				fprintf(fff, "V:%d:0x%02X:0x%02X:0x%02X:0x%02X\n\n",
				        i, kv, rv, gv, bv);
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Output footer */
			pref_footer(fff, mark);

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("カラーの設定をファイルに書き出しました。");
#else /* JP */
			msg_print("Dumped color redefinitions.");
#endif /* JP */
		}

		/* Edit colors */
		else if (ch == '3')
		{
			static byte a = 0;

			/* Prompt */
#ifdef JP
			prt("コマンド: カラーの設定を変更します", 8, 0);
#else /* JP */
			prt("Command: Modify colors", 8, 0);
#endif /* JP */

			/* Hack -- query until done */
			while (1)
			{
				cptr name;

				/* Clear */
				clear_from(10);

				/* Exhibit the normal colors */
				for (i = 0; i < 16; i++)
				{
					/* Exhibit this color */
					Term_putstr(i*4, 20, -1, a, "###");

					/* Exhibit all colors */
					Term_putstr(i*4, 22, -1, (byte)i, format("%3d", i));
				}

				/* Describe the color */
#ifdef JP
				name = ((a < 16) ? color_names[a] : "未定義");
#else /* JP */
				name = ((a < 16) ? color_names[a] : "undefined");
#endif /* JP */

				/* Describe the color */
#ifdef JP
				Term_putstr(5, 10, -1, TERM_WHITE,
				            format("カラー = %d, 名前 = %s", a, name));
#else /* JP */
				Term_putstr(5, 10, -1, TERM_WHITE,
				            format("Color = %d, Name = %s", a, name));
#endif /* JP */

				/* Label the Current values */
				Term_putstr(5, 12, -1, TERM_WHITE,
				            format("K = 0x%02x / R,G,B = 0x%02x,0x%02x,0x%02x",
				                   angband_color_table[a][0],
				                   angband_color_table[a][1],
				                   angband_color_table[a][2],
				                   angband_color_table[a][3]));

				/* Prompt */
#ifdef JP
				Term_putstr(0, 14, -1, TERM_WHITE,
				            "コマンド (n/N/k/K/r/R/g/G/b/B): ");
#else /* JP */
				Term_putstr(0, 14, -1, TERM_WHITE,
				            "Command (n/N/k/K/r/R/g/G/b/B): ");
#endif /* JP */

				/* Get a command */
				cx = inkey();

				/* All done */
				if (cx == ESCAPE) break;

				/* Analyze */
				if (cx == 'n') a = (byte)(a + 1);
				if (cx == 'N') a = (byte)(a - 1);
				if (cx == 'k') angband_color_table[a][0] = (byte)(angband_color_table[a][0] + 1);
				if (cx == 'K') angband_color_table[a][0] = (byte)(angband_color_table[a][0] - 1);
				if (cx == 'r') angband_color_table[a][1] = (byte)(angband_color_table[a][1] + 1);
				if (cx == 'R') angband_color_table[a][1] = (byte)(angband_color_table[a][1] - 1);
				if (cx == 'g') angband_color_table[a][2] = (byte)(angband_color_table[a][2] + 1);
				if (cx == 'G') angband_color_table[a][2] = (byte)(angband_color_table[a][2] - 1);
				if (cx == 'b') angband_color_table[a][3] = (byte)(angband_color_table[a][3] + 1);
				if (cx == 'B') angband_color_table[a][3] = (byte)(angband_color_table[a][3] - 1);

				/* Hack -- react to changes */
				Term_xtra(TERM_XTRA_REACT, 0);

				/* Hack -- redraw */
				Term_redraw();
			}
		}

#endif /* ALLOW_COLORS */

		/* Unknown option */
		else
		{
#ifdef JP
			bell("無効なキーです！");
#else /* JP */
			bell("Illegal command for colors!");
#endif /* JP */
		}

		/* Flush messages */
		message_flush();
	}


	/* Load screen */
	screen_load();
}


/*
 * Note something in the message recall
 */
void do_cmd_note(void)
{
	char tmp[80];

	/* Default */
	strcpy(tmp, "");

	/* Input */
#ifdef JP
	if (!get_string("メモ: ", tmp, sizeof(tmp))) return;
#else /* JP */
	if (!get_string("Note: ", tmp, sizeof(tmp))) return;
#endif /* JP */

	/* Ignore empty notes */
	if (!tmp[0] || (tmp[0] == ' ')) return;

	/* Add the note to the message recall */
#ifdef JP
	msg_format("メモ: %s", tmp);
#else /* JP */
	msg_format("Note: %s", tmp);
#endif /* JP */
}


/*
 * Mention the current version
 */
void do_cmd_version(void)
{
	/* Silly message */
#ifdef JP
	msg_format("%s %s",
	           JVERSION_NAME, JVERSION_STRING);
#else /* JP */
	msg_format("You are playing %s %s.  Type '?' for more info.",
	           VERSION_NAME, VERSION_STRING);
#endif /* JP */
}



/*
 * Array of feeling strings
 */
static cptr do_cmd_feeling_text[11] =
{
#ifdef JP
	"この階の雰囲気を感じとれなかった...",
	"この階には何か特別なものがあるような気がする。",
	"この階はこの上なく素晴らしい感じがする。",
	"素晴らしい感じがする...",
	"とても良い感じがする...",
	"良い感じがする...",
	"ちょっと幸運な感じがする...",
	"多少は運が向いてきたか...",
	"見た感じ悪くはない...",
	"全然駄目ということはないが...",
	"なんて退屈なところだ..."
#else /* JP */
	"Looks like any other level.",
	"You feel there is something special about this level.",
	"You have a superb feeling about this level.",
	"You have an excellent feeling...",
	"You have a very good feeling...",
	"You have a good feeling...",
	"You feel strangely lucky...",
	"You feel your luck is turning...",
	"You like the look of this place...",
	"This level can't be all bad...",
	"What a boring place..."
#endif /* JP */
};


/*
 * Note that "feeling" is set to zero unless some time has passed.
 * Note that this is done when the level is GENERATED, not entered.
 */
void do_cmd_feeling(void)
{
	/* Verify the feeling */
	if (feeling > 10) feeling = 10;

	/* No useful feeling in town */
	if (!p_ptr->depth)
	{
#ifdef JP
		msg_print("典型的な町のようだ。");
#else /* JP */
		msg_print("Looks like a typical town.");
#endif /* JP */
		return;
	}

	/* Display the feeling */
	msg_print(do_cmd_feeling_text[feeling]);
}





/*
 * Encode the screen colors
 */
static const char hack[17] = "dwsorgbuDWvyRGBU";


/*
 * Hack -- load a screen dump from a file
 *
 * ToDo: Add support for loading/saving screen-dumps with graphics
 * and pseudo-graphics.  Allow the player to specify the filename
 * of the dump.
 */
void do_cmd_load_screen(void)
{
	int i, y, x;

	byte a = 0;
	char c = ' ';

	bool okay = TRUE;

	FILE *fp;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, "dump.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Oops */
	if (!fp) return;


	/* Save screen */
	screen_save();


	/* Clear the screen */
	Term_clear();


	/* Load the screen */
	for (y = 0; okay && (y < 24); y++)
	{
		/* Get a line of data */
		if (my_fgets(fp, buf, sizeof(buf))) okay = FALSE;


		/* Show each row */
		for (x = 0; x < 79; x++)
		{
			/* Put the attr/char */
			Term_draw(x, y, TERM_WHITE, buf[x]);
		}
	}

	/* Get the blank line */
	if (my_fgets(fp, buf, sizeof(buf))) okay = FALSE;


	/* Dump the screen */
	for (y = 0; okay && (y < 24); y++)
	{
		/* Get a line of data */
		if (my_fgets(fp, buf, sizeof(buf))) okay = FALSE;

		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Look up the attr */
			for (i = 0; i < 16; i++)
			{
				/* Use attr matches */
				if (hack[i] == buf[x]) a = i;
			}

			/* Put the attr/char */
			Term_draw(x, y, a, c);
		}
	}


	/* Close it */
	my_fclose(fp);


	/* Message */
#ifdef JP
	msg_print("ファイルに書き出された画面(記念撮影)をロードしました。");
#else /* JP */
	msg_print("Screen dump loaded.");
#endif /* JP */
	message_flush();


	/* Load screen */
	screen_load();
}


/*
 * Hack -- save a screen dump to a file
 */
void do_cmd_save_screen(void)
{
	char tmp_val[256];

	/* Ask for a file */
	strcpy(tmp_val, "dump.html");
	if (!get_string("File: ", tmp_val, sizeof(tmp_val))) return;

	html_screenshot(tmp_val);
#ifdef JP
	msg_print("ファイルに書き出しました。");
#else /* JP */
	msg_print("Dump saved.");
#endif /* JP */
}




/*
 * Display known artifacts
 */
static void do_cmd_knowledge_artifacts(void)
{
	int i, k, z, x, y;

	FILE *fff;

	char file_name[1024];

	char o_name[80];

	bool *okay;


	/* Temporary file */
	fff = my_fopen_temp(file_name, sizeof(file_name));

	/* Failure */
	if (!fff) return;

	/* Allocate the "okay" array */
	C_MAKE(okay, z_info->a_max, bool);

	/* Scan the artifacts */
	for (k = 0; k < z_info->a_max; k++)
	{
		artifact_type *a_ptr = &a_info[k];

		/* Default */
		okay[k] = FALSE;

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Skip "uncreated" artifacts */
		if (!a_ptr->cur_num) continue;

		/* Assume okay */
		okay[k] = TRUE;
	}

	/* Check the dungeon */
	for (y = 0; y < DUNGEON_HGT; y++)
	{
		for (x = 0; x < DUNGEON_WID; x++)
		{
			object_type *o_ptr;

			/* Scan all objects in the grid */
			for (o_ptr = get_first_object(y, x); o_ptr; o_ptr = get_next_object(o_ptr))
			{
				/* Ignore non-artifacts */
				if (!artifact_p(o_ptr)) continue;

				/* Ignore known items */
				if (object_known_p(o_ptr)) continue;

				/* Note the artifact */
				okay[o_ptr->name1] = FALSE;
			}
		}
	}

	/* Check the inventory and equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Ignore non-objects */
		if (!o_ptr->k_idx) continue;

		/* Ignore non-artifacts */
		if (!artifact_p(o_ptr)) continue;

		/* Ignore known items */
		if (object_known_p(o_ptr)) continue;

		/* Note the artifact */
		okay[o_ptr->name1] = FALSE;
	}

	/* Scan the artifacts */
	for (k = 0; k < z_info->a_max; k++)
	{
		artifact_type *a_ptr = &a_info[k];

		/* List "dead" ones */
		if (!okay[k]) continue;

		/* Paranoia */
#ifdef JP
		strcpy(o_name, "未知の伝説のアイテム");
#else /* JP */
		strcpy(o_name, "Unknown Artifact");
#endif /* JP */

		/* Obtain the base object type */
		z = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* Real object */
		if (z)
		{
			object_type *i_ptr;
			object_type object_type_body;

			/* Get local object */
			i_ptr = &object_type_body;

			/* Create fake object */
			object_prep(i_ptr, z);

			/* Make it an artifact */
			i_ptr->name1 = k;

			/* Describe the artifact */
			object_desc_spoil(o_name, sizeof(o_name), i_ptr, FALSE, 0);
		}

		/* Hack -- Build the artifact name */
#ifdef JP
		/* 英日切り替え機能対応 */
		fprintf(fff, "     %s%s\n", X_object("The ", ""), o_name);
#else /* JP */
		fprintf(fff, "     The %s\n", o_name);
#endif /* JP */
	}

	/* Free the "okay" array */
	FREE(okay);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(file_name, "既知の(あるいは失われた)伝説のアイテム", 0, 0);
#else /* JP */
	show_file(file_name, "Known (or lost) artifacts", 0, 0);
#endif /* JP */

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known uniques
 *
 * Note that the player ghosts are ignored.  XXX XXX XXX
 */
static void do_cmd_knowledge_uniques(void)
{
	int i, n;
	FILE *fff;
	char file_name[1024];
	u16b why = 2;
	u16b *who;
	int killed = 0;
	char header[80];


	/* Temporary file */
	fff = my_fopen_temp(file_name, sizeof(file_name));

	/* Failure */
	if (!fff) return;

	/* Allocate the "who" array */
	C_MAKE(who, z_info->r_max, u16b);

	/* Collect matching monsters */
	for (i = 1, n = 0; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];
		monster_lore *l_ptr = &l_list[i];

		/* Require known monsters */
		if (!cheat_know && !l_ptr->sights) continue;

		/* Require unique monsters */
		if (!(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Collect "appropriate" monsters */
		who[n++] = i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, n);


	/* Print the monsters */
	for (i = 0; i < n; i++)
	{
		monster_race *r_ptr = &r_info[who[i]];
		bool dead = (r_ptr->max_num == 0);

		if (dead) killed++;

		/* Print a message */
#ifdef JP
		/* 英日切り替え機能対応 */
		fprintf(fff, "     %-40sは%s\n",
		        X_r_name(r_ptr),
		        (dead ? "既に死んでいる" : "まだ生きている"));
#else /* JP */
		fprintf(fff, "     %-30s is %s\n",
			    (r_name + r_ptr->name),
			    (dead ? "dead" : "alive"));
#endif /* JP */
	}

	/* Free the "who" array */
	FREE(who);

	/* Close the file */
	my_fclose(fff);

	/* Construct header line */
#ifdef JP
	strnfmt(header, sizeof(header), "ユニークモンスター: 既知 %d, 死亡 %d", n, killed);
#else /* JP */
	strnfmt(header, sizeof(header), "Uniques: %d known, %d killed", n, killed);
#endif /* JP */

	/* Display the file contents */
	show_file(file_name, header, 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known objects
 */
static void do_cmd_knowledge_objects(void)
{
	int k;

	FILE *fff;

	char o_name[80];

	char file_name[1024];


	/* Temporary file */
	fff = my_fopen_temp(file_name, sizeof(file_name));

	/* Failure */
	if (!fff) return;

	/* Scan the object kinds */
	for (k = 1; k < z_info->k_max; k++)
	{
		object_kind *k_ptr = &k_info[k];

		/* Hack -- skip artifacts */
		if (k_ptr->flags3 & (TR3_INSTA_ART)) continue;

		/* List known flavored objects */
		if (k_ptr->flavor && k_ptr->aware)
		{
			object_type *i_ptr;
			object_type object_type_body;

			/* Get local object */
			i_ptr = &object_type_body;

			/* Create fake object */
			object_prep(i_ptr, k);

			/* Describe the object */
			object_desc_spoil(o_name, sizeof(o_name), i_ptr, FALSE, 0);

			/* Print a message */
			fprintf(fff, "     %s\n", o_name);
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(file_name, "既知のアイテム", 0, 0);
#else /* JP */
	show_file(file_name, "Known Objects", 0, 0);
#endif /* JP */

	/* Remove the file */
	fd_kill(file_name);
}


#ifdef ALLOW_AUTO_PICKUP

/*
 * Check the status of "autopick"
 */
static void do_cmd_knowledge_autopick(void)
{
	int i;

	FILE *fff;

	char desc[1024];

	char file_name[1024];


	/* Temporary file */
	fff = my_fopen_temp(file_name, 1024);

	/* Failure */
	if (!fff) return;

	if (!max_autopick)
	{
#ifdef JP
	    fprintf(fff, "自動破壊/拾いには何も登録されていません。");
#else /* JP */
	    fprintf(fff, "No preference for auto picker/destroyer.");
#endif /* JP */
	}
	else
	{
#ifdef JP
	    fprintf(fff, "   自動拾い/破壊には現在 %d行登録されています。\n\n", max_autopick);
#else /* JP */
	    fprintf(fff, "   There are %d registered lines for auto picker/destroyer.\n\n", max_autopick);
#endif /* JP */
	}

	/* Scan the autopick list */
	for (i = 0; i < max_autopick; i++)
	{
		cptr tmp;

		autopick_type *ap_ptr = &autopick_list[i];


		/* print action */
		if (ap_ptr->act & AUTOPICK_ACT_PICKUP)
		{
#ifdef JP
			tmp = "拾う";
#else /* JP */
			tmp = "Pickup";
#endif /* JP */
		}
		else if (ap_ptr->act & AUTOPICK_ACT_DESTROY)
		{
#ifdef JP
			tmp = "破壊";
#else /* JP */
			tmp = "Destroy";
#endif /* JP */
		}
		else
		{
#ifdef JP
			tmp = "放置";
#else /* JP */
			tmp = "Leave";
#endif /* JP */
		}
		fprintf(fff, "%11s", format("[%s]", tmp));

		/* print description */
		autopick_desc(desc, ap_ptr, FALSE, FALSE);
		fprintf(fff, " %s", desc);

		/* print inscription */
		if (ap_ptr->insc)
		{
			fprintf(fff, " {%s}", ap_ptr->insc);
		}

		fprintf(fff, "\n");
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(file_name, "自動拾い/破壊 設定リスト", 0, 0);
#else /* JP */
	show_file(file_name, "Auto-picker/Destroyer", 0, 0);
#endif /* JP */

	/* Remove the file */
	fd_kill(file_name);
}

#endif /* ALLOW_AUTO_PICKUP */



/*
 * Display kill counts
 */
static void do_cmd_knowledge_kills(void)
{
	int n, i;

	FILE *fff;

	char file_name[1024];

	u16b *who;
	u16b why = 4;


	/* Temporary file */
	fff = my_fopen_temp(file_name, sizeof(file_name));

	/* Failure */
	if (!fff) return;


	/* Allocate the "who" array */
	C_MAKE(who, z_info->r_max, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < z_info->r_max - 1; i++)
	{
		monster_race *r_ptr = &r_info[i];
		monster_lore *l_ptr = &l_list[i];

		/* Require non-unique monsters */
		if (r_ptr->flags1 & RF1_UNIQUE) continue;

		/* Collect "appropriate" monsters */
		if (l_ptr->pkills > 0) who[n++] = i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort by kills (and level) */
	ang_sort(who, &why, n);

	/* Print the monsters (highest kill counts first) */
	for (i = n - 1; i >= 0; i--)
	{
		monster_race *r_ptr = &r_info[who[i]];
		monster_lore *l_ptr = &l_list[who[i]];

		/* Print a message */
		fprintf(fff, "     %-40s  %5d\n",
		        (r_name + r_ptr->name), l_ptr->pkills);
	}

	/* Free the "who" array */
	FREE(who);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(file_name, "倒したモンスター", 0, 0);
#else /* JP */
	show_file(file_name, "Kill counts", 0, 0);
#endif /* JP */

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Interact with "knowledge"
 */
void do_cmd_knowledge(void)
{
	char ch;


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save screen */
	screen_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("現在の知識を確認する", 2, 0);
#else /* JP */
		prt("Display current knowledge", 2, 0);
#endif /* JP */

		/* Give some choices */
#ifdef JP
		prt("(1) 既知の 伝説のアイテム       の一覧", 4, 5);
		prt("(2) 既知の ユニーク・モンスター の一覧", 5, 5);
		prt("(3) 既知の アイテム             の一覧", 6, 5);
		prt("(4) 勇者の殿堂                  の一覧", 7, 5);
		prt("(5) 倒したモンスター            の一覧", 8, 5);
#else /* JP */
		prt("(1) Display known artifacts", 4, 5);
		prt("(2) Display known uniques", 5, 5);
		prt("(3) Display known objects", 6, 5);
		prt("(4) Display hall of fame", 7, 5);
		prt("(5) Display kill counts", 8, 5);
#endif /* JP */
#ifdef ALLOW_AUTO_PICKUP
#ifdef JP
		prt("(6) 現在の自動拾い/破壊設定     の一覧", 9, 5);
#else /* JP */
		prt("(6) Display auto pick/destroy", 9, 5);
#endif /* JP */
#endif /* ALLOW_AUTO_PICKUP */

		/* Prompt */
#ifdef JP
		prt("コマンド: ", 10, 0);
#else /* JP */
		prt("Command: ", 10, 0);
#endif /* JP */

		/* Prompt */
		ch = inkey();

		/* Done */
		if (ch == ESCAPE) break;

		/* Artifacts */
		if (ch == '1')
		{
			do_cmd_knowledge_artifacts();
		}

		/* Uniques */
		else if (ch == '2')
		{
			do_cmd_knowledge_uniques();
		}

		/* Objects */
		else if (ch == '3')
		{
			do_cmd_knowledge_objects();
		}

		/* Scores */
		else if (ch == '4')
		{
			show_scores();
		}

		/* Scores */
		else if (ch == '5')
		{
			do_cmd_knowledge_kills();
		}

#ifdef ALLOW_AUTO_PICKUP
		/* Auto pick/destroy */
		else if (ch == '6')
		{
			/* Spawn */
			do_cmd_knowledge_autopick();
		}
#endif /* ALLOW_AUTO_PICKUP */

		/* Unknown option */
		else
		{
#ifdef JP
			bell("無効なキーです！");
#else /* JP */
			bell("Illegal command for knowledge!");
#endif /* JP */
		}

		/* Flush messages */
		message_flush();
	}


	/* Load screen */
	screen_load();
}
