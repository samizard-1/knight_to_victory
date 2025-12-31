#include "level.h"

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

Level level7_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 6};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {1200.0f, 538.0f},
        .goal_radius = 50.0f,
        .hazards_to_defeat = 2,
        .hazards_defeated = 0};

    Level level = level_create(
        7,                         // level number
        "Trials of Fire",          // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    Hazard lava = {
        .type = HAZARD_LAVA_PIT,
        .bounds = {500.0f, 600.0f, 50.0f, 50.0f}, // x, y, width, height
        .damage = 1,                              // Lose 1 heart
        .active = true};
    hazard_list_add(&level.hazards, lava);

    Hazard lava2 = {
        .type = HAZARD_LAVA_PIT,
        .bounds = {700.0f, 600.0f, 50.0f, 50.0f}, // x, y, width, height
        .damage = 1,                              // Lose 1 heart
        .active = true};
    hazard_list_add(&level.hazards, lava2);

    return level;
}
