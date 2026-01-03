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

Level level15_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 14};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {4000.0f, 538.0f},
        .goal_radius = 50.0f,
    };

    Level level = level_create(
        15,                        // level number
        "Hazards of Havoc",        // level name
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

    Hazard dust_tornado = {
        .type = HAZARD_DUST_STORM,
        .bounds = {1000.0f, 300.0f, 160.0f, 300.0f}, // x, y, width, height (below ground)
        .damage = 1,                                 // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 800.0f,
        .patrol_right_bound = 1200.0f,
        .patrol_speed = 80.0f,
        .can_fade = false}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[1], 0.0f, 2.0f, 1.0f, 15.0f);

    Hazard jet7 = {
        .type = HAZARD_LAVA_JET,
        .bounds = {1900.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 1300.0f,
        .patrol_right_bound = 1300.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jet7);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[2], 1.0f, 0.0f, 3.0f, 0.0f);

    Hazard dust_tornado2 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {2000.0f, 500.0f, 150.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                 // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 1700.0f,
        .patrol_right_bound = 2000.0f,
        .patrol_speed = 50.0f,
        .can_fade = true}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado2);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[3], 0.0f, 2.0f, 1.0f, 5.0f);

    Hazard lava_pit4 = {
        .type = HAZARD_LAVA_PIT,
        .bounds = {3000.0f, 550.0f, 400.0f, 50.0f}, // x, y, width, height (below ground)
        .damage = 2,                                // Lose 2 hearts
        .active = true,
        .can_move = false,
        .velocity = {0.0f, 0.0f}, // Initial velocity (stationary)
        .patrol_left_bound = 3000.0f,
        .patrol_right_bound = 3000.0f,
        .patrol_speed = 0.0f,
        .can_fade = false};

    hazard_list_add(&level.hazards, lava_pit4);

    return level;
}
