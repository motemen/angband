-- Use objects


-- Larger values makes using devices harder
USE_DEVICE = 3


function eat_food(object)
	local ident = false

	--if object.sval == SV_FOOD_RATION or
	--   object.sval == SV_FOOD_BISCUIT or
	--   object.sval == SV_FOOD_JERKY or
	--   object.sval == SV_FOOD_SLIME_MOLD or
	--   object.sval == SV_FOOD_PINT_OF_ALE or
	--   object.sval == SV_FOOD_PINT_OF_WINE then
	--	msg_print("That tastes good.")
	--	ident = true
	-- ���ꂼ��̐H�ו��̊��z���I���W�i�����ׂ����\��
	if object.sval == SV_FOOD_RATION then
		msg_print("����͂��������B")
		ident = true
	elseif object.sval == SV_FOOD_BISCUIT then
		msg_print("�Â��ăT�N�T�N���ĂƂĂ����������B")
		ident = true
	elseif object.sval == SV_FOOD_JERKY then
		msg_print("���������������Ă��������B")
		ident = true
	elseif object.sval == SV_FOOD_SLIME_MOLD then
		msg_print("����͂Ȃ�Ƃ��`�e�������������B")
		ident = true
	elseif object.sval == SV_FOOD_PINT_OF_ALE then
		msg_print("�̂ǂ����u�₩���B")
		ident = true
	elseif object.sval == SV_FOOD_PINT_OF_WINE then
		msg_print("����͂�����B")
		ident = true
	elseif object.sval == SV_FOOD_WAYBREAD then
		--msg_print("That tastes good.")
		msg_print("����͂Ђ��傤�ɔ������B")
		set_poisoned(0)
		hp_player(damroll(4, 8))
		ident = true
	elseif object.sval == SV_FOOD_RESTORING then
		if do_res_stat(A_STR) then ident = true end
		if do_res_stat(A_INT) then ident = true end
		if do_res_stat(A_WIS) then ident = true end
		if do_res_stat(A_DEX) then ident = true end
		if do_res_stat(A_CON) then ident = true end
		if do_res_stat(A_CHR) then ident = true end
	elseif object.sval == SV_FOOD_RESTORE_CON then
		if do_res_stat(A_CON) then ident = true end
	elseif object.sval == SV_FOOD_RESTORE_STR then
		if do_res_stat(A_STR) then ident = true end
	elseif object.sval == SV_FOOD_CURE_SERIOUS then
		if hp_player(damroll(4, 8)) then ident = true end
	elseif object.sval == SV_FOOD_CURE_CONFUSION then
		if set_confused(0) then ident = true end
	elseif object.sval == SV_FOOD_CURE_PARANOIA then
		if set_afraid(0) then ident = true end
	elseif object.sval == SV_FOOD_CURE_BLINDNESS then
		if set_blind(0) then ident = true end
	elseif object.sval == SV_FOOD_CURE_POISON then
		if set_poisoned(0) then ident = true end
	elseif object.sval == SV_FOOD_DISEASE then
		--take_hit(damroll(10, 10), "poisonous food")
		take_hit(damroll(10, 10), "�œ���H��")
		do_dec_stat(A_STR)
		ident = true
	elseif object.sval == SV_FOOD_UNHEALTH then
		--take_hit(damroll(10, 10), "poisonous food")
		take_hit(damroll(10, 10), "�œ���H��")
		do_dec_stat(A_CON)
		ident = true
	elseif object.sval == SV_FOOD_NAIVETY then
		--take_hit(damroll(8, 8), "poisonous food")
		take_hit(damroll(8, 8), "�œ���H��")
		do_dec_stat(A_WIS)
		ident = true
	elseif object.sval == SV_FOOD_STUPIDITY then
		--take_hit(damroll(8, 8), "poisonous food")
		take_hit(damroll(8, 8), "�œ���H��")
		do_dec_stat(A_INT)
		ident = true
	elseif object.sval == SV_FOOD_SICKNESS then
		--take_hit(damroll(6, 6), "poisonous food")
		take_hit(damroll(6, 6), "�œ���H��")
		do_dec_stat(A_CON)
		ident = true
	elseif object.sval == SV_FOOD_WEAKNESS then
		--take_hit(damroll(6, 6), "poisonous food")
		take_hit(damroll(6, 6), "�œ���H��")
		do_dec_stat(A_STR)
		ident = true
	elseif object.sval == SV_FOOD_PARALYSIS then
		if not player.free_act then
			if set_paralyzed(player.paralyzed + rand_int(10) + 10) then
				ident = true
			end
		end
	elseif object.sval == SV_FOOD_HALLUCINATION then
		if not player.resist_chaos then
			if set_image(player.image + rand_int(250) + 250) then
				ident = true
			end
		end
	elseif object.sval == SV_FOOD_CONFUSION then
		if  not player.resist_confu then
			if set_confused(player.confused + rand_int(10) + 10) then
				ident = true
			end
		end
	elseif object.sval == SV_FOOD_PARANOIA then
		if not player.resist_fear then
			if set_afraid(player.afraid + rand_int(10) + 10) then
				ident = true
			end
		end
	elseif object.sval == SV_FOOD_POISON then
		if not (player.resist_pois or (player.oppose_pois > 0)) then
			if set_poisoned(player.poisoned + rand_int(10) + 10) then
				ident = true
			end
		end
	elseif object.sval == SV_FOOD_BLINDNESS then
		if not player.resist_blind then
			if set_blind(player.blind + rand_int(200) + 200) then
				ident = true
			end
		end
	end

	-- Food can feed the player
	set_food(player.food + object.pval)

	return ident, true
end


