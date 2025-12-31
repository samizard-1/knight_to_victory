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

Level level8_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 7};

    LevelGoal goal = {
        .type = GOAL_TYPE_MONSTERS,
        .goal_position = {1000.0f, 538.0f},
        .goal_radius = 50.0f,
        .monsters_to_defeat = 9,
        .monsters_defeated = 0};

    Level level = level_create(
        8,                         // level number
        "Sluggato the Sluggy",     // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    // Add unique slug 1
    Monster slug1 = monster_create(
        800.0f,                    // x position
        560.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        2,                         // max hearts
        650.0f,                    // patrol left boundary
        1100.0f,                   // patrol right boundary
        120.0f,                    // patrol speed
        "assets/monster_slug.png", // texture path
        0.08f                      // scale
    );
    monster_list_add(&level.monsters, slug1);

    // Add unique slug 2
    Monster slug2 = monster_create(
        1300.0f,                   // x position
        520.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        5,                         // max hearts
        1100.0f,                   // patrol left boundary
        1900.0f,                   // patrol right boundary
        140.0f,                    // patrol speed
        "assets/monster_slug.png", // texture path
        0.16f                      // scale
    );
    monster_list_add(&level.monsters, slug2);

    // Add unique slug 3
    Monster slug3 = monster_create(
        800.0f,                    // x position
        560.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        2,                         // max hearts
        1900.0f,                   // patrol left boundary
        2300.0f,                   // patrol right boundary
        120.0f,                    // patrol speed
        "assets/monster_slug.png", // texture path
        0.09f                      // scale
    );
    monster_list_add(&level.monsters, slug3);

    // Add unique slug 4
    Monster slug4 = monster_create(
        1300.0f,                   // x position
        535.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        4,                         // max hearts
        2300.0f,                   // patrol left boundary
        2700.0f,                   // patrol right boundary
        100.0f,                    // patrol speed
        "assets/monster_slug.png", // texture path
        0.14f                      // scale
    );
    monster_list_add(&level.monsters, slug4);

    // Sluggato - The mighty boss slug!
    Monster Sluggato = monster_create(
        1300.0f,                   // x position
        425.0f,                    // y position
        100.0f,                    // width
        180.0f,                    // height - increased to block underpass
        7,                         // max hearts - very tough!
        800.0f,                    // patrol left boundary
        3500.0f,                   // patrol right boundary
        80.0f,                     // patrol speed
        "assets/monster_slug.png", // texture path
        0.35f                      // scale - really big!
    );
    monster_list_add(&level.monsters, Sluggato);

    // Add unique slug 5
    Monster slug5 = monster_create(
        800.0f,                    // x position
        549.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        3,                         // max hearts
        1900.0f,                   // patrol left boundary
        2700.0f,                   // patrol right boundary
        110.0f,                    // patrol speed
        "assets/monster_slug.png", // texture path
        0.11f                      // scale
    );
    monster_list_add(&level.monsters, slug5);

    // Add unique slug 6
    Monster slug6 = monster_create(
        1300.0f,                   // x position
        535.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        4,                         // max hearts
        800.0f,                    // patrol left boundary
        1900.0f,                   // patrol right boundary
        100.0f,                    // patrol speed
        "assets/monster_slug.png", // texture path
        0.13f                      // scale
    );
    monster_list_add(&level.monsters, slug6);
    Monster slug7 = monster_create(
        1300.0f,                   // x position
        520.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        5,                         // max hearts
        3100.0f,                   // patrol left boundary
        3500.0f,                   // patrol right boundary
        90.0f,                     // patrol speed
        "assets/monster_slug.png", // texture path
        0.16f                      // scale
    );
    monster_list_add(&level.monsters, slug7);
    Monster slug8 = monster_create(
        1300.0f,                   // x position
        520.0f,                    // y position
        70.0f,                     // width
        70.0f,                     // height
        5,                         // max hearts
        2700.0f,                   // patrol left boundary
        3100.0f,                   // patrol right boundary
        90.0f,                     // patrol speed
        "assets/monster_slug.png", // texture path
        0.16f                      // scale
    );
    monster_list_add(&level.monsters, slug8);

    return level;
}
