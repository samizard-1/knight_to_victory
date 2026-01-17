#include "level.h"
#include "dragon.h"

/*
 * Level 6: Final Challenge
 *
 * Features:
 * - The ultimate boss level
 * - Goal: Defeat all monsters to complete the game
 * - Background: Procedural terrain with variant 5
 * - Player start: (100, 400)
 *
 * TODO: Add platform definitions here
 * TODO: Add obstacle definitions here
 * TODO: Add hazard definitions here
 */

Level level17_create(void)
{
    // Define the background for this level
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 16};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {4000.0f, 538.0f},
        .goal_radius = 50.0f,
    };

    Level level = level_create(
        17,                        // level number
        "The Mixed Trials-part one",               // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

        Hazard trial_dust = {
        .type = HAZARD_DUST_STORM,
        .bounds = {1400.0f, 300.0f, 150.0f, 300.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 600.0f,
        .patrol_right_bound = 800.0f,
        .patrol_speed = 50.0f,
        .can_fade = true}; // Set to false initially
    hazard_list_add(&level.hazards, trial_dust);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[0], 1.0f, 1.0f, 2.0f, 1.0f); 

    Monster trial_slug = monster_create(
        600.0f,                    // x position
        550.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        3,                         // max hearts
        820.0f,                    // patrol left boundary
        1000.0f,                    // patrol right boundary
        80.0f,                     // patrol speed
        "monster_slug.png", // texture path
        0.1f,// scale
    "slug"
    );
    monster_list_add(&level.monsters, trial_slug);
    // Add bat monster patrolling in the middle area
    Monster trial_bat = monster_create(
        400.0f,           // x position
        535.0f,           // y position
        80.0f,            // width
        80.0f,            // height
        2,                // max hearts
        650.0f,           // patrol left boundary
        850.0f,           // patrol right boundary
        150.0f,           // patrol speed
        "bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, trial_bat);

    Hazard trial_daggers = {
        .type = HAZARD_WIND_DAGGERS,
        .bounds = {17000.0f, 500.0f, 700.0f, 50.0f}, // x, y, width, height
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {0.0f, 0.0f}, // Initial velocity (stationary)
        .patrol_left_bound = -100000000000000000000000000.0f,
        .patrol_right_bound = 14000.0f,
        .patrol_speed = 500.0f,
        .can_fade = false};

    hazard_list_add(&level.hazards, trial_daggers);
    
    return level;
}
