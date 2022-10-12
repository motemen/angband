/* File: obj-info.c */

/*
 * Copyright (c) 2002 Andrew Sidwell, Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/* TRUE if a paragraph break should be output before next p_text_out() */
static bool new_paragraph = FALSE;


static void p_text_out(cptr str)
{
	if (new_paragraph)
	{
#ifdef JP
		text_out("\n\n　");
#else /* JP */
		text_out("\n\n   ");
#endif /* JP */
		new_paragraph = FALSE;
	}

	text_out(str);
}


#ifdef JP
static void output_list_v(cptr list[], int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (i != n - 1)
		{
			char buf[64];

			jverb(list[i], buf, JVERB_AND);
			p_text_out(buf);

			p_text_out("、");
		}
		else p_text_out(list[i]);
	}
}

static void output_desc_list_v(cptr intro, cptr outro, cptr list[], int n)
{
	if (n > 0)
	{
		/* Output intro */
		p_text_out(intro);

		/* Output list */
		output_list_v(list, n);

		/* Output outro */
		p_text_out(outro);
	}
}
#endif /* JP */

static void output_list(cptr list[], int n)
{
	int i;

#ifndef JP
	cptr conjunction = "and ";
#endif /* JP */

	if (n < 0)
	{
		n = -n;
#ifndef JP
		conjunction = "or ";
#endif /* JP */
	}

	for (i = 0; i < n; i++)
	{
		if (i != 0)
		{
#ifdef JP
			p_text_out("、");
#else /* JP */
			p_text_out((i == 1 && i == n - 1) ? " " : ", ");

			if (i == n - 1) p_text_out(conjunction);
#endif /* JP */
		}

		p_text_out(list[i]);
	}
}


#ifdef JP
static void output_desc_list(cptr intro, cptr outro, cptr list[], int n)
#else /* JP */
static void output_desc_list(cptr intro, cptr list[], int n)
#endif /* JP */
{
	if (n != 0)
	{
		/* Output intro */
		p_text_out(intro);

		/* Output list */
		output_list(list, n);

#ifdef JP
		/* Output outro */
		p_text_out(outro);
#else /* JP */
		/* Output end */
		p_text_out(".  ");
#endif /* JP */
	}
}


/*
 * Describe stat modifications.
 */
static bool describe_stats(const object_type *o_ptr, u32b f1)
{
	cptr descs[A_MAX];
	int cnt = 0;
	int pval = (o_ptr->pval > 0 ? o_ptr->pval : -o_ptr->pval);

	/* Abort if the pval is zero */
	if (!pval) return (FALSE);

	/* Collect stat bonuses */
	if (f1 & (TR1_STR)) descs[cnt++] = stat_names_full[A_STR];
	if (f1 & (TR1_INT)) descs[cnt++] = stat_names_full[A_INT];
	if (f1 & (TR1_WIS)) descs[cnt++] = stat_names_full[A_WIS];
	if (f1 & (TR1_DEX)) descs[cnt++] = stat_names_full[A_DEX];
	if (f1 & (TR1_CON)) descs[cnt++] = stat_names_full[A_CON];
	if (f1 & (TR1_CHR)) descs[cnt++] = stat_names_full[A_CHR];

	/* Skip */
	if (cnt == 0) return (FALSE);

	/* Shorten to "all stats", if appropriate. */
	if (cnt == A_MAX)
	{
#ifdef JP
		p_text_out("それはすべてのステータス");
#else /* JP */
		p_text_out(format("It %s all your stats", (o_ptr->pval > 0 ? "increases" : "decreases")));
#endif /* JP */
	}
	else
	{
#ifdef JP
		p_text_out("それは");
#else /* JP */
		p_text_out(format("It %s your ", (o_ptr->pval > 0 ? "increases" : "decreases")));
#endif /* JP */

		/* Output list */
		output_list(descs, cnt);
	}

	/* Output end */
#ifdef JP
	p_text_out(format("を %i ", pval));

	if (o_ptr->pval > 0) p_text_out("増加させる。");
	else                 p_text_out("減少させる。");
#else /* JP */
	p_text_out(format(" by %i.  ", pval));
#endif /* JP */

	/* We found something */
	return (TRUE);
}


/*
 * Describe "secondary bonuses" of an item.
 */
