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

Level level6_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 5};

    LevelGoal goal = {
        .type = GOAL_TYPE_MONSTERS,
        .goal_position = {1000.0f, 538.0f},
        .goal_radius = 50.0f,
        .monsters_to_defeat = 7,
        .monsters_defeated = 0};

    Level level = level_create(
        6,                         // level number
        "Crabono the Cruel",       // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    // Add crab on the middle area
    Monster crab1 = monster_create(
        800.0f,            // x position
        550.0f,            // y position
        70.0f,             // width
        70.0f,             // height
        3,                 // max hearts
        650.0f,            // patrol left boundary
        1100.0f,           // patrol right boundary
        170.0f,            // patrol speed
        "crab.png", // texture path
        0.12f              // scale
    );
    monster_list_add(&level.monsters, crab1);

    // Add strong crab
    Monster crab2 = monster_create(
        1300.0f,           // x position
        550.0f,            // y position
        70.0f,             // width
        70.0f,             // height
        3,                 // max hearts
        1100.0f,           // patrol left boundary
        1900.0f,           // patrol right boundary
        160.0f,            // patrol speed
        "crab.png", // texture path
        0.13f              // scale
    );
    monster_list_add(&level.monsters, crab2);
    Monster crab3 = monster_create(
        800.0f,            // x position
        550.0f,            // y position
        70.0f,             // width
        70.0f,             // height
        3,                 // max hearts
        1900.0f,           // patrol left boundary
        2300.0f,           // patrol right boundary
        170.0f,            // patrol speed
        "crab.png", // texture path
        0.12f              // scale
    );
    monster_list_add(&level.monsters, crab3);

    // Add strong crab
    Monster crab4 = monster_create(
        1300.0f,           // x position
        550.0f,            // y position
        70.0f,             // width
        70.0f,             // height
        3,                 // max hearts
        2300.0f,           // patrol left boundary
        2700.0f,           // patrol right boundary
        160.0f,            // patrol speed
        "crab.png", // texture path
        0.13f              // scale
    );
    monster_list_add(&level.monsters, crab4);
    Monster Crabono = monster_create(
        1300.0f,           // x position
        520.0f,            // y position
        70.0f,             // width
        70.0f,             // height
        5,                 // max hearts
        800.0f,            // patrol left boundary
        2700.0f,           // patrol right boundary
        140.0f,            // patrol speed
        "crab.png", // texture path
        0.19f              // scale
    );
    monster_list_add(&level.monsters, Crabono);
    Monster crab5 = monster_create(
        800.0f,            // x position
        567.0f,            // y position
        70.0f,             // width
        70.0f,             // height
        3,                 // max hearts
        1900.0f,           // patrol left boundary
        2700.0f,           // patrol right boundary
        170.0f,            // patrol speed
        "crab.png", // texture path
        0.12f              // scale
    );
    monster_list_add(&level.monsters, crab5);

    // Add strong crab
    Monster crab6 = monster_create(
        1300.0f,           // x position
        550.0f,            // y position
        70.0f,             // width
        70.0f,             // height
        3,                 // max hearts
        800.0f,            // patrol left boundary
        1900.0f,           // patrol right boundary
        170.0f,            // patrol speed
        "crab.png", // texture path
        0.13f              // scale
    );
    monster_list_add(&level.monsters, crab6);

    return level;
}
