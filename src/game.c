#include "game.h"
#include "player.h"
#include "background.h"
#include "level.h"
#include "level_definitions.h"
#include "hazard.h"
#include "monster.h"
#include "projectile.h"
#include "pickup.h"
#include "config.h"
#include "dragon.h"
#include "asset_paths.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static Player player;
static Background background;

// Helper function to draw all hearts UI with textures
static void draw_hearts_ui(Player *player, int screen_width, int screen_height)
{
    float heart_size = 32.0f; // Size for the heart texture
    float spacing = 40.0f;    // Spacing between hearts
    float start_x = screen_width - 50.0f;
    float start_y = screen_height - 30.0f;

    // Draw max hearts first (empty hearts)
    for (int i = 0; i < player->max_hearts; i++)
    {
        Rectangle source = {0, 0, (float)player->empty_heart_texture.width, (float)player->empty_heart_texture.height};
        Rectangle dest = {start_x - (i * spacing), start_y, heart_size, heart_size};
        DrawTexturePro(player->empty_heart_texture, source, dest, (Vector2){heart_size / 2.0f, heart_size / 2.0f}, 0.0f, WHITE);
    }

    // Draw current hearts on top (filled hearts)
    for (int i = 0; i < player->hearts; i++)
    {
        Rectangle source = {0, 0, (float)player->filled_heart_texture.width, (float)player->filled_heart_texture.height};
        Rectangle dest = {start_x - (i * spacing), start_y, heart_size, heart_size};
        DrawTexturePro(player->filled_heart_texture, source, dest, (Vector2){heart_size / 2.0f, heart_size / 2.0f}, 0.0f, WHITE);
    }
}

// Helper function to draw loot inventory UI on right side
static void draw_loot_inventory_ui(Player *player, int screen_width)
{
    float icon_height = 36.0f;
    float padding = 10.0f;
    float spacing = icon_height + 8.0f; // Space between icons
    float start_x = screen_width - padding - (icon_height * 2);
    float start_y = padding + icon_height / 2.0f;

    // Draw inventory items (LOOT_COIN, LOOT_HEALTH_POTION, PROTECTION_POTION, LOOT_FIREBALL)
    for (int i = 0; i < LOOT_TYPE_COUNT; i++)
    {
        int count = player->inventory.counts[i];

        // Skip if no items and no texture loaded
        if (count == 0 && player->inventory.loot_textures[i].id == 0)
            continue;

        // Skip if texture is not loaded
        if (player->inventory.loot_textures[i].id == 0)
            continue;

        Texture2D texture = player->inventory.loot_textures[i];

        // Calculate width based on texture aspect ratio
        float aspect_ratio = (float)texture.width / (float)texture.height;
        float icon_width = icon_height * aspect_ratio;

        // Position for this item (vertical stack)
        float item_x = start_x + icon_width / 2.0f;
        float item_y = start_y + (i * spacing);

        // Draw icon background (semi-transparent dark)
        DrawRectangle((int)(item_x - icon_width / 2.0f - 2.0f),
                      (int)(item_y - icon_height / 2.0f - 2.0f),
                      (int)(icon_width + 4.0f),
                      (int)(icon_height + 4.0f),
                      (Color){0, 0, 0, 100});

        // Draw item icon
        Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
        Rectangle dest = {item_x, item_y, icon_width, icon_height};
        DrawTexturePro(texture, source, dest, (Vector2){icon_width / 2.0f, icon_height / 2.0f}, 0.0f, WHITE);

        // Draw count if > 0
        if (count > 0)
        {
            const char *count_text = TextFormat("%d", count);
            int text_width = MeasureText(count_text, 16);
            float text_x = item_x + icon_width / 2.0f - text_width / 2.0f - 2.0f;
            float text_y = item_y + icon_height / 2.0f - 8.0f;

            // Draw text with outline for better readability
            DrawText(count_text, (int)text_x - 1, (int)text_y, 16, BLACK);
            DrawText(count_text, (int)text_x + 1, (int)text_y, 16, BLACK);
            DrawText(count_text, (int)text_x, (int)text_y - 1, 16, BLACK);
            DrawText(count_text, (int)text_x, (int)text_y + 1, 16, BLACK);
            DrawText(count_text, (int)text_x, (int)text_y, 16, YELLOW);
        }
    }
}

// Helper function to draw level info
static void draw_level_ui(GameState *state)
{
    Level *current_level = &state->levels[state->current_level_index];
    DrawText(TextFormat("Level: %d - %s", current_level->level_number, current_level->name), 10, 40, 20, BLACK);

    // Draw goal info based on goal type
    if (current_level->goal.type == GOAL_TYPE_LOCATION)
    {
        DrawText("Goal: Reach the goal location", 10, 65, 16, BLACK);
    }
    else if (current_level->goal.type == GOAL_TYPE_HAZARDS)
    {
        DrawText(TextFormat("Goal: Defeat %d hazards (%d/%d)",
                            current_level->goal.hazards_to_defeat,
                            current_level->goal.hazards_defeated,
                            current_level->goal.hazards_to_defeat),
                 10, 65, 16, BLACK);
    }
    else if (current_level->goal.type == GOAL_TYPE_MONSTERS)
    {
        DrawText(TextFormat("Goal: Defeat %d monsters (%d/%d)",
                            current_level->goal.monsters_to_defeat,
                            current_level->goal.monsters_defeated,
                            current_level->goal.monsters_to_defeat),
                 10, 65, 16, BLACK);
    }
}

// Helper function to draw goal marker (door-like rectangle)
static void draw_goal_marker(Level *level, float camera_x)
{
    if (level->goal.type == GOAL_TYPE_LOCATION)
    {
        // Convert world position to screen position
        float screen_x = level->goal.goal_position.x - camera_x + GetScreenWidth() / 2.0f;
        float screen_y = level->goal.goal_position.y;

        // Draw door-like rectangular marker
        float door_width = 40.0f;
        float door_height = 80.0f;
        float y_offset = -60.0f; // Offset to position door above ground

        // Draw door frame (red color)
        DrawRectangle((int)(screen_x - door_width / 2), (int)(screen_y - door_height - y_offset),
                      (int)door_width, (int)door_height, RED);

        // Draw door outline (darker red)
        DrawRectangleLines((int)(screen_x - door_width / 2), (int)(screen_y - door_height - y_offset),
                           (int)door_width, (int)door_height, RED);

        // Draw door handle (small circle)
        DrawCircle((int)(screen_x + door_width / 4), (int)(screen_y - door_height / 2 - y_offset), 3.0f, YELLOW);
    }
}