static bool describe_secondary(const object_type *o_ptr, u32b f1)
{
	cptr descs[8];
	int cnt = 0;
	int pval = (o_ptr->pval > 0 ? o_ptr->pval : -o_ptr->pval);

	/* Collect */
#ifdef JP
	if (f1 & (TR1_STEALTH)) descs[cnt++] = "隠密能力";
	if (f1 & (TR1_SEARCH))  descs[cnt++] = "探索能力";
	if (f1 & (TR1_INFRA))   descs[cnt++] = "赤外線視力";
	if (f1 & (TR1_TUNNEL))  descs[cnt++] = "採掘能力";
	if (f1 & (TR1_SPEED))   descs[cnt++] = "速度";
	if (f1 & (TR1_BLOWS))   descs[cnt++] = "打撃速度";
	if (f1 & (TR1_SHOTS))   descs[cnt++] = "射撃速度";
	if (f1 & (TR1_MIGHT))   descs[cnt++] = "射撃の威力";
#else /* JP */
	if (f1 & (TR1_STEALTH)) descs[cnt++] = "stealth";
	if (f1 & (TR1_SEARCH))  descs[cnt++] = "searching";
	if (f1 & (TR1_INFRA))   descs[cnt++] = "infravision";
	if (f1 & (TR1_TUNNEL))  descs[cnt++] = "tunneling";
	if (f1 & (TR1_SPEED))   descs[cnt++] = "speed";
	if (f1 & (TR1_BLOWS))   descs[cnt++] = "attack speed";
	if (f1 & (TR1_SHOTS))   descs[cnt++] = "shooting speed";
	if (f1 & (TR1_MIGHT))   descs[cnt++] = "shooting power";
#endif /* JP */

	/* Skip */
	if (!cnt) return (FALSE);

	/* Start */
#ifdef JP
	p_text_out("それは");
#else /* JP */
	p_text_out(format("It %s your ", (o_ptr->pval > 0 ? "increases" : "decreases")));
#endif /* JP */

	/* Output list */
	output_list(descs, cnt);

	/* Output end */
#ifdef JP
	p_text_out(format("を %i ", pval));

	if (o_ptr->pval > 0) p_text_out("増加させる。");
	else                 p_text_out("減少させる。");
#else /* JP */
	p_text_out(format(" by %i.  ", pval));
#endif /* JP */

	/* We found something */
	return (TRUE);
}


/*
 * Describe the special slays and executes of an item.
 */
static bool describe_slay(const object_type *o_ptr, u32b f1)
{
	cptr slays[8], execs[3];
	int slcnt = 0, excnt = 0;

	/* Unused parameter */
	(void)o_ptr;

	/* Collect brands */
#ifdef JP
	if (f1 & (TR1_SLAY_ANIMAL)) slays[slcnt++] = "動物";
	if (f1 & (TR1_SLAY_ORC))    slays[slcnt++] = "オーク";
	if (f1 & (TR1_SLAY_TROLL))  slays[slcnt++] = "トロル";
	if (f1 & (TR1_SLAY_GIANT))  slays[slcnt++] = "ジャイアント";
#else /* JP */
	if (f1 & (TR1_SLAY_ANIMAL)) slays[slcnt++] = "animals";
	if (f1 & (TR1_SLAY_ORC))    slays[slcnt++] = "orcs";
	if (f1 & (TR1_SLAY_TROLL))  slays[slcnt++] = "trolls";
	if (f1 & (TR1_SLAY_GIANT))  slays[slcnt++] = "giants";
#endif /* JP */

	/* Dragon slay/execute */
	if (f1 & TR1_KILL_DRAGON)
#ifdef JP
		execs[excnt++] = "ドラゴン";
#else /* JP */
		execs[excnt++] = "dragons";
#endif /* JP */
	else if (f1 & TR1_SLAY_DRAGON)
#ifdef JP
		slays[slcnt++] = "ドラゴン";
#else /* JP */
		slays[slcnt++] = "dragons";
#endif /* JP */

	/* Demon slay/execute */
	if (f1 & TR1_KILL_DEMON)
#ifdef JP
		execs[excnt++] = "デーモン";
#else /* JP */
		execs[excnt++] = "demons";
#endif /* JP */
	else if (f1 & TR1_SLAY_DEMON)
#ifdef JP
		slays[slcnt++] = "デーモン";
#else /* JP */
		slays[slcnt++] = "demons";
#endif /* JP */

	/* Undead slay/execute */
	if (f1 & TR1_KILL_UNDEAD)
#ifdef JP
		execs[excnt++] = "アンデッド";
#else /* JP */
		execs[excnt++] = "undead";
#endif /* JP */
	else if (f1 & TR1_SLAY_UNDEAD)
#ifdef JP
		slays[slcnt++] = "アンデッド";
#else /* JP */
		slays[slcnt++] = "undead";
#endif /* JP */

#ifdef JP
	if (f1 & (TR1_SLAY_EVIL)) slays[slcnt++] = "すべての邪悪な生物";
#else /* JP */
	if (f1 & (TR1_SLAY_EVIL)) slays[slcnt++] = "all evil creatures";
#endif /* JP */

	/* Describe */
	if (slcnt)
	{
#ifdef JP
		output_desc_list("それは", "に大きなダメージを与え", slays, slcnt);
#else /* JP */
		/* Output intro */
		p_text_out("It slays ");

		/* Output list */
		output_list(slays, slcnt);
#endif /* JP */

		/* Output end (if needed) */
#ifdef JP
		if (!excnt) p_text_out("る。");
#else /* JP */
		if (!excnt) p_text_out(".  ");
#endif /* JP */
	}

	if (excnt)
	{
#ifdef JP
		output_desc_list((slcnt ? "" : "それは"),
		                 "に対して特に恐るべき力を発揮する。",
		                 execs, excnt);
#else /* JP */
		/* Output intro */
		if (slcnt) p_text_out(", and is especially deadly against ");
		else p_text_out("It is especially deadly against ");

		/* Output list */
		output_list(execs, excnt);

		/* Output end */
		p_text_out(".  ");
#endif /* JP */
	}

	/* We are done here */
	return ((excnt || slcnt) ? TRUE : FALSE);
}


