#include "game.h"
#include "asset_paths.h"

int main(void)
{
    GameState game_state = {0};

    // Initialize asset paths
    init_asset_paths();

    // Initialize game (this will call InitWindow and InitAudioDevice)
    game_init(&game_state);

    // Load and play background music after game is initialized
    const char *music_path = get_asset_path("fantasy-craft-loop-431346.mp3");

    // Use LoadMusicStream for long audio files like background music
    game_state.background_music = LoadMusicStream(music_path);

    if (game_state.background_music.frameCount > 0)
    {
        SetMusicVolume(game_state.background_music, 0.5f); // Set volume to 50%
        PlayMusicStream(game_state.background_music);
    }

    // Main game loop
    // We use game_state.running as the primary exit condition to allow ESC to be handled by our pause menu
    // However, we still check WindowShouldClose() which will be set by the window close button (X)
    while (game_state.running)
    {
        // Check if user clicked the window close button
        if (WindowShouldClose())
        {
            game_state.running = false;
        }

        // Update music stream (required for streaming music to work)
        UpdateMusicStream(game_state.background_music);

        // Update
        game_update(&game_state);

        // Draw
        game_draw(&game_state);
    }

    // Cleanup
    game_cleanup(&game_state);
    UnloadMusicStream(game_state.background_music);

    CloseAudioDevice();

    return 0;
}
