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

Level level20_create(void)
{
    // Define the background for this level
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 19};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {6000.0f, 538.0f},
        .goal_radius = 50.0f,
    };

    Level level = level_create(
        20,                        // level number
        "The Long Desert Trekk",               // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position (3/000)
        goal                       // level goal
    );

    Hazard trial_dust = {
        .type = HAZARD_DUST_STORM,
        .bounds = {1400.0f, 300.0f, 150.0f, 300.0f}, // x, y, width, height (below ground)
        .damage = 1,                                 // Lose 1 heart
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

    Monster trial_slug2 = monster_create(
        600.0f,             // x position
        520.0f,             // y position
        70.0f,              // width
        70.0f,              // height
        5,                  // max hearts
        1030.0f,            // patrol left boundary
        1130.0f,            // patrol right boundary
        80.0f,              // patrol speed
        "monster_slug.png", // texture path
        0.16f               // scale
    );
    monster_list_add(&level.monsters, trial_slug2);
    // Add bat monster patrolling in the middle area

    Monster trial_crab = monster_create(
        800.0f,     // x position
        557.0f,     // y position
        70.0f,      // width
        70.0f,      // height
        3,          // max hearts
        1140.0f,    // patrol left boundary
        1510.0f,    // patrol right boundary
        170.0f,     // patrol speed
        "crab.png", // texture path
        0.12f       // scale
    );
    monster_list_add(&level.monsters, trial_crab);

    Monster trial_crab2 = monster_create(
        800.0f,     // x position
        545.0f,     // y position
        70.0f,      // width
        70.0f,      // height
        4,          // max hearts
        1520.0f,    // patrol left boundary
        1980.0f,    // patrol right boundary
        165.0f,     // patrol speed
        "crab.png", // texture path
        0.15f       // scale
    );
    monster_list_add(&level.monsters, trial_crab2);

    Hazard trial_pit = {
        .type = HAZARD_LAVA_PIT,
        .bounds = {2100.0f, 650.0f, 150.0f, 80.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = false,
        .velocity = {0.0f, 0.0f}, // Initial velocity (stationary)
        .patrol_left_bound = 2100.0f,
        .patrol_right_bound = 2100.0f,
        .patrol_speed = 0.0f,
        .can_fade = false};
    hazard_list_add(&level.hazards, trial_pit);

    PickupSpawner trial_spawner = pickup_spawner_create(
        PICKUP_FIREBALL,            // Pickup type
        (Vector2){2155.0f, 630.0f}, // Spawn location
        1,                          // Value (1 projectile)
        3.0f                        // Spawn interval
    );
    pickup_spawner_list_add(&level.spawners, trial_spawner);

    Monster trial_bat2 = monster_create(
        1300.0f,             // x position
        535.0f,              // y position
        70.0f,               // width
        70.0f,               // height
        3,                   // max hearts
        2200.0f,             // patrol left boundary
        2500.0f,             // patrol right boundary
        100.0f,              // patrol speed
        "../assets/bat.png", // texture path
        0.1f                 // scale
    );
    monster_list_add(&level.monsters, trial_bat2);

    Monster trial_crab3 = monster_create(
        800.0f,     // x position
        545.0f,     // y position
        70.0f,      // width
        70.0f,      // height
        4,          // max hearts
        2550.0f,    // patrol left boundary
        3000.0f,    // patrol right boundary
        165.0f,     // patrol speed
        "crab.png", // texture path
        0.15f       // scale
    );
    monster_list_add(&level.monsters, trial_crab3);

    Hazard jetty = {
        .type = HAZARD_LAVA_JET,
        .bounds = {3150.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 3150.0f,
        .patrol_right_bound = 3150.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, jetty);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[2], 1.0f, 0.0f, 3.0f, 0.0f);

    Hazard tj = {
        .type = HAZARD_LAVA_JET,
        .bounds = {3350.0f, 500.0f, 70.0f, 100.0f}, // x, y, width, height (below ground)
        .damage = 1,                                // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 3350.0f,
        .patrol_right_bound = 3350.0f,
        .patrol_speed = 70.0f,
        .can_fade = true};

    hazard_list_add(&level.hazards, tj);

    // Initialize fade properties (0 sec opaque, 0.5 sec fade out, 0.5 sec gap, 0.5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[3], 1.0f, 0.0f, 3.0f, 0.0f);

    Monster trial_dragon = monster_create(
        3600.0f,           // x position
        515.0f,            // y position
        150.0f,            // width
        150.0f,            // height
        6,                 // max hearts
        3700.0f,           // patrol left boundary
        4500.0f,           // patrol right boundary
        120.0f,            // patrol speed
        "baby_dragon.png", // texture path
        0.2f               // scale
    );
    monster_list_add(&level.monsters, trial_dragon);

    Hazard trial_dust2 = {
        .type = HAZARD_DUST_STORM,
        .bounds = {1400.0f, 300.0f, 150.0f, 300.0f}, // x, y, width, height (below ground)
        .damage = 1,                                 // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {50.0f, 0.0f}, // Initial velocity (moving right)
        .patrol_left_bound = 4600.0f,
        .patrol_right_bound = 4850.0f,
        .patrol_speed = 50.0f,
        .can_fade = true}; // Set to false initially
    hazard_list_add(&level.hazards, trial_dust2);

    // Initialize fade properties (0 sec opaque, 2 sec fade out, 1 sec gap, 5 sec fade in)
    hazard_init_fade(&level.hazards.hazards[4], 1.0f, 1.0f, 2.0f, 1.0f); 

    Monster trial_slug3 = monster_create(
        600.0f,             // x position
        520.0f,             // y position
        70.0f,              // width
        70.0f,              // height
        5,                  // max hearts
        4900.0f,            // patrol left boundary
        5100.0f,            // patrol right boundary
        80.0f,              // patrol speed
        "monster_slug.png", // texture path
        0.16f               // scale
    );
    monster_list_add(&level.monsters, trial_slug3);
    // Add bat monster patrolling in the middle area

    Hazard trial_daggers4 = {
        .type = HAZARD_WIND_DAGGERS,
        .bounds = {20000.0f, 500.0f, 700.0f, 50.0f}, // x, y, width, height
        .damage = 1,                                 // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {0.0f, 0.0f}, // Initial velocity (stationary)
        .patrol_left_bound = -100000000000000000000000000.0f,
        .patrol_right_bound = 20000.0f,
        .patrol_speed = 500.0f,
        .can_fade = false};

    hazard_list_add(&level.hazards, trial_daggers4);

    Hazard blabla = {
        .type = HAZARD_WIND_DAGGERS,
        .bounds = {50000.0f, 500.0f, 700.0f, 50.0f}, // x, y, width, height
        .damage = 1,                                 // Lose 1 heart
        .active = true,
        .can_move = true,
        .velocity = {0.0f, 0.0f}, // Initial velocity (stationary)
        .patrol_left_bound = -100000000000000000000000000.0f,
        .patrol_right_bound = 50000.0f,
        .patrol_speed = 540.0f,
        .can_fade = false};

    hazard_list_add(&level.hazards, blabla);

    Monster crabby = monster_create(
        800.0f,     // x position
        557.0f,     // y position
        70.0f,      // width
        70.0f,      // height
        3,          // max hearts
        5200.0f,    // patrol left boundary
        5450.0f,    // patrol right boundary
        170.0f,     // patrol speed
        "crab.png", // texture path
        0.12f       // scale
    );
    monster_list_add(&level.monsters, crabby);

    return level;
}
