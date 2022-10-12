/* File: monster1.c */

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
 * 2.8.3      �Ή�        : FIRST
 *
 * ���{��ŋ@�\�ǉ� : �p���؂�ւ��@�\
 */

#include "angband.h"


/*
 * Pronoun arrays, by gender.
 */
static cptr wd_he[3] =
#ifdef JP
{ "����", "��", "�ޏ�" };
#else /* JP */
{ "it", "he", "she" };
#endif /* JP */
static cptr wd_his[3] =
#ifdef JP
{ "�����", "�ނ�", "�ޏ���" };
#else /* JP */
{ "its", "his", "her" };
#endif /* JP */


/*
 * Pluralizer.  Args(count, singular, plural)
 */
#define plural(c,s,p) \
	(((c) == 1) ? (s) : (p))






/*
 * Determine if the "armor" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_armour(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b kills = l_ptr->tkills;

	/* Normal monsters */
	if (kills > 304 / (4 + level)) return (TRUE);

	/* Skip non-uniques */
	if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

	/* Unique monsters */
	if (kills > 304 / (38 + (5 * level) / 4)) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Determine if the "damage" of the given attack is known
 * the higher the level of the monster, the fewer the attacks you need,
 * the more damage an attack does, the more attacks you need
 */
static bool know_damage(int r_idx, const monster_lore *l_ptr, int i)
{
	const monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b a = l_ptr->blows[i];

	s32b d1 = r_ptr->blow[i].d_dice;
	s32b d2 = r_ptr->blow[i].d_side;

	s32b d = d1 * d2;

	/* Normal monsters */
	if ((4 + level) * a > 80 * d) return (TRUE);

	/* Skip non-uniques */
	if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

	/* Unique monsters */
	if ((4 + level) * (2 * a) > 80 * d) return (TRUE);

	/* Assume false */
	return (FALSE);
}


static void describe_monster_desc(int r_idx)
{
	const monster_race *r_ptr = &r_info[r_idx];
	char buf[2048];

	/* Simple method */
	my_strcpy(buf, r_text + r_ptr->text, sizeof(buf));

	/* Dump it */
	text_out(buf);
	text_out("\n");
}


static void describe_monster_spells(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];
	int m, n;
	int msex = 0;
	bool breath = FALSE;
	bool magic = FALSE;
	int vn;
	cptr vp[64];
#ifdef JP
	char jverb_buf[64];
#endif /* JP */


	/* Extract a gender (if applicable) */
	if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;

	/* Collect innate attacks */
	vn = 0;
#ifdef JP
	if (l_ptr->flags4 & RF4_SHRIEK)  vp[vn++] = "�ߖŏ��������߂�";
	if (l_ptr->flags4 & RF4_XXX2)    vp[vn++] = "����������";
	if (l_ptr->flags4 & RF4_XXX3)    vp[vn++] = "����������";
	if (l_ptr->flags4 & RF4_XXX4)    vp[vn++] = "����������";
	if (l_ptr->flags4 & RF4_ARROW_1) vp[vn++] = "�������";
	if (l_ptr->flags4 & RF4_ARROW_2) vp[vn++] = "����������";
	if (l_ptr->flags4 & RF4_ARROW_3) vp[vn++] = "�ˌ�������";
	if (l_ptr->flags4 & RF4_ARROW_4) vp[vn++] = "����ˌ�������";
	if (l_ptr->flags4 & RF4_BOULDER) vp[vn++] = "�΂𓊂���";
#else /* JP */
	if (l_ptr->flags4 & RF4_SHRIEK)  vp[vn++] = "shriek for help";
	if (l_ptr->flags4 & RF4_XXX2)    vp[vn++] = "do something";
	if (l_ptr->flags4 & RF4_XXX3)    vp[vn++] = "do something";
	if (l_ptr->flags4 & RF4_XXX4)    vp[vn++] = "do something";
	if (l_ptr->flags4 & RF4_ARROW_1) vp[vn++] = "fire an arrow";
	if (l_ptr->flags4 & RF4_ARROW_2) vp[vn++] = "fire arrows";
	if (l_ptr->flags4 & RF4_ARROW_3) vp[vn++] = "fire a missile";
	if (l_ptr->flags4 & RF4_ARROW_4) vp[vn++] = "fire missiles";
	if (l_ptr->flags4 & RF4_BOULDER) vp[vn++] = "throw boulders";
#endif /* JP */

	/* Describe innate attacks */
	if (vn)
	{
		/* Intro */
#ifdef JP
		text_out(format("%^s��", wd_he[msex]));
#else /* JP */
		text_out(format("%^s", wd_he[msex]));
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
#ifdef JP
			if(n!=vn-1)
			{
				jverb(vp[n],jverb_buf,JVERB_AND);
				text_out_c(TERM_L_RED, jverb_buf);
				text_out("����A");
			}
			else text_out_c(TERM_L_RED, vp[n]);

#else /* JP */
			/* Intro */
			if (n == 0) text_out(" may ");
			else if (n < vn-1) text_out(", ");
			else text_out(" or ");

			/* Dump */
			text_out_c(TERM_L_RED, vp[n]);
#endif /* JP */
		}

		/* End */
#ifdef JP
		text_out("���Ƃ�����B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}


	/* Collect breaths */
	vn = 0;
#ifdef JP
	if (l_ptr->flags4 & RF4_BR_ACID)		vp[vn++] = "�_";
	if (l_ptr->flags4 & RF4_BR_ELEC)		vp[vn++] = "���";
	if (l_ptr->flags4 & RF4_BR_FIRE)		vp[vn++] = "�Ή�";
	if (l_ptr->flags4 & RF4_BR_COLD)		vp[vn++] = "��C";
	if (l_ptr->flags4 & RF4_BR_POIS)		vp[vn++] = "��";
	if (l_ptr->flags4 & RF4_BR_NETH)		vp[vn++] = "�n��";
	if (l_ptr->flags4 & RF4_BR_LITE)		vp[vn++] = "�M��";
	if (l_ptr->flags4 & RF4_BR_DARK)		vp[vn++] = "�Í�";
	if (l_ptr->flags4 & RF4_BR_CONF)		vp[vn++] = "����";
	if (l_ptr->flags4 & RF4_BR_SOUN)		vp[vn++] = "����";
	if (l_ptr->flags4 & RF4_BR_CHAO)		vp[vn++] = "�J�I�X";
	if (l_ptr->flags4 & RF4_BR_DISE)		vp[vn++] = "��";
	if (l_ptr->flags4 & RF4_BR_NEXU)		vp[vn++] = "���ʍ���";
	if (l_ptr->flags4 & RF4_BR_TIME)		vp[vn++] = "���ԋt�]";
	if (l_ptr->flags4 & RF4_BR_INER)		vp[vn++] = "�x��";
	if (l_ptr->flags4 & RF4_BR_GRAV)		vp[vn++] = "�d��";
	if (l_ptr->flags4 & RF4_BR_SHAR)		vp[vn++] = "�j��";
	if (l_ptr->flags4 & RF4_BR_PLAS)		vp[vn++] = "�v���Y�}";
	if (l_ptr->flags4 & RF4_BR_WALL)		vp[vn++] = "�t�H�[�X";
	if (l_ptr->flags4 & RF4_BR_MANA)		vp[vn++] = "����";
	if (l_ptr->flags4 & RF4_XXX5)			vp[vn++] = "����";
	if (l_ptr->flags4 & RF4_XXX6)			vp[vn++] = "����";
	if (l_ptr->flags4 & RF4_XXX7)			vp[vn++] = "����";
#else /* JP */
	if (l_ptr->flags4 & RF4_BR_ACID)		vp[vn++] = "acid";
	if (l_ptr->flags4 & RF4_BR_ELEC)		vp[vn++] = "lightning";
	if (l_ptr->flags4 & RF4_BR_FIRE)		vp[vn++] = "fire";
	if (l_ptr->flags4 & RF4_BR_COLD)		vp[vn++] = "frost";
	if (l_ptr->flags4 & RF4_BR_POIS)		vp[vn++] = "poison";
	if (l_ptr->flags4 & RF4_BR_NETH)		vp[vn++] = "nether";
	if (l_ptr->flags4 & RF4_BR_LITE)		vp[vn++] = "light";
	if (l_ptr->flags4 & RF4_BR_DARK)		vp[vn++] = "darkness";
	if (l_ptr->flags4 & RF4_BR_CONF)		vp[vn++] = "confusion";
	if (l_ptr->flags4 & RF4_BR_SOUN)		vp[vn++] = "sound";
	if (l_ptr->flags4 & RF4_BR_CHAO)		vp[vn++] = "chaos";
	if (l_ptr->flags4 & RF4_BR_DISE)		vp[vn++] = "disenchantment";
	if (l_ptr->flags4 & RF4_BR_NEXU)		vp[vn++] = "nexus";
	if (l_ptr->flags4 & RF4_BR_TIME)		vp[vn++] = "time";
	if (l_ptr->flags4 & RF4_BR_INER)		vp[vn++] = "inertia";
	if (l_ptr->flags4 & RF4_BR_GRAV)		vp[vn++] = "gravity";
	if (l_ptr->flags4 & RF4_BR_SHAR)		vp[vn++] = "shards";
	if (l_ptr->flags4 & RF4_BR_PLAS)		vp[vn++] = "plasma";
	if (l_ptr->flags4 & RF4_BR_WALL)		vp[vn++] = "force";
	if (l_ptr->flags4 & RF4_BR_MANA)		vp[vn++] = "mana";
	if (l_ptr->flags4 & RF4_XXX5)		vp[vn++] = "something";
	if (l_ptr->flags4 & RF4_XXX6)		vp[vn++] = "something";
	if (l_ptr->flags4 & RF4_XXX7)		vp[vn++] = "something";
#endif /* JP */

	/* Describe breaths */
	if (vn)
	{
		/* Note breath */
		breath = TRUE;

		/* Intro */
#ifdef JP
		text_out(format("%^s��", wd_he[msex]));
#else /* JP */
		text_out(format("%^s", wd_he[msex]));
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) text_out("��");
#else /* JP */
			if (n == 0) text_out(" may breathe ");
			else if (n < vn-1) text_out(", ");
			else text_out(" or ");
#endif /* JP */

			/* Dump */
			text_out_c(TERM_L_RED, vp[n]);
		}
#ifdef JP
		text_out("�̃u���X��f�����Ƃ�����");
#endif /* JP */
	}


	/* Collect spells */
	vn = 0;
