/* File: autopick.c */

/*
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

#ifdef ALLOW_AUTO_PICKUP

#define AUTOPICK_ALL                    0x00000001L
#define AUTOPICK_UNIDENTIFIED           0x00000002L
#define AUTOPICK_IDENTIFIED             0x00000004L
#define AUTOPICK_FULLY_IDENTIFIED       0x00000008L
#define AUTOPICK_UNAWARE                0x00000010L
#define AUTOPICK_WORTHLESS              0x00000020L
#define AUTOPICK_NAMELESS               0x00000040L
#define AUTOPICK_ARTIFACT               0x00000080L
#define AUTOPICK_EGO                    0x00000100L
#define AUTOPICK_CARRYING               0x00000200L
#define AUTOPICK_DICE_BOOSTED           0x00000400L
#define AUTOPICK_MORE_THAN_NN_DICE      0x00000800L
#define AUTOPICK_UNREADABLE             0x00001000L
#define AUTOPICK_NTH_BOOK               0x00002000L

#define AUTOPICK_TYPE_NONE              0
#define AUTOPICK_TYPE_ITEM              1
#define AUTOPICK_TYPE_TVAL              2
#define AUTOPICK_TYPE_WEAPON            3
#define AUTOPICK_TYPE_AMMO              4
#define AUTOPICK_TYPE_ARMOR             5
#define AUTOPICK_TYPE_BODY_ARMOR        6
#define AUTOPICK_TYPE_HELM              7
#define AUTOPICK_TYPE_DEVICE            8
#define AUTOPICK_TYPE_SPELLBOOK         9
#define AUTOPICK_TYPE_JUNK              10

struct autopick_keyword
{
	cptr name;
	u32b flag;
};

static struct autopick_keyword autopick_flag_group[] = 
{
#ifdef JP
	{ "すべての",           AUTOPICK_ALL },
	{ "未鑑定の",           AUTOPICK_UNIDENTIFIED },
	{ "鑑定済みの",         AUTOPICK_IDENTIFIED },
	{ "*鑑定*済みの",       AUTOPICK_FULLY_IDENTIFIED },
	{ "未判明の",           AUTOPICK_UNAWARE },
	{ "無価値の",           AUTOPICK_WORTHLESS },
	{ "無銘の",             AUTOPICK_NAMELESS },
	{ "収集中の",           AUTOPICK_CARRYING },
	{ "ダイス目の違う",     AUTOPICK_DICE_BOOSTED },
	{ "特別製の",           AUTOPICK_ARTIFACT },
	{ "伝説の",             AUTOPICK_ARTIFACT },
	{ "高級品の",           AUTOPICK_EGO },
	{ "名のある",           AUTOPICK_EGO },
	{ "読めない",           AUTOPICK_UNREADABLE },
#endif
	{ "all",                AUTOPICK_ALL },
	{ "unidentified",       AUTOPICK_UNIDENTIFIED },
	{ "identified",         AUTOPICK_IDENTIFIED },
	{ "*identified*",       AUTOPICK_FULLY_IDENTIFIED },
	{ "unaware",            AUTOPICK_UNAWARE },
	{ "worthless",          AUTOPICK_WORTHLESS },
	{ "nameless",           AUTOPICK_NAMELESS },
	{ "collecting",         AUTOPICK_CARRYING },
	{ "dice boosted",       AUTOPICK_DICE_BOOSTED },
	{ "artifact",           AUTOPICK_ARTIFACT },
	{ "ego",                AUTOPICK_EGO },
	{ "special",            AUTOPICK_ARTIFACT },
	{ "excellent",          AUTOPICK_EGO },
	{ "unreadable",         AUTOPICK_UNREADABLE },
	{ NULL,                 0 },
};

static struct autopick_keyword autopick_type_group[] = 
{
#ifdef JP
	{ "アイテム",           AUTOPICK_TYPE_ITEM },
	{ "武器",               AUTOPICK_TYPE_WEAPON},
	{ "矢",                 AUTOPICK_TYPE_AMMO},
	{ "防具",               AUTOPICK_TYPE_ARMOR},
	{ "鎧",                 AUTOPICK_TYPE_BODY_ARMOR},
	{ "兜",                 AUTOPICK_TYPE_HELM},
	{ "魔法アイテム",       AUTOPICK_TYPE_DEVICE},
	{ "魔法書",             AUTOPICK_TYPE_SPELLBOOK},
	{ "がらくた",           AUTOPICK_TYPE_JUNK},
#endif
	{ "items",              AUTOPICK_TYPE_ITEM },
	{ "weapons",            AUTOPICK_TYPE_WEAPON},
	{ "ammos",              AUTOPICK_TYPE_AMMO},
	{ "missiles",           AUTOPICK_TYPE_AMMO},
	{ "armors",             AUTOPICK_TYPE_ARMOR},
	{ "armours",            AUTOPICK_TYPE_ARMOR},
	{ "suits",              AUTOPICK_TYPE_BODY_ARMOR},
	{ "helms",              AUTOPICK_TYPE_HELM},
	{ "magical devices",    AUTOPICK_TYPE_DEVICE},
	{ "spellbooks",         AUTOPICK_TYPE_SPELLBOOK},
	{ "junks",              AUTOPICK_TYPE_JUNK},

	{ NULL,                 0 },
};

struct tval_name
{
	byte tval;
	cptr name;
};

static struct tval_name tval_name[] = 
{
#ifdef JP
	{TV_HAFTED, "鈍器"},
	{TV_BOW, "弓"},
	{TV_SHIELD, "盾"},
	{TV_RING, "指輪"},
	{TV_CLOAK, "クローク"},
	{TV_GLOVES, "籠手"},
	{TV_BOOTS, "靴"},
	{TV_LITE, "光源"},
	{TV_AMULET, "アミュレット"},
	{TV_POTION, "薬"},
#endif
	{TV_HAFTED, "hafted weapons"},
	{TV_BOW, "bows"},
	{TV_SHIELD, "shields"},
	{TV_RING, "rings"},
	{TV_CLOAK, "cloaks"},
	{TV_GLOVES, "gloves"},
	{TV_BOOTS, "boots"},
	{TV_LITE, "lights"},
	{TV_AMULET, "amulets"},
	{TV_POTION, "potions"},

	{0, ""},
};

static cptr ordinal[] =
{
	"first", "second", "third", "fourth", "fifth",
	"sixth", "seventh", "eighth", "ninth"
};


/*
 * A function to create new autopick info
 */
