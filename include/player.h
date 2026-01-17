#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "damage.h"
#include "loot.h"

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    float speed;
    float jump_power;
    bool is_jumping;
    Texture2D texture;
    Texture2D flipleft_texture;         // Character texture for moving left
    Texture2D hurt_texture;             // Character texture when hit by monster
    Texture2D hurt_flipleft_texture;    // Character hurt texture for moving left
    Texture2D on_fire_texture;          // Character texture when on fire
    Texture2D on_fire_flipleft_texture; // Character on fire texture for moving left
    Texture2D dust_texture;             // Character texture when affected by dust
    Texture2D dust_flipleft_texture;    // Character dust texture for moving left
    Texture2D dead_texture;
    Texture2D filled_heart_texture;
    Texture2D empty_heart_texture;
    Texture2D fireball_texture;
    Texture2D sword_texture;
    Texture2D sword_flipleft_texture;
    Texture2D ducking_texture;
    Texture2D ducking_flipleft_texture;
    Texture2D protection_potion_texture;
    Rectangle sword_hitbox;
    float scale;
    int hearts;
    int max_hearts;
    bool is_dead;
    DamageType active_damage_type; // Current active damage type
    float damage_timer;            // Timer for damage effect duration
    int projectile_inventory;      // Number of projectiles currently available
    int max_projectiles;           // Maximum projectiles player can carry
    int facing_direction;          // 1 for right, -1 for left
    bool is_using_sword;
    bool is_ducking;               // True when player is using sword attack
    bool protection_potion_active; // True when protection potion is in effect
    float protection_potion_timer; // Timer for protection potion duration
    Inventory inventory;           // Inventory system for tracking collected loot

} Player;

// Player functions
Player player_create(float x, float y);
void player_update(Player *player);
void player_update_with_hazards(Player *player, const void *hazard_list);
void player_update_sword_hitbox(Player *player);
void player_draw(Player *player, float camera_x);
void player_handle_input(Player *player);
void player_take_damage(Player *player, int damage);
void player_apply_damage_type(Player *player, DamageType damage_type, float duration);
void player_clear_damage_type(Player *player);
void player_heal(Player *player, int amount);
void player_cleanup(Player *player);

#endif // PLAYER_H;