#ifdef JP
	if (l_ptr->flags5 & RF5_BA_ACID)     vp[vn++] = "�A�V�b�h�E�{�[��";
	if (l_ptr->flags5 & RF5_BA_ELEC)     vp[vn++] = "�T���_�[�E�{�[��";
	if (l_ptr->flags5 & RF5_BA_FIRE)     vp[vn++] = "�t�@�C�A�E�{�[��";
	if (l_ptr->flags5 & RF5_BA_COLD)     vp[vn++] = "�A�C�X�E�{�[��";
	if (l_ptr->flags5 & RF5_BA_POIS)     vp[vn++] = "���L�_";
	if (l_ptr->flags5 & RF5_BA_NETH)     vp[vn++] = "�n����";
	if (l_ptr->flags5 & RF5_BA_WATE)     vp[vn++] = "�E�H�[�^�[�E�{�[��";
	if (l_ptr->flags5 & RF5_BA_MANA)     vp[vn++] = "���̗͂�";
	if (l_ptr->flags5 & RF5_BA_DARK)     vp[vn++] = "�Í��̗�";
	if (l_ptr->flags5 & RF5_DRAIN_MANA)  vp[vn++] = "���͋z��";
	if (l_ptr->flags5 & RF5_MIND_BLAST)  vp[vn++] = "���_�U��";
	if (l_ptr->flags5 & RF5_BRAIN_SMASH) vp[vn++] = "�]�U��";
	if (l_ptr->flags5 & RF5_CAUSE_1)     vp[vn++] = "�y��";
	if (l_ptr->flags5 & RF5_CAUSE_2)     vp[vn++] = "�d��";
	if (l_ptr->flags5 & RF5_CAUSE_3)     vp[vn++] = "�v����";
	if (l_ptr->flags5 & RF5_CAUSE_4)     vp[vn++] = "�m����";
	if (l_ptr->flags5 & RF5_BO_ACID)     vp[vn++] = "�A�V�b�h�E�{���g";
	if (l_ptr->flags5 & RF5_BO_ELEC)     vp[vn++] = "�T���_�[�E�{���g";
	if (l_ptr->flags5 & RF5_BO_FIRE)     vp[vn++] = "�t�@�C�A�E�{���g";
	if (l_ptr->flags5 & RF5_BO_COLD)     vp[vn++] = "�A�C�X�E�{���g";
	if (l_ptr->flags5 & RF5_BO_POIS)     vp[vn++] = "�|�C�Y���E�{���g";
	if (l_ptr->flags5 & RF5_BO_NETH)     vp[vn++] = "�n���̖�";
	if (l_ptr->flags5 & RF5_BO_WATE)     vp[vn++] = "�E�H�[�^�[�E�{���g";
	if (l_ptr->flags5 & RF5_BO_MANA)     vp[vn++] = "���̖͂�";
	if (l_ptr->flags5 & RF5_BO_PLAS)     vp[vn++] = "�v���Y�}�E�{���g";
	if (l_ptr->flags5 & RF5_BO_ICEE)     vp[vn++] = "�Ɋ��̖�";
	if (l_ptr->flags5 & RF5_MISSILE)     vp[vn++] = "�}�W�b�N�~�T�C��";
	if (l_ptr->flags5 & RF5_SCARE)       vp[vn++] = "���|";
	if (l_ptr->flags5 & RF5_BLIND)       vp[vn++] = "�ڂ���܂�";
	if (l_ptr->flags5 & RF5_CONF)        vp[vn++] = "����";
	if (l_ptr->flags5 & RF5_SLOW)        vp[vn++] = "����";
	if (l_ptr->flags5 & RF5_HOLD)        vp[vn++] = "���";
	if (l_ptr->flags6 & RF6_HASTE)       vp[vn++] = "����";
	if (l_ptr->flags6 & RF6_XXX1)        vp[vn++] = "����";
	if (l_ptr->flags6 & RF6_HEAL)        vp[vn++] = "����";
	if (l_ptr->flags6 & RF6_XXX2)        vp[vn++] = "����";
	if (l_ptr->flags6 & RF6_BLINK)       vp[vn++] = "�V���[�g�E�e���|�[�g";
	if (l_ptr->flags6 & RF6_TPORT)       vp[vn++] = "�e���|�[�g";
	if (l_ptr->flags6 & RF6_XXX3)        vp[vn++] = "����";
	if (l_ptr->flags6 & RF6_XXX4)        vp[vn++] = "����";
	if (l_ptr->flags6 & RF6_TELE_TO)     vp[vn++] = "�e���|�[�g�E�o�b�N";
	if (l_ptr->flags6 & RF6_TELE_AWAY)   vp[vn++] = "�e���|�[�g�E�A�E�F�C";
	if (l_ptr->flags6 & RF6_TELE_LEVEL)  vp[vn++] = "���x���E�e���|�[�g";
	if (l_ptr->flags6 & RF6_XXX5)        vp[vn++] = "����";
	if (l_ptr->flags6 & RF6_DARKNESS)    vp[vn++] = "�È�";
	if (l_ptr->flags6 & RF6_TRAPS)       vp[vn++] = "�g���b�v�n��";
	if (l_ptr->flags6 & RF6_FORGET)      vp[vn++] = "�L������";
	if (l_ptr->flags6 & RF6_XXX6)        vp[vn++] = "����������";
	if (l_ptr->flags6 & RF6_S_KIN)       vp[vn++] = "�~������";
	if (l_ptr->flags6 & RF6_S_MONSTER)   vp[vn++] = "�����X�^�[��̏���";
	if (l_ptr->flags6 & RF6_S_MONSTERS)  vp[vn++] = "�����X�^�[��������";
	if (l_ptr->flags6 & RF6_S_ANIMAL)    vp[vn++] = "��������";
	if (l_ptr->flags6 & RF6_S_SPIDER)    vp[vn++] = "�N������";
	if (l_ptr->flags6 & RF6_S_HOUND)     vp[vn++] = "�n�E���h����";
	if (l_ptr->flags6 & RF6_S_HYDRA)     vp[vn++] = "�q�h������";
	if (l_ptr->flags6 & RF6_S_ANGEL)     vp[vn++] = "�V�g��̏���";
	if (l_ptr->flags6 & RF6_S_DEMON)     vp[vn++] = "�f�[������̏���";
	if (l_ptr->flags6 & RF6_S_UNDEAD)    vp[vn++] = "�A���f�b�h��̏���";
	if (l_ptr->flags6 & RF6_S_DRAGON)    vp[vn++] = "�h���S����̏���";
	if (l_ptr->flags6 & RF6_S_HI_UNDEAD) vp[vn++] = "���͂ȃA���f�b�h����";
	if (l_ptr->flags6 & RF6_S_HI_DRAGON) vp[vn++] = "�Ñ�h���S������";
	if (l_ptr->flags6 & RF6_S_HI_DEMON)  vp[vn++] = "���͂ȃf�[��������";
	if (l_ptr->flags6 & RF6_S_WRAITH)    vp[vn++] = "�w�ւ̗H�S����";
	if (l_ptr->flags6 & RF6_S_UNIQUE)    vp[vn++] = "���j�[�N�E�����X�^�[����";
