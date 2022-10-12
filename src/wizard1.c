/* File: wizard1.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


#ifdef ALLOW_SPOILERS


/*
 * The spoiler file being created
 */
static FILE *fff = NULL;


/*
 * Write out `n' of the character `c' to the spoiler file
 */
static void spoiler_out_n_chars(int n, char c)
{
	while (--n >= 0) fputc(c, fff);
}

/*
 * Write out `n' blank lines to the spoiler file
 */
static void spoiler_blanklines(int n)
{
	spoiler_out_n_chars(n, '\n');
}

/*
 * Write a line to the spoiler file and then "underline" it with hypens
 */
static void spoiler_underline(cptr str, char c)
{
	text_out(str);
	text_out("\n");
	spoiler_out_n_chars(strlen(str), c);
	text_out("\n");
}


/*
 * A tval grouper
 */
typedef struct
{
	byte tval;
	cptr name;
} grouper;



/*
 * Item Spoilers by Ben Harrison (benh@phial.com)
 */


/*
 * The basic items categorized by type
 */
static const grouper group_item[] =
{
#ifdef JP
	{ TV_SHOT,		"�e" },
#else
	{ TV_SHOT,		"Ammo" },
#endif
	{ TV_ARROW,		  NULL },
	{ TV_BOLT,		  NULL },

#ifdef JP
	{ TV_BOW,		"�ˌ�����" },
#else
	{ TV_BOW,		"Bows" },
#endif

#ifdef JP
	{ TV_SWORD,		"����" },
#else
	{ TV_SWORD,		"Weapons" },
#endif
	{ TV_POLEARM,	  NULL },
	{ TV_HAFTED,	  NULL },
	{ TV_DIGGING,	  NULL },

#ifdef JP
	{ TV_SOFT_ARMOR,	"�h��i�́j" },
#else
	{ TV_SOFT_ARMOR,	"Armour (Body)" },
#endif
	{ TV_HARD_ARMOR,	  NULL },
	{ TV_DRAG_ARMOR,	  NULL },

#ifdef JP
	{ TV_CLOAK,		"�h��i���̑��j" },
#else
	{ TV_CLOAK,		"Armour (Misc)" },
#endif
	{ TV_SHIELD,	  NULL },
	{ TV_HELM,		  NULL },
	{ TV_CROWN,		  NULL },
	{ TV_GLOVES,	  NULL },
	{ TV_BOOTS,		  NULL },

#ifdef JP
	{ TV_AMULET,        "�A�~�����b�g" },
	{ TV_RING,          "�w��" },
#else
	{ TV_AMULET,	"Amulets" },
	{ TV_RING,		"Rings" },
#endif

#ifdef JP
	{ TV_SCROLL,        "����" },
	{ TV_POTION,        "��" },
	{ TV_FOOD,          "�H��" },
#else
	{ TV_SCROLL,	"Scrolls" },
	{ TV_POTION,	"Potions" },
	{ TV_FOOD,		"Food" },
#endif

#ifdef JP
        { TV_ROD,           "���b�h" },
	{ TV_WAND,          "���@�_" },
	{ TV_STAFF,         "��" },
#else
	{ TV_ROD,		"Rods" },
	{ TV_WAND,		"Wands" },
	{ TV_STAFF,		"Staffs" },
#endif

#ifdef JP
	{ TV_MAGIC_BOOK,	"���@�� (���C�W�j" },
	{ TV_PRAYER_BOOK,	"���@�� (�v���[�X�g�j" },
#else
	{ TV_MAGIC_BOOK,	"Books (Mage)" },
	{ TV_PRAYER_BOOK,	"Books (Priest)" },
#endif

#ifdef JP
	{ TV_CHEST,         "��" },
#else
	{ TV_CHEST,		"Chests" },
#endif

#ifdef JP
	{ TV_SPIKE,         "���̑�" },
#else
	{ TV_SPIKE,		"Various" },
#endif
	{ TV_LITE,		  NULL },
	{ TV_FLASK,		  NULL },
	{ TV_JUNK,		  NULL },
	{ TV_BOTTLE,	  NULL },
	{ TV_SKELETON,	  NULL },

	{ 0, "" }
};





/*
 * Describe the kind
 */
