#include "monster.h"
#include "asset_paths.h"
#include <stdlib.h>

Monster monster_create(float x, float y, float width, float height, int max_hearts,
                       float left_bound, float right_bound, float patrol_speed,
                       const char *texture_path, float scale)
{
    Monster m;
    m.position = (Vector2){x, y};
    m.velocity = (Vector2){patrol_speed, 0}; // Start moving right
    m.width = width;
    m.height = height;
    m.texture = LoadTexture(get_asset_path(texture_path));
    m.scale = scale;
    m.hearts = max_hearts;
    m.max_hearts = max_hearts;
    m.patrol_left_bound = left_bound;
    m.patrol_right_bound = right_bound;
    m.patrol_speed = patrol_speed;
    m.active = true;

    // Load heart textures
    m.filled_heart_texture = LoadTexture(get_asset_path("filled_heart.png"));
    m.empty_heart_texture = LoadTexture(get_asset_path("empty_heart.png"));

    return m;
}

void monster_update(Monster *monster)
{
    if (!monster->active)
        return;

    // Update position based on velocity
    monster->position.x += monster->velocity.x * GetFrameTime();

    // Patrol logic - bounce back and forth at boundaries
    if (monster->position.x <= monster->patrol_left_bound)
    {
        monster->position.x = monster->patrol_left_bound;
        monster->velocity.x = monster->patrol_speed; // Move right
    }
    else if (monster->position.x >= monster->patrol_right_bound)
    {
        monster->position.x = monster->patrol_right_bound;
        monster->velocity.x = -monster->patrol_speed; // Move left
    }
}

void monster_draw(Monster *monster, float camera_x)
{
    if (!monster->active)
        return;

    // Apply camera offset to center on screen
    Vector2 screen_pos = (Vector2){
        monster->position.x - camera_x + GetScreenWidth() / 2.0f,
        monster->position.y};

    // Draw monster texture
    DrawTextureEx(
        monster->texture,
        screen_pos,
        0.0f,
        monster->scale,
        WHITE);

    // draw monster texture hitbox for testing
    /*
    Rectangle monster_rect = {
        monster->position.x,
        monster->position.y,
        monster->width,
        monster->height};
    DrawRectangle(
        monster_rect.x - camera_x + GetScreenWidth() / 2.0f,
        monster_rect.y,
        monster_rect.width,
        monster_rect.height,
        RED);
    */

    // Draw hearts above monster using textures
    float heart_size = 24.0f;
    float heart_spacing = 28.0f;
    float total_hearts_width = (monster->max_hearts * heart_spacing) - 4.0f; // Spacing adjustment

    // Calculate actual drawn dimensions of monster texture
    float drawn_width = monster->texture.width * monster->scale;
    float drawn_height = monster->texture.height * monster->scale;
    float monster_center_x = screen_pos.x + drawn_width / 2.0f;
    float hearts_start_x = monster_center_x - total_hearts_width / 2.0f;
    float hearts_y = screen_pos.y - drawn_height - 15.0f; // Above the monster

    // Draw max hearts first (empty hearts)
    for (int i = 0; i < monster->max_hearts; i++)
    {
        Rectangle source = {0, 0, (float)monster->empty_heart_texture.width, (float)monster->empty_heart_texture.height};
        Rectangle dest = {hearts_start_x + i * heart_spacing, hearts_y, heart_size, heart_size};
        DrawTexturePro(monster->empty_heart_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }

    // Draw current hearts on top (filled hearts)
    for (int i = 0; i < monster->hearts; i++)
    {
        Rectangle source = {0, 0, (float)monster->filled_heart_texture.width, (float)monster->filled_heart_texture.height};
        Rectangle dest = {hearts_start_x + i * heart_spacing, hearts_y, heart_size, heart_size};
        DrawTexturePro(monster->filled_heart_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

void monster_take_damage(Monster *monster, int damage)
{
    if (!monster->active)
        return;

    monster->hearts -= damage;
    if (monster->hearts <= 0)
    {
        monster->hearts = 0;
        monster->active = false;
    }
}

bool monster_check_collision(Monster *monster, Rectangle player_rect)
{
    if (!monster->active)
        return false;

    Rectangle monster_rect = {
        monster->position.x,
        monster->position.y,
        monster->width,
        monster->height};

    return CheckCollisionRecs(monster_rect, player_rect);
}

MonsterList monster_list_create(int capacity)
{
    MonsterList list;
    list.monsters = (Monster *)malloc(sizeof(Monster) * capacity);
    list.count = 0;
    list.capacity = capacity;
    return list;
}

void monster_list_add(MonsterList *list, Monster monster)
{
    if (list->count < list->capacity)
    {
        list->monsters[list->count++] = monster;
    }
}

void monster_list_cleanup(MonsterList *list)
{
    if (list->monsters)
    {
        // Unload textures for each monster
        for (int i = 0; i < list->count; i++)
        {
            UnloadTexture(list->monsters[i].texture);
            UnloadTexture(list->monsters[i].filled_heart_texture);
            UnloadTexture(list->monsters[i].empty_heart_texture);
        }
        free(list->monsters);
        list->monsters = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}