bool make_autopick(autopick_type *ap_ptr, cptr str)
{
	int i;
	cptr s, t;
	char *s2;
	char buf[1024];

	u32b flags = 0;
	u32b type = 0;
	byte act = AUTOPICK_ACT_PICKUP | AUTOPICK_ACT_DISPLAY;
	cptr name = NULL;
	cptr insc = NULL;
	byte tval = 0;
	byte sval = 0;
	s16b dice = 0;

	/* Action */
	while (strchr("!~;(", *str))
	{
		switch (*str)
		{
			case '!':
				act &= ~(AUTOPICK_ACT_PICKUP | AUTOPICK_ACT_LEAVE);
				act |= AUTOPICK_ACT_DESTROY;
				break;

			case '~':
				act &= ~(AUTOPICK_ACT_PICKUP | AUTOPICK_ACT_DESTROY);
				act |= AUTOPICK_ACT_LEAVE;
				break;

			case ';':
				act |= AUTOPICK_ACT_QUERY;
				break;

			case '(':
				act &= ~AUTOPICK_ACT_DISPLAY;
				break;

			default:
				msg_format("*BUG*: unknown action('%s').", *str);
				break;
		}
		str++;
	}

	strncpy(buf, str, sizeof(buf));
	buf[sizeof(buf)-1] = '\0';

	/* Auto-inscription? */
#ifdef JP
	s2 = jstrstr(buf, "#");
#else /* JP */
	s2 = strstr(buf, "#");
#endif /* JP */
	if (s2)
	{
		/* Save inscription */
		*s2 = '\0';
		insc = s2 + 1;
	}

	/* Skip empty line */
	if (buf[0] == 0) return FALSE;

	/* don't mind upper or lower case */
	string_lower(buf);

	s = buf;

	/*** check flags ***/
	while (*s)
	{
		bool found = FALSE;
		for (i = 0; autopick_flag_group[i].name; i++)
		{
			if (prefix(s, autopick_flag_group[i].name))
			{
				flags |= autopick_flag_group[i].flag;
				s += strlen(autopick_flag_group[i].name);
				while (*s == ' ') s++;
				found = TRUE;
			}
		}

		/* Hack -- Nth spellbook */
#ifdef JP
		if (isdigit((unsigned char)*s) && prefix(s + 1, "冊目の"))
		{
			flags |= AUTOPICK_NTH_BOOK;
			sval = *s - '1';
			s += strlen("冊目の") + 1;
			while (*s == ' ') s++;
			found = TRUE;
		}
#endif

		for (i = 0; i < 9; i++)
		{
			if (prefix(s, ordinal[i]))
			{
				flags |= AUTOPICK_NTH_BOOK;
				sval = i;
				s += strlen(ordinal[i]);
				while (*s == ' ') s++;
				found = TRUE;
				break;
			}
		}

		/* Hack -- more than NN dice */
#ifdef JP
		if (prefix(s, "ダイス目") &&
		    isdigit((unsigned char)*(s+8)))
		{
			flags |= AUTOPICK_MORE_THAN_NN_DICE;
			s += strlen("ダイス目");
			for (dice = 0; isascii((unsigned char)*s) && isdigit((unsigned char)*s); s++)
			{
				dice = dice * 10 + D2I(*s);
			}
			while (*s == ' ') s++;
			/* Optional string */
			if (prefix(s, "以上の"))
			{
				s += strlen("以上の");
			}
			while (*s == ' ') s++;
			found = TRUE;
		}
#endif

		if (prefix(s, "more than") &&
		    isdigit((unsigned char)*(s+8)))
		{
			flags |= AUTOPICK_MORE_THAN_NN_DICE;
			s += strlen("more than");
			while (*s == ' ') s++;
			for (dice = 0; isascii((unsigned char)*s) && isdigit((unsigned char)*s); s++)
			{
				dice = dice * 10 + D2I(*s);
			}
			while (*s == ' ') s++;
			/* Optional string */
			if (prefix(s, "dice"))
			{
				s += strlen("dice");
			}
			while (*s == ' ') s++;
			found = TRUE;
		}

		if (!found) break;
	}

	/* Save name */
	t = s;

	/*** check type ***/
	for (i = 0; autopick_type_group[i].name; i++)
	{
		if (prefix(s, autopick_type_group[i].name))
		{
			type = autopick_type_group[i].flag;
			s += strlen(autopick_type_group[i].name);
			goto make_autopick_type_finish;
		}
	}

	for (i = 0; tval_name[i].tval; i++)
	{
		if (prefix(s, tval_name[i].name))
		{
			type = AUTOPICK_TYPE_TVAL;
			tval = tval_name[i].tval;
			s += strlen(tval_name[i].name);
			goto make_autopick_type_finish;
		}
	}

make_autopick_type_finish:

	/* Skip separator */
	if (type)
	{
		if (*s == ':') s++;
#ifdef JP
		else if (prefix(s, "：")) s += 2;
#endif
		else if (*s)
		{
			/* A type keyword didn't end correctly */
			s = t;
			type = 0;
			tval = 0;
		}
	}

	/* Save name */
	name = s;

	ap_ptr->name  = string_make(name);
	ap_ptr->insc  = string_make(insc);
	ap_ptr->act   = act;
	ap_ptr->flags = flags;
	ap_ptr->type  = type;
	ap_ptr->tval  = tval;
	ap_ptr->sval  = sval;
	ap_ptr->dice  = dice;

	return TRUE;
}

