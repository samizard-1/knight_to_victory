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

Level level13_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 12};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {3000.0f, 538.0f},
        .goal_radius = 50.0f,
    };

    Level level = level_create(
        13,                        // level number
        "Toastyfoot Desert",               // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );
    
    Hazard jet1 = {
        .type = HAZARD_LAVA_JET,
        .bounds = {500.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 500.0f,
        .patrol_right_bound = 500.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jet1);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[0], 1.0f, 0.0f, 3.0f, 0.0f);

        Hazard jet2 = {
        .type = HAZARD_LAVA_JET,
        .bounds = {700.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 700.0f,
        .patrol_right_bound = 700.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jet2);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[1], 1.0f, 0.0f, 3.0f, 0.0f);

            Hazard jet3 = {
        .type = HAZARD_LAVA_JET,
        .bounds = {900.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 900.0f,
        .patrol_right_bound = 900.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jet3);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[2], 1.0f, 0.0f, 3.0f, 0.0f);

            Hazard jet4 = {
        .type = HAZARD_LAVA_JET,
        .bounds = {1100.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 1100.0f,
        .patrol_right_bound = 1100.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jet4);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[3], 1.0f, 0.0f, 3.0f, 0.0f);

            Hazard jet5 = {
        .type = HAZARD_LAVA_JET,
        .bounds = {1300.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 1300.0f,
        .patrol_right_bound = 1300.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jet5);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[4], 1.0f, 0.0f, 3.0f, 0.0f);

            Hazard jet6 = {
        .type = HAZARD_LAVA_JET,
        .bounds = {1500.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 1500.0f,
        .patrol_right_bound = 1500.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jet6);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[5], 1.0f, 0.0f, 3.0f, 0.0f);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[7], 1.0f, 0.0f, 3.0f, 0.0f);

    return level;
}