function quaff_potion(object)
	local ident = false

	--if object.sval == SV_POTION_WATER or
	--   object.sval == SV_POTION_APPLE_JUICE or
	--   object.sval == SV_POTION_SLIME_MOLD then
	--	msg_print("You feel less thirsty.")
	--	ident = true
	if object.sval == SV_POTION_WATER then
		msg_print("���̒��������ς肵���B")
		msg_print("�̂ǂ̊��������������܂����B")
		ident = true
	elseif object.sval == SV_POTION_APPLE_JUICE then
		msg_print("�Â��ăT�b�p���Ƃ��Ă��āA�ƂĂ����������B")
		msg_print("�̂ǂ̊��������������܂����B")
		ident = true
	elseif object.sval == SV_POTION_SLIME_MOLD then
		msg_print("�Ȃ�Ƃ��s�C���Ȗ����B")
		msg_print("�̂ǂ̊��������������܂����B")
		ident = true
	elseif object.sval == SV_POTION_SLOWNESS then
		if set_slow(player.slow + randint(25) + 15) then ident = true end
	elseif object.sval == SV_POTION_SALT_WATER then
		--msg_print("The potion makes you vomit!")
		msg_print("�����I�v�킸�f���Ă��܂����B")
		set_food(PY_FOOD_STARVE - 1)
		set_poisoned(0)
		set_paralyzed(player.paralyzed + 4)
		ident = true
	elseif object.sval == SV_POTION_POISON then
		if not (player.resist_pois or (player.oppose_pois > 0)) then
			if set_poisoned(player.poisoned + rand_int(15) + 10) then
				ident = true
			end
		end
	elseif object.sval == SV_POTION_BLINDNESS then
		if not player.resist_blind then
			if set_blind(player.blind + rand_int(100) + 100) then
				ident = true
			end
		end
	elseif object.sval == SV_POTION_CONFUSION then
		if not player.resist_confu then
			if set_confused(player.confused + rand_int(20) + 15) then
				ident = true
			end
		end
	elseif object.sval == SV_POTION_SLEEP then
		if not player.free_act then
			if set_paralyzed(player.paralyzed + rand_int(4) + 4) then
				ident = true
			end
		end
	elseif object.sval == SV_POTION_LOSE_MEMORIES then
		if not player.hold_life and (player.exp > 0) then
			--msg_print("You feel your memories fade.")
			msg_print("�ߋ��̋L��������Ă����C������B")
			lose_exp(player.exp / 4)
			ident = true
		end
	elseif object.sval == SV_POTION_RUINATION then
		--msg_print("Your nerves and muscles feel weak and lifeless!")
		msg_print("�g���S������Ă��āA���C�������Ă����悤���B")
		--take_hit(damroll(10, 10), "a potion of Ruination")
		take_hit(damroll(10, 10), "�j�ł̖�")
		dec_stat(A_DEX, 25, true)
		dec_stat(A_WIS, 25, true)
		dec_stat(A_CON, 25, true)
		dec_stat(A_STR, 25, true)
		dec_stat(A_CHR, 25, true)
		dec_stat(A_INT, 25, true)
		ident = true
	elseif object.sval == SV_POTION_DEC_STR then
		if do_dec_stat(A_STR) then ident = true end
	elseif object.sval == SV_POTION_DEC_INT then
		if do_dec_stat(A_INT) then ident = true end
	elseif object.sval == SV_POTION_DEC_WIS then
		if do_dec_stat(A_WIS) then ident = true end
	elseif object.sval == SV_POTION_DEC_DEX then
		if do_dec_stat(A_DEX) then ident = true end
	elseif object.sval == SV_POTION_DEC_CON then
		if do_dec_stat(A_CON) then ident = true end
	elseif object.sval == SV_POTION_DEC_CHR then
		if do_dec_stat(A_CHR) then ident = true end
	elseif object.sval == SV_POTION_DETONATIONS then
		--msg_print("Massive explosions rupture your body!")
		msg_print("�̂̒��Ō������������N�����I")
		--take_hit(damroll(50, 20), "a potion of Detonation")
		take_hit(damroll(50, 20), "�����̖�")
		set_stun(player.stun + 75)
		set_cut(player.cut + 5000)
		ident = true
	elseif object.sval == SV_POTION_DEATH then
		--msg_print("A feeling of Death flows through your body.")
		msg_print("���̗\�����̒����삯�߂������B")
		--take_hit(5000, "a potion of Death")
		take_hit(5000, "���̖�")
		ident = true
	elseif object.sval == SV_POTION_INFRAVISION then
		if set_tim_infra(player.tim_infra + 100 + randint(100)) then
			ident = true
		end
	elseif object.sval == SV_POTION_DETECT_INVIS then
		if set_tim_invis(player.tim_invis + 12 + randint(12)) then
			ident = true
		end
	elseif object.sval == SV_POTION_SLOW_POISON then
		if set_poisoned(player.poisoned / 2) then ident = true end
	elseif object.sval == SV_POTION_CURE_POISON then
		if set_poisoned(0) then ident = true end
	elseif object.sval == SV_POTION_BOLDNESS then
		if set_afraid(0) then ident = true end
	elseif object.sval == SV_POTION_SPEED then
		if player.fast == 0 then
			if set_fast(randint(25) + 15) then ident = true end
		else
			set_fast(player.fast + 5)
		end
	elseif object.sval == SV_POTION_RESIST_HEAT then
		if set_oppose_fire(player.oppose_fire + randint(10) + 10) then
			ident = true
		end
	elseif object.sval == SV_POTION_RESIST_COLD then
		if set_oppose_cold(player.oppose_cold + randint(10) + 10) then
			ident = true
		end
	elseif object.sval == SV_POTION_HEROISM then
		if hp_player(10) then ident = true end
		if set_afraid(0) then ident = true end
		if set_hero(player.hero + randint(25) + 25) then ident = true end
	elseif object.sval == SV_POTION_BERSERK_STRENGTH then
		if hp_player(30) then ident = true end
		if set_afraid(0) then ident = true end
		if set_shero(player.shero + randint(25) + 25) then ident = true end
	elseif object.sval == SV_POTION_CURE_LIGHT then
		if hp_player(damroll(2, 8)) then ident = true end
		if set_blind(0) then ident = true end
		if set_cut(player.cut - 10) then ident = true end
	elseif object.sval == SV_POTION_CURE_SERIOUS then
		if hp_player(damroll(4, 8)) then ident = true end
		if set_blind(0) then ident = true end
		if set_confused(0) then ident = true end
		if set_cut((player.cut / 2) - 50) then ident = true end
	elseif object.sval == SV_POTION_CURE_CRITICAL then
		if hp_player(damroll(6, 8)) then ident = true end
		if set_blind(0) then ident = true end
		if set_confused(0) then ident = true end
		if set_poisoned(0) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif object.sval == SV_POTION_HEALING then
		if hp_player(300) then ident = true end
		if set_blind(0) then ident = true end
		if set_confused(0) then ident = true end
		if set_poisoned(0) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif object.sval == SV_POTION_STAR_HEALING then
		if hp_player(1200) then ident = true end
		if set_blind(0) then ident = true end
		if set_confused(0) then ident = true end
		if set_poisoned(0) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif object.sval == SV_POTION_LIFE then
		--msg_print("You feel life flow through your body!")
		msg_print("�̒��ɐ����͂��������ӂ�Ă����I")
		restore_level()
		set_poisoned(0)
		set_blind(0)
		set_confused(0)
		set_image(0)
		set_stun(0)
		set_cut(0)
		do_res_stat(A_STR)
		do_res_stat(A_CON)
		do_res_stat(A_DEX)
		do_res_stat(A_WIS)
		do_res_stat(A_INT)
		do_res_stat(A_CHR)

		-- Recalculate max. hitpoints
		update_stuff()

		hp_player(5000)

		ident = true
	elseif object.sval == SV_POTION_RESTORE_MANA then
		if player.csp < player.msp then
			player.csp = player.msp
			player.csp_frac = 0
			--msg_print("Your feel your head clear.")
			msg_print("�����n�b�L���Ƃ����B")
			player.redraw = bitlib.bOr(player.redraw, PR_MANA)
			player.window = bitlib.bOr(player.window, PW_PLAYER_0, PW_PLAYER_1)
			ident = true
		end
	elseif object.sval == SV_POTION_RESTORE_EXP then
		if restore_level() then ident = true end
	elseif object.sval == SV_POTION_RES_STR then
		if do_res_stat(A_STR) then ident = true end
	elseif object.sval == SV_POTION_RES_INT then
		if do_res_stat(A_INT) then ident = true end
	elseif object.sval == SV_POTION_RES_WIS then
		if do_res_stat(A_WIS) then ident = true end
	elseif object.sval == SV_POTION_RES_DEX then
		if do_res_stat(A_DEX) then ident = true end
	elseif object.sval == SV_POTION_RES_CON then
		if do_res_stat(A_CON) then ident = true end
	elseif object.sval == SV_POTION_RES_CHR then
		if do_res_stat(A_CHR) then ident = true end
	elseif object.sval == SV_POTION_INC_STR then
		if do_inc_stat(A_STR) then ident = true end
	elseif object.sval == SV_POTION_INC_INT then
		if do_inc_stat(A_INT) then ident = true end
	elseif object.sval == SV_POTION_INC_WIS then
		if do_inc_stat(A_WIS) then ident = true end
	elseif object.sval == SV_POTION_INC_DEX then
		if do_inc_stat(A_DEX) then ident = true end
	elseif object.sval == SV_POTION_INC_CON then
		if do_inc_stat(A_CON) then ident = true end
	elseif object.sval == SV_POTION_INC_CHR then
		if do_inc_stat(A_CHR) then ident = true end
	elseif object.sval == SV_POTION_AUGMENTATION then
		if do_inc_stat(A_STR) then ident = true end
		if do_inc_stat(A_INT) then ident = true end
		if do_inc_stat(A_WIS) then ident = true end
		if do_inc_stat(A_DEX) then ident = true end
		if do_inc_stat(A_CON) then ident = true end
		if do_inc_stat(A_CHR) then ident = true end
	elseif object.sval == SV_POTION_ENLIGHTENMENT then
		--msg_print("An image of your surroundings forms in your mind...")
		msg_print("�����̒u����Ă���󋵂��]���ɕ�����ł���...")
		wiz_lite()
		ident = true
	elseif object.sval == SV_POTION_STAR_ENLIGHTENMENT then
		--msg_print("You begin to feel more enlightened...")
		msg_print("�X�Ȃ�[�ւ�������...")
		message_flush()
		wiz_lite()
		do_inc_stat(A_INT)
		do_inc_stat(A_WIS)
		detect_traps()
		detect_doors()
		detect_stairs()
		detect_treasure()
		detect_objects_gold()
		detect_objects_normal()
		identify_pack()
		self_knowledge()
		ident = true
	elseif object.sval == SV_POTION_SELF_KNOWLEDGE then
		--msg_print("You begin to know yourself a little better...")
		msg_print("�������g�̂��Ƃ������͕��������C������...")
		message_flush()
		self_knowledge()
		ident = true
	elseif object.sval == SV_POTION_EXPERIENCE then
		if (player.exp < PY_MAX_EXP) then
			local ee = (player.exp / 2) + 10
			if (ee > 100000) then ee = 100000 end
			--msg_print("You feel more experienced.")
			msg_print("�X�Ɍo����ς񂾂悤�ȋC������B")
			gain_exp(ee)
			ident = true
		end
	end

	-- Potions can feed the player
	set_food(player.food + object.pval)

	return ident, true
end


