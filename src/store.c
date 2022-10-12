/* File: store.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/*
 * 2.7.9v3-v6 ���{��Ő���    : ���Ƃ���
 * �P�y�[�W�߂�R�}���h�̒ǉ� : BUG
 * sell_huggle() �̃o�O(?)���: FIRST
 * 2.8.0 �Ή�                 : sayu, ���Ƃ���
 * 2.8.1 �Ή�                 : FIRST
 * 2.8.3 �Ή�                 : FIRST, TeO, ���Ƃ���
 *
 * ���{��ŋ@�\�ǉ� : �䂪�Ƃ̊g��
 *                    �u���b�N�}�[�P�b�g�̒ǉ����b�Z�[�W
 */
/* 
 * �I���W�i���ł� 2.8.3 ���珤�i�̃��x����1�y�[�W�� a-l , 2�y�[�W�� m-x ��
 * ����U����悤�ɂȂ������A���{��łł͉B���I�v�V�����ŉ䂪�Ƃ� 20
 * �y�[�W�܂ő�������̂ŁA�ȑO�܂ł̃o�[�W�����Ɠ��l�ǂ̃y�[�W�� a-l ��
 * ����U�邱�Ƃɂ���
 * ���̊֌W�̃\�[�X�ύX�����́u���x���ύX�p�v�ƃR�����g���ӂ��Ă���
 */

#include "angband.h"

#include "script.h"


#ifdef JP
/* ���̕�����ړ����Ă��܂��� */
static int store_num = 0;
static int store_top = 0;
static store_type *st_ptr = NULL;
static owner_type *ot_ptr = NULL;

#define X_comment(m)  ((store_num != STORE_B_MARKET) ? (m) : (m ## _B))
#endif /* JP */


#define MAX_COMMENT_ACCEPT	6

static cptr comment_accept[MAX_COMMENT_ACCEPT] =
{
#ifdef JP
	"�I�[�P�[���B",
	"���\���B",
	"�������悤�I",
	"�^�����I",
	"�悵�I",
	"�킩�����I"
#else /* JP */
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
#endif /* JP */
};

#ifdef JP
/* �u���b�N�}�[�P�b�g�ǉ����b�Z�[�W�i�����j */
static cptr comment_accept_B[MAX_COMMENT_ACCEPT] =
{
	"�܂��A����ł�����B",
	"�����͂���Ŋ��ق��Ă��B",
	"����������B",
	"���傤���Ȃ��B",
	"����ŉ䖝�����B",
	"����Ȃ��񂾂낤�B"
};

#define X_comment_accept X_comment(comment_accept)

#endif /* JP */


#define MAX_COMMENT_INSULT	8

static cptr comment_insult[MAX_COMMENT_INSULT] =
{
#ifdef JP
	"�l�������Ă���B",
	"����Ⴈ�������I",
	"�����Ɛ^�ʖڂɌ����Ă���I",
	"������C������̂����H",
	"��₩���ɗ����̂��I",
	"������k���I",
	"�䖝����ׂ����B",
	"�Ӂ[�ށA�ǂ��V�C���B"
#else /* JP */
	"Try again.",
	"Ridiculous!",
	"You will have to do better than that!",
	"Do you wish to do business or not?",
	"You've got to be kidding!",
	"You'd better be kidding!",
	"You try my patience.",
	"Hmmm, nice weather we're having."
#endif /* JP */
};


#ifdef JP
/* �u���b�N�}�[�P�b�g�p�ǉ����b�Z�[�W�i�{��j */
static cptr comment_insult_B[MAX_COMMENT_INSULT] =
{
	"���Ԃ̖��ʂ��ȁA����́B",
	"���Ȃ��q�l���ȁI",
	"�b���ĕ����鑊�肶��Ȃ��������B",
	"�ɂ��ڂɂ��������炵���ȁI",
	"�Ȃ�ċ��~�ȓz���I",
	"�b�ɂȂ��y���I",
	"�ǂ����悤���Ȃ��n�R�l���I",
	"���܂𔄂��Ă���̂��H"
};

#define X_comment_insult X_comment(comment_insult)

#endif /* JP */


#define MAX_COMMENT_NONSENSE	4

static cptr comment_nonsense[MAX_COMMENT_NONSENSE] =
{
#ifdef JP
	"�ǂ���畷���ԈႦ���炵���B",
	"����A�悭�������Ȃ�������B",
	"���܂Ȃ��A�������āH",
	"�����A������x�����Ă����H"
#else /* JP */
	"I must have heard you wrong.",
	"I'm sorry, I missed that.",
	"I'm sorry, what was that?",
	"Sorry, what was that again?"
#endif /* JP */
};



/*
 * Successful haggle.
 */
static void say_comment_accept(void)
{
#ifdef JP
	message(MSG_STORE5, 0, X_comment_accept[rand_int(MAX_COMMENT_ACCEPT)]);
#else /* JP */
	message(MSG_STORE5, 0, comment_accept[rand_int(MAX_COMMENT_ACCEPT)]);
#endif /* JP */
}


/*
 * You are insulting me
 */
static void say_comment_insult(void)
{
#ifdef JP
	/* �u���b�N�}�[�P�b�g�̎��͕ʂ̃��b�Z�[�W���o�� */
	msg_print(X_comment_insult[rand_int(MAX_COMMENT_INSULT)]);
#else /* JP */
	msg_print(comment_insult[rand_int(MAX_COMMENT_INSULT)]);
#endif /* JP */
}


/*
 * You are making no sense.
 */
static void say_comment_nonsense(void)
{
	msg_print(comment_nonsense[rand_int(MAX_COMMENT_NONSENSE)]);
}



/*
 * Messages for reacting to purchase prices.
 */

#define MAX_COMMENT_WORTHLESS	4

static cptr comment_worthless[MAX_COMMENT_WORTHLESS] =
{
#ifdef JP
	"���킠�������I",
	"�Ȃ�Ă������I",
	"�N�����ނ��ы���������������...�B",
	"�X�傪�������ɂ�߂��Ă���I"
#else /* JP */
	"Arrgghh!",
	"You bastard!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
#endif /* JP */
};

#define MAX_COMMENT_BAD	4

static cptr comment_bad[MAX_COMMENT_BAD] =
{
#ifdef JP
	"�������I",
	"���̈����߁I",
	"�X�傪���߂������Ɍ��Ă���B",
	"�X�傪�ɂ�ł���B"
#else /* JP */
	"Damn!",
	"You fiend!",
	"The shopkeeper curses at you.",
	"The shopkeeper glares at you."
#endif /* JP */
};

#define MAX_COMMENT_GOOD	4

static cptr comment_good[MAX_COMMENT_GOOD] =
{
#ifdef JP
	"���΂炵���I",
	"�N���V�g�Ɍ������I",
	"�X�傪�N�X�N�X�΂��Ă���B",
	"�X�傪�吺�ŏ΂��Ă���B"
#else /* JP */
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
#endif /* JP */
};

#define MAX_COMMENT_GREAT	4

static cptr comment_great[MAX_COMMENT_GREAT] =
{
#ifdef JP
	"����ق��I",
	"����Ȃ��������v����������A�^�ʖڂɓ����Ȃ��Ȃ�Ȃ��B",
	"�X��͊������Ē��ˉ���Ă���B",
	"�X��͖��ʂɏ΂݂��������Ă���B"
#else /* JP */
	"Yipee!",
	"I think I'll retire!",
	"The shopkeeper jumps for joy.",
	"The shopkeeper smiles gleefully."
#endif /* JP */
};


/*
 * Let a shop-keeper React to a purchase
 *
 * We paid "price", it was worth "value", and we thought it was worth "guess"
 */
