#include "hazard.h"
#include "asset_paths.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

HazardList hazard_list_create(int capacity)
{
    HazardList list;
    list.capacity = capacity;
    list.count = 0;
    list.hazards = (Hazard *)malloc(sizeof(Hazard) * capacity);
    return list;
}

void hazard_list_cleanup(HazardList *list)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->hazards[i].texture.id != 0)
        {
            UnloadTexture(list->hazards[i].texture);
        }
    }

    if (list->hazards != NULL)
    {
        free(list->hazards);
        list->hazards = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}

void hazard_list_add(HazardList *list, Hazard hazard)
{
    if (hazard.type == HAZARD_DUST_STORM && hazard.texture.id == 0)
    {
        hazard.texture = LoadTexture(get_asset_path("dust_tornado.png"));
    }

    if (list->count < list->capacity)
    {
        list->hazards[list->count++] = hazard;
    }
}

void hazard_update(Hazard *hazard)
{
    if (!hazard->active || !hazard->can_move)
        return;

    // Update position based on velocity (similar to monster patrol logic)
    hazard->bounds.x += hazard->velocity.x * GetFrameTime();

    // Patrol logic - bounce back and forth at boundaries
    if (hazard->bounds.x <= hazard->patrol_left_bound)
    {
        hazard->bounds.x = hazard->patrol_left_bound;
        hazard->velocity.x = hazard->patrol_speed; // Move right
    }
    else if (hazard->bounds.x >= hazard->patrol_right_bound)
    {
        hazard->bounds.x = hazard->patrol_right_bound;
        hazard->velocity.x = -hazard->patrol_speed; // Move left
    }
}

void hazard_init_movement(Hazard *hazard, float left_bound, float right_bound, float speed)
{
    // Set default movement properties based on hazard type
    hazard->patrol_left_bound = left_bound;
    hazard->patrol_right_bound = right_bound;
    hazard->patrol_speed = speed;

    // Determine if this hazard should move by default
    switch (hazard->type)
    {
    case HAZARD_DUST_STORM:
        hazard->can_move = true; // Dust storms move by default
        hazard->velocity = (Vector2){speed, 0};
        break;
    case HAZARD_LAVA_PIT:
    case HAZARD_SPIKE_TRAP:
    default:
        hazard->can_move = false; // Lava pits and spikes don't move by default
        hazard->velocity = (Vector2){0, 0};
        break;
    }
}
bool hazard_check_collision(Hazard *hazard, Rectangle player_rect)
{
    return CheckCollisionRecs(hazard->bounds, player_rect);
}

void hazard_draw(Hazard *hazard, float camera_x)
{
    if (!hazard->active)
        return;

    // Apply same camera offset as player drawing
    Rectangle draw_rect = hazard->bounds;
    draw_rect.x = hazard->bounds.x - camera_x + GetScreenWidth() / 2.0f;

    switch (hazard->type)
    {
    case HAZARD_LAVA_PIT:
    {
        // Draw lava pit as orange/red filled rectangle
        DrawRectangle((int)draw_rect.x, (int)draw_rect.y, (int)draw_rect.width, (int)draw_rect.height,
                      (Color){255, 100, 0, 255});

        // Draw animated flame effects on top
        static float flame_time = 0.0f;
        flame_time += 0.016f; // Approximate frame time
        float wave = sinf(flame_time * 3.0f) * 2.0f;

        // Draw flame-like lines
        for (int i = 0; i < 3; i++)
        {
            float x_offset = draw_rect.x + (i * draw_rect.width / 3.0f) + draw_rect.width / 6.0f;
            float flame_height = 20.0f + wave;
            DrawLine((int)x_offset, (int)(draw_rect.y + draw_rect.height),
                     (int)x_offset, (int)(draw_rect.y + draw_rect.height - flame_height),
                     (Color){255, 200, 0, 200});
        }
        break;
    }
    case HAZARD_SPIKE_TRAP:
    {
        // Draw spike trap as red rectangle
        DrawRectangle((int)draw_rect.x, (int)draw_rect.y, (int)draw_rect.width, (int)draw_rect.height,
                      (Color){200, 0, 0, 255});
        break;
    }
    case HAZARD_DUST_STORM:
    {
        // Draw dust storm
        DrawTexturePro(hazard->texture,
                       (Rectangle){0, 0, (float)hazard->texture.width, (float)hazard->texture.height},
                       draw_rect,
                       (Vector2){0, 0},
                       0.0f,
                       (Color){255, 255, 255, 150});
        break;
    }
    }
}
