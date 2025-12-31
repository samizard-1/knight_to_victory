#include "level.h"
#include "config.h"

/*
 * Level 1: Intro Level
 *
 * Features:
 * - Simple introduction level
 * - Goal: Reach the door on the right
 * - Background: Procedural terrain with variant 0
 * - Player start: (100, 400)
 * - Hazard: Lava pit at x=400-500
 */

Level level1_create(void)
{
    // Define the background for this level
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 0};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {800.0f, 538.0f},
        .goal_radius = 50.0f,
        .hazards_to_defeat = 0,
        .hazards_defeated = 0};

    Level level = level_create(
        1,                         // level number
        "Intro Level",             // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    // Add lava pit hazard (positioned at ground level)
    Hazard lava_pit = {
        .type = HAZARD_LAVA_PIT,
        .bounds = {400.0f, 650.0f, 100.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true};
    hazard_list_add(&level.hazards, lava_pit);

    // Add a fireball spawner from the lava pit (spawns every 1.5 seconds with value 1)
    PickupSpawner fireball_spawner = pickup_spawner_create(
        PICKUP_FIREBALL,           // Pickup type
        (Vector2){400.0f, 600.0f}, // Spawn location
        1,                         // Value (1 projectile)
        1.5f                       // Spawn interval
    );
    pickup_spawner_list_add(&level.spawners, fireball_spawner);

    return level;
}
