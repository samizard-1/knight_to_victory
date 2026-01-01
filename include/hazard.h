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
    Rectangle bounds; // Position and size of the hazard
    int damage;       // Damage dealt on contact
    bool active;      // Whether the hazard is still active
    Texture2D texture; // Texture for visual representation
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

#endif // HAZARD_H
