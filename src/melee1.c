/* File: melee1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/*
 * 2.7.9v3 日本語版製作: しとしん
 * 2.7.9v6 対応        : 岸康司, しとしん
 * 2.8.0   対応        : sayu, しとしん
 * 2.8.1   対応        : FIRST
 * 2.8.1   対応        : FIRST, しとしん
 */

#include "angband.h"



/*
 * Critical blow.  All hits that do 95% of total possible damage,
 * and which also do at least 20 damage, or, sometimes, N damage.
 * This is used only to determine "cuts" and "stuns".
 */
static int monster_critical(int dice, int sides, int dam)
{
	int max = 0;
	int total = dice * sides;

	/* Must do at least 95% of perfect */
	if (dam < total * 19 / 20) return (0);

	/* Weak blows rarely work */
	if ((dam < 20) && (rand_int(100) >= dam)) return (0);

	/* Perfect damage */
	if (dam == total) max++;

	/* Super-charge */
	if (dam >= 20)
	{
		while (rand_int(100) < 2) max++;
	}

	/* Critical damage */
	if (dam > 45) return (6 + max);
	if (dam > 33) return (5 + max);
	if (dam > 25) return (4 + max);
	if (dam > 18) return (3 + max);
	if (dam > 11) return (2 + max);
	return (1 + max);
}





/*
 * Determine if a monster attack against the player succeeds.
 */
static bool check_hit(int power, int level)
{
	int chance, ac;

	/* Calculate the "attack quality" */
	chance = (power + (level * 3));

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Check if the player was hit */
	return test_hit(chance, ac, TRUE);
}


#define MAX_DESC_INSULT 8


/*
 * Hack -- possible "insult" messages
 */
static cptr desc_insult[MAX_DESC_INSULT] =
{
#ifdef JP
	"があなたを侮辱した！",
	"があなたの母を侮辱した！",
	"があなたを軽蔑した！",
	"があなたを辱めた！",
	"があなたを汚した！",
	"があなたの回りで踊った！",
	"が猥褻な身ぶりをした！",
	"があなたをぼんやりと見た！！！"
#else
	"insults you!",
	"insults your mother!",
	"gives you the finger!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obscene gestures!",
	"moons you!!!"
#endif
};


#define MAX_DESC_MOAN 4


/*
 * Hack -- possible "insult" messages
 */
static cptr desc_moan[MAX_DESC_MOAN] =
{
#ifdef JP
	"は何かを悲しんでいるようだ。",
	"が彼の飼い犬を見なかったかと尋ねている。",
	"が縄張りから出て行けと言っている。",
	"はキノコがどうとか呟いている。"
#else
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms."
#endif
};


/*
 * Attack the player via physical attacks.
 */
