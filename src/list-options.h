/**
 * \file list-options.h
 * \brief options
 *
 * Currently, if there are more than 21 of any option type, the later ones
 * will be ignored
 * Cheat options need to be followed by corresponding score options
 */

/* name                   description
type     normal */
OP(none,                  "",
SPECIAL, false)
OP(rogue_like_commands,   N_("Use the roguelike command keyset"),
INTERFACE, false)
OP(use_sound,             N_("Use sound"),
INTERFACE, false)
OP(show_damage,           N_("Show damage player deals to monsters"),
INTERFACE, false)
OP(use_old_target,        N_("Use old target by default"),
INTERFACE, false)
OP(pickup_always,         N_("Always pickup items"),
INTERFACE, false)
OP(pickup_inven,          N_("Always pickup items matching inventory"),
INTERFACE, true)
OP(show_flavors,          N_("Show flavors in object descriptions"),
INTERFACE, false)
OP(show_target,           N_("Highlight target with cursor"),
INTERFACE, true)
OP(highlight_player,      N_("Highlight player with cursor between turns"),
INTERFACE, false)
OP(disturb_near,          N_("Disturb whenever viewable monster moves"),
INTERFACE, true)
OP(solid_walls,           N_("Show walls as solid blocks"),
INTERFACE, false)
OP(hybrid_walls,          N_("Show walls with shaded background"),
INTERFACE, false)
OP(view_yellow_light,     N_("Color: Illuminate torchlight in yellow"),
INTERFACE, false)
OP(animate_flicker,       N_("Color: Shimmer multi-colored things"),
INTERFACE, false)
OP(center_player,         N_("Center map continuously"),
INTERFACE, false)
OP(purple_uniques,        N_("Color: Show unique monsters in purple"),
INTERFACE, false)
OP(auto_more,             N_("Automatically clear '-more-' prompts"),
INTERFACE, false)
OP(hp_changes_color,      N_("Color: Player color indicates % hit points"),
INTERFACE, true)
OP(mouse_movement,        N_("Allow mouse clicks to move the player"),
INTERFACE, true)
OP(notify_recharge,       N_("Notify on object recharge"),
INTERFACE, false)
OP(effective_speed,       N_("Show effective speed as multiplier"),
INTERFACE, false)
OP(cheat_hear,            N_("Cheat: Peek into monster creation"),
CHEAT, false)
OP(score_hear,            N_("Score: Peek into monster creation"),
SCORE, false)
OP(cheat_room,            N_("Cheat: Peek into dungeon creation"),
CHEAT, false)
OP(score_room,            N_("Score: Peek into dungeon creation"),
SCORE, false)
OP(cheat_xtra,            N_("Cheat: Peek into something else"),
CHEAT, false)
OP(score_xtra,            N_("Score: Peek into something else"),
SCORE, false)
OP(cheat_live,            N_("Cheat: Allow player to avoid death"),
CHEAT, false)
OP(score_live,            N_("Score: Allow player to avoid death"),
SCORE, false)
OP(birth_randarts,        N_("Generate a new, random artifact set"),
BIRTH, false)
OP(birth_connect_stairs,  N_("Generate connected stairs"),
BIRTH, true)
OP(birth_force_descend,   N_("Force player descent (never make up stairs)"),
BIRTH, false)
OP(birth_no_recall,       N_("Word of Recall has no effect"),
BIRTH, false)
OP(birth_no_artifacts,    N_("Restrict creation of artifacts"),
BIRTH, false)
OP(birth_stacking,        N_("Stack objects on the floor"),
BIRTH, true)
OP(birth_lose_arts,       N_("Lose artifacts when leaving level"),
BIRTH, false)
OP(birth_feelings,        N_("Show level feelings"),
BIRTH, true)
OP(birth_no_selling,      N_("Increase gold drops but disable selling"),
BIRTH, true)
OP(birth_start_kit,       N_("Start with a kit of useful gear"),
BIRTH, true)
OP(birth_ai_learn,        N_("Monsters learn from their mistakes"),
BIRTH, true)
OP(birth_know_runes,      N_("Know all runes on birth"),
BIRTH, false)
OP(birth_know_flavors,    N_("Know all flavors on birth"),
BIRTH, false)
OP(birth_levels_persist,  N_("Persistent levels (experimental)"),
BIRTH, false)
OP(birth_percent_damage,  N_("To-damage is a percentage of dice (experimental)"),
BIRTH, false)