#else /* JP */
	if (l_ptr->flags5 & RF5_BA_ACID)     vp[vn++] = "produce acid balls";
	if (l_ptr->flags5 & RF5_BA_ELEC)     vp[vn++] = "produce lightning balls";
	if (l_ptr->flags5 & RF5_BA_FIRE)     vp[vn++] = "produce fire balls";
	if (l_ptr->flags5 & RF5_BA_COLD)     vp[vn++] = "produce frost balls";
	if (l_ptr->flags5 & RF5_BA_POIS)     vp[vn++] = "produce poison balls";
	if (l_ptr->flags5 & RF5_BA_NETH)     vp[vn++] = "produce nether balls";
	if (l_ptr->flags5 & RF5_BA_WATE)     vp[vn++] = "produce water balls";
	if (l_ptr->flags5 & RF5_BA_MANA)     vp[vn++] = "invoke mana storms";
	if (l_ptr->flags5 & RF5_BA_DARK)     vp[vn++] = "invoke darkness storms";
	if (l_ptr->flags5 & RF5_DRAIN_MANA)  vp[vn++] = "drain mana";
	if (l_ptr->flags5 & RF5_MIND_BLAST)  vp[vn++] = "cause mind blasting";
	if (l_ptr->flags5 & RF5_BRAIN_SMASH) vp[vn++] = "cause brain smashing";
	if (l_ptr->flags5 & RF5_CAUSE_1)     vp[vn++] = "cause light wounds";
	if (l_ptr->flags5 & RF5_CAUSE_2)     vp[vn++] = "cause serious wounds";
	if (l_ptr->flags5 & RF5_CAUSE_3)     vp[vn++] = "cause critical wounds";
	if (l_ptr->flags5 & RF5_CAUSE_4)     vp[vn++] = "cause mortal wounds";
	if (l_ptr->flags5 & RF5_BO_ACID)     vp[vn++] = "produce acid bolts";
	if (l_ptr->flags5 & RF5_BO_ELEC)     vp[vn++] = "produce lightning bolts";
	if (l_ptr->flags5 & RF5_BO_FIRE)     vp[vn++] = "produce fire bolts";
	if (l_ptr->flags5 & RF5_BO_COLD)     vp[vn++] = "produce frost bolts";
	if (l_ptr->flags5 & RF5_BO_POIS)     vp[vn++] = "produce poison bolts";
	if (l_ptr->flags5 & RF5_BO_NETH)     vp[vn++] = "produce nether bolts";
	if (l_ptr->flags5 & RF5_BO_WATE)     vp[vn++] = "produce water bolts";
	if (l_ptr->flags5 & RF5_BO_MANA)     vp[vn++] = "produce mana bolts";
	if (l_ptr->flags5 & RF5_BO_PLAS)     vp[vn++] = "produce plasma bolts";
	if (l_ptr->flags5 & RF5_BO_ICEE)     vp[vn++] = "produce ice bolts";
	if (l_ptr->flags5 & RF5_MISSILE)     vp[vn++] = "produce magic missiles";
	if (l_ptr->flags5 & RF5_SCARE)       vp[vn++] = "terrify";
	if (l_ptr->flags5 & RF5_BLIND)       vp[vn++] = "blind";
	if (l_ptr->flags5 & RF5_CONF)        vp[vn++] = "confuse";
	if (l_ptr->flags5 & RF5_SLOW)        vp[vn++] = "slow";
	if (l_ptr->flags5 & RF5_HOLD)        vp[vn++] = "paralyze";
	if (l_ptr->flags6 & RF6_HASTE)       vp[vn++] = "haste-self";
	if (l_ptr->flags6 & RF6_XXX1)        vp[vn++] = "do something";
	if (l_ptr->flags6 & RF6_HEAL)        vp[vn++] = "heal-self";
	if (l_ptr->flags6 & RF6_XXX2)        vp[vn++] = "do something";
	if (l_ptr->flags6 & RF6_BLINK)       vp[vn++] = "blink-self";
	if (l_ptr->flags6 & RF6_TPORT)       vp[vn++] = "teleport-self";
	if (l_ptr->flags6 & RF6_XXX3)        vp[vn++] = "do something";
	if (l_ptr->flags6 & RF6_XXX4)        vp[vn++] = "do something";
	if (l_ptr->flags6 & RF6_TELE_TO)     vp[vn++] = "teleport to";
	if (l_ptr->flags6 & RF6_TELE_AWAY)   vp[vn++] = "teleport away";
	if (l_ptr->flags6 & RF6_TELE_LEVEL)  vp[vn++] = "teleport level";
	if (l_ptr->flags6 & RF6_XXX5)        vp[vn++] = "do something";
	if (l_ptr->flags6 & RF6_DARKNESS)    vp[vn++] = "create darkness";
	if (l_ptr->flags6 & RF6_TRAPS)       vp[vn++] = "create traps";
	if (l_ptr->flags6 & RF6_FORGET)      vp[vn++] = "cause amnesia";
	if (l_ptr->flags6 & RF6_XXX6)        vp[vn++] = "do something";
	if (l_ptr->flags6 & RF6_S_KIN)       vp[vn++] = "summon similar monsters";
	if (l_ptr->flags6 & RF6_S_MONSTER)   vp[vn++] = "summon a monster";
	if (l_ptr->flags6 & RF6_S_MONSTERS)  vp[vn++] = "summon monsters";
	if (l_ptr->flags6 & RF6_S_ANIMAL)    vp[vn++] = "summon animals";
	if (l_ptr->flags6 & RF6_S_SPIDER)    vp[vn++] = "summon spiders";
	if (l_ptr->flags6 & RF6_S_HOUND)     vp[vn++] = "summon hounds";
	if (l_ptr->flags6 & RF6_S_HYDRA)     vp[vn++] = "summon hydras";
	if (l_ptr->flags6 & RF6_S_ANGEL)     vp[vn++] = "summon an angel";
	if (l_ptr->flags6 & RF6_S_DEMON)     vp[vn++] = "summon a demon";
	if (l_ptr->flags6 & RF6_S_UNDEAD)    vp[vn++] = "summon an undead";
	if (l_ptr->flags6 & RF6_S_DRAGON)    vp[vn++] = "summon a dragon";
	if (l_ptr->flags6 & RF6_S_HI_UNDEAD) vp[vn++] = "summon Greater Undead";
	if (l_ptr->flags6 & RF6_S_HI_DRAGON) vp[vn++] = "summon Ancient Dragons";
	if (l_ptr->flags6 & RF6_S_HI_DEMON)  vp[vn++] = "summon Greater Demons";
	if (l_ptr->flags6 & RF6_S_WRAITH)    vp[vn++] = "summon Ring Wraiths";
	if (l_ptr->flags6 & RF6_S_UNIQUE)    vp[vn++] = "summon Unique Monsters";
#endif /* JP */

	/* Describe spells */
	if (vn)
	{
		/* Note magic */
		magic = TRUE;

		/* Intro */
		if (breath)
		{
#ifdef JP
			text_out("�A�Ȃ�����");
#else /* JP */
			text_out(", and is also");
#endif /* JP */
		}
		else
		{
#ifdef JP
			text_out(format("%^s��", wd_he[msex]));
#else /* JP */
			text_out(format("%^s is", wd_he[msex]));
#endif /* JP */
		}

#ifdef JP
		/* Adverb */
		if (l_ptr->flags2 & RF2_SMART) text_out_c(TERM_ORANGE, "�I�m��");

		/* Verb Phrase */
		text_out("���@���g�����Ƃ��ł��A");
#else /* JP */
		/* Verb Phrase */
		text_out(" magical, casting spells");

		/* Adverb */
		if (l_ptr->flags2 & RF2_SMART) text_out_c(TERM_ORANGE, " intelligently");
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) text_out("�A");
#else /* JP */
			if (n == 0) text_out(" which ");
			else if (n < vn-1) text_out(", ");
			else text_out(" or ");
#endif /* JP */

			/* Dump */
			text_out_c(TERM_L_RED, vp[n]);
		}
#ifdef JP
		text_out("�̎����������邱�Ƃ�����");
