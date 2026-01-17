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

Level level9_create(void)
{
    // Define the background for this level (variant 5 creates the final terrain)
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 8};

    LevelGoal goal = {
        .type = GOAL_TYPE_MONSTERS,
        .goal_position = {1000.0f, 538.0f},
        .goal_radius = 50.0f,
        .monsters_to_defeat = 20,
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
    Monster guard = monster_create(
        800.0f,                       // x position
        550.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        650.0f,                       // patrol left boundary
        1100.0f,                      // patrol right boundary
        120.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard);

    // Add unique slug 2
    Monster guard1 = monster_create(
        1300.0f,                      // x position
        520.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        1100.0f,                      // patrol left boundary
        1900.0f,                      // patrol right boundary
        140.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard1);

    // Add unique slug 3
    Monster guard2 = monster_create(
        800.0f,                       // x position
        560.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        1900.0f,                      // patrol left boundary
        2300.0f,                      // patrol right boundary
        120.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard2);

    // Add unique slug 4
    Monster guard3 = monster_create(
        1300.0f,                      // x position
        535.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        2300.0f,                      // patrol left boundary
        2700.0f,                      // patrol right boundary
        100.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard3);

    // Sluggato - The mighty boss slug!
    Monster guard4 = monster_create(
        1300.0f,                      // x position
        425.0f,                       // y position
        100.0f,                       // width
        180.0f,                       // height - increased to block underpass
        3,                            // max hearts - very tough!
        800.0f,                       // patrol left boundary
        3500.0f,                      // patrol right boundary
        80.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale - really big!
    "bat"
    );
    monster_list_add(&level.monsters, guard4);

    // Add unique slug 5
    Monster guard5 = monster_create(
        800.0f,                       // x position
        549.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        1900.0f,                      // patrol left boundary
        2700.0f,                      // patrol right boundary
        110.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard5);

    // Add unique slug 6
    Monster guard6 = monster_create(
        1300.0f,                      // x position
        535.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        800.0f,                       // patrol left boundary
        1900.0f,                      // patrol right boundary
        100.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard6);
        Monster guard7 = monster_create(
        1300.0f,                      // x position
        520.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        3100.0f,                      // patrol left boundary
        3500.0f,                      // patrol right boundary
        90.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard7);
        Monster guard8 = monster_create(
        1300.0f,                      // x position
        520.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        2700.0f,                      // patrol left boundary
        3100.0f,                      // patrol right boundary
        90.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard8);
        // Add unique slug 3
    Monster guard9 = monster_create(
        800.0f,                       // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        1700.0f,                      // patrol left boundary
        2100.0f,                      // patrol right boundary
        145.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard9);

    // Add unique slug 4
    Monster guard10 = monster_create(
        1300.0f,                      // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        2100.0f,                      // patrol left boundary
        2500.0f,                      // patrol right boundary
        135.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard10);

    // Sluggato - The mighty boss slug!
    Monster guard11 = monster_create(
        1300.0f,                      // x position
        450.0f,                       // y position
        100.0f,                       // width
        180.0f,                       // height - increased to block underpass
        3,                            // max hearts - very tough!
        600.0f,                       // patrol left boundary
        3300.0f,                      // patrol right boundary
        135.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale - really big!
    "bat"
    );
    monster_list_add(&level.monsters, guard11);

    // Add unique slug 5
    Monster guard12 = monster_create(
        800.0f,                       // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        1700.0f,                      // patrol left boundary
        2500.0f,                      // patrol right boundary
        135.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard12);

    // Add unique slug 6
    Monster guard13 = monster_create(
        1300.0f,                      // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        600.0f,                       // patrol left boundary
        1700.0f,                      // patrol right boundary
        145.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard13);
        Monster guard14 = monster_create(
        1300.0f,                      // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        2900.0f,                      // patrol left boundary
        3300.0f,                      // patrol right boundary
        135.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard14);
        Monster guard15 = monster_create(
        1300.0f,                      // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        2500.0f,                      // patrol left boundary
        2900.0f,                      // patrol right boundary
        145.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard15);
       // Add unique slug 5
       //blip
    Monster guard16 = monster_create(
        800.0f,                       // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        1800.0f,                      // patrol left boundary
        2600.0f,                      // patrol right boundary
        130.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard16);

    // Add unique slug 6
    Monster guard17 = monster_create(
        1300.0f,                      // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        700.0f,                       // patrol left boundary
        1800.0f,                      // patrol right boundary
        140.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard17);
        Monster guard18 = monster_create(
        1300.0f,                      // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        3,                            // max hearts
        3000.0f,                      // patrol left boundary
        3400.0f,                      // patrol right boundary
        130.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.1f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard18);
        Monster guard19 = monster_create(
        1300.0f,                      // x position
        490.0f,                       // y position
        70.0f,                        // width
        70.0f,                        // height
        2,                            // max hearts
        2600.0f,                      // patrol left boundary
        3000.0f,                      // patrol right boundary
        140.0f,                       // patrol speed
        "../assets/bat.png", // texture path
        0.08f,// scale
    "bat"
    );
    monster_list_add(&level.monsters, guard19);

    return level;
}