// Initialize levels for the game
static void initialize_levels(GameState *state)
{
    state->level_count = 20; // Total number of levels

    // Load levels from level definition files
    state->levels[0] = level1_create();
    state->levels[1] = level2_create();
    state->levels[2] = level3_create();
    state->levels[3] = level4_create();
    state->levels[4] = level5_create();
    state->levels[5] = level6_create();
    state->levels[6] = level7_create();
    state->levels[7] = level8_create();
    state->levels[8] = level9_create();
    state->levels[9] = level10_create();
    state->levels[10] = level11_create();
    state->levels[11] = level12_create();
    state->levels[12] = level13_create();
    state->levels[13] = level14_create();
    state->levels[14] = level15_create();
    state->levels[15] = level16_create();
    state->levels[16] = level17_create();
    state->levels[17] = level18_create();
    state->levels[18] = level19_create();
    state->levels[19] = level20_create();

    state->current_level_index = 0;
}

// Helper function to initialize the loot system with default loot table
static void init_loot_system(GameState *state)
{
    state->loot_system = loot_system_create();

    // Create a default loot table for monsters
    LootTable default_table = loot_table_create("DEFAULT", 4);

    // Add loot items with drop chances and values
    // Coins: 80% chance to drop 1 coin per monster kill
    LootItemDef coin_def = {.type = LOOT_COIN, .drop_chance = 0.8f, .value = 1, .scale = 0.03f};
    loot_table_add_item(&default_table, coin_def);

    // Health Potions: 30% chance to drop 1 health potion
    LootItemDef health_potion_def = {.type = LOOT_HEALTH_POTION, .drop_chance = 0.3f, .value = 1, .scale = 0.05f};
    loot_table_add_item(&default_table, health_potion_def);

    // Protection Potions: 15% chance to drop 1 protection potion
    LootItemDef protection_potion_def = {.type = PROTECTION_POTION, .drop_chance = 0.15f, .value = 1, .scale = 0.04f};
    loot_table_add_item(&default_table, protection_potion_def);

    // Fireballs: 40% chance to drop 1 fireball
    LootItemDef fireball_def = {.type = LOOT_FIREBALL, .drop_chance = 0.4f, .value = 1, .scale = 0.04f};
    loot_table_add_item(&default_table, fireball_def);

    // Set this as the default loot table for all monsters
    loot_system_set_default_table(&state->loot_system, default_table);
}

void game_init(GameState *state)
{
    state->screen_width = 1280;
    state->screen_height = 720;
    state->fps = 60;
    state->running = true;
    state->burnt_message_timer = 0.0f;
    state->is_paused = false;
    state->hazard_cooldown = 0.0f;
    state->sword_attack_cooldown = 0.0f;
    state->game_over = false;
    state->last_collision_type = COLLISION_TYPE_NONE;
    state->projectiles = projectile_list_create(100); // Max 100 projectiles active at once
    state->in_level_transition = false;
    state->next_level_index = 0;
    state->game_victory = false;
    state->victory_timer = 0.0f;
    state->elapsed_time = 0.0f;
    state->current_screen = GAME_SCREEN_TITLE;
    state->selected_menu_item = 1; // Start on "Start Game" menu item
    state->selected_level = 0;     // Default to Level 1 (index 0)
    state->pause_menu_active = false;
    state->pause_menu_selection = 0;
    state->music_volume = 0.5f; // Default volume to 50%
    state->options_menu_active = false;
    state->options_menu_selection = 0;
    state->previous_screen = GAME_SCREEN_TITLE;

    InitWindow(state->screen_width, state->screen_height, "Knight To Victory");
    InitAudioDevice();
    SetTargetFPS(state->fps);
    SetExitKey(KEY_NULL); // Disable default ESC-to-close behavior so we can handle ESC for pause menu

    // Load menu cursor texture (character.png)
    state->menu_cursor_texture = LoadTexture(get_asset_path("character.png"));

    // Initialize levels
    initialize_levels(state);

    // Initialize loot system
    init_loot_system(state);

    // Initialize game objects
    Level *current_level = &state->levels[state->current_level_index];
    player = player_create(current_level->player_start_position.x, current_level->player_start_position.y);
    background = background_create_with_variant(current_level->background.variant);
}