bool make_attack_normal(int m_idx)
{
	monster_type *m_ptr = &mon_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	monster_lore *l_ptr = &l_list[m_ptr->r_idx];

	int ap_cnt;

	int i, k, tmp, ac, rlev;
	int do_cut, do_stun;

	s32b gold;

	object_type *o_ptr;

	char o_name[80];

	char m_name[80];

	char ddesc[80];

	bool blinked;
	
	int sound_msg;


	/* Not allowed to attack */
	if (r_ptr->flags1 & (RF1_NEVER_BLOW)) return (FALSE);


	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Extract the effective monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);


	/* Get the monster name (or "it") */
	monster_desc(m_name, sizeof(m_name), m_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	monster_desc(ddesc, sizeof(ddesc), m_ptr, 0x88);


	/* Assume no blink */
	blinked = FALSE;

	/* Scan through all blows */
	for (ap_cnt = 0; ap_cnt < MONSTER_BLOW_MAX; ap_cnt++)
	{
		bool visible = FALSE;
		bool obvious = FALSE;
		bool do_break = FALSE;

		int power = 0;
		int damage = 0;

		cptr act = NULL;

		/* Extract the attack infomation */
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;


		/* Hack -- no more attacks */
		if (!method) break;


		/* Handle "leaving" */
		if (p_ptr->leaving) break;


		/* Extract visibility (before blink) */
		if (m_ptr->ml) visible = TRUE;



		/* Extract the attack "power" */
		switch (effect)
		{
			case RBE_HURT:      power = 60; break;
			case RBE_POISON:    power =  5; break;
			case RBE_UN_BONUS:  power = 20; break;
			case RBE_UN_POWER:  power = 15; break;
			case RBE_EAT_GOLD:  power =  5; break;
			case RBE_EAT_ITEM:  power =  5; break;
			case RBE_EAT_FOOD:  power =  5; break;
			case RBE_EAT_LITE:  power =  5; break;
			case RBE_ACID:      power =  0; break;
			case RBE_ELEC:      power = 10; break;
			case RBE_FIRE:      power = 10; break;
			case RBE_COLD:      power = 10; break;
			case RBE_BLIND:     power =  2; break;
			case RBE_CONFUSE:   power = 10; break;
			case RBE_TERRIFY:   power = 10; break;
			case RBE_PARALYZE:  power =  2; break;
			case RBE_LOSE_STR:  power =  0; break;
			case RBE_LOSE_DEX:  power =  0; break;
			case RBE_LOSE_CON:  power =  0; break;
			case RBE_LOSE_INT:  power =  0; break;
			case RBE_LOSE_WIS:  power =  0; break;
			case RBE_LOSE_CHR:  power =  0; break;
			case RBE_LOSE_ALL:  power =  2; break;
			case RBE_SHATTER:   power = 60; break;
			case RBE_EXP_10:    power =  5; break;
			case RBE_EXP_20:    power =  5; break;
			case RBE_EXP_40:    power =  5; break;
			case RBE_EXP_80:    power =  5; break;
			case RBE_HALLU:     power = 10; break;
		}


		/* Monster hits player */
		if (!effect || check_hit(power, rlev))
		{
			/* Always disturbing */
			disturb(1, 0);


			/* Hack -- Apply "protection from evil" */
			if ((p_ptr->protevil > 0) &&
			    (r_ptr->flags3 & (RF3_EVIL)) &&
			    (p_ptr->lev >= rlev) &&
			    ((rand_int(100) + p_ptr->lev) > 50))
			{
				/* Remember the Evil-ness */
				if (m_ptr->ml)
				{
					l_ptr->flags3 |= (RF3_EVIL);
				}

				/* Message */
#ifdef JP
				msg_format("%^sは撃退された。", m_name);
#else /* JP */
				msg_format("%^s is repelled.", m_name);
#endif /* JP */

				/* Hack -- Next attack */
				continue;
			}


			/* Assume no cut or stun */
			do_cut = do_stun = 0;

			/* Assume no sound */
			sound_msg = MSG_GENERIC;

			/* Describe the attack method */
			switch (method)
			{
				case RBM_HIT:
				{
#ifdef JP
					act = "に殴られた。";
#else /* JP */
					act = "hits you.";
#endif /* JP */
					do_cut = do_stun = 1;
					sound_msg = MSG_MON_HIT;
					break;
				}

				case RBM_TOUCH:
				{
#ifdef JP
					act = "に触られた。";
#else /* JP */
					act = "touches you.";
#endif /* JP */
					sound_msg = MSG_MON_TOUCH;
					break;
				}

				case RBM_PUNCH:
				{
#ifdef JP
					act = "にパンチされた。";
#else /* JP */
					act = "punches you.";
#endif /* JP */
					do_stun = 1;
					sound_msg = MSG_MON_PUNCH;
					break;
				}

				case RBM_KICK:
				{
#ifdef JP
					act = "に蹴られた。";
#else /* JP */
					act = "kicks you.";
#endif /* JP */
					do_stun = 1;
					sound_msg = MSG_MON_KICK;
					break;
				}

				case RBM_CLAW:
				{
#ifdef JP
					act = "にひっかかれた。";
#else /* JP */
					act = "claws you.";
#endif /* JP */
					do_cut = 1;
					sound_msg = MSG_MON_CLAW;
					break;
				}

				case RBM_BITE:
				{
#ifdef JP
					act = "に噛まれた。";
#else /* JP */
					act = "bites you.";
#endif /* JP */
					do_cut = 1;
					sound_msg = MSG_MON_BITE;
					break;
				}

				case RBM_STING:
				{
#ifdef JP
					act = "に刺された。";
#else /* JP */
					act = "stings you.";
#endif /* JP */
					sound_msg = MSG_MON_STING;
					break;
				}

				case RBM_XXX1:
				{
#ifdef JP
					act = "に XXX1 された。";
#else /* JP */
					act = "XXX1's you.";
#endif /* JP */
					break;
				}

				case RBM_BUTT:
				{
#ifdef JP
					act = "に角で突かれた。";
#else /* JP */
					act = "butts you.";
#endif /* JP */
					do_stun = 1;
					sound_msg = MSG_MON_BUTT;
					break;
				}

				case RBM_CRUSH:
				{
#ifdef JP
					act = "に体当たりされた。";
#else /* JP */
					act = "crushes you.";
#endif /* JP */
					do_stun = 1;
					sound_msg = MSG_MON_CRUSH;
					break;
				}

				case RBM_ENGULF:
				{
#ifdef JP
					act = "に飲み込まれた。";
#else /* JP */
					act = "engulfs you.";
#endif /* JP */
					sound_msg = MSG_MON_ENGULF;
					break;
				}

				case RBM_XXX2:
				{
#ifdef JP
					act = "に XXX2 された";
#else /* JP */
					act = "XXX2's you.";
#endif /* JP */
					break;
				}

				case RBM_CRAWL:
				{
#ifdef JP
					act = "が体の上を這い回った。";
#else /* JP */
					act = "crawls on you.";
#endif /* JP */
					sound_msg = MSG_MON_CRAWL;
					break;
				}

				case RBM_DROOL:
				{
#ifdef JP
					act = "によだれをたらされた。";
#else /* JP */
					act = "drools on you.";
#endif /* JP */
					sound_msg = MSG_MON_DROOL;
					break;
				}

				case RBM_SPIT:
				{
#ifdef JP
					act = "に唾を吐かれた。";
#else /* JP */
					act = "spits on you.";
#endif /* JP */
					sound_msg = MSG_MON_SPIT; 
					break;
				}

				case RBM_XXX3:
				{
#ifdef JP
					act = "に XXX3 された。";
#else /* JP */
					act = "XXX3's on you.";
#endif /* JP */
					break;
				}

				case RBM_GAZE:
				{
#ifdef JP
					act = "ににらまれた。";
#else /* JP */
					act = "gazes at you.";
#endif /* JP */
					sound_msg = MSG_MON_GAZE; 
					break;
				}

				case RBM_WAIL:
				{
#ifdef JP
					act = "に泣き叫ばれた。";
#else /* JP */
					act = "wails at you.";
#endif /* JP */
					sound_msg = MSG_MON_WAIL; 
					break;
				}

				case RBM_SPORE:
				{
#ifdef JP
					act = "に胞子を飛ばされた。";
#else /* JP */
					act = "releases spores at you.";
#endif /* JP */
					sound_msg = MSG_MON_SPORE; 
					break;
				}

				case RBM_XXX4:
				{
#ifdef JP
					act = "が XXX4 を発射した。";
#else /* JP */
					act = "projects XXX4's at you.";
#endif /* JP */
					break;
				}

				case RBM_BEG:
				{
#ifdef JP
					act = "に金をせがまれた。";
#else /* JP */
					act = "begs you for money.";
#endif /* JP */
					sound_msg = MSG_MON_BEG;
					break;
				}

				case RBM_INSULT:
				{
					act = desc_insult[rand_int(MAX_DESC_INSULT)];
					sound_msg = MSG_MON_INSULT; 
					break;
				}

				case RBM_MOAN:
				{
					act = desc_moan[rand_int(MAX_DESC_MOAN)];
					sound_msg = MSG_MON_MOAN; 
					break;
				}

				case RBM_XXX5:
				{
#ifdef JP
					act = "に XXX5 された。";
#else /* JP */
					act = "XXX5's you.";
#endif /* JP */
					break;
				}
			}

			/* Message */
#ifdef JP
			if (act) message_format(sound_msg, 0, "%^s%s", m_name, act);
#else /* JP */
			if (act) message_format(sound_msg, 0, "%^s %s", m_name, act);
#endif /* JP */


			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);

			/* Apply appropriate damage */
			switch (effect)
			{
				case 0:
				{
					/* Hack -- Assume obvious */
					obvious = TRUE;

					/* Hack -- No damage */
					damage = 0;

					break;
				}

				case RBE_HURT:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Player armor reduces total damage */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					take_hit(damage, ddesc);

					break;
				}

				case RBE_POISON:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Take "poison" effect */
					if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
					{
						if (set_poisoned(p_ptr->poisoned + randint(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_POIS);

					break;
				}

				case RBE_UN_BONUS:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Allow complete resist */
					if (!p_ptr->resist_disen)
					{
						/* Apply disenchantment */
						if (apply_disenchant(0)) obvious = TRUE;
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_DISEN);

					break;
				}

				case RBE_UN_POWER:
				{
					int drained = 0;

					/* Take damage */
					take_hit(damage, ddesc);

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item */
						i = rand_int(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Drain charged wands/staves */
						if ((o_ptr->tval == TV_STAFF) ||
						    (o_ptr->tval == TV_WAND))
						{
							/* Charged? */
							if (o_ptr->pval)
							{
								drained = o_ptr->pval;

								/* Uncharge */
								o_ptr->pval = 0;
							}
						}

						if (drained)
						{
							int heal = rlev * drained;

#ifdef JP
							msg_print("ザックからエネルギーが吸い取られた！");
#else /* JP */
							msg_print("Energy drains from your pack!");
#endif /* JP */

							obvious = TRUE;

							/* Don't heal more than max hp */
							heal = MIN(heal, m_ptr->maxhp - m_ptr->hp);

							/* Heal */
							m_ptr->hp += heal;

							/* Redraw (later) if needed */
							if (p_ptr->health_who == m_idx)
								p_ptr->redraw |= (PR_HEALTH);

							/* Combine / Reorder the pack */
							p_ptr->notice |= (PN_COMBINE | PN_REORDER);

							/* Window stuff */
							p_ptr->window |= (PW_INVEN);

							/* Affect only a single inventory slot */
							break;
						}
					}

					break;
				}

				case RBE_EAT_GOLD:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Obvious */
					obvious = TRUE;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed &&
					    (rand_int(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
					                      p_ptr->lev)))
					{
						/* Saving throw message */
#ifdef JP
						msg_print("しかし素早く財布を守った！");
#else /* JP */
						msg_print("You quickly protect your money pouch!");
#endif /* JP */

						/* Occasional blink anyway */
						if (rand_int(3)) blinked = TRUE;
					}

					/* Eat gold */
					else
					{
						gold = (p_ptr->au / 10) + randint(25);
						if (gold < 2) gold = 2;
						if (gold > 5000) gold = (p_ptr->au / 20) + randint(3000);
						if (gold > p_ptr->au) gold = p_ptr->au;
						p_ptr->au -= gold;
						if (gold <= 0)
						{
#ifdef JP
							msg_print("しかし何も盗まれなかった。");
#else /* JP */
							msg_print("Nothing was stolen.");
#endif /* JP */
						}
						else if (p_ptr->au)
						{
#ifdef JP
							msg_print("財布が軽くなった気がする。");
							msg_format("$%ld のお金が盗まれた！", (long)gold);
#else /* JP */
							msg_print("Your purse feels lighter.");
							msg_format("%ld coins were stolen!", (long)gold);
#endif /* JP */
						}
						else
						{
#ifdef JP
							msg_print("財布が軽くなった気がする。");
							msg_print("お金が全部盗まれた！");
#else /* JP */
							msg_print("Your purse feels lighter.");
							msg_print("All of your coins were stolen!");
#endif /* JP */
						}

						/* Redraw gold */
						p_ptr->redraw |= (PR_GOLD);

						/* Window stuff */
						p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);

						/* Blink away */
						blinked = TRUE;
					}

					break;
				}

				case RBE_EAT_ITEM:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed &&
					    (rand_int(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
					                      p_ptr->lev)))
					{
						/* Saving throw message */
#ifdef JP
						msg_print("しかしあわててザックを取り返した！");
#else /* JP */
						msg_print("You grab hold of your backpack!");
#endif /* JP */

						/* Occasional "blink" anyway */
						blinked = TRUE;

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						object_type *i_ptr;
						object_type object_type_body;

						/* Pick an item */
						i = rand_int(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip artifacts */
						if (artifact_p(o_ptr)) continue;

						/* Get a description */
						object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 3);

						/* Message */
#ifdef JP
						msg_format("%s(%c)を%s盗まれた！",
						           o_name, index_to_label(i),
						           ((o_ptr->number > 1) ? "一つ" : ""));
#else /* JP */
						msg_format("%sour %s (%c) was stolen!",
						           ((o_ptr->number > 1) ? "One of y" : "Y"),
						           o_name, index_to_label(i));
#endif /* JP */

						/* Get local object */
						i_ptr = &object_type_body;

						/* Obtain local object */
						object_copy(i_ptr, o_ptr);

						/* Modify number */
						i_ptr->number = 1;

						/* Hack -- If a rod, staff, or wand, allocate total
						 * maximum timeouts or charges between those
						 * stolen and those missed. -LM-
						 */
						distribute_charges(o_ptr, i_ptr, 1);

						/* Carry the object */
						(void)monster_carry(m_idx, i_ptr);

						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Blink away */
						blinked = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_FOOD:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Steal some food */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item from the pack */
						i = rand_int(INVEN_PACK);

						/* Get the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip non-food objects */
						if (o_ptr->tval != TV_FOOD) continue;

						/* Get a description */
						object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 0);

						/* Message */
