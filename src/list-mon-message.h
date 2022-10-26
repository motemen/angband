/**
 * \file list-mon-message.h
 * \brief List of monster message types
 *
 * id           - the message constant name
 * msg          - MSG_ type for the printed message
 * omit_subject - true to omit a monster name beforehand (see mon-msg.c)
 * text         - the message text
 */

/* id						MSG_GENERIC,	text */
MON_MSG(NONE,				MSG_GENERIC,	false,	N_GAMEDATA("[is|are] hurt."))
MON_MSG(DIE,				MSG_KILL,		false,	N_GAMEDATA("die[s]."))
MON_MSG(DESTROYED,			MSG_KILL,		false,	N_GAMEDATA("[is|are] destroyed."))
MON_MSG(RESIST_A_LOT,		MSG_GENERIC,	false,	N_GAMEDATA("resist[s] a lot."))
MON_MSG(HIT_HARD,			MSG_GENERIC,	false,	N_GAMEDATA("[is|are] hit hard."))
MON_MSG(RESIST,				MSG_GENERIC,	false,	N_GAMEDATA("resist[s]."))
MON_MSG(IMMUNE,				MSG_GENERIC,	false,	N_GAMEDATA("[is|are] immune."))
MON_MSG(RESIST_SOMEWHAT,	MSG_GENERIC,	false,	N_GAMEDATA("resist[s] somewhat."))
MON_MSG(UNAFFECTED,			MSG_GENERIC,	false,	N_GAMEDATA("[is|are] unaffected!"))
MON_MSG(SPAWN,				MSG_GENERIC,	false,	N_GAMEDATA("spawn[s]!"))
MON_MSG(HEALTHIER,			MSG_GENERIC,	false,	N_GAMEDATA("look[s] healthier."))
MON_MSG(FALL_ASLEEP,		MSG_GENERIC,	false,	N_GAMEDATA("fall[s] asleep!"))
MON_MSG(WAKES_UP,			MSG_GENERIC,	false,	N_GAMEDATA("wake[s] up."))
MON_MSG(CRINGE_LIGHT,		MSG_GENERIC,	false,	N_GAMEDATA("cringe[s] from the light!"))
MON_MSG(SHRIVEL_LIGHT,		MSG_KILL,		false,	N_GAMEDATA("shrivel[s] away in the light!"))
MON_MSG(LOSE_SKIN,			MSG_GENERIC,	false,	N_GAMEDATA("lose[s] some skin!"))
MON_MSG(DISSOLVE,			MSG_KILL,		false,	N_GAMEDATA("dissolve[s]!"))
MON_MSG(CATCH_FIRE,			MSG_GENERIC,	false,	N_GAMEDATA("catch[es] fire!"))
MON_MSG(BADLY_FROZEN,		MSG_GENERIC,	false,	N_GAMEDATA("[is|are] badly frozen."))
MON_MSG(SHUDDER,			MSG_GENERIC,	false,	N_GAMEDATA("shudder[s]."))
MON_MSG(CHANGE,				MSG_GENERIC,	false,	N_GAMEDATA("change[s]!"))
MON_MSG(DISAPPEAR,			MSG_GENERIC,	false,	N_GAMEDATA("disappear[s]!"))
MON_MSG(MORE_DAZED,			MSG_GENERIC,	false,	N_GAMEDATA("[is|are] even more stunned."))
MON_MSG(DAZED,				MSG_GENERIC,	false,	N_GAMEDATA("[is|are] stunned."))
MON_MSG(NOT_DAZED,			MSG_GENERIC,	false,	N_GAMEDATA("[is|are] no longer stunned."))
MON_MSG(MORE_CONFUSED,		MSG_GENERIC,	false,	N_GAMEDATA("look[s] more confused."))
MON_MSG(CONFUSED,			MSG_GENERIC,	false,	N_GAMEDATA("look[s] confused."))
MON_MSG(NOT_CONFUSED,		MSG_GENERIC,	false,	N_GAMEDATA("[is|are] no longer confused."))
MON_MSG(MORE_SLOWED,		MSG_GENERIC,	false,	N_GAMEDATA("look[s] more slowed."))
MON_MSG(SLOWED,				MSG_GENERIC,	false,	N_GAMEDATA("look[s] slowed."))
MON_MSG(NOT_SLOWED	,		MSG_GENERIC,	false,	N_GAMEDATA("speed[s] up."))
MON_MSG(MORE_HASTED,		MSG_GENERIC,	false,	N_GAMEDATA("look[s] even faster!"))
MON_MSG(HASTED,				MSG_GENERIC,	false,	N_GAMEDATA("start[s] moving faster."))
MON_MSG(NOT_HASTED,			MSG_GENERIC,	false,	N_GAMEDATA("slow[s] down."))
MON_MSG(MORE_AFRAID,		MSG_GENERIC,	false,	N_GAMEDATA("look[s] more terrified!"))
MON_MSG(FLEE_IN_TERROR,		MSG_FLEE,		false,	N_GAMEDATA("flee[s] in terror!"))
MON_MSG(NOT_AFRAID,			MSG_GENERIC,	false,	N_GAMEDATA("[is|are] no longer afraid."))
MON_MSG(HELD,				MSG_GENERIC,    false,  N_GAMEDATA("[is|are] frozen to the spot!"))
MON_MSG(NOT_HELD,			MSG_GENERIC,    false,  N_GAMEDATA("can move again."))
MON_MSG(DISEN,				MSG_GENERIC,    false,  N_GAMEDATA("seem[s] less magical!"))
MON_MSG(NOT_DISEN,			MSG_GENERIC,    false,  N_GAMEDATA("seem[s] magical again."))
MON_MSG(COMMAND,			MSG_GENERIC,    false,  N_GAMEDATA("falls under your spell!"))
MON_MSG(NOT_COMMAND,		MSG_GENERIC,    false,  N_GAMEDATA("is no longer under your control."))
MON_MSG(SHAPE_FAIL,			MSG_GENERIC,    false,  N_GAMEDATA("shimmers for a moment."))
MON_MSG(MORIA_DEATH,		MSG_KILL,		true,	N_GAMEDATA("You hear [a|several] scream[|s] of agony!"))
MON_MSG(DISINTEGRATES,		MSG_KILL,		false,	N_GAMEDATA("disintegrate[s]!"))
MON_MSG(FREEZE_SHATTER,		MSG_KILL,		false,	N_GAMEDATA("freeze[s] and shatter[s]!"))
MON_MSG(MANA_DRAIN,			MSG_GENERIC,	false,	N_GAMEDATA("lose[s] some mana!"))
MON_MSG(BRIEF_PUZZLE,		MSG_GENERIC,	false,	N_GAMEDATA("look[s] briefly puzzled."))
MON_MSG(MAINTAIN_SHAPE,		MSG_GENERIC,	false,	N_GAMEDATA("maintain[s] the same shape."))
MON_MSG(UNHARMED,			MSG_GENERIC,	false,	N_GAMEDATA("[is|are] unharmed."))
MON_MSG(APPEAR,			MSG_GENERIC,	false,	N_GAMEDATA("appear[s]!"))
MON_MSG(HIT_AND_RUN,		MSG_GENERIC,	true,	N_GAMEDATA("There is a puff of smoke!"))
/* Dummy messages for monster pain - we use edit file info instead. */
MON_MSG(95,					MSG_GENERIC,	false,	"")
MON_MSG(75,					MSG_GENERIC,	false,	"")
MON_MSG(50,					MSG_GENERIC,	false,	"")
MON_MSG(35,					MSG_GENERIC,	false,	"")
MON_MSG(20,					MSG_GENERIC,	false,	"")
MON_MSG(10,					MSG_GENERIC,	false,	"")
MON_MSG(0,					MSG_GENERIC,	false,	"")
MON_MSG(MAX,				MSG_GENERIC,	false,	"")
