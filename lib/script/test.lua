-- A very simple test-suite for Lua support in Angband

function test_use_object_hook(test_tval, test_svals)
	for test_sval, _ in test_svals do
		local test_object = {tval = test_tval, sval = test_sval, pval = 1}
		use_object_hook(test_object)
	end
end

-- Test the food
test_use_object_hook(TV_FOOD, {
		SV_FOOD_POISON,
		SV_FOOD_BLINDNESS,
		SV_FOOD_PARANOIA,
		SV_FOOD_CONFUSION,
		SV_FOOD_HALLUCINATION,
		SV_FOOD_PARALYSIS,
		SV_FOOD_WEAKNESS,
		SV_FOOD_SICKNESS,
		SV_FOOD_STUPIDITY,
		SV_FOOD_NAIVETY,
		SV_FOOD_UNHEALTH,
		SV_FOOD_DISEASE,
		SV_FOOD_CURE_POISON,
		SV_FOOD_CURE_BLINDNESS,
		SV_FOOD_CURE_PARANOIA,
		SV_FOOD_CURE_CONFUSION,
		SV_FOOD_CURE_SERIOUS,
		SV_FOOD_RESTORE_STR,
		SV_FOOD_RESTORE_CON,
		SV_FOOD_RESTORING,
		SV_FOOD_BISCUIT,
		SV_FOOD_JERKY,
		SV_FOOD_RATION,
		SV_FOOD_SLIME_MOLD,
		SV_FOOD_WAYBREAD,
		SV_FOOD_PINT_OF_ALE,
		SV_FOOD_PINT_OF_WINE})

-- Test the potions
test_use_object_hook(TV_POTION, {
		SV_POTION_WATER,
		SV_POTION_APPLE_JUICE,
		SV_POTION_SLIME_MOLD,
		SV_POTION_SLOWNESS,
		SV_POTION_SALT_WATER,
		SV_POTION_POISON,
		SV_POTION_BLINDNESS,
		SV_POTION_CONFUSION,
		SV_POTION_SLEEP,
		SV_POTION_LOSE_MEMORIES,
		SV_POTION_RUINATION,
		SV_POTION_DEC_STR,
		SV_POTION_DEC_INT,
		SV_POTION_DEC_WIS,
		SV_POTION_DEC_DEX,
		SV_POTION_DEC_CON,
		SV_POTION_DEC_CHR,
		SV_POTION_DETONATIONS,
		SV_POTION_DEATH,
		SV_POTION_INFRAVISION,
		SV_POTION_DETECT_INVIS,
		SV_POTION_SLOW_POISON,
		SV_POTION_CURE_POISON,
		SV_POTION_BOLDNESS,
		SV_POTION_SPEED,
		SV_POTION_RESIST_HEAT,
		SV_POTION_RESIST_COLD,
		SV_POTION_HEROISM,
		SV_POTION_BERSERK_STRENGTH,
		SV_POTION_CURE_LIGHT,
		SV_POTION_CURE_SERIOUS,
		SV_POTION_CURE_CRITICAL,
		SV_POTION_HEALING,
		SV_POTION_STAR_HEALING,
		SV_POTION_LIFE,
		SV_POTION_RESTORE_MANA,
		SV_POTION_RESTORE_EXP,
		SV_POTION_RES_STR,
		SV_POTION_RES_INT,
		SV_POTION_RES_WIS,
		SV_POTION_RES_DEX,
		SV_POTION_RES_CON,
		SV_POTION_RES_CHR,
		SV_POTION_INC_STR,
		SV_POTION_INC_INT,
		SV_POTION_INC_WIS,
		SV_POTION_INC_DEX,
		SV_POTION_INC_CON,
		SV_POTION_INC_CHR,
		SV_POTION_AUGMENTATION,
		SV_POTION_ENLIGHTENMENT,
		SV_POTION_STAR_ENLIGHTENMENT,
		SV_POTION_SELF_KNOWLEDGE,
		SV_POTION_EXPERIENCE})

-- Test the scrolls
test_use_object_hook(TV_SCROLL, {
		SV_SCROLL_DARKNESS,
		SV_SCROLL_AGGRAVATE_MONSTER,
		SV_SCROLL_CURSE_ARMOR,
		SV_SCROLL_CURSE_WEAPON,
		SV_SCROLL_SUMMON_MONSTER,
		SV_SCROLL_SUMMON_UNDEAD,
		SV_SCROLL_TRAP_CREATION,
		SV_SCROLL_PHASE_DOOR,
		SV_SCROLL_TELEPORT,
		SV_SCROLL_TELEPORT_LEVEL,
		SV_SCROLL_WORD_OF_RECALL,
		SV_SCROLL_IDENTIFY,
		SV_SCROLL_STAR_IDENTIFY,
		SV_SCROLL_REMOVE_CURSE,
		SV_SCROLL_STAR_REMOVE_CURSE,
		SV_SCROLL_ENCHANT_ARMOR,
		SV_SCROLL_ENCHANT_WEAPON_TO_HIT,
		SV_SCROLL_ENCHANT_WEAPON_TO_DAM,
		SV_SCROLL_STAR_ENCHANT_ARMOR,
		SV_SCROLL_STAR_ENCHANT_WEAPON,
		SV_SCROLL_RECHARGING,
		SV_SCROLL_LIGHT,
		SV_SCROLL_MAPPING,
		SV_SCROLL_DETECT_GOLD,
		SV_SCROLL_DETECT_ITEM,
		SV_SCROLL_DETECT_TRAP,
		SV_SCROLL_DETECT_DOOR,
		SV_SCROLL_DETECT_INVIS,
		SV_SCROLL_SATISFY_HUNGER,
		SV_SCROLL_BLESSING,
		SV_SCROLL_HOLY_CHANT,
		SV_SCROLL_HOLY_PRAYER,
		SV_SCROLL_MONSTER_CONFUSION,
		SV_SCROLL_PROTECTION_FROM_EVIL,
		SV_SCROLL_RUNE_OF_PROTECTION,
		SV_SCROLL_TRAP_DOOR_DESTRUCTION,
		SV_SCROLL_STAR_DESTRUCTION,
		SV_SCROLL_DISPEL_UNDEAD,
		SV_SCROLL_GENOCIDE,
		SV_SCROLL_MASS_GENOCIDE,
		SV_SCROLL_ACQUIREMENT,
		SV_SCROLL_STAR_ACQUIREMENT})

