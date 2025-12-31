#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"

typedef enum
{
    PROJECTILE_FIREBALL
    // Future projectile types can be added here (PROJECTILE_LIGHTNING, etc.)
} ProjectileType;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float speed;
    float width;
    float height;
    Texture2D texture;
    float scale;
    ProjectileType type;
    bool active;
    float lifetime;     // Time remaining for the projectile to live
    float max_lifetime; // Maximum lifetime before despawning
} Projectile;

typedef struct
{
    Projectile *projectiles;
    int count;
    int capacity;
} ProjectileList;

// Projectile functions
Projectile projectile_create_fireball(Vector2 start_pos, Vector2 target_pos);
void projectile_update(Projectile *projectile);
void projectile_draw(Projectile *projectile, float camera_x);

// Projectile list functions
ProjectileList projectile_list_create(int capacity);
void projectile_list_add(ProjectileList *list, Projectile projectile);
void projectile_list_cleanup(ProjectileList *list);

#endif // PROJECTILE_H