static void kind_info(char *buf, char *dam, char *wgt, int *lev, s32b *val, int k)
{
	object_kind *k_ptr;

	object_type *i_ptr;
	object_type object_type_body;


	/* Get local object */
	i_ptr = &object_type_body;

	/* Prepare a fake item */
	object_prep(i_ptr, k);

	/* Obtain the "kind" info */
	k_ptr = &k_info[i_ptr->k_idx];

	/* Cancel bonuses */
	i_ptr->pval = 0;
	i_ptr->to_a = 0;
	i_ptr->to_h = 0;
	i_ptr->to_d = 0;


	/* Level */
	(*lev) = k_ptr->level;

	/* Make known */
	i_ptr->ident |= (IDENT_KNOWN);
	
	/* Value */
	(*val) = object_value(i_ptr);


	/* Hack */
	if (!buf || !dam || !wgt) return;


	/* Description (too brief) */
	object_desc_spoil(buf, 80, i_ptr, FALSE, 0);


	/* Misc info */
	strcpy(dam, "");

	/* Damage */
	switch (i_ptr->tval)
	{
		/* Bows */
		case TV_BOW:
		{
			break;
		}

		/* Ammo */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		{
			sprintf(dam, "%dd%d", i_ptr->dd, i_ptr->ds);
			break;
		}

		/* Weapons */
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			sprintf(dam, "%dd%d", i_ptr->dd, i_ptr->ds);
			break;
		}

		/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			sprintf(dam, "%d", i_ptr->ac);
			break;
		}
	}


	/* Weight */
#ifdef JP
	sprintf(wgt, "%3d.%02d", lbtokg1(i_ptr->weight) , lbtokg2(i_ptr->weight));
#else
	sprintf(wgt, "%3d.%d", i_ptr->weight / 10, i_ptr->weight % 10);
#endif
}


/*
 * Create a spoiler file for items
 */
static void spoil_obj_desc(cptr fname)
{
	int i, k, s, t, n = 0;

	u16b who[200];

	char buf[1024];

	char wgt[80];
	char dam[80];

	cptr format = "%-51s  %7s%6s%4s%9s\n";

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����쐬�ł��܂���B");
#else
		msg_print("Cannot create spoiler file.");
#endif
		return;
	}


	/* Header */
	fprintf(fff, "Spoiler File -- Basic Items (%s)\n\n\n", VERSION_STRING);

	/* More Header */
	fprintf(fff, format, "Description", "Dam/AC", "Wgt", "Lev", "Cost");
	fprintf(fff, format, "----------------------------------------",
	        "------", "---", "---", "----");

	/* List the groups */
	for (i = 0; TRUE; i++)
	{
		/* Write out the group title */
		if (group_item[i].name)
		{
			/* Hack -- bubble-sort by cost and then level */
			for (s = 0; s < n - 1; s++)
			{
				for (t = 0; t < n - 1; t++)
				{
					int i1 = t;
					int i2 = t + 1;

					int e1;
					int e2;

					s32b t1;
					s32b t2;

					kind_info(NULL, NULL, NULL, &e1, &t1, who[i1]);
					kind_info(NULL, NULL, NULL, &e2, &t2, who[i2]);

					if ((t1 > t2) || ((t1 == t2) && (e1 > e2)))
					{
						int tmp = who[i1];
						who[i1] = who[i2];
						who[i2] = tmp;
					}
				}
			}

			/* Spoil each item */
			for (s = 0; s < n; s++)
			{
				int e;
				s32b v;

				/* Describe the kind */
				kind_info(buf, dam, wgt, &e, &v, who[s]);

				/* Dump it */
				fprintf(fff, "  %-51s%7s%6s%4d%9ld\n",
				        buf, dam, wgt, e, (long)(v));
			}

			/* Start a new set */
			n = 0;

			/* Notice the end */
			if (!group_item[i].tval) break;

			/* Start a new set */
			fprintf(fff, "\n\n%s\n\n", group_item[i].name);
		}

		/* Get legal item types */
		for (k = 1; k < z_info->k_max; k++)
		{
			object_kind *k_ptr = &k_info[k];

			/* Skip wrong tval's */
			if (k_ptr->tval != group_item[i].tval) continue;

			/* Hack -- Skip instant-artifacts */
			if (k_ptr->flags3 & (TR3_INSTA_ART)) continue;

			/* Save the index */
			who[n++] = k;
		}
	}


	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����N���[�Y�ł��܂���B");
#else
		msg_print("Cannot close spoiler file.");
#endif
		return;
	}

	/* Message */
