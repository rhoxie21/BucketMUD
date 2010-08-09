/***************************************************************************
 *  File: bit.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was written by Jason Dinkel and inspired by Russ Taylor,     *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
 The code below uses a table lookup system that is based on suggestions
 from Russ Taylor.  There are many routines in handler.c that would benefit
 with the use of tables.  You may consider simplifying your code base by
 implementing a system like below with such functions. -Jason Dinkel
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

struct flag_stat_type
{
    const struct flag_type *structure;
    bool stat;
};

/*****************************************************************************
 Name:          flag_stat_table
 Purpose:       This table catagorizes the tables following the lookup
		functions below into stats and flags.  Flags can be toggled
		but stats can only be assigned.  Update this table when a
		new set of flags is installed.
 ****************************************************************************/
static const struct flag_stat_type flag_stat_table[] =
{
    /*  {   structure               stat    }, */
    {area_flags, FALSE},
    {sex_flags, TRUE},
    {exit_flags, FALSE},
    {door_resets, TRUE},
    {room_flags, FALSE},
    {sector_flags, TRUE},
    {type_flags, TRUE},
    {extra_flags, FALSE},
    {wear_flags, FALSE},
    {act_flags, FALSE},
    {affect_flags, FALSE},
    {apply_flags, TRUE},
    {wear_loc_flags, TRUE},
    {wear_loc_strings, TRUE},
    {weapon_flags, TRUE},
    {container_flags, FALSE},
    {liquid_flags, TRUE},
    {mprog_type_flags, TRUE},
    {clan_flags, FALSE},
    {clan_join_flags, FALSE},

    /* ROM specific flags: */
    {furniture_flags, FALSE},
    {material_type, TRUE},
    {form_flags, FALSE},
    {part_flags, FALSE},
    {ac_type, TRUE},
    {size_flags, TRUE},
    {position_flags, TRUE},
    {imm_flags, FALSE},
    {res_flags, FALSE},
    {vuln_flags, FALSE},
    {weapon_class, TRUE},
    {weapon_type, FALSE},
    {rnd_obj_flags, FALSE},
    {0, 0}
};

/*****************************************************************************
 Name:          is_stat( table )
 Purpose:       Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:     flag_value and flag_string.
 Note:          This function is local and used only in bit.c.
 ****************************************************************************/
static bool is_stat(const struct flag_type *flag_table)
{
    int flag;

    for (flag = 0; flag_stat_table[flag].structure; flag++)
    {
        if (flag_stat_table[flag].structure == flag_table
                && flag_stat_table[flag].stat)
            return TRUE;
    }
    return FALSE;
}

/*
 * This function is Russ Taylor's creation.  Thanks Russ!
 * All code copyright (C) Russ Taylor, permission to use and/or distribute
 * has NOT been granted.  Use only in this OLC package has been granted.
 */
/*****************************************************************************
 Name:          flag_lookup( flag, table )
 Purpose:       Returns the value of a single, settable flag from the table.
 Called by:     flag_value and flag_string.
 Note:          This function is local and used only in bit.c.
 ****************************************************************************/
static int flag_lookup(const char *name,
                       const struct flag_type *flag_table)
{
    int flag;

    for (flag = 0; flag_table[flag].name[0] != '\0'; flag++)
    {
        if (!str_cmp(name, flag_table[flag].name)
                && flag_table[flag].settable)
            return flag_table[flag].bit;
    }

    return NO_FLAG;
}

/*****************************************************************************
 Name:          flag_value( table, flag )
 Purpose:       Returns the value of the flags entered.  Multi-flags accepted.
 Called by:     olc.c and olc_act.c.
 ****************************************************************************/
int flag_value(const struct flag_type *flag_table, char *argument)
{
    char word[MAX_INPUT_LENGTH];
    int bit;
    int marked = 0;
    bool found = FALSE;

    if (is_stat(flag_table))
    {
        one_argument(argument, word);

        if ((bit = flag_lookup(word, flag_table)) != NO_FLAG)
            return bit;
        else
            return NO_FLAG;
    }

    /*
     * Accept multiple flags.
     */
    for (;;)
    {
        argument = one_argument(argument, word);

        if (word[0] == '\0')
            break;

        if ((bit = flag_lookup(word, flag_table)) != NO_FLAG)
        {
            SET_BIT(marked, bit);
            found = TRUE;
        }
    }

    if (found)
        return marked;
    else
        return NO_FLAG;
}