/*
 * Initialize auto-picker preference
 */
void init_autopicker(void)
{
	int i;

	/* いままで使っていたメモリ解放 */
	for( i = 0; i < max_autopick; i++)
	{
		autopick_type *ap_ptr = &autopick_list[i];

		string_free(ap_ptr->name);
		string_free(ap_ptr->insc);
	}

	max_autopick = 0;
}

/*
 * A function for Auto-picker/destroyer
 * Examine whether the object matches to the list of keywords or not.
 */
static int is_autopick(const object_type *o_ptr)
{
	int i;

	char o_name[80];
#ifdef JP
	char J_o_name[80];
#endif

	bool carried = FALSE;

	if (o_ptr->tval == TV_GOLD) return -1;

	/* Description */
#ifdef JP
	E_object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 3);
	J_object_desc(J_o_name, sizeof(J_o_name), o_ptr, FALSE, 3);
#else
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 3);
#endif

	/* Lowercase the name */
	string_lower(o_name);

	/* Check if there is a same item */
	carried = FALSE;
	for (i = 0; i < INVEN_PACK; i++)
	{
		if (object_similar(&inventory[i], o_ptr))
		{
			carried = TRUE;
			break;
		}
	}

	/* Scan all autopick info */
	for (i = 0; i < max_autopick; i++)
	{
		autopick_type *ap_ptr = &autopick_list[i];

		/*** Unidentified ***/
		if (ap_ptr->flags & AUTOPICK_UNIDENTIFIED)
		{
			if ((object_known_p(o_ptr)) ||
			    (o_ptr->ident & IDENT_SENSE)) continue;
		}

		/*** Identified ***/
		if (ap_ptr->flags & AUTOPICK_IDENTIFIED)
		{
			if (!(object_known_p(o_ptr))) continue;
		}

		/*** *Identified* ***/
		if (ap_ptr->flags & AUTOPICK_FULLY_IDENTIFIED)
		{
			if (!(object_known_p(o_ptr)) ||
			    (o_ptr->ident & IDENT_MENTAL)) continue;
		}

		/*** Unaware items ***/
		if (ap_ptr->flags & AUTOPICK_UNAWARE)
		{
			if (object_aware_p(o_ptr)) continue;
		}

		/*** Worthless items ***/
		if (ap_ptr->flags & AUTOPICK_WORTHLESS)
		{
			if (object_value(o_ptr) > 0) continue;
		}

		/*** Nameless ***/
		if (ap_ptr->flags & AUTOPICK_NAMELESS)
		{
			if ((o_ptr->tval != TV_SHOT) &&
			    (o_ptr->tval != TV_ARROW) &&
			    (o_ptr->tval != TV_BOLT) &&
			    (o_ptr->tval != TV_BOW) && 
			    (o_ptr->tval != TV_DIGGING) &&
			    (o_ptr->tval != TV_HAFTED) &&
			    (o_ptr->tval != TV_POLEARM) &&
			    (o_ptr->tval != TV_SWORD) &&  
			    (o_ptr->tval != TV_BOOTS) &&
			    (o_ptr->tval != TV_GLOVES) &&
			    (o_ptr->tval != TV_HELM) &&
			    (o_ptr->tval != TV_CROWN) && 
			    (o_ptr->tval != TV_SHIELD) &&
			    (o_ptr->tval != TV_CLOAK) && 
			    (o_ptr->tval != TV_SOFT_ARMOR) &&
			    (o_ptr->tval != TV_HARD_ARMOR) &&
			    (o_ptr->tval != TV_DRAG_ARMOR) && 
			    (o_ptr->tval != TV_LITE) &&
			    (o_ptr->tval != TV_AMULET) &&
			    (o_ptr->tval != TV_RING)) continue;

			/* skip unknown */
			if (!(object_known_p(o_ptr))) continue;

			/* skip inscribed */
			if (o_ptr->note) continue;

			/* skip artifacts and ego-objects */
			if (artifact_p(o_ptr) || ego_item_p(o_ptr)) continue;
		}

		/*** Artifact object ***/
		if (ap_ptr->flags & AUTOPICK_ARTIFACT)
		{
			/* skip unknown */
			if (!(object_known_p(o_ptr))) continue;

			/* skip non artifacts */
			if (!artifact_p(o_ptr)) continue;
		}

		/*** Ego object ***/
		if (ap_ptr->flags & AUTOPICK_EGO)
		{
			/* skip unknown */
			if (!(object_known_p(o_ptr))) continue;

			/* skip non ego-objects */
			if (!ego_item_p(o_ptr)) continue;
		}

		/* Check if there is a same item */
		if (ap_ptr->flags & AUTOPICK_CARRYING)
		{
			/* skip if not carried */
			if (!carried) continue;
		}

		/*** Dice boosted ***/
		if (ap_ptr->flags & AUTOPICK_DICE_BOOSTED)
		{
			object_kind *k_ptr = &k_info[o_ptr->k_idx];

			/* skip if dice is not boosted */
			if ((o_ptr->dd == k_ptr->dd) &&
			    (o_ptr->ds == k_ptr->ds)) continue;
		}

		/*** Weapons of which dd*ds is more than nn ***/
		if (ap_ptr->flags & AUTOPICK_MORE_THAN_NN_DICE)
		{
			s16b dice = o_ptr->dd * o_ptr->ds;

			if (dice < ap_ptr->dice) continue;
		}

		/*** Unreadable Spellbook ***/
		if (ap_ptr->flags & AUTOPICK_UNREADABLE)
		{
			if ((o_ptr->tval != TV_MAGIC_BOOK) &&
			    (o_ptr->tval != TV_PRAYER_BOOK)) continue;

			if (o_ptr->tval == cp_ptr->spell_book) continue;
		}

		/*** Nth Magic book ***/
		if (ap_ptr->flags & AUTOPICK_NTH_BOOK)
		{
			if ((o_ptr->tval != TV_MAGIC_BOOK) &&
			    (o_ptr->tval != TV_PRAYER_BOOK)) continue;

			if (o_ptr->sval != ap_ptr->sval) continue;
		}

		switch (ap_ptr->type)
		{
			/*** TVAL ***/
			case AUTOPICK_TYPE_TVAL:
			{
				if (o_ptr->tval != ap_ptr->tval) continue;

				break;
			}

			/*** Weapons ***/
			case AUTOPICK_TYPE_WEAPON:
			{
				if ((o_ptr->tval != TV_BOW) &&
				    (o_ptr->tval != TV_HAFTED) &&
				    (o_ptr->tval != TV_POLEARM) &&
				    (o_ptr->tval != TV_SWORD) &&
				    (o_ptr->tval != TV_DIGGING)) continue;

				break;
			}

			/*** Ammos ***/
			case AUTOPICK_TYPE_AMMO:
			{
				if ((o_ptr->tval != TV_SHOT) &&
				    (o_ptr->tval != TV_ARROW) &&
				    (o_ptr->tval != TV_BOLT)) continue;

				break;
			}

			/*** Armours ***/
			case AUTOPICK_TYPE_ARMOR:
			{
				if ((o_ptr->tval != TV_DRAG_ARMOR) &&
				    (o_ptr->tval != TV_HARD_ARMOR) &&
				    (o_ptr->tval != TV_SOFT_ARMOR) &&
				    (o_ptr->tval != TV_CLOAK) &&
				    (o_ptr->tval != TV_SHIELD) &&
				    (o_ptr->tval != TV_CROWN) &&
				    (o_ptr->tval != TV_HELM) &&
				    (o_ptr->tval != TV_GLOVES) &&
				    (o_ptr->tval != TV_BOOTS)) continue;

				break;
			}

			/*** Body Armours ***/
			case AUTOPICK_TYPE_BODY_ARMOR:
			{
				if ((o_ptr->tval != TV_DRAG_ARMOR) &&
				    (o_ptr->tval != TV_HARD_ARMOR) &&
				    (o_ptr->tval != TV_SOFT_ARMOR)) continue;

				break;
			}

			/*** Helms and Crowns ***/
			case AUTOPICK_TYPE_HELM:
			{
				if ((o_ptr->tval != TV_CROWN) &&
				    (o_ptr->tval != TV_HELM)) continue;

				break;
			}

			/*** Magical Devices ***/
			case AUTOPICK_TYPE_DEVICE:
			{
				if ((o_ptr->tval != TV_SCROLL) &&
				    (o_ptr->tval != TV_STAFF) &&
				    (o_ptr->tval != TV_WAND) &&
				    (o_ptr->tval != TV_ROD)) continue;

				break;
			}

			/*** Spellbook ***/
			case AUTOPICK_TYPE_SPELLBOOK:
			{
				if ((o_ptr->tval != TV_MAGIC_BOOK) &&
				    (o_ptr->tval != TV_PRAYER_BOOK)) continue;

				break;
			}

			/*** Junks ***/
			case AUTOPICK_TYPE_JUNK:
			{
				if ((o_ptr->tval != TV_SKELETON) &&
				    (o_ptr->tval != TV_BOTTLE) &&
				    (o_ptr->tval != TV_JUNK)) continue;

				break;
			}
		}

		/*** Name ***/
		if (ap_ptr->name[0] != '\0')
		{
			if (ap_ptr->name[0] == '^')
			{
#ifdef JP
				if ((!prefix(o_name, ap_ptr->name + 1)) &&
				    (!prefix(J_o_name, ap_ptr->name + 1))) continue;
#else
				if (!prefix(o_name, ap_ptr->name + 1)) continue;
#endif
			}

			else
			{
#ifdef JP
				if ((!jstrstr(o_name, ap_ptr->name)) &&
				    (!jstrstr(J_o_name, ap_ptr->name))) continue;
#else
				if (!strstr(o_name, ap_ptr->name)) continue;
#endif
			}
		}

		/* Found */
		return i;
	}

	/* Not found */
	return -1;
}