function read_scroll(object)
	local ident = false

	-- Assume the scroll will get used up
	local used_up = true

	if object.sval == SV_SCROLL_DARKNESS then
		if not player.resist_blind then
			set_blind(player.blind + 3 + randint(5))
		end
		if unlite_area(10, 3) then ident = true end
	elseif object.sval == SV_SCROLL_AGGRAVATE_MONSTER then
		--msg_print("There is a high pitched humming noise.")
		msg_print("�J���������Ȃ�l�ȉ����ӂ�𕢂����B")
		aggravate_monsters(0)
		ident = true
	elseif object.sval == SV_SCROLL_CURSE_ARMOR then
		if curse_armor() then ident = true end
	elseif object.sval == SV_SCROLL_CURSE_WEAPON then
		if curse_weapon() then ident = true end
	elseif object.sval == SV_SCROLL_SUMMON_MONSTER then
		sound(MSG_SUM_MONSTER)
		for k = 0, randint(3) do
			if summon_specific(player.py, player.px, player.depth, 0) then
				ident = true
			end
		end
	elseif object.sval == SV_SCROLL_SUMMON_UNDEAD then
		sound(MSG_SUM_UNDEAD)
		for k = 0, randint(3) do
			if summon_specific(player.py, player.px, player.depth, SUMMON_UNDEAD) then
				ident = true
			end
		end
	elseif object.sval == SV_SCROLL_TRAP_CREATION then
		if trap_creation() then ident = true end
	elseif object.sval == SV_SCROLL_PHASE_DOOR then
		teleport_player(10)
		ident = true
	elseif object.sval == SV_SCROLL_TELEPORT then
		teleport_player(100)
		ident = true
	elseif object.sval == SV_SCROLL_TELEPORT_LEVEL then
		teleport_player_level()
		ident = true
	elseif object.sval == SV_SCROLL_WORD_OF_RECALL then
		set_recall()
		ident = true
	elseif object.sval == SV_SCROLL_IDENTIFY then
		ident = true
		if not ident_spell() then used_up = false end
	elseif object.sval == SV_SCROLL_STAR_IDENTIFY then
		ident = true
		if not identify_fully() then used_up = false end
	elseif object.sval == SV_SCROLL_REMOVE_CURSE then
		if remove_curse() then
			--msg_print("You feel as if someone is watching over you.")
			msg_print("�N���Ɍ�����Ă���悤�ȋC������B")
			ident = true
		end
	elseif object.sval == SV_SCROLL_STAR_REMOVE_CURSE then
		remove_all_curse()
		ident = true
	elseif object.sval == SV_SCROLL_ENCHANT_ARMOR then
		ident = true
		if not enchant_spell(0, 0, 1) then used_up = false end
	elseif object.sval == SV_SCROLL_ENCHANT_WEAPON_TO_HIT then
		if not enchant_spell(1, 0, 0) then used_up = false end
		ident = true
	elseif object.sval == SV_SCROLL_ENCHANT_WEAPON_TO_DAM then
		if not enchant_spell(0, 1, 0) then used_up = false end
		ident = true
	elseif object.sval == SV_SCROLL_STAR_ENCHANT_ARMOR then
		if not enchant_spell(0, 0, randint(3) + 2) then used_up = false end
		ident = true
	elseif object.sval == SV_SCROLL_STAR_ENCHANT_WEAPON then
		if not enchant_spell(randint(3), randint(3), 0) then used_up = false end
		ident = true
	elseif object.sval == SV_SCROLL_RECHARGING then
		if not recharge(60) then used_up = false end
		ident = true
	elseif object.sval == SV_SCROLL_LIGHT then
		if lite_area(damroll(2, 8), 2) then ident = true end
	elseif object.sval == SV_SCROLL_MAPPING then
		map_area()
		ident = true
	elseif object.sval == SV_SCROLL_DETECT_GOLD then
		if detect_treasure() then ident = true end
		if detect_objects_gold() then ident = true end
	elseif object.sval == SV_SCROLL_DETECT_ITEM then
		if detect_objects_normal() then ident = true end
	elseif object.sval == SV_SCROLL_DETECT_TRAP then
		if detect_traps() then ident = true end
	elseif object.sval == SV_SCROLL_DETECT_DOOR then
		if detect_doors() then ident = true end
		if detect_stairs() then ident = true end
	elseif object.sval == SV_SCROLL_DETECT_INVIS then
		if detect_monsters_invis() then ident = true end
	elseif object.sval == SV_SCROLL_SATISFY_HUNGER then
		if set_food(PY_FOOD_MAX - 1) then ident = true end
	elseif object.sval == SV_SCROLL_BLESSING then
		if set_blessed(player.blessed + randint(12) + 6) then ident = true end
	elseif object.sval == SV_SCROLL_HOLY_CHANT then
		if set_blessed(player.blessed + randint(24) + 12) then ident = true end
	elseif object.sval == SV_SCROLL_HOLY_PRAYER then
		if set_blessed(player.blessed + randint(48) + 24) then ident = true end
	elseif object.sval == SV_SCROLL_MONSTER_CONFUSION then
		if player.confusing == 0 then
			--msg_print("Your hands begin to glow.")
			msg_print("�肪�P���n�߂��B")
			player.confusing = 1
			ident = true
		end
	elseif object.sval == SV_SCROLL_PROTECTION_FROM_EVIL then
		local k = 3 * player.lev
		if set_protevil(player.protevil + randint(25) + k) then ident = true end
	elseif object.sval == SV_SCROLL_RUNE_OF_PROTECTION then
		warding_glyph()
		ident = true
	elseif object.sval == SV_SCROLL_TRAP_DOOR_DESTRUCTION then
		if destroy_doors_touch() then ident = true end
	elseif object.sval == SV_SCROLL_STAR_DESTRUCTION then
		destroy_area(player.py, player.px, 15, true)
		ident = true
	elseif object.sval == SV_SCROLL_DISPEL_UNDEAD then
		if dispel_undead(60) then ident = true end
	elseif object.sval == SV_SCROLL_BANISHMENT then
		if not banishment() then used_up = false end
		ident = true
	elseif object.sval == SV_SCROLL_MASS_BANISHMENT then
		mass_banishment()
		ident = true
	elseif object.sval == SV_SCROLL_ACQUIREMENT then
		acquirement(player.py, player.px, 1, true)
		ident = true
	elseif object.sval == SV_SCROLL_STAR_ACQUIREMENT then
		acquirement(player.py, player.px, randint(2) + 1, true)
		ident = true
	end

	return ident, used_up
end


function use_staff(object)
	local ident = false
	local used_charge = true

	if object.sval == SV_STAFF_DARKNESS then
		if not player.resist_blind then
			if set_blind(player.blind + 3 + randint(5)) then ident = true end
		end
		if unlite_area(10, 3) then ident = true end
	elseif object.sval == SV_STAFF_SLOWNESS then
		if set_slow(player.slow + randint(30) + 15) then ident = true end
	elseif object.sval == SV_STAFF_HASTE_MONSTERS then
		if speed_monsters() then ident = true end
	elseif object.sval == SV_STAFF_SUMMONING then
		sound(MSG_SUM_MONSTER)
		for k = 0, randint(4) do
			if summon_specific(player.py, player.px, player.depth, 0) then
				ident = true
			end
		end
	elseif object.sval == SV_STAFF_TELEPORTATION then
		teleport_player(100)
		ident = true
	elseif object.sval == SV_STAFF_IDENTIFY then
		if not ident_spell() then used_charge = false end
		ident = true
	elseif object.sval == SV_STAFF_REMOVE_CURSE then
		if remove_curse() then
			if player.blind == 0 then
				--msg_print("The staff glows blue for a moment...")
				msg_print("��͈�u�u���[�ɋP����...")
			end
			ident = true
		end
	elseif object.sval == SV_STAFF_STARLITE then
		if player.blind == 0 then
			--msg_print("The end of the staff glows brightly...")
			msg_print("��̐悪���邭�P����...")
		end

		for i = 1, 9 do
			lite_line(ddd[i])
		end
		ident = true
	elseif object.sval == SV_STAFF_LITE then
		if lite_area(damroll(2, 8), 2) then ident = true end
	elseif object.sval == SV_STAFF_MAPPING then
		map_area()
		ident = true
	elseif object.sval == SV_STAFF_DETECT_GOLD then
		if detect_treasure() then ident = true end
		if detect_objects_gold() then ident = true end
	elseif object.sval == SV_STAFF_DETECT_ITEM then
		if detect_objects_normal() then ident = true end
	elseif object.sval == SV_STAFF_DETECT_TRAP then
		if detect_traps() then ident = true end
	elseif object.sval == SV_STAFF_DETECT_DOOR then
		if detect_doors() then ident = true end
		if detect_stairs() then ident = true end
	elseif object.sval == SV_STAFF_DETECT_INVIS then
		if detect_monsters_invis() then ident = true end
	elseif object.sval == SV_STAFF_DETECT_EVIL then
		if detect_monsters_evil() then ident = true end
	elseif object.sval == SV_STAFF_CURE_LIGHT then
		if hp_player(randint(8)) then ident = true end
	elseif object.sval == SV_STAFF_CURING then
		if set_blind(0) then ident = true end
		if set_poisoned(0) then ident = true end
		if set_confused(0) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif object.sval == SV_STAFF_HEALING then
		if hp_player(300) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif object.sval == SV_STAFF_THE_MAGI then
		if do_res_stat(A_INT) then ident = true end

		if player.csp < player.msp then
			player.csp = player.msp
			player.csp_frac = 0
			--msg_print("Your feel your head clear.")
			msg_print("�����n�b�L���Ƃ����B")
			player.redraw = bitlib.bOr(player.redraw, PR_MANA)
			player.window = bitlib.bOr(player.window, PW_PLAYER_0, PW_PLAYER_1)
			ident = true
		end
	elseif object.sval == SV_STAFF_SLEEP_MONSTERS then
		if sleep_monsters() then ident = true end
	elseif object.sval == SV_STAFF_SLOW_MONSTERS then
		if slow_monsters() then ident = true end
	elseif object.sval == SV_STAFF_SPEED then
		if player.fast == 0 then
			if set_fast(randint(30) + 15) then ident = true end
		else
			set_fast(player.fast + 5)
		end
	elseif object.sval == SV_STAFF_PROBING then
		if probing() then ident = true end
	elseif object.sval == SV_STAFF_DISPEL_EVIL then
		if dispel_evil(60) then ident = true end
	elseif object.sval == SV_STAFF_POWER then
		if dispel_monsters(120) then ident = true end
	elseif object.sval == SV_STAFF_HOLINESS then
		if dispel_evil(120) then ident = true end
		if set_protevil(player.protevil + randint(25) + 3 * player.lev) then
			ident = true
		end
		if set_poisoned(0) then ident = true end
		if set_afraid(0) then ident = true end
		if hp_player(50) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif object.sval == SV_STAFF_BANISHMENT then
		if not banishment() then used_charge = false end
		ident = true
	elseif object.sval == SV_STAFF_EARTHQUAKES then
		earthquake(player.py, player.px, 10)
		ident = true
	elseif object.sval == SV_STAFF_DESTRUCTION then
		destroy_area(player.py, player.px, 15, true)
		ident = true
	end

	return ident, used_charge