/*****************************************************************************
 Name:          flag_string( table, flags/stat )
 Purpose:       Returns string with name(s) of the flags or stat entered.
 Called by:     act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
char *flag_string(const struct flag_type *flag_table, int bits)
{
    static char buf[512];
    int flag;

    buf[0] = '\0';

    for (flag = 0; flag_table[flag].name[0] != '\0'; flag++)
    {
        if (!is_stat(flag_table) && IS_SET(bits, flag_table[flag].bit))
        {
            strcat(buf, " ");
            strcat(buf, flag_table[flag].name);
        }
        else if (flag_table[flag].bit == bits)
        {
            strcat(buf, " ");
            strcat(buf, flag_table[flag].name);
            break;
        }
    }
    return (buf[0] != '\0') ? buf + 1 : "none";
}

const struct flag_type area_flags[] =
{
    {"none", AREA_NONE, FALSE},
    {"changed", AREA_CHANGED, TRUE},
    {"added", AREA_ADDED, TRUE},
    {"loading", AREA_LOADING, FALSE},
    {"", 0, 0}
};

const struct flag_type sex_flags[] =
{
    {"male", SEX_MALE, TRUE},
    {"female", SEX_FEMALE, TRUE},
    {"neutral", SEX_NEUTRAL, TRUE},
    {"random", 3, TRUE},	/* ROM */
    {"none", SEX_NEUTRAL, TRUE},
    {"", 0, 0}
};

const struct flag_type exit_flags[] =
{
    {"door", EX_ISDOOR, TRUE},
    {"closed", EX_CLOSED, TRUE},
    {"locked", EX_LOCKED, TRUE},
    {"pickproof", EX_PICKPROOF, TRUE},
    {"passproof", EX_PASSPROOF, TRUE},
    {"hidden", EX_HIDDEN, TRUE},
    {"", 0, 0}
};

const struct flag_type door_resets[] =
{
    {"open and unlocked", 0, TRUE},
    {"closed and unlocked", 1, TRUE},
    {"closed and locked", 2, TRUE},
    {"close,lock,pass", 3, TRUE},
    {"close,lock,pass,pick", 4, TRUE},
    {"closed,hidden", 5, TRUE},
    {"closed,hidden,locked", 6, TRUE},
    {"closed,hidden,locked,pass", 7, TRUE},
    {"closed,hidden,locked,pass,pick", 8, TRUE},
    {"closed,hidden,locked,pick", 9, TRUE},
    {"", 0, 0}
};

const struct flag_type room_flags[] =
{
    {"dark", ROOM_DARK, TRUE},
    {"no_mob", ROOM_NO_MOB, TRUE},
    {"no_flee_to", ROOM_NO_FLEE_TO, TRUE},
    {"arena", ROOM_ARENA, TRUE},
    {"indoors", ROOM_INDOORS, TRUE},
    {"private", ROOM_PRIVATE, TRUE},
    {"safe", ROOM_SAFE, TRUE},
    {"solitary", ROOM_SOLITARY, TRUE},
    {"pet_shop", ROOM_PET_SHOP, TRUE},
    {"no_recall", ROOM_NO_RECALL, TRUE},
    {"imp_only", ROOM_IMP_ONLY, TRUE},
    {"gods_only", ROOM_GODS_ONLY, TRUE},
    {"heroes_only", ROOM_HEROES_ONLY, TRUE},
    {"newbies_only", ROOM_NEWBIES_ONLY, TRUE},
    {"law", ROOM_LAW, TRUE},
    {"no_teleport", ROOM_NOTELEPORT, TRUE},
    {"no_magic", ROOM_NOMAGIC, TRUE},
    {"admin_only", ROOM_ADMIN_ONLY, TRUE},
    {"", 0, 0}
};

const struct flag_type sector_flags[] =
{
    {"inside", SECT_INSIDE, TRUE},
    {"city", SECT_CITY, TRUE},
    {"field", SECT_FIELD, TRUE},
    {"forest", SECT_FOREST, TRUE},
    {"hills", SECT_HILLS, TRUE},
    {"mountain", SECT_MOUNTAIN, TRUE},
    {"swim", SECT_WATER_SWIM, TRUE},
    {"noswim", SECT_WATER_NOSWIM, TRUE},
    {"air", SECT_AIR, TRUE},
    {"desert", SECT_DESERT, TRUE},
    {"", 0, 0}
};

