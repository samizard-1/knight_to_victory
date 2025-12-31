#ifndef PICKUP_H
#define PICKUP_H

#include "raylib.h"

typedef enum
{
    PICKUP_FIREBALL
} PickupType;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    float speed;
    Texture2D texture;
    float scale;
    PickupType type;
    bool active;
    float lifetime;
    float max_lifetime;
    float rotation;
    int value; // How much this pickup gives (e.g., projectiles added)
} Pickup;

typedef struct
{
    Pickup *pickups;
    int count;
    int capacity;
} PickupList;

// Pickup spawner configuration for flexible spawning of different pickup types
typedef struct
{
    Vector2 spawn_location; // Where pickups spawn from
    PickupType type;        // What type of pickup to spawn
    int value;              // Value of spawned pickups (e.g., projectiles)
    float spawn_interval;   // Time between spawns
    float spawn_timer;      // Current spawn timer
    bool enabled;           // Whether this spawner is active
} PickupSpawner;

typedef struct
{
    PickupSpawner *spawners;
    int count;
    int capacity;
} PickupSpawnerList;

// Pickup functions
Pickup pickup_create(PickupType type, Vector2 spawn_pos, int value);
void pickup_update(Pickup *pickup);
void pickup_draw(Pickup *pickup, float camera_x);
PickupList pickup_list_create(int capacity);
void pickup_list_add(PickupList *list, Pickup pickup);
void pickup_list_cleanup(PickupList *list);

// Spawner functions
PickupSpawner pickup_spawner_create(PickupType type, Vector2 location, int value, float interval);
PickupSpawnerList pickup_spawner_list_create(int capacity);
void pickup_spawner_list_add(PickupSpawnerList *list, PickupSpawner spawner);
void pickup_spawner_list_cleanup(PickupSpawnerList *list);
void pickup_spawner_update(PickupSpawner *spawner, PickupList *pickup_list);

#endif // PICKUP_H
