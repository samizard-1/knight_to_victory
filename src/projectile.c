#include "projectile.h"
#include "asset_paths.h"
#include <stdlib.h>
#include <math.h>

Projectile projectile_create_fireball(Vector2 start_pos, Vector2 target_pos, ProjectileSource source)
{
    Projectile p;
    p.position = start_pos;

    // Calculate direction to target
    float dx = target_pos.x - start_pos.x;
    float dy = target_pos.y - start_pos.y;
    float distance = sqrtf(dx * dx + dy * dy);

    // Normalize direction and apply speed
    p.speed = 400.0f; // Fireball speed
    if (distance > 0)
    {
        p.velocity = (Vector2){(dx / distance) * p.speed, (dy / distance) * p.speed};
    }
    else
    {
        p.velocity = (Vector2){0, 0};
    }

    p.width = 16.0f;
    p.height = 16.0f;
    p.texture = LoadTexture(get_asset_path("fireball.png"));
    p.scale = 0.04f;
    p.type = PROJECTILE_FIREBALL;
    p.source = source;
    p.active = true;
    p.lifetime = 10.0f; // 10 seconds max lifetime
    p.max_lifetime = 10.0f;

    return p;
}

void projectile_update(Projectile *projectile)
{
    if (!projectile->active)
        return;

    // Update position based on velocity
    projectile->position.x += projectile->velocity.x * GetFrameTime();
    projectile->position.y += projectile->velocity.y * GetFrameTime();

    // Update lifetime
    projectile->lifetime -= GetFrameTime();
    if (projectile->lifetime <= 0.0f)
    {
        projectile->active = false;
    }

    // Despawn if projectile goes too far off screen
    float screen_x = projectile->position.x - GetScreenWidth() / 2.0f;
    if (screen_x < -1000.0f || screen_x > GetScreenWidth() + 1000.0f)
    {
        projectile->active = false;
    }
}

void projectile_draw(Projectile *projectile, float camera_x)
{
    if (!projectile->active)
        return;

    // Apply camera offset
    Vector2 screen_pos = (Vector2){
        projectile->position.x - camera_x + GetScreenWidth() / 2.0f,
        projectile->position.y};

    // Offset by half the scaled texture size to center it on the position
    float scaled_width = projectile->width * projectile->scale;
    float scaled_height = projectile->height * projectile->scale;
    screen_pos.x -= scaled_width / 2.0f;
    screen_pos.y -= scaled_height / 2.0f;

    // Draw projectile
    DrawTextureEx(
        projectile->texture,
        screen_pos,
        0.0f,
        projectile->scale,
        WHITE);
}

bool projectile_check_player_collision(Projectile *projectile, Rectangle player_rect)
{
    if (!projectile->active || projectile->source == PROJECTILE_SOURCE_PLAYER)
        return false;

    Rectangle projectile_rect = {
        projectile->position.x,
        projectile->position.y,
        projectile->width,
        projectile->height};

    return CheckCollisionRecs(projectile_rect, player_rect);
}

ProjectileList projectile_list_create(int capacity)
{
    ProjectileList list;
    list.projectiles = (Projectile *)malloc(sizeof(Projectile) * capacity);
    list.count = 0;
    list.capacity = capacity;
    return list;
}

void projectile_list_add(ProjectileList *list, Projectile projectile)
{
    if (list->count < list->capacity)
    {
        list->projectiles[list->count++] = projectile;
    }
}

void projectile_list_cleanup(ProjectileList *list)
{
    if (list->projectiles)
    {
        // Unload textures for each projectile
        for (int i = 0; i < list->count; i++)
        {
            if (list->projectiles[i].active)
            {
                UnloadTexture(list->projectiles[i].texture);
            }
        }
        free(list->projectiles);
        list->projectiles = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}