const struct flag_type type_flags[] =
{
    {"light", ITEM_LIGHT, TRUE},
    {"scroll", ITEM_SCROLL, TRUE},
    {"wand", ITEM_WAND, TRUE},
    {"staff", ITEM_STAFF, TRUE},
    {"weapon", ITEM_WEAPON, TRUE},
    {"treasure", ITEM_TREASURE, TRUE},
    {"armor", ITEM_ARMOR, TRUE},
    {"potion", ITEM_POTION, TRUE},
    {"furniture", ITEM_FURNITURE, TRUE},
    {"trash", ITEM_TRASH, TRUE},
    {"container", ITEM_CONTAINER, TRUE},
    {"drink-container", ITEM_DRINK_CON, TRUE},
    {"key", ITEM_KEY, TRUE},
    {"food", ITEM_FOOD, TRUE},
    {"money", ITEM_MONEY, TRUE},
    {"boat", ITEM_BOAT, TRUE},
    {"npc corpse", ITEM_CORPSE_NPC, TRUE},
    {"pc corpse", ITEM_CORPSE_PC, FALSE},
    {"fountain", ITEM_FOUNTAIN, TRUE},
    {"pill", ITEM_PILL, TRUE},
    {"clothing", ITEM_CLOTHING, TRUE},
    {"protect", ITEM_PROTECT, TRUE},
    {"map", ITEM_MAP, TRUE},
    {"portal", ITEM_PORTAL, TRUE},
    {"", 0, 0}
};

const struct flag_type extra_flags[] =
{
    {"glow", ITEM_GLOW, TRUE},
    {"hum", ITEM_HUM, TRUE},
    {"dark", ITEM_DARK, TRUE},
    {"lock", ITEM_LOCK, TRUE},
    {"evil", ITEM_EVIL, TRUE},
    {"invis", ITEM_INVIS, TRUE},
    {"magic", ITEM_MAGIC, TRUE},
    {"nodrop", ITEM_NODROP, TRUE},
    {"bless", ITEM_BLESS, TRUE},
    {"anti-good", ITEM_ANTI_GOOD, TRUE},
    {"anti-evil", ITEM_ANTI_EVIL, TRUE},
    {"anti-neutral", ITEM_ANTI_NEUTRAL, TRUE},
    {"noremove", ITEM_NOREMOVE, TRUE},
    {"inventory", ITEM_INVENTORY, TRUE},
    {"nopurge", ITEM_NOPURGE, TRUE},
    {"rot-death", ITEM_ROT_DEATH, TRUE},
    {"vis-death", ITEM_VIS_DEATH, TRUE},
    {"anti-cleric", ITEM_ANTI_CLERIC, TRUE},
    {"anti-thief", ITEM_ANTI_THIEF, TRUE},
    {"anti-warrior", ITEM_ANTI_WARRIOR, TRUE},
    {"anti-mage", ITEM_ANTI_MAGE, TRUE},
    {"anti-male", ITEM_ANTI_MALE, TRUE},
    {"anti-neuter", ITEM_ANTI_NEUTER, TRUE},
    {"anti-female", ITEM_ANTI_FEMALE, TRUE},
    {"", 0, 0}
};

const struct flag_type wear_flags[] =
{
    {"take", ITEM_TAKE, TRUE},
    {"finger", ITEM_WEAR_FINGER, TRUE},
    {"neck", ITEM_WEAR_NECK, TRUE},
    {"body", ITEM_WEAR_BODY, TRUE},
    {"head", ITEM_WEAR_HEAD, TRUE},
    {"legs", ITEM_WEAR_LEGS, TRUE},
    {"feet", ITEM_WEAR_FEET, TRUE},
    {"hands", ITEM_WEAR_HANDS, TRUE},
    {"arms", ITEM_WEAR_ARMS, TRUE},
    {"shield", ITEM_WEAR_SHIELD, TRUE},
    {"about", ITEM_WEAR_ABOUT, TRUE},
    {"waist", ITEM_WEAR_WAIST, TRUE},
    {"wrist", ITEM_WEAR_WRIST, TRUE},
    {"wield", ITEM_WIELD, TRUE},
    {"hold", ITEM_HOLD, TRUE},
    {"two-hands", ITEM_TWO_HANDS, TRUE},
    {"", 0, 0}
};

const struct flag_type act_flags[] =
{
    {"npc", ACT_IS_NPC, FALSE},
    {"sentinel", ACT_SENTINEL, TRUE},
    {"scavenger", ACT_SCAVENGER, TRUE},
    {"aggressive", ACT_AGGRESSIVE, TRUE},
    {"stay-area", ACT_STAY_AREA, TRUE},
    {"wimpy", ACT_WIMPY, TRUE},
    {"pet", ACT_PET, TRUE},
    {"train", ACT_TRAIN, TRUE},
    {"practice", ACT_PRACTICE, TRUE},
    {"undead", ACT_UNDEAD, TRUE},
    {"cleric", ACT_CLERIC, TRUE},
    {"mage", ACT_MAGE, TRUE},
    {"thief", ACT_THIEF, TRUE},
    {"warrior", ACT_WARRIOR, TRUE},
    {"noalign", ACT_NOALIGN, TRUE},
    {"nopurge", ACT_NOPURGE, TRUE},
    {"no-steal", ACT_NO_STEAL, TRUE},
    {"healer", ACT_IS_HEALER, TRUE},
    {"gain", ACT_GAIN, TRUE},
    {"update-always", ACT_UPDATE_ALWAYS, TRUE},
    {"no-kill", ACT_NO_KILL, TRUE},
    {"mob-invis", ACT_MOBINVIS, TRUE},
    {"", 0, 0}
};

