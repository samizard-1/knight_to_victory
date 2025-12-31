#ifndef CONFIG_H
#define CONFIG_H

// Physics settings
#define GRAVITY 800.0f
#define GROUND_Y 600.0f
#define JUMP_POWER 400.0f
#define PLAYER_SPEED 200.0f

// Player health settings
#define MAX_HEARTS 3
#define INITIAL_HEARTS 3

// Game state settings
#define PAUSE_DURATION 2.0f // Duration of pause after losing a heart or game over (in seconds)

// Background settings
#define MOUNTAIN_BASE_HEIGHT 500
#define CHUNK_WIDTH 800
#define CLOUD_SPACING 300

#endif // CONFIG_H
