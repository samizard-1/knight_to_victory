#ifndef DAMAGE_H
#define DAMAGE_H

// Define all possible damage types
typedef enum
{
    DAMAGE_TYPE_NONE = 0,    // No active damage state
    DAMAGE_TYPE_MONSTER_HIT, // Damage from monster collision
    DAMAGE_TYPE_FIRE,        // Damage from fire/lava
    DAMAGE_TYPE_DUST,        // Damage from dust/particles
} DamageType;

// Display properties for damage textures
typedef struct
{
    float scale_multiplier; // Multiplier applied to base player scale (1.0 = no change)
    float y_offset;         // Vertical offset adjustment to keep feet on ground
} DamageTextureDisplay;

// Get the texture suffix for a given damage type
// Returns a string like "hurt" or "on_fire" that can be appended to filenames
// For DAMAGE_TYPE_NONE, returns NULL
const char *damage_type_get_texture_suffix(DamageType damage_type);

// Get a human-readable name for the damage type
const char *damage_type_get_name(DamageType damage_type);

// Get display properties for a damage type's texture
// Returns scale multiplier and y-offset to prevent character movement when damaged
DamageTextureDisplay damage_type_get_display_properties(DamageType damage_type);

// Get the on-screen message for a damage type
// Returns a string describing what happened to the player
const char *damage_type_get_message(DamageType damage_type);

#endif // DAMAGE_H