void game_update(GameState *state)
{
    state->delta_time = GetFrameTime();

    // Handle options menu first (before screen-specific handling)
    if (state->options_menu_active)
    {
        // Volume control with LEFT/RIGHT arrow keys
        if (IsKeyPressed(KEY_LEFT))
        {
            state->music_volume = state->music_volume - 0.1f;
            if (state->music_volume < 0.0f)
                state->music_volume = 0.0f;
            SetMusicVolume(state->background_music, state->music_volume);
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            state->music_volume = state->music_volume + 0.1f;
            if (state->music_volume > 1.0f)
                state->music_volume = 1.0f;
            SetMusicVolume(state->background_music, state->music_volume);
        }

        // Menu navigation
        if (IsKeyPressed(KEY_UP))
        {
            state->options_menu_selection = (state->options_menu_selection - 1 + 2) % 2;
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            state->options_menu_selection = (state->options_menu_selection + 1) % 2;
        }

        // Back option
        if (IsKeyPressed(KEY_ENTER) && state->options_menu_selection == 1)
        {
            state->options_menu_active = false;
        }

        // ESC to go back
        if (IsKeyPressed(KEY_ESCAPE))
        {
            state->options_menu_active = false;
        }

        return;
    }

    // Handle title screen
    if (state->current_screen == GAME_SCREEN_TITLE)
    {
        // Update background animation even on title screen
        background_update(&background, (Vector2){0, 0});

        // Menu navigation
        if (IsKeyPressed(KEY_UP))
        {
            state->selected_menu_item = (state->selected_menu_item - 1 + 4) % 4;
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            state->selected_menu_item = (state->selected_menu_item + 1) % 4;
        }

        // Level selection with LEFT/RIGHT when on level selector
        if (state->selected_menu_item == 0)
        {
            if (IsKeyPressed(KEY_LEFT))
            {
                state->selected_level = (state->selected_level - 1 + state->level_count) % state->level_count;
            }
            else if (IsKeyPressed(KEY_RIGHT))
            {
                state->selected_level = (state->selected_level + 1) % state->level_count;
            }
        }

        // Menu selection
        if (IsKeyPressed(KEY_ENTER))
        {
            if (state->selected_menu_item == 1)
            {
                // Start Game - use selected_level instead of 0
                state->current_screen = GAME_SCREEN_PLAYING;
                state->current_level_index = state->selected_level;
                state->game_over = false;
                state->game_victory = false;
                state->elapsed_time = 0.0f;
                state->is_paused = false;

                // Reset player and level
                Level *level = &state->levels[state->selected_level];
                player.position = level->player_start_position;
                player.velocity = (Vector2){0, 0};
                player.hearts = player.max_hearts;
                player.is_dead = false;
                player_clear_damage_type(&player);
                player.projectile_inventory = 0;

                // Reactivate all enemies in all levels
                for (int i = 0; i < state->level_count; i++)
                {
                    level_reactivate_enemies(&state->levels[i]);
                    level_reset(&state->levels[i]);
                }

                // Reinitialize background
                background_cleanup(&background);
                background = background_create_with_variant(level->background.variant);
            }
            else if (state->selected_menu_item == 2)
            {
                // Options
                state->options_menu_active = true;
                state->previous_screen = GAME_SCREEN_TITLE;
                state->options_menu_selection = 0;
            }
            else if (state->selected_menu_item == 3)
            {
                // Exit Game
                state->running = false;
            }
        }
        return;
    }

    // Increment elapsed time (always, even when paused, to track total play time)
    if (!state->game_over && !state->game_victory)
    {
        state->elapsed_time += state->delta_time;
    }

    Level *current_level = &state->levels[state->current_level_index];

    // Handle level transition screen
    if (state->in_level_transition)
    {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
        {
            // Transition to next level
            state->current_level_index = state->next_level_index;
            state->in_level_transition = false;
            state->is_paused = false;

            Level *next_level = &state->levels[state->current_level_index];

            // Reset player position to new level's start
            player.position = next_level->player_start_position;
            player.velocity = (Vector2){0, 0};
            player.hearts = player.max_hearts; // Full health for new level
            player.is_dead = false;            // Reset dead flag so damage can be taken
            player_clear_damage_type(&player); // Reset any active damage effects
            level_reset(next_level);

            // Reinitialize background with new level's variant
            background_cleanup(&background);
            background = background_create_with_variant(next_level->background.variant);

            // Update current_level reference
            current_level = &state->levels[state->current_level_index];
        }
        return; // Don't process other updates during transition
    }

    // Update all hazards (only if not paused and pause menu is not active)
    if (!state->is_paused && !state->pause_menu_active)
    {
        for (int i = 0; i < current_level->hazards.count; i++)
        {
            hazard_update(&current_level->hazards.hazards[i]);
        }
    }

    // Update game objects (only if not paused and pause menu is not active)
    if (!state->is_paused && !state->pause_menu_active)
    {
        player_handle_input(&player);
        player_update_with_hazards(&player, &current_level->hazards);
        player_update_sword_hitbox(&player);
        background_update(&background, player.position);

        // Update all monsters
        for (int i = 0; i < current_level->monsters.count; i++)
        {
            monster_update(&current_level->monsters.monsters[i]);
        }

        // Update dragon AI - make dragons fire at the player
        for (int i = 0; i < current_level->monsters.count; i++)
        {
            Monster *monster = &current_level->monsters.monsters[i];
            // Check if this is a dragon (has custom_data allocated for dragon behavior)
            if (monster->active && monster->custom_data != NULL && monster->custom_update == dragon_custom_update)
            {
                dragon_fire_at_target(monster, &state->projectiles, player.position);
            }
        }

        // Handle projectile firing on left mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player.projectile_inventory > 0)
        {
            // Get mouse position in world coordinates
            Vector2 mouse_pos = GetMousePosition();
            float world_mouse_x = mouse_pos.x - GetScreenWidth() / 2.0f + background.camera.target.x;

            // Create and fire a fireball projectile from player to mouse position
            Projectile fireball = projectile_create_fireball(
                (Vector2){player.position.x + player.width / 2.0f, player.position.y},
                (Vector2){world_mouse_x, mouse_pos.y},
                PROJECTILE_SOURCE_PLAYER);
            projectile_list_add(&state->projectiles, fireball);
            player.projectile_inventory--;            // Consume one projectile
            player.inventory.counts[LOOT_FIREBALL]--; // Update inventory display
        }

        // Update all projectiles
        for (int i = 0; i < state->projectiles.count; i++)
        {
            if (state->projectiles.projectiles[i].active)
            {
                projectile_update(&state->projectiles.projectiles[i]);
            }
        }

        // Update all pickups
        for (int i = 0; i < current_level->pickups.count; i++)
        {
            if (current_level->pickups.pickups[i].active)
            {
                pickup_update(&current_level->pickups.pickups[i]);
            }
        }

        // Update loot items
        loot_list_update(&current_level->loot);

        // Update all spawners in the level (spawn new pickups on a timer)
        for (int i = 0; i < current_level->spawners.count; i++)
        {
            pickup_spawner_update(&current_level->spawners.spawners[i], &current_level->pickups);
        }
    }
    // Check for hazard collisions (only if cooldown expired)
    Rectangle player_rect = {
        player.position.x,
        player.position.y,
        player.width,
        player.height};

    if (state->hazard_cooldown <= 0.0f)
    {
        for (int i = 0; i < current_level->hazards.count; i++)
        {
            Hazard *hazard = &current_level->hazards.hazards[i];
            if (hazard->active && hazard_check_collision(hazard, player_rect) && hazard_is_dangerous(hazard))
            {
                // Only process collision if protection potion is not active
                if (!player.protection_potion_active)
                {
                    // Player hit a hazard (only if it's dangerous/not faded out)
                    player_take_damage(&player, hazard->damage);

                    // Apply damage type based on hazard type
                    DamageType damage_type = DAMAGE_TYPE_FIRE; // Default to fire
                    float damage_duration = DAMAGE_DISPLAY_FIRE;

                    switch (hazard->type)
                    {
                    case HAZARD_LAVA_PIT:
                        damage_type = DAMAGE_TYPE_FIRE;
                        damage_duration = DAMAGE_DISPLAY_FIRE;
                        break;
                    case HAZARD_DUST_STORM:
                        damage_type = DAMAGE_TYPE_DUST;
                        damage_duration = DAMAGE_DISPLAY_DUST;
                        break;
                    case HAZARD_SPIKE_TRAP:
                        damage_type = DAMAGE_TYPE_MONSTER_HIT; // Spikes feel like sharp impacts
                        damage_duration = DAMAGE_DISPLAY_MONSTER_HIT;
                        break;
                    case HAZARD_LAVA_JET:
                        damage_type = DAMAGE_TYPE_FIRE;
                        damage_duration = DAMAGE_DISPLAY_FIRE;
                        break;
                    case HAZARD_WIND_DAGGERS:
                        damage_type = DAMAGE_TYPE_MONSTER_HIT;
                        damage_duration = DAMAGE_DISPLAY_MONSTER_HIT;
                        break;
                    }

                    player_apply_damage_type(&player, damage_type, damage_duration);
                    state->burnt_message_timer = PAUSE_DURATION;        // Display message
                    state->is_paused = true;                            // Pause the game
                    state->hazard_cooldown = 3.0f;                      // Cooldown to prevent re-collision
                    state->last_collision_type = COLLISION_TYPE_HAZARD; // Track collision type
                    // Check if player is out of hearts
                    if (player.hearts <= 0)
                    {
                        state->game_over = true; // Trigger game over
                    }
                }
                else
                {
                    // Protection potion is active, consume it and set cooldown to prevent re-collision
                    player_take_damage(&player, hazard->damage);
                    state->hazard_cooldown = 3.0f; // 3 second cooldown to escape
                }
            }
        }
    }

    // Check for monster collisions (also protected by cooldown)
    if (state->hazard_cooldown <= 0.0f)
    {
        for (int i = 0; i < current_level->monsters.count; i++)
        {
            Monster *monster = &current_level->monsters.monsters[i];
            if (state->sword_attack_cooldown <= 0.0f && monster->active && player.is_using_sword)
            {
                Rectangle monster_rect = {
                    monster->position.x,
                    monster->position.y,
                    monster->width,
                    monster->height};

                if (CheckCollisionRecs(player.sword_hitbox, monster_rect))
                {
                    // Player hit monster with sword
                    bool was_alive = monster->active;
                    monster_take_damage(monster, 1);     // Sword deals 1 damage
                    state->sword_attack_cooldown = 1.0f; // Set cooldown after attack

                    // Generate loot if monster died
                    if (was_alive && !monster->active)
                    {
                        // Generate loot drops
                        LootTable *loot_table = loot_system_get_table_or_default(&state->loot_system, monster->type);
                        LootList drops = generate_loot_drops(monster->position, loot_table, &player.inventory);
                        for (int l = 0; l < drops.count; l++)
                        {
                            loot_list_add(&current_level->loot, drops.loot[l]);
                        }
                        loot_list_cleanup(&drops);
                    }

                    // Track defeated monsters for goal
                    if (was_alive && !monster->active && current_level->goal.type == GOAL_TYPE_MONSTERS)
                    {
                        current_level->goal.monsters_defeated++;
                    }
                }
            }

            Rectangle player_rect = {
                player.position.x,
                player.position.y,
                player.width,
                player.height};

            Rectangle monster_rect = {
                monster->position.x,
                monster->position.y,
                monster->width,
                monster->height};

            if (monster->active && CheckCollisionRecs(player_rect, monster_rect))
            {
                // Only process collision if protection potion is not active
                if (!player.protection_potion_active)
                {
                    // Player hit a monster
                    player_take_damage(&player, 1); // Monsters deal 1 damage
                    player_apply_damage_type(&player, DAMAGE_TYPE_MONSTER_HIT, DAMAGE_DISPLAY_MONSTER_HIT);

                    // Pause the game and set cooldown
                    state->burnt_message_timer = PAUSE_DURATION;         // Control pause duration
                    state->is_paused = true;                             // Pause the game
                    state->hazard_cooldown = 3.0f;                       // Cooldown to prevent re-collision
                    state->last_collision_type = COLLISION_TYPE_MONSTER; // Track collision type

                    // Check if player is out of hearts
                    if (player.hearts <= 0)
                    {
                        state->game_over = true; // Trigger game over
                    }
                }
                else
                {
                    // Protection potion is active, consume it and set cooldown to prevent re-collision
                    player_take_damage(&player, 1);
                    state->hazard_cooldown = 3.0f; // 3 second cooldown to escape
                }
            }
        }
    }

    // Check for pickup collisions
    for (int i = 0; i < current_level->pickups.count; i++)
    {
        Pickup *pickup = &current_level->pickups.pickups[i];
        if (pickup->active)
        {
            Rectangle pickup_rect = {
                pickup->position.x - (pickup->width * pickup->scale) / 2.0f,
                pickup->position.y - (pickup->height * pickup->scale) / 2.0f,
                pickup->width * pickup->scale,
                pickup->height * pickup->scale};

            if (CheckCollisionRecs(player_rect, pickup_rect))
            {
                // Player picked up fireball
                if (pickup->type == PICKUP_FIREBALL)
                {
                    player.inventory.counts[LOOT_FIREBALL] += pickup->value;
                    player.projectile_inventory += pickup->value;

                    // Cap at max
                    if (player.inventory.counts[LOOT_FIREBALL] > player.max_projectiles)
                    {
                        player.inventory.counts[LOOT_FIREBALL] = player.max_projectiles;
                    }
                    if (player.projectile_inventory > player.max_projectiles)
                    {
                        player.projectile_inventory = player.max_projectiles;
                    }
                }

                // Deactivate pickup
                pickup->active = false;
            }
        }
    }

    // Check for loot-player collisions
    for (int i = 0; i < current_level->loot.count; i++)
    {
        Loot *loot = &current_level->loot.loot[i];
        if (loot->active)
        {
            Rectangle loot_rect = {
                loot->position.x - (loot->texture.width * loot->scale) / 2.0f,
                loot->position.y - (loot->texture.height * loot->scale) / 2.0f,
                loot->texture.width * loot->scale,
                loot->texture.height * loot->scale};

            if (CheckCollisionRecs(player_rect, loot_rect))
            {
                // Player picked up loot - add to inventory
                inventory_add_loot(&player.inventory, loot->type, loot->value);

                // Handle special case for fireballs - also add to projectile inventory
                if (loot->type == LOOT_FIREBALL)
                {
                    player.projectile_inventory += loot->value;

                    // Cap at max
                    if (player.inventory.counts[LOOT_FIREBALL] > player.max_projectiles)
                    {
                        player.inventory.counts[LOOT_FIREBALL] = player.max_projectiles;
                    }
                    if (player.projectile_inventory > player.max_projectiles)
                    {
                        player.projectile_inventory = player.max_projectiles;
                    }
                }

                // Deactivate loot
                loot->active = false;
            }
        }
    }

    // Check for projectile-monster collisions (only player projectiles hit monsters)
    for (int p = 0; p < state->projectiles.count; p++)
    {
        Projectile *projectile = &state->projectiles.projectiles[p];
        if (!projectile->active || projectile->source != PROJECTILE_SOURCE_PLAYER)
            continue;

        Rectangle projectile_rect = {
            projectile->position.x,
            projectile->position.y,
            projectile->width,
            projectile->height};

        for (int m = 0; m < current_level->monsters.count; m++)
        {
            Monster *monster = &current_level->monsters.monsters[m];
            if (!monster->active)
                continue;

            Rectangle monster_rect = {
                monster->position.x,
                monster->position.y,
                monster->width,
                monster->height};

            if (CheckCollisionRecs(projectile_rect, monster_rect))
            {
                // Projectile hit monster
                bool was_alive = monster->active;
                monster_take_damage(monster, 1); // Each projectile deals 1 damage
                projectile->active = false;      // Destroy projectile on impact

                // Generate loot if monster died
                if (was_alive && !monster->active)
                {
                    // Generate loot drops
                    LootTable *loot_table = loot_system_get_table_or_default(&state->loot_system, monster->type);
                    LootList drops = generate_loot_drops(monster->position, loot_table, &player.inventory);
                    for (int l = 0; l < drops.count; l++)
                    {
                        loot_list_add(&current_level->loot, drops.loot[l]);
                    }
                    loot_list_cleanup(&drops);
                }

                // Track defeated monsters for goal
                if (was_alive && !monster->active && current_level->goal.type == GOAL_TYPE_MONSTERS)
                {
                    current_level->goal.monsters_defeated++;
                }
            }
        }
    }

    // Check for monster projectile-player collisions
    for (int p = 0; p < state->projectiles.count; p++)
    {
        Projectile *projectile = &state->projectiles.projectiles[p];

        if (projectile_check_player_collision(projectile, player_rect))
        {
            // Only process collision if protection potion is not active
            if (!player.protection_potion_active)
            {
                // Monster projectile hit player
                player_take_damage(&player, 1); // Each projectile deals 1 damage

                // Apply fire damage type (projectiles are fire-based)
                player_apply_damage_type(&player, DAMAGE_TYPE_FIRE, DAMAGE_DISPLAY_FIRE);
                state->burnt_message_timer = PAUSE_DURATION;        // Display message
                state->is_paused = true;                            // Pause the game
                state->hazard_cooldown = 3.0f;                      // Cooldown to prevent re-collision
                state->last_collision_type = COLLISION_TYPE_HAZARD; // Track collision type

                // Check if player is out of hearts
                if (player.hearts <= 0)
                {
                    state->game_over = true; // Trigger game over
                }
            }
            else
            {
                // Protection potion is active, consume it and prevent collision sequence
                player_take_damage(&player, 1);
            }

            projectile->active = false; // Destroy projectile on impact
        }
    }

    // Update hazard cooldown (skip if pause menu is active)
    if (state->hazard_cooldown > 0.0f && !state->pause_menu_active)
    {
        state->hazard_cooldown -= state->delta_time;
    }

    // update sword attack cooldown (skip if pause menu is active)
    if (state->sword_attack_cooldown > 0.0f && !state->pause_menu_active)
    {
        state->sword_attack_cooldown -= state->delta_time;
    }

    // Update burnt message timer (skip if pause menu is active)
    if (state->burnt_message_timer > 0.0f && !state->pause_menu_active)
    {
        state->burnt_message_timer -= state->delta_time;
    }
    else if (state->is_paused && !state->pause_menu_active)
    {
        // Timer expired, resume game and respawn
        state->is_paused = false;

        // Respawn player at level start
        player.position = current_level->player_start_position;
        player.velocity = (Vector2){0, 0};
        player_clear_damage_type(&player); // Clear any active damage effects on respawn
    }

    // Update game over timer
    if (state->game_over)
    {
        // When burnt message timer expires and game is over, return to title screen
        if (state->burnt_message_timer <= 0.0f)
        {
            // Return to title screen
            state->current_screen = GAME_SCREEN_TITLE;
            state->game_over = false;
            state->selected_menu_item = 1; // Default to "Start Game"
            state->selected_level = 0;     // Reset to level 1
            state->hazard_cooldown = 0.0f;
            player.hearts = player.max_hearts;
            player.is_dead = false;
            player_clear_damage_type(&player);
            player.projectile_inventory = 0;

            // Reactivate all enemies for next playthrough
            for (int i = 0; i < state->level_count; i++)
            {
                level_reactivate_enemies(&state->levels[i]);
                level_reset(&state->levels[i]);
            }
        }
    }

    // Check if level goal is reached
    if (level_check_goal_reached(current_level, player.position))
    {
        // Move to next level
        if (state->current_level_index < state->level_count - 1)
        {
            // Start level transition screen
            state->in_level_transition = true;
            state->next_level_index = state->current_level_index + 1;
            state->is_paused = true; // Pause game during transition
        }
        else
        {
            // Game complete - show victory screen
            state->game_victory = true;
            state->victory_timer = 0.0f;
            state->is_paused = true;
        }
    }

    // Handle victory state
    if (state->game_victory)
    {
        state->victory_timer += state->delta_time;
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE))
        {
            // Return to title screen after victory
            state->current_screen = GAME_SCREEN_TITLE;
            state->game_victory = false;
            state->selected_menu_item = 1; // Default to "Start Game"
            state->selected_level = 0;     // Reset to level 1
            state->current_level_index = 0;
            state->victory_timer = 0.0f;
            state->is_paused = false;

            // Reactivate all enemies for next playthrough
            for (int i = 0; i < state->level_count; i++)
            {
                level_reactivate_enemies(&state->levels[i]);
                level_reset(&state->levels[i]);
            }
        }
    }

    // Check for exit
    if (IsKeyPressed(KEY_ESCAPE) && state->current_screen != GAME_SCREEN_TITLE && !state->game_victory)
    {
        if (state->current_screen == GAME_SCREEN_PLAYING)
        {
            // Toggle pause menu during gameplay
            state->pause_menu_active = !state->pause_menu_active;
            state->pause_menu_selection = 0; // Reset to "Resume Game" when opening
        }
    }

    // Handle pause menu navigation
    if (state->pause_menu_active && state->current_screen == GAME_SCREEN_PLAYING)
    {
        if (IsKeyPressed(KEY_UP))
        {
            state->pause_menu_selection = (state->pause_menu_selection - 1 + 3) % 3;
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            state->pause_menu_selection = (state->pause_menu_selection + 1) % 3;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            if (state->pause_menu_selection == 0)
            {
                // Resume Game
                state->pause_menu_active = false;
            }
            else if (state->pause_menu_selection == 1)
            {
                // Options
                state->options_menu_active = true;
                state->previous_screen = GAME_SCREEN_PLAYING;
                state->options_menu_selection = 0;
            }
            else if (state->pause_menu_selection == 2)
            {
                // Quit To Menu - reset all levels and return to title
                state->current_screen = GAME_SCREEN_TITLE;
                state->pause_menu_active = false;
                state->selected_menu_item = 1; // Default to "Start Game"
                state->selected_level = 0;     // Reset to level 1
                state->current_level_index = 0;
                state->game_over = false;
                state->is_paused = false;

                // Reactivate all enemies for next playthrough
                for (int i = 0; i < state->level_count; i++)
                {
                    level_reactivate_enemies(&state->levels[i]);
                    level_reset(&state->levels[i]);
                }
            }
        }
    }
}

// Helper function to draw victory screen with fireworks
static void draw_victory_screen(GameState *state)
{
    int screen_width = state->screen_width;
    int screen_height = state->screen_height;

    // Draw background
    DrawRectangle(0, 0, screen_width, screen_height, BLACK);

    // Draw fireworks (random colored circles)
    srand((unsigned int)(state->victory_timer * 1000)); // Use timer for pseudo-random animation
    for (int i = 0; i < 50; i++)
    {
        float x = (rand() % screen_width);
        float y = (rand() % screen_height);
        float size = 5.0f + (rand() % 15);

        // Create pulsing effect
        float pulse = sinf(state->victory_timer * 3.0f + i) * 0.5f + 0.5f;
        size *= pulse;

        Color firework_colors[] = {RED, YELLOW, GREEN, BLUE, MAGENTA, ORANGE, SKYBLUE};
        Color color = firework_colors[rand() % 7];
        color.a = (unsigned char)(pulse * 255);

        DrawCircle((int)x, (int)y, size, color);
    }

    // Draw victory text
    const char *victory_text = "You have beat the game!";
    int text_width = MeasureText(victory_text, 80);
    int text_x = (screen_width - text_width) / 2;
    int text_y = screen_height / 2 - 100;

    // Draw text with outline effect
    DrawText(victory_text, text_x - 2, text_y - 2, 80, BLACK);
    DrawText(victory_text, text_x + 2, text_y + 2, 80, BLACK);
    DrawText(victory_text, text_x, text_y, 80, GOLD);

    // Draw time taken
    const char *time_text = TextFormat("Time: %.1f seconds", state->elapsed_time);
    int time_width = MeasureText(time_text, 40);
    int time_x = (screen_width - time_width) / 2;
    int time_y = screen_height / 2 + 20;

    DrawText(time_text, time_x - 1, time_y - 1, 40, BLACK);
    DrawText(time_text, time_x + 1, time_y + 1, 40, BLACK);
    DrawText(time_text, time_x, time_y, 40, YELLOW);

    // Draw continue text
    const char *continue_text = "Press SPACE or ENTER to exit";
    int continue_width = MeasureText(continue_text, 24);
    int continue_x = (screen_width - continue_width) / 2;
    int continue_y = screen_height - 60;

    // Blinking effect for continue text
    if ((int)(state->victory_timer * 2) % 2 == 0)
    {
        DrawText(continue_text, continue_x, continue_y, 24, WHITE);
    }
}

// Helper function to draw level transition screen
static void draw_level_transition_screen(GameState *state)
{
    int screen_width = state->screen_width;
    int screen_height = state->screen_height;

    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 180});

    // Draw transition text
    int current_level_num = state->current_level_index + 1;
    int next_level_num = state->next_level_index + 1;

    const char *level_complete_text = TextFormat("Level %d Complete!", current_level_num);
    const char *next_level_text = TextFormat("Onwards to Level %d", next_level_num);
    const char *continue_text = "Press SPACE or ENTER to continue...";

    // Get goal text for next level
    Level *next_level = &state->levels[state->next_level_index];
    const char *goal_text = "";

    if (next_level->goal.type == GOAL_TYPE_LOCATION)
    {
        goal_text = "Goal: Reach the goal location";
    }
    else if (next_level->goal.type == GOAL_TYPE_HAZARDS)
    {
        goal_text = TextFormat("Goal: Defeat %d hazards", next_level->goal.hazards_to_defeat);
    }
    else if (next_level->goal.type == GOAL_TYPE_MONSTERS)
    {
        goal_text = TextFormat("Goal: Defeat %d monsters", next_level->goal.monsters_to_defeat);
    }

    int complete_width = MeasureText(level_complete_text, 60);
    int next_width = MeasureText(next_level_text, 40);
    int goal_width = MeasureText(goal_text, 24);
    int continue_width = MeasureText(continue_text, 20);

    int complete_x = (screen_width - complete_width) / 2;
    int next_x = (screen_width - next_width) / 2;
    int goal_x = (screen_width - goal_width) / 2;
    int continue_x = (screen_width - continue_width) / 2;

    DrawText(level_complete_text, complete_x, screen_height / 2 - 100, 60, GOLD);
    DrawText(next_level_text, next_x, screen_height / 2 - 20, 40, WHITE);
    DrawText(goal_text, goal_x, screen_height / 2 + 35, 24, SKYBLUE);
    DrawText(continue_text, continue_x, screen_height / 2 + 90, 20, LIGHTGRAY);
}

