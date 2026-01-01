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
#include <math.h>
#include <stdlib.h>

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

// Helper function to draw projectile inventory UI
static void draw_projectile_inventory_ui(Player *player, int screen_height)
{
    float icon_height = 40.0f;
    float padding = 10.0f;

    // Calculate width based on texture aspect ratio
    float aspect_ratio = (float)player->fireball_texture.width / (float)player->fireball_texture.height;
    float icon_width = icon_height * aspect_ratio;

    float start_x = padding + icon_width / 2.0f;
    float start_y = screen_height - padding - icon_height / 2.0f;

    // Draw fireball icon
    Rectangle source = {0, 0, (float)player->fireball_texture.width, (float)player->fireball_texture.height};
    Rectangle dest = {start_x, start_y, icon_width, icon_height};
    DrawTexturePro(player->fireball_texture, source, dest, (Vector2){icon_width / 2.0f, icon_height / 2.0f}, 0.0f, WHITE);

    // Draw inventory count centered on icon
    const char *count_text = TextFormat("%d", player->projectile_inventory);
    int text_width = MeasureText(count_text, 20);
    float text_x = start_x - text_width / 2.0f + 8.0f; // Shifted right for better visibility
    float text_y = start_y - 10.0f;                    // Vertically center on icon
    DrawText(count_text, (int)text_x, (int)text_y, 20, BLACK);
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
    state->level_count = 20;

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

    state->current_level_index = 10;
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

    InitWindow(state->screen_width, state->screen_height, "Side-Scrolling Game");
    SetTargetFPS(state->fps);

    // Initialize levels
    initialize_levels(state);

    // Initialize game objects
    Level *current_level = &state->levels[state->current_level_index];
    player = player_create(current_level->player_start_position.x, current_level->player_start_position.y);
    background = background_create_with_variant(current_level->background.variant);
}

void game_update(GameState *state)
{
    state->delta_time = GetFrameTime();

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
            player.is_burnt = false;           // Reset burnt effect
            level_reset(next_level);

            // Reinitialize background with new level's variant
            background_cleanup(&background);
            background = background_create_with_variant(next_level->background.variant);

            // Update current_level reference
            current_level = &state->levels[state->current_level_index];
        }
        return; // Don't process other updates during transition
    }

    // Update game objects (only if not paused)
    if (!state->is_paused)
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
            player.projectile_inventory--; // Consume one projectile
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
            if (hazard->active && hazard_check_collision(hazard, player_rect))
            {
                // Player hit a hazard
                player_take_damage(&player, hazard->damage);

                // Set burnt effect
                player.is_burnt = true;
                player.burnt_timer = PAUSE_DURATION;                // Show burnt effect
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
                // Player hit a monster
                player_take_damage(&player, 1); // Monsters deal 1 damage
                player_set_hurt(&player);       // Set hurt state for monster damage

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
                    player.projectile_inventory += pickup->value;

                    // Cap at max
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
            // Monster projectile hit player
            player_take_damage(&player, 1); // Each projectile deals 1 damage

            // Set burnt effect (same as lava hazard)
            player.is_burnt = true;
            player.burnt_timer = PAUSE_DURATION;                // Show burnt effect
            state->burnt_message_timer = PAUSE_DURATION;        // Display message
            state->is_paused = true;                            // Pause the game
            state->hazard_cooldown = 3.0f;                      // Cooldown to prevent re-collision
            state->last_collision_type = COLLISION_TYPE_HAZARD; // Track collision type
            projectile->active = false;                         // Destroy projectile on impact

            // Check if player is out of hearts
            if (player.hearts <= 0)
            {
                state->game_over = true; // Trigger game over
            }
        }
    }

    // Update hazard cooldown
    if (state->hazard_cooldown > 0.0f)
    {
        state->hazard_cooldown -= state->delta_time;
    }

    // update sword attack cooldown
    if (state->sword_attack_cooldown > 0.0f)
    {
        state->sword_attack_cooldown -= state->delta_time;
    }

    // Update burnt timer
    if (player.is_burnt)
    {
        player.burnt_timer -= state->delta_time;
        if (player.burnt_timer <= 0.0f)
        {
            player.is_burnt = false;
            player.burnt_timer = 0.0f;
        }
    }

    // Update burnt message timer
    if (state->burnt_message_timer > 0.0f)
    {
        state->burnt_message_timer -= state->delta_time;
    }
    else if (state->is_paused)
    {
        // Timer expired, resume game and respawn
        state->is_paused = false;

        // Respawn player at level start
        player.position = current_level->player_start_position;
        player.velocity = (Vector2){0, 0};
    }

    // Update game over timer
    if (state->game_over)
    {
        // When burnt message timer expires and game is over, restart to level 1
        if (state->burnt_message_timer <= 0.0f)
        {
            // Restart from level 1 - reset all levels and player state
            state->current_level_index = 0;
            state->game_over = false;
            state->hazard_cooldown = 0.0f; // Reset cooldown to allow immediate damage detection
            player.hearts = player.max_hearts;
            player.is_dead = false;
            player.is_burnt = false;
            player.projectile_inventory = 0; // Reset inventory on game over

            // Reactivate all enemies (monsters and hazards) in all levels
            for (int i = 0; i < state->level_count; i++)
            {
                level_reactivate_enemies(&state->levels[i]);
                level_reset(&state->levels[i]);
            }

            // Reset to level 1
            Level *level1 = &state->levels[0];
            player.position = level1->player_start_position;
            player.velocity = (Vector2){0, 0};

            // Reinitialize background
            background_cleanup(&background);
            background = background_create_with_variant(level1->background.variant);
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
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
        {
            state->running = false;
        }
    }

    // Check for exit
    if (IsKeyPressed(KEY_ESCAPE))
    {
        state->running = false;
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

void game_draw(GameState *state)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

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

    // Draw goal marker
    draw_goal_marker(current_level, background.camera.target.x);

    // Draw game objects
    player_draw(&player, background.camera.target.x);

    // Draw UI
    draw_hearts_ui(&player, state->screen_width, state->screen_height);
    draw_projectile_inventory_ui(&player, state->screen_height);
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

        // Display appropriate damage message
        const char *damage_message;
        if (state->last_collision_type == COLLISION_TYPE_MONSTER)
        {
            damage_message = "You've been Hit!";
        }
        else
        {
            damage_message = "You're burnt!";
        }

        int message_text_width = MeasureText(damage_message, 40);
        int message_center_x = (state->screen_width - message_text_width) / 2;
        int message_center_y = state->screen_height / 2 + (state->game_over ? 20 : -20);
        DrawText(damage_message, message_center_x, message_center_y, 40, message_color);
    }

    // Draw debug info
    DrawFPS(10, 10);

    EndDrawing();
}

void game_cleanup(GameState *state)
{
    player_cleanup(&player);
    background_cleanup(&background);

    // Cleanup all levels
    for (int i = 0; i < state->level_count; i++)
    {
        level_cleanup(&state->levels[i]);
    }

    CloseWindow();
}
