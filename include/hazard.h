#ifndef HAZARD_H
#define HAZARD_H

#include "raylib.h"

typedef enum
{
    HAZARD_LAVA_PIT,
    HAZARD_SPIKE_TRAP,
    HAZARD_DUST_STORM
} HazardType;

typedef struct
{
    HazardType type;
    Rectangle bounds;         // Position and size of the hazard
    int damage;               // Damage dealt on contact
    bool active;              // Whether the hazard is still active
    Texture2D texture;        // Texture for visual representation
    bool can_move;            // Whether this hazard can move/patrol
    Vector2 velocity;         // Current movement velocity
    float patrol_left_bound;  // Left boundary for movement
    float patrol_right_bound; // Right boundary for movement
    float patrol_speed;       // Speed of movement
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

// Initialize movement properties for a hazard based on type
// Sets default movement properties: dust storms move by default, lava pits don't
void hazard_init_movement(Hazard *hazard, float left_bound, float right_bound, float speed);

#endif // HAZARD_H