#ifdef JP
						msg_format("%s(%c)を%s食べられてしまった！",
						          o_name, index_to_label(i),
						          ((o_ptr->number > 1) ? "一つ" : ""));
#else /* JP */
						msg_format("%sour %s (%c) was eaten!",
						           ((o_ptr->number > 1) ? "One of y" : "Y"),
						           o_name, index_to_label(i));
#endif /* JP */

						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_LITE:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Get the lite */
					o_ptr = &inventory[INVEN_LITE];

					/* Drain fuel */
					if ((o_ptr->pval > 0) && (!artifact_p(o_ptr)))
					{
						/* Reduce fuel */
						o_ptr->pval -= (250 + randint(250));
						if (o_ptr->pval < 1) o_ptr->pval = 1;

						/* Notice */
						if (!p_ptr->blind)
						{
#ifdef JP
							msg_print("明かりが暗くなってしまった。");
#else /* JP */
							msg_print("Your light dims.");
#endif /* JP */
							obvious = TRUE;
						}

						/* Window stuff */
						p_ptr->window |= (PW_EQUIP);
					}

					break;
				}

				case RBE_ACID:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("酸を浴びせられた！");
#else /* JP */
					msg_print("You are covered in acid!");
#endif /* JP */

					/* Special damage */
					acid_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_ACID);

					break;
				}

				case RBE_ELEC:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("電撃を浴びせられた！");