static void purchase_analyze(s32b price, s32b value, s32b guess)
{
	/* Item was worthless, but we bought it */
	if ((value <= 0) && (price > value))
	{
		/* Comment */
		message(MSG_STORE1, 0, comment_worthless[rand_int(MAX_COMMENT_WORTHLESS)]);
	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if ((value < guess) && (price > value))
	{
		/* Comment */
		message(MSG_STORE2, 0, comment_bad[rand_int(MAX_COMMENT_BAD)]);
	}

	/* Item was a good bargain, and we got away with it */
	else if ((value > guess) && (value < (4 * guess)) && (price < value))
	{
		/* Comment */
		message(MSG_STORE3, 0, comment_good[rand_int(MAX_COMMENT_GOOD)]);
	}

	/* Item was a great bargain, and we got away with it */
	else if ((value > guess) && (price < value))
	{
		/* Comment */
		message(MSG_STORE4, 0, comment_great[rand_int(MAX_COMMENT_GREAT)]);
	}
}





#ifndef JP
/* ���{��ł̏ꍇ�͏�̕��Ɉړ����Ă���܂� */

/*
 * We store the current "store number" here so everyone can access it
 */
static int store_num = 7;

/*
 * We store the current "store page" here so everyone can access it
 */
static int store_top = 0;

/*
 * We store the current "store pointer" here so everyone can access it
 */
static store_type *st_ptr = NULL;

/*
 * We store the current "owner type" here so everyone can access it
 */
static owner_type *ot_ptr = NULL;

#endif /* JP */





/*
 * Determine the price of an object (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * object, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
static s32b price_item(const object_type *o_ptr, int greed, bool flip)
{
	int factor;
	int adjust;
	s32b price;


	/* Get the value of one of the items */
	price = object_value(o_ptr);

	/* Worthless items */
	if (price <= 0) return (0L);


	/* Compute the racial factor */
	factor = g_info[(ot_ptr->owner_race * z_info->p_max) + p_ptr->prace];

	/* Add in the charisma factor */
	factor += adj_chr_gold[p_ptr->stat_ind[A_CHR]];


	/* Shop is buying */
	if (flip)
	{
		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (store_num == STORE_B_MARKET) price = price / 2;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (store_num == STORE_B_MARKET) price = price * 2;
	}

	/* Compute the final price (with rounding) */
	price = (price * adjust + 50L) / 100L;

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Special "mass production" computation.
 */
static int mass_roll(int num, int max)
{
	int i, t = 0;
	for (i = 0; i < num; i++)
	{
		t += ((max > 1) ? rand_int(max) : 1);
	}
	return (t);
}


/*
 * Certain "cheap" objects should be created in "piles".
 *
 * Some objects can be sold at a "discount" (in smaller piles).
 *
 * Standard percentage discounts include 10, 25, 50, 75, and 90.
 */
static void mass_produce(object_type *o_ptr)
{
	int size = 1;

	int discount = 0;

	s32b cost = object_value(o_ptr);


	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Food, Flasks, and Lites */
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		{
			if (cost <= 5L) size += mass_roll(3, 5);
			if (cost <= 20L) size += mass_roll(3, 5);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			if (cost <= 60L) size += mass_roll(3, 5);
			if (cost <= 240L) size += mass_roll(1, 5);
			break;
		}

		case TV_MAGIC_BOOK:
		case TV_PRAYER_BOOK:
		{
			if (cost <= 50L) size += mass_roll(2, 3);
			if (cost <= 500L) size += mass_roll(1, 3);
			break;
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SHIELD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_CLOAK:
		case TV_HELM:
		case TV_CROWN:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DIGGING:
		case TV_BOW:
		{
			if (o_ptr->name2) break;
			if (cost <= 10L) size += mass_roll(3, 5);
			if (cost <= 100L) size += mass_roll(3, 5);
			break;
		}

		case TV_SPIKE:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (cost <= 5L) size += mass_roll(5, 5);
			if (cost <= 50L) size += mass_roll(5, 5);
			if (cost <= 500L) size += mass_roll(5, 5);
			break;
		}
	}


	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (rand_int(25) == 0)
	{
		discount = 10;
	}
	else if (rand_int(50) == 0)
	{
		discount = 25;
	}
	else if (rand_int(150) == 0)
	{
		discount = 50;
	}
	else if (rand_int(300) == 0)
	{
		discount = 75;
	}
	else if (rand_int(500) == 0)
	{
		discount = 90;
	}


	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);

	/* Hack -- rods need to increase PVAL if stacked */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval = o_ptr->number * k_info[o_ptr->k_idx].pval;
	}
}



/*
 * Convert a store item index into a one character label
 *
 * We use labels "a"-"l" for page 1, and labels "m"-"x" for page 2.
 */
static s16b store_to_label(int i)
{
	/* Assume legal */
	return (I2A(i));
}


/*
 * Convert a one character label into a store item index.
 *
 * Return "-1" if the label does not indicate a real store item.
 */
static s16b label_to_store(int c)
{
	int i;

	/* Convert */
	i = (islower((unsigned char)c) ? A2I(c) : -1);

	/* Verify the index */
#ifdef JP
	/* ���x���ύX�p */
	if ((i < 0) || (i >= (st_ptr->stock_num - store_top) )) return (-1);
#else /* JP */
	if ((i < 0) || (i >= st_ptr->stock_num)) return (-1);
#endif /* JP */

	/* Return the index */
	return (i);
}



/*
 * Determine if a store object can "absorb" another object.
 *
 * See "object_similar()" for the same function for the "player".
 *
 * This function can ignore many of the checks done for the player,
 * since stores (but not the home) only get objects under certain
 * restricted circumstances.
 */
static bool store_object_similar(const object_type *o_ptr, const object_type *j_ptr)
{
	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr) return (0);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);

	/* Different pvals cannot be stacked, except for wands, staves, or rods */
	if ((o_ptr->pval != j_ptr->pval) &&
	    (o_ptr->tval != TV_WAND) &&
	    (o_ptr->tval != TV_STAFF) &&
	    (o_ptr->tval != TV_ROD)) return (0);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h) return (0);
	if (o_ptr->to_d != j_ptr->to_d) return (0);
	if (o_ptr->to_a != j_ptr->to_a) return (0);

	/* Require identical "artifact" names */
	if (o_ptr->name1 != j_ptr->name1) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2 != j_ptr->name2) return (0);

	/* Hack -- Never stack "powerful" items */
	if (o_ptr->xtra1 || j_ptr->xtra1) return (0);

	/* Hack -- Never stack recharging items */
	if (o_ptr->timeout || j_ptr->timeout) return (0);

	/* Require many identical values */
	if (o_ptr->ac != j_ptr->ac) return (0);
	if (o_ptr->dd != j_ptr->dd) return (0);
	if (o_ptr->ds != j_ptr->ds) return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (0);

	/* Require matching "discount" fields */
	if (o_ptr->discount != j_ptr->discount) return (0);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store object to absorb another object
 */
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > 99) ? 99 : total;

	/*
	 * Hack -- if rods are stacking, add the pvals (maximum timeouts)
	 * and any charging timeouts together
	 */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval += j_ptr->pval;
		o_ptr->timeout += j_ptr->timeout;
	}

	/* Hack -- if wands/staves are stacking, combine the charges */
	if ((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_STAFF))
	{
		o_ptr->pval += j_ptr->pval;
	}
}


/*
 * Check to see if the shop will be carrying too many objects
 *
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.  Before, one could "nuke" objects this way, by
 * adding them to a pile which was already full.
 */