/*
 * Describe elemental brands.
 */
static bool describe_brand(const object_type *o_ptr, u32b f1)
{
	cptr descs[5];
	int cnt = 0;

	/* Unused parameter */
	(void)o_ptr;

	/* Collect brands */
#ifdef JP
	if (f1 & (TR1_BRAND_ACID)) descs[cnt++] = "酸";
	if (f1 & (TR1_BRAND_ELEC)) descs[cnt++] = "電撃";
	if (f1 & (TR1_BRAND_FIRE)) descs[cnt++] = "火炎";
	if (f1 & (TR1_BRAND_COLD)) descs[cnt++] = "冷気";
	if (f1 & (TR1_BRAND_POIS)) descs[cnt++] = "毒";
#else /* JP */
	if (f1 & (TR1_BRAND_ACID)) descs[cnt++] = "acid";
	if (f1 & (TR1_BRAND_ELEC)) descs[cnt++] = "electricity";
	if (f1 & (TR1_BRAND_FIRE)) descs[cnt++] = "fire";
	if (f1 & (TR1_BRAND_COLD)) descs[cnt++] = "frost";
	if (f1 & (TR1_BRAND_POIS)) descs[cnt++] = "poison";
#endif /* JP */

	/* Describe brands */
#ifdef JP
	output_desc_list("それは", "の属性が付与されている。", descs, cnt);
#else /* JP */
	output_desc_list("It is branded with ", descs, cnt);
#endif /* JP */

	/* We are done here */
	return (cnt ? TRUE : FALSE);
}


/*
 * Describe immunities granted by an object.
 *
 * ToDo - Merge intro describe_resist() below.
 */
static bool describe_immune(const object_type *o_ptr, u32b f2)
{
	cptr descs[4];
	int cnt = 0;

	/* Unused parameter */
	(void)o_ptr;

	/* Collect immunities */
#ifdef JP
	if (f2 & (TR2_IM_ACID)) descs[cnt++] = "酸";
	if (f2 & (TR2_IM_ELEC)) descs[cnt++] = "電撃";
	if (f2 & (TR2_IM_FIRE)) descs[cnt++] = "火炎";
	if (f2 & (TR2_IM_COLD)) descs[cnt++] = "冷気";
#else /* JP */
	if (f2 & (TR2_IM_ACID)) descs[cnt++] = "acid";
	if (f2 & (TR2_IM_ELEC)) descs[cnt++] = "lightning";
	if (f2 & (TR2_IM_FIRE)) descs[cnt++] = "fire";
	if (f2 & (TR2_IM_COLD)) descs[cnt++] = "cold";
#endif /* JP */

	/* Describe immunities */
#ifdef JP
	output_desc_list("それは", "に対する完全な免疫を授ける。", descs, cnt);
#else /* JP */
	output_desc_list("It provides immunity to ", descs, cnt);
#endif /* JP */

	/* We are done here */
	return (cnt ? TRUE : FALSE);
}


