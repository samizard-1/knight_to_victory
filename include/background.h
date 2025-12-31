#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "raylib.h"

typedef struct
{
    int chunk_index;
    Vector3 *mountain_points;
    int mountain_point_count;
    bool generated;
} BackgroundChunk;

typedef struct
{
    int cloud_id;
    Vector2 position;
    float size;
    bool generated;
} Cloud;

typedef struct
{
    BackgroundChunk *chunks;
    Cloud *clouds;
    int max_chunks;
    int max_clouds;
    int chunk_size; // Width of each chunk in pixels
    float mountain_base_height;
    int seed_variant; // Variant seed for different procedural backgrounds per level
    Camera2D camera;
} Background;

// Background functions
Background background_create(void);
Background background_create_with_variant(int seed_variant);
void background_update(Background *bg, Vector2 player_pos);
void background_draw(Background *bg);
void background_draw_with_hazards(Background *bg, const void *hazard_list);
void background_cleanup(Background *bg);

#endif // BACKGROUND_H