#ifdef JP
	msg_print("�X�|�C���[�t�@�C���̍쐬�ɐ������܂����B");
#else
	msg_print("Successfully created a spoiler file.");
#endif
}



/*
 * Artifact Spoilers by: randy@PICARD.tamu.edu (Randy Hutson)
 *
 * (Mostly) rewritten in 2002 by Andrew Sidwell and Robert Ruehlmann.
 */


/*
 * The artifacts categorized by type
 */
static const grouper group_artifact[] =
{
#ifdef JP
	{ TV_SWORD,             "�n�̂�������" },
	{ TV_POLEARM,           "�Ə󕐊�" },
	{ TV_HAFTED,            "�݊�" },
	{ TV_BOW,               "�|" },
	{ TV_DIGGING,		"�̌@����" },
#else
	{ TV_SWORD,         "Edged Weapons" },
	{ TV_POLEARM,       "Polearms" },
	{ TV_HAFTED,        "Hafted Weapons" },
	{ TV_BOW,           "Bows" },
	{ TV_DIGGING,       "Diggers" },
#endif

#ifdef JP
	{ TV_SOFT_ARMOR,        "�h�� (��)" },
#else
	{ TV_SOFT_ARMOR,    "Body Armor" },
#endif
	{ TV_HARD_ARMOR,    NULL },
	{ TV_DRAG_ARMOR,    NULL },

#ifdef JP
	{ TV_CLOAK,             "�N���[�N" },
	{ TV_SHIELD,            "�V�[���h" },
	{ TV_HELM,              "�X�q/��" },
	{ TV_CROWN,             NULL },
	{ TV_GLOVES,            "�O���[�u" },
	{ TV_BOOTS,             "�u�[�c" },
#else
	{ TV_CLOAK,         "Cloaks" },
	{ TV_SHIELD,        "Shields" },
	{ TV_HELM,          "Helms/Crowns" },
	{ TV_CROWN,         NULL },
	{ TV_GLOVES,        "Gloves" },
	{ TV_BOOTS,         "Boots" },
#endif

#ifdef JP
	{ TV_LITE,              "����" },
	{ TV_AMULET,            "�A�~�����b�g" },
	{ TV_RING,              "�w��" },
#else
	{ TV_LITE,          "Light Sources" },
	{ TV_AMULET,        "Amulets" },
	{ TV_RING,          "Rings" },
#endif

	{ 0, NULL }
};


/*
 * Hack -- Create a "forged" artifact
 */
static bool make_fake_artifact(object_type *o_ptr, byte name1)
{
	int i;

	artifact_type *a_ptr = &a_info[name1];


	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return FALSE;

	/* Get the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	if (!i) return (FALSE);

	/* Create the artifact */
	object_prep(o_ptr, i);

	/* Save the name */
	o_ptr->name1 = name1;

	/* Extract the fields */
	o_ptr->pval = a_ptr->pval;
	o_ptr->ac = a_ptr->ac;
	o_ptr->dd = a_ptr->dd;
	o_ptr->ds = a_ptr->ds;
	o_ptr->to_a = a_ptr->to_a;
	o_ptr->to_h = a_ptr->to_h;
	o_ptr->to_d = a_ptr->to_d;
	o_ptr->weight = a_ptr->weight;

	/* Hack -- extract the "cursed" flag */
	if (a_ptr->flags3 & (TR3_LIGHT_CURSE)) o_ptr->ident |= (IDENT_CURSED);

	/* Success */
	return (TRUE);
}


/*
 * Create a spoiler file for artifacts
 */