/*
 * Describe resistances granted by an object.
 */
static bool describe_resist(const object_type *o_ptr, u32b f2, u32b f3)
{
	cptr vp[17];
	int vn = 0;

	/* Unused parameter */
	(void)o_ptr;

	/* Collect resistances */
	if ((f2 & (TR2_RES_ACID)) && !(f2 & (TR2_IM_ACID)))
#ifdef JP
		vp[vn++] = "酸";
#else /* JP */
		vp[vn++] = "acid";
#endif /* JP */
	if ((f2 & (TR2_RES_ELEC)) && !(f2 & (TR2_IM_ELEC)))
#ifdef JP
		vp[vn++] = "電撃";
#else /* JP */
		vp[vn++] = "lightning";
#endif /* JP */
	if ((f2 & (TR2_RES_FIRE)) && !(f2 & (TR2_IM_FIRE)))
#ifdef JP
		vp[vn++] = "火炎";
#else /* JP */
		vp[vn++] = "fire";
#endif /* JP */
	if ((f2 & (TR2_RES_COLD)) && !(f2 & (TR2_IM_COLD)))
#ifdef JP
		vp[vn++] = "冷気";
#else /* JP */
		vp[vn++] = "cold";
#endif /* JP */

#ifdef JP
	if (f2 & (TR2_RES_POIS))  vp[vn++] = "毒";
	if (f2 & (TR2_RES_FEAR))  vp[vn++] = "恐怖";
	if (f2 & (TR2_RES_LITE))  vp[vn++] = "閃光";
	if (f2 & (TR2_RES_DARK))  vp[vn++] = "暗黒";
	if (f2 & (TR2_RES_BLIND)) vp[vn++] = "盲目";
	if (f2 & (TR2_RES_CONFU)) vp[vn++] = "混乱";
	if (f2 & (TR2_RES_SOUND)) vp[vn++] = "轟音";
	if (f2 & (TR2_RES_SHARD)) vp[vn++] = "破片";
	if (f2 & (TR2_RES_NEXUS)) vp[vn++] = "因果混乱" ;
	if (f2 & (TR2_RES_NETHR)) vp[vn++] = "地獄";
	if (f2 & (TR2_RES_CHAOS)) vp[vn++] = "カオス";
	if (f2 & (TR2_RES_DISEN)) vp[vn++] = "劣化";
	if (f3 & (TR3_HOLD_LIFE)) vp[vn++] = "生命力吸収";
#else /* JP */
	if (f2 & (TR2_RES_POIS))  vp[vn++] = "poison";
	if (f2 & (TR2_RES_FEAR))  vp[vn++] = "fear";
	if (f2 & (TR2_RES_LITE))  vp[vn++] = "light";
	if (f2 & (TR2_RES_DARK))  vp[vn++] = "dark";
	if (f2 & (TR2_RES_BLIND)) vp[vn++] = "blindness";
	if (f2 & (TR2_RES_CONFU)) vp[vn++] = "confusion";
	if (f2 & (TR2_RES_SOUND)) vp[vn++] = "sound";
	if (f2 & (TR2_RES_SHARD)) vp[vn++] = "shards";
	if (f2 & (TR2_RES_NEXUS)) vp[vn++] = "nexus" ;
	if (f2 & (TR2_RES_NETHR)) vp[vn++] = "nether";
	if (f2 & (TR2_RES_CHAOS)) vp[vn++] = "chaos";
	if (f2 & (TR2_RES_DISEN)) vp[vn++] = "disenchantment";
	if (f3 & (TR3_HOLD_LIFE)) vp[vn++] = "life draining";
#endif /* JP */

	/* Describe resistances */
#ifdef JP
	output_desc_list("それは", "への耐性を授ける。", vp, vn);
#else /* JP */
	output_desc_list("It provides resistance to ", vp, vn);
#endif /* JP */

	/* We are done here */
	return (vn ? TRUE : FALSE);
}


/*
 * Describe 'ignores' of an object.
 */