const struct flag_type affect_flags[] =
{
    {"blind", AFF_BLIND, TRUE},
    {"invisible", AFF_INVISIBLE, TRUE},
    {"detect-evil", AFF_DETECT_EVIL, TRUE},
    {"detect-invis", AFF_DETECT_INVIS, TRUE},
    {"detect-magic", AFF_DETECT_MAGIC, TRUE},
    {"detect-hidden", AFF_DETECT_HIDDEN, TRUE},
    {"hold", AFF_HOLD, TRUE},
    {"sanctuary", AFF_SANCTUARY, TRUE},
    {"faerie-fire", AFF_FAERIE_FIRE, TRUE},
    {"infrared", AFF_INFRARED, TRUE},
    {"curse", AFF_CURSE, TRUE},
    {"flaming", AFF_FLAMING, FALSE},
    {"poison", AFF_POISON, TRUE},
    {"protect", AFF_PROTECT, TRUE},
    {"paralysis", AFF_PARALYSIS, FALSE},
    {"sneak", AFF_SNEAK, TRUE},
    {"hide", AFF_HIDE, TRUE},
    {"sleep", AFF_SLEEP, TRUE},
    {"charm", AFF_CHARM, TRUE},
    {"flying", AFF_FLYING, TRUE},
    {"pass-door", AFF_PASS_DOOR, TRUE},
    {"haste", AFF_HASTE, TRUE},	/* ROM: */
    {"calm", AFF_CALM, TRUE},
    {"plague", AFF_PLAGUE, TRUE},
    {"weaken", AFF_WEAKEN, TRUE},
    {"dark-vision", AFF_DARK_VISION, TRUE},
    {"berserk", AFF_BERSERK, TRUE},
    {"swim", AFF_SWIM, TRUE},
    {"regeneration", AFF_REGENERATION, TRUE},
    {"web", AFF_WEB, TRUE},
    {"", 0, 0}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] =
{
    {"none", APPLY_NONE, TRUE},
    {"strength", APPLY_STR, TRUE},
    {"dexterity", APPLY_DEX, TRUE},
    {"intelligence", APPLY_INT, TRUE},
    {"wisdom", APPLY_WIS, TRUE},
    {"constitution", APPLY_CON, TRUE},
    {"sex", APPLY_SEX, TRUE},
    {"class", APPLY_CLASS, TRUE},
    {"level", APPLY_LEVEL, TRUE},
    {"age", APPLY_AGE, TRUE},
    {"height", APPLY_HEIGHT, TRUE},
    {"weight", APPLY_WEIGHT, TRUE},
    {"mana", APPLY_MANA, TRUE},
    {"hp", APPLY_HIT, TRUE},
    {"move", APPLY_MOVE, TRUE},
    {"gold", APPLY_GOLD, TRUE},
    {"experience", APPLY_EXP, TRUE},
    {"ac", APPLY_AC, TRUE},
    {"hitroll", APPLY_HITROLL, TRUE},
    {"damroll", APPLY_DAMROLL, TRUE},
    {"saving-para", APPLY_SAVING_PARA, TRUE},
    {"saving-rod", APPLY_SAVING_ROD, TRUE},
    {"saving-petri", APPLY_SAVING_PETRI, TRUE},
    {"saving-breath", APPLY_SAVING_BREATH, TRUE},
    {"saving-spell", APPLY_SAVING_SPELL, TRUE},
    {"alignment", APPLY_ALIGN, TRUE},
    {"", 0, 0}
};

