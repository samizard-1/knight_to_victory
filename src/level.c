#include "level.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

Level level_create(int level_number, const char *name, BackgroundConfig background,
                   Vector2 start_pos, LevelGoal goal)
{
    Level level;
    level.level_number = level_number;
    level.name = (char *)malloc(strlen(name) + 1);
    strcpy(level.name, name);

    level.background = background;
    level.player_start_position = start_pos;
    level.goal = goal;
    level.completed = false;
    level.hazards = hazard_list_create(10);          // Max 10 hazards per level
    level.monsters = monster_list_create(20);        // Max 20 monsters per level
    level.pickups = pickup_list_create(20);          // Max 20 pickups per level
    level.spawners = pickup_spawner_list_create(10); // Max 10 spawners per level

    return level;
}

void level_cleanup(Level *level)
{
    if (level->name != NULL)
    {
        free(level->name);
        level->name = NULL;
    }
    hazard_list_cleanup(&level->hazards);
    monster_list_cleanup(&level->monsters);
    pickup_list_cleanup(&level->pickups);
    pickup_spawner_list_cleanup(&level->spawners);
}

bool level_check_goal_reached(Level *level, Vector2 player_pos)
{
    switch (level->goal.type)
    {
    case GOAL_TYPE_LOCATION:
    {
        // Check if player is within goal radius
        float dx = player_pos.x - level->goal.goal_position.x;
        float dy = player_pos.y - level->goal.goal_position.y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < level->goal.goal_radius)
        {
            if (!level->completed)
            {
                level->completed = true;
                return true;
            }
        }
        break;
    }
    case GOAL_TYPE_HAZARDS:
    {
        // Check if enough hazards have been defeated
        if (level->goal.hazards_defeated >= level->goal.hazards_to_defeat)
        {
            if (!level->completed)
            {
                level->completed = true;
                return true;
            }
        }
        break;
    }
    case GOAL_TYPE_MONSTERS:
    {
        // Check if enough monsters have been defeated
        if (level->goal.monsters_defeated >= level->goal.monsters_to_defeat)
        {
            if (!level->completed)
            {
                level->completed = true;
                return true;
            }
        }
        break;
    }
    }

    return false;
}

void level_reset(Level *level)
{
    level->completed = false;
    level->goal.hazards_defeated = 0;
    level->goal.monsters_defeated = 0;

    // Reset all hazards to their initial positions
    for (int i = 0; i < level->hazards.count; i++)
    {
        hazard_reset(&level->hazards.hazards[i]);
    }

    // Reset all spawner timers
    for (int i = 0; i < level->spawners.count; i++)
    {
        level->spawners.spawners[i].spawn_timer = level->spawners.spawners[i].spawn_interval;
    }

    // Reset all pickups to active
    for (int i = 0; i < level->pickups.count; i++)
    {
        level->pickups.pickups[i].active = true;
        level->pickups.pickups[i].lifetime = level->pickups.pickups[i].max_lifetime;
    }
}

void level_reactivate_enemies(Level *level)
{
    // Reactivate all monsters with full health
    for (int i = 0; i < level->monsters.count; i++)
    {
        level->monsters.monsters[i].active = true;
        level->monsters.monsters[i].hearts = level->monsters.monsters[i].max_hearts;
    }

    // Reactivate all hazards
    for (int i = 0; i < level->hazards.count; i++)
    {
        level->hazards.hazards[i].active = true;
    }
}
