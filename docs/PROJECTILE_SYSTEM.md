# Projectile System - Monster Firing Update

## Overview

The projectile system has been updated to support both players and monsters (including the dragon) firing projectiles. The system now includes collision detection for monster projectiles hitting the player.

## Key Changes

### 1. Projectile Source Tracking

Added a `ProjectileSource` enum to track who fired each projectile:

```c
typedef enum
{
    PROJECTILE_SOURCE_PLAYER,
    PROJECTILE_SOURCE_MONSTER
} ProjectileSource;
```

**Usage:**
- When the player fires: `projectile_create_fireball(..., PROJECTILE_SOURCE_PLAYER)`
- When a monster fires: `projectile_create_fireball(..., PROJECTILE_SOURCE_MONSTER)`

### 2. Updated Projectile Structure

The `Projectile` struct now includes:
```c
ProjectileSource source;    // Who fired this projectile
```

This allows the game to distinguish between player and monster projectiles.

### 3. New Collision Detection Function

Added `projectile_check_player_collision()` for detecting monster projectile hits:

```c
bool projectile_check_player_collision(Projectile *projectile, Rectangle player_rect);
```

This function:
- Only returns true if the projectile is from a MONSTER source
- Checks collision with the player hitbox
- Prevents player projectiles from hitting the player

### 4. Updated Game Loop

The game loop now:
1. **Checks player projectile-monster collisions** (unchanged behavior)
2. **Checks monster projectile-player collisions** (new feature)
3. **Updates dragon firing** (new feature)

Monster projectiles deal 1 damage to the player when they hit.

## Dragon Integration

### Dragon Firing System

The dragon uses the new projectile system via:

```c
void dragon_fire_at_target(Monster *dragon, ProjectileList *projectiles, Vector2 target_pos);
```

**Features:**
- **Firing Range**: Dragons only fire if the player is within 800 units (configurable via `DragonData.firing_range`)
- **Cooldown**: Dragons have a 2-second cooldown between shots (configurable via `DragonData.fire_cooldown_max`)
- **Automatic**: Dragons automatically update their cooldown and fire when conditions are met

### Dragon Data Structure

```c
typedef struct
{
    float fire_cooldown;      // Time remaining before next shot
    float fire_cooldown_max;  // Cooldown duration (2.0f seconds)
    float firing_range;       // Maximum firing distance (800.0f units)
} DragonData;
```

### Integration in Game Loop

The game loop automatically calls dragon firing:

```c
for (int i = 0; i < current_level->monsters.count; i++)
{
    Monster *monster = &current_level->monsters.monsters[i];
    // Check if this is a dragon with custom behavior
    if (monster->active && monster->custom_data != NULL && 
        monster->custom_update == dragon_custom_update)
    {
        dragon_fire_at_target(monster, &state->projectiles, player.position);
    }
}
```

## Files Modified

1. **include/projectile.h**
   - Added `ProjectileSource` enum
   - Updated `Projectile` struct with `source` field
   - Added `projectile_check_player_collision()` function declaration

2. **src/projectile.c**
   - Updated `projectile_create_fireball()` to accept `source` parameter
   - Added `projectile_check_player_collision()` implementation

3. **src/game.c**
   - Added dragon include
   - Updated player projectile creation to use `PROJECTILE_SOURCE_PLAYER`
   - Updated projectile-monster collision to only check player projectiles
   - Added monster projectile-player collision detection
   - Added dragon firing AI in the game loop

4. **include/dragon.h**
   - Added `DragonData` struct definition
   - Added `dragon_fire_at_target()` function declaration

5. **src/dragon.c**
   - Implemented `dragon_fire_at_target()` for firing projectiles
   - Updated `dragon_apply_customizations()` to initialize dragon data
   - Updated `dragon_custom_update()` to handle patrol + custom behavior pattern

## Usage Example: Adding Monster Projectiles to Another Monster

To add projectile firing to another monster type (e.g., a boss):

1. Create a new monster customization file (e.g., `src/boss.c`)
2. Create a firing function:
```c
void boss_fire_at_target(Monster *boss, ProjectileList *projectiles, Vector2 target_pos)
{
    // Your custom firing logic
    Projectile projectile = projectile_create_fireball(
        (Vector2){boss->position.x, boss->position.y},
        target_pos,
        PROJECTILE_SOURCE_MONSTER);
    projectile_list_add(projectiles, projectile);
}
```

3. Call it from the game loop similar to dragon firing

## Configuration

Dragon firing parameters can be adjusted in `dragon.c`:

```c
data->fire_cooldown_max = 2.0f;    // Change to adjust fire rate
data->firing_range = 800.0f;       // Change to adjust range
```

Or make these configurable through a parameter to `dragon_apply_customizations()` for future flexibility.

## Testing Recommendations

1. Test dragon projectiles reach the player and deal damage
2. Test player projectiles still hit dragons correctly
3. Test collision detection accuracy
4. Test multiple dragons firing simultaneously
5. Verify projectile cleanup doesn't leak memory