static bool store_check_num(const object_type *o_ptr)
{
	int i;
	object_type *j_ptr;

#ifdef JP
	/*
	 * �I�v�V���� powerup_home ���ݒ肳��Ă����
	 * �䂪�Ƃ� 20 �y�[�W�܂Ŏg����
	 */
	/* Free space is always usable */
	if ((store_num == STORE_HOME) && (powerup_home == FALSE))
	{
		if (st_ptr->stock_num < ((st_ptr->stock_size) / 10))
			return TRUE;
	}
	else
	{
		if (st_ptr->stock_num < st_ptr->stock_size)
			return TRUE;
	}
#else /* JP */
	/* Free space is always usable */
	if (st_ptr->stock_num < st_ptr->stock_size) return TRUE;
#endif /* JP */

	/* The "home" acts like the player */
	if (store_num == STORE_HOME)
	{
		/* Check all the objects */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing object */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the objects */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing object */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* But there was no room at the inn... */
	return (FALSE);
}


/*
 * Determine if the current store will purchase the given object
 */
static bool store_will_buy_tester(const object_type *o_ptr)
{
	return store_will_buy(store_num, o_ptr);
}


/*
 * Add an object to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed.
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" bacoming
 * known, the player may have to pick stuff up and drop it again.
 */
static int home_carry(object_type *o_ptr)
{
	int i, slot;
	s32b value, j_value;
	object_type *j_ptr;


	/* Check each existing object (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing object */
		j_ptr = &st_ptr->stock[slot];

		/* The home acts just like the player */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

#ifdef JP
	/*
	 * �B���@�\: �I�v�V���� powerup_home ���ݒ肳��Ă����
	 *           �䂪�Ƃ� 20 �y�[�W�܂Ŏg����
	 */
	/* No space? */
	if ( powerup_home == TRUE)
	{
		if (st_ptr->stock_num >= st_ptr->stock_size)
			return (-1);
	}
	else
	{
		if (st_ptr->stock_num >= ((st_ptr->stock_size) / 10))
			return (-1);
	}
#else /* JP */
	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size) return (-1);
#endif /* JP */


	/* Determine the "value" of the object */
	value = object_value(o_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that object */
		j_ptr = &st_ptr->stock[slot];

		/* Hack -- readable books always come first */
		if ((o_ptr->tval == cp_ptr->spell_book) &&
		    (j_ptr->tval != cp_ptr->spell_book)) break;
		if ((j_ptr->tval == cp_ptr->spell_book) &&
		    (o_ptr->tval != cp_ptr->spell_book)) continue;

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Can happen in the home */
		if (!object_aware_p(o_ptr)) continue;
		if (!object_aware_p(j_ptr)) break;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/* Objects in the home can be unknown */
		if (!object_known_p(o_ptr)) continue;
		if (!object_known_p(j_ptr)) break;

		/* Objects sort by decreasing value */
		j_value = object_value(j_ptr);
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		/* Hack -- slide the objects */
		object_copy(&st_ptr->stock[i], &st_ptr->stock[i-1]);
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Hack -- Insert the new object */
	object_copy(&st_ptr->stock[slot], o_ptr);

	/* Return the location */
	return (slot);
}


/*
 * Add an object to a real stores inventory.
 *
 * If the object is "worthless", it is thrown away (except in the home).
 *
 * If the object cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.  Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an object to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
static int store_carry(object_type *o_ptr)
{
	int i, slot;
	s32b value, j_value;
	object_type *j_ptr;


	/* Evaluate the object */
	value = object_value(o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	if (value <= 0) return (-1);

	/* Erase the inscription */
	o_ptr->note = 0;

	/* Remove special inscription, if any */
	if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;


	/* Check each existing object (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing object */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* Absorb (some of) the object */
			store_object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size) return (-1);


	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that object */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/* Evaluate that slot */
		j_value = object_value(j_ptr);

		/* Objects sort by decreasing value */
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		/* Hack -- slide the objects */
		object_copy(&st_ptr->stock[i], &st_ptr->stock[i-1]);
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Hack -- Insert the new object */
	object_copy(&st_ptr->stock[slot], o_ptr);

	/* Return the location */
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.  This can result in zero items.
 */
static void store_item_increase(int item, int num)
{
	int cnt;
	object_type *o_ptr;

	/* Get the object */
	o_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = o_ptr->number + num;
	if (cnt > 255) cnt = 255;
	else if (cnt < 0) cnt = 0;
	num = cnt - o_ptr->number;

	/* Save the new number */
	o_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(int item)
{
	int j;
	object_type *o_ptr;

	/* Get the object */
	o_ptr = &st_ptr->stock[item];

	/* Must exist */
	if (!o_ptr->k_idx) return;

	/* Must have no items */
	if (o_ptr->number) return;

	/* One less object */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
	{
		st_ptr->stock[j] = st_ptr->stock[j + 1];
	}

	/* Nuke the final slot */
	object_wipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any object that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
static bool black_market_crap(const object_type *o_ptr)
{
	int i, j;

	/* Ego items are never crap */
	if (o_ptr->name2) return (FALSE);

	/* Good items are never crap */
	if (o_ptr->to_a > 0) return (FALSE);
	if (o_ptr->to_h > 0) return (FALSE);
	if (o_ptr->to_d > 0) return (FALSE);

	/* Check the other stores */
	for (i = 0; i < MAX_STORES; i++)
	{
		/* Skip home and black market */
		if (i == STORE_B_MARKET || i == STORE_HOME)
		  continue;

		/* Check every object in the store */
		for (j = 0; j < store[i].stock_num; j++)
		{
			object_type *j_ptr = &store[i].stock[j];

			/* Duplicate object "type", assume crappy */
			if (o_ptr->k_idx == j_ptr->k_idx) return (TRUE);
		}
	}

	/* Assume okay */
	return (FALSE);
}


/*
 * Attempt to delete (some of) a random object from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(void)
{
	int what, num;
	object_type *o_ptr;

	/* Paranoia */
	if (st_ptr->stock_num <= 0) return;

	/* Pick a random slot */
	what = rand_int(st_ptr->stock_num);

	/* Get the object */
	o_ptr = &st_ptr->stock[what];

	/* Determine how many objects are in the slot */
	num = o_ptr->number;

	/* Hack -- sometimes, only destroy half the objects */
	if (rand_int(100) < 50) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single object */
	if (rand_int(100) < 50) num = 1;

	/* Hack -- decrement the maximum timeouts and total charges of rods and wands. */
	if ((o_ptr->tval == TV_ROD) ||
	    (o_ptr->tval == TV_STAFF) ||
	    (o_ptr->tval == TV_WAND))
	{
		o_ptr->pval -= num * o_ptr->pval / o_ptr->number;
	}

	/* Actually destroy (part of) the object */
	store_item_increase(what, -num);
	store_item_optimize(what);
}


/*
 * Creates a random object and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given object?
 */
static void store_create(void)
{
	int k_idx, tries, level;

	object_type *i_ptr;
	object_type object_type_body;


	/* Paranoia -- no room left */
	if (st_ptr->stock_num >= st_ptr->stock_size) return;


	/* Hack -- consider up to four items */
	for (tries = 0; tries < 4; tries++)
	{
		/* Black Market */
		if (store_num == STORE_B_MARKET)
		{
			/* Pick a level for object/magic */
			level = 25 + rand_int(25);

			/* Random object kind (usually of given level) */
			k_idx = get_obj_num(level);

			/* Handle failure */
			if (!k_idx) continue;
		}

		/* Normal Store */
		else
		{
			/* Hack -- Pick an object kind to sell */
			k_idx = get_store_choice(store_num);

			/* Hack -- fake level for apply_magic() */
			level = rand_range(1, STORE_OBJ_LEVEL);
		}


		/* Get local object */
		i_ptr = &object_type_body;

		/* Create a new object of the chosen kind */
		object_prep(i_ptr, k_idx);

		/* Apply some "low-level" magic (no artifacts) */
		apply_magic(i_ptr, level, FALSE, FALSE, FALSE);

		/* Hack -- Charge lite's */
		if (i_ptr->tval == TV_LITE)
		{
			if (i_ptr->sval == SV_LITE_TORCH) i_ptr->pval = FUEL_TORCH / 2;
			if (i_ptr->sval == SV_LITE_LANTERN) i_ptr->pval = FUEL_LAMP / 2;
		}


		/* The object is "known" */
		object_known(i_ptr);

		/* Item belongs to a store */
		i_ptr->ident |= IDENT_STORE;

		/* Mega-Hack -- no chests in stores */
		if (i_ptr->tval == TV_CHEST) continue;

		/* Prune the black market */
		if (store_num == STORE_B_MARKET)
		{
			/* Hack -- No "crappy" items */
			if (black_market_crap(i_ptr)) continue;

			/* Hack -- No "cheap" items */
			if (object_value(i_ptr) < 10) continue;
		}

		/* Prune normal stores */
		else
		{
			/* No "worthless" items */
			if (object_value(i_ptr) <= 0) continue;
		}


		/* Mass produce and/or Apply discount */
		mass_produce(i_ptr);

		/* Attempt to carry the (known) object */
		(void)store_carry(i_ptr);

		/* Definitely done */
		break;
	}
}


/*
 * Redisplay a single store entry
 */
static void display_entry(int item)
{
	int y;
	object_type *o_ptr;
	s32b x;

	char o_name[80];
	char out_val[160];
	int maxwid;


	/* Must be on current "page" to get displayed */
	if (!((item >= store_top) && (item < store_top + 12))) return;


	/* Get the object */
	o_ptr = &st_ptr->stock[item];

	/* Get the row */
	y = (item % 12) + 6;

	/* Label it, clear the line --(-- */
#ifdef JP
	/* ���x���ύX�p */
	sprintf(out_val, "%c) ", store_to_label( (item % 12) ));
#else /* JP */
	sprintf(out_val, "%c) ", store_to_label(item));
#endif /* JP */
	prt(out_val, y, 0);

	/* Describe an object in the home */
	if (store_num == STORE_HOME)
	{
		byte attr;

		maxwid = 75;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 10;

		/* Describe the object */
		object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		/* Get inventory color */
		attr = tval_to_attr[o_ptr->tval & 0x7F];

		/* Display the object */
		c_put_str(attr, o_name, y, 3);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of a single object */
			int wgt = o_ptr->weight;
#ifdef JP
			sprintf(out_val, "%3d.%02d kg", lbtokg1(wgt), lbtokg2(wgt));
			put_str(out_val, y, 67);
#else /* JP */
			sprintf(out_val, "%3d.%d lb", wgt / 10, wgt % 10);
			put_str(out_val, y, 68);
#endif /* JP */
		}
	}

	/* Describe an object (fully) in a store */
	else
	{
		byte attr;

		/* Must leave room for the "price" */
		maxwid = 65;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 7;

		/* Describe the object (fully) */
		object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		/* Get inventory color */
		attr = tval_to_attr[o_ptr->tval & 0x7F];

		/* Display the object */
		c_put_str(attr, o_name, y, 3);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of a single object */
			int wgt = o_ptr->weight;
#ifdef JP
			sprintf(out_val, "%3d.%02d", lbtokg1(wgt), lbtokg2(wgt));
			put_str(out_val, y, 60);
#else /* JP */
			sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, y, 61);
#endif /* JP */
		}

		/* Extract the "minimum" price */
		x = price_item(o_ptr, ot_ptr->inflate, FALSE);

		/* Actually draw the price (with tax) */
		if (((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_STAFF)) &&
		    ((o_ptr->pval % o_ptr->number) > 0))
#ifdef JP
			sprintf(out_val, "%9ld avg", (long)x); /* mada */
#else /* JP */
			sprintf(out_val, "%9ld avg", (long)x);
#endif /* JP */
		else
			sprintf(out_val, "%9ld  ", (long)x);

		put_str(out_val, y, 68);
	}
}