static bool describe_ignores(const object_type *o_ptr, u32b f3)
{
	cptr list[4];
	int n = 0;

	/* Unused parameter */
	(void)o_ptr;

	/* Collect the ignores */
#ifdef JP
	if (f3 & (TR3_IGNORE_ACID)) list[n++] = "酸";
	if (f3 & (TR3_IGNORE_ELEC)) list[n++] = "電撃";
	if (f3 & (TR3_IGNORE_FIRE)) list[n++] = "火炎";
	if (f3 & (TR3_IGNORE_COLD)) list[n++] = "冷気";
#else /* JP */
	if (f3 & (TR3_IGNORE_ACID)) list[n++] = "acid";
	if (f3 & (TR3_IGNORE_ELEC)) list[n++] = "electricity";
	if (f3 & (TR3_IGNORE_FIRE)) list[n++] = "fire";
	if (f3 & (TR3_IGNORE_COLD)) list[n++] = "cold";
#endif /* JP */

	/* Describe ignores */
	if (n == 4)
#ifdef JP
		p_text_out("それは四元素の攻撃では傷つかない。");
#else /* JP */
		p_text_out("It cannot be harmed by the elements.  ");
#endif /* JP */
	else
#ifdef JP
		output_desc_list("それは", "では傷つかない。", list, n);
#else /* JP */
		output_desc_list("It cannot be harmed by ", list, -n);
#endif /* JP */

	return ((n > 0) ? TRUE : FALSE);
}


/*
 * Describe stat sustains.
 */
static bool describe_sustains(const object_type *o_ptr, u32b f2)
{
	cptr list[A_MAX];
	int n = 0;

	/* Unused parameter */
	(void)o_ptr;

	/* Collect the sustains */
	if (f2 & (TR2_SUST_STR)) list[n++] = stat_names_full[A_STR];
	if (f2 & (TR2_SUST_INT)) list[n++] = stat_names_full[A_INT];
	if (f2 & (TR2_SUST_WIS)) list[n++] = stat_names_full[A_WIS];
	if (f2 & (TR2_SUST_DEX)) list[n++] = stat_names_full[A_DEX];
	if (f2 & (TR2_SUST_CON)) list[n++] = stat_names_full[A_CON];
	if (f2 & (TR2_SUST_CHR)) list[n++] = stat_names_full[A_CHR];

	/* Describe immunities */
	if (n == A_MAX)
#ifdef JP
		p_text_out("それはすべてのステータスを維持する。");
#else /* JP */
		p_text_out("It sustains all your stats.  ");
#endif /* JP */
	else
#ifdef JP
		output_desc_list("それは", "を維持する。", list, n);
#else /* JP */
		output_desc_list("It sustains your ", list, n);
#endif /* JP */

	/* We are done here */
	return (n ? TRUE : FALSE);
}


/*
 * Describe miscellaneous powers such as see invisible, free action,
 * permanent light, etc; also note curses and penalties.
 */