static void spoil_artifact(cptr fname)
{
	int i, j;

	object_type *i_ptr;
	object_type object_type_body;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����쐬�ł��܂���B");
#else
		msg_print("Cannot create spoiler file.");
#endif
		return;
	}

	/* Dump to the spoiler file */
	text_out_hook = text_out_to_file;
	text_out_file = fff;

	/* Set object_info_out() hook */
	object_info_out_flags = object_flags;

	/* Dump the header */
	spoiler_underline(format("Artifact Spoilers for %s %s",
	                         VERSION_NAME, VERSION_STRING), '=');

	/* List the artifacts by tval */
	for (i = 0; group_artifact[i].tval; i++)
	{
		/* Write out the group title */
		if (group_artifact[i].name)
		{
			spoiler_blanklines(2);
			spoiler_underline(group_artifact[i].name, '=');
			spoiler_blanklines(1);
		}

		/* Now search through all of the artifacts */
		for (j = 1; j < z_info->a_max; ++j)
		{
			artifact_type *a_ptr = &a_info[j];
			char buf[80];

			/* We only want objects in the current group */
			if (a_ptr->tval != group_artifact[i].tval) continue;

			/* Get local object */
			i_ptr = &object_type_body;

			/* Wipe the object */
			object_wipe(i_ptr);

			/* Attempt to "forge" the artifact */
			if (!make_fake_artifact(i_ptr, (byte)j)) continue;

			/* Grab artifact name */
			object_desc_spoil(buf, sizeof(buf), i_ptr, TRUE, 1);

			/* Print name and underline */
			spoiler_underline(buf, '-');

			/* Write out the artifact description to the spoiler file */
			object_info_out(i_ptr);

			/*
			 * Determine the minimum depth an artifact can appear, its rarity,
			 * its weight, and its value in gold pieces.
			 */
			text_out(format("\nLevel %u, Rarity %u, %d.%d lbs, %ld AU\n",
			                a_ptr->level, a_ptr->rarity, (a_ptr->weight / 10),
			                (a_ptr->weight % 10), ((long)a_ptr->cost)));

			/* Terminate the entry */
			spoiler_blanklines(2);
		}
	}

	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����N���[�Y�ł��܂���B");
#else
		msg_print("Cannot close spoiler file.");
#endif
		return;
	}

	/* Message */
#ifdef JP
	msg_print("�X�|�C���[�t�@�C���̍쐬�ɐ������܂����B");
#else
	msg_print("Successfully created a spoiler file.");
#endif
}





/*
 * Create a spoiler file for monsters
 */
static void spoil_mon_desc(cptr fname)
{
	int i, n = 0;

	char buf[1024];

	char nam[80];
	char lev[80];
	char rar[80];
	char spd[80];
	char ac[80];
	char hp[80];
	char exp[80];

	u16b *who;
	u16b why = 2;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����쐬�ł��܂���B");
#else
		msg_print("Cannot create spoiler file.");
#endif
		return;
	}

	/* Dump the header */
	fprintf(fff, "Monster Spoilers for %s Version %s\n",
	        VERSION_NAME, VERSION_STRING);
	fprintf(fff, "------------------------------------------\n\n");

	/* Dump the header */
	fprintf(fff, "%-40.40s%4s%4s%6s%8s%4s  %11.11s\n",
	        "Name", "Lev", "Rar", "Spd", "Hp", "Ac", "Visual Info");
	fprintf(fff, "%-40.40s%4s%4s%6s%8s%4s  %11.11s\n",
	        "----", "---", "---", "---", "--", "--", "-----------");

	/* Allocate the "who" array */
	C_MAKE(who, z_info->r_max, u16b);

	/* Scan the monsters (except the ghost) */
	for (i = 1; i < z_info->r_max - 1; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[n++] = (u16b)i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, n);

	/* Scan again */
	for (i = 0; i < n; i++)
	{
		monster_race *r_ptr = &r_info[who[i]];

#ifdef JP
		cptr name = X_r_name(r_ptr);
#else
		cptr name = (r_name + r_ptr->name);
#endif

		/* Get the "name" */
		if (r_ptr->flags1 & (RF1_QUESTOR))
		{
			strnfmt(nam, sizeof(nam), "[Q] %s", name);
		}
		else if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			strnfmt(nam, sizeof(nam), "[U] %s", name);
		}
		else
		{
#ifdef JP
			strnfmt(nam, sizeof(nam), X_monster("The %s", "    %s"), name);
#else /* JP */
			strnfmt(nam, sizeof(nam), "The %s", name);
#endif /* JP */
		}


		/* Level */
		sprintf(lev, "%d", r_ptr->level);

		/* Rarity */
		sprintf(rar, "%d", r_ptr->rarity);

		/* Speed */
		if (r_ptr->speed >= 110)
		{
			sprintf(spd, "+%d", (r_ptr->speed - 110));
		}
		else
		{
			sprintf(spd, "-%d", (110 - r_ptr->speed));
		}

		/* Armor Class */
		sprintf(ac, "%d", r_ptr->ac);

		/* Hitpoints */
		if ((r_ptr->flags1 & (RF1_FORCE_MAXHP)) || (r_ptr->hside == 1))
		{
			sprintf(hp, "%d", r_ptr->hdice * r_ptr->hside);
		}
		else
		{
			sprintf(hp, "%dd%d", r_ptr->hdice, r_ptr->hside);
		}


		/* Experience */
		sprintf(exp, "%ld", (long)(r_ptr->mexp));

		/* Hack -- use visual instead */
		strnfmt(exp, sizeof(exp), "%s '%c'", attr_to_text(r_ptr->d_attr), r_ptr->d_char);

		/* Dump the info */
		fprintf(fff, "%-40.40s%4s%4s%6s%8s%4s  %11.11s\n",
		        nam, lev, rar, spd, hp, ac, exp);
	}

	/* End it */
	fprintf(fff, "\n");

	/* Free the "who" array */
	FREE(who);


	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����N���[�Y�ł��܂���B");
#else
		msg_print("Cannot close spoiler file.");
#endif
		return;
	}

	/* Worked */
