#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "level.h"
#include "projectile.h"

#define MAX_LEVELS 20

typedef enum
{
    GAME_SCREEN_TITLE,
    GAME_SCREEN_PLAYING,
    GAME_SCREEN_VICTORY
} GameScreen;

typedef enum
{
    COLLISION_TYPE_NONE,
    COLLISION_TYPE_HAZARD,
    COLLISION_TYPE_MONSTER
} CollisionType;

typedef struct
{
    int screen_width;
    int screen_height;
    float delta_time;
    int fps;
    bool running;
    Level levels[MAX_LEVELS];
    int level_count;
    int current_level_index;
    float burnt_message_timer;         // Timer for displaying damage message
    bool is_paused;                    // True when game is paused due to collision
    float hazard_cooldown;             // Cooldown to prevent immediate re-collision after respawn
    float sword_attack_cooldown;       // Cooldown to prevent immediate re-attack
    bool game_over;                    // True when player loses all hearts
    CollisionType last_collision_type; // Track type of last collision
    ProjectileList projectiles;        // Active projectiles in the level
    bool in_level_transition;          // True when showing level transition screen
    int next_level_index;              // The level to transition to
    bool game_victory;                 // True when game is complete
    float victory_timer;               // Timer for victory animation
    float elapsed_time;                // Total elapsed time in seconds
    GameScreen current_screen;         // Current screen being displayed (TITLE, PLAYING, VICTORY)
    int selected_menu_item;            // 0 = Select Level, 1 = Start Game, 2 = Exit (title) | 0 = Resume, 1 = Quit (pause)
    int selected_level;                // 0-13 corresponds to level 1-14
    Texture2D menu_cursor_texture;     // Character texture used as menu cursor
    bool pause_menu_active;            // True when pause menu is displayed during gameplay
    int pause_menu_selection;          // 0 = Resume Game, 1 = Quit To Menu
    Music background_music;            // Background music that loops throughout the game
} GameState;

// Game functions
void game_init(GameState *state);
void game_update(GameState *state);
void game_draw(GameState *state);
void game_cleanup(GameState *state);

#endif // GAME_H
