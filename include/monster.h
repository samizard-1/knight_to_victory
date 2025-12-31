#ifndef MONSTER_H
#define MONSTER_H

#include "raylib.h"

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    Texture2D texture;
    float scale;
    int hearts;
    int max_hearts;
    float patrol_left_bound;        // Left boundary for patrolling
    float patrol_right_bound;       // Right boundary for patrolling
    float patrol_speed;             // Speed of movement
    bool active;                    // Whether monster is active
    Texture2D filled_heart_texture; // Filled heart texture
    Texture2D empty_heart_texture;  // Empty heart texture
} Monster;

typedef struct
{
    Monster *monsters;
    int count;
    int capacity;
} MonsterList;

// Monster functions
Monster monster_create(float x, float y, float width, float height, int max_hearts,
                       float left_bound, float right_bound, float patrol_speed,
                       const char *texture_path, float scale);
void monster_update(Monster *monster);
void monster_draw(Monster *monster, float camera_x);
void monster_take_damage(Monster *monster, int damage);
bool monster_check_collision(Monster *monster, Rectangle player_rect);

// Monster list functions
MonsterList monster_list_create(int capacity);
void monster_list_add(MonsterList *list, Monster monster);
void monster_list_cleanup(MonsterList *list);

#endif // MONSTER_H
