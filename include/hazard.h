#ifndef HAZARD_H
#define HAZARD_H

#include "raylib.h"

typedef enum
{
    HAZARD_LAVA_PIT,
    HAZARD_SPIKE_TRAP,
    HAZARD_DUST_STORM,
    HAZARD_LAVA_JET,
    HAZARD_WIND_DAGGERS
} HazardType;

typedef struct
{
    HazardType type;
    Rectangle bounds;         // Position and size of the hazard
    Rectangle initial_bounds; // Original position and size (for reset)
    int damage;               // Damage dealt on contact
    bool active;              // Whether the hazard is still active
    Texture2D texture;        // Texture for visual representation
    bool can_move;            // Whether this hazard can move/patrol
    Vector2 velocity;         // Current movement velocity
    float patrol_left_bound;  // Left boundary for movement
    float patrol_right_bound; // Right boundary for movement
    float patrol_speed;       // Speed of movement
    // Fade properties
    bool can_fade;              // Whether this hazard fades in/out
    float fade_timer;           // Current time in fade cycle
    float fade_opaque_duration; // Time hazard remains fully opaque before fading
    float fade_out_duration;    // Time to fade out (0 = instant)
    float fade_out_interval;    // Time hazard stays faded out before fading back in
    float fade_in_duration;     // Time to fade in (0 = instant)
    float current_opacity;      // Current opacity (0.0 = invisible, 1.0 = fully visible)
    bool is_faded_out;          // Whether hazard is currently faded out (safe to pass)
} Hazard;

typedef struct
{
    Hazard *hazards;
    int count;
    int capacity;
} HazardList;

// Hazard functions
HazardList hazard_list_create(int capacity);
void hazard_list_cleanup(HazardList *list);
void hazard_list_add(HazardList *list, Hazard hazard);
bool hazard_check_collision(Hazard *hazard, Rectangle player_rect);
void hazard_draw(Hazard *hazard, float camera_x);
void hazard_update(Hazard *hazard);
void hazard_reset(Hazard *hazard);

// Initialize movement properties for a hazard based on type
// Sets default movement properties: dust storms move by default, lava pits don't
void hazard_init_movement(Hazard *hazard, float left_bound, float right_bound, float speed);

// Initialize fade properties for a hazard
// Allows hazard to fade in/out on a cycle
// Parameters: fade_opaque_duration (time fully visible), fade_out_duration (fade time),
//             fade_out_interval (time faded out), fade_in_duration (fade in time)
void hazard_init_fade(Hazard *hazard, float fade_opaque_duration, float fade_out_duration, float fade_out_interval, float fade_in_duration);

// Check if hazard is dangerous (not faded out)
// Returns true if hazard can damage, false if faded out
bool hazard_is_dangerous(Hazard *hazard);

#endif // HAZARD_H