static bool describe_misc_magic(const object_type *o_ptr, u32b f3)
{
	cptr good[6], bad[4];
	int gc = 0, bc = 0;
	bool something = FALSE;

	/* Collect stuff which can't be categorized */
#ifdef JP
	if (f3 & (TR3_BLESSED))     good[gc++] = "神に祝福されている";
	if (f3 & (TR3_IMPACT))      good[gc++] = "衝撃で地震を引き起こす";
	if (f3 & (TR3_SLOW_DIGEST)) good[gc++] = "新陳代謝を遅くする";
	if (f3 & (TR3_FEATHER))     good[gc++] = "あなたを羽のように落下させる";
#else /* JP */
	if (f3 & (TR3_BLESSED))     good[gc++] = "is blessed by the gods";
	if (f3 & (TR3_IMPACT))      good[gc++] = "creates earthquakes on impact";
	if (f3 & (TR3_SLOW_DIGEST)) good[gc++] = "slows your metabolism";
	if (f3 & (TR3_FEATHER))     good[gc++] = "makes you fall like a feather";
#endif /* JP */
	if (((o_ptr->tval == TV_LITE) && artifact_p(o_ptr)) || (f3 & (TR3_LITE)))
#ifdef JP
		good[gc++] = "周囲のダンジョンを照らす";
#else /* JP */
		good[gc++] = "lights the dungeon around you";
#endif /* JP */
#ifdef JP
	if (f3 & (TR3_REGEN))       good[gc++] = "再生速度を速める";
#else /* JP */
	if (f3 & (TR3_REGEN))       good[gc++] = "speeds your regeneration";
#endif /* JP */

	/* Describe */
#ifdef JP
	output_desc_list_v("それは", "。", good, gc);
#else /* JP */
	output_desc_list("It ", good, gc);
#endif /* JP */

	/* Set "something" */
	if (gc) something = TRUE;

	/* Collect granted powers */
	gc = 0;
#ifdef JP
	if (f3 & (TR3_FREE_ACT))  good[gc++] = "麻痺に対する免疫";
	if (f3 & (TR3_TELEPATHY)) good[gc++] = "テレパシーの能力";
	if (f3 & (TR3_SEE_INVIS)) good[gc++] = "透明なものを見る能力";
#else /* JP */
	if (f3 & (TR3_FREE_ACT))  good[gc++] = "immunity to paralysis";
	if (f3 & (TR3_TELEPATHY)) good[gc++] = "the power of telepathy";
	if (f3 & (TR3_SEE_INVIS)) good[gc++] = "the ability to see invisible things";
#endif /* JP */

	/* Collect penalties */
#ifdef JP
	if (f3 & (TR3_AGGRAVATE)) bad[bc++] = "周囲のモンスターを怒らせる";
	if (f3 & (TR3_DRAIN_EXP)) bad[bc++] = "経験値を吸収する";
	if (f3 & (TR3_TELEPORT))  bad[bc++] = "ランダムなテレポートを引き起こす";
#else /* JP */
	if (f3 & (TR3_AGGRAVATE)) bad[bc++] = "aggravates creatures around you";
	if (f3 & (TR3_DRAIN_EXP)) bad[bc++] = "drains experience";
	if (f3 & (TR3_TELEPORT))  bad[bc++] = "induces random teleportation";
#endif /* JP */

	/* Deal with cursed stuff */
	if (cursed_p(o_ptr))
	{
#ifdef JP
		if (f3 & (TR3_PERMA_CURSE)) bad[bc++] = "永遠の呪いがかけられている";
		else if (f3 & (TR3_HEAVY_CURSE)) bad[bc++] = "強力な呪いがかけられている";
		else if (object_known_p(o_ptr)) bad[bc++] = "呪われている";
#else /* JP */
		if (f3 & (TR3_PERMA_CURSE)) bad[bc++] = "is permanently cursed";
		else if (f3 & (TR3_HEAVY_CURSE)) bad[bc++] = "is heavily cursed";
		else if (object_known_p(o_ptr)) bad[bc++] = "is cursed";
#endif /* JP */
	}

	/* Describe */
	if (gc)
	{
#ifdef JP
		/* Output list */
		output_desc_list("それは", "を授ける", good, gc);
#else /* JP */
		/* Output intro */
		p_text_out("It grants you ");

		/* Output list */
		output_list(good, gc);
#endif /* JP */

		/* Output end (if needed) */
#ifdef JP
		if (!bc) p_text_out("。");
#else /* JP */
		if (!bc) p_text_out(".  ");
#endif /* JP */
	}

	if (bc)
	{
		/* Output intro */
#ifdef JP
		if (gc) p_text_out("が、");
		else p_text_out("それは");
#else /* JP */
		if (gc) p_text_out(", but it also ");
		else p_text_out("It ");
#endif /* JP */

		/* Output list */
#ifdef JP
		output_list_v(bad, bc);
#else /* JP */
		output_list(bad, bc);
#endif /* JP */

		/* Output end */
#ifdef JP
		p_text_out("。");
#else /* JP */
		p_text_out(".  ");
#endif /* JP */
	}

	/* Set "something" */
	if (gc || bc) something = TRUE;

	/* Return "something" */
	return (something);
}


/*
 * Describe an object's activation, if any.
 */
static bool describe_activation(const object_type *o_ptr, u32b f3)
{
	/* Check for the activation flag */
	if (f3 & TR3_ACTIVATE)
	{
#ifdef JP
		p_text_out("それは発動可能だ: ");
		describe_item_activation(o_ptr);
		p_text_out("。");
#else /* JP */
		p_text_out("It activates for ");
		describe_item_activation(o_ptr);
		p_text_out(".  ");
#endif /* JP */

		return (TRUE);
	}

	/* No activation */
	return (FALSE);
}


/*
 * Output object information
 */
