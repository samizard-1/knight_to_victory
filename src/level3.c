#include "level.h"

/*
 * Level 3: Challenge Course
 *
 * Features:
 * - Advanced location-based goal level
 * - Goal: Reach the door at the end of the course
 * - Background: Procedural terrain with variant 2 (yet another different landscape)
 * - Player start: (100, 400)
 *
 * TODO: Add platform definitions here
 * TODO: Add obstacle definitions here
 * TODO: Add hazard definitions here
 */

Level level3_create(void)
{
    // Define the background for this level (variant 2 creates different terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 2};

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {1000.0f, 538.0f},
        .goal_radius = 50.0f,
        .hazards_to_defeat = 0,
        .hazards_defeated = 0};

    Level level = level_create(
        3,                         // level number
        "Challenge Course",        // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    // Add bat monster patrolling in the middle area
    Monster slug = monster_create(
        600.0f,                    // x position
        550.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        3,                         // max hearts
        300.0f,                    // patrol left boundary
        700.0f,                    // patrol right boundary
        80.0f,                     // patrol speed
        "assets/monster_slug.png", // texture path
        0.1f                       // scale
    );
    monster_list_add(&level.monsters, slug);
    // Add bat monster patrolling in the middle area
    Monster bat2 = monster_create(
        400.0f,           // x position
        535.0f,           // y position
        80.0f,            // width
        80.0f,            // height
        2,                // max hearts
        100.0f,           // patrol left boundary
        600.0f,           // patrol right boundary
        150.0f,           // patrol speed
        "assets/bat.png", // texture path
        0.08f             // scale
    );
    monster_list_add(&level.monsters, bat2);
    return level;
}
