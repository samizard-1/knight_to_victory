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

// Monster settings
#define MONSTER_DEAD_TEXTURE_TIME 5.0f // Time to show dead texture before removing monster

// Game state settings
#define PAUSE_DURATION 2.0f             // Duration of pause after losing a heart (in seconds)
#define DAMAGE_DISPLAY_MONSTER_HIT 0.5f // Duration to display monster hit texture (in seconds)
#define DAMAGE_DISPLAY_FIRE 2.0f        // Duration to display fire damage texture (in seconds)
#define DAMAGE_DISPLAY_DUST 2.0f        // Duration to display dust damage texture (in seconds)
#define RESPAWN_DELAY PAUSE_DURATION    // Delay before respawning player to level start (in seconds)
#define PROTECTION_POTION_DURATION 5.0f // Duration of protection potion effect (in seconds)
#define PROTECTION_POTION_GRACE_PERIOD 3.0f // Grace period after potion deactivates (in seconds)

// Background settings
#define MOUNTAIN_BASE_HEIGHT 500
#define CHUNK_WIDTH 800
#define CLOUD_SPACING 300

#endif // CONFIG_H