#ifdef JP
	msg_print("�X�|�C���[�t�@�C���̍쐬�ɐ������܂����B");
#else
	msg_print("Successfully created a spoiler file.");
#endif
}




/*
 * Monster spoilers originally by: smchorse@ringer.cs.utsa.edu (Shawn McHorse)
 */


/*
 * Create a spoiler file for monsters (-SHAWN-)
 */
static void spoil_mon_info(cptr fname)
{
	char buf[1024];
	int i, n;
	u16b why = 2;
	u16b *who;
	int count = 0;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����쐬�ł��܂���B");
#else
		msg_print("Cannot create spoiler file.");
#endif
		return;
	}

	/* Dump to the spoiler file */
	text_out_hook = text_out_to_file;
	text_out_file = fff;

	/* Dump the header */
	strnfmt(buf, sizeof(buf), "Monster Spoilers for %s Version %s\n",
	        VERSION_NAME, VERSION_STRING);
	text_out(buf);
	text_out("------------------------------------------\n\n");

	/* Allocate the "who" array */
	C_MAKE(who, z_info->r_max, u16b);

	/* Scan the monsters */
	for (i = 1; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[count++] = (u16b)i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, count);

	/*
	 * List all monsters in order (except the ghost).
	 */
	for (n = 0; n < count; n++)
	{
		int r_idx = who[n];
		monster_race *r_ptr = &r_info[r_idx];

		/* Prefix */
		if (r_ptr->flags1 & RF1_QUESTOR)
		{
			text_out("[Q] ");
		}
		else if (r_ptr->flags1 & RF1_UNIQUE)
		{
			text_out("[U] ");
		}
		else
		{
#ifndef JP
			text_out("The ");
#endif
		}

		/* Name */
#ifdef JP
		strnfmt(buf, sizeof(buf), "%s/%s  (", (r_name + r_ptr->J_name), (r_name+r_ptr->name));	/* ---)--- */
#else
		strnfmt(buf, sizeof(buf), "%s  (", (r_name + r_ptr->name));	/* ---)--- */
#endif
		text_out(buf);

		/* Color */
		text_out(attr_to_text(r_ptr->d_attr));

		/* Symbol --(-- */
		sprintf(buf, " '%c')\n", r_ptr->d_char);
		text_out(buf);


		/* Indent */
		sprintf(buf, "=== ");
		text_out(buf);

		/* Number */
#ifdef JP
		sprintf(buf, "�ԍ�:%d  ", r_idx);
#else
		sprintf(buf, "Num:%d  ", r_idx);
#endif
		text_out(buf);

		/* Level */
#ifdef JP
		sprintf(buf, "�K:%d  ", r_ptr->level);
#else
		sprintf(buf, "Lev:%d  ", r_ptr->level);
#endif
		text_out(buf);

		/* Rarity */
#ifdef JP
		sprintf(buf, "�H���x:%d  ", r_ptr->rarity);
#else
		sprintf(buf, "Rar:%d  ", r_ptr->rarity);
#endif
		text_out(buf);

		/* Speed */
		if (r_ptr->speed >= 110)
		{
#ifdef JP
			sprintf(buf, "�X�s�[�h:+%d  ", (r_ptr->speed - 110));
#else
			sprintf(buf, "Spd:+%d  ", (r_ptr->speed - 110));
#endif
		}
		else
		{
#ifdef JP
			sprintf(buf, "�X�s�[�h:-%d  ", (110 - r_ptr->speed));
#else
			sprintf(buf, "Spd:-%d  ", (110 - r_ptr->speed));
#endif
		}
		text_out(buf);

		/* Hitpoints */
		if ((r_ptr->flags1 & RF1_FORCE_MAXHP) || (r_ptr->hside == 1))
		{
#ifdef JP
			sprintf(buf, "HP:%d  ", r_ptr->hdice * r_ptr->hside);
#else
			sprintf(buf, "Hp:%d  ", r_ptr->hdice * r_ptr->hside);
#endif
		}
		else
		{
#ifdef JP
			sprintf(buf, "HP:%dd%d  ", r_ptr->hdice, r_ptr->hside);
#else
			sprintf(buf, "Hp:%dd%d  ", r_ptr->hdice, r_ptr->hside);
#endif
		}
		text_out(buf);

		/* Armor Class */
#ifdef JP
		sprintf(buf, "AC:%d  ", r_ptr->ac);
#else
		sprintf(buf, "Ac:%d  ", r_ptr->ac);
#endif
		text_out(buf);

		/* Experience */
#ifdef JP
		sprintf(buf, "�o���l:%ld\n", (long)(r_ptr->mexp));
#else
		sprintf(buf, "Exp:%ld\n", (long)(r_ptr->mexp));
#endif
		text_out(buf);

		/* Describe */
		describe_monster(r_idx, TRUE);

		/* Terminate the entry */
		text_out("\n");
	}

	/* Free the "who" array */
	FREE(who);

	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
