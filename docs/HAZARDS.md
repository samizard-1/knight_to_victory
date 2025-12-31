# Hazard System Guide

The game now includes a hazard system that allows you to place obstacles and dangers in levels. Hazards can cause damage to the player and trigger respawning mechanics.

## Hazard Types

### HAZARD_LAVA_PIT
A pit of lava that the player must avoid or jump over. Features:
- Orange/red colored rectangle with animated flame effects
- Damage dealt on contact (configurable)
- Player respawns at level start when hit

### HAZARD_SPIKE_TRAP
A spike trap (expandable for future use). Features:
- Red colored rectangle
- Damage dealt on contact (configurable)
- Player respawns at level start when hit

## Hazard Structure

```c
typedef struct
{
    HazardType type;        // Type of hazard (HAZARD_LAVA_PIT, HAZARD_SPIKE_TRAP)
    Rectangle bounds;       // Position and size (x, y, width, height)
    int damage;             // Damage in hearts (usually 1)
    bool active;            // Whether the hazard is active
} Hazard;
```

## Adding Hazards to a Level

To add a hazard to a level, use `hazard_list_add()` in the level's create function:

### Example: Lava Pit in Level 1

```c
Level level1_create(void)
{
    // ... level setup code ...

    Level level = level_create(
        1,
        "Intro Level",
        background,
        (Vector2){100.0f, 400.0f},
        goal
    );

    // Add a lava pit hazard
    Hazard lava_pit = {
        .type = HAZARD_LAVA_PIT,
        .bounds = {400.0f, 550.0f, 100.0f, 50.0f},  // x=400, y=550, width=100, height=50
        .damage = 1,                                  // Player loses 1 heart
        .active = true
    };
    hazard_list_add(&level.hazards, lava_pit);

    return level;
}
```

## Hazard Collision and Respawn

When a player collides with an active hazard:

1. Player takes damage (loses hearts)
2. Player position resets to the level's starting position
3. Player velocity is reset to zero
4. If player has no hearts remaining:
   - Player is fully healed (max_hearts)
   - Level continues from the starting position

## Adding New Hazard Types

To add a new hazard type:

1. Add the type to the `HazardType` enum in [include/hazard.h](include/hazard.h):
   ```c
   typedef enum
   {
       HAZARD_LAVA_PIT,
       HAZARD_SPIKE_TRAP,
       HAZARD_MOVING_PLATFORM,  // New type
   } HazardType;
   ```

2. Add rendering code in `hazard_draw()` in [src/hazard.c](src/hazard.c):
   ```c
   case HAZARD_MOVING_PLATFORM:
   {
       // Drawing code for moving platform
       break;
   }
   ```

3. Add the hazard to levels using the new type.

## Positioning Hazards Correctly

Important notes for hazard positioning:

- **Y-coordinate**: For hazards on the ground level, use `y ≈ 550-560` (above the ground line at 600)
- **Width and Height**: Should be sized appropriately for collision detection
- **X-coordinates**: Measure from the level's left edge (x=0)

Example hazard at ground level spanning from x=400 to x=500:
```c
Hazard ground_hazard = {
    .type = HAZARD_LAVA_PIT,
    .bounds = {400.0f, 550.0f, 100.0f, 50.0f},  // 100px wide, 50px tall
    .damage = 1,
    .active = true
};
```

## Level 1 Hazards

Level 1 currently features:
- **Lava Pit**: Located at x=400-500, requires player to jump over it to progress
- **Goal**: At x=800, must reach after passing the lava pit

## Hazard Properties Reference

| Property | Type | Purpose |
|----------|------|---------|
| `type` | HazardType | Determines rendering and behavior |
| `bounds` | Rectangle | Position (x, y) and size (width, height) |
| `damage` | int | Hearts lost on collision |
| `active` | bool | Whether hazard is enabled |

