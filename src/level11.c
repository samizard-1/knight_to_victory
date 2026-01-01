#include "level.h"
#include "dragon.h"

/*
 * Level 6: Final Challenge
 *
 * Features:
 * - The ultimate boss level
 * - Goal: Defeat all monsters to complete the game
 * - Background: Procedural terrain with variant 5 (final landscape)
 * - Player start: (100, 400)
 *
 * TODO: Add platform definitions here
 * TODO: Add obstacle definitions here
 * TODO: Add hazard definitions here
 */

Level level11_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 10};

    LevelGoal goal = {
        .type = GOAL_TYPE_HAZARDS,
        .goal_position = {1000.0f, 538.0f},
        .goal_radius = 50.0f,
        .hazards_to_defeat = 4,
        .hazards_defeated = 0};

    Level level = level_create(
        9,                         // level number
        "The Dragon's Guards",     // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    // Add dust storm hazard
    Hazard dust_storm = {
        .type = HAZARD_DUST_STORM,
        .bounds = {500.0f, 500.0f, 150.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true};
    hazard_list_add(&level.hazards, dust_storm);

    return level;
}