/*
 * Display a store's inventory
 *
 * All prices are listed as "per individual object"
 */
static void display_inventory(void)
{
	int i, k;

	/* Display the next 12 items */
	for (k = 0; k < 12; k++)
	{
		/* Stop when we run out of items */
		if (store_top + k >= st_ptr->stock_num) break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < 13; i++) prt("", i + 6, 0);

	/* Assume "no current page" */
#ifdef JP
	put_str("          ", 5, 20);
#else /* JP */
	put_str("        ", 5, 20);
#endif /* JP */

	/* Visual reminder of "more items" */
	if (st_ptr->stock_num > 12)
	{
		/* Show "more" reminder (after the last object ) */
#ifdef JP
		prt("-����-", k + 6, 3);
#else /* JP */
		prt("-more-", k + 6, 3);
#endif /* JP */

		/* Indicate the "current page" */
#ifdef JP
		put_str(format("(%d�y�[�W)", store_top/12 + 1), 5, 20);
#else /* JP */
		put_str(format("(Page %d)", store_top/12 + 1), 5, 20);
#endif /* JP */
	}
}


/*
 * Display players gold
 */
static void store_prt_gold(void)
{
	char out_val[64];

#ifdef JP
	prt("�莝���̂���: ", 19, 53);
#else /* JP */
	prt("Gold Remaining: ", 19, 53);
#endif /* JP */

	sprintf(out_val, "%9ld", (long)p_ptr->au);
	prt(out_val, 19, 68);
}


/*
 * Display store (after clearing screen)
 */
static void display_store(void)
{
	char buf[80];


	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (store_num == STORE_HOME)
	{
		/* Put the owner name */
#ifdef JP
		put_str("�䂪��", 3, 31);
#else /* JP */
		put_str("Your Home", 3, 30);
#endif /* JP */

		/* Label the object descriptions */
#ifdef JP
		put_str("�A�C�e���̈ꗗ", 5, 4);
#else /* JP */
		put_str("Item Description", 5, 3);
#endif /* JP */

		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("�d��", 5, 72);
#else /* JP */
			put_str("Weight", 5, 70);
#endif /* JP */
		}
	}

	/* Normal stores */
	else
	{
#ifdef JP
		cptr store_name = X_f_name(&f_info[FEAT_SHOP_HEAD + store_num]);
		cptr owner_name = X_owner_name(ot_ptr);
		cptr race_name = X_p_name(&p_info[ot_ptr->owner_race]);
#else /* JP */
		cptr store_name = (f_name + f_info[FEAT_SHOP_HEAD + store_num].name);
		cptr owner_name = &(b_name[ot_ptr->owner_name]);
		cptr race_name = p_name + p_info[ot_ptr->owner_race].name;
#endif /* JP */

		/* Put the owner name and race */
		strnfmt(buf, sizeof(buf), "%s (%s)", owner_name, race_name);
		put_str(buf, 3, 10);

		/* Show the max price in the store (above prices) */
		strnfmt(buf, sizeof(buf), "%s (%ld)", store_name, (long)(ot_ptr->max_cost));
		prt(buf, 3, 50);

		/* Label the object descriptions */
#ifdef JP
		put_str("���i�̈ꗗ", 5, 7);
#else /* JP */
		put_str("Item Description", 5, 3);
#endif /* JP */

		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("�d��", 5, 62);
#else /* JP */
			put_str("Weight", 5, 60);
#endif /* JP */
		}

		/* Label the asking price (in stores) */
#ifdef JP
		put_str("���i", 5, 73);
#else /* JP */
		put_str("Price", 5, 72);
#endif /* JP */
	}

	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory();
}



/*
 * Get the index of a store object
 *
 * Return TRUE if an object was selected
 */
static bool get_stock(int *com_val, cptr pmt)
{
	int item;

#ifdef JP
	/* ���x���ύX�p */
	int i;
#endif /* JP */

	char which;

	char buf[160];

	char o_name[80];

	char out_val[160];

	object_type *o_ptr;

#ifdef ALLOW_REPEAT

	/* Get the item index */
	if (repeat_pull(com_val))
	{
		/* Verify the item */
		if ((*com_val >= 0) && (*com_val <= (st_ptr->stock_num - 1)))
		{
			/* Success */
			return (TRUE);
		}
		else
		{
			/* Invalid repeat - reset it */
			repeat_clear();
		}
	}

#endif /* ALLOW_REPEAT */

	/* Assume failure */
	*com_val = (-1);

#ifdef JP
	/* ���x���ύX�p */

	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

	/* Build the prompt */
	strnfmt(buf, sizeof(buf), "(%s:%c-%c, ESC�Œ��f) %s",
	        ((store_num == STORE_HOME) ? "�A�C�e��" : "���i"), 
	        store_to_label(0), store_to_label( i-1 ),
	        pmt);
#else /* JP */
	/* Build the prompt */
	strnfmt(buf, sizeof(buf), "(Items %c-%c, ESC to exit) %s",
	        store_to_label(0), store_to_label(st_ptr->stock_num - 1),
	        pmt);
#endif /* JP */

	/* Ask until done */
	while (TRUE)
	{
		bool verify;

		/* Escape */
		if (!get_com(buf, &which)) return (FALSE);

		/* Note verify */
		verify = (isupper((unsigned char)which) ? TRUE : FALSE);

		/* Lowercase */
		which = tolower((unsigned char)which);

		/* Convert response to item */
		item = label_to_store(which);

		/* Oops */
		if (item < 0)
		{
			/* Oops */
#ifdef JP
			bell("�����ȃR�}���h�ł��I");
#else /* JP */
			bell("Illegal store object choice!");
#endif /* JP */

			continue;
		}

		/* No verification */
		if (!verify) break;

		/* Object */
		o_ptr = &st_ptr->stock[item];

		/* Describe */
		object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);

		/* Prompt */
#ifdef JP
		strnfmt(out_val, sizeof(out_val), "�{���� %s �ł����H", o_name);
#else /* JP */
		strnfmt(out_val, sizeof(out_val), "Try %s? ", o_name);
#endif /* JP */

		/* Query */
		if (!get_check(out_val)) return (FALSE);

		/* Done */
		break;
	}

	/* Save item */
	(*com_val) = item;

