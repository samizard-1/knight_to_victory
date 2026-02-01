#include "game.h"
#include "asset_paths.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Global game state for Emscripten callback
static GameState game_state_global = {0};
static Music background_music = {0};

// Update frame function for Emscripten loop
void UpdateDrawFrame(void)
{
    // Check if user clicked the window close button
    if (WindowShouldClose())
    {
        game_state_global.running = false;
    }

    // Update music stream (required for streaming music to work)
    UpdateMusicStream(background_music);

    // Update
    game_update(&game_state_global);

    // Draw
    game_draw(&game_state_global);
}

int main(void)
{
    game_state_global = (GameState){0};

    // Initialize asset paths
    init_asset_paths();

    // Initialize game (this will call InitWindow and InitAudioDevice)
    game_init(&game_state_global);

    // Load and play background music after game is initialized
    const char *music_path = get_asset_path("fantasy-craft-loop-431346.mp3");

    // Use LoadMusicStream for long audio files like background music
    background_music = LoadMusicStream(music_path);

    if (background_music.frameCount > 0)
    {
        SetMusicVolume(background_music, 0.5f); // Set volume to 50%
        PlayMusicStream(background_music);
    }

#if defined(PLATFORM_WEB)
    // For web, use Emscripten's main loop
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    // Main game loop for desktop
    // We use game_state_global.running as the primary exit condition to allow ESC to be handled by our pause menu
    // However, we still check WindowShouldClose() which will be set by the window close button (X)
    while (game_state_global.running)
    {
        UpdateDrawFrame();
    }
#endif

    // Cleanup
    game_cleanup(&game_state_global);
    UnloadMusicStream(background_music);

    CloseAudioDevice();

    return 0;
}