#else /* JP */
					msg_print("You are struck by electricity!");
#endif /* JP */

					/* Take damage (special) */
					elec_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_ELEC);

					break;
				}

				case RBE_FIRE:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("全身が炎に包まれた！");
#else /* JP */
					msg_print("You are enveloped in flames!");
#endif /* JP */

					/* Take damage (special) */
					fire_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_FIRE);

					break;
				}

				case RBE_COLD:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("全身が冷気で覆われた！");
#else /* JP */
					msg_print("You are covered with frost!");
#endif /* JP */

					/* Take damage (special) */
					cold_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_COLD);

					break;
				}

				case RBE_BLIND:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Increase "blind" */
					if (!p_ptr->resist_blind)
					{
						if (set_blind(p_ptr->blind + 10 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_BLIND);

					break;
				}

				case RBE_CONFUSE:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Increase "confused" */
					if (!p_ptr->resist_confu)
					{
						if (set_confused(p_ptr->confused + 3 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_CONFU);

					break;
				}

				case RBE_TERRIFY:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Increase "afraid" */
					if (p_ptr->resist_fear)
					{
#ifdef JP
						msg_print("しかし恐怖に侵されなかった！");
#else /* JP */
						msg_print("You stand your ground!");
#endif /* JP */
						obvious = TRUE;
					}
					else if (rand_int(100) < p_ptr->skill_sav)
					{
#ifdef JP
						msg_print("しかし恐怖に侵されなかった！");
#else /* JP */
						msg_print("You stand your ground!");
#endif /* JP */
						obvious = TRUE;
					}
					else
					{
						if (set_afraid(p_ptr->afraid + 3 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_FEAR);

					break;
				}

				case RBE_PARALYZE:
				{
					/* Hack -- Prevent perma-paralysis via damage */
					if (p_ptr->paralyzed && (damage < 1)) damage = 1;

					/* Take damage */
					take_hit(damage, ddesc);

					/* Increase "paralyzed" */
					if (p_ptr->free_act)
					{
#ifdef JP
						msg_print("しかし効果がなかった！");
#else /* JP */
						msg_print("You are unaffected!");
#endif /* JP */
						obvious = TRUE;
					}
					else if (rand_int(100) < p_ptr->skill_sav)
					{
#ifdef JP
						msg_print("しかし効力を跳ね返した！");
#else /* JP */
						msg_print("You resist the effects!");
#endif /* JP */
						obvious = TRUE;
					}
					else
					{
						if (set_paralyzed(p_ptr->paralyzed + 3 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FREE);

					break;
				}

				case RBE_LOSE_STR:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_STR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_INT:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_INT)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_WIS:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_WIS)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_DEX:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_DEX)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CON:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_CON)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CHR:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_ALL:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Damage (stats) */
					if (do_dec_stat(A_STR)) obvious = TRUE;
					if (do_dec_stat(A_DEX)) obvious = TRUE;
					if (do_dec_stat(A_CON)) obvious = TRUE;
					if (do_dec_stat(A_INT)) obvious = TRUE;
					if (do_dec_stat(A_WIS)) obvious = TRUE;
					if (do_dec_stat(A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_SHATTER:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Reduce damage based on the player armor class */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					take_hit(damage, ddesc);

					/* Radius 8 earthquake centered at the monster */
					if (damage > 23)
					{
						int px_old = p_ptr->px;
						int py_old = p_ptr->py;
						
						earthquake(m_ptr->fy, m_ptr->fx, 8);

						/* Stop the blows if the player is pushed away */
						if ((px_old != p_ptr->px) ||
						    (py_old != p_ptr->py))
						    do_break = TRUE;
					}
					break;
				}

				case RBE_EXP_10:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && (rand_int(100) < 95))
					{
#ifdef JP
						msg_print("しかし自己の生命力を守りきった！");
#else /* JP */
						msg_print("You keep hold of your life force!");
#endif /* JP */
					}
					else
					{
						s32b d = damroll(10, 6) + (p_ptr->exp/100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life)
						{
#ifdef JP
							msg_print("生命力を少し吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life slipping away!");
#endif /* JP */
							lose_exp(d/10);
						}
						else
						{
#ifdef JP
							msg_print("生命力が体から吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life draining away!");
#endif /* JP */
							lose_exp(d);
						}
					}
					break;
				}

				case RBE_EXP_20:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && (rand_int(100) < 90))
					{
#ifdef JP
						msg_print("しかし自己の生命力を守りきった！");
#else /* JP */
						msg_print("You keep hold of your life force!");
#endif /* JP */
					}
					else
					{
						s32b d = damroll(20, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

						if (p_ptr->hold_life)
						{
#ifdef JP
							msg_print("生命力を少し吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life slipping away!");
#endif /* JP */
							lose_exp(d / 10);
						}
						else
						{
#ifdef JP
							msg_print("生命力が体から吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life draining away!");
#endif /* JP */
							lose_exp(d);
						}
					}
					break;
				}

				case RBE_EXP_40:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && (rand_int(100) < 75))
					{
#ifdef JP
						msg_print("しかし自己の生命力を守りきった！");
#else /* JP */
						msg_print("You keep hold of your life force!");
#endif /* JP */
					}
					else
					{
						s32b d = damroll(40, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

						if (p_ptr->hold_life)
						{
#ifdef JP
							msg_print("生命力を少し吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life slipping away!");
#endif /* JP */
							lose_exp(d / 10);
						}
						else
						{
#ifdef JP
							msg_print("生命力が体から吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life draining away!");
#endif /* JP */
							lose_exp(d);
						}
					}
					break;
				}

				case RBE_EXP_80:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && (rand_int(100) < 50))
					{
#ifdef JP
						msg_print("しかし自己の生命力を守りきった！");
#else /* JP */
						msg_print("You keep hold of your life force!");
#endif /* JP */
					}
					else
					{
						s32b d = damroll(80, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

						if (p_ptr->hold_life)
						{
#ifdef JP
							msg_print("生命力を少し吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life slipping away!");
#endif /* JP */
							lose_exp(d / 10);
						}
						else
						{
#ifdef JP
							msg_print("生命力が体から吸い取られた気がする！");
#else /* JP */
							msg_print("You feel your life draining away!");
#endif /* JP */
							lose_exp(d);
						}
					}
					break;
				}

				case RBE_HALLU:
				{
					/* Take damage */
					take_hit(damage, ddesc);

					/* Increase "image" */
					if (!p_ptr->resist_chaos)
					{
						if (set_image(p_ptr->image + 3 + randint(rlev / 2)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_RES_CHAOS);

					break;
				}
			}


			/* Hack -- only one of cut or stun */
			if (do_cut && do_stun)
			{
				/* Cancel cut */
				if (rand_int(100) < 50)
				{
					do_cut = 0;
				}

				/* Cancel stun */
				else
				{
					do_stun = 0;
				}
			}

			/* Handle cut */
			if (do_cut)
			{
				int k;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint(5); break;
					case 2: k = randint(5) + 5; break;
					case 3: k = randint(20) + 20; break;
					case 4: k = randint(50) + 50; break;
					case 5: k = randint(100) + 100; break;
					case 6: k = 300; break;
					default: k = 500; break;
				}

				/* Apply the cut */
				if (k) (void)set_cut(p_ptr->cut + k);
			}

			/* Handle stun */
			if (do_stun)
			{
				int k;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint(5); break;
					case 2: k = randint(10) + 10; break;
					case 3: k = randint(20) + 20; break;
					case 4: k = randint(30) + 30; break;
					case 5: k = randint(40) + 40; break;
					case 6: k = 100; break;
					default: k = 200; break;
				}

				/* Apply the stun */
				if (k) (void)set_stun(p_ptr->stun + k);
			}
		}

		/* Monster missed player */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
				case RBM_HIT:
				case RBM_TOUCH:
				case RBM_PUNCH:
				case RBM_KICK:
				case RBM_CLAW:
				case RBM_BITE:
				case RBM_STING:
				case RBM_XXX1:
				case RBM_BUTT:
				case RBM_CRUSH:
				case RBM_ENGULF:
				case RBM_XXX2:

				/* Visible monsters */
				if (m_ptr->ml)
				{
					/* Disturbing */
					disturb(1, 0);

					/* Message */
#ifdef JP
					msg_format("%^sの攻撃をかわした。", m_name);
#else /* JP */
					msg_format("%^s misses you.", m_name);
#endif /* JP */
				}

				break;
			}
		}


		/* Analyze "visible" monsters only */
		if (visible)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (l_ptr->blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (l_ptr->blows[ap_cnt] < MAX_UCHAR)
				{
					l_ptr->blows[ap_cnt]++;
				}
			}
		}

		/* Skip the other blows if necessary */
		if (do_break) break;
	}


	/* Blink away */
	if (blinked)
	{
#ifdef JP
		msg_print("突如、煙が立ち昇った！");
#else /* JP */
		msg_print("There is a puff of smoke!");
#endif /* JP */
		teleport_away(m_idx, MAX_SIGHT * 2 + 5);
	}


	/* Always notice cause of death */
	if (p_ptr->is_dead && (l_ptr->deaths < MAX_SHORT))
	{
		l_ptr->deaths++;
	}


	/* Assume we attacked */
	return (TRUE);
}