#endif /* JP */
	}


	/* End the sentence about innate/other spells */
	if (breath || magic)
	{
		/* Total casting */
		m = l_ptr->cast_innate + l_ptr->cast_spell;

		/* Average frequency */
		n = (r_ptr->freq_innate + r_ptr->freq_spell) / 2;

		/* Describe the spell frequency */
		if (m > 100)
		{
#ifdef JP
			text_out(format("(�m��:1/%d)", 100 / n));
#else /* JP */
			text_out(format("; 1 time in %d", 100 / n));
#endif /* JP */
		}

		/* Guess at the frequency */
		else if (m)
		{
			n = ((n + 9) / 10) * 10;
#ifdef JP
			text_out(format("(�m��:��1/%d)", 100 / n));
#else /* JP */
			text_out(format("; about 1 time in %d", 100 / n));
#endif /* JP */
		}

		/* End this sentence */
#ifdef JP
		text_out("�B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}
}


static void describe_monster_drop(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

	bool sin = FALSE;

	int n;

	cptr p;

	int msex = 0;


	/* Extract a gender (if applicable) */
	if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;

	/* Drops gold and/or items */
	if (l_ptr->drop_gold || l_ptr->drop_item)
	{
		/* Intro */
#ifdef JP
		text_out(format("%^s��", wd_he[msex]));
#else /* JP */
		text_out(format("%^s may carry", wd_he[msex]));
#endif /* JP */

		/* Count maximum drop */
		n = MAX(l_ptr->drop_gold, l_ptr->drop_item);

		/* One drop (may need an "n") */
		if (n == 1)
		{
#ifdef JP
			text_out("���");
#else /* JP */
			text_out(" a");
#endif /* JP */
			sin = TRUE;
		}

		/* Two drops */
		else if (n == 2)
		{
#ifdef JP
			text_out("������");
#else /* JP */
			text_out(" one or two");
#endif /* JP */
		}

		/* Many drops */
		else
		{
#ifdef JP
			text_out(format(" %d �܂ł�", n));
#else /* JP */
			text_out(format(" up to %d", n));
#endif /* JP */
		}


		/* Great */
		if (l_ptr->flags1 & RF1_DROP_GREAT)
		{
#ifdef JP
			p = "���ʂ�";
#else /* JP */
			p = " exceptional";
#endif /* JP */
		}

		/* Good (no "n" needed) */
		else if (l_ptr->flags1 & RF1_DROP_GOOD)
		{
#ifdef JP
			p = "�㎿��";
#else /* JP */
			p = " good";
#endif /* JP */
			sin = FALSE;
		}

		/* Okay */
		else
		{
			p = NULL;
		}


		/* Objects */
		if (l_ptr->drop_item)
		{
			/* Handle singular "an" */
#ifndef JP
			if (sin) text_out("n");
#endif /* JP */
			sin = FALSE;

			/* Dump "object(s)" */
			if (p) text_out(p);
#ifdef JP
			text_out("�A�C�e��");
#else /* JP */
			text_out(" object");
			if (n != 1) text_out("s");
#endif /* JP */

			/* Conjunction replaces variety, if needed for "gold" below */
#ifdef JP
			p = "��";
#else /* JP */
			p = " or";
#endif /* JP */
		}

		/* Treasures */
		if (l_ptr->drop_gold)
		{
			/* Cancel prefix */
			if (!p) sin = FALSE;

			/* Handle singular "an" */
#ifndef JP
			if (sin) text_out("n");
#endif /* JP */

			/* Dump "treasure(s)" */
			if (p) text_out(p);
#ifdef JP
			text_out("����");
#else /* JP */
			text_out(" treasure");
			if (n != 1) text_out("s");
#endif /* JP */
		}

		/* End this sentence */
#ifdef JP
		text_out("�������Ă��邱�Ƃ�����B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}
}


static void describe_monster_attack(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];
	int m, n, r;
	cptr p, q;
#ifdef JP
	char jverb_buf[64];
#endif /* JP */

	int msex = 0;

	/* Extract a gender (if applicable) */
	if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;

	
	/* Count the number of "known" attacks */
	for (n = 0, m = 0; m < MONSTER_BLOW_MAX; m++)
	{
		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;

		/* Count known attacks */
		if (l_ptr->blows[m]) n++;
	}

	/* Examine (and count) the actual attacks */
	for (r = 0, m = 0; m < MONSTER_BLOW_MAX; m++)
	{
		int method, effect, d1, d2;

		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;

		/* Skip unknown attacks */
		if (!l_ptr->blows[m]) continue;


		/* Extract the attack info */
		method = r_ptr->blow[m].method;
		effect = r_ptr->blow[m].effect;
		d1 = r_ptr->blow[m].d_dice;
		d2 = r_ptr->blow[m].d_side;


		/* No method yet */
		p = NULL;

		/* Get the method */
		switch (method)
		{
#ifdef JP
			case RBM_HIT:	p = "����"; break;
			case RBM_TOUCH:	p = "�G��"; break;
			case RBM_PUNCH:	p = "�p���`����"; break;
			case RBM_KICK:	p = "�R��"; break;
			case RBM_CLAW:	p = "�Ђ�����"; break;
			case RBM_BITE:	p = "����"; break;
			case RBM_STING:	p = "�h��"; break;
			case RBM_XXX1:	break;
			case RBM_BUTT:	p = "�p�œ˂�"; break;
			case RBM_CRUSH:	p = "�̓����肷��"; break;
			case RBM_ENGULF:p = "���ݍ���"; break;
			case RBM_XXX2:	break;
			case RBM_CRAWL:	p = "�̂̏�𔇂����"; break;
			case RBM_DROOL:	p = "�悾������炷"; break;
			case RBM_SPIT:	p = "�΂�f��"; break;
			case RBM_XXX3:	break;
			case RBM_GAZE:	p = "�ɂ��"; break;
			case RBM_WAIL:	p = "��������"; break;
			case RBM_SPORE:	p = "�E�q���΂�"; break;
			case RBM_XXX4:	break;
			case RBM_BEG:	p = "����������"; break;
			case RBM_INSULT:p = "���J����"; break;
			case RBM_MOAN:	p = "���߂�"; break;
			case RBM_XXX5:	break;
#else /* JP */
			case RBM_HIT:	p = "hit"; break;
			case RBM_TOUCH:	p = "touch"; break;
			case RBM_PUNCH:	p = "punch"; break;
			case RBM_KICK:	p = "kick"; break;
			case RBM_CLAW:	p = "claw"; break;
			case RBM_BITE:	p = "bite"; break;
			case RBM_STING:	p = "sting"; break;
			case RBM_XXX1:	break;
			case RBM_BUTT:	p = "butt"; break;
			case RBM_CRUSH:	p = "crush"; break;
			case RBM_ENGULF:	p = "engulf"; break;
			case RBM_XXX2:	break;
			case RBM_CRAWL:	p = "crawl on you"; break;
			case RBM_DROOL:	p = "drool on you"; break;
			case RBM_SPIT:	p = "spit"; break;
			case RBM_XXX3:	break;
			case RBM_GAZE:	p = "gaze"; break;
			case RBM_WAIL:	p = "wail"; break;
			case RBM_SPORE:	p = "release spores"; break;
			case RBM_XXX4:	break;
			case RBM_BEG:	p = "beg"; break;
			case RBM_INSULT:	p = "insult"; break;
			case RBM_MOAN:	p = "moan"; break;
			case RBM_XXX5:	break;
#endif /* JP */
		}


		/* Default effect */
		q = NULL;

		/* Get the effect */
		switch (effect)
		{
#ifdef JP
			case RBE_HURT:      q = "�U������"; break;
			case RBE_POISON:    q = "�ł�����킷"; break;
			case RBE_UN_BONUS:  q = "�򉻂�����"; break;
			case RBE_UN_POWER:  q = "���͂��z�����"; break;
			case RBE_EAT_GOLD:  q = "���𓐂�"; break;
			case RBE_EAT_ITEM:  q = "�A�C�e���𓐂�"; break;
			case RBE_EAT_FOOD:  q = "���Ȃ��̐H����H�ׂ�"; break;
			case RBE_EAT_LITE:  q = "��������z������"; break;
			case RBE_ACID:      q = "�_���΂�"; break;
			case RBE_ELEC:      q = "�d��������킷"; break;
			case RBE_FIRE:      q = "�R�₷"; break;
			case RBE_COLD:      q = "���点��"; break;
			case RBE_BLIND:     q = "�Ӗڂɂ���"; break;
			case RBE_CONFUSE:   q = "����������"; break;
			case RBE_TERRIFY:   q = "���|������"; break;
			case RBE_PARALYZE:  q = "��Ⴢ�����"; break;
			case RBE_LOSE_STR:  q = "�r�͂�����������"; break;
			case RBE_LOSE_INT:  q = "�m�\������������"; break;
			case RBE_LOSE_WIS:  q = "����������������"; break;
			case RBE_LOSE_DEX:  q = "��p��������������"; break;
			case RBE_LOSE_CON:  q = "�ϋv�͂�����������"; break;
			case RBE_LOSE_CHR:  q = "���͂�����������"; break;
			case RBE_LOSE_ALL:  q = "�S�X�e�[�^�X������������"; break;
			case RBE_SHATTER:   q = "���ӂ���"; break;
			case RBE_EXP_10:    q = "�o���l������������"; break;
			case RBE_EXP_20:    q = "�o���l������������"; break;
			case RBE_EXP_40:    q = "�o���l������������"; break;
			case RBE_EXP_80:    q = "�o���l������������"; break;
			case RBE_HALLU:     q = "���o�������N����"; break;
#else /* JP */
			case RBE_HURT:      q = "attack"; break;
			case RBE_POISON:    q = "poison"; break;
			case RBE_UN_BONUS:  q = "disenchant"; break;
			case RBE_UN_POWER:  q = "drain charges"; break;
			case RBE_EAT_GOLD:  q = "steal gold"; break;
			case RBE_EAT_ITEM:  q = "steal items"; break;
			case RBE_EAT_FOOD:  q = "eat your food"; break;
			case RBE_EAT_LITE:  q = "absorb light"; break;
			case RBE_ACID:      q = "shoot acid"; break;
			case RBE_ELEC:      q = "electrify"; break;
			case RBE_FIRE:      q = "burn"; break;
			case RBE_COLD:      q = "freeze"; break;
			case RBE_BLIND:     q = "blind"; break;
			case RBE_CONFUSE:   q = "confuse"; break;
			case RBE_TERRIFY:   q = "terrify"; break;
			case RBE_PARALYZE:  q = "paralyze"; break;
			case RBE_LOSE_STR:  q = "reduce strength"; break;
			case RBE_LOSE_INT:  q = "reduce intelligence"; break;
			case RBE_LOSE_WIS:  q = "reduce wisdom"; break;
			case RBE_LOSE_DEX:  q = "reduce dexterity"; break;
			case RBE_LOSE_CON:  q = "reduce constitution"; break;
			case RBE_LOSE_CHR:  q = "reduce charisma"; break;
			case RBE_LOSE_ALL:  q = "reduce all stats"; break;
			case RBE_SHATTER:   q = "shatter"; break;
			case RBE_EXP_10:    q = "lower experience"; break;
			case RBE_EXP_20:    q = "lower experience"; break;
			case RBE_EXP_40:    q = "lower experience"; break;
			case RBE_EXP_80:    q = "lower experience"; break;
			case RBE_HALLU:     q = "cause hallucinations"; break;
#endif /* JP */
		}


#ifdef JP
		/* Introduce the attack description */
		if (!r)
		{
			text_out(format("%^s��", wd_he[msex]));
		}

		/***�኱�\����ύX ita ***/

		/* Describe damage (if known) */
		if (d1 && d2 && know_damage(r_idx, l_ptr, m))
		{
			/* Display the damage */
			text_out(format(" %dd%d ", d1, d2));
			text_out("�̃_���[�W��");
		}
		/* Hack -- force a method */
		if (!p) p = "������Ȃ��Ƃ�����";

		/* Describe the method */
		/* XX����YY��/XX����YY����/XX��/XX���� */
		if (q)             jverb( p ,jverb_buf, JVERB_TO);
		else if (r < n-1)  jverb( p ,jverb_buf, JVERB_AND);
		else               strcpy(jverb_buf, p);

		text_out(jverb_buf);

		/* Describe the effect (if any) */
		if (q)
		{
			if (r < n-1)  jverb( q,jverb_buf, JVERB_AND);
			else          strcpy(jverb_buf, q); 
			text_out_c(TERM_L_RED, jverb_buf);
		}
		if (r != n-1)
		{
			text_out("�A");
		}
#else /* JP */
		/* Introduce the attack description */
		if (!r)
		{
			text_out(format("%^s can ", wd_he[msex]));
		}
		else if (r < n-1)
		{
			text_out(", ");
		}
		else
		{
			text_out(", and ");
		}


		/* Hack -- force a method */
		if (!p) p = "do something weird";

		/* Describe the method */
		text_out(p);


		/* Describe the effect (if any) */
		if (q)
		{
			/* Describe the attack type */
			text_out(" to ");
			text_out_c(TERM_L_RED, q);

			/* Describe damage (if known) */
			if (d1 && d2 && know_damage(r_idx, l_ptr, m))
			{
				/* Display the damage */
				text_out(" with damage");
				text_out(format(" %dd%d", d1, d2));
			}
		}
#endif /* JP */


		/* Count the attacks as printed */
		r++;
	}

	/* Finish sentence above */
	if (r)
	{
#ifdef JP
		text_out("�B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}

	/* Notice lack of attacks */
	else if (l_ptr->flags1 & RF1_NEVER_BLOW)
	{
#ifdef JP
		text_out(format("%^s�͕����I�ȍU�����@�������Ȃ��B", wd_he[msex]));
#else /* JP */
		text_out(format("%^s has no physical attacks.  ", wd_he[msex]));
#endif /* JP */
	}

	/* Or describe the lack of knowledge */
	else
	{
#ifdef JP
		text_out(format("%s�U���ɂ��Ă͉����m��Ȃ��B", wd_his[msex]));
#else /* JP */
		text_out(format("Nothing is known about %s attack.  ", wd_his[msex]));
#endif /* JP */
	}
}


static void describe_monster_abilities(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

	int n;

	int vn;
	cptr vp[64];
#ifdef JP
	char jverb_buf[64];
#endif /* JP */

	int msex = 0;


	/* Extract a gender (if applicable) */
	if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;

	/* Collect special abilities. */
	vn = 0;
#ifdef JP
	if (l_ptr->flags2 & RF2_OPEN_DOOR) vp[vn++] = "�h�A���J����";
	if (l_ptr->flags2 & RF2_BASH_DOOR) vp[vn++] = "�h�A��ł��j��";
	if (l_ptr->flags2 & RF2_PASS_WALL) vp[vn++] = "�ǂ����蔲����";
	if (l_ptr->flags2 & RF2_KILL_WALL) vp[vn++] = "�ǂ��@��i��";
	if (l_ptr->flags2 & RF2_MOVE_BODY) vp[vn++] = "�ア�����X�^�[�������̂���";
	if (l_ptr->flags2 & RF2_KILL_BODY) vp[vn++] = "�ア�����X�^�[��|��";
	if (l_ptr->flags2 & RF2_TAKE_ITEM) vp[vn++] = "�A�C�e�����E��";
	if (l_ptr->flags2 & RF2_KILL_ITEM) vp[vn++] = "�A�C�e������";
#else /* JP */
	if (l_ptr->flags2 & RF2_OPEN_DOOR) vp[vn++] = "open doors";
	if (l_ptr->flags2 & RF2_BASH_DOOR) vp[vn++] = "bash down doors";
	if (l_ptr->flags2 & RF2_PASS_WALL) vp[vn++] = "pass through walls";
	if (l_ptr->flags2 & RF2_KILL_WALL) vp[vn++] = "bore through walls";
	if (l_ptr->flags2 & RF2_MOVE_BODY) vp[vn++] = "push past weaker monsters";
	if (l_ptr->flags2 & RF2_KILL_BODY) vp[vn++] = "destroy weaker monsters";
	if (l_ptr->flags2 & RF2_TAKE_ITEM) vp[vn++] = "pick up objects";
	if (l_ptr->flags2 & RF2_KILL_ITEM) vp[vn++] = "destroy objects";
#endif /* JP */

	/* Describe special abilities. */
	if (vn)
	{
		/* Intro */
#ifdef JP
		text_out(format("%^s��", wd_he[msex]));
#else /* JP */
		text_out(format("%^s", wd_he[msex]));
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
#ifdef JP
			/* Dump */
			if (n < vn-1)
			{
				jverb(vp[n], jverb_buf, JVERB_AND);
				text_out(jverb_buf);
				text_out("�A");
			}
			else text_out(vp[n]);
#else /* JP */
			/* Intro */
			if (n == 0) text_out(" can ");
			else if (n < vn-1) text_out(", ");
			else text_out(" and ");

			/* Dump */
			text_out(vp[n]);
#endif /* JP */
		}

		/* End */
#ifdef JP
		text_out("���Ƃ��ł���B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}


	/* Describe special abilities. */
	if (l_ptr->flags2 & RF2_INVISIBLE)
	{
#ifdef JP
		text_out(format("%^s�͓����ŖڂɌ����Ȃ��B", wd_he[msex]));
#else /* JP */
		text_out(format("%^s is invisible.  ", wd_he[msex]));
#endif /* JP */
	}
	if (l_ptr->flags2 & RF2_COLD_BLOOD)
	{
#ifdef JP
		text_out(format("%^s�͗⌌�����ł���B", wd_he[msex]));
#else /* JP */
		text_out(format("%^s is cold blooded.  ", wd_he[msex]));
#endif /* JP */
	}
	if (l_ptr->flags2 & RF2_EMPTY_MIND)
	{
#ifdef JP
		text_out(format("%^s�̓e���p�V�[�ł͊��m�ł��Ȃ��B", wd_he[msex]));
#else /* JP */
		text_out(format("%^s is not detected by telepathy.  ", wd_he[msex]));
#endif /* JP */
	}
	if (l_ptr->flags2 & RF2_WEIRD_MIND)
	{
#ifdef JP
		text_out(format("%^s�͂܂�Ƀe���p�V�[�Ŋ��m�ł���B", wd_he[msex]));
#else /* JP */
		text_out(format("%^s is rarely detected by telepathy.  ", wd_he[msex]));
#endif /* JP */
	}
	if (l_ptr->flags2 & RF2_MULTIPLY)
	{
#ifdef JP
		text_out(format("%^s�͔����I�ɑ��B����B", wd_he[msex]));
#else /* JP */
		text_out(format("%^s breeds explosively.  ", wd_he[msex]));
#endif /* JP */
	}
	if (l_ptr->flags2 & RF2_REGENERATE)
	{
#ifdef JP
		text_out(format("%^s�͑f�����̗͂��񕜂���B", wd_he[msex]));
#else /* JP */
		text_out(format("%^s regenerates quickly.  ", wd_he[msex]));
#endif /* JP */
	}


	/* Collect susceptibilities */
	vn = 0;
#ifdef JP
	if (l_ptr->flags3 & RF3_HURT_ROCK) vp[vn++] = "��������������";
	if (l_ptr->flags3 & RF3_HURT_LITE) vp[vn++] = "���邢��";
	if (l_ptr->flags3 & RF3_HURT_FIRE) vp[vn++] = "��";
	if (l_ptr->flags3 & RF3_HURT_COLD) vp[vn++] = "��C";
#else /* JP */
	if (l_ptr->flags3 & RF3_HURT_ROCK) vp[vn++] = "rock remover";
	if (l_ptr->flags3 & RF3_HURT_LITE) vp[vn++] = "bright light";
	if (l_ptr->flags3 & RF3_HURT_FIRE) vp[vn++] = "fire";
	if (l_ptr->flags3 & RF3_HURT_COLD) vp[vn++] = "cold";
#endif /* JP */

	/* Describe susceptibilities */
	if (vn)
	{
		/* Intro */
#ifdef JP
		text_out(format("%^s�ɂ�", wd_he[msex]));
#else /* JP */
		text_out(format("%^s", wd_he[msex]));
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if (n != 0) text_out("��");
#else /* JP */
			if (n == 0) text_out(" is hurt by ");
			else if (n < vn-1) text_out(", ");
			else text_out(" and ");
#endif /* JP */

			/* Dump */
			text_out_c(TERM_YELLOW, vp[n]);
		}

		/* End */
#ifdef JP
		text_out("�Ń_���[�W��^������B");
#else /* JP */
		text_out(".  ");
#endif
	}


	/* Collect immunities */
	vn = 0;
#ifdef JP
	if (l_ptr->flags3 & RF3_IM_ACID) vp[vn++] = "�_";
	if (l_ptr->flags3 & RF3_IM_ELEC) vp[vn++] = "�d��";
	if (l_ptr->flags3 & RF3_IM_FIRE) vp[vn++] = "��";
	if (l_ptr->flags3 & RF3_IM_COLD) vp[vn++] = "��C";
	if (l_ptr->flags3 & RF3_IM_POIS) vp[vn++] = "��";
	if (l_ptr->flags3 & RF3_IM_WATER) vp[vn++] = "��";
#else /* JP */
	if (l_ptr->flags3 & RF3_IM_ACID) vp[vn++] = "acid";
	if (l_ptr->flags3 & RF3_IM_ELEC) vp[vn++] = "lightning";
	if (l_ptr->flags3 & RF3_IM_FIRE) vp[vn++] = "fire";
	if (l_ptr->flags3 & RF3_IM_COLD) vp[vn++] = "cold";
	if (l_ptr->flags3 & RF3_IM_POIS) vp[vn++] = "poison";
	if (l_ptr->flags3 & RF3_IM_WATER) vp[vn++] = "water";
#endif /* JP */

	/* Describe immunities */
	if (vn)
	{
		/* Intro */
#ifdef JP
		text_out(format("%^s��", wd_he[msex]));
#else /* JP */
		text_out(format("%^s", wd_he[msex]));
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if (n != 0) text_out("��");
#else /* JP */
			if (n == 0) text_out(" resists ");
			else if (n < vn-1) text_out(", ");
			else text_out(" and ");
#endif /* JP */

			/* Dump */
			text_out_c(TERM_ORANGE, vp[n]);
		}

		/* End */
#ifdef JP
		text_out("�̑ϐ��������Ă���B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}


	/* Collect resistances */
	vn = 0;
#ifdef JP
	if (l_ptr->flags3 & RF3_RES_NETH)  vp[vn++] = "�n��";
	if (l_ptr->flags3 & RF3_RES_PLAS)  vp[vn++] = "�v���Y�}";
	if (l_ptr->flags3 & RF3_RES_NEXUS) vp[vn++] = "���ʍ���";
	if (l_ptr->flags3 & RF3_RES_DISE)  vp[vn++] = "��";
#else /* JP */
	if (l_ptr->flags3 & RF3_RES_NETH)  vp[vn++] = "nether";
	if (l_ptr->flags3 & RF3_RES_PLAS)  vp[vn++] = "plasma";
	if (l_ptr->flags3 & RF3_RES_NEXUS) vp[vn++] = "nexus";
	if (l_ptr->flags3 & RF3_RES_DISE)  vp[vn++] = "disenchantment";
#endif /* JP */

	/* Describe resistances */
	if (vn)
	{
		/* Intro */
#ifdef JP
		text_out(format("%^s��", wd_he[msex]));
#else /* JP */
		text_out(format("%^s", wd_he[msex]));
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) text_out("��");
#else /* JP */
			if (n == 0) text_out(" resists ");
			else if (n < vn-1) text_out(", ");
			else text_out(" and ");
#endif /* JP */

			/* Dump */
			text_out_c(TERM_ORANGE, vp[n]);
		}

		/* End */
#ifdef JP
		text_out("�̑ϐ��������Ă���B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}


	/* Collect non-effects */
	vn = 0;
#ifdef JP
	if (l_ptr->flags3 & RF3_NO_STUN) vp[vn++] = "�N�O�Ƃ��Ȃ�";
	if (l_ptr->flags3 & RF3_NO_FEAR) vp[vn++] = "���|�������Ȃ�";
	if (l_ptr->flags3 & RF3_NO_CONF) vp[vn++] = "�������Ȃ�";
	if (l_ptr->flags3 & RF3_NO_SLEEP) vp[vn++] = "���炳��Ȃ�";
#else /* JP */
	if (l_ptr->flags3 & RF3_NO_STUN) vp[vn++] = "stunned";
	if (l_ptr->flags3 & RF3_NO_FEAR) vp[vn++] = "frightened";
	if (l_ptr->flags3 & RF3_NO_CONF) vp[vn++] = "confused";
	if (l_ptr->flags3 & RF3_NO_SLEEP) vp[vn++] = "slept";
#endif /* JP */

	/* Describe non-effects */
	if (vn)
	{
		/* Intro */
#ifdef JP
		text_out(format("%^s��", wd_he[msex]));
#else /* JP */
		text_out(format("%^s", wd_he[msex]));
#endif /* JP */

		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) text_out("���A");
#else /* JP */
			if (n == 0) text_out(" cannot be ");
			else if (n < vn-1) text_out(", ");
			else text_out(" or ");
#endif /* JP */

			/* Dump */
			text_out_c(TERM_YELLOW, vp[n]);
		}

		/* End */
#ifdef JP
		text_out("�B");
#else /* JP */
		text_out(".  ");
#endif /* JP */
	}


	/* Do we know how aware it is? */
	if ((((int)l_ptr->wake * (int)l_ptr->wake) > r_ptr->sleep) ||
	    (l_ptr->ignore == MAX_UCHAR) ||
	    ((r_ptr->sleep == 0) && (l_ptr->tkills >= 10)))
	{
		cptr act;

		if (r_ptr->sleep > 200)
		{
#ifdef JP
			act = "�𖳎��������ł��邪";
#else /* JP */
			act = "prefers to ignore";
#endif /* JP */
		}
		else if (r_ptr->sleep > 95)
		{
#ifdef JP
			act = "�ɑ΂��ĂقƂ�ǒ��ӂ𕥂�Ȃ���";
#else /* JP */
			act = "pays very little attention to";
#endif /* JP */
		}
		else if (r_ptr->sleep > 75)
		{
#ifdef JP
			act = "�ɑ΂��Ă��܂蒍�ӂ𕥂�Ȃ���";
#else /* JP */
			act = "pays little attention to";
#endif /* JP */
		}
		else if (r_ptr->sleep > 45)
		{
#ifdef JP
			act = "�����߂��������ł��邪";
#else /* JP */
			act = "tends to overlook";
#endif /* JP */
		}
		else if (r_ptr->sleep > 25)
		{
#ifdef JP
			act = "���ق�̏����͌��Ă���";
#else /* JP */
			act = "takes quite a while to see";
#endif /* JP */
		}
		else if (r_ptr->sleep > 10)
		{
#ifdef JP
			act = "�����΂炭�͌��Ă���";
#else /* JP */
			act = "takes a while to see";
#endif /* JP */
		}
		else if (r_ptr->sleep > 5)
		{
#ifdef JP
			act = "���������Ӑ[�����Ă���";
#else /* JP */
			act = "is fairly observant of";
#endif /* JP */
		}
		else if (r_ptr->sleep > 3)
		{
#ifdef JP
			act = "�𒍈Ӑ[�����Ă���";
#else /* JP */
			act = "is observant of";
#endif /* JP */
		}
		else if (r_ptr->sleep > 1)
		{
#ifdef JP
			act = "�����Ȃ蒍�Ӑ[�����Ă���";
#else /* JP */
			act = "is very observant of";
#endif /* JP */
		}
		else if (r_ptr->sleep > 0)
		{
#ifdef JP
			act = "���x�����Ă���";
#else /* JP */
			act = "is vigilant for";
#endif /* JP */
		}
		else
		{
#ifdef JP
			act = "�����Ȃ�x�����Ă���";
#else /* JP */
			act = "is ever vigilant for";
#endif /* JP */
		}

#ifdef JP
		text_out(format("%^s�͐N����%s�A %d �t�B�[�g�悩��N���҂ɋC�t�����Ƃ�����B",
		     wd_he[msex], act, 10 * r_ptr->aaf));
#else /* JP */
		text_out(format("%^s %s intruders, which %s may notice from %d feet.  ",
		            wd_he[msex], act, wd_he[msex], 10 * r_ptr->aaf));
#endif /* JP */
	}

	/* Describe escorts */
	if ((l_ptr->flags1 & RF1_ESCORT) || (l_ptr->flags1 & RF1_ESCORTS))
	{
#ifdef JP
		text_out(format("%^s�͒ʏ��q�𔺂��Č����B",
#else /* JP */
		text_out(format("%^s usually appears with escorts.  ",
#endif /* JP */
		            wd_he[msex]));
	}

	/* Describe friends */
	else if ((l_ptr->flags1 & RF1_FRIEND) || (l_ptr->flags1 & RF1_FRIENDS))
	{
#ifdef JP
		text_out(format("%^s�͒ʏ�W�c�Ō����B",
#else /* JP */
		text_out(format("%^s usually appears in groups.  ",
#endif /* JP */
		            wd_he[msex]));
	}
}


static void describe_monster_kills(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

	int msex = 0;

	bool out = TRUE;


	/* Extract a gender (if applicable) */
	if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;


	/* Treat uniques differently */
	if (l_ptr->flags1 & RF1_UNIQUE)
	{
		/* Hack -- Determine if the unique is "dead" */
		bool dead = (r_ptr->max_num == 0) ? TRUE : FALSE;

		/* We've been killed... */
		if (l_ptr->deaths)
		{
			/* Killed ancestors */
#ifdef JP
			text_out(format("%^s�͂��Ȃ��̐�c�� %d �l�����Ă���",
#else /* JP */
			text_out(format("%^s has slain %d of your ancestors",
#endif /* JP */
			            wd_he[msex], l_ptr->deaths));

			/* But we've also killed it */
			if (dead)
			{
#ifdef JP
				text_out("���A���łɋw�����͉ʂ����Ă���I");
#else /* JP */
				text_out(", but you have taken revenge!  ");
#endif /* JP */
			}

			/* Unavenged (ever) */
			else
			{
#ifdef JP
				text_out("�̂ɁA�܂��w�������ʂ����Ă��Ȃ��B");
#else /* JP */
				text_out(format(", who %s unavenged.  ",
				            plural(l_ptr->deaths, "remains", "remain")));
#endif /* JP */
			}
		}

		/* Dead unique who never hurt us */
		else if (dead)
		{
#ifdef JP
			text_out("���Ȃ��͂��̋w�G�����łɑ��苎���Ă���B");
#else /* JP */
			text_out("You have slain this foe.  ");
#endif /* JP */
		}
		else
		{
			/* Alive and never killed us */
			out = FALSE;
		}
	}

	/* Not unique, but killed us */
	else if (l_ptr->deaths)
	{
		/* Dead ancestors */
#ifdef JP
		text_out(format("���̃����X�^�[�͂��Ȃ��̐�c�� %d �l�����Ă���",
		            l_ptr->deaths));
#else /* JP */
		text_out(format("%d of your ancestors %s been killed by this creature, ",
		            l_ptr->deaths, plural(l_ptr->deaths, "has", "have")));
#endif /* JP */

		/* Some kills this life */
		if (l_ptr->pkills)
		{
#ifdef JP
			text_out(format("���A���Ȃ��͂��̃����X�^�[�����Ȃ��Ƃ� %d �͓̂|���Ă���B",
#else /* JP */
			text_out(format("and you have exterminated at least %d of the creatures.  ",
#endif /* JP */
			            l_ptr->pkills));
		}

		/* Some kills past lives */
		else if (l_ptr->tkills)
		{
#ifdef JP
			text_out(format("���A%s�͂��̃����X�^�[�����Ȃ��Ƃ� %d �͓̂|���Ă���B",
			            "���Ȃ��̐�c", l_ptr->tkills));
#else /* JP */
			text_out(format("and %s have exterminated at least %d of the creatures.  ",
			            "your ancestors", l_ptr->tkills));
#endif /* JP */
		}

		/* No kills */
		else
		{
#ifdef JP
			text_out_c(TERM_RED, format("���A�܂�%s��|�������Ƃ͂Ȃ��B",
#else /* JP */
			text_out_c(TERM_RED, format("and %s is not ever known to have been defeated.  ",
#endif /* JP */
			            wd_he[msex]));
		}
	}

	/* Normal monsters */
	else
	{
		/* Killed some this life */
		if (l_ptr->pkills)
		{
#ifdef JP
			text_out(format("���Ȃ��͂��̃����X�^�[�����Ȃ��Ƃ� %d �͎̂E���Ă���B",
#else /* JP */
			text_out(format("You have killed at least %d of these creatures.  ",
#endif /* JP */
			            l_ptr->pkills));
		}

		/* Killed some last life */
		else if (l_ptr->tkills)
		{
#ifdef JP
			text_out(format("���Ȃ��̐�c�͂��̃����X�^�[�����Ȃ��Ƃ� %d �͎̂E���Ă���B",
#else /* JP */
			text_out(format("Your ancestors have killed at least %d of these creatures.  ",
#endif /* JP */
			            l_ptr->tkills));
		}

		/* Killed none */
		else
		{
#ifdef JP
			text_out("���̃����X�^�[��|�������Ƃ͂Ȃ��B");
#else /* JP */
			text_out("No battles to the death are recalled.  ");
#endif /* JP */
		}
	}

	/* Separate */
	if (out) text_out("\n");
}


static void describe_monster_toughness(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

	int msex = 0;


	/* Extract a gender (if applicable) */
	if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;
	
	/* Describe monster "toughness" */
	if (know_armour(r_idx, l_ptr))
	{
		/* Armor */
#ifdef JP
		text_out(format("%^s�� AC%d �̖h��͂�",
#else /* JP */
		text_out(format("%^s has an armor rating of %d",
#endif /* JP */
		            wd_he[msex], r_ptr->ac));

		/* Maximized hitpoints */
		if (l_ptr->flags1 & RF1_FORCE_MAXHP)
		{
#ifdef JP
			text_out(format(" %d �̗̑͂�����B",
#else /* JP */
			text_out(format(" and a life rating of %d.  ",
#endif /* JP */
			            r_ptr->hdice * r_ptr->hside));
		}

		/* Variable hitpoints */
		else
		{
#ifdef JP
			text_out(format(" %dd%d �̗̑͂�����B",
#else /* JP */
			text_out(format(" and a life rating of %dd%d.  ",
#endif /* JP */
			            r_ptr->hdice, r_ptr->hside));
		}
	}
}


static void describe_monster_exp(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

#ifndef JP
	cptr p, q;
#endif /* JP */

	long i, j;


	/* Describe experience if known */
	if (l_ptr->tkills)
	{
		/* Introduction */
#ifndef JP
		if (l_ptr->flags1 & RF1_UNIQUE)
			text_out("Killing");
		else
			text_out("A kill of");
#endif /* JP */

#ifdef JP
		text_out("���̃����X�^�[");
#else /* JP */
		text_out(" this creature");
#endif /* JP */

#ifdef JP
		text_out("��|�����Ƃ�");
#endif /* JP */

#ifdef JP
		i = p_ptr->lev;

		/* Mention the dependance on the player's level */
		text_out(format(" %lu ���x���̃L�����N�^�ɂƂ���", (long)i));

		/* calculate the integer exp part */
		i = (long)r_ptr->mexp * r_ptr->level / p_ptr->lev;

		/* calculate the fractional exp part scaled by 100, */
		/* must use long arithmetic to avoid overflow */
		j = ((((long)r_ptr->mexp * r_ptr->level % p_ptr->lev) *
		       (long)1000 / p_ptr->lev + 5) / 10);

		/* Mention the experience */
		text_out(format(" %ld.%02ld �|�C���g�̌o���ƂȂ�B",
		        (long)i, (long)j ));
#else /* JP */
		/* calculate the integer exp part */
		i = (long)r_ptr->mexp * r_ptr->level / p_ptr->lev;

		/* calculate the fractional exp part scaled by 100, */
		/* must use long arithmetic to avoid overflow */
		j = ((((long)r_ptr->mexp * r_ptr->level % p_ptr->lev) *
			  (long)1000 / p_ptr->lev + 5) / 10);

		/* Mention the experience */
		text_out(format(" is worth %ld.%02ld point%s",
			        (long)i, (long)j,
			        (((i == 1) && (j == 0)) ? "" : "s")));

		/* Take account of annoying English */
		p = "th";
		i = p_ptr->lev % 10;
		if ((p_ptr->lev / 10) == 1) /* nothing */;
		else if (i == 1) p = "st";
		else if (i == 2) p = "nd";
		else if (i == 3) p = "rd";

		/* Take account of "leading vowels" in numbers */
		q = "";
		i = p_ptr->lev;
		if ((i == 8) || (i == 11) || (i == 18)) q = "n";

		/* Mention the dependance on the player's level */
		text_out(format(" for a%s %lu%s level character.  ",
			        q, (long)i, p));
#endif /* JP */
	}
}


static void describe_monster_movement(int r_idx, const monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

	bool old = FALSE;


#ifdef JP
	text_out("����");
#else /* JP */
	text_out("This");
#endif /* JP */

#ifdef JP
	if (l_ptr->flags3 & RF3_ANIMAL) text_out_c(TERM_L_BLUE, "���R�E��");
	if (l_ptr->flags3 & RF3_EVIL) text_out_c(TERM_L_BLUE, "�׈��Ȃ�");
	if (l_ptr->flags3 & RF3_UNDEAD) text_out_c(TERM_L_BLUE, "�A���f�b�h��");
#else /* JP */
	if (l_ptr->flags3 & RF3_ANIMAL) text_out_c(TERM_L_BLUE, " natural");
	if (l_ptr->flags3 & RF3_EVIL) text_out_c(TERM_L_BLUE, " evil");
	if (l_ptr->flags3 & RF3_UNDEAD) text_out_c(TERM_L_BLUE, " undead");
#endif /* JP */

#ifdef JP
	if (l_ptr->flags3 & RF3_DRAGON) text_out_c(TERM_L_BLUE, "�h���S��");
	else if (l_ptr->flags3 & RF3_DEMON) text_out_c(TERM_L_BLUE, "�f�[����");
	else if (l_ptr->flags3 & RF3_GIANT) text_out_c(TERM_L_BLUE, "�W���C�A���g");
	else if (l_ptr->flags3 & RF3_TROLL) text_out_c(TERM_L_BLUE, "�g����");
	else if (l_ptr->flags3 & RF3_ORC) text_out_c(TERM_L_BLUE, "�I�[�N");
	else text_out("�����X�^�[");
#else /* JP */
	if (l_ptr->flags3 & RF3_DRAGON) text_out_c(TERM_L_BLUE, " dragon");
	else if (l_ptr->flags3 & RF3_DEMON) text_out_c(TERM_L_BLUE, " demon");
	else if (l_ptr->flags3 & RF3_GIANT) text_out_c(TERM_L_BLUE, " giant");
	else if (l_ptr->flags3 & RF3_TROLL) text_out_c(TERM_L_BLUE, " troll");
	else if (l_ptr->flags3 & RF3_ORC) text_out_c(TERM_L_BLUE, " orc");
	else text_out(" creature");
#endif /* JP */

#ifdef JP
	text_out("��");
#endif /* JP */

	/* Describe location */
	if (r_ptr->level == 0)
	{
#ifdef JP
		text_out_c(TERM_SLATE, "���ɏZ��");
#else /* JP */
		text_out_c(TERM_SLATE, " lives in the town");
#endif /* JP */
		old = TRUE;
	}
	else if (l_ptr->tkills)
	{
		if (l_ptr->flags1 & RF1_FORCE_DEPTH)
#ifdef JP
			text_out_c(TERM_SLATE, "�n��");
#else /* JP */
			text_out_c(TERM_SLATE, " is found ");
#endif /* JP */
		else
#ifdef JP
			text_out_c(TERM_SLATE, "�ʏ�n��");
#else /* JP */
			text_out_c(TERM_SLATE, " is normally found ");
#endif /* JP */
		
		if (depth_in_feet)
		{
#ifdef JP
			text_out_c(TERM_SLATE, format(" %d �t�B�[�g",
#else /* JP */
			text_out_c(TERM_SLATE, format("at depths of %d feet",
#endif /* JP */
			                            r_ptr->level * 50));
		}
		else
		{
#ifdef JP
			text_out_c(TERM_SLATE, format(" %d �K",
#else /* JP */
			text_out_c(TERM_SLATE, format("on dungeon level %d",
#endif /* JP */
			                            r_ptr->level));
		}
#ifdef JP
		text_out_c(TERM_SLATE, "�ŏo����");
#endif /* JP */
		old = TRUE;
	}

#ifdef JP
	if (old) text_out("�A");
#else /* JP */
	if (old) text_out(", and");
#endif /* JP */

#ifndef JP
	text_out(" moves");
#endif /* JP */

	/* Random-ness */
	if ((l_ptr->flags1 & RF1_RAND_50) || (l_ptr->flags1 & RF1_RAND_25))
	{
		/* Adverb */
		if ((l_ptr->flags1 & RF1_RAND_50) && (l_ptr->flags1 & RF1_RAND_25))
		{
#ifdef JP
			text_out("���Ȃ�");
#else /* JP */
			text_out(" extremely");
#endif /* JP */
		}
		else if (l_ptr->flags1 & RF1_RAND_50)
		{
#ifdef JP
			text_out("����");
#else /* JP */
			text_out(" somewhat");
#endif /* JP */
		}
		else if (l_ptr->flags1 & RF1_RAND_25)
		{
#ifdef JP
			text_out("���X");
#else /* JP */
			text_out(" a bit");
#endif /* JP */
		}

		/* Adjective */
#ifdef JP
		text_out("�s�K����");
#else /* JP */
		text_out(" erratically");
#endif /* JP */

		/* Hack -- Occasional conjunction */
#ifdef JP
		if (r_ptr->speed != 110) text_out("�A����");
#else /* JP */
		if (r_ptr->speed != 110) text_out(", and");
#endif /* JP */
	}

	/* Speed */
	if (r_ptr->speed > 110)
	{
#ifdef JP
		if (r_ptr->speed > 130) text_out_c(TERM_GREEN, "�M����ق�");
		else if (r_ptr->speed > 120) text_out_c(TERM_GREEN, "����");
		text_out_c(TERM_GREEN, "�f����");
#else /* JP */
		if (r_ptr->speed > 130) text_out_c(TERM_GREEN, " incredibly");
		else if (r_ptr->speed > 120) text_out_c(TERM_GREEN, " very");
		text_out_c(TERM_GREEN, " quickly");
#endif /* JP */
	}
	else if (r_ptr->speed < 110)
	{
#ifdef JP
		if (r_ptr->speed < 90) text_out_c(TERM_GREEN, "�M����ق�");
		else if (r_ptr->speed < 100) text_out_c(TERM_GREEN, "����");
		text_out_c(TERM_GREEN, "��������");
#else /* JP */
		if (r_ptr->speed < 90) text_out_c(TERM_GREEN, " incredibly");
		else if (r_ptr->speed < 100) text_out_c(TERM_GREEN, " very");
		text_out_c(TERM_GREEN, " slowly");
#endif /* JP */
	}
	else
	{
#ifdef JP
		text_out_c(TERM_GREEN, "���ʂ̑�����");
#else /* JP */
		text_out_c(TERM_GREEN, " at normal speed");
#endif /* JP */
	}

#ifdef JP
	text_out("�����Ă���");
#endif /* JP */

	/* The code above includes "attack speed" */
	if (l_ptr->flags1 & RF1_NEVER_MOVE)
	{
#ifdef JP
		text_out("�A�������N���҂�ǐՂ��Ȃ�");
#else /* JP */
		text_out(", but does not deign to chase intruders");
#endif /* JP */
	}

	/* End this sentence */
#ifdef JP
	text_out("�B");
#else /* JP */
	text_out(".  ");
#endif /* JP */
}



/*
 * Learn everything about a monster (by cheating)
 */
static void cheat_monster_lore(int r_idx, monster_lore *l_ptr)
{
	const monster_race *r_ptr = &r_info[r_idx];

	int i;


	/* Hack -- Maximal kills */
	l_ptr->tkills = MAX_SHORT;

	/* Hack -- Maximal info */
	l_ptr->wake = l_ptr->ignore = MAX_UCHAR;

	/* Observe "maximal" attacks */
	for (i = 0; i < MONSTER_BLOW_MAX; i++)
	{
		/* Examine "actual" blows */
		if (r_ptr->blow[i].effect || r_ptr->blow[i].method)
		{
			/* Hack -- maximal observations */
			l_ptr->blows[i] = MAX_UCHAR;
		}
	}

	/* Hack -- maximal drops */
	l_ptr->drop_gold = l_ptr->drop_item =
	(((r_ptr->flags1 & RF1_DROP_4D2) ? 8 : 0) +
	 ((r_ptr->flags1 & RF1_DROP_3D2) ? 6 : 0) +
	 ((r_ptr->flags1 & RF1_DROP_2D2) ? 4 : 0) +
	 ((r_ptr->flags1 & RF1_DROP_1D2) ? 2 : 0) +
	 ((r_ptr->flags1 & RF1_DROP_90)  ? 1 : 0) +
	 ((r_ptr->flags1 & RF1_DROP_60)  ? 1 : 0));

	/* Hack -- but only "valid" drops */
	if (r_ptr->flags1 & RF1_ONLY_GOLD) l_ptr->drop_item = 0;
	if (r_ptr->flags1 & RF1_ONLY_ITEM) l_ptr->drop_gold = 0;

	/* Hack -- observe many spells */
	l_ptr->cast_innate = MAX_UCHAR;
	l_ptr->cast_spell = MAX_UCHAR;

	/* Hack -- know all the flags */
	l_ptr->flags1 = r_ptr->flags1;
	l_ptr->flags2 = r_ptr->flags2;
	l_ptr->flags3 = r_ptr->flags3;
	l_ptr->flags4 = r_ptr->flags4;
	l_ptr->flags5 = r_ptr->flags5;
	l_ptr->flags6 = r_ptr->flags6;
}


/*
 * Hack -- display monster information using "roff()"
 *
 * Note that there is now a compiler option to only read the monster
 * descriptions from the raw file when they are actually needed, which
 * saves about 60K of memory at the cost of disk access during monster
 * recall, which is optional to the user.
 *
 * This function should only be called with the cursor placed at the
 * left edge of the screen, on a cleared line, in which the recall is
 * to take place.  One extra blank line is left after the recall.
 */
void describe_monster(int r_idx, bool spoilers)
{
	monster_lore lore;

	/* Get the race and lore */
	const monster_race *r_ptr = &r_info[r_idx];
	monster_lore *l_ptr = &l_list[r_idx];


	/* Hack -- create a copy of the monster-memory */
	COPY(&lore, l_ptr, monster_lore);

	/* Assume some "obvious" flags */
	lore.flags1 |= (r_ptr->flags1 & RF1_OBVIOUS_MASK);


	/* Killing a monster reveals some properties */
	if (lore.tkills)
	{
		/* Know "race" flags */
		lore.flags3 |= (r_ptr->flags3 & RF3_RACE_MASK);

		/* Know "forced" flags */
		lore.flags1 |= (r_ptr->flags1 & (RF1_FORCE_DEPTH | RF1_FORCE_MAXHP));
	}

	/* Cheat -- know everything */
	if (cheat_know || spoilers) cheat_monster_lore(r_idx, &lore);

	/* Show kills of monster vs. player(s) */
	if (!spoilers && show_details)
		describe_monster_kills(r_idx, &lore);

	/* Monster description */
	if (spoilers || show_details)
		describe_monster_desc(r_idx);

	/* Describe the movement and level of the monster */
	describe_monster_movement(r_idx, &lore);

	/* Describe experience */
	if (!spoilers) describe_monster_exp(r_idx, &lore);

	/* Describe spells and innate attacks */
	describe_monster_spells(r_idx, &lore);

	/* Describe monster "toughness" */
	if (!spoilers) describe_monster_toughness(r_idx, &lore);

	/* Describe the abilities of the monster */
	describe_monster_abilities(r_idx, &lore);

	/* Describe the monster drop */
	describe_monster_drop(r_idx, &lore);

	/* Describe the known attacks */
	describe_monster_attack(r_idx, &lore);

	/* Notice "Quest" monsters */
	if (lore.flags1 & RF1_QUESTOR)
	{
#ifdef JP
		text_out("���Ȃ��͂��̃����X�^�[���E�������Ƃ��������~�]�������Ă���...");
#else /* JP */
		text_out("You feel an intense desire to kill this monster...  ");
#endif /* JP */
	}

	/* All done */
	text_out("\n");
}





/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
void roff_top(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	byte a1, a2;
	char c1, c2;
#ifdef JP
	byte a3;
	char c3;
#endif /* JP */


	/* Get the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Get the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

	/* A title (use "The" for non-uniques) */
#ifdef JP
	if (!(r_ptr->flags1 & RF1_UNIQUE) && (english_monster == TRUE))
#else /* JP */
	if (!(r_ptr->flags1 & RF1_UNIQUE))
#endif /* JP */
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}

	/* Dump the name */
#ifdef JP
	/* �p���؂�ւ��@�\�ɑΉ� */
	Term_addstr(-1, TERM_WHITE, X_r_name(r_ptr));
#else /* JP */
	Term_addstr(-1, TERM_WHITE, (r_name + r_ptr->name));
#endif /* JP */

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_addch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
#ifdef JP
	if (use_bigtile && (a2 & 0x80)) bigtile_attr(&c2, &a2, &c3, &a3);
#endif /* JP */
	Term_addch(a2, c2);
#ifdef JP
	if (use_bigtile && (a2 & 0x80)) Term_addch(a3, c3);
#else
	if (use_bigtile && (a2 & 0x80)) Term_addch(255, -1);
#endif /* JP */
	Term_addstr(-1, TERM_WHITE, "'):");
}



/*
 * Hack -- describe the given monster race at the top of the screen
 */
void screen_roff(int r_idx)
{
	/* Flush messages */
	message_flush();

	/* Begin recall */
	Term_erase(0, 1, 255);

	/* Output to the screen */
	text_out_hook = text_out_to_screen;

	/* Recall monster */
	describe_monster(r_idx, FALSE);

	/* Describe monster */
	roff_top(r_idx);
}




/*
 * Hack -- describe the given monster race in the current "term" window
 */
void display_roff(int r_idx)
{
	int y;

	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* Begin recall */
	Term_gotoxy(0, 1);

	/* Output to the screen */
	text_out_hook = text_out_to_screen;

	/* Recall monster */
	describe_monster(r_idx, FALSE);

	/* Describe monster */
	roff_top(r_idx);
}