#ifdef ALLOW_REPEAT

	repeat_push(*com_val);

#endif /* ALLOW_REPEAT */

	/* Success */
	return (TRUE);
}


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b *poffer, s32b price)
{
	/* Paranoia XXX XXX XXX */
	message_flush();


	/* Ask until done */
	while (TRUE)
	{
		cptr p;

		char out_val[80];

		/* Default */
		strcpy(out_val, "");

		/* Ask the user for a response */
		if (!get_string(pmt, out_val, sizeof(out_val))) return (FALSE);

		/* Skip leading spaces */
		for (p = out_val; *p == ' '; p++) /* loop */;

		/* Empty response */
		if (*p == '\0')
		{
			/* Accept current price */
			*poffer = price;
			break;
		}

		/* Normal response */
		else
		{
			s32b i;
			char *err_ptr;

			/* Extract a number */
			i = strtol(p, &err_ptr, 10);

			/* Valid number? */
			if (*err_ptr == '\0')
			{
				/* Use the given "number" */
				*poffer = i;
				break;
			}
		}

		/* Warning */
#ifdef JP
		msg_print("�l�����������ł��B");
#else /* JP */
		msg_print("Invalid response.");
#endif /* JP */
		message_flush();
	}

	/* Success */
	return (TRUE);
}


/*
 * Receive an offer (from the player)
 *
 * Return TRUE if offer is NOT okay
 */
static bool receive_offer(cptr pmt, s32b *poffer,
                          int factor, s32b price)
{
	/* Haggle till done */
	while (TRUE)
	{
		/* Get a haggle (or cancel) */
		if (!get_haggle(pmt, poffer, price)) return (TRUE);

		/* Acceptable offer */
		if (((*poffer) * factor) >= (price * factor)) break;

		/* Display and flush insult */
		say_comment_insult();

		/* Reject offer (correctly) */
		(*poffer) = price;
	}

	/* Success */
	return (FALSE);
}


/*
 * Haggling routine
 *
 * Return TRUE if purchase is successful
 */
static bool purchase_haggle(object_type *o_ptr, s32b *price)
{
	s32b ask_price;
	s32b offer;
	int flag, loop_flag;

	bool cancel = FALSE;

#ifdef JP
	cptr pmt = "�ŏI�񎦉��i";
#else /* JP */
	cptr pmt = "Final Offer";
#endif /* JP */

	char out_val[160];


	*price = 0;


	/* Extract the asking price */
	ask_price = price_item(o_ptr, ot_ptr->inflate, FALSE);

	/* Message summary */
#ifdef JP
	msg_print("����Ȃ�Ƃ��̋��z�ɂ܂Ƃ܂����B");
#else /* JP */
	msg_print("You quickly agree upon the price.");
#endif /* JP */
	message_flush();

	/* Haggle for the whole pile */
	ask_price *= o_ptr->number;


	/* No offer yet */
	offer = 0;

	/* Haggle until done */
	for (flag = FALSE; !flag; )
	{
		loop_flag = TRUE;

		while (!flag && loop_flag)
		{
			strnfmt(out_val, sizeof(out_val), "%s :  %ld", pmt, (long)ask_price);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer("�񎦂�����z? [����] ",
			                       &offer, 1, ask_price);
#else /* JP */
			cancel = receive_offer("What do you offer? [accept] ",
			                       &offer, 1, ask_price);
#endif /* JP */

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer > ask_price)
			{
				say_comment_nonsense();
			}
			else if (offer == ask_price)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}
		}

		if (!flag)
		{
			if (offer >= ask_price)
			{
				flag = TRUE;
				*price = offer;
			}
		}
	}

	/* Cancel */
	if (cancel) return (FALSE);

	/* Do not cancel */
	return (TRUE);
}


/*
 * Haggling routine
 *
 * Return TRUE if purchase is successful
 */
static bool sell_haggle(object_type *o_ptr, s32b *price)
{
	s32b purse, ask_price;
	s32b offer = 0;

	int flag, loop_flag;

	bool cancel = FALSE;

#ifdef JP
	cptr pmt = "�ŏI�񎦉��i";
#else /* JP */
	cptr pmt = "Final Offer";
#endif /* JP */

	char out_val[160];


	*price = 0;


	/* Obtain the asking price */
	ask_price = price_item(o_ptr, ot_ptr->inflate, TRUE);

	/* Get the owner's payout limit */
	purse = ot_ptr->max_cost;

	/* No reason to haggle */
	if (ask_price >= purse)
	{
		/* Message */
#ifdef JP
		msg_print("�����ɂ��̋��z�ɂ܂Ƃ܂����B");
#else /* JP */
		msg_print("You instantly agree upon the price.");
#endif /* JP */
		message_flush();

		/* Offer full purse */
		ask_price = purse;
	}

	/* No haggle option */
	else
	{
		/* Message summary */
#ifdef JP
		msg_print("����Ȃ�Ƃ��̋��z�ɂ܂Ƃ܂����B");
#else /* JP */
		msg_print("You quickly agree upon the price.");
#endif /* JP */
		message_flush();
	}

	/* Haggle for the whole pile */
	ask_price *= o_ptr->number;


	/* Display commands XXX XXX XXX */

	/* No offer yet */
	offer = 0;

	/* Haggle */
	for (flag = FALSE; !flag; )
	{
		while (1)
		{
			loop_flag = TRUE;

			strnfmt(out_val, sizeof(out_val), "%s :  %ld", pmt, (long)ask_price);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer("�񎦂��鉿�i? ",
			                       &offer, -1, ask_price);
#else /* JP */
			cancel = receive_offer("What price do you ask? ",
			                       &offer, -1, ask_price);
#endif /* JP */

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer < ask_price)
			{
				say_comment_nonsense();
			}
			else if (offer == ask_price)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}

			/* Stop */
			if (flag || !loop_flag) break;
		}

		if (!flag)
		{
			if (offer <= ask_price)
			{
				flag = TRUE;
				*price = offer;
			}
		}
	}

	/* Cancel */
	if (cancel) return (FALSE);

	/* Do not cancel */
	return (TRUE);
}





/*
 * Buy an object from a store
 */
static void store_purchase(void)
{
	int n;
	int amt;
	int item, item_new;

	s32b price;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	char o_name[80];

	char out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (store_num == STORE_HOME)
		{
#ifdef JP
			msg_print("�䂪�Ƃɂ͉����u���Ă���܂���B");
#else /* JP */
			msg_print("Your home is empty.");
#endif /* JP */
		}
		else
		{
#ifdef JP
			msg_print("���ݏ��i�̍݌ɂ�؂炵�Ă��܂��B");
#else /* JP */
			msg_print("I am currently out of stock.");
#endif /* JP */
		}
		return;
	}


	/* Prompt */
#ifdef JP
	/* �u���b�N�}�[�P�b�g�̎��͕ʂ̃��b�Z�[�W */
	switch( store_num )
	{
		case STORE_HOME:
			sprintf(out_val, "�ǂ̃A�C�e�������܂���? ");
			break;
		case STORE_B_MARKET:
			sprintf(out_val, "�ǂ�? ");
			break;
		default:
			sprintf(out_val, "�ǂ̕i�����~�����񂾂�? ");
			break;
	}
