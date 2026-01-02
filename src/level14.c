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

Level level14_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 13};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {3000.0f, 538.0f},
        .goal_radius = 50.0f,
    };

    Level level = level_create(
        14,                        // level number
        "Desert of Daggers",               // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );
    
    Hazard daggers1 = {
        .type = HAZARD_WIND_DAGGERS,
        .bounds = {2000.0f, 500.0f, 700.0f, 50.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = -100000000000000000.0f,
        .patrol_right_bound = 2000.0f,
        .patrol_speed = 500.0f,
        .can_fade = false};
    hazard_list_add(&level.hazards, daggers1);

    Hazard daggers2 = {
        .type = HAZARD_WIND_DAGGERS,
        .bounds = {2500.0f, 500.0f, 700.0f, 50.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = -100000000000000000.0f,
        .patrol_right_bound = 3000.0f,
        .patrol_speed = 500.0f,
        .can_fade = false};
    hazard_list_add(&level.hazards, daggers2);

    return level;
}
