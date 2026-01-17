#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include "hazard.h"
#include "monster.h"
#include "pickup.h"
#include "loot.h"

typedef enum
{
    GOAL_TYPE_LOCATION, // Goal is to reach a specific position
    GOAL_TYPE_HAZARDS,  // Goal is to defeat/destroy a number of hazards
    GOAL_TYPE_MONSTERS  // Goal is to defeat a number of monsters
} GoalType;

typedef enum
{
    BG_TYPE_PROCEDURAL // Procedurally generated background with terrain
} BackgroundType;

typedef struct
{
    BackgroundType type;
    int variant; // Seed variant for procedural generation
} BackgroundConfig;

typedef struct
{
    GoalType type;

    // For GOAL_TYPE_LOCATION
    Vector2 goal_position;
    float goal_radius; // How close player needs to be

    // For GOAL_TYPE_HAZARDS
    int hazards_to_defeat;
    int hazards_defeated;

    // For GOAL_TYPE_MONSTERS
    int monsters_to_defeat;
    int monsters_defeated;
} LevelGoal;

typedef struct
{
    int level_number;
    char *name;
    BackgroundConfig background;
    Vector2 player_start_position;
    LevelGoal goal;
    HazardList hazards;
    MonsterList monsters;
    PickupList pickups;
    bool completed;
    // Pickup spawning configuration
    PickupSpawnerList spawners; // List of pickup spawners for this level
    LootList loot;              // Active loot items in this level
} Level;

// Level functions
Level level_create(int level_number, const char *name, BackgroundConfig background,
                   Vector2 start_pos, LevelGoal goal);
void level_cleanup(Level *level);
bool level_check_goal_reached(Level *level, Vector2 player_pos);
void level_reset(Level *level);
void level_reactivate_enemies(Level *level);

#endif // LEVEL_H
