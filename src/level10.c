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

Level level10_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 9};

    LevelGoal goal = {
        .type = GOAL_TYPE_MONSTERS,
        .goal_position = {1000.0f, 538.0f},
        .goal_radius = 50.0f,
        .monsters_to_defeat = 2,
        .monsters_defeated = 0};

    Level level = level_create(
        9,                         // level number
        "The Dragon's Guards",     // level name
        background,                // background configuration
        (Vector2){100.0f, 400.0f}, // player start position
        goal                       // level goal
    );

    // Add unique slug 1
    //"bat"
    Monster dragon = monster_create(
        800.0f,                 // x position
        305.0f,                 // y position
        475.0f,                 // width
        300.0f,                 // height
        12,                     // max hearts
        850.0f,                 // patrol left boundary
        950.0f,                 // patrol right boundary
        90.0f,                  // patrol speed
        "../assets/dragon.png", // texture path
        0.5f                    // scale
    );

    // Apply dragon-specific customizations
    dragon_apply_customizations(&dragon);

    monster_list_add(&level.monsters, dragon);
    Monster baby1 = monster_create(
        800.0f,                 // x position
        525.0f,                 // y position
        475.0f,                 // width
        300.0f,                 // height
        6,                     // max hearts
        650.0f,                 // patrol left boundary
        850.0f,                 // patrol right boundary
        135.0f,                  // patrol speed
        "../assets/baby_dragon.png", // texture path
        0.18f                    // scale
    );
    monster_list_add(&level.monsters, baby1);
    return level;
}
