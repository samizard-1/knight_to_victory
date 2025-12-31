# Level System Guide

The game uses a modular level system where each level is defined in its own file. This makes it easy to manage level-specific features like backgrounds, goals, and future platforms/hazards.

## Level File Structure

Each level has its own file that defines all its properties in one place:

**Example: [../src/level1.c](../src/level1.c)**
```c
Level level1_create(void)
{
    // Define the background for this level
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 0
    };

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,
        .goal_position = {800.0f, 538.0f},
        .goal_radius = 50.0f,
        .hazards_to_defeat = 0,
        .hazards_defeated = 0
    };

    Level level = level_create(
        1,                          // level number
        "Intro Level",              // level name
        background,                 // background configuration
        (Vector2){100.0f, 400.0f},  // player start position
        goal                        // level goal
    );

    return level;
}
```

## Adding a New Level

To add a new level to your game, follow these steps:

### 1. Create a new level file

Create a new file `src/levelN.c` (e.g., `src/level4.c`) with the following structure:

```c
#include "level.h"

/*
 * Level 4: Your Level Name
 * 
 * Features:
 * - Brief description of this level
 * - Goal type and objectives
 * - Background variant used
 * - Any special features
 * 
 * TODO: Add platforms here
 * TODO: Add hazards here
 */

Level level4_create(void)
{
    // Define the background for this level
    BackgroundConfig background = {
        .type = BG_TYPE_PROCEDURAL,
        .variant = 3  // Use a different variant number for different terrain
    };

    LevelGoal goal = {
        .type = GOAL_TYPE_LOCATION,     // or GOAL_TYPE_HAZARDS
        .goal_position = {1500.0f, 538.0f},
        .goal_radius = 50.0f,
        .hazards_to_defeat = 0,
        .hazards_defeated = 0
    };

    Level level = level_create(
        4,                          // level number
        "Your Level Name",          // level name
        background,                 // background configuration
        (Vector2){100.0f, 400.0f},  // player start position
        goal                        // level goal
    );

    return level;
}
```

### 2. Add the function declaration

Add the function declaration to [../include/level_definitions.h](../include/level_definitions.h):

```c
Level level4_create(void);
```

### 3. Update [../src/game.c](../src/game.c)

In the `initialize_levels()` function:

```c
static void initialize_levels(GameState *state)
{
    state->level_count = 4;  // Change from 3 to 4

    // Load levels from level definition files
    state->levels[0] = level1_create();
    state->levels[1] = level2_create();
    state->levels[2] = level3_create();
    state->levels[3] = level4_create();
    state->levels[4] = level5_create();  // Add this line
    state->levels[4] = level8_create();  // Add this line

    state->current_level_index = 0;
}
```

### 4. Update [../CMakeLists.txt](../CMakeLists.txt)

Add your new level file to the executable sources:

```cmake
add_executable(game
    src/main.c
    src/game.c
    src/player.c
    src/background.c
    src/level.c
    src/level1.c
    src/level2.c
    src/level3.c
    src/level4.c
    src/level4.c          # Add this line
)
```

### 5. Update [../include/game.h](../include/game.h) if needed

If you're adding more than 5 levels, increase `MAX_LEVELS`:

```c
#define MAX_LEVELS 10  // Increase from 5 if needed
```

## Level Properties

### BackgroundConfig

Defines the background for a level:

```c
typedef struct {
    BackgroundType type;    // BG_TYPE_PROCEDURAL (can be extended with other types)
    int variant;           // Seed variant for procedural generation (0, 1, 2, 3, etc.)
} BackgroundConfig;
```

Different `variant` values create different procedurally generated landscapes.

### LevelGoal

Defines the goal for a level:

```c
typedef struct {
    GoalType type;                  // GOAL_TYPE_LOCATION or GOAL_TYPE_HAZARDS
    
    // For location-based goals:
    Vector2 goal_position;          // Position to reach
    float goal_radius;              // How close player needs to be (in pixels)
    
    // For hazard-based goals:
    int hazards_to_defeat;          // Number of hazards/enemies to defeat
    int hazards_defeated;           // Counter for defeated hazards
} LevelGoal;
```

**Important Note for Location-Based Goals:** The player walks at approximately `y = 538.0f` (calculated as `GROUND_Y - player_height`). Set location-based goal positions with `y = 538.0f` to match the player's walking height. The `goal_radius` of 50.0f provides a good margin for reaching the goal.

## Goal Types

- **GOAL_TYPE_LOCATION**: Player must reach a specific location marked by a door
- **GOAL_TYPE_HAZARDS**: Player must defeat/destroy a target number of hazards

## Important Notes

- Each level file contains ALL its configuration in one place (background, goal, starting position)
- Background variants (0, 1, 2, 3, etc.) each create uniquely different procedurally generated landscapes
- Levels automatically transition when the goal is reached
- Player position and velocity reset when transitioning to the next level
- Maximum of `MAX_LEVELS` levels can be added (currently 5, easily increased)

## Future Level Features

Each level file includes TODO comments for where to add level-specific features:
- Platforms and obstacles
- Enemies and hazards
- NPCs
- Collectibles
- Special level mechanics