end


function aim_wand(object)
	local ident = false

	local success
	local dir

	-- Allow direction to be cancelled for free
	success, dir = get_aim_dir()
	if not success then return false, false end

	-- Take a turn
	player.energy_use = 100

	-- Get the object level
	local lev = k_info[object.k_idx + 1].level

	-- Base chance of success
	local chance = player.skill_dev

	-- Confusion hurts skill
	if player.confused > 0 then chance = chance / 2 end

	-- High level objects are harder
	if lev > 50 then
		chance = chance - 50
	else
		chance = chance - lev
	end

	-- Give everyone a (slight) chance
	if (chance < USE_DEVICE) and (rand_int(USE_DEVICE - chance + 1) == 0) then
		chance = USE_DEVICE
	end

	-- Roll for usage
	if (chance < USE_DEVICE) or (randint(chance) < USE_DEVICE) then
		flush_fail()
		--msg_print("You failed to use the wand properly.")
		msg_print("���@�_�����܂��g���Ȃ������B")
		return false, false
	end

	-- The wand is already empty!
	if object.pval <= 0 then
		flush_fail()
		--msg_print("The wand has no charges left.")
		msg_print("���̖��@�_�ɂ͂������͂��c���Ă��Ȃ��B")

		object.ident = bitlib.bOr(object.ident, IDENT_EMPTY)
		player.notice = bitlib.bOr(player.notice, PN_COMBINE, PN_REORDER)
		player.window = bitlib.bOr(player.window, PW_INVEN)

		return false, false
	end

	-- Sound
	-- TODO: Wand sound?
	-- sound(MSG_ZAP)

	local sval = object.sval

	-- Hack -- Wand of wonder can do anything before it
	if sval == SV_WAND_WONDER then sval = rand_int(SV_WAND_WONDER) end

	if sval == SV_WAND_HEAL_MONSTER then
		if heal_monster(dir) then ident = true end
	elseif sval == SV_WAND_HASTE_MONSTER then
		if speed_monster(dir) then ident = true end
	elseif sval == SV_WAND_CLONE_MONSTER then
		if clone_monster(dir) then ident = true end
	elseif sval == SV_WAND_TELEPORT_AWAY then
		if teleport_monster(dir) then ident = true end
	elseif sval == SV_WAND_DISARMING then
		if disarm_trap(dir) then ident = true end
	elseif sval == SV_WAND_TRAP_DOOR_DEST then
		if destroy_door(dir) then ident = true end
	elseif sval == SV_WAND_STONE_TO_MUD then
		if wall_to_mud(dir) then ident = true end
	elseif sval == SV_WAND_LITE then
		if player.blind == 0 then
			--msg_print("A line of blue shimmering light appears.")
			msg_print("���P�������������ꂽ�B")
			ident = true
		end
		if lite_line(dir) then ident = true end
	elseif sval == SV_WAND_SLEEP_MONSTER then
		if sleep_monster(dir) then ident = true end
	elseif sval == SV_WAND_SLOW_MONSTER then
		if slow_monster(dir) then ident = true end
	elseif sval == SV_WAND_CONFUSE_MONSTER then
		if confuse_monster(dir, 10) then ident = true end
	elseif sval == SV_WAND_FEAR_MONSTER then
		if fear_monster(dir, 10) then ident = true end
	elseif sval == SV_WAND_DRAIN_LIFE then
		if drain_life(dir, 150) then ident = true end
	elseif sval == SV_WAND_POLYMORPH then
		if poly_monster(dir) then ident = true end
	elseif sval == SV_WAND_STINKING_CLOUD then
		fire_ball(GF_POIS, dir, 12, 2)
		ident = true
	elseif sval == SV_WAND_MAGIC_MISSILE then
		fire_bolt_or_beam(20, GF_MISSILE, dir, damroll(3, 4))
		ident = true
	elseif sval == SV_WAND_ACID_BOLT then
		fire_bolt_or_beam(20, GF_ACID, dir, damroll(10, 8))
		ident = true
	elseif sval == SV_WAND_ELEC_BOLT then
		fire_bolt_or_beam(20, GF_ELEC, dir, damroll(6, 6))
		ident = true
	elseif sval == SV_WAND_FIRE_BOLT then
		fire_bolt_or_beam(20, GF_FIRE, dir, damroll(12, 8))
		ident = true
	elseif sval == SV_WAND_COLD_BOLT then
		fire_bolt_or_beam(20, GF_COLD, dir, damroll(6, 8))
		ident = true
	elseif sval == SV_WAND_ACID_BALL then
		fire_ball(GF_ACID, dir, 120, 2)
		ident = true
	elseif sval == SV_WAND_ELEC_BALL then
		fire_ball(GF_ELEC, dir, 64, 2)
		ident = true
	elseif sval == SV_WAND_FIRE_BALL then
		fire_ball(GF_FIRE, dir, 144, 2)
		ident = true
	elseif sval == SV_WAND_COLD_BALL then
		fire_ball(GF_COLD, dir, 96, 2)
		ident = true
	elseif sval == SV_WAND_WONDER then
		--msg_print("Oops.  Wand of wonder activated.")
		msg_print("�����ƁA��̖��@�_���n���������B")
	elseif sval == SV_WAND_DRAGON_FIRE then
		fire_ball(GF_FIRE, dir, 200, 3)
		ident = true
	elseif sval == SV_WAND_DRAGON_COLD then
		fire_ball(GF_COLD, dir, 160, 3)
		ident = true
	elseif sval == SV_WAND_DRAGON_BREATH then
		local choice = randint(5)

		if choice == 1 then
			fire_ball(GF_ACID, dir, 200, 3)
		elseif choice == 2 then
			fire_ball(GF_ELEC, dir, 160, 3)
		elseif choice == 3 then
			fire_ball(GF_FIRE, dir, 200, 3)
		elseif choice == 4 then
			fire_ball(GF_COLD, dir, 160, 3)
		else
			fire_ball(GF_POIS, dir, 120, 3)
		end

		ident = true
	elseif sval == SV_WAND_ANNIHILATION then
		if drain_life(dir, 250) then ident = true end
	end

	return ident, true
end