bool object_info_out(const object_type *o_ptr)
{
	u32b f1, f2, f3;
	bool something = FALSE;

	/* Grab the object flags */
	object_info_out_flags(o_ptr, &f1, &f2, &f3);

	/* Describe the object */
	if (describe_stats(o_ptr, f1)) something = TRUE;
	if (describe_secondary(o_ptr, f1)) something = TRUE;
	if (describe_slay(o_ptr, f1)) something = TRUE;
	if (describe_brand(o_ptr, f1)) something = TRUE;
	if (describe_immune(o_ptr, f2)) something = TRUE;
	if (describe_resist(o_ptr, f2, f3)) something = TRUE;
	if (describe_sustains(o_ptr, f2)) something = TRUE;
	if (describe_misc_magic(o_ptr, f3)) something = TRUE;
	if (describe_activation(o_ptr, f3)) something = TRUE;
	if (describe_ignores(o_ptr, f3)) something = TRUE;

	/* Unknown extra powers (ego-item with random extras or artifact) */
	if (object_known_p(o_ptr) && (!(o_ptr->ident & IDENT_MENTAL)) &&
	    ((o_ptr->xtra1) || artifact_p(o_ptr)))
	{
		/* Hack -- Put this in a separate paragraph if screen dump */
		if (text_out_hook == text_out_to_screen)
			new_paragraph = TRUE;

#ifdef JP
		p_text_out("それは隠された力を持っているかもしれない。");
#else /* JP */
		p_text_out("It might have hidden powers.");
#endif /* JP */
		something = TRUE;
	}

	/* We are done. */
	return something;
}


/*
 * Header for additional information when printing to screen.
 *
 * Return TRUE if an object description was displayed.
 */
static bool screen_out_head(const object_type *o_ptr)
{
	char *o_name;
	int name_size = Term->wid;
	bool has_description = FALSE;

	/* Allocate memory to the size of the screen */
	o_name = C_RNEW(name_size, char);

	/* Description */
	object_desc(o_name, name_size, o_ptr, TRUE, 3);

	/* Print, in colour */
	text_out_c(TERM_YELLOW, format("%^s", o_name));

	/* Free up the memory */
	FREE(o_name);

	/* Display the known artifact description */
	if (!adult_rand_artifacts && o_ptr->name1 &&
	    object_known_p(o_ptr) && a_info[o_ptr->name1].text)
	{
		p_text_out("\n\n   ");
		p_text_out(a_text + a_info[o_ptr->name1].text);
		has_description = TRUE;
	}

	/* Display the known object description */
	else if (object_aware_p(o_ptr) || object_known_p(o_ptr))
	{
		if (k_info[o_ptr->k_idx].text)
		{
			p_text_out("\n\n   ");
			p_text_out(k_text + k_info[o_ptr->k_idx].text);
			has_description = TRUE;
		}

		/* Display an additional ego-item description */
		if (o_ptr->name2 && object_known_p(o_ptr) && e_info[o_ptr->name2].text)
		{
			p_text_out("\n\n   ");
			p_text_out(e_text + e_info[o_ptr->name2].text);
			has_description = TRUE;
		}
	}

	return (has_description);
}


/*
 * Place an item description on the screen.
 */
void object_info_screen(const object_type *o_ptr)
{
	bool has_description, has_info;

	/* Redirect output to the screen */
	text_out_hook = text_out_to_screen;

	/* Save the screen */
	screen_save();

	has_description = screen_out_head(o_ptr);

	object_info_out_flags = object_flags_known;

	/* Dump the info */
	new_paragraph = TRUE;
	has_info = object_info_out(o_ptr);
	new_paragraph = FALSE;

	if (!object_known_p(o_ptr))
#ifdef JP
		p_text_out("\n\n   このアイテムはまだ鑑定されていない。");
#else /* JP */
		p_text_out("\n\n   This item has not been identified.");
#endif /* JP */
	else if (!has_description && !has_info)
#ifdef JP
		p_text_out("\n\n   このアイテムは特殊な能力を持っていないようだ。");
#else /* JP */
		p_text_out("\n\n   This item does not seem to possess any special abilities.");
#endif /* JP */

#ifdef JP
	text_out_c(TERM_L_BLUE, "\n\n[何かキーを押して下さい]\n");
#else /* JP */
	text_out_c(TERM_L_BLUE, "\n\n[Press any key to continue]\n");
#endif /* JP */

	/* Wait for input */
	(void)inkey();

	/* Load the screen */
	screen_load();

	/* Hack -- Browse book, then prompt for a command */
	if (o_ptr->tval == cp_ptr->spell_book)
	{
		/* Call the aux function */
		do_cmd_browse_aux(o_ptr);
	}
}
