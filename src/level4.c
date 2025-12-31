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

Level level4_create(void)
{
    // Define the background for this level (variant 2 creates different terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 3};

    LevelGoal goal = {
        .type = GOAL_TYPE_MONSTERS,
        .goal_position = {1000.0f, 538.0f},
        .goal_radius = 50.0f,
        .monsters_to_defeat = 5,
        .monsters_defeated = 0};

    Level level = level_create(
        4,                         // level number
        "Monster Mayhem",          // level name
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
        "monster_slug.png", // texture path
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
        "bat.png", // texture path
        0.08f             // scale
    );
    Monster slug2 = monster_create(
        600.0f,                    // x position
        550.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        3,                         // max hearts
        600.0f,                    // patrol left boundary
        900.0f,                    // patrol right boundary
        80.0f,                     // patrol speed
        "monster_slug.png", // texture path
        0.1f                       // scale
    );
    Monster bat3 = monster_create(
        400.0f,           // x position
        475.0f,           // y position
        80.0f,            // width
        80.0f,            // height
        4,                // max hearts
        1000.0f,          // patrol left boundary
        1600.0f,          // patrol right boundary
        150.0f,           // patrol speed
        "bat.png", // texture path
        0.35f             // scale
    );
    Monster slug3 = monster_create(
        600.0f,                    // x position
        565.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        1,                         // max hearts
        1600.0f,                   // patrol left boundary
        2000.0f,                   // patrol right boundary
        100.0f,                    // patrol speed
        "monster_slug.png", // texture path
        0.07f                      // scale
    );
    monster_list_add(&level.monsters, bat2);
    monster_list_add(&level.monsters, bat3);
    monster_list_add(&level.monsters, slug2);
    monster_list_add(&level.monsters, slug3);
    return level;
}