function zap_rod(object)
	local ident = false
	local used_charge = true

	local dir

	-- Get a direction (unless KNOWN not to need it)
	if (object.sval >= SV_ROD_MIN_DIRECTION) or not object_aware_p(object) then
		local success

		-- Allow direction to be cancelled for free
		success, dir = get_aim_dir()
		if not success then return false, false end
	end

	-- Take a turn
	player.energy_use = 100

	-- Get the object level
	local lev = k_info[object.k_idx + 1].level

	-- Get the object pval (timeout)
	local pval = k_info[object.k_idx + 1].pval

	-- Base chance of success
	local chance = player.skill_dev

	-- Confusion hurts skill
	if player.confused > 0 then chance = chance / 2 end

	-- High level objects are harder
	if lev > 50 then
		chance = chance - 50
	else
		chance = chance - lev
	end

	-- Give everyone a (slight) chance
	if (chance < USE_DEVICE) and (rand_int(USE_DEVICE - chance + 1) == 0) then
		chance = USE_DEVICE
	end

	-- Roll for usage
	if (chance < USE_DEVICE) or (randint(chance) < USE_DEVICE) then
		flush_fail()
		--msg_print("You failed to use the rod properly.")
		msg_print("���܂����b�h���g���Ȃ������B")
		return false, false
	end

	-- Still charging?
	if object.timeout > (object.pval - pval) then
		flush_fail()

		if object.number == 1 then
			--msg_print("The rod is still charging.")
			msg_print("���̃��b�h�͂܂����͂��[�U���Ă���Œ����B")
		else
			--msg_print("The rods are all still charging.")
			msg_print("���̃��b�h�͂܂����͂��[�U���Ă���Œ����B")
		end

		return false, false
	end

	-- Sound
	sound(MSG_ZAP_ROD)

	local sval = object.sval

	if sval == SV_ROD_DETECT_TRAP then
		if detect_traps() then ident = true end
	elseif sval == SV_ROD_DETECT_DOOR then
		if detect_doors() then ident = true end
		if detect_stairs() then ident = true end
	elseif sval == SV_ROD_IDENTIFY then
		ident = true
		if not ident_spell() then used_charge = false end
	elseif sval == SV_ROD_RECALL then
		set_recall()
		ident = true
	elseif sval == SV_ROD_ILLUMINATION then
		if lite_area(damroll(2, 8), 2) then ident = true end
	elseif sval == SV_ROD_MAPPING then
		map_area()
		ident = true
	elseif sval == SV_ROD_DETECTION then
		detect_all()
		ident = true
	elseif sval == SV_ROD_PROBING then
		if probing() then ident = true end
	elseif sval == SV_ROD_CURING then
		if set_blind(0) then ident = true end
		if set_poisoned(0) then ident = true end
		if set_confused(0) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif sval == SV_ROD_HEALING then
		if hp_player(500) then ident = true end
		if set_stun(0) then ident = true end
		if set_cut(0) then ident = true end
	elseif sval == SV_ROD_RESTORATION then
		if restore_level() then ident = true end
		if do_res_stat(A_STR) then ident = true end
		if do_res_stat(A_INT) then ident = true end
		if do_res_stat(A_WIS) then ident = true end
		if do_res_stat(A_DEX) then ident = true end
		if do_res_stat(A_CON) then ident = true end
		if do_res_stat(A_CHR) then ident = true end
	elseif sval == SV_ROD_SPEED then
		if player.fast == 0 then
			if set_fast(randint(30) + 15) then ident = true end
		else
			set_fast(player.fast + 5)
		end
	elseif sval == SV_ROD_TELEPORT_AWAY then
		if teleport_monster(dir) then ident = true end
	elseif sval == SV_ROD_DISARMING then
		if disarm_trap(dir) then ident = true end
	elseif sval == SV_ROD_LITE then
		if player.blind == 0 then
			--msg_print("A line of blue shimmering light appears.")
			msg_print("���P�������������ꂽ�B")
			ident = true
		end
		if lite_line(dir) then ident = true end
	elseif sval == SV_ROD_SLEEP_MONSTER then
		if sleep_monster(dir) then ident = true end
	elseif sval == SV_ROD_SLOW_MONSTER then
		if slow_monster(dir) then ident = true end
	elseif sval == SV_ROD_DRAIN_LIFE then
		if drain_life(dir, 150) then ident = true end
	elseif sval == SV_ROD_POLYMORPH then
		if poly_monster(dir) then ident = true end
	elseif sval == SV_ROD_ACID_BOLT then
		fire_bolt_or_beam(10, GF_ACID, dir, damroll(12, 8))
		ident = true
	elseif sval == SV_ROD_ELEC_BOLT then
		fire_bolt_or_beam(10, GF_ELEC, dir, damroll(6, 6))
		ident = true
	elseif sval == SV_ROD_FIRE_BOLT then
		fire_bolt_or_beam(10, GF_FIRE, dir, damroll(16, 8))
		ident = true
	elseif sval == SV_ROD_COLD_BOLT then
		fire_bolt_or_beam(10, GF_COLD, dir, damroll(10, 8))
		ident = true
	elseif sval == SV_ROD_ACID_BALL then
		fire_ball(GF_ACID, dir, 120, 2)
		ident = true
	elseif sval == SV_ROD_ELEC_BALL then
		fire_ball(GF_ELEC, dir, 64, 2)
		ident = true
	elseif sval == SV_ROD_FIRE_BALL then
		fire_ball(GF_FIRE, dir, 144, 2)
		ident = true
	elseif sval == SV_ROD_COLD_BALL then
		fire_ball(GF_COLD, dir, 96, 2)
		ident = true
	end

	-- Drain the charge
	if used_charge == true then
		object.timeout = object.timeout + pval
	end

	return ident, used_charge
end