#ifdef JP
		msg_print("�X�|�C���[�t�@�C�����N���[�Y�ł��܂���B");
#else
		msg_print("Cannot close spoiler file.");
#endif
		return;
	}

#ifdef JP
	msg_print("�X�|�C���[�t�@�C���̍쐬�ɐ������܂����B");
#else
	msg_print("Successfully created a spoiler file.");
#endif
}



/*
 * Create Spoiler files
 */
void do_cmd_spoilers(void)
{
	char ch;


	/* Save screen */
	screen_save();


	/* Interact */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Info */
#ifdef JP
		prt("�X�|�C���[�t�@�C���쐬", 2, 0);
#else
		prt("Create a spoiler file.", 2, 0);
#endif

		/* Prompt for a file */
#ifdef JP
		prt("(1) �Ȍ��ȃA�C�e����� (obj-desc.spo)", 5, 5);
		prt("(2) �Ȍ��ȓ`���̃A�C�e����� (artifact.spo)", 6, 5);
		prt("(3) �Ȍ��ȃ����X�^�[��� (mon-desc.spo)", 7, 5);
		prt("(4) ���S�ȃ����X�^�[��� (mon-info.spo)", 8, 5);
#else
		prt("(1) Brief Object Info (obj-desc.spo)", 5, 5);
		prt("(2) Brief Artifact Info (artifact.spo)", 6, 5);
		prt("(3) Brief Monster Info (mon-desc.spo)", 7, 5);
		prt("(4) Full Monster Info (mon-info.spo)", 8, 5);
#endif

		/* Prompt */
#ifdef JP
		prt("�R�}���h:", 18, 0);
#else
		prt("Command: ", 12, 0);
#endif


		/* Get a choice */
		ch = inkey();

		/* Escape */
		if (ch == ESCAPE)
		{
			break;
		}

		/* Option (1) */
		else if (ch == '1')
		{
			spoil_obj_desc("obj-desc.spo");
		}

		/* Option (2) */
		else if (ch == '2')
		{
			spoil_artifact("artifact.spo");
		}

		/* Option (3) */
		else if (ch == '3')
		{
			spoil_mon_desc("mon-desc.spo");
		}

		/* Option (4) */
		else if (ch == '4')
		{
			spoil_mon_info("mon-info.spo");
		}

		/* Oops */
		else
		{
#ifdef JP
			bell("�����ȃR�}���h�ł��I");
#else
			bell("Illegal command for spoilers!");
#endif
		}

		/* Flush messages */
		message_flush();
	}


	/* Load screen */
	screen_load();
}


#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif
