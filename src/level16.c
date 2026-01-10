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

Level level16_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 15};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {4000.0f, 538.0f},
        .goal_radius = 50.0f,
    };

    Level level = level_create(
        16,                        // level number
        "Desert of Dusty Destruction",               // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    Hazard dust_tornado3 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {1000.0f, 350.0f, 150.0f, 250.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 800.0f,
        .patrol_right_bound = 1000.0f,
        .patrol_speed = 80.0f,
        .can_fade = true}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado3);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[0], 1.0f, 2.0f, 1.0f, 5.0f);

        Hazard dust_tornado4 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {1400.0f, 300.0f, 150.0f, 300.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 1200.0f,
        .patrol_right_bound = 1400.0f,
        .patrol_speed = 50.0f,
        .can_fade = true}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado4);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[1], 1.0f, 2.0f, 1.0f, 5.0f); 

        Hazard dust_tornado5 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {1800.0f, 350.0f, 150.0f, 250.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 1600.0f,
        .patrol_right_bound = 1800.0f,
        .patrol_speed = 80.0f,
        .can_fade = false}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado5);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[2], 1.0f, 2.0f, 1.0f, 15.0f);

        Hazard dust_tornado6 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {2200.0f, 300.0f, 160.0f, 300.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 2000.0f,
        .patrol_right_bound = 2200.0f,
        .patrol_speed = 50.0f,
        .can_fade = true}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado4);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[3], 1.0f, 2.0f, 1.0f, 5.0f);
    
        Hazard dust_tornado7 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {2600.0f, 350.0f, 150.0f, 250.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 2400.0f,
        .patrol_right_bound = 2600.0f,
        .patrol_speed = 80.0f,
        .can_fade = false}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado7);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[4], 1.0f, 2.0f, 1.0f, 15.0f);

        Hazard dust_tornado8 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {3000.0f, 300.0f, 160.0f, 300.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 2800.0f,
        .patrol_right_bound = 3000.0f,
        .patrol_speed = 50.0f,
        .can_fade = true}; // Set to false initially
    hazard_list_add(&level.hazards, dust_tornado4);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[5], 1.0f, 2.0f, 1.0f, 5.0f); 
    
    return level;
}
