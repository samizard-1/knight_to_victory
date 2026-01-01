#include "dragon.h"
#include "raylib.h"

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

void dragon_apply_customizations(Monster *dragon)
{
    // Apply all dragon-specific customizations to the monster
    dragon->draw_hearts = dragon_draw_hearts;
    // dragon->custom_update = dragon_update; // Can add custom update logic here
    // dragon->custom_cleanup = dragon_cleanup; // Can add custom cleanup logic here
}
