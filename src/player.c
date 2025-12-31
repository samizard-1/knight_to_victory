#include "player.h"
#include "hazard.h"
#include "config.h"
#include "asset_paths.h"
#include <stddef.h>

Player player_create(float x, float y)
{
    Player p;
    p.position = (Vector2){x, y};
    p.velocity = (Vector2){0, 0};
    p.speed = 200.0f;
    p.jump_power = 400.0f;
    p.is_jumping = false;

    // Load character texture
    p.texture = LoadTexture(get_asset_path("character.png"));
    p.flipleft_texture = LoadTexture(get_asset_path("character_flipleft.png"));
    p.hurt_texture = LoadTexture(get_asset_path("character_hurt.png"));
    p.hurt_flipleft_texture = LoadTexture(get_asset_path("character_hurt_flipleft.png"));
    p.on_fire_texture = LoadTexture(get_asset_path("character_on_fire.png"));
    p.on_fire_flipleft_texture = LoadTexture(get_asset_path("character_on_fire_flipleft.png"));
    p.sword_texture = LoadTexture(get_asset_path("sword.png"));
    p.sword_flipleft_texture = LoadTexture(get_asset_path("sword_flipleft.png"));
    p.sword_hitbox = (Rectangle){0, 0, 20, 40}; // Example sword hitbox size
    p.scale = 0.06f;                            // Scale down smaller
    p.width = (float)p.texture.width * p.scale;
    p.height = (float)p.texture.height * p.scale;

    // Load dead texture (optional - create a simple fallback if file doesn't exist)
    p.dead_texture = LoadTexture(get_asset_path("dead_character.png"));

    // Load heart textures
    p.filled_heart_texture = LoadTexture(get_asset_path("filled_heart.png"));
    p.empty_heart_texture = LoadTexture(get_asset_path("empty_heart.png"));

    // Load fireball texture for inventory display
    p.fireball_texture = LoadTexture(get_asset_path("fireball.png"));

    // Initialize health
    p.hearts = INITIAL_HEARTS;
    p.max_hearts = MAX_HEARTS;
    p.is_dead = false;
    p.is_hurt = false;
    p.hurt_timer = 0.0f;
    p.is_burnt = false;
    p.burnt_timer = 0.0f;

    // Initialize projectile inventory
    p.projectile_inventory = 0; // Start with 0 projectiles
    p.max_projectiles = 10;     // Maximum 10 projectiles can carry

    // Initialize facing direction (1 = right, -1 = left)
    p.facing_direction = 1;

    // Initialize sword usage state
    p.is_using_sword = false;

    return p;
}

void player_handle_input(Player *player)
{
    // Horizontal movement
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        player->velocity.x = -player->speed;
        player->facing_direction = -1; // Facing left
    }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        player->velocity.x = player->speed;
        player->facing_direction = 1; // Facing right
    }
    else
    {
        player->velocity.x = 0;
    }

    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        player->is_using_sword = true;
    }
    else
    {
        player->is_using_sword = false;
    }

    // Jumping
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && !player->is_jumping)
    {
        player->velocity.y = -player->jump_power;
        player->is_jumping = true;
    }
}

void player_update(Player *player)
{
    float delta_time = GetFrameTime();

    // Update hurt timer
    if (player->is_hurt)
    {
        player->hurt_timer -= delta_time;
        if (player->hurt_timer <= 0.0f)
        {
            player->is_hurt = false;
        }
    }

    // Apply gravity
    player->velocity.y += GRAVITY * delta_time;

    // Update position
    player->position.x += player->velocity.x * delta_time;
    player->position.y += player->velocity.y * delta_time;

    // Ground collision
    if (player->position.y + player->height >= GROUND_Y)
    {
        player->position.y = GROUND_Y - player->height;
        player->velocity.y = 0;
        player->is_jumping = false;
    }
}