/*
 * Automatically pickup/destroy items in this grid.
 */
bool do_autopick(s16b o_idx)
{
	object_type *o_ptr;
	int idx;

	char o_name[160];


	/* Get the object */
	o_ptr = &o_list[o_idx];

	/* Describe the object */
	object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);

	/* Check autopick */
	idx = is_autopick(o_ptr);

	if (idx < 0) return FALSE;
	else
	{
		/* Acquire autopick info */
		autopick_type *ap_ptr = &autopick_list[idx];

		/* Auto-inscription */
		if (ap_ptr->insc)
		{
			if (!o_ptr->note)
			{
				o_ptr->note = quark_add(ap_ptr->insc);
			}
		}

		/* Auto-pickup */
		if (ap_ptr->act & AUTOPICK_ACT_PICKUP)
		{
			/* It can't be carried */
			if (!inven_carry_okay(o_ptr))
			{
				return FALSE;
			}

			if (ap_ptr->act & AUTOPICK_ACT_QUERY)
			{
				char out_val[120];

				strnfmt(out_val, sizeof(out_val),
				        "%sを拾いますか? ", o_name);

				if (!get_check(out_val))
				{
					return FALSE;
				}
			}

			py_pickup_aux(o_idx);

			return TRUE;
		}

		/* Auto-destroy */
		else if (ap_ptr->act & AUTOPICK_ACT_DESTROY)
		{
			if (ap_ptr->act & AUTOPICK_ACT_QUERY)
			{
				char out_val[120];

				strnfmt(out_val, sizeof(out_val),
				        "%sを破壊しますか? ", o_name);

				if (!get_check(out_val))
				{
					return FALSE;
				}
			}

			/* Mega-Hack -- preserve artifacts */
			if (adult_preserve)
			{
				/* Hack -- Preserve unknown artifacts */
				if (artifact_p(o_ptr) && !object_known_p(o_ptr))
				{
					/* Mega-Hack -- Preserve the artifact */
					a_info[o_ptr->name1].cur_num = 0;
				}
			}

			/* Print a message */
#ifdef JP
			msg_format("自動破壊: %s。", o_name);
#else
			msg_format("Auto-destroying %s.", o_name);
#endif

			/* Destroy the item */
			delete_object_idx(o_idx);

			return TRUE;
		}
	}

	return FALSE;
}

