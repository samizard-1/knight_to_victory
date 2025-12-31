#include "hazard.h"
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
    if (list->count < list->capacity)
    {
        list->hazards[list->count++] = hazard;
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
    }
}
