#include "pickup.h"
#include <stdlib.h>
#include <math.h>

// Generic pickup creation function that handles different types
Pickup pickup_create(PickupType type, Vector2 spawn_pos, int value)
{
    Pickup p = {0};

    p.position = spawn_pos;
    p.velocity = (Vector2){0, -300.0f}; // Move straight upward
    p.speed = 300.0f;
    p.type = type;
    p.active = true;
    p.lifetime = 15.0f; // 15 seconds max lifetime
    p.max_lifetime = 15.0f;
    p.rotation = 0.0f;
    p.value = value;

    // Type-specific configuration
    switch (type)
    {
    case PICKUP_FIREBALL:
        p.width = 16.0f;
        p.height = 16.0f;
        p.texture = LoadTexture("assets/fireball.png");
        p.scale = 0.04f;
        break;
    default:
        p.width = 16.0f;
        p.height = 16.0f;
        p.scale = 0.04f;
        break;
    }

    return p;
}

void pickup_update(Pickup *pickup)
{
    if (!pickup->active)
        return;

    // Apply gravity to simulate arc trajectory (bubble popping up then falling)
    float gravity = 300.0f; // Gravity constant
    pickup->velocity.y += gravity * GetFrameTime();

    // Update position based on velocity
    pickup->position.x += pickup->velocity.x * GetFrameTime();
    pickup->position.y += pickup->velocity.y * GetFrameTime();

    // Update rotation for visual effect
    pickup->rotation += 5.0f * GetFrameTime();

    // Update lifetime
    pickup->lifetime -= GetFrameTime();
    if (pickup->lifetime <= 0.0f)
    {
        pickup->active = false;
    }

    // Despawn if projectile goes too far off screen
    float screen_x = pickup->position.x - GetScreenWidth() / 2.0f;
    if (screen_x < -1000.0f || screen_x > GetScreenWidth() + 1000.0f)
    {
        pickup->active = false;
    }

    // Despawn if falls back to ground level or below
    if (pickup->position.y > 700.0f)
    {
        pickup->active = false;
    }
}

void pickup_draw(Pickup *pickup, float camera_x)
{
    if (!pickup->active)
        return;

    // Apply camera offset
    Vector2 screen_pos = (Vector2){
        pickup->position.x - camera_x + GetScreenWidth() / 2.0f,
        pickup->position.y};

    // Offset by half the scaled texture size to center it on the position
    float scaled_width = pickup->width * pickup->scale;
    float scaled_height = pickup->height * pickup->scale;
    screen_pos.x -= scaled_width / 2.0f;
    screen_pos.y -= scaled_height / 2.0f;

    // Draw pickup with rotation
    DrawTextureEx(
        pickup->texture,
        screen_pos,
        pickup->rotation,
        pickup->scale,
        WHITE);
}

PickupList pickup_list_create(int capacity)
{
    PickupList list;
    list.pickups = (Pickup *)malloc(sizeof(Pickup) * capacity);
    list.count = 0;
    list.capacity = capacity;
    return list;
}

void pickup_list_add(PickupList *list, Pickup pickup)
{
    if (list->count < list->capacity)
    {
        list->pickups[list->count++] = pickup;
    }
}

void pickup_list_cleanup(PickupList *list)
{
    if (list->pickups)
    {
        // Unload textures for each pickup
        for (int i = 0; i < list->count; i++)
        {
            if (list->pickups[i].active)
            {
                UnloadTexture(list->pickups[i].texture);
            }
        }
        free(list->pickups);
        list->pickups = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}
// Spawner functions
PickupSpawner pickup_spawner_create(PickupType type, Vector2 location, int value, float interval)
{
    PickupSpawner spawner;
    spawner.type = type;
    spawner.spawn_location = location;
    spawner.value = value;
    spawner.spawn_interval = interval;
    spawner.spawn_timer = interval; // Start ready to spawn
    spawner.enabled = true;
    return spawner;
}

PickupSpawnerList pickup_spawner_list_create(int capacity)
{
    PickupSpawnerList list;
    list.spawners = (PickupSpawner *)malloc(sizeof(PickupSpawner) * capacity);
    list.count = 0;
    list.capacity = capacity;
    return list;
}

void pickup_spawner_list_add(PickupSpawnerList *list, PickupSpawner spawner)
{
    if (list->count < list->capacity)
    {
        list->spawners[list->count++] = spawner;
    }
}

void pickup_spawner_list_cleanup(PickupSpawnerList *list)
{
    if (list->spawners)
    {
        free(list->spawners);
        list->spawners = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}

void pickup_spawner_update(PickupSpawner *spawner, PickupList *pickup_list)
{
    if (!spawner->enabled)
        return;

    spawner->spawn_timer -= GetFrameTime();
    if (spawner->spawn_timer <= 0.0f)
    {
        // Create and add the pickup based on the spawner type
        Pickup pickup = pickup_create(spawner->type, spawner->spawn_location, spawner->value);
        pickup_list_add(pickup_list, pickup);
        spawner->spawn_timer = spawner->spawn_interval; // Reset timer
    }
}