#ifndef DRAGON_H
#define DRAGON_H

#include "monster.h"
#include "projectile.h"

typedef struct
{
    float fire_cooldown;     // Time before dragon can fire again
    float fire_cooldown_max; // Max cooldown between fires
    float firing_range;      // Maximum range to fire projectiles
} DragonData;

// Dragon-specific customizations
void dragon_apply_customizations(Monster *dragon);

// Custom dragon heart drawing function
void dragon_draw_hearts(Monster *dragon, float screen_pos_x, float screen_pos_y);

// Custom dragon update for firing projectiles
void dragon_custom_update(Monster *dragon);

// Custom dragon cleanup
void dragon_cleanup(Monster *dragon);

// Dragon firing function - call this to make dragon fire at a target
void dragon_fire_at_target(Monster *dragon, ProjectileList *projectiles, Vector2 target_pos);

#endif // DRAGON_H