#else /* JP */
	if (store_num == STORE_HOME)
	{
		sprintf(out_val, "Which item do you want to take? ");
	}
	else
	{
		sprintf(out_val, "Which item are you interested in? ");
	}
#endif /* JP */

	/* Get the object number to be bought */
	if (!get_stock(&item, out_val)) return;

#ifdef JP
	/* ���x���ύX�p */
	/* Get the actual index */
	item = item + store_top;
#endif /* JP */

	/* Get the actual object */
	o_ptr = &st_ptr->stock[item];

	/* Get a quantity */
	amt = get_quantity(NULL, o_ptr->number);

	/* Allow user abort */
	if (amt <= 0) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Get desired object */
	object_copy(i_ptr, o_ptr);

	/*
	 * Hack -- If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(i_ptr, i_ptr->number - amt);

	/* Modify quantity */
	i_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(i_ptr))
	{
#ifdef JP
		msg_print("�U�b�N�ɂ��̃A�C�e�������錄�Ԃ��Ȃ��B");
#else /* JP */
		msg_print("You cannot carry that many items.");
#endif /* JP */
		return;
	}

	/* Attempt to buy it */
	if (store_num != STORE_HOME)
	{
		/* Describe the object (fully) */
		object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

		/* Message */
#ifdef JP
		msg_format("%s(%c)���w������B",
#else /* JP */
		msg_format("Buying %s (%c).",
#endif /* JP */
		           o_name, store_to_label(item));
		message_flush();

		/* Haggle for a final price */
		if (purchase_haggle(i_ptr, &price))
		{
			/* Player can afford it */
			if (p_ptr->au >= price)
			{
				/* Say "okay" */
				say_comment_accept();

				/* Spend the money */
				p_ptr->au -= price;

				/* Update the display */
				store_prt_gold();

				/* Buying an object makes you aware of it */
				object_aware(i_ptr);

				/* Combine / Reorder the pack (later) */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				/* The object no longer belongs to the store */
				i_ptr->ident &= ~(IDENT_STORE);

				/* Describe the transaction */
				object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

				/* Message */
#ifdef JP
				msg_format("%s(%c)�� $%ld �Ŕ������B",
#else /* JP */
				msg_format("You bought %s (%c) for %ld gold.",
#endif /* JP */
				           o_name, store_to_label(item),
				           (long)price);

				/* Erase the inscription */
				i_ptr->note = 0;

				/* Remove special inscription, if any */
				if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

				/* Give it to the player */
				item_new = inven_carry(i_ptr);

				/* Describe the final result */
				object_desc(o_name, sizeof(o_name), &inventory[item_new], TRUE, 3);

				/* Message */
#ifdef JP
				msg_format("%s(%c)�������Ă���B",
#else /* JP */
				msg_format("You have %s (%c).",
#endif /* JP */
				           o_name, index_to_label(item_new));

				/* Now, reduce the original stack's pval */
				if ((o_ptr->tval == TV_ROD) ||
				    (o_ptr->tval == TV_WAND) ||
				    (o_ptr->tval == TV_STAFF))
				{
					o_ptr->pval -= i_ptr->pval;
				}

				/* Handle stuff */
				handle_stuff();

				/* Note how many slots the store used to have */
				n = st_ptr->stock_num;

				/* Remove the bought objects from the store */
				store_item_increase(item, -amt);
				store_item_optimize(item);

				/* Store is empty */
				if (st_ptr->stock_num == 0)
				{
					int i;

					/* Shuffle */
					if (rand_int(STORE_SHUFFLE) == 0)
					{
						/* Message */
#ifdef JP
						msg_print("�X��͈��ނ����B");
#else /* JP */
						msg_print("The shopkeeper retires.");
#endif /* JP */

						/* Shuffle the store */
						store_shuffle(store_num);
					}

					/* Maintain */
					else
					{
						/* Message */
#ifdef JP
						msg_print("�X��͐V���ȍ݌ɂ����o�����B");
#else /* JP */
						msg_print("The shopkeeper brings out some new stock.");
#endif /* JP */
					}

					/* New inventory */
					for (i = 0; i < 10; ++i)
					{
						/* Maintain the store */
						store_maint(store_num);
					}

					/* Start over */
					store_top = 0;

					/* Redraw everything */
					display_inventory();
				}

				/* The object is gone */
				else if (st_ptr->stock_num != n)
				{
					/* Only one screen left */
					if (st_ptr->stock_num <= 12)
					{
						store_top = 0;
					}

					/* Redraw everything */
					display_inventory();
				}

				/* The object is still here */
				else
				{
					/* Redraw the object */
					display_entry(item);
				}
			}

			/* Player cannot afford it */
			else
			{
				/* Simple message (no insult) */
#ifdef JP
				msg_print("����������܂���B");
#else /* JP */
				msg_print("You do not have enough gold.");
#endif /* JP */
			}
		}
	}

	/* Home is much easier */
	else
	{
		/* Distribute charges of wands, staves, or rods */
		distribute_charges(o_ptr, i_ptr, amt);

		/* Give it to the player */
		item_new = inven_carry(i_ptr);

		/* Describe just the result */
		object_desc(o_name, sizeof(o_name), &inventory[item_new], TRUE, 3);

		/* Message */
#ifdef JP
		msg_format("%s(%c)��������B", o_name, index_to_label(item_new));
#else /* JP */
		msg_format("You have %s (%c).", o_name, index_to_label(item_new));
#endif /* JP */

		/* Handle stuff */
		handle_stuff();

		/* Take note if we take the last one */
		n = st_ptr->stock_num;

		/* Remove the items from the home */
		store_item_increase(item, -amt);
		store_item_optimize(item);

		/* The object is gone */
		if (st_ptr->stock_num != n)
		{
			/* Only one screen left */
			if (st_ptr->stock_num <= 12)
			{
				store_top = 0;
			}

			/* Redraw everything */
			display_inventory();
		}

		/* The object is still here */
		else
		{
			/* Redraw the object */
			display_entry(item);
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an object to the store (or home)
 */
static void store_sell(void)
{
	int item, item_pos;
	int amt;

	s32b price, value, dummy;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	cptr q, s;

	char o_name[120];


	/* Home */
#ifdef JP
	q = "�ǂ̃A�C�e����u���܂���? ";
#else /* JP */
	q = "Drop which item? ";
#endif /* JP */

	/* Real store */
	if (store_num != STORE_HOME)
	{
		/* New prompt */
#ifdef JP
		q = "�ǂ̃A�C�e���𔄂�܂���? ";
#else /* JP */
		q = "Sell which item? ";
#endif /* JP */

		/* Only allow items the store will buy */
		item_tester_hook = store_will_buy_tester;
	}

	/* Get an item */
#ifdef JP
	/* �䂪�Ƃł������ȃ��b�Z�[�W���o��I���W�i���̃o�O���C�� */
	s = (store_num == STORE_HOME) ? "�u����A�C�e���������Ă��܂���B"
	                              : "�~���������Ȃ��ł��˂��B";
#else /* JP */
	s = "You have nothing that I want.";
#endif /* JP */
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

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


	/* Hack -- Cannot remove cursed objects */
	if ((item >= INVEN_WIELD) && cursed_p(o_ptr))
	{
		/* Oops */
#ifdef JP
		msg_print("�Ӂ[�ށA�ǂ���炻��͎���Ă���悤���ˁB");
#else /* JP */
		msg_print("Hmmm, it seems to be cursed.");
#endif /* JP */

		/* Nope */
		return;
	}

	/* Get a quantity */
	amt = get_quantity(NULL, o_ptr->number);

	/* Allow user abort */
	if (amt <= 0) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Get a copy of the object */
	object_copy(i_ptr, o_ptr);

	/* Modify quantity */
	i_ptr->number = amt;

	/* Hack -- If a rod, wand, or staff, allocate total maximum
	 * timeouts or charges to those being sold.
	 */
	if ((o_ptr->tval == TV_ROD) ||
	    (o_ptr->tval == TV_WAND) ||
	    (o_ptr->tval == TV_STAFF))
	{
		i_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Get a full description */
	object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);


	/* Is there room in the store (or the home?) */
	if (!store_check_num(i_ptr))
	{
		if (store_num == STORE_HOME)
		{
#ifdef JP
			msg_print("�䂪�Ƃɂ͂����u���ꏊ���Ȃ��B");
#else /* JP */
			msg_print("Your home is full.");
#endif /* JP */
		}
		else
		{
#ifdef JP
			msg_print("�����܂��񂪁A�X�ɂ͂����u���ꏊ������܂���B");
#else /* JP */
			msg_print("I have not the room in my store to keep it.");
#endif /* JP */
		}
		return;
	}


	/* Real store */
	if (store_num != STORE_HOME)
	{
		/* Describe the transaction */
#ifdef JP
		msg_format("%s(%c)�𔄋p����B", o_name, index_to_label(item));
#else /* JP */
		msg_format("Selling %s (%c).", o_name, index_to_label(item));
#endif /* JP */
		message_flush();

		/* Haggle for it */
		if (sell_haggle(i_ptr, &price))
		{
			/* Say "okay" */
			say_comment_accept();

			/* Get some money */
			p_ptr->au += price;

			/* Update the display */
			store_prt_gold();

			/* Get the "apparent" value */
			dummy = object_value(i_ptr) * i_ptr->number;

			/* Erase the inscription */
			i_ptr->note = 0;

			/* Remove special inscription, if any */
			if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

			/* Identify original object */
			object_aware(o_ptr);
			object_known(o_ptr);

			/* Combine / Reorder the pack (later) */
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

			p_ptr->redraw |= (PR_EQUIPPY);

			/* Get local object */
			i_ptr = &object_type_body;

			/* Get a copy of the object */
			object_copy(i_ptr, o_ptr);

			/* Modify quantity */
			i_ptr->number = amt;

			/* The object belongs to the store now */
			i_ptr->ident |= IDENT_STORE;

			/*
			 * Hack -- Allocate charges between those wands, staves, or rods
			 * sold and retained, unless all are being sold.
			 */
			distribute_charges(o_ptr, i_ptr, amt);

			/* Get the "actual" value */
			value = object_value(i_ptr) * i_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

			/* Describe the result (in message buffer) */
#ifdef JP
			msg_format("%s(%c)�� $%ld �Ŕ������B",
#else /* JP */
			msg_format("You sold %s (%c) for %ld gold.",
#endif /* JP */
			           o_name, index_to_label(item), (long)price);

			/* Analyze the prices (and comment verbally) */
			purchase_analyze(price, value, dummy);

			/* Take the object from the player */
			inven_item_increase(item, -amt);
			inven_item_describe(item);
			inven_item_optimize(item);

			/* Handle stuff */
			handle_stuff();

			/* The store gets that (known) object */
			item_pos = store_carry(i_ptr);

			/* Update the display */
			if (item_pos >= 0)
			{
				/* Redisplay wares */
				display_inventory();
			}
		}
	}

	/* Player is at home */
	else
	{
		/* Distribute charges of wands/staves/rods */
		distribute_charges(o_ptr, i_ptr, amt);

		/* Describe */
#ifdef JP
		msg_format("%s��u�����B(%c)", o_name, index_to_label(item));
#else /* JP */
		msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif /* JP */

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(i_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			/* Redisplay wares */
			display_inventory();
		}
	}
}


/*
 * Examine an item in a store
 */
static void store_examine(void)
{
	int         item;
	object_type *o_ptr;
	char        out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (store_num == STORE_HOME)
		{
#ifdef JP
			msg_print("�䂪�Ƃɂ͉����u���Ă���܂���B");
#else /* JP */
			msg_print("Your home is empty.");
#endif /* JP */
		}
		else
		{
#ifdef JP
			msg_print("���ݏ��i�̍݌ɂ�؂炵�Ă��܂��B");
#else /* JP */
			msg_print("I am currently out of stock.");
#endif /* JP */
		}
		return;
	}


	/* Prompt */
#ifdef JP
	sprintf(out_val, "�ǂ̃A�C�e���𒲂ׂ܂���? ");
#else /* JP */
	if (rogue_like_commands)
		sprintf(out_val, "Which item do you want to examine? ");
	else
		sprintf(out_val, "Which item do you want to look at? ");
#endif /* JP */

	/* Get the item number to be examined */
	if (!get_stock(&item, out_val)) return;

#ifdef JP
	/* ���x���ύX�p */
	/* Get the actual index */
	item = item + store_top;
#endif /* JP */

	/* Get the actual object */
	o_ptr = &st_ptr->stock[item];

	/* Describe it fully */
	object_info_screen(o_ptr);
}



/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands
 * in the stores which are not allowed in the dungeon, and we
 * must disable some commands which are allowed in the dungeon
 * but not in the stores, to prevent chaos.
 *
 * Hack -- note the bizarre code to handle the "=" command,
 * which is needed to prevent the "redraw" from affecting
 * the display of the store.  XXX XXX XXX
 */
static void store_process_command(void)
{

#ifdef ALLOW_REPEAT

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT */

	/* Parse the command */
	switch (p_ptr->command_cmd)
	{
		/* Leave */
		case ESCAPE:
		{
			leave_store = TRUE;
			break;
		}

#ifdef JP
		/* ���{��Œǉ� */
		/* 1 �y�[�W�߂�R�}���h: �䂪�Ƃ̃y�[�W���������̂ŏd�󂷂�͂� By BUG */
		case '-':
		{
			if (st_ptr->stock_num <= 12)
			{
				msg_print("����őS���ł��B");
			}
			else
			{
				store_top -= 12;
				if (store_top < 0)
				{
					store_top = ((st_ptr->stock_num - 1 )/12) * 12;
				}
				if ((store_num == STORE_HOME) && (powerup_home == FALSE))
				{
					if ( store_top >= 12 ) store_top = 12;
				}
				display_inventory();
			}
			break;
		}
#endif /* JP */

		/* Browse */
		case ' ':
		{
			if (st_ptr->stock_num <= 12)
			{
				/* Nothing to see */
#ifdef JP
				msg_print("����őS���ł��B");
#else /* JP */
				msg_print("Entire inventory is shown.");
#endif /* JP */
			}

#ifdef JP
			else
			{
				store_top += 12;
				/*
				 * �B���I�v�V����(powerup_home)���Z�b�g����Ă��Ȃ��Ƃ���
				 * �䂪�Ƃł� 2 �y�[�W�܂ł����\�����Ȃ�
				 */
				if ((store_num == STORE_HOME) && 
				    (powerup_home == FALSE) && 
					(st_ptr->stock_num >= STORE_INVEN_MAX))
				{
					if (store_top >= (STORE_INVEN_MAX - 1))
						store_top = 0;
				}
				else
				{
					if (store_top >= st_ptr->stock_num)
						store_top = 0;
				}
				display_inventory();
			}
#else /* JP */
			else if (store_top == 0)
			{
				/* Page 2 */
				store_top = 12;

				/* Redisplay wares */
				display_inventory();
			}

			else
			{
				/* Page 1 */
				store_top = 0;

				/* Redisplay wares */
				display_inventory();
			}
#endif /* JP */

			break;
		}

		/* Ignore */
		case '\n':
		case '\r':
		{
			break;
		}


		/* Redraw */
		case KTRL('R'):
		{
			do_cmd_redraw();
			display_store();
			break;
		}

		/* Get (purchase) */
		case 'g':
		{
			store_purchase();
			break;
		}

		/* Drop (Sell) */
		case 'd':
		{
			store_sell();
			break;
		}

		/* Examine */
		case 'l':
		{
			store_examine();
			break;
		}


		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			do_cmd_wield();
			break;
		}

		/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('E'):
		{
			toggle_inven_equip();
			break;
		}



		/*** Use various objects ***/

		/* Browse a book */
		case 'b':
		{
			do_cmd_browse();
			break;
		}

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			do_cmd_redraw();
			display_store();
			break;
		}


		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages();
			break;
		}

		/* Check knowledge */
		case '~':
		case '|':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}


		/* Hack -- Unknown command */
		default:
		{
#ifdef JP
			msg_print("���̃R�}���h�͓X�̒��ł͎g���܂���B");
#else /* JP */
			msg_print("That command does not work in stores.");
#endif /* JP */
			break;
		}
	}
}


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "p_ptr->command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
void do_cmd_store(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int which;

	int tmp_chr;


	/* Verify a store */
	if (!((cave_feat[py][px] >= FEAT_SHOP_HEAD) &&
	      (cave_feat[py][px] <= FEAT_SHOP_TAIL)))
	{
#ifdef JP
		msg_print("�����ɂ͓X������܂���B");
#else /* JP */
		msg_print("You see no store here.");
#endif /* JP */
		return;
	}

	/* Hack -- Extract the store code */
	which = (cave_feat[py][px] - FEAT_SHOP_HEAD);

	/* Hack -- Check the "locked doors" */
	if (adult_no_stores)
	{
#ifdef JP
		msg_print("�h�A�Ɍ����������Ă���B");
#else /* JP */
		msg_print("The doors are locked.");
#endif /* JP */
		return;
	}


	/* Forget the view */
	forget_view();


	/* Hack -- Increase "icky" depth */
	character_icky++;


	/* No command argument */
	p_ptr->command_arg = 0;

	/* No repeated command */
	p_ptr->command_rep = 0;

	/* No automatic command */
	p_ptr->command_new = 0;


	/* Save the store number */
	store_num = which;

	/* Save the store and owner pointers */
	st_ptr = &store[store_num];
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];


	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Do not leave */
	leave_store = FALSE;

	/*
	 * Play a special sound if entering the 
	 * home, otherwise play a generic store sound.
	 */
	if (store_num == STORE_HOME)
		sound(MSG_STORE_HOME);
	else
		sound(MSG_STORE_ENTER);


	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Hack -- Check the charisma */
		tmp_chr = p_ptr->stat_use[A_CHR];

		/* Clear */
		clear_from(21);

/* ��ʕ\���̗�
 ESC) ��������o��        g) �A�C�e�������(get)        i/e)������/�����̈ꗗ
 �X�y�[�X) ���y�[�W       d) �A�C�e����u��(drop)       w/T)��������/�͂���
 ESC) ��������o��           p) ����(purchase)          i/e)������/�����̈ꗗ
 �X�y�[�X) ���y�[�W          s) ����(sell)              w/T)��������/�͂���
*/

		/* Basic commands */
#ifdef JP
		prt(" ESC) ��������o��", 22, 0);
#else /* JP */
		prt(" ESC) Exit from Building.", 22, 0);
#endif /* JP */

		/* Browse if necessary */
		if (st_ptr->stock_num > 12)
		{
#ifdef JP
			prt(" �X�y�[�X) ���y�[�W", 23, 0);
#else /* JP */
			prt(" SPACE) Next page of stock.", 23, 0);
#endif /* JP */
		}

		/* Commands */
#ifdef JP
		if (store_num == 7)
		{
			prt(" g) �A�C�e�������(get)", 22, 29);
			prt(" d) �A�C�e����u��(drop)", 23, 29);
		}
		else
		{
			prt(" p) ����(purchase)", 22, 29);
			prt(" s) ����(sell)", 23, 29);
		}
#else /* JP */
		prt(" g) Get/Purchase an item.", 22, 29);
		prt(" d) Drop/Sell an item.", 23, 29);
#endif /* JP */

		/* Add in the eXamine option */
		if (rogue_like_commands)
#ifdef JP
			prt(" x) �A�C�e���𒲂ׂ�", 22, 56);
#else /* JP */
			prt(" x) eXamine an item.", 22, 56);
#endif /* JP */
		else
#ifdef JP
			prt(" l) �A�C�e���𒲂ׂ�", 22, 56);
#else /* JP */
			prt(" l) Look at an item.", 22, 56);
#endif /* JP */

		/* Prompt */
#ifdef JP
		prt("�R�}���h:", 21, 0);
#else /* JP */
		prt("You may: ", 21, 0);
#endif /* JP */

		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		store_process_command();

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* Pack Overflow XXX XXX XXX */
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &inventory[item];

			/* Hack -- Flee from the store */
			if (store_num != STORE_HOME)
			{
				/* Message */
#ifdef JP
				msg_print("�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂēX����o��...");
#else /* JP */
				msg_print("Your pack is so full that you flee the store...");
#endif /* JP */

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
#ifdef JP
				msg_print("�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂĉƂ���o��...");
#else /* JP */
				msg_print("Your pack is so full that you flee your home...");
#endif /* JP */

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type *i_ptr;
				object_type object_type_body;

				char o_name[80];


				/* Give a message */
#ifdef JP
				msg_print("�U�b�N����A�C�e�������ӂ�Ă��܂����I");
#else /* JP */
				msg_print("Your pack overflows!");
#endif /* JP */

				/* Get local object */
				i_ptr = &object_type_body;

				/* Grab a copy of the object */
				object_copy(i_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

				/* Message */
#ifdef JP
				msg_format("%s���������B(%c)", o_name, index_to_label(item));
#else /* JP */
				msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif /* JP */

				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(i_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					/* Redisplay wares */
					display_inventory();
				}
			}
		}

		/* Hack -- Handle charisma changes */
		if (tmp_chr != p_ptr->stat_use[A_CHR])
		{
			/* Redisplay wares */
			display_inventory();
		}
	}

	/* Leave the store sound */
	sound(MSG_STORE_LEAVE);


	/* Take a turn */
	p_ptr->energy_use = 100;


	/* Hack -- Cancel automatic command */
	p_ptr->command_new = 0;

	/* Hack -- Cancel "see" mode */
	p_ptr->command_see = FALSE;


	/* Flush messages XXX XXX XXX */
	message_flush();


	/* Hack -- Decrease "icky" depth */
	character_icky--;


	/* Clear the screen */
	Term_clear();


	/* Update the visuals */
	p_ptr->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_MAP);
}



