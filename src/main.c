#include "game.h"

int main(void) {
    GameState game_state = {0};
    
    // Initialize game
    game_init(&game_state);
    
    // Main game loop
    while (!WindowShouldClose() && game_state.running) {
        // Update
        game_update(&game_state);
        
        // Draw
        game_draw(&game_state);
    }
    
    // Cleanup
    game_cleanup(&game_state);
    
    return 0;
}