#define BIT(x) (1 << (x))

/*
 * Describe autopick info
 */
void autopick_desc(char *desc, const autopick_type *ap_ptr, bool act, bool insc)
{
	int i, j;


	*desc = '\0';

	/* Action */
	if (act)
	{
		if (ap_ptr->act & AUTOPICK_ACT_DESTROY) strcat(desc, "!");
		else if (!(ap_ptr->act & AUTOPICK_ACT_PICKUP)) strcat(desc, "~");
	}

	/* flags */
	if (ap_ptr->flags)
	{
		for (i = 0; i < 32; i++)
		{
			if (!(ap_ptr->flags & BIT(i))) continue;

			for (j = 0; autopick_flag_group[j].name; j++)
			{
				if (autopick_flag_group[j].flag & BIT(i))
				{
					strcat(desc, autopick_flag_group[j].name);
					break;
				}
			}
		}

		/* Hack -- Nth spellbook */
		if (ap_ptr->flags & AUTOPICK_NTH_BOOK)
		{
			char buf[80];
			sprintf(buf, "%d冊目の", ap_ptr->sval + 1);
			strcat(desc, buf);
		}

		/* Hack -- more than NN dice */
		if (ap_ptr->flags & AUTOPICK_MORE_THAN_NN_DICE)
		{
			char buf[80];
			sprintf(buf, "ダイス目%d以上の", ap_ptr->dice);
			strcat(desc, buf);
		}
	}


	/* type */
	if (ap_ptr->type)
	{
		if (ap_ptr->type == AUTOPICK_TYPE_TVAL)
		{
			for (i = 0; tval_name[i].tval; i++)
			{
				if (tval_name[i].tval == ap_ptr->tval)
				{
					strcat(desc, tval_name[i].name);
					break;
				}
			}
		}
		else
		{
			for (i = 0; autopick_type_group[i].name; i++)
			{
				if (autopick_type_group[i].flag == ap_ptr->type)
				{
					strcat(desc, autopick_type_group[i].name);
					break;
				}
			}
		}
	}

	/* name */
	if (!streq(ap_ptr->name, ""))
	{
		if (ap_ptr->type)
		{
			strcat(desc, ":");
		}
		strcat(desc, ap_ptr->name);
	}

	/* inscription */
	if (insc)
	{
		if (ap_ptr->insc)
		{
			strcat(desc, "#");
			strcat(desc, ap_ptr->insc);
		}
	}

	return;
}

#endif /* ALLOW_AUTO_PICKUP */