void player_update_with_hazards(Player *player, const void *hazard_list)
{
    float delta_time = GetFrameTime();

    // Update hurt timer
    if (player->is_hurt)
    {
        player->hurt_timer -= delta_time;
        if (player->hurt_timer <= 0.0f)
        {
            player->is_hurt = false;
        }
    }

    // Apply gravity
    player->velocity.y += GRAVITY * delta_time;

    // Update position
    player->position.x += player->velocity.x * delta_time;
    player->position.y += player->velocity.y * delta_time;

    // Ground collision with hazard awareness
    bool is_over_gap = false;

    if (hazard_list != NULL)
    {
        const HazardList *hazards = (const HazardList *)hazard_list;

        // Check if player is over a lava pit gap
        Rectangle player_rect = {
            player->position.x,
            player->position.y,
            player->width,
            player->height};

        for (int i = 0; i < hazards->count; i++)
        {
            const Hazard *hazard = &hazards->hazards[i];

            // Check if player is horizontally over a lava pit
            if (hazard->active && hazard->type == HAZARD_LAVA_PIT)
            {
                // Check if player's center is within the pit's horizontal bounds
                float player_center_x = player->position.x + player->width / 2.0f;
                if (player_center_x >= hazard->bounds.x &&
                    player_center_x <= hazard->bounds.x + hazard->bounds.width)
                {
                    // Allow player to fall through
                    is_over_gap = true;
                    break;
                }
            }
        }
    }

    // Only apply ground collision if not over a gap
    if (!is_over_gap && player->position.y + player->height >= GROUND_Y)
    {
        player->position.y = GROUND_Y - player->height;
        player->velocity.y = 0;
        player->is_jumping = false;
    }
}

void player_update_sword_hitbox(Player *player)
{
    // Update sword hitbox based on current state
    if (player->is_using_sword)
    {
        float sword_scale = player->scale * 1.5;
        if (player->facing_direction == 1)
        {
            player->sword_hitbox = (Rectangle){
                player->position.x + player->width * 0.5f,
                player->position.y + player->height * 0.3f,
                player->sword_texture.width * sword_scale,
                player->sword_texture.height * sword_scale};
        }
        else
        {
            player->sword_hitbox = (Rectangle){
                player->position.x - player->width * 0.5f - player->sword_flipleft_texture.width * sword_scale,
                player->position.y + player->height * 0.3f,
                player->sword_flipleft_texture.width * sword_scale,
                player->sword_flipleft_texture.height * sword_scale};
        }
    }
}

