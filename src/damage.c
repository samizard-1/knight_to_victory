#include "damage.h"
#include <stddef.h>

const char *damage_type_get_texture_suffix(DamageType damage_type)
{
    switch (damage_type)
    {
    case DAMAGE_TYPE_MONSTER_HIT:
        return "hurt";
    case DAMAGE_TYPE_FIRE:
        return "on_fire";
    case DAMAGE_TYPE_DUST:
        return "dust";
    case DAMAGE_TYPE_NONE:
    default:
        return NULL;
    }
}

const char *damage_type_get_name(DamageType damage_type)
{
    switch (damage_type)
    {
    case DAMAGE_TYPE_MONSTER_HIT:
        return "Monster Hit";
    case DAMAGE_TYPE_FIRE:
        return "Fire";
    case DAMAGE_TYPE_DUST:
        return "Dust";
    case DAMAGE_TYPE_NONE:
    default:
        return "None";
    }
}

DamageTextureDisplay damage_type_get_display_properties(DamageType damage_type)
{
    switch (damage_type)
    {
    case DAMAGE_TYPE_MONSTER_HIT:
        return (DamageTextureDisplay){
            .scale_multiplier = 1.2f, // Slightly larger hurt texture
            .y_offset = -0.2f         // Small upward offset
        };
    case DAMAGE_TYPE_FIRE:
        return (DamageTextureDisplay){
            .scale_multiplier = 1.8f, // Significantly larger fire texture
            .y_offset = -0.3f         // Larger upward offset for fire
        };
    case DAMAGE_TYPE_DUST:
        return (DamageTextureDisplay){
            .scale_multiplier = 2.0f, // Slightly larger dust texture
            .y_offset = -0.01f        // Small upward offset
        };
    case DAMAGE_TYPE_NONE:
    default:
        return (DamageTextureDisplay){
            .scale_multiplier = 1.0f,
            .y_offset = 0.0f};
    }
}

const char *damage_type_get_message(DamageType damage_type)
{
    switch (damage_type)
    {
    case DAMAGE_TYPE_MONSTER_HIT:
        return "You've been Hit!";
    case DAMAGE_TYPE_FIRE:
        return "You're burnt!";
    case DAMAGE_TYPE_DUST:
        return "You've Been Blasted Into Pulp!";
    case DAMAGE_TYPE_NONE:
    default:
        return "You took damage!";
    }
}