/*
 * Shuffle one of the stores.
 */
void store_shuffle(int which)
{
	int i, j;


	/* Ignore home */
	if (which == STORE_HOME) return;


	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Pick a new owner */
	for (j = st_ptr->owner; j == st_ptr->owner; )
	{
		st_ptr->owner = (byte)rand_int(z_info->b_max);
	}

	/* Activate the new owner */
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];


	/* Discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &st_ptr->stock[i];

		/* Discount non-discounted items by 40 percent */
		if (o_ptr->discount == 0) o_ptr->discount = 40;
	}
}


/*
 * Maintain the inventory at the stores.
 */
void store_maint(int which)
{
	int j;

	int old_rating = rating;


	/* Ignore home */
	if (which == STORE_HOME) return;


	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Activate the owner */
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];


	/* Mega-Hack -- prune the black market */
	if (store_num == STORE_B_MARKET)
	{
		/* Destroy crappy black market items */
		for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *o_ptr = &st_ptr->stock[j];

			/* Destroy crappy items */
			if (black_market_crap(o_ptr))
			{
				/* Destroy the object */
				store_item_increase(j, 0 - o_ptr->number);
				store_item_optimize(j);
			}
		}
	}


	/* Choose the number of slots to keep */
	j = st_ptr->stock_num;

	/* Sell a few items */
	j = j - randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "underflow" */
	if (j < 0) j = 0;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j) store_delete();


	/* Choose the number of slots to fill */
	j = st_ptr->stock_num;

	/* Buy some more items */
	j = j + randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "overflow" */
	if (j >= st_ptr->stock_size) j = st_ptr->stock_size - 1;

	/* Create some new items */
	while (st_ptr->stock_num < j) store_create();


	/* Hack -- Restore the rating */
	rating = old_rating;
}


/*
 * Initialize the stores
 */
void store_init(int which)
{
	int k;


	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];


	/* Pick an owner */
	st_ptr->owner = (byte)rand_int(z_info->b_max);

	/* Activate the new owner */
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];


	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/* Clear any old items */
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		object_wipe(&st_ptr->stock[k]);
	}
}