/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] =
{
    {"in the inventory", WEAR_NONE, TRUE},
    {"as a light", WEAR_LIGHT, TRUE},
    {"on the left finger", WEAR_FINGER_L, TRUE},
    {"on the right finger", WEAR_FINGER_R, TRUE},
    {"around the neck (1)", WEAR_NECK_1, TRUE},
    {"around the neck (2)", WEAR_NECK_2, TRUE},
    {"on the body", WEAR_BODY, TRUE},
    {"over the head", WEAR_HEAD, TRUE},
    {"on the legs", WEAR_LEGS, TRUE},
    {"on the feet", WEAR_FEET, TRUE},
    {"on the hands", WEAR_HANDS, TRUE},
    {"on the arms", WEAR_ARMS, TRUE},
    {"as a shield", WEAR_SHIELD, TRUE},
    {"about the shoulders", WEAR_ABOUT, TRUE},
    {"around the waist", WEAR_WAIST, TRUE},
    {"on the left wrist", WEAR_WRIST_L, TRUE},
    {"on the right wrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"held in the hands", WEAR_HOLD, TRUE},
    {"", 0}
};

const struct flag_type wear_loc_flags[] =
{
    {"none", WEAR_NONE, TRUE},
    {"light", WEAR_LIGHT, TRUE},
    {"lfinger", WEAR_FINGER_L, TRUE},
    {"rfinger", WEAR_FINGER_R, TRUE},
    {"neck1", WEAR_NECK_1, TRUE},
    {"neck2", WEAR_NECK_2, TRUE},
    {"body", WEAR_BODY, TRUE},
    {"head", WEAR_HEAD, TRUE},
    {"legs", WEAR_LEGS, TRUE},
    {"feet", WEAR_FEET, TRUE},
    {"hands", WEAR_HANDS, TRUE},
    {"arms", WEAR_ARMS, TRUE},
    {"shield", WEAR_SHIELD, TRUE},
    {"about", WEAR_ABOUT, TRUE},
    {"waist", WEAR_WAIST, TRUE},
    {"lwrist", WEAR_WRIST_L, TRUE},
    {"rwrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"hold", WEAR_HOLD, TRUE},
    {"", 0, 0}
};

const struct flag_type weapon_flags[] =
{
    {"hit", 0, TRUE},
    {"slice", 1, TRUE},
    {"stab", 2, TRUE},
    {"slash", 3, TRUE},
    {"whip", 4, TRUE},
    {"claw", 5, TRUE},
    {"blast", 6, TRUE},
    {"pound", 7, TRUE},
    {"crush", 8, TRUE},
    {"grep", 9, TRUE},
    {"bite", 10, TRUE},
    {"pierce", 11, TRUE},
    {"suction", 12, TRUE},
    {"beating", 13, TRUE},	/* ROM */
    {"digestion", 14, TRUE},
    {"charge", 15, TRUE},
    {"slap", 16, TRUE},
    {"punch", 17, TRUE},
    {"wrath", 18, TRUE},
    {"magic", 19, TRUE},
    {"divine-power", 20, TRUE},
    {"cleave", 21, TRUE},
    {"scratch", 22, TRUE},
    {"peck-pierce", 23, TRUE},
    {"peck-bash", 24, TRUE},
    {"chop", 25, TRUE},
    {"sting", 26, TRUE},
    {"smash", 27, TRUE},
    {"shocking-bite", 28, TRUE},
    {"flaming-bite", 29, TRUE},
    {"freezing-bite", 30, TRUE},
    {"acidic-bite", 31, TRUE},
    {"chomp", 32, TRUE},
    {"weapon's bite", 33, TRUE},
    {"weapon's flame", 34, TRUE},
    {"weapon's frost", 35, TRUE},
    {"", 0, TRUE}
};

const struct flag_type container_flags[] =
{
    {"closeable", 1, TRUE},
    {"pickproof", 2, TRUE},
    {"closed", 4, TRUE},
    {"locked", 8, TRUE},
    {"", 0, 0}
};

const struct flag_type liquid_flags[] =
{
    {"water", 0, TRUE},
    {"beer", 1, TRUE},
    {"wine", 2, TRUE},
    {"ale", 3, TRUE},
    {"dark-ale", 4, TRUE},
    {"whisky", 5, TRUE},
    {"lemonade", 6, TRUE},
    {"firebreather", 7, TRUE},
    {"local-specialty", 8, TRUE},
    {"slime-mold-juice", 9, TRUE},
    {"milk", 10, TRUE},
    {"tea", 11, TRUE},
    {"coffee", 12, TRUE},
    {"blood", 13, TRUE},
    {"salt-water", 14, TRUE},
    {"cola", 15, TRUE},
    {"", 0, 0}
};

/*****************************************************************************
		      ROM - specific tables:
 ****************************************************************************/

const struct flag_type form_flags[] =
{
    {"edible", FORM_EDIBLE, TRUE},
    {"poison", FORM_POISON, TRUE},
    {"magical", FORM_MAGICAL, TRUE},
    {"decay", FORM_INSTANT_DECAY, TRUE},
    {"other", FORM_OTHER, TRUE},

    {"animal", FORM_ANIMAL, TRUE},
    {"sentient", FORM_SENTIENT, TRUE},
    {"undead", FORM_UNDEAD, TRUE},
    {"construct", FORM_CONSTRUCT, TRUE},
    {"mist", FORM_MIST, TRUE},
    {"intangible", FORM_INTANGIBLE, TRUE},

    {"biped", FORM_BIPED, TRUE},
    {"centaur", FORM_CENTAUR, TRUE},
    {"insect", FORM_INSECT, TRUE},
    {"spider", FORM_SPIDER, TRUE},
    {"crustacean", FORM_CRUSTACEAN, TRUE},
    {"worm", FORM_WORM, TRUE},
    {"blob", FORM_BLOB, TRUE},

    {"mammal", FORM_MAMMAL, TRUE},
    {"bird", FORM_BIRD, TRUE},
    {"reptile", FORM_REPTILE, TRUE},
    {"snake", FORM_SNAKE, TRUE},
    {"dragon", FORM_DRAGON, TRUE},
    {"amphibian", FORM_AMPHIBIAN, TRUE},
    {"fish", FORM_FISH, TRUE},
    {"cold-blood", FORM_COLD_BLOOD, TRUE},
    {"", 0, 0}
};

const struct flag_type part_flags[] =
{
    {"head", PART_HEAD, TRUE},
    {"arms", PART_ARMS, TRUE},
    {"legs", PART_LEGS, TRUE},
    {"heart", PART_HEART, TRUE},
    {"brains", PART_BRAINS, TRUE},
    {"guts", PART_GUTS, TRUE},
    {"hands", PART_HANDS, TRUE},
    {"feet", PART_FEET, TRUE},
    {"fingers", PART_FINGERS, TRUE},
    {"ear", PART_EAR, TRUE},
    {"eye", PART_EYE, TRUE},
    {"long-tongue", PART_LONG_TONGUE, TRUE},
    {"eyestalks", PART_EYESTALKS, TRUE},
    {"tentacles", PART_TENTACLES, TRUE},
    {"fins", PART_FINS, TRUE},
    {"wings", PART_WINGS, TRUE},
    {"tail", PART_TAIL, TRUE},

    {"claws", PART_CLAWS, TRUE},
    {"fangs", PART_FANGS, TRUE},
    {"horns", PART_HORNS, TRUE},
    {"scales", PART_SCALES, TRUE},
    {"tusks", PART_TUSKS, TRUE},
    {"", 0, 0}
};

const struct flag_type ac_type[] =
{
    {"pierce", AC_PIERCE, TRUE},
    {"bash", AC_BASH, TRUE},
    {"slash", AC_SLASH, TRUE},
    {"exotic", AC_EXOTIC, TRUE},
    {"", 0, 0}
};

const struct flag_type size_flags[] =
{
    {"tiny", SIZE_TINY, TRUE},
    {"small", SIZE_SMALL, TRUE},
    {"medium", SIZE_MEDIUM, TRUE},
    {"large", SIZE_LARGE, TRUE},
    {"huge", SIZE_HUGE, TRUE},
    {"giant", SIZE_GIANT, TRUE},
    {"", 0, 0},
};

const struct flag_type weapon_class[] =
{
    {"exotic", 0, TRUE},
    {"sword", 1, TRUE},
    {"dagger", 2, TRUE},
    {"spear", 3, TRUE},
    {"mace", 4, TRUE},
    {"axe", 5, TRUE},
    {"flail", 6, TRUE},
    {"whip", 7, TRUE},
    {"polearm", 8, TRUE},
    {"", 0, 0}
};

const struct flag_type weapon_type[] =
{
    {"flaming", WEAPON_FLAMING, TRUE},
    {"frost", WEAPON_FROST, TRUE},
    {"vampiric", WEAPON_VAMPIRIC, TRUE},
    {"sharp", WEAPON_SHARP, TRUE},
    {"vorpal", WEAPON_VORPAL, TRUE},
    {"two-hands", WEAPON_TWO_HANDS, TRUE},
    {"", 0, 0}
};

const struct flag_type furniture_flags[] =
{
    {"stand_at", STAND_AT, TRUE},
    {"stand_on", STAND_ON, TRUE},
    {"stand_in", STAND_IN, TRUE},
    {"sit_at", SIT_AT, TRUE},
    {"sit_on", SIT_ON, TRUE},
    {"sit_in", SIT_IN, TRUE},
    {"rest_at", REST_AT, TRUE},
    {"rest_on", REST_ON, TRUE},
    {"rest_in", REST_IN, TRUE},
    {"sleep_at", SLEEP_AT, TRUE},
    {"sleep_on", SLEEP_ON, TRUE},
    {"sleep_in", SLEEP_IN, TRUE},
    {"put_at", PUT_AT, TRUE},
    {"put_on", PUT_ON, TRUE},
    {"put_in", PUT_IN, TRUE},
    {"put_inside", PUT_INSIDE, TRUE},
    {"", 0, 0}
};

const struct flag_type imm_flags[] =
{
    {"summon", IMM_SUMMON, TRUE},
    {"charm", IMM_CHARM, TRUE},
    {"magic", IMM_MAGIC, TRUE},
    {"weapon", IMM_WEAPON, TRUE},
    {"bash", IMM_BASH, TRUE},
    {"pierce", IMM_PIERCE, TRUE},
    {"slash", IMM_SLASH, TRUE},
    {"fire", IMM_FIRE, TRUE},
    {"cold", IMM_COLD, TRUE},
    {"lightning", IMM_LIGHTNING, TRUE},
    {"acid", IMM_ACID, TRUE},
    {"poison", IMM_POISON, TRUE},
    {"negative", IMM_NEGATIVE, TRUE},
    {"holy", IMM_HOLY, TRUE},
    {"energy", IMM_ENERGY, TRUE},
    {"mental", IMM_MENTAL, TRUE},
    {"disease", IMM_DISEASE, TRUE},
    {"drowning", IMM_DROWNING, TRUE},
    {"light", IMM_LIGHT, TRUE},
    {"", 0, 0}
};

const struct flag_type res_flags[] =
{
    {"charm", RES_CHARM, TRUE},
    {"magic", RES_MAGIC, TRUE},
    {"weapon", RES_WEAPON, TRUE},
    {"bash", RES_BASH, TRUE},
    {"pierce", RES_PIERCE, TRUE},
    {"slash", RES_SLASH, TRUE},
    {"fire", RES_FIRE, TRUE},
    {"cold", RES_COLD, TRUE},
    {"lightning", RES_LIGHTNING, TRUE},
    {"acid", RES_ACID, TRUE},
    {"poison", RES_POISON, TRUE},
    {"negative", RES_NEGATIVE, TRUE},
    {"holy", RES_HOLY, TRUE},
    {"energy", RES_ENERGY, TRUE},
    {"mental", RES_MENTAL, TRUE},
    {"disease", RES_DISEASE, TRUE},
    {"drowning", RES_DROWNING, TRUE},
    {"light", RES_LIGHT, TRUE},
    {"", 0, 0}
};

const struct flag_type vuln_flags[] =
{
    {"magic", VULN_MAGIC, TRUE},
    {"weapon", VULN_WEAPON, TRUE},
    {"bash", VULN_BASH, TRUE},
    {"pierce", VULN_PIERCE, TRUE},
    {"slash", VULN_SLASH, TRUE},
    {"fire", VULN_FIRE, TRUE},
    {"cold", VULN_COLD, TRUE},
    {"lightning", VULN_LIGHTNING, TRUE},
    {"acid", VULN_ACID, TRUE},
    {"poison", VULN_POISON, TRUE},
    {"negative", VULN_NEGATIVE, TRUE},
    {"holy", VULN_HOLY, TRUE},
    {"energy", VULN_ENERGY, TRUE},
    {"mental", VULN_MENTAL, TRUE},
    {"disease", VULN_DISEASE, TRUE},
    {"drowning", VULN_DROWNING, TRUE},
    {"light", VULN_LIGHT, TRUE},
    {"wood", VULN_WOOD, TRUE},
    {"silver", VULN_SILVER, TRUE},
    {"iron", VULN_IRON, TRUE},
    {"", 0, 0}
};

const struct flag_type material_type[] =	/* not yet implemented, table made by Kyle Boyd */
{
    {"none", 0, TRUE
    },
    {"wood", 1, TRUE},
    {"silver", 2, TRUE},
    {"gold", 3, TRUE},
    {"platinum", 4, TRUE},
    {"iron", 5, TRUE},
    {"steel", 6, TRUE},
    {"mithril", 7, TRUE},
    {"adamantium", 8, TRUE},
    {"leather", 9, TRUE},
    {"cloth", 10, TRUE},
    {"paper", 11, TRUE},
    {"copper", 12, TRUE},
    {"bronze", 13, TRUE},
    {"glass", 14, TRUE},
    {"diamond", 15, TRUE},
    {"emerald", 16, TRUE},
    {"sapphire", 17, TRUE},
    {"ruby", 18, TRUE},
    {"misc-metal", 19, TRUE},
    {"misc-gem", 20, TRUE},
    {"unknown", 21, TRUE},
    {"slime", 22, TRUE},
    {"liquid", 23, TRUE},
    {"concrete", 24, TRUE},
    {"ceramic", 25, TRUE},
    {"stone", 26, TRUE},
    {"rust", 27, TRUE},
    {"magic", 28, TRUE},
    {"", 0, 0}
};

const struct flag_type position_flags[] =
{
    {"dead", POS_DEAD, FALSE},
    {"mortal", POS_MORTAL, FALSE},
    {"incap", POS_INCAP, FALSE},
    {"stunned", POS_STUNNED, FALSE},
    {"sleeping", POS_SLEEPING, TRUE},
    {"resting", POS_RESTING, TRUE},
    {"sitting", POS_SITTING, TRUE},
    {"fighting", POS_FIGHTING, FALSE},
    {"standing", POS_STANDING, TRUE},
    {"", 0, 0}
};

const struct flag_type rnd_obj_flags[] =
{
    {"plain-armor", ITEM_PLAIN_ARMOR, TRUE},
    {"magic-armor", ITEM_MAGIC_ARMOR, TRUE},
    {"ring", ITEM_RAND_RING, TRUE},
    /*{ "magic-item",    ITEM_MAGIC_ITEM,        TRUE }, */
    {"plain-weapon", ITEM_PLAIN_WEAPON, TRUE},
    {"magic-weapon", ITEM_MAGIC_WEAPON, TRUE},
    {"container", ITEM_RAND_CONTAINER, TRUE},
    /*{ "light",         ITEM_RAND_LIGHT,        TRUE }, */
    {"any", ITEM_RAND_ANY, TRUE},
    {"", 0, 0}
};

const struct flag_type mprog_type_flags[] =
{
    {"in_file_prog", IN_FILE_PROG, TRUE},
    {"act_prog", ACT_PROG, TRUE},
    {"speech_prog", SPEECH_PROG, TRUE},
    {"rand_prog", RAND_PROG, TRUE},
    {"fight_prog", FIGHT_PROG, TRUE},
    {"fightgroup_prog", FIGHTGROUP_PROG, TRUE},
    {"hitprcnt_prog", HITPRCNT_PROG, TRUE},
    {"death_prog", DEATH_PROG, TRUE},
    {"entry_prog", ENTRY_PROG, TRUE},
    {"greet_prog", GREET_PROG, TRUE},
    {"all_greet_prog", ALL_GREET_PROG, TRUE},
    {"give_prog", GIVE_PROG, TRUE},
    {"bribe_prog", BRIBE_PROG, TRUE},
    /*  to be added for room progs */
    {"enter_prog", ENTER_PROG, TRUE},
    {"leave_prog", LEAVE_PROG, TRUE},
    {"sleep_prog", SLEEP_PROG, TRUE},
    {"rest_prog", REST_PROG, TRUE},
    {"rfight_prog", RFIGHT_PROG, TRUE},
    {"rdeath_prog", RDEATH_PROG, TRUE},
    {"wear_prog", WEAR_PROG, TRUE},
    {"remove_prog", REMOVE_PROG, TRUE},
    {"sac_prog", SAC_PROG, TRUE},
    {"examine_prog", EXA_PROG, TRUE},
    {"look_prog", LOOK_PROG, TRUE},
    {"zap_prog", ZAP_PROG, TRUE},
    {"get_prog", GET_PROG, TRUE},
    {"drop_prog", DROP_PROG, TRUE},
    {"use_prog", USE_PROG, TRUE},
    {"hit_prog", HIT_PROG, TRUE},
    {"command_prog", COMMAND_PROG, TRUE},
    {"", 0, 0}
};

const struct flag_type clan_join_flags[] =
{
    {"anti-mage", CLAN_ANTI_MAGE, TRUE},
    {"anti-cleric", CLAN_ANTI_CLERIC, TRUE},
    {"anti-thief", CLAN_ANTI_THIEF, TRUE},
    {"anti-warrior", CLAN_ANTI_WARRIOR, TRUE},
    {"anti-good", CLAN_ANTI_GOOD, TRUE},
    {"anti-evil", CLAN_ANTI_EVIL, TRUE},
    {"required-pk", CLAN_REQ_PK, TRUE},
    {"no-pk", CLAN_NO_PK, TRUE},
    {"", 0, 0,},
};

const struct flag_type clan_flags[] =
{
    {"creating", CLAN_CREATING, FALSE},
    {"active", CLAN_ACTIVE, FALSE},
    {"inactive", CLAN_INACTIVE, FALSE},
    {"disbanding", CLAN_DISBANDING, FALSE},
    {"dead", CLAN_DEAD, FALSE},
    {"auto-accept", CLAN_AUTO_ACCEPT, TRUE},
    {"private", CLAN_PRIVATE, TRUE},
    {"secret", CLAN_SECRET, TRUE},
    {"", 0, 0,}
};