// Helper function to draw title/menu screen
static void draw_title_screen(GameState *state)
{
    int screen_width = state->screen_width;
    int screen_height = state->screen_height;

    // Draw procedural background (same as gameplay)
    background_draw(&background);

    // Draw semi-transparent overlay for better text visibility
    DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 120});

    // Draw title
    const char *title_text = "Knight To Victory";
    int title_width = MeasureText(title_text, 80);
    int title_x = (screen_width - title_width) / 2;
    int title_y = screen_height / 2 - 150;

    // Draw title with shadow effect
    DrawText(title_text, title_x - 2, title_y - 2, 80, BLACK);
    DrawText(title_text, title_x + 2, title_y + 2, 80, BLACK);
    DrawText(title_text, title_x, title_y, 80, GOLD);

    // Menu items
    float menu_start_y = screen_height / 2 + 20;
    float menu_item_height = 70.0f;
    float character_offset_x = -200.0f; // Offset to the left of menu items

    // Draw menu items
    // Item 0: Select Level
    {
        float item_y = menu_start_y;
        char level_text[50];
        snprintf(level_text, sizeof(level_text), "Select Level: %d", state->selected_level + 1);
        int text_width = MeasureText(level_text, 40);
        int text_x = (screen_width - text_width) / 2;

        Color text_color = (0 == state->selected_menu_item) ? YELLOW : WHITE;
        DrawText(level_text, text_x, (int)item_y, 40, text_color);

        // Draw character cursor for selected item
        if (0 == state->selected_menu_item && state->menu_cursor_texture.width > 0)
        {
            float cursor_x = text_x + character_offset_x;
            float cursor_y = item_y;
            float cursor_size = 40.0f;

            Rectangle source = {0, 0, (float)state->menu_cursor_texture.width, (float)state->menu_cursor_texture.height};
            Rectangle dest = {cursor_x, cursor_y, cursor_size, cursor_size};
            DrawTexturePro(state->menu_cursor_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }

        // Draw left/right navigation hint for level selector
        if (0 == state->selected_menu_item)
        {
            const char *nav_hint = "(LEFT/RIGHT to change level)";
            int hint_width = MeasureText(nav_hint, 16);
            int hint_x = (screen_width - hint_width) / 2;
            DrawText(nav_hint, hint_x, (int)(item_y + 45), 16, LIGHTGRAY);
        }
    }

    // Item 1: Start Game
    {
        float item_y = menu_start_y + menu_item_height;
        const char *menu_text = "Start Game";
        int text_width = MeasureText(menu_text, 40);
        int text_x = (screen_width - text_width) / 2;

        Color text_color = (1 == state->selected_menu_item) ? YELLOW : WHITE;
        DrawText(menu_text, text_x, (int)item_y, 40, text_color);

        // Draw character cursor for selected item
        if (1 == state->selected_menu_item && state->menu_cursor_texture.width > 0)
        {
            float cursor_x = text_x + character_offset_x;
            float cursor_y = item_y;
            float cursor_size = 40.0f;

            Rectangle source = {0, 0, (float)state->menu_cursor_texture.width, (float)state->menu_cursor_texture.height};
            Rectangle dest = {cursor_x, cursor_y, cursor_size, cursor_size};
            DrawTexturePro(state->menu_cursor_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }

    // Item 2: Settings
    {
        float item_y = menu_start_y + (2 * menu_item_height);
        const char *menu_text = "Settings";
        int text_width = MeasureText(menu_text, 40);
        int text_x = (screen_width - text_width) / 2;

        Color text_color = (2 == state->selected_menu_item) ? YELLOW : WHITE;
        DrawText(menu_text, text_x, (int)item_y, 40, text_color);

        // Draw character cursor for selected item
        if (2 == state->selected_menu_item && state->menu_cursor_texture.width > 0)
        {
            float cursor_x = text_x + character_offset_x;
            float cursor_y = item_y;
            float cursor_size = 40.0f;

            Rectangle source = {0, 0, (float)state->menu_cursor_texture.width, (float)state->menu_cursor_texture.height};
            Rectangle dest = {cursor_x, cursor_y, cursor_size, cursor_size};
            DrawTexturePro(state->menu_cursor_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }

    // Item 3: Exit
    {
        float item_y = menu_start_y + (3 * menu_item_height);
        const char *menu_text = "Exit";
        int text_width = MeasureText(menu_text, 40);
        int text_x = (screen_width - text_width) / 2;

        Color text_color = (3 == state->selected_menu_item) ? YELLOW : WHITE;
        DrawText(menu_text, text_x, (int)item_y, 40, text_color);

        // Draw character cursor for selected item
        if (3 == state->selected_menu_item && state->menu_cursor_texture.width > 0)
        {
            float cursor_x = text_x + character_offset_x;
            float cursor_y = item_y;
            float cursor_size = 40.0f;

            Rectangle source = {0, 0, (float)state->menu_cursor_texture.width, (float)state->menu_cursor_texture.height};
            Rectangle dest = {cursor_x, cursor_y, cursor_size, cursor_size};
            DrawTexturePro(state->menu_cursor_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }

    // Draw instructions
    const char *instructions = "Press UP/DOWN to navigate, ENTER to select";
    int instr_width = MeasureText(instructions, 16);
    int instr_x = (screen_width - instr_width) / 2;
    int instr_y = screen_height - 40;
    DrawText(instructions, instr_x, instr_y, 16, LIGHTGRAY);
}

// Helper function to draw options menu
static void draw_options_menu(GameState *state)
{
    int screen_width = state->screen_width;
    int screen_height = state->screen_height;

    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 150});

    // Draw title
    const char *title_text = "Options";
    int title_width = MeasureText(title_text, 60);
    int title_x = (screen_width - title_width) / 2;
    int title_y = screen_height / 2 - 100;

    // Draw title with shadow
    DrawText(title_text, title_x - 2, title_y - 2, 60, BLACK);
    DrawText(title_text, title_x + 2, title_y + 2, 60, BLACK);
    DrawText(title_text, title_x, title_y, 60, YELLOW);

    // Volume control section
    float item_y = screen_height / 2 + 20;
    float slider_width = 300.0f;
    float slider_height = 20.0f;
    float slider_x = (screen_width - slider_width) / 2;
    float slider_y = item_y;

    // Draw volume label
    const char *volume_label = "Volume:";
    int label_width = MeasureText(volume_label, 30);
    DrawText(volume_label, (int)(slider_x - label_width - 20), (int)(slider_y - 10), 30, WHITE);

    // Draw slider background
    DrawRectangle((int)slider_x, (int)slider_y, (int)slider_width, (int)slider_height, DARKGRAY);

    // Draw slider fill
    float fill_width = state->music_volume * slider_width;
    DrawRectangle((int)slider_x, (int)slider_y, (int)fill_width, (int)slider_height, ORANGE);

    // Draw slider border
    DrawRectangleLines((int)slider_x, (int)slider_y, (int)slider_width, (int)slider_height, WHITE);

    // Draw volume percentage text
    const char *volume_text = TextFormat("%.0f%%", state->music_volume * 100.0f);
    int volume_width = MeasureText(volume_text, 24);
    DrawText(volume_text, (int)(slider_x + slider_width + 20), (int)(slider_y - 5), 24, WHITE);

    // Draw hint for volume control
    const char *volume_hint = "(LEFT/RIGHT to adjust)";
    int hint_width = MeasureText(volume_hint, 16);
    DrawText(volume_hint, (int)(slider_x + (slider_width - hint_width) / 2), (int)(slider_y + 35), 16, LIGHTGRAY);

    // Back button
    float back_y = item_y + 100.0f;
    const char *back_text = "Back";
    int back_width = MeasureText(back_text, 40);
    int back_x = (screen_width - back_width) / 2;

    Color back_color = (state->options_menu_selection == 1) ? YELLOW : WHITE;
    DrawText(back_text, back_x, (int)back_y, 40, back_color);

    // Draw instructions
    const char *instructions = "Press UP/DOWN to navigate, ENTER to select, ESC to go back";
    int instr_width = MeasureText(instructions, 16);
    int instr_x = (screen_width - instr_width) / 2;
    int instr_y = screen_height - 40;
    DrawText(instructions, instr_x, instr_y, 16, LIGHTGRAY);
}

// Helper function to draw pause menu
static void draw_pause_menu(GameState *state)
{
    int screen_width = state->screen_width;
    int screen_height = state->screen_height;

    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 150});

    // Draw title
    const char *title_text = "Game Paused";
    int title_width = MeasureText(title_text, 60);
    int title_x = (screen_width - title_width) / 2;
    int title_y = screen_height / 2 - 100;

    // Draw title with shadow
    DrawText(title_text, title_x - 2, title_y - 2, 60, BLACK);
    DrawText(title_text, title_x + 2, title_y + 2, 60, BLACK);
    DrawText(title_text, title_x, title_y, 60, YELLOW);

    // Menu items
    const char *menu_items[3] = {"Resume Game", "Settings", "Quit To Menu"};
    float menu_start_y = screen_height / 2 + 20;
    float menu_item_height = 70.0f;
    float character_offset_x = -180.0f; // Offset to the left of menu items

    // Draw menu items
    for (int i = 0; i < 3; i++)
    {
        float item_y = menu_start_y + (i * menu_item_height);
        int text_width = MeasureText(menu_items[i], 40);
        int text_x = (screen_width - text_width) / 2;

        // Highlight selected item
        Color text_color = (i == state->pause_menu_selection) ? YELLOW : WHITE;
        DrawText(menu_items[i], text_x, (int)item_y, 40, text_color);

        // Draw character cursor for selected item
        if (i == state->pause_menu_selection && state->menu_cursor_texture.width > 0)
        {
            float cursor_x = text_x + character_offset_x;
            float cursor_y = item_y;
            float cursor_size = 40.0f;

            Rectangle source = {0, 0, (float)state->menu_cursor_texture.width, (float)state->menu_cursor_texture.height};
            Rectangle dest = {cursor_x, cursor_y, cursor_size, cursor_size};
            DrawTexturePro(state->menu_cursor_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }

    // Draw instructions
    const char *instructions = "Press UP/DOWN to navigate, ENTER to select";
    int instr_width = MeasureText(instructions, 16);
    int instr_x = (screen_width - instr_width) / 2;
    int instr_y = screen_height - 40;
    DrawText(instructions, instr_x, instr_y, 16, LIGHTGRAY);
}

void game_draw(GameState *state)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw options menu if active (overlays everything)
    if (state->options_menu_active)
    {
        draw_options_menu(state);
        EndDrawing();
        return;
    }

    // If title screen, draw it
    if (state->current_screen == GAME_SCREEN_TITLE)
    {
        draw_title_screen(state);
        EndDrawing();
        return;
    }

    // If victory screen, draw it
    if (state->game_victory)
    {
        draw_victory_screen(state);
        EndDrawing();
        return;
    }

    // If in level transition, only draw transition screen
    if (state->in_level_transition)
    {
        draw_level_transition_screen(state);
        EndDrawing();
        return;
    }

    // Draw background with hazard gaps
    Level *current_level = &state->levels[state->current_level_index];
    background_draw_with_hazards(&background, &current_level->hazards);

    // Draw hazards
    for (int i = 0; i < current_level->hazards.count; i++)
    {
        hazard_draw(&current_level->hazards.hazards[i], background.camera.target.x);
    }

    // Draw monsters
    for (int i = 0; i < current_level->monsters.count; i++)
    {
        monster_draw(&current_level->monsters.monsters[i], background.camera.target.x);
    }

    // Draw projectiles
    for (int i = 0; i < state->projectiles.count; i++)
    {
        if (state->projectiles.projectiles[i].active)
        {
            projectile_draw(&state->projectiles.projectiles[i], background.camera.target.x);
        }
    }

    // Draw pickups
    for (int i = 0; i < current_level->pickups.count; i++)
    {
        if (current_level->pickups.pickups[i].active)
        {
            pickup_draw(&current_level->pickups.pickups[i], background.camera.target.x);
        }
    }

    // Draw loot items
    loot_list_draw(&current_level->loot, background.camera.target.x);

    // Draw goal marker
    draw_goal_marker(current_level, background.camera.target.x);

    // Draw game objects
    player_draw(&player, background.camera.target.x);

    // Draw UI
    draw_hearts_ui(&player, state->screen_width, state->screen_height);
    draw_loot_inventory_ui(&player, state->screen_width);
    draw_level_ui(state);

    // Draw damage message if active (and game over message if applicable)
    if (state->burnt_message_timer > 0.0f)
    {
        // Flashing effect based on timer
        float alpha = (state->burnt_message_timer > 1.0f) ? 255.0f : (state->burnt_message_timer * 255.0f);
        Color message_color = (Color){255, 100, 0, (unsigned char)alpha}; // Orange color

        if (state->game_over)
        {
            // Display "You Lose!" prominently above damage message
            const char *lose_message = "You Lose!";
            int lose_text_width = MeasureText(lose_message, 60);
            int lose_center_x = (state->screen_width - lose_text_width) / 2;
            int lose_center_y = state->screen_height / 2 - 60;
            DrawText(lose_message, lose_center_x, lose_center_y, 60, RED);
        }

        // Display appropriate damage message based on damage type
        const char *damage_message = damage_type_get_message(player.active_damage_type);

        int message_text_width = MeasureText(damage_message, 40);
        int message_center_x = (state->screen_width - message_text_width) / 2;
        int message_center_y = state->screen_height / 2 + (state->game_over ? 20 : -20);
        DrawText(damage_message, message_center_x, message_center_y, 40, message_color);
    }

    // Draw debug info
    DrawFPS(10, 10);

    // Draw pause menu if active
    if (state->pause_menu_active && state->current_screen == GAME_SCREEN_PLAYING)
    {
        draw_pause_menu(state);
    }

    EndDrawing();
}

void game_cleanup(GameState *state)
{
    player_cleanup(&player);
    background_cleanup(&background);

    // Unload menu cursor texture
    if (state->menu_cursor_texture.width > 0)
    {
        UnloadTexture(state->menu_cursor_texture);
    }

    // Cleanup all levels
    for (int i = 0; i < state->level_count; i++)
    {
        level_cleanup(&state->levels[i]);
    }

    CloseWindow();
}
