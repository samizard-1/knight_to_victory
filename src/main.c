#include "game.h"
#include "asset_paths.h"

int main(void)
{
    GameState game_state = {0};

    // Initialize asset paths
    init_asset_paths();

    // Initialize game
    game_init(&game_state);

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

        // Update
        game_update(&game_state);

        // Draw
        game_draw(&game_state);
    }

    // Cleanup
    game_cleanup(&game_state);

    return 0;
}
