#include "level.h"
#include "monster.h"

/*
 * Level 2: Danger Zone
 *
 * Features:
 * - Hazard-based goal level
 * - Goal: Defeat 5 hazards
 * - Background: Procedural terrain with variant 1 (different landscape)
 * - Player start: (100, 400)
 * - Dragon enemy patrolling in the middle of the level
 *
 * TODO: Add hazard/enemy definitions here
 * TODO: Add obstacle definitions here
 */

Level level2_create(void)
{
    // Define the background for this level (variant 1 creates different terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 1};

    LevelGoal goal = {
        .type = GOAL_TYPE_MONSTERS,
        .goal_position = {0, 0},
        .goal_radius = 0,
        .monsters_to_defeat = 1,
        .monsters_defeated = 0};

    Level level = level_create(
        2,                         // level number
        "Danger Zone",             // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    // Add bat monster patrolling in the middle area
    Monster bat = monster_create(
        600.0f,           // x position
        535.0f,           // y position
        80.0f,            // width
        80.0f,            // height
        2,                // max hearts
        400.0f,           // patrol left boundary
        800.0f,           // patrol right boundary
        150.0f,           // patrol speed
        "bat.png", // texture path
        0.08f             // scale
    );
    monster_list_add(&level.monsters, bat);
    PickupSpawner test_pickup = pickup_spawner_create(
        PICKUP_FIREBALL,           // Pickup type
        (Vector2){120.0f, 630.0f}, // Spawn location
        1,                         // Value (1 projectile)
        3.0f                       // Spawn interval
    );
    pickup_spawner_list_add(&level.spawners, test_pickup);

    return level;
}