function activate_object(object)
	local success
	local dir

	-- Check the recharge
	if object.timeout > 0 then
		--msg_print("It whines, glows and fades...")
		msg_print("����͔����ɉ��𗧂āA�P���A������...")
		return false, false
	end

	--message(MSG_ACT_ARTIFACT, 0, "You activate it...")
	message(MSG_ACT_ARTIFACT, 0, "�n��������...")

	-- Artifacts
	if object.name1 > 0 then
		local artifact = a_info[object.name1 + 1]

		-- Get the basic name of the object
		local o_name = angband.object_desc(object, 0, 0)

		if artifact.activation == ACT_ILLUMINATION then
			--msg_print(string.format("The %s wells with clear light...", o_name))
			msg_print(string.format("%s���琟�񂾌������ӂ�o��...", o_name))
			lite_area(damroll(2, 15), 3)
		elseif artifact.activation == ACT_MAGIC_MAP then
			--msg_print(string.format("The %s shines brightly...", o_name))
			msg_print(string.format("%s��ῂ����P����...", o_name))
			map_area()
		elseif artifact.activation == ACT_CLAIRVOYANCE then
			--msg_print(string.format("The %s glows a deep green...", o_name))
			msg_print(string.format("%s�͐[���O���[���ɋP����...", o_name))
			wiz_lite()
			detect_traps()
			detect_doors()
			detect_stairs()
		elseif artifact.activation == ACT_PROT_EVIL then
			--msg_print(string.format("The %s lets out a shrill wail...", o_name))
			msg_print(string.format("%s����s����������o��...", o_name))
			set_protevil(player.protevil + randint(25) + 3 * player.lev)
		elseif artifact.activation == ACT_DISP_EVIL then
			--msg_print(string.format("The %s floods the area with goodness...", o_name))
			msg_print(string.format("%s�͕ӂ��P�̃I�[���Ŗ�������...", o_name))
			dispel_evil(player.lev * 5)
		elseif artifact.activation == ACT_HASTE2 then
			--msg_print(string.format("The %s glows brightly...", o_name))
			msg_print(string.format("%s�͖��邭�P����...", o_name))
			if player.fast == 0 then
				set_fast(randint(75) + 75)
			else
				set_fast(player.fast + 5)
			end
		elseif artifact.activation == ACT_FIRE3 then
			--msg_print(string.format("The %s glows deep red...", o_name))
			msg_print(string.format("%s�͐[�g�ɋP����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_ball(GF_FIRE, dir, 120, 3)
		elseif artifact.activation == ACT_FROST5 then
			--msg_print(string.format("The %s glows bright white...", o_name))
			msg_print(string.format("%s�͔������邭�P����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_ball(GF_COLD, dir, 200, 3)
		elseif artifact.activation == ACT_ELEC2 then
			--msg_print(string.format("The %s glows deep blue...", o_name))
			msg_print(string.format("%s�͐[���u���[�ɋP����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_ball(GF_ELEC, dir, 250, 3)
		elseif artifact.activation == ACT_BIZZARE then
			--msg_print(string.format("The %s glows intensely black...", o_name))
			msg_print(string.format("%s�͎����ɋP����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			ring_of_power(dir)
		elseif artifact.activation == ACT_STAR_BALL then
			--msg_print(string.format("Your %s is surrounded by lightning...", o_name))
			msg_print(string.format("%s����Ȃŕ���ꂽ...", o_name))
			for i = 1, 8 do fire_ball(GF_ELEC, ddd[i], 150, 3) end
		elseif artifact.activation == ACT_RAGE_BLESS_RESIST then
			--msg_print(string.format("Your %s glows many colours...", o_name))
			msg_print(string.format("%s���l�X�ȐF�ɋP����...", o_name))
			hp_player(30)
			set_afraid(0)
			set_shero(player.shero + randint(50) + 50)
			set_blessed(player.blessed + randint(50) + 50)
			set_oppose_acid(player.oppose_acid + randint(50) + 50)
			set_oppose_elec(player.oppose_elec + randint(50) + 50)
			set_oppose_fire(player.oppose_fire + randint(50) + 50)
			set_oppose_cold(player.oppose_cold + randint(50) + 50)
			set_oppose_pois(player.oppose_pois + randint(50) + 50)
		elseif artifact.activation == ACT_HEAL2 then
			--msg_print(string.format("Your %s glows a bright white...", o_name))
			msg_print(string.format("%s���������邭�P����...", o_name))
			--msg_print("You feel much better...")
			msg_print("�ƂĂ��C�����悭�Ȃ���...")
			hp_player(1000)
			set_cut(0)
		elseif artifact.activation == ACT_PHASE then
			--msg_print(string.format("Your %s twists space around you...", o_name))
			msg_print(string.format("%s���ӂ�̋�Ԃ��䂪�܂���...", o_name))
			teleport_player(10)
		elseif artifact.activation == ACT_BANISHMENT then
			--msg_print(string.format("Your %s glows deep blue...", o_name))
			msg_print(string.format("%s���[���u���[�ɋP����...", o_name))
			if not banishment() then return false, false end
		elseif artifact.activation == ACT_TRAP_DOOR_DEST then
			--msg_print(string.format("Your %s glows bright red...", o_name))
			msg_print(string.format("%s���Ԃ����邭�P����...", o_name))
			destroy_doors_touch()
		elseif artifact.activation == ACT_DETECT then
			--msg_print(string.format("Your %s glows bright white...", o_name))
			msg_print(string.format("%s���������邭�P����...", o_name))
			--msg_print("An image forms in your mind...")
			msg_print("�S�ɃC���[�W��������ł���...")
			detect_all()
		elseif artifact.activation == ACT_HEAL1 then
			--msg_print(string.format("Your %s glows deep blue...", o_name))
			msg_print(string.format("%s���[���u���[�ɋP����...", o_name))
			--msg_print("You feel a warm tingling inside...")
			msg_print("�̓��ɒg�����ۓ�����������...")
			hp_player(500)
			set_cut(0)
		elseif artifact.activation == ACT_RESIST then
			--msg_print(string.format("Your %s glows many colours...", o_name))
			msg_print(string.format("%s���l�X�ȐF�ɋP����...", o_name))
			set_oppose_acid(player.oppose_acid + randint(20) + 20)
			set_oppose_elec(player.oppose_elec + randint(20) + 20)
			set_oppose_fire(player.oppose_fire + randint(20) + 20)
			set_oppose_cold(player.oppose_cold + randint(20) + 20)
			set_oppose_pois(player.oppose_pois + randint(20) + 20)
		elseif artifact.activation == ACT_SLEEP then
			--msg_print(string.format("Your %s glows deep blue...", o_name))
			msg_print(string.format("%s���[���u���[�ɋP����...", o_name))
			sleep_monsters_touch()
		elseif artifact.activation == ACT_RECHARGE1 then
			--msg_print(string.format("Your %s glows bright yellow...", o_name))
			msg_print(string.format("%s�����F�����邭�P����...", o_name))
			if not recharge(60) then return false, false end
		elseif artifact.activation == ACT_TELEPORT then
			--msg_print(string.format("Your %s twists space around you...", o_name))
			msg_print(string.format("%s���ӂ�̋�Ԃ��䂪�܂���...", o_name))
			teleport_player(100)
		elseif artifact.activation == ACT_RESTORE_LIFE then
			--msg_print(string.format("Your %s glows a deep red...", o_name))
			msg_print(string.format("%s���[�g�ɋP����...", o_name))
			restore_level()
		elseif artifact.activation == ACT_MISSILE then
			--msg_print(string.format("Your %s glows extremely brightly...", o_name))
			msg_print(string.format("%s��ῂ������炢�ɖ��邭�P����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_MISSILE, dir, damroll(2, 6))
		elseif artifact.activation == ACT_FIRE1 then
			--msg_print(string.format("Your %s is covered in fire...", o_name))
			msg_print(string.format("%s�����ɕ���ꂽ...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_FIRE, dir, damroll(9, 8))
		elseif artifact.activation == ACT_FROST1 then
			--msg_print(string.format("Your %s is covered in frost...", o_name))
			msg_print(string.format("%s����C�ɕ���ꂽ...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_COLD, dir, damroll(6, 8))
		elseif artifact.activation == ACT_LIGHTNING_BOLT then
			--msg_print(string.format("Your %s is covered in sparks...", o_name))
			msg_print(string.format("%s���ΉԂɕ���ꂽ...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_ELEC, dir, damroll(4, 8))
		elseif artifact.activation == ACT_ACID1 then
			--msg_print(string.format("Your %s is covered in acid...", o_name))
			msg_print(string.format("%s���_�ɕ���ꂽ...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_ACID, dir, damroll(5, 8))
		elseif artifact.activation == ACT_ARROW then
			--msg_print(string.format("Your %s grows magical spikes...", o_name))
			msg_print(string.format("%s�ɖ��@�̃g�Q�����ꂽ...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_ARROW, dir, 150)
		elseif artifact.activation == ACT_HASTE1 then
			--msg_print(string.format("Your %s glows bright green...", o_name))
			msg_print(string.format("%s���O���[���ɖ��邭�P����...", o_name))
			if player.fast == 0 then
				set_fast(randint(20) + 20)
			else
				set_fast(player.fast + 5)
			end
		elseif artifact.activation == ACT_REM_FEAR_POIS then
			--msg_print(string.format("Your %s glows deep blue...", o_name))
			msg_print(string.format("%s���[���u���[�ɋP����...", o_name))
			set_afraid(0)
			set_poisoned(0)
		elseif artifact.activation == ACT_STINKING_CLOUD then
			--msg_print(string.format("Your %s throbs deep green...", o_name))
			msg_print(string.format("%s���[���ΐF�Ɍۓ����Ă���...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_ball(GF_POIS, dir, 12, 3)
		elseif artifact.activation == ACT_FROST2 then
			--msg_print(string.format("Your %s is covered in frost...", o_name))
			msg_print(string.format("%s����C�ɕ���ꂽ...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_ball(GF_COLD, dir, 48, 2)
		elseif artifact.activation == ACT_FROST4 then
			--msg_print(string.format("Your %s glows a pale blue...", o_name))
			msg_print(string.format("%s���W���u���[�ɋP����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_COLD, dir, damroll(12, 8))
		elseif artifact.activation == ACT_FROST3 then
			--msg_print(string.format("Your %s glows a intense blue...", o_name))
			msg_print(string.format("%s�����������P����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_ball(GF_COLD, dir, 100, 2)
		elseif artifact.activation == ACT_FIRE2 then
			--msg_print(string.format("Your %s rages in fire...", o_name))
			msg_print(string.format("%s���牊�������o����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_ball(GF_FIRE, dir, 72, 2)
		elseif artifact.activation == ACT_DRAIN_LIFE2 then
			--msg_print(string.format("Your %s glows black...", o_name))
			msg_print(string.format("%s�������P����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			drain_life(dir, 120)
		elseif artifact.activation == ACT_STONE_TO_MUD then
			--msg_print(string.format("Your %s pulsates...", o_name))
			msg_print(string.format("%s���ۓ�����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			wall_to_mud(dir)
		elseif artifact.activation == ACT_MASS_BANISHMENT then
			--msg_print(string.format("Your %s lets out a long, shrill note...", o_name))
			msg_print(string.format("%s����Ђǂ��s����������o��...", o_name))
			mass_banishment()
		elseif artifact.activation == ACT_CURE_WOUNDS then
			--msg_print(string.format("Your %s radiates deep purple...", o_name))
			msg_print(string.format("%s���[���̌�����˂���...", o_name))
			hp_player(damroll(4, 8))
			set_cut((player.cut / 2) - 50)
		elseif artifact.activation == ACT_TELE_AWAY then
			--msg_print(string.format("Your %s glows deep red...", o_name))
			msg_print(string.format("%s���[�g�ɋP����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			teleport_monster(dir)
		elseif artifact.activation == ACT_WOR then
			--msg_print(string.format("Your %s glows soft white...", o_name))
			msg_print(string.format("%s���_�炩�������P����...", o_name))
			set_recall()
		elseif artifact.activation == ACT_CONFUSE then
			--msg_print(string.format("Your %s glows in scintillating colours...", o_name))
			msg_print(string.format("%s���l�X�ȐF�̉ΉԂ𔭂���...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			confuse_monster(dir, 20)
		elseif artifact.activation == ACT_IDENTIFY then
			--msg_print(string.format("Your %s glows yellow...", o_name))
			msg_print(string.format("%s�����F���P����...", o_name))
			if not ident_spell() then return false, false end
		elseif artifact.activation == ACT_PROBE then
			--msg_print(string.format("Your %s glows brightly...", o_name))
			msg_print(string.format("%s�����邭�P����...", o_name))
			probing()
		elseif artifact.activation == ACT_DRAIN_LIFE1 then
			--msg_print(string.format("Your %s glows white...", o_name))
			msg_print(string.format("%s�������P����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			drain_life(dir, 90)
		elseif artifact.activation == ACT_FIREBRAND then
			--msg_print(string.format("Your %s glows deep red...", o_name))
			msg_print(string.format("%s���[�g�ɋP����...", o_name))
			if not brand_bolts() then return false, false end
		elseif artifact.activation == ACT_STARLIGHT then
			--msg_print(string.format("Your %s glows with the light of a thousand stars...", o_name))
			msg_print(string.format("%s������̐��̌��ŋP����...", o_name))
			for i = 1, 9 do strong_lite_line(ddd[i]) end
		elseif artifact.activation == ACT_MANA_BOLT then
			--msg_print(string.format("Your %s glows white...", o_name))
			msg_print(string.format("%s�������P����...", o_name))
			success, dir = get_aim_dir()
			if not success then return false, false end
			fire_bolt(GF_MANA, dir, damroll(12, 8))
		elseif artifact.activation == ACT_BERSERKER then
			--msg_print(string.format("Your %s glows in anger...", o_name))
			msg_print(string.format("%s���{��悤�ɋP����...", o_name))
			set_shero(player.shero + randint(50) + 50)
		end

		-- Set the recharge time
		if artifact.randtime then
			object.timeout = artifact.time + randint(artifact.randtime)
		else
			object.timeout = artifact.time
		end

		-- Window stuff
		player.window = bitlib.bOr(player.window, PW_INVEN, PW_EQUIP)

		return false, false
	end


	-- Dragon Scale Mail
	if object.tval == TV_DRAG_ARMOR then
		-- Get a direction for breathing (or abort)
		success, dir = get_aim_dir()
		if not success then return false, false end

		-- Branch on the sub-type
		if object.sval == SV_DRAGON_BLUE then
			sound(MSG_BR_ELEC)
			--msg_print("You breathe lightning.")
			msg_print("���Ȃ��͈�Ȃ̃u���X��f�����B")
			fire_ball(GF_ELEC, dir, 100, 2)
			object.timeout = rand_int(450) + 450
		elseif object.sval == SV_DRAGON_WHITE then
			sound(MSG_BR_FROST)
			--msg_print("You breathe frost.")
			msg_print("���Ȃ��͗�C�̃u���X��f�����B")
			fire_ball(GF_COLD, dir, 110, 2)
			object.timeout = rand_int(450) + 450
		elseif object.sval == SV_DRAGON_BLACK then
			sound(MSG_BR_ACID)
			--msg_print("You breathe acid.")
			msg_print("���Ȃ��͎_�̃u���X��f�����B")
			fire_ball(GF_ACID, dir, 130, 2)
			object.timeout = rand_int(450) + 450
		elseif object.sval == SV_DRAGON_GREEN then
			sound(MSG_BR_GAS)
			--msg_print("You breathe poison gas.")
			msg_print("���Ȃ��͓ŃK�X�̃u���X��f�����B")
			fire_ball(GF_POIS, dir, 150, 2)
			object.timeout = rand_int(450) + 450
		elseif object.sval == SV_DRAGON_RED then
			sound(MSG_BR_FIRE)
			--msg_print("You breathe fire.")
			msg_print("���Ȃ��͉Ή��̃u���X��f�����B")
			fire_ball(GF_FIRE, dir, 200, 2)
			object.timeout = rand_int(450) + 450
		elseif object.sval == SV_DRAGON_MULTIHUED then
			local chance = randint(5)
			--local name = {"fire", "lightning", "frost", "acid", "poison gas"}
			local name = {"�Ή�", "���", "��C", "�_", "�ŃK�X"}
			local effect = {GF_FIRE, GF_ELEC, GF_COLD, GF_ACID, GF_POIS}
			local soundfx = {MSG_BR_FIRE,MSG_BR_ELEC,MSG_BR_FROST,MSG_BR_ACID,MSG_BR_GAS}
			sound(soundfx[chance])
			--msg_print(string.format("You breathe %s.", name[chance]))
			msg_print("���Ȃ���%s�̃u���X��f�����B", name[chance])
			fire_ball(effect[chance], dir, 250, 2)
			object.timeout = rand_int(225) + 225
		elseif object.sval == SV_DRAGON_BRONZE then
			sound(MSG_BR_CONF);
			--msg_print("You breathe confusion.")
			msg_print("���Ȃ��͍����̃u���X��f�����B")
			fire_ball(GF_CONFUSION, dir, 120, 2)
			object.timeout = rand_int(450) + 450
		elseif object.sval == SV_DRAGON_GOLD then
			sound(MSG_BR_SOUND)
			--msg_print("You breathe sound.")
			msg_print("���Ȃ��͍����̃u���X��f�����B")
			fire_ball(GF_SOUND, dir, 130, 2)
			object.timeout = rand_int(450) + 450
		elseif object.sval == SV_DRAGON_CHAOS then
			local chance = randint(2)
			--local name = {"disenchantment", "chaos"}
			local name = {"��", "�J�I�X"}
			local effect = {GF_DISENCHANT, GF_CHAOS}
			local soundfx = {MSG_BR_DISENCHANT, MSG_BR_CHAOS}
			sound(soundfx[chance])
			--msg_print(string.format("You breathe %s.", name[chance]))
			msg_print("���Ȃ���%s�̃u���X��f�����B", name[chance])
			fire_ball(effect[chance], dir, 220, 2)
			object.timeout = rand_int(300) + 300
		elseif object.sval == SV_DRAGON_LAW then
			local chance = randint(2)
			--local name = {"shards", "sound"}
			local name = {"�j��", "����"}
			local effect = {GF_SHARD, GF_SOUND}
			local soundfx = {MSG_BR_SHARDS, MSG_BR_SOUND}
			sound(soundfx[chance])
			--msg_print(string.format("You breathe %s.", name[chance]))
			msg_print("���Ȃ���%s�̃u���X��f�����B", name[chance])
			fire_ball(effect[chance], dir, 230, 2)
			object.timeout = rand_int(300) + 300
		elseif object.sval == SV_DRAGON_BALANCE then
			local chance = randint(4)
			--local name = {"shards", "sound", "chaos", "disenchantment"}
			local name = {"�j��", "����", "�J�I�X", "��"}
			local effect = {GF_SHARD, GF_SOUND, GF_CHAOS, GF_DISENCHANT}
			local soundfx = {MSG_BR_SHARDS, MSG_BR_SOUND, MSG_BR_CHAOS, MSG_BR_DISENCHANT}
			sound(soundfx[chance])
			--msg_print(string.format("You breathe %s.", name[chance]))
			msg_print("���Ȃ���%s�̃u���X��f�����B", name[chance])
			fire_ball(effect[chance], dir, 250, 2)
			object.timeout = rand_int(300) + 300
		elseif object.sval == SV_DRAGON_SHINING then
			local chance = randint(2)
			--local name = {"light", "darkness"}
			local name = {"�M��", "�Í�"}
			local effect = {GF_LITE, GF_DARK}
			local soundfx = {MSG_BR_LIGHT, MSG_BR_DARK}
			sound(soundfx[chance])
			--msg_print(string.format("You breathe %s.", name[chance]))
			msg_print("���Ȃ���%s�̃u���X��f�����B", name[chance])
			fire_ball(effect[chance], dir, 200, 2)
			object.timeout = rand_int(300) + 300
		elseif object.sval == SV_DRAGON_POWER then
			sound(MSG_BR_ELEMENTS)
			--msg_print("You breathe the elements.")
			msg_print("���Ȃ��̓G�������g�̃u���X��f�����B")
			fire_ball(GF_MISSILE, dir, 300, 2)
			object.timeout = rand_int(300) + 300
		end

		-- Window stuff
		player.window = bitlib.bOr(player.window, PW_INVEN, PW_EQUIP)

		return false, false
	end

	-- Some Rings can be activated for double resist and element ball
	if object.tval == TV_RING then
		-- Get a direction for firing (or abort)
		success, dir = get_aim_dir()
		if not success then return false, false end

		-- Branch on the sub-type
		if object.sval == SV_RING_ACID then
			fire_ball(GF_ACID, dir, 70, 2)
			set_oppose_acid(player.oppose_acid + randint(20) + 20)
			object.timeout = rand_int(50) + 50
		elseif object.sval == SV_RING_FLAMES then
			fire_ball(GF_FIRE, dir, 80, 2)
			set_oppose_fire(player.oppose_fire + randint(20) + 20)
			object.timeout = rand_int(50) + 50
		elseif object.sval == SV_RING_ICE then
			fire_ball(GF_COLD, dir, 75, 2)
			set_oppose_cold(player.oppose_cold + randint(20) + 20)
			object.timeout = rand_int(50) + 50
		elseif object.sval == SV_RING_LIGHTNING then
			fire_ball(GF_ELEC, dir, 85, 2)
			set_oppose_elec(player.oppose_elec + randint(20) + 20)
			object.timeout = rand_int(50) + 50
		end

		-- Window stuff
		player.window = bitlib.bOr(player.window, PW_EQUIP)

		return false, false
	end

	-- Mistake
	--msg_print("Oops.  That object cannot be activated.")
	msg_print("�����ƁA���̃A�C�e���͎n���ł��Ȃ��B")
end


function use_object_hook(object)
	local ident = false
	local used = false

	if object.tval == TV_FOOD then
		ident, used = eat_food(object)
	elseif object.tval == TV_POTION then
		ident, used = quaff_potion(object)
	elseif object.tval == TV_SCROLL then
		ident, used = read_scroll(object)
	elseif object.tval == TV_STAFF then
		ident, used = use_staff(object)
	elseif object.tval == TV_WAND then
		ident, used = aim_wand(object)
	elseif object.tval == TV_ROD then
		ident, used = zap_rod(object)
	else
		ident, used = activate_object(object)
	end

	return ident, used
end


function describe_item_activation_hook(object)
	-- Artifact activations
	if object.name1 > 0 then
		local act_description = {
			--"illumination",
			"�C���~�l�[�V����",
			--"magic mapping",
			"���@�̒n�}",
			--"clairvoyance",
			"�痢��",
			--"protection from evil",
			"�Ύ׈����E",
			--"dispel evil (x5)",
			"�׈��ގU (x5)",
			--"heal (500)",
			"�̗͉� (500)",
			--"heal (1000)",
			"�̗͉� (1000)",
			--"cure wounds (4d8)",
			"���̎��� (4d8)",
			--"haste self (20+d20 turns)",
			"�X�s�[�h (20+d20�^�[��)",
			--"haste self (75+d75 turns)",
			"�X�s�[�h (75+d75�^�[��)",
			--"fire bolt (9d8)",
			"�t�@�C�A�E�{���g (9d8)",
			--"fire ball (72)",
			"�t�@�C�A�E�{�[�� (72)",
			--"large fire ball (120)",
			"����t�@�C�A�E�{�[�� (120)",
			--"frost bolt (6d8)",
			"�A�C�X�E�{���g (6d8)",
			--"frost ball (48)",
			"�A�C�X�E�{�[�� (48)",
			--"frost ball (100)",
			"�A�C�X�E�{�[�� (100)",
			--"frost bolt (12d8)",
			"�A�C�X�E�{���g (12d8)",
			--"large frost ball (200)",
			"����A�C�X�E�{�[�� (200)",
			--"acid bolt (5d8)",
			"�A�V�b�h�E�{���g (5d8)",
			--"recharge item I",
			"���͏[�U(I)",
			--"sleep II",
			"�X���[�v(II)",
			--"lightning bolt (4d8)",
			"�T���_�[�E�{���g (4d8)",
			--"large lightning ball (250)",
			"����T���_�[�E�{�[�� (250)",
			--"banishment",
			"�Ǖ�",
			--"mass banishment",
			"���ӒǕ�",
			--"identify",
			"�Ӓ�",
			--"drain life (90)",
			"�����͋z�� (90)",
			--"drain life (120)",
			"�����͋z�� (120)",
			--"bizarre things",
			"�M�������",
			--"star ball (150)",
			"�X�^�[�E�{�[�� (150)",
			--"berserk rage, bless, and resistance",
			"��b��/�j��/�S�ϐ�",
			--"phase door",
			"�t�F�C�Y�E�h�A",
			--"door and trap destruction",
			"�h�A/�g���b�v����",
			--"detection",
			"�S���m",
			--"resistance (20+d20 turns)",
			"�S�ϐ� (20+d20�^�[��)",
			--"teleport",
			"�e���|�[�g",
			--"restore life levels",
			"�����͕���",
			--"magic missile (2d6)",
			"�}�W�b�N�E�~�T�C�� (2d6)",
			--"a magical arrow (150)",
			"���@�̖� (150)",
			--"remove fear and cure poison",
			"���|����/�ŏ���",
			--"stinking cloud (12)",
			"���L�_ (12)",
			--"stone to mud",
			"��Ηn��",
			--"teleport away",
			"�e���|�[�g�E�A�E�F�C",
			--"word of recall",
			"�A�҂̏�",
			--"confuse monster",
			"�p�j�b�N�E�����X�^�[",
			--"probing",
			"����",
			--"fire branding of bolts",
			"�n��̃t�@�C�A�E�{���g",
			--"starlight (10d8)",
			"�X�^�[���C�g (10d8)",
			--"mana bolt (12d8)",
			"���̖͂� (12d8)",
			--"berserk rage (50+d50 turns)"}
			"��b�� (50+d50�^�[��)"}

		local artifact = a_info[object.name1 + 1]

		-- Paranoia
		if artifact.activation >= ACT_MAX then return "" end

		-- Some artifacts can be activated
		local description = act_description[artifact.activation + 1]

		-- Output the number of turns
		if (artifact.time > 0) and (artifact.randtime > 0) then
			--description = string.format("%s every %d+d%d turns", description, artifact.time, artifact.randtime)
			description = string.format("%s : %d+d%d �^�[����", description, artifact.time, artifact.randtime)
		elseif (artifact.time > 0) then
			--description = string.format("%s every %d turns", description, artifact.time)
			description = string.format("%s : %d �^�[����", description, artifact.time)
		elseif (artifact.randtime > 0) then
			--description = string.format("%s every d%d turns", description, artifact.randtime)
			description = string.format("%s : d%d �^�[����", description, artifact.randtime)
		end

		return description
	end

	-- Dragon scale mail
	if object.tval == TV_DRAG_ARMOR then
		local activations = {
			--[SV_DRAGON_BLUE] = "breathe lightning (100) every 450+d450 turns",
			[SV_DRAGON_BLUE] = "��Ȃ̃u���X(100) : 450+d450 �^�[����",
			--[SV_DRAGON_WHITE] = "breathe frost (110) every 450+d450 turns",
			[SV_DRAGON_WHITE] = "��C�̃u���X(110) : 450+d450 �^�[����",
			--[SV_DRAGON_BLACK] = "breathe acid (130) every 450+d450 turns",
			[SV_DRAGON_BLACK] = "�_�̃u���X(130) : 450+d450 �^�[����",
			--[SV_DRAGON_GREEN] = "breathe poison gas (150) every 450+d450 turns",
			[SV_DRAGON_GREEN] = "�ŃK�X�̃u���X(150) : 450+d450 �^�[����",
			--[SV_DRAGON_RED] = "breathe fire (200) every 450+d450 turns",
			[SV_DRAGON_RED] = "�Ή��̃u���X(200) : 450+d450 �^�[����",
			--[SV_DRAGON_MULTIHUED] = "breathe multi-hued (250) every 225+d225 turns",
			[SV_DRAGON_MULTIHUED] = "���F�̃u���X(250) : 225+d225 �^�[����",
			--[SV_DRAGON_BRONZE] = "breathe confusion (120) every 450+d450 turns",
			[SV_DRAGON_BRONZE] = "�����̃u���X(120) : 450+d450 �^�[����",
			--[SV_DRAGON_GOLD] = "breathe sound (130) every 450+d450 turns",
			[SV_DRAGON_GOLD] = "�����̃u���X(130) : 450+d450 �^�[����",
			--[SV_DRAGON_CHAOS] = "breathe chaos/disenchant (220) every 300+d300 turns",
			[SV_DRAGON_CHAOS] = "�J�I�X/�򉻂̃u���X(220) : 300+d300 �^�[����",
			--[SV_DRAGON_LAW] = "breathe sound/shards (230) every 300+d300 turns",
			[SV_DRAGON_LAW] = "����/�j�Ђ̃u���X(230) : 300+d300 �^�[����",
			--[SV_DRAGON_BALANCE] = "breathe balance (250) every 300+d300 turns",
			[SV_DRAGON_BALANCE] = "�o�����X�̃u���X(250) : 300+d300 �^�[����",
			--[SV_DRAGON_SHINING] = "breathe light/darkness (200) every 300+d300 turns",
			[SV_DRAGON_SHINING] = "�M��/�Í��̃u���X(200) : 300+d300 �^�[����",
			--[SV_DRAGON_POWER] = "breathe the elements (300) every 300+d300 turns",
			[SV_DRAGON_POWER] = "�G�������g�̃u���X(300) : 300+d300 �^�[����",
		}

		return activations[object.sval]
	end

	-- Elemental rings
	if object.tval == TV_RING then
		local activations = {
			--[SV_RING_ACID] = "acid resistance (20+d20 turns) and acid ball (70) every 50+d50 turns",
			[SV_RING_ACID] = "�ώ_ (20+d20�^�[��), �A�V�b�h�E�{�[�� (70) : 50+d50 �^�[����",
			--[SV_RING_FLAMES] = "fire resistance (20+d20 turns) and fire ball (80) every 50+d50 turns",
			[SV_RING_FLAMES] = "�ω� (20+d20�^�[��), �t�@�C�A�E�{�[�� (80) : 50+d50 �^�[����",
			--[SV_RING_ICE] = "cold resistance (20+d20 turns) and cold ball (75) every 50+d50 turns",
			[SV_RING_ICE] = "�ϗ� (20+d20�^�[��),  �A�C�X�E�{�[�� (75) : 50+d50 �^�[����",
			--[SV_RING_LIGHTNING] = "electricity resistance (20+d20 turns) and electricity ball (85) every 50+d50 turns"}
			[SV_RING_LIGHTNING] = "�ϓd (20+d20�^�[��),  �T���_�[�E�{�[�� (85) : 50+d50 �^�[����"}
	
		return activations[object.sval]
	end

	-- No activation
	return ""
end


-- Add event handlers
add_event_handler("use_object", use_object_hook)
add_event_handler("describe_item_activation", describe_item_activation_hook)