void player_draw(Player *player, float camera_x)
{
    // Apply camera offset to center player on screen
    Vector2 screen_pos = (Vector2){
        player->position.x - camera_x + GetScreenWidth() / 2.0f,
        player->position.y};

    // Choose texture based on alive/dead state and facing direction
    Texture2D texture_to_draw = player->texture;
    float draw_scale = player->scale;

    if (player->is_dead)
    {
        texture_to_draw = player->dead_texture;
        draw_scale = player->scale * 1.5f; // Draw dead texture 1.5x larger
        // Fallback to alive texture if dead texture isn't loaded
        if (player->dead_texture.id == 0)
        {
            texture_to_draw = player->texture;
        }
        else
        {
            // Adjust Y position so dead texture sits on ground instead of floating
            float original_height = (float)player->texture.height * player->scale;
            float dead_height = (float)player->dead_texture.height * draw_scale;
            screen_pos.y -= (dead_height - original_height);
        }
    }
    else if (player->is_hurt)
    {
        // Use hurt texture when hit by monster
        if (player->facing_direction == -1)
        {
            texture_to_draw = player->hurt_flipleft_texture;
            if (player->hurt_flipleft_texture.id == 0)
            {
                texture_to_draw = player->hurt_texture;
            }
        }
        else
        {
            texture_to_draw = player->hurt_texture;
        }

        if (texture_to_draw.id == 0)
        {
            texture_to_draw = player->facing_direction == -1 ? player->flipleft_texture : player->texture;
        }
        else
        {
            // Make hurt texture slightly larger
            draw_scale = player->scale * 1.2f;

            // Adjust Y position so bottom remains on ground
            float original_height = (float)player->texture.height * player->scale;
            float hurt_height = (float)texture_to_draw.height * draw_scale;
            screen_pos.y -= (hurt_height - original_height);
        }
    }
    else if (player->is_burnt)
    {
        // Use on fire texture when burnt
        if (player->facing_direction == -1)
        {
            texture_to_draw = player->on_fire_flipleft_texture;
            if (player->on_fire_flipleft_texture.id == 0)
            {
                texture_to_draw = player->on_fire_texture;
            }
        }
        else
        {
            texture_to_draw = player->on_fire_texture;
        }

        if (texture_to_draw.id == 0)
        {
            texture_to_draw = player->facing_direction == -1 ? player->flipleft_texture : player->texture;
        }
        else
        {
            // Make on fire texture slightly larger
            draw_scale = player->scale * 1.8f;

            // Adjust Y position so feet remain on ground (not the top)
            float original_height = (float)player->texture.height * player->scale;
            float on_fire_height = (float)texture_to_draw.height * draw_scale;
            screen_pos.y -= (on_fire_height - original_height);
        }
    }
    else if (player->facing_direction == -1)
    {
        // Use flipped texture when facing left
        texture_to_draw = player->flipleft_texture;
        if (player->flipleft_texture.id == 0)
        {
            texture_to_draw = player->texture;
        }
    }

    // Use white color for all cases (textures already have appropriate colors)
    Color draw_color = WHITE;

    DrawTextureEx(
        texture_to_draw,
        screen_pos,
        0.0f,
        draw_scale,
        draw_color);

    // if the player is using sword, draw sword texture on top
    if (player->is_using_sword)
    {
        Vector2 sword_pos = screen_pos;
        Texture2D sword_texture_to_draw = player->sword_texture;
        float sword_scale = player->scale * 1.5;
        // Adjust sword position based on facing direction
        sword_pos.y += player->height * 0.3f; // Slightly above player's center
        if (player->facing_direction == 1)
        {
            sword_pos.x += player->width * 0.5f; // In front of player
        }
        else
        {
            sword_pos.x -= player->width * 0.5f; // In front of player
            sword_texture_to_draw = player->sword_flipleft_texture;
        }

        DrawTextureEx(
            sword_texture_to_draw,
            sword_pos,
            0.0f,
            player->scale,
            draw_color);

        // draw the sword hitbox for testing
        // Rectangle screen_sword_hitbox = player->sword_hitbox;
        // screen_sword_hitbox.x = player->sword_hitbox.x - camera_x + GetScreenWidth() / 2.0f;
        // DrawRectangleRec(
        //     screen_sword_hitbox,
        //     RED);
    }
}

void player_take_damage(Player *player, int damage)
{
    if (player->is_dead)
        return;

    player->hearts -= damage;

    // Check if player is dead
    if (player->hearts <= 0)
    {
        player->hearts = 0;
        player->is_dead = true;
    }
}

void player_set_hurt(Player *player)
{
    // Set hurt state for monster damage
    player->is_hurt = true;
    player->hurt_timer = 0.5f; // Show hurt texture for 0.5 seconds
}

void player_heal(Player *player, int amount)
{
    if (player->is_dead)
        return;

    player->hearts += amount;

    // Cap at max hearts
    if (player->hearts > player->max_hearts)
    {
        player->hearts = player->max_hearts;
    }
}

void player_cleanup(Player *player)
{
    UnloadTexture(player->texture);
    if (player->flipleft_texture.id > 0)
    {
        UnloadTexture(player->flipleft_texture);
    }
    if (player->hurt_texture.id > 0)
    {
        UnloadTexture(player->hurt_texture);
    }
    if (player->hurt_flipleft_texture.id > 0)
    {
        UnloadTexture(player->hurt_flipleft_texture);
    }
    if (player->on_fire_texture.id > 0)
    {
        UnloadTexture(player->on_fire_texture);
    }
    if (player->on_fire_flipleft_texture.id > 0)
    {
        UnloadTexture(player->on_fire_flipleft_texture);
    }
    if (player->dead_texture.id > 0)
    {
        UnloadTexture(player->dead_texture);
    }
    UnloadTexture(player->filled_heart_texture);
    UnloadTexture(player->empty_heart_texture);
    UnloadTexture(player->fireball_texture);
}
