/* File: use_obj.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


#ifndef USE_SCRIPT

#include "script.h"

static bool eat_food(object_type *o_ptr, bool *ident)
{
	/* Analyze the food */
	switch (o_ptr->sval)
	{
		case SV_FOOD_POISON:
		{
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
			{
				if (set_poisoned(p_ptr->poisoned + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_BLINDNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + rand_int(200) + 200))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_PARANOIA:
		{
			if (!p_ptr->resist_fear)
			{
				if (set_afraid(p_ptr->afraid + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_CONFUSION:
		{
			if (!p_ptr->resist_confu)
			{
				if (set_confused(p_ptr->confused + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_HALLUCINATION:
		{
			if (!p_ptr->resist_chaos)
			{
				if (set_image(p_ptr->image + rand_int(250) + 250))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_PARALYSIS:
		{
			if (!p_ptr->free_act)
			{
				if (set_paralyzed(p_ptr->paralyzed + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_WEAKNESS:
		{
#ifdef JP
			take_hit(damroll(6, 6), "�œ���H��");
#else /* JP */
			take_hit(damroll(6, 6), "poisonous food");
#endif /* JP */
			(void)do_dec_stat(A_STR);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_SICKNESS:
		{
#ifdef JP
			take_hit(damroll(6, 6), "�œ���H��");
#else /* JP */
			take_hit(damroll(6, 6), "poisonous food");
#endif /* JP */
			(void)do_dec_stat(A_CON);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_STUPIDITY:
		{
#ifdef JP
			take_hit(damroll(8, 8), "�œ���H��");
#else /* JP */
			take_hit(damroll(8, 8), "poisonous food");
#endif /* JP */
			(void)do_dec_stat(A_INT);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_NAIVETY:
		{
#ifdef JP
			take_hit(damroll(8, 8), "�œ���H��");
#else /* JP */
			take_hit(damroll(8, 8), "poisonous food");
#endif /* JP */
			(void)do_dec_stat(A_WIS);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_UNHEALTH:
		{
#ifdef JP
			take_hit(damroll(10, 10), "�œ���H��");
#else /* JP */
			take_hit(damroll(10, 10), "poisonous food");
#endif /* JP */
			(void)do_dec_stat(A_CON);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_DISEASE:
		{
#ifdef JP
			take_hit(damroll(10, 10), "�œ���H��");
#else /* JP */
			take_hit(damroll(10, 10), "poisonous food");
#endif /* JP */
			(void)do_dec_stat(A_STR);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_POISON:
		{
			if (set_poisoned(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_BLINDNESS:
		{
			if (set_blind(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_PARANOIA:
		{
			if (set_afraid(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_CONFUSION:
		{
			if (set_confused(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_SERIOUS:
		{
			if (hp_player(damroll(4, 8))) *ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORE_STR:
		{
			if (do_res_stat(A_STR)) *ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORE_CON:
		{
			if (do_res_stat(A_CON)) *ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORING:
		{
			if (do_res_stat(A_STR)) *ident = TRUE;
			if (do_res_stat(A_INT)) *ident = TRUE;
			if (do_res_stat(A_WIS)) *ident = TRUE;
			if (do_res_stat(A_DEX)) *ident = TRUE;
			if (do_res_stat(A_CON)) *ident = TRUE;
			if (do_res_stat(A_CHR)) *ident = TRUE;
			break;
		}


		/* ���ꂼ��̐H�ו��̊��z���I���W�i�����ׂ����\�� */
		case SV_FOOD_RATION:
#ifdef JP
		{
			msg_print("����͂��������B");
			*ident = TRUE;
			break;
		}
#endif /* JP */
		case SV_FOOD_BISCUIT:
#ifdef JP
		{
			msg_print("�Â��ăT�N�T�N���ĂƂĂ����������B");
			*ident = TRUE;
			break;
		}
#endif /* JP */
		case SV_FOOD_JERKY:
#ifdef JP
		{
			msg_print("���������������Ă��������B");
			*ident = TRUE;
			break;
		}
#endif /* JP */
		case SV_FOOD_SLIME_MOLD:
		{
#ifdef JP
			msg_print("����͂Ȃ�Ƃ��`�e�������������B");
#else /* JP */
			msg_print("That tastes good.");
#endif /* JP */
			*ident = TRUE;
			break;
		}

		case SV_FOOD_WAYBREAD:
		{
#ifdef JP
			msg_print("����͂Ђ��傤�ɔ������B");
#else /* JP */
			msg_print("That tastes good.");
#endif /* JP */
			(void)set_poisoned(0);
			(void)hp_player(damroll(4, 8));
			*ident = TRUE;
			break;
		}

		case SV_FOOD_PINT_OF_ALE:
#ifdef JP
		{
			msg_print("�̂ǂ����u�₩���B");
			*ident = TRUE;
			break;
		}
#endif /* JP */
		case SV_FOOD_PINT_OF_WINE:
		{
#ifdef JP
			msg_print("����͂�����B");
#else /* JP */
			msg_print("That tastes good.");
#endif /* JP */
			*ident = TRUE;
			break;
		}
	}

	/* Food can feed the player */
	(void)set_food(p_ptr->food + o_ptr->pval);

	return (TRUE);
}


static bool quaff_potion(object_type *o_ptr, bool *ident)
{
	/* Analyze the potion */
	switch (o_ptr->sval)
	{
		case SV_POTION_WATER:
#ifdef JP
		{
			msg_print("���̒��������ς肵���B");
			msg_print("�̂ǂ̊��������������܂����B");
			*ident = TRUE;
			break;
		}
#endif /* JP */
		case SV_POTION_APPLE_JUICE:
#ifdef JP
		{
			msg_print("�Â��ăT�b�p���Ƃ��Ă��āA�ƂĂ����������B");
			msg_print("�̂ǂ̊��������������܂����B");
			*ident = TRUE;
			break;
		}
#endif /* JP */
		case SV_POTION_SLIME_MOLD:
		{
#ifdef JP
			msg_print("�Ȃ�Ƃ��s�C���Ȗ����B");
			msg_print("�̂ǂ̊��������������܂����B");
#else /* JP */
			msg_print("You feel less thirsty.");
#endif /* JP */
			*ident = TRUE;
			break;
		}

		case SV_POTION_SLOWNESS:
		{
			if (set_slow(p_ptr->slow + randint(25) + 15)) *ident = TRUE;
			break;
		}

		case SV_POTION_SALT_WATER:
		{
#ifdef JP
			msg_print("�����I�v�킸�f���Ă��܂����B");
#else /* JP */
			msg_print("The potion makes you vomit!");
#endif /* JP */
			(void)set_food(PY_FOOD_STARVE - 1);
			(void)set_poisoned(0);
			(void)set_paralyzed(p_ptr->paralyzed + 4);
			*ident = TRUE;
			break;
		}

		case SV_POTION_POISON:
		{
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
			{
				if (set_poisoned(p_ptr->poisoned + rand_int(15) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_BLINDNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + rand_int(100) + 100))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_CONFUSION:
		{
			if (!p_ptr->resist_confu)
			{
				if (set_confused(p_ptr->confused + rand_int(20) + 15))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_SLEEP:
		{
			if (!p_ptr->free_act)
			{
				if (set_paralyzed(p_ptr->paralyzed + rand_int(4) + 4))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_LOSE_MEMORIES:
		{
			if (!p_ptr->hold_life && (p_ptr->exp > 0))
			{
#ifdef JP
				msg_print("�ߋ��̋L��������Ă����C������B");
#else /* JP */
				msg_print("You feel your memories fade.");
#endif /* JP */
				lose_exp(p_ptr->exp / 4);
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RUINATION:
		{
#ifdef JP
			msg_print("�g���S������Ă��āA���C�������Ă����悤���B");
			take_hit(damroll(10, 10), "�j�ł̖�");
#else /* JP */
			msg_print("Your nerves and muscles feel weak and lifeless!");
			take_hit(damroll(10, 10), "a potion of Ruination");
#endif /* JP */
			(void)dec_stat(A_DEX, 25, TRUE);
			(void)dec_stat(A_WIS, 25, TRUE);
			(void)dec_stat(A_CON, 25, TRUE);
			(void)dec_stat(A_STR, 25, TRUE);
			(void)dec_stat(A_CHR, 25, TRUE);
			(void)dec_stat(A_INT, 25, TRUE);
			*ident = TRUE;
			break;
		}

		case SV_POTION_DEC_STR:
		{
			if (do_dec_stat(A_STR)) *ident = TRUE;
			break;
		}

		case SV_POTION_DEC_INT:
		{
			if (do_dec_stat(A_INT)) *ident = TRUE;
			break;
		}

		case SV_POTION_DEC_WIS:
		{
			if (do_dec_stat(A_WIS)) *ident = TRUE;
			break;
		}

		case SV_POTION_DEC_DEX:
		{
			if (do_dec_stat(A_DEX)) *ident = TRUE;
			break;
		}

		case SV_POTION_DEC_CON:
		{
			if (do_dec_stat(A_CON)) *ident = TRUE;
			break;
		}

		case SV_POTION_DEC_CHR:
		{
			if (do_dec_stat(A_CHR)) *ident = TRUE;
			break;
		}

		case SV_POTION_DETONATIONS:
		{
#ifdef JP
			msg_print("�̂̒��Ō������������N�����I");
			take_hit(damroll(50, 20), "�����̖�");
#else /* JP */
			msg_print("Massive explosions rupture your body!");
			take_hit(damroll(50, 20), "a potion of Detonation");
#endif /* JP */
			(void)set_stun(p_ptr->stun + 75);
			(void)set_cut(p_ptr->cut + 5000);
			*ident = TRUE;
			break;
		}

		case SV_POTION_DEATH:
		{
#ifdef JP
			msg_print("���̗\�����̒����삯�߂������B");
			take_hit(5000, "���̖�");
#else /* JP */
			msg_print("A feeling of Death flows through your body.");
			take_hit(5000, "a potion of Death");
#endif /* JP */
			*ident = TRUE;
			break;
		}

		case SV_POTION_INFRAVISION:
		{
			if (set_tim_infra(p_ptr->tim_infra + 100 + randint(100)))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_DETECT_INVIS:
		{
			if (set_tim_invis(p_ptr->tim_invis + 12 + randint(12)))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_SLOW_POISON:
		{
			if (set_poisoned(p_ptr->poisoned / 2)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_POISON:
		{
			if (set_poisoned(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_BOLDNESS:
		{
			if (set_afraid(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(randint(25) + 15)) *ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5);
			}
			break;
		}

		case SV_POTION_RESIST_HEAT:
		{
			if (set_oppose_fire(p_ptr->oppose_fire + randint(10) + 10))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESIST_COLD:
		{
			if (set_oppose_cold(p_ptr->oppose_cold + randint(10) + 10))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_HEROISM:
		{
			if (hp_player(10)) *ident = TRUE;
			if (set_afraid(0)) *ident = TRUE;
			if (set_hero(p_ptr->hero + randint(25) + 25)) *ident = TRUE;
			break;
		}

		case SV_POTION_BERSERK_STRENGTH:
		{
			if (hp_player(30)) *ident = TRUE;
			if (set_afraid(0)) *ident = TRUE;
			if (set_shero(p_ptr->shero + randint(25) + 25)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_LIGHT:
		{
			if (hp_player(damroll(2, 8))) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_cut(p_ptr->cut - 10)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_SERIOUS:
		{
			if (hp_player(damroll(4, 8))) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_cut((p_ptr->cut / 2) - 50)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_CRITICAL:
		{
			if (hp_player(damroll(6, 8))) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_HEALING:
		{
			if (hp_player(300)) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_STAR_HEALING:
		{
			if (hp_player(1200)) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_LIFE:
		{
#ifdef JP
			msg_print("�̒��ɐ����͂��������ӂ�Ă����I");
#else /* JP */
			msg_print("You feel life flow through your body!");
#endif /* JP */
			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_CHR);

			/* Recalculate max. hitpoints */
			update_stuff();

			hp_player(5000);

			*ident = TRUE;
			break;
		}

		case SV_POTION_RESTORE_MANA:
		{
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
#ifdef JP
				msg_print("�����n�b�L���Ƃ����B");
#else /* JP */
				msg_print("Your feel your head clear.");
#endif /* JP */
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESTORE_EXP:
		{
			if (restore_level()) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_STR:
		{
			if (do_res_stat(A_STR)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_INT:
		{
			if (do_res_stat(A_INT)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_WIS:
		{
			if (do_res_stat(A_WIS)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_DEX:
		{
			if (do_res_stat(A_DEX)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_CON:
		{
			if (do_res_stat(A_CON)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_CHR:
		{
			if (do_res_stat(A_CHR)) *ident = TRUE;
			break;
		}

		case SV_POTION_INC_STR:
		{
			if (do_inc_stat(A_STR)) *ident = TRUE;
			break;
		}

		case SV_POTION_INC_INT:
		{
			if (do_inc_stat(A_INT)) *ident = TRUE;
			break;
		}

		case SV_POTION_INC_WIS:
		{
			if (do_inc_stat(A_WIS)) *ident = TRUE;
			break;
		}

		case SV_POTION_INC_DEX:
		{
			if (do_inc_stat(A_DEX)) *ident = TRUE;
			break;
		}

		case SV_POTION_INC_CON:
		{
			if (do_inc_stat(A_CON)) *ident = TRUE;
			break;
		}

		case SV_POTION_INC_CHR:
		{
			if (do_inc_stat(A_CHR)) *ident = TRUE;
			break;
		}

		case SV_POTION_AUGMENTATION:
		{
			if (do_inc_stat(A_STR)) *ident = TRUE;
			if (do_inc_stat(A_INT)) *ident = TRUE;
			if (do_inc_stat(A_WIS)) *ident = TRUE;
			if (do_inc_stat(A_DEX)) *ident = TRUE;
			if (do_inc_stat(A_CON)) *ident = TRUE;
			if (do_inc_stat(A_CHR)) *ident = TRUE;
			break;
		}

		case SV_POTION_ENLIGHTENMENT:
		{
#ifdef JP
			msg_print("�����̒u����Ă���󋵂��]���ɕ�����ł���...");
#else /* JP */
			msg_print("An image of your surroundings forms in your mind...");
#endif /* JP */
			wiz_lite();
			*ident = TRUE;
			break;
		}

		case SV_POTION_STAR_ENLIGHTENMENT:
		{
#ifdef JP
			msg_print("�X�Ȃ�[�ւ�������...");
#else /* JP */
			msg_print("You begin to feel more enlightened...");
#endif /* JP */
			message_flush();
			wiz_lite();
			(void)do_inc_stat(A_INT);
			(void)do_inc_stat(A_WIS);
			(void)detect_traps();
			(void)detect_doors();
			(void)detect_stairs();
			(void)detect_treasure();
			(void)detect_objects_gold();
			(void)detect_objects_normal();
			identify_pack();
			self_knowledge();
			*ident = TRUE;
			break;
		}

		case SV_POTION_SELF_KNOWLEDGE:
		{
#ifdef JP
			msg_print("�������g�̂��Ƃ������͕��������C������...");
#else /* JP */
			msg_print("You begin to know yourself a little better...");
#endif /* JP */
			message_flush();
			self_knowledge();
			*ident = TRUE;
			break;
		}

		case SV_POTION_EXPERIENCE:
		{
			if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
				msg_print("�X�Ɍo����ς񂾂悤�ȋC������B");
#else /* JP */
				msg_print("You feel more experienced.");
#endif /* JP */
				gain_exp(ee);
				*ident = TRUE;
			}
			break;
		}
	}

	return (TRUE);
}


static bool read_scroll(object_type *o_ptr, bool *ident)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int k;

	bool used_up = TRUE;


	/* Analyze the scroll */
	switch (o_ptr->sval)
	{
		case SV_SCROLL_DARKNESS:
		{
			if (!p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + 3 + randint(5));
			}
			if (unlite_area(10, 3)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_AGGRAVATE_MONSTER:
		{
#ifdef JP
			msg_print("�J���������Ȃ�l�ȉ����ӂ�𕢂����B");
#else /* JP */
			msg_print("There is a high pitched humming noise.");
#endif /* JP */
			aggravate_monsters(0);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_ARMOR:
		{
			if (curse_armor()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_WEAPON:
		{
			if (curse_weapon()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_SUMMON_MONSTER:
		{
			sound(MSG_SUM_MONSTER);
			for (k = 0; k < randint(3); k++)
			{
				if (summon_specific(py, px, p_ptr->depth, 0))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_UNDEAD:
		{
			sound(MSG_SUM_UNDEAD);
			for (k = 0; k < randint(3); k++)
			{
				if (summon_specific(py, px, p_ptr->depth, SUMMON_UNDEAD))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_TRAP_CREATION:
		{
			if (trap_creation()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_PHASE_DOOR:
		{
			teleport_player(10);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT:
		{
			teleport_player(100);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT_LEVEL:
		{
			(void)teleport_player_level();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_WORD_OF_RECALL:
		{
			set_recall();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_IDENTIFY:
		{
			*ident = TRUE;
			if (!ident_spell()) used_up = FALSE;
			break;
		}

		case SV_SCROLL_STAR_IDENTIFY:
		{
			*ident = TRUE;
			if (!identify_fully()) used_up = FALSE;
			break;
		}

		case SV_SCROLL_REMOVE_CURSE:
		{
			if (remove_curse())
			{
#ifdef JP
				msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else /* JP */
				msg_print("You feel as if someone is watching over you.");
#endif /* JP */
				*ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_STAR_REMOVE_CURSE:
		{
			remove_all_curse();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_ARMOR:
		{
			*ident = TRUE;
			if (!enchant_spell(0, 0, 1)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			if (!enchant_spell(1, 0, 0)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			if (!enchant_spell(0, 1, 0)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if (!enchant_spell(0, 0, randint(3) + 2)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			if (!enchant_spell(randint(3), randint(3), 0)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_RECHARGING:
		{
			if (!recharge(60)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_LIGHT:
		{
			if (lite_area(damroll(2, 8), 2)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_MAPPING:
		{
			map_area();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_GOLD:
		{
			if (detect_treasure()) *ident = TRUE;
			if (detect_objects_gold()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_ITEM:
		{
			if (detect_objects_normal()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_TRAP:
		{
			if (detect_traps()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_DOOR:
		{
			if (detect_doors()) *ident = TRUE;
			if (detect_stairs()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_INVIS:
		{
			if (detect_monsters_invis()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_SATISFY_HUNGER:
		{
			if (set_food(PY_FOOD_MAX - 1)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_BLESSING:
		{
			if (set_blessed(p_ptr->blessed + randint(12) + 6)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_HOLY_CHANT:
		{
			if (set_blessed(p_ptr->blessed + randint(24) + 12)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_HOLY_PRAYER:
		{
			if (set_blessed(p_ptr->blessed + randint(48) + 24)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_MONSTER_CONFUSION:
		{
			if (p_ptr->confusing == 0)
			{
#ifdef JP
				msg_print("�肪�P���n�߂��B");
#else /* JP */
				msg_print("Your hands begin to glow.");
#endif /* JP */
				p_ptr->confusing = TRUE;
				*ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			k = 3 * p_ptr->lev;
			if (set_protevil(p_ptr->protevil + randint(25) + k)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			warding_glyph();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
		{
			if (destroy_doors_touch()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_DESTRUCTION:
		{
			destroy_area(py, px, 15, TRUE);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_DISPEL_UNDEAD:
		{
			if (dispel_undead(60)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_BANISHMENT:
		{
			if (!banishment()) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_MASS_BANISHMENT:
		{
			(void)mass_banishment();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_ACQUIREMENT:
		{
			acquirement(py, px, 1, TRUE);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ACQUIREMENT:
		{
			acquirement(py, px, randint(2) + 1, TRUE);
			*ident = TRUE;
			break;
		}
	}

	return (used_up);
}


static bool use_staff(object_type *o_ptr, bool *ident)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int k;

	bool use_charge = TRUE;

	/* Analyze the staff */
	switch (o_ptr->sval)
	{
		case SV_STAFF_DARKNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + 3 + randint(5))) *ident = TRUE;
			}
			if (unlite_area(10, 3)) *ident = TRUE;
			break;
		}

		case SV_STAFF_SLOWNESS:
		{
			if (set_slow(p_ptr->slow + randint(30) + 15)) *ident = TRUE;
			break;
		}

		case SV_STAFF_HASTE_MONSTERS:
		{
			if (speed_monsters()) *ident = TRUE;
			break;
		}

		case SV_STAFF_SUMMONING:
		{
			sound(MSG_SUM_MONSTER);
			for (k = 0; k < randint(4); k++)
			{
				if (summon_specific(py, px, p_ptr->depth, 0))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_STAFF_TELEPORTATION:
		{
			teleport_player(100);
			*ident = TRUE;
			break;
		}

		case SV_STAFF_IDENTIFY:
		{
			if (!ident_spell()) use_charge = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_STAFF_REMOVE_CURSE:
		{
			if (remove_curse())
			{
				if (!p_ptr->blind)
				{
#ifdef JP
					msg_print("��͈�u�u���[�ɋP����...");
#else /* JP */
					msg_print("The staff glows blue for a moment...");
#endif /* JP */
				}
				*ident = TRUE;
			}
			break;
		}

		case SV_STAFF_STARLITE:
		{
			if (!p_ptr->blind)
			{
#ifdef JP
				msg_print("��̐悪���邭�P����...");
#else /* JP */
				msg_print("The end of the staff glows brightly...");
#endif /* JP */
			}
			for (k = 0; k < 8; k++) lite_line(ddd[k]);
			*ident = TRUE;
			break;
		}

		case SV_STAFF_LITE:
		{
			if (lite_area(damroll(2, 8), 2)) *ident = TRUE;
			break;
		}

		case SV_STAFF_MAPPING:
		{
			map_area();
			*ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_GOLD:
		{
			if (detect_treasure()) *ident = TRUE;
			if (detect_objects_gold()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_ITEM:
		{
			if (detect_objects_normal()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_TRAP:
		{
			if (detect_traps()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_DOOR:
		{
			if (detect_doors()) *ident = TRUE;
			if (detect_stairs()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_INVIS:
		{
			if (detect_monsters_invis()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_EVIL:
		{
			if (detect_monsters_evil()) *ident = TRUE;
			break;
		}

		case SV_STAFF_CURE_LIGHT:
		{
			if (hp_player(randint(8))) *ident = TRUE;
			break;
		}

		case SV_STAFF_CURING:
		{
			if (set_blind(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_STAFF_HEALING:
		{
			if (hp_player(300)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_STAFF_THE_MAGI:
		{
			if (do_res_stat(A_INT)) *ident = TRUE;
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
				*ident = TRUE;
#ifdef JP
				msg_print("�����n�b�L���Ƃ����B");
#else /* JP */
				msg_print("Your feel your head clear.");
#endif /* JP */
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
			}
			break;
		}

		case SV_STAFF_SLEEP_MONSTERS:
		{
			if (sleep_monsters()) *ident = TRUE;
			break;
		}

		case SV_STAFF_SLOW_MONSTERS:
		{
			if (slow_monsters()) *ident = TRUE;
			break;
		}

		case SV_STAFF_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(randint(30) + 15)) *ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5);
			}
			break;
		}

		case SV_STAFF_PROBING:
		{
			probing();
			*ident = TRUE;
			break;
		}

		case SV_STAFF_DISPEL_EVIL:
		{
			if (dispel_evil(60)) *ident = TRUE;
			break;
		}

		case SV_STAFF_POWER:
		{
			if (dispel_monsters(120)) *ident = TRUE;
			break;
		}

		case SV_STAFF_HOLINESS:
		{
			if (dispel_evil(120)) *ident = TRUE;
			k = 3 * p_ptr->lev;
			if (set_protevil(p_ptr->protevil + randint(25) + k)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_afraid(0)) *ident = TRUE;
			if (hp_player(50)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_STAFF_BANISHMENT:
		{
			if (!banishment()) use_charge = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_STAFF_EARTHQUAKES:
		{
			earthquake(py, px, 10);
			*ident = TRUE;
			break;
		}

		case SV_STAFF_DESTRUCTION:
		{
			destroy_area(py, px, 15, TRUE);
			*ident = TRUE;
			break;
		}
	}

	return (use_charge);
}


static bool aim_wand(object_type *o_ptr, bool *ident)
{
	int lev, chance, dir, sval;


	/* Allow direction to be cancelled for free */
	if (!get_aim_dir(&dir)) return (FALSE);

	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Not identified yet */
	*ident = FALSE;

	/* Get the level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* High level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���@�_�����܂��g���Ȃ������B");
#else /* JP */
		msg_print("You failed to use the wand properly.");
#endif /* JP */
		return (FALSE);
	}

	/* The wand is already empty! */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���̖��@�_�ɂ͂������͂��c���Ă��Ȃ��B");
#else /* JP */
		msg_print("The wand has no charges left.");
#endif /* JP */
		o_ptr->ident |= (IDENT_EMPTY);
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN);
		return (FALSE);
	}


	/* Sound */
	/* TODO: Create wand sound?  Do the individual effects have sounds? */
	/* sound(MSG_ZAP_ROD); */


	/* XXX Hack -- Extract the "sval" effect */
	sval = o_ptr->sval;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if (sval == SV_WAND_WONDER) sval = rand_int(SV_WAND_WONDER);

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_MONSTER:
		{
			if (heal_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if (speed_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if (clone_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			if (teleport_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if (disarm_trap(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_TRAP_DOOR_DEST:
		{
			if (destroy_door(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_STONE_TO_MUD:
		{
			if (wall_to_mud(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_LITE:
		{
#ifdef JP
			msg_print("���P�������������ꂽ�B");
#else /* JP */
			msg_print("A line of blue shimmering light appears.");
#endif /* JP */
			lite_line(dir);
			*ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			if (sleep_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_SLOW_MONSTER:
		{
			if (slow_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			if (confuse_monster(dir, 10)) *ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			if (fear_monster(dir, 10)) *ident = TRUE;
			break;
		}

		case SV_WAND_DRAIN_LIFE:
		{
			if (drain_life(dir, 150)) *ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if (poly_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_STINKING_CLOUD:
		{
			fire_ball(GF_POIS, dir, 12, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_MAGIC_MISSILE:
		{
			fire_bolt_or_beam(20, GF_MISSILE, dir, damroll(3, 4));
			*ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BOLT:
		{
			fire_bolt_or_beam(20, GF_ACID, dir, damroll(10, 8));
			*ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BOLT:
		{
			fire_bolt_or_beam(20, GF_ELEC, dir, damroll(6, 6));
			*ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BOLT:
		{
			fire_bolt_or_beam(20, GF_FIRE, dir, damroll(12, 8));
			*ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BOLT:
		{
			fire_bolt_or_beam(20, GF_COLD, dir, damroll(6, 8));
			*ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 120, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 64, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 144, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 96, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_WONDER:
		{
#ifdef JP
			msg_print("�����ƁA��̖��@�_���n���������B");
#else /* JP */
			msg_print("Oops.  Wand of wonder activated.");
#endif /* JP */
			break;
		}

		case SV_WAND_DRAGON_FIRE:
		{
			fire_ball(GF_FIRE, dir, 200, 3);
			*ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			fire_ball(GF_COLD, dir, 160, 3);
			*ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			switch (randint(5))
			{
				case 1:
				{
					fire_ball(GF_ACID, dir, 200, 3);
					break;
				}

				case 2:
				{
					fire_ball(GF_ELEC, dir, 160, 3);
					break;
				}

				case 3:
				{
					fire_ball(GF_FIRE, dir, 200, 3);
					break;
				}

				case 4:
				{
					fire_ball(GF_COLD, dir, 160, 3);
					break;
				}

				default:
				{
					fire_ball(GF_POIS, dir, 120, 3);
					break;
				}
			}

			*ident = TRUE;
			break;
		}

		case SV_WAND_ANNIHILATION:
		{
			if (drain_life(dir, 250)) *ident = TRUE;
			break;
		}
	}

	return (TRUE);
}


static bool zap_rod(object_type *o_ptr, bool *ident)
{
	int chance, dir, lev;
	bool used_charge = TRUE;
	object_kind *k_ptr = &k_info[o_ptr->k_idx];


	/* Get a direction (unless KNOWN not to need it) */
	if ((o_ptr->sval >= SV_ROD_MIN_DIRECTION) || !object_aware_p(o_ptr))
	{
		/* Get a direction, allow cancel */
		if (!get_aim_dir(&dir)) return FALSE;
	}


	/* Take a turn */
	p_ptr->energy_use = 100;

	/* Not identified yet */
	*ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* High level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
		msg_print("You failed to use the rod properly.");
		return FALSE;
	}

	/* Still charging? */
	if (o_ptr->timeout > (o_ptr->pval - k_ptr->pval))
	{
		if (flush_failure) flush();

		if (o_ptr->number == 1)
#ifdef JP
			msg_print("���̃��b�h�͂܂����͂��[�U���Ă���Œ����B");
#else /* JP */
			msg_print("The rod is still charging."); /* BUGFIX - added a period */
#endif /* JP */
		else
#ifdef JP
			msg_print("���̃��b�h�͂܂����͂��[�U���Ă���Œ����B");
#else /* JP */
			msg_print("The rods are all still charging."); /* BUGFIX - added a period */
#endif /* JP */

		return FALSE;
	}

	/* Sound */
	sound(MSG_ZAP_ROD);

	/* Analyze the rod */
	switch (o_ptr->sval)
	{
		case SV_ROD_DETECT_TRAP:
		{
			if (detect_traps()) *ident = TRUE;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			if (detect_doors()) *ident = TRUE;
			if (detect_stairs()) *ident = TRUE;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			*ident = TRUE;
			if (!ident_spell()) used_charge = FALSE;
			break;
		}

		case SV_ROD_RECALL:
		{
			set_recall();
			*ident = TRUE;
			break;
		}

		case SV_ROD_ILLUMINATION:
		{
			if (lite_area(damroll(2, 8), 2)) *ident = TRUE;
			break;
		}

		case SV_ROD_MAPPING:
		{
			map_area();
			*ident = TRUE;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detect_all();
			*ident = TRUE;
			break;
		}

		case SV_ROD_PROBING:
		{
			probing();
			*ident = TRUE;
			break;
		}

		case SV_ROD_CURING:
		{
			if (set_blind(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_ROD_HEALING:
		{
			if (hp_player(500)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_ROD_RESTORATION:
		{
			if (restore_level()) *ident = TRUE;
			if (do_res_stat(A_STR)) *ident = TRUE;
			if (do_res_stat(A_INT)) *ident = TRUE;
			if (do_res_stat(A_WIS)) *ident = TRUE;
			if (do_res_stat(A_DEX)) *ident = TRUE;
			if (do_res_stat(A_CON)) *ident = TRUE;
			if (do_res_stat(A_CHR)) *ident = TRUE;
			break;
		}

		case SV_ROD_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(randint(30) + 15)) *ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5);
			}
			break;
		}

		case SV_ROD_TELEPORT_AWAY:
		{
			if (teleport_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_DISARMING:
		{
			if (disarm_trap(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_LITE:
		{
#ifdef JP
			msg_print("���P�������������ꂽ�B");
#else /* JP */
			msg_print("A line of blue shimmering light appears.");
#endif /* JP */
			lite_line(dir);
			*ident = TRUE;
			break;
		}

		case SV_ROD_SLEEP_MONSTER:
		{
			if (sleep_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_SLOW_MONSTER:
		{
			if (slow_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_DRAIN_LIFE:
		{
			if (drain_life(dir, 150)) *ident = TRUE;
			break;
		}

		case SV_ROD_POLYMORPH:
		{
			if (poly_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BOLT:
		{
			fire_bolt_or_beam(10, GF_ACID, dir, damroll(12, 8));
			*ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BOLT:
		{
			fire_bolt_or_beam(10, GF_ELEC, dir, damroll(6, 6));
			*ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BOLT:
		{
			fire_bolt_or_beam(10, GF_FIRE, dir, damroll(16, 8));
			*ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BOLT:
		{
			fire_bolt_or_beam(10, GF_COLD, dir, damroll(10, 8));
			*ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 120, 2);
			*ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 64, 2);
			*ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 144, 2);
			*ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 96, 2);
			*ident = TRUE;
			break;
		}
	}

	/* Drain the charge */
	if (used_charge) o_ptr->timeout += k_ptr->pval;

	return TRUE;
}


/*
 * Activate a wielded object.  Wielded objects never stack.
 * And even if they did, activatable objects never stack.
 *
 * Currently, only (some) artifacts, and Dragon Scale Mail, can be activated.
 * But one could, for example, easily make an activatable "Ring of Plasma".
 *
 * Note that it always takes a turn to activate an artifact, even if
 * the user hits "escape" at the "direction" prompt.
 */
static bool activate_object(object_type *o_ptr, bool *ident)
{
	int k, dir, i, chance;


	/* Check the recharge */
	if (o_ptr->timeout)
	{
#ifdef JP
		msg_print("����͔����ɉ��𗧂āA�P���A������...");
#else /* JP */
		msg_print("It whines, glows and fades...");
#endif /* JP */
		return FALSE;
	}

	/* Activate the artifact */
#ifdef JP
	message(MSG_ACT_ARTIFACT, 0, "�n��������...");
#else /* JP */
	message(MSG_ACT_ARTIFACT, 0, "You activate it...");
#endif /* JP */

	/* Artifacts */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];
		char o_name[80];

		/* Get the basic name of the object */
		object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 0);

		switch (a_ptr->activation)
		{
			case ACT_ILLUMINATION:
			{
#ifdef JP
				msg_format("%s���琟�񂾌������ӂ�o��...", o_name);
#else /* JP */
				msg_format("The %s wells with clear light...", o_name);
#endif /* JP */
				lite_area(damroll(2, 15), 3);
				break;
			}

			case ACT_MAGIC_MAP:
			{
#ifdef JP
				msg_format("%s��ῂ����P����...", o_name);
#else /* JP */
				msg_format("The %s shines brightly...", o_name);
#endif /* JP */
				map_area();
				break;
			}

			case ACT_CLAIRVOYANCE:
			{
#ifdef JP
				msg_format("%s�͐[���O���[���ɋP����...", o_name);
#else /* JP */
				msg_format("The %s glows a deep green...", o_name);
#endif /* JP */
				wiz_lite();
				(void)detect_traps();
				(void)detect_doors();
				(void)detect_stairs();
				break;
			}

			case ACT_PROT_EVIL:
			{
#ifdef JP
				msg_format("%s����s����������o��...", o_name);
#else /* JP */
				msg_format("The %s lets out a shrill wail...", o_name);
#endif /* JP */
				k = 3 * p_ptr->lev;
				(void)set_protevil(p_ptr->protevil + randint(25) + k);
				break;
			}

			case ACT_DISP_EVIL:
			{
#ifdef JP
				msg_format("%s�͕ӂ��P�̃I�[���Ŗ�������...", o_name);
#else /* JP */
				msg_format("The %s floods the area with goodness...", o_name);
#endif /* JP */
				dispel_evil(p_ptr->lev * 5);
				break;
			}

			case ACT_HASTE2:
			{
#ifdef JP
				msg_format("%s�͖��邭�P����...", o_name);
#else /* JP */
				msg_format("The %s glows brightly...", o_name);
#endif /* JP */
				if (!p_ptr->fast)
				{
					(void)set_fast(randint(75) + 75);
				}
				else
				{
					(void)set_fast(p_ptr->fast + 5);
				}
				break;
			}

			case ACT_FIRE3:
			{
#ifdef JP
				msg_format("%s�͐[�g�ɋP����...", o_name);
#else /* JP */
				msg_format("The %s glows deep red...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_FIRE, dir, 120, 3);
				break;
			}

			case ACT_FROST5:
			{
#ifdef JP
				msg_format("%s�͔������邭�P����...", o_name);
#else /* JP */
				msg_format("The %s glows bright white...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_COLD, dir, 200, 3);
				break;
			}

			case ACT_ELEC2:
			{
#ifdef JP
				msg_format("%s�͐[���u���[�ɋP����...", o_name);
#else /* JP */
				msg_format("The %s glows deep blue...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_ELEC, dir, 250, 3);
				break;
			}

			case ACT_BIZZARE:
			{
#ifdef JP
				msg_format("%s�͎����ɋP����...", o_name);
#else /* JP */
				msg_format("The %s glows intensely black...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				ring_of_power(dir);
				break;
			}


			case ACT_STAR_BALL:
			{
#ifdef JP
				msg_format("%s����Ȃŕ���ꂽ...", o_name);
#else /* JP */
				msg_format("Your %s is surrounded by lightning...", o_name);
#endif /* JP */
				for (i = 0; i < 8; i++) fire_ball(GF_ELEC, ddd[i], 150, 3);
				break;
			}

			case ACT_RAGE_BLESS_RESIST:
			{
#ifdef JP
				msg_format("%s���l�X�ȐF�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows many colours...", o_name);
#endif /* JP */
				(void)hp_player(30);
				(void)set_afraid(0);
				(void)set_shero(p_ptr->shero + randint(50) + 50);
				(void)set_blessed(p_ptr->blessed + randint(50) + 50);
				(void)set_oppose_acid(p_ptr->oppose_acid + randint(50) + 50);
				(void)set_oppose_elec(p_ptr->oppose_elec + randint(50) + 50);
				(void)set_oppose_fire(p_ptr->oppose_fire + randint(50) + 50);
				(void)set_oppose_cold(p_ptr->oppose_cold + randint(50) + 50);
				(void)set_oppose_pois(p_ptr->oppose_pois + randint(50) + 50);
				break;
			}

			case ACT_HEAL2:
			{
#ifdef JP
				msg_format("%s���������邭�P����...", o_name);
				msg_print("�ƂĂ��C�����悭�Ȃ���...");
#else /* JP */
				msg_format("Your %s glows a bright white...", o_name);
				msg_print("You feel much better...");
#endif /* JP */
				(void)hp_player(1000);
				(void)set_cut(0);
				break;
			}

			case ACT_PHASE:
			{
#ifdef JP
				msg_format("%s���ӂ�̋�Ԃ��䂪�܂���...", o_name);
#else /* JP */
				msg_format("Your %s twists space around you...", o_name);
#endif /* JP */
				teleport_player(10);
				break;
			}

			case ACT_BANISHMENT:
			{
#ifdef JP
				msg_format("%s���[���u���[�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows deep blue...", o_name);
#endif /* JP */
				if (!banishment()) return FALSE;
				break;
			}

			case ACT_TRAP_DOOR_DEST:
			{
#ifdef JP
				msg_format("%s���Ԃ����邭�P����...", o_name);
#else /* JP */
				msg_format("Your %s glows bright red...", o_name);
#endif /* JP */
				destroy_doors_touch();
				break;
			}

			case ACT_DETECT:
			{
#ifdef JP
				msg_format("%s���������邭�P����...", o_name);
				msg_print("�S�ɃC���[�W��������ł���...");
#else /* JP */
				msg_format("Your %s glows bright white...", o_name);
				msg_print("An image forms in your mind...");
#endif /* JP */
				detect_all();
				break;
			}

			case ACT_HEAL1:
			{
#ifdef JP
				msg_format("%s���[���u���[�ɋP����...", o_name);
				msg_print("�̓��ɒg�����ۓ�����������...");
#else /* JP */
				msg_format("Your %s glows deep blue...", o_name);
				msg_print("You feel a warm tingling inside...");
#endif /* JP */
				(void)hp_player(500);
				(void)set_cut(0);
				break;
			}

			case ACT_RESIST:
			{
#ifdef JP
				msg_format("%s���l�X�ȐF�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows many colours...", o_name);
#endif /* JP */
				(void)set_oppose_acid(p_ptr->oppose_acid + randint(20) + 20);
				(void)set_oppose_elec(p_ptr->oppose_elec + randint(20) + 20);
				(void)set_oppose_fire(p_ptr->oppose_fire + randint(20) + 20);
				(void)set_oppose_cold(p_ptr->oppose_cold + randint(20) + 20);
				(void)set_oppose_pois(p_ptr->oppose_pois + randint(20) + 20);
				break;
			}

			case ACT_SLEEP:
			{
#ifdef JP
				msg_format("%s���[���u���[�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows deep blue...", o_name);
#endif /* JP */
				sleep_monsters_touch();
				break;
			}

			case ACT_RECHARGE1:
			{
#ifdef JP
				msg_format("%s�����F�����邭�P����...", o_name);
#else /* JP */
				msg_format("Your %s glows bright yellow...", o_name);
#endif /* JP */
				if (!recharge(60)) return FALSE;
				break;
			}

			case ACT_TELEPORT:
			{
#ifdef JP
				msg_format("%s���ӂ�̋�Ԃ��䂪�܂���...", o_name);
#else /* JP */
				msg_format("Your %s twists space around you...", o_name);
#endif /* JP */
				teleport_player(100);
				break;
			}

			case ACT_RESTORE_LIFE:
			{
#ifdef JP
				msg_format("%s���[�g�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows a deep red...", o_name);
#endif /* JP */
				restore_level();
				break;
			}

			case ACT_MISSILE:
			{
#ifdef JP
				msg_format("%s��ῂ������炢�ɖ��邭�P����...", o_name);
#else /* JP */
				msg_format("Your %s glows extremely brightly...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_MISSILE, dir, damroll(2, 6));
				break;
			}

			case ACT_FIRE1:
			{
#ifdef JP
				msg_format("%s�����ɕ���ꂽ...", o_name);
#else /* JP */
				msg_format("Your %s is covered in fire...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_FIRE, dir, damroll(9, 8));
				break;
			}

			case ACT_FROST1:
			{
#ifdef JP
				msg_format("%s����C�ɕ���ꂽ...", o_name);
#else /* JP */
				msg_format("Your %s is covered in frost...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_COLD, dir, damroll(6, 8));
				break;
			}

			case ACT_LIGHTNING_BOLT:
			{
#ifdef JP
				msg_format("%s���ΉԂɕ���ꂽ...", o_name);
#else /* JP */
				msg_format("Your %s is covered in sparks...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_ELEC, dir, damroll(4, 8));
				break;
			}

			case ACT_ACID1:
			{
#ifdef JP
				msg_format("%s���_�ɕ���ꂽ...", o_name);
#else /* JP */
				msg_format("Your %s is covered in acid...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_ACID, dir, damroll(5, 8));
				break;
			}

			case ACT_ARROW:
			{
#ifdef JP
				msg_format("%s�ɖ��@�̃g�Q�����ꂽ...", o_name);
#else /* JP */
				msg_format("Your %s grows magical spikes...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_ARROW, dir, 150);
				break;
			}

			case ACT_HASTE1:
			{
#ifdef JP
				msg_format("%s���O���[���ɖ��邭�P����...", o_name);
#else /* JP */
				msg_format("Your %s glows bright green...", o_name);
#endif /* JP */
				if (!p_ptr->fast)
				{
					(void)set_fast(randint(20) + 20);
				}
				else
				{
					(void)set_fast(p_ptr->fast + 5);
				}
				break;
			}

			case ACT_REM_FEAR_POIS:
			{
#ifdef JP
				msg_format("%s���[���u���[�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows deep blue...", o_name);
#endif /* JP */
				(void)set_afraid(0);
				(void)set_poisoned(0);
				break;
			}

			case ACT_STINKING_CLOUD:
			{
#ifdef JP
				msg_format("%s���[���ΐF�Ɍۓ����Ă���...", o_name);
#else /* JP */
				msg_format("Your %s throbs deep green...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_POIS, dir, 12, 3);
				break;
			}

			case ACT_FROST2:
			{
#ifdef JP
				msg_format("%s����C�ɕ���ꂽ...", o_name);
#else /* JP */
				msg_format("Your %s is covered in frost...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_COLD, dir, 48, 2);
				break;
			}

			case ACT_FROST4:
			{
#ifdef JP
				msg_format("%s���W���u���[�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows a pale blue...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_COLD, dir, damroll(12, 8));
				break;
			}

			case ACT_FROST3:
			{
#ifdef JP
				msg_format("%s�����������P����...", o_name);
#else /* JP */
				msg_format("Your %s glows a intense blue...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_COLD, dir, 100, 2);
				break;
			}

			case ACT_FIRE2:
			{
#ifdef JP
				msg_format("%s���牊�������o����...", o_name);
#else /* JP */
				msg_format("Your %s rages in fire...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_FIRE, dir, 72, 2);
				break;
			}

			case ACT_DRAIN_LIFE2:
			{
#ifdef JP
				msg_format("%s�������P����...", o_name);
#else /* JP */
				msg_format("Your %s glows black...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				drain_life(dir, 120);
				break;
			}

			case ACT_STONE_TO_MUD:
			{
#ifdef JP
				msg_format("%s���ۓ�����...", o_name);
#else /* JP */
				msg_format("Your %s pulsates...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				wall_to_mud(dir);
				break;
			}

			case ACT_MASS_BANISHMENT:
			{
#ifdef JP
				msg_format("%s����Ђǂ��s����������o��...", o_name);
#else /* JP */
				msg_format("Your %s lets out a long, shrill note...", o_name);
#endif /* JP */
				(void)mass_banishment();
				break;
			}

			case ACT_CURE_WOUNDS:
			{
#ifdef JP
				msg_format("%s���[���̌�����˂���...", o_name);
#else /* JP */
				msg_format("Your %s radiates deep purple...", o_name);
#endif /* JP */
				hp_player(damroll(4, 8));
				(void)set_cut((p_ptr->cut / 2) - 50);
				break;
			}

			case ACT_TELE_AWAY:
			{
#ifdef JP
				msg_format("%s���[�g�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows deep red...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				teleport_monster(dir);
				break;
			}

			case ACT_WOR:
			{
#ifdef JP
				msg_format("%s���_�炩�������P����...", o_name);
#else /* JP */
				msg_format("Your %s glows soft white...", o_name);
#endif /* JP */
				set_recall();
				break;
			}

			case ACT_CONFUSE:
			{
#ifdef JP
				msg_format("%s���l�X�ȐF�̉ΉԂ𔭂���...", o_name);
#else /* JP */
				msg_format("Your %s glows in scintillating colours...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				confuse_monster(dir, 20);
				break;
			}

			case ACT_IDENTIFY:
			{
#ifdef JP
				msg_format("%s�����F���P����...", o_name);
#else /* JP */
				msg_format("Your %s glows yellow...", o_name);
#endif /* JP */
				if (!ident_spell()) return FALSE;
				break;
			}

			case ACT_PROBE:
			{
#ifdef JP
				msg_format("%s�����邭�P����...", o_name);
#else /* JP */
				msg_format("Your %s glows brightly...", o_name);
#endif /* JP */
				probing();
				break;
			}

			case ACT_DRAIN_LIFE1:
			{
#ifdef JP
				msg_format("%s�������P����...", o_name);
#else /* JP */
				msg_format("Your %s glows white...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				drain_life(dir, 90);
				break;
			}

			case ACT_FIREBRAND:
			{
#ifdef JP
				msg_format("%s���[�g�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows deep red...", o_name);
#endif /* JP */
				if (!brand_bolts()) return FALSE;
				break;
			}

			case ACT_STARLIGHT:
			{
#ifdef JP
				msg_format("%s������̐��̌��ŋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows with the light of a thousand stars...", o_name);
#endif /* JP */
				for (k = 0; k < 8; k++) strong_lite_line(ddd[k]);
				break;
			}

			case ACT_MANA_BOLT:
			{
#ifdef JP
				msg_format("%s�������P����...", o_name);
#else /* JP */
				msg_format("Your %s glows white...", o_name);
#endif /* JP */
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_MANA, dir, damroll(12, 8));
				break;
			}

			case ACT_BERSERKER:
			{
#ifdef JP
				msg_format("%s���{��悤�ɋP����...", o_name);
#else /* JP */
				msg_format("Your %s glows in anger...", o_name);
#endif /* JP */
				set_shero(p_ptr->shero + randint(50) + 50);
				break;
			}
		}

		/* Set the recharge time */
		if (a_ptr->randtime)
			o_ptr->timeout = a_ptr->time + (byte)randint(a_ptr->randtime);
		else
			o_ptr->timeout = a_ptr->time;

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return FALSE;
	}


	/* Hack -- Dragon Scale Mail can be activated as well */
	if (o_ptr->tval == TV_DRAG_ARMOR)
	{
		/* Get a direction for breathing (or abort) */
		if (!get_aim_dir(&dir)) return FALSE;

		/* Branch on the sub-type */
		switch (o_ptr->sval)
		{
			case SV_DRAGON_BLUE:
			{
				sound(MSG_BR_ELEC);
#ifdef JP
				msg_print("���Ȃ��͈�Ȃ̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe lightning.");
#endif /* JP */
				fire_ball(GF_ELEC, dir, 100, 2);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

			case SV_DRAGON_WHITE:
			{
				sound(MSG_BR_FROST);
#ifdef JP
				msg_print("���Ȃ��͗�C�̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe frost.");
#endif /* JP */
				fire_ball(GF_COLD, dir, 110, 2);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

			case SV_DRAGON_BLACK:
			{
				sound(MSG_BR_ACID);
#ifdef JP
				msg_print("���Ȃ��͎_�̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe acid.");
#endif /* JP */
				fire_ball(GF_ACID, dir, 130, 2);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

			case SV_DRAGON_GREEN:
			{
				sound(MSG_BR_GAS);
#ifdef JP
				msg_print("���Ȃ��͓ŃK�X�̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe poison gas.");
#endif /* JP */
				fire_ball(GF_POIS, dir, 150, 2);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

			case SV_DRAGON_RED:
			{
				sound(MSG_BR_FIRE);
#ifdef JP
				msg_print("���Ȃ��͉Ή��̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe fire.");
#endif /* JP */
				fire_ball(GF_FIRE, dir, 200, 2);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

			case SV_DRAGON_MULTIHUED:
			{
				chance = rand_int(5);
				sound(     ((chance == 1) ? MSG_BR_ELEC :
				            ((chance == 2) ? MSG_BR_FROST :
				             ((chance == 3) ? MSG_BR_ACID :
				              ((chance == 4) ? MSG_BR_GAS : MSG_BR_FIRE)))));
#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B",
				           ((chance == 1) ? "���" :
				            ((chance == 2) ? "��C" :
				             ((chance == 3) ? "�_" :
				              ((chance == 4) ? "�ŃK�X" : "�Ή�")))));
#else /* JP */
				msg_format("You breathe %s.",
				           ((chance == 1) ? "lightning" :
				            ((chance == 2) ? "frost" :
				             ((chance == 3) ? "acid" :
				              ((chance == 4) ? "poison gas" : "fire")))));
#endif /* JP */
				fire_ball(((chance == 1) ? GF_ELEC :
				           ((chance == 2) ? GF_COLD :
				            ((chance == 3) ? GF_ACID :
				             ((chance == 4) ? GF_POIS : GF_FIRE)))),
				          dir, 250, 2);
				o_ptr->timeout = rand_int(225) + 225;
				break;
			}

			case SV_DRAGON_BRONZE:
			{
				sound(MSG_BR_CONF);
#ifdef JP
				msg_print("���Ȃ��͍����̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe confusion.");
#endif /* JP */
				fire_ball(GF_CONFUSION, dir, 120, 2);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

			case SV_DRAGON_GOLD:
			{
				sound(MSG_BR_SOUND);
#ifdef JP
				msg_print("���Ȃ��͍����̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe sound.");
#endif /* JP */
				fire_ball(GF_SOUND, dir, 130, 2);
				o_ptr->timeout = rand_int(450) + 450;
				break;
			}

			case SV_DRAGON_CHAOS:
			{
				chance = rand_int(2);
				sound(((chance == 1 ? MSG_BR_CHAOS : MSG_BR_DISENCHANT)));
#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B",
				           ((chance == 1 ? "�J�I�X" : "��")));
#else /* JP */
				msg_format("You breathe %s.",
				           ((chance == 1 ? "chaos" : "disenchantment")));
#endif /* JP */
				fire_ball((chance == 1 ? GF_CHAOS : GF_DISENCHANT),
				          dir, 220, 2);
				o_ptr->timeout = rand_int(300) + 300;
				break;
			}

			case SV_DRAGON_LAW:
			{
				chance = rand_int(2);
				sound(((chance == 1 ? MSG_BR_SOUND : MSG_BR_SHARDS)));
#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B",
				           ((chance == 1 ? "����" : "�j��")));
#else /* JP */
				msg_format("You breathe %s.",
				           ((chance == 1 ? "sound" : "shards")));
#endif /* JP */
				fire_ball((chance == 1 ? GF_SOUND : GF_SHARD),
				          dir, 230, 2);
				o_ptr->timeout = rand_int(300) + 300;
				break;
			}

			case SV_DRAGON_BALANCE:
			{
				chance = rand_int(4);
#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B",
				           ((chance == 1) ? "�J�I�X" :
				            ((chance == 2) ? "��" :
				             ((chance == 3) ? "����" : "�j��"))));
#else /* JP */
				msg_format("You breathe %s.",
				           ((chance == 1) ? "chaos" :
				            ((chance == 2) ? "disenchantment" :
				             ((chance == 3) ? "sound" : "shards"))));
#endif /* JP */
				fire_ball(((chance == 1) ? GF_CHAOS :
				           ((chance == 2) ? GF_DISENCHANT :
				            ((chance == 3) ? GF_SOUND : GF_SHARD))),
				          dir, 250, 2);
				o_ptr->timeout = rand_int(300) + 300;
				break;
			}

			case SV_DRAGON_SHINING:
			{
				chance = rand_int(2);
				sound(((chance == 0 ? MSG_BR_LIGHT : MSG_BR_DARK)));
#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B",
				           ((chance == 0 ? "�M��" : "�Í�")));
#else /* JP */
				msg_format("You breathe %s.",
				           ((chance == 0 ? "light" : "darkness")));
#endif /* JP */
				fire_ball((chance == 0 ? GF_LITE : GF_DARK), dir, 200, 2);
				o_ptr->timeout = rand_int(300) + 300;
				break;
			}

			case SV_DRAGON_POWER:
			{
				sound(MSG_BR_ELEMENTS);
#ifdef JP
				msg_print("���Ȃ��̓G�������g�̃u���X��f�����B");
#else /* JP */
				msg_print("You breathe the elements.");
#endif /* JP */
				fire_ball(GF_MISSILE, dir, 300, 2);
				o_ptr->timeout = rand_int(300) + 300;
				break;
			}
		}

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return FALSE;
	}

	/* Hack -- some Rings can be activated for double resist and element ball */
	if (o_ptr->tval == TV_RING)
	{
		/* Get a direction for firing (or abort) */
		if (!get_aim_dir(&dir)) return FALSE;

		/* Branch on the sub-type */
		switch (o_ptr->sval)
		{
			case SV_RING_ACID:
			{
				fire_ball(GF_ACID, dir, 70, 2);
				set_oppose_acid(p_ptr->oppose_acid + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_FLAMES:
			{
				fire_ball(GF_FIRE, dir, 80, 2);
				set_oppose_fire(p_ptr->oppose_fire + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_ICE:
			{
				fire_ball(GF_COLD, dir, 75, 2);
				set_oppose_cold(p_ptr->oppose_cold + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_LIGHTNING:
			{
				fire_ball(GF_ELEC, dir, 85, 2);
				set_oppose_elec(p_ptr->oppose_elec + randint(20) + 20);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}
		}

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);

		/* Success */
		return FALSE;
	}

	/* Mistake */
#ifdef JP
	msg_print("�����ƁA���̃A�C�e���͎n���ł��Ȃ��B");
#else /* JP */
	msg_print("Oops.  That object cannot be activated.");
#endif /* JP */

	/* Not used up */
	return (FALSE);
}


bool use_object(object_type *o_ptr, bool *ident)
{
	bool used;

	/* Analyze the object */
	switch (o_ptr->tval)
	{
		case TV_FOOD:
		{
			used = eat_food(o_ptr, ident);
			break;
		}

		case TV_POTION:
		{
			used = quaff_potion(o_ptr, ident);
			break;
		}

		case TV_SCROLL:
		{
			used = read_scroll(o_ptr, ident);
			break;
		}

		case TV_STAFF:
		{
			used = use_staff(o_ptr, ident);
			break;
		}

		case TV_WAND:
		{
			used = aim_wand(o_ptr, ident);
			break;
		}

		case TV_ROD:
		{
			used = zap_rod(o_ptr, ident);
			break;
		}

		default:
		{
			used = activate_object(o_ptr, ident);
			break;
		}
	}

	return (used);
}


static cptr act_description[ACT_MAX] =
{
#ifdef JP
	"�C���~�l�[�V����",
	"���@�̒n�}",
	"�痢��",
	"�Ύ׈����E",
	"�׈��ގU (x5)",
	"�̗͉� (500)",
	"�̗͉� (1000)",
	"���̎��� (4d8)",
	"�X�s�[�h (20+d20�^�[��)",
	"�X�s�[�h (75+d75�^�[��)",
	"�t�@�C�A�E�{���g (9d8)",
	"�t�@�C�A�E�{�[�� (72)",
	"����t�@�C�A�E�{�[�� (120)",
	"�A�C�X�E�{���g (6d8)",
	"�A�C�X�E�{�[�� (48)",
	"�A�C�X�E�{�[�� (100)",
	"�A�C�X�E�{���g (12d8)",
	"����A�C�X�E�{�[�� (200)",
	"�A�V�b�h�E�{���g (5d8)",
	"���͏[�U(I)",
	"�X���[�v(II)",
	"�T���_�[�E�{���g (4d8)",
	"����T���_�[�E�{�[�� (250)",
	"�Ǖ�",
	"���ӒǕ�",
	"�Ӓ�",
	"�����͋z�� (90)",
	"�����͋z�� (120)",
	"�M�������",
	"�X�^�[�E�{�[�� (150)",
	"��b��/�j��/�S�ϐ�",
	"�t�F�C�Y�E�h�A",
	"�h�A/�g���b�v����",
	"�S���m",
	"�S�ϐ� (20+d20 turns)",
	"�e���|�[�g",
	"�����͕���",
	"�}�W�b�N�E�~�T�C�� (2d6)",
	"���@�̖� (150)",
	"���|����/�ŏ���",
	"���L�_ (12)",
	"��Ηn��",
	"�e���|�[�g�E�A�E�F�C",
	"�A�҂̏�",
	"�p�j�b�N�E�����X�^�[",
	"����",
	"�n��̃t�@�C�A�E�{���g",
	"�X�^�[���C�g (10d8)",
	"���̖͂� (12d8)",
	"��b�� (50+d50�^�[��)"
#else /* JP */
	"illumination",
	"magic mapping",
	"clairvoyance",
	"protection from evil",
	"dispel evil (x5)",
	"heal (500)",
	"heal (1000)",
	"cure wounds (4d8)",
	"haste self (20+d20 turns)",
	"haste self (75+d75 turns)",
	"fire bolt (9d8)",
	"fire ball (72)",
	"large fire ball (120)",
	"frost bolt (6d8)",
	"frost ball (48)",
	"frost ball (100)",
	"frost bolt (12d8)",
	"large frost ball (200)",
	"acid bolt (5d8)",
	"recharge item I",
	"sleep II",
	"lightning bolt (4d8)",
	"large lightning ball (250)",
	"banishment",
	"mass banishment",
	"identify",
	"drain life (90)",
	"drain life (120)",
	"bizarre things",
	"star ball (150)",
	"berserk rage, bless, and resistance",
	"phase door",
	"door and trap destruction",
	"detection",
	"resistance (20+d20 turns)",
	"teleport",
	"restore life levels",
	"magic missile (2d6)",
	"a magical arrow (150)",
	"remove fear and cure poison",
	"stinking cloud (12)",
	"stone to mud",
	"teleport away",
	"word of recall",
	"confuse monster",
	"probing",
	"fire branding of bolts",
	"starlight (10d8)",
	"mana bolt (12d8)",
	"berserk rage (50+d50 turns)"
#endif /* JP */
};



/*
 * Determine the "Activation" (if any) for an artifact
 */
void describe_item_activation(const object_type *o_ptr)
{
	u32b f1, f2, f3;

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* Require activation ability */
	if (!(f3 & TR3_ACTIVATE)) return;

	/* Artifact activations */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		/* Paranoia */
		if (a_ptr->activation >= ACT_MAX) return;

		/* Some artifacts can be activated */
		text_out(act_description[a_ptr->activation]);

		/* Output the number of turns */
		if (a_ptr->time && a_ptr->randtime)
#ifdef JP
			text_out(format(" : %d+d%d �^�[����", a_ptr->time, a_ptr->randtime));
#else /* JP */
			text_out(format(" every %d+d%d turns", a_ptr->time, a_ptr->randtime));
#endif /* JP */
		else if (a_ptr->time)
#ifdef JP
			text_out(format(" : %d �^�[����", a_ptr->time));
#else /* JP */
			text_out(format(" every %d turns", a_ptr->time));
#endif /* JP */
		else if (a_ptr->randtime)
#ifdef JP
			text_out(format(" : d%d �^�[����", a_ptr->randtime));
#else /* JP */
			text_out(format(" every d%d turns", a_ptr->randtime));
#endif /* JP */

		return;
	}

	/* Ring activations */
	if (o_ptr->tval == TV_RING)
	{
		/* Branch on the sub-type */
		switch (o_ptr->sval)
		{
			case SV_RING_ACID:
			{
#ifdef JP
				text_out("�ώ_ (20+d20�^�[��), �A�V�b�h�E�{�[�� (70) : 50+d50 �^�[����");
#else /* JP */
				text_out("acid resistance (20+d20 turns) and acid ball (70) every 50+d50 turns");
#endif /* JP */
				break;
			}
			case SV_RING_FLAMES:
			{
#ifdef JP
				text_out("�ω� (20+d20�^�[��), �t�@�C�A�E�{�[�� (80) : 50+d50 �^�[����");
#else /* JP */
				text_out("fire resistance (20+d20 turns) and fire ball (80) every 50+d50 turns");
#endif /* JP */
				break;
			}
			case SV_RING_ICE:
			{
#ifdef JP
				text_out("�ϗ� (20+d20�^�[��),  �A�C�X�E�{�[�� (75) : 50+d50 �^�[����");
#else /* JP */
				text_out("cold resistance (20+d20 turns) and cold ball (75) every 50+d50 turns");
#endif /* JP */
				break;
			}

			case SV_RING_LIGHTNING:
			{
#ifdef JP
				text_out("�ϓd (20+d20�^�[��),  �T���_�[�E�{�[�� (85) : 50+d50 �^�[����");
#else /* JP */
				text_out("electricity resistance (20+d20 turns) and electricity ball (85) every 50+d50 turns");
#endif /* JP */
				break;
			}
		}

		return;
	}

	/* Require dragon scale mail */
	if (o_ptr->tval != TV_DRAG_ARMOR) return;

	/* Branch on the sub-type */
	switch (o_ptr->sval)
	{
		case SV_DRAGON_BLUE:
		{
#ifdef JP
			text_out("��Ȃ̃u���X(100) : 450+d450 �^�[����");
#else /* JP */
			text_out("breathe lightning (100) every 450+d450 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_WHITE:
		{
#ifdef JP
			text_out("��C�̃u���X(110) : 450+d450 �^�[����");
#else /* JP */
			text_out("breathe frost (110) every 450+d450 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_BLACK:
		{
#ifdef JP
			text_out("�_�̃u���X(130) : 450+d450 �^�[����");
#else /* JP */
			text_out("breathe acid (130) every 450+d450 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_GREEN:
		{
#ifdef JP
			text_out("�ŃK�X�̃u���X(150) : 450+d450 �^�[����");
#else /* JP */
			text_out("breathe poison gas (150) every 450+d450 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_RED:
		{
#ifdef JP
			text_out("�Ή��̃u���X(200) : 450+d450 �^�[����");
#else /* JP */
			text_out("breathe fire (200) every 450+d450 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_MULTIHUED:
		{
#ifdef JP
			text_out("���F�̃u���X(250) : 225+d225 �^�[����");
#else /* JP */
			text_out("breathe multi-hued (250) every 225+d225 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_BRONZE:
		{
#ifdef JP
			text_out("�����̃u���X(120) : 450+d450 �^�[����");
#else /* JP */
			text_out("breathe confusion (120) every 450+d450 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_GOLD:
		{
#ifdef JP
			text_out("�����̃u���X(130) : 450+d450 �^�[����");
#else /* JP */
			text_out("breathe sound (130) every 450+d450 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_CHAOS:
		{
#ifdef JP
			text_out("�J�I�X/�򉻂̃u���X(220) : 300+d300 �^�[����");
#else /* JP */
			text_out("breathe chaos/disenchant (220) every 300+d300 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_LAW:
		{
#ifdef JP
			text_out("����/�j�Ђ̃u���X(230) : 300+d300 �^�[����");
#else /* JP */
			text_out("breathe sound/shards (230) every 300+d300 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_BALANCE:
		{
#ifdef JP
			text_out("�o�����X�̃u���X(250) : 300+d300 �^�[����");
#else /* JP */
			text_out("breathe balance (250) every 300+d300 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_SHINING:
		{
#ifdef JP
			text_out("�M��/�Í��̃u���X(200) : 300+d300 �^�[����");
#else /* JP */
			text_out("breathe light/darkness (200) every 300+d300 turns");
#endif /* JP */
			break;
		}
		case SV_DRAGON_POWER:
		{
#ifdef JP
			text_out("�G�������g�̃u���X(300) : 300+d300 �^�[����");
#else /* JP */
			text_out("breathe the elements (300) every 300+d300 turns");
#endif /* JP */
			break;
		}
	}
}

#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif /* USE_SCRIPT */
