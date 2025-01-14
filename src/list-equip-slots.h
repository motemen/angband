/**
 * \file list-equip-slots.h
 * \brief types of slot for equipment
 *
 * Fields:
 * slot - The index name of the slot
 * acid_v - whether equipment in the slot needs checking for acid damage
 * name - whether the actual item name is mentioned when things happen to it
 * mention - description for when the slot is mentioned briefly
 * heavy describe - description for when the slot item is too heavy
 * describe - description for when the slot is described at length
 */
/* slot				acid_v	name	mention			heavy decribe	describe */
EQUIP(NONE,			false,	false,	"",				"",				"")
EQUIP(WEAPON,		false,	false,	N_GAMEDATA("Wielding"),		N_GAMEDATA("just lifting"),	N_GAMEDATA("attacking monsters with"))
EQUIP(BOW,			false,	false,	N_GAMEDATA("Shooting"),		N_GAMEDATA("just holding"),	N_GAMEDATA("shooting missiles with"))
EQUIP(RING,			false,	true,	N_GAMEDATA("On %s"),		"",				N_GAMEDATA("wearing on your %s"))
EQUIP(AMULET,		false,	true,	N_GAMEDATA("Around %s"),	"",				N_GAMEDATA("wearing around your %s"))
EQUIP(LIGHT,		false,	false,	N_GAMEDATA("Light source"),	"",				N_GAMEDATA("using to light your way"))
EQUIP(BODY_ARMOR,	true,	true,	N_GAMEDATA("On %s"),		"",				N_GAMEDATA("wearing on your %s"))
EQUIP(CLOAK,		true,	true,	N_GAMEDATA("On %s"),		"",				N_GAMEDATA("wearing on your %s"))
EQUIP(SHIELD,		true,	true,	N_GAMEDATA("On %s"),		"",				N_GAMEDATA("wearing on your %s"))
EQUIP(HAT,			true,	true,	N_GAMEDATA("On %s"),		"",				N_GAMEDATA("wearing on your %s"))
EQUIP(GLOVES,		true,	true,	N_GAMEDATA("On %s"),		"",				N_GAMEDATA("wearing on your %s"))
EQUIP(BOOTS,		true,	true,	N_GAMEDATA("On %s"),		"",				N_GAMEDATA("wearing on your %s"))
