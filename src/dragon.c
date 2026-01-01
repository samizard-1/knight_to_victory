#include "dragon.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

void dragon_draw_hearts(Monster *dragon, float screen_pos_x, float screen_pos_y)
{
    // Custom heart drawing for dragon - positioned closer to the dragon's head
    float heart_size = 24.0f;
    float heart_spacing = 28.0f;
    float total_hearts_width = (dragon->max_hearts * heart_spacing) - 4.0f; // Spacing adjustment

    // Calculate actual drawn dimensions of monster texture
    float drawn_width = dragon->texture.width * dragon->scale;
    float drawn_height = dragon->texture.height * dragon->scale;
    float monster_center_x = screen_pos_x + drawn_width / 2.0f;
    float hearts_start_x = monster_center_x - total_hearts_width / 2.0f;

    // Position hearts closer to the dragon's head (halfway down the monster)
    float hearts_y = screen_pos_y + (drawn_height / 2.0f) - 200.0f;

    // Draw max hearts first (empty hearts)
    for (int i = 0; i < dragon->max_hearts; i++)
    {
        Rectangle source = {0, 0, (float)dragon->empty_heart_texture.width, (float)dragon->empty_heart_texture.height};
        Rectangle dest = {hearts_start_x + i * heart_spacing, hearts_y, heart_size, heart_size};
        DrawTexturePro(dragon->empty_heart_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }

    // Draw current hearts on top (filled hearts)
    for (int i = 0; i < dragon->hearts; i++)
    {
        Rectangle source = {0, 0, (float)dragon->filled_heart_texture.width, (float)dragon->filled_heart_texture.height};
        Rectangle dest = {hearts_start_x + i * heart_spacing, hearts_y, heart_size, heart_size};
        DrawTexturePro(dragon->filled_heart_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

void dragon_custom_update(Monster *dragon)
{
    if (!dragon->active)
        return;

    DragonData *data = (DragonData *)dragon->custom_data;

    // Update fire cooldown
    data->fire_cooldown -= GetFrameTime();

    // Default patrol behavior (from monster.c)
    dragon->position.x += dragon->velocity.x * GetFrameTime();

    // Patrol logic - bounce back and forth at boundaries
    if (dragon->position.x <= dragon->patrol_left_bound)
    {
        dragon->position.x = dragon->patrol_left_bound;
        dragon->velocity.x = dragon->patrol_speed; // Move right
    }
    else if (dragon->position.x >= dragon->patrol_right_bound)
    {
        dragon->position.x = dragon->patrol_right_bound;
        dragon->velocity.x = -dragon->patrol_speed; // Move left
    }
}

void dragon_cleanup(Monster *dragon)
{
    if (dragon->custom_data)
    {
        free(dragon->custom_data);
        dragon->custom_data = NULL;
    }
}

void dragon_apply_customizations(Monster *dragon)
{
    // Allocate and initialize dragon-specific data
    DragonData *data = (DragonData *)malloc(sizeof(DragonData));
    data->fire_cooldown = 0.0f;
    data->fire_cooldown_max = 2.0f; // Fire every 2 seconds
    data->firing_range = 800.0f;    // Only fire if player is within this distance

    dragon->custom_data = data;

    // Apply all dragon-specific customizations to the monster
    dragon->draw_hearts = dragon_draw_hearts;
    dragon->custom_update = dragon_custom_update;
    dragon->custom_cleanup = dragon_cleanup;
}

void dragon_fire_at_target(Monster *dragon, ProjectileList *projectiles, Vector2 target_pos)
{
    if (!dragon->active)
        return;

    DragonData *data = (DragonData *)dragon->custom_data;

    // Check if cooldown is ready
    if (data->fire_cooldown > 0.0f)
        return;

    // Check if target is within firing range
    float dx = target_pos.x - dragon->position.x;
    float dy = target_pos.y - dragon->position.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance > data->firing_range)
        return;

    // Fire projectile from dragon's position
    Projectile fireball = projectile_create_fireball(
        (Vector2){dragon->position.x + dragon->width / 2.0f, dragon->position.y},
        target_pos,
        PROJECTILE_SOURCE_MONSTER);

    projectile_list_add(projectiles, fireball);

    // Reset cooldown
    data->fire_cooldown = data->fire_cooldown_max;
}
