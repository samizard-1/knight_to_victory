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
    // Note: Textures are cached statically and managed globally, not per-list
    // So we don't unload them here to avoid double-unload issues
    
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
    // Cache textures by type to avoid loading the same texture multiple times
    static Texture2D dust_storm_texture = {0};
    static Texture2D lava_jet_texture = {0};
    static Texture2D wind_daggers_texture = {0};
    
    if (hazard.texture.id == 0)
    {
        switch (hazard.type)
        {
        case HAZARD_DUST_STORM:
            if (dust_storm_texture.id == 0)
            {
                dust_storm_texture = LoadTexture(get_asset_path("dust_tornado.png"));
            }
            hazard.texture = dust_storm_texture;
            break;
        case HAZARD_LAVA_JET:
            if (lava_jet_texture.id == 0)
            {
                lava_jet_texture = LoadTexture(get_asset_path("lava_jet.png"));
            }
            hazard.texture = lava_jet_texture;
            break;
        case HAZARD_WIND_DAGGERS:
            if (wind_daggers_texture.id == 0)
            {
                wind_daggers_texture = LoadTexture(get_asset_path("wind_daggers.png"));
            }
            hazard.texture = wind_daggers_texture;
            break;
        case HAZARD_LAVA_PIT:
        case HAZARD_SPIKE_TRAP:
        default:
            // These don't use textures
            break;
        }
    }

    if (list->count < list->capacity)
    {
        list->hazards[list->count++] = hazard;
    }
}

void hazard_update(Hazard *hazard)
{
    if (!hazard->active)
        return;

    // Update movement
    if (hazard->can_move)
    {
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

    // Update fading
    if (hazard->can_fade)
    {
        hazard->fade_timer += GetFrameTime();

        float total_cycle_time = hazard->fade_opaque_duration + hazard->fade_out_duration + hazard->fade_out_interval + hazard->fade_in_duration;

        // Wrap timer to cycle
        if (hazard->fade_timer >= total_cycle_time)
        {
            hazard->fade_timer -= total_cycle_time;
        }

        // Determine current phase and opacity
        float current_time = hazard->fade_timer;

        if (current_time < hazard->fade_opaque_duration)
        {
            // Fully opaque phase
            hazard->is_faded_out = false;
            hazard->current_opacity = 1.0f;
        }
        else if (current_time < hazard->fade_opaque_duration + hazard->fade_out_duration)
        {
            // Fading out phase
            hazard->is_faded_out = true;
            float fade_out_time = current_time - hazard->fade_opaque_duration;
            hazard->current_opacity = 1.0f - (fade_out_time / hazard->fade_out_duration);
        }
        else if (current_time < hazard->fade_opaque_duration + hazard->fade_out_duration + hazard->fade_out_interval)
        {
            // Fully faded out phase
            hazard->is_faded_out = true;
            hazard->current_opacity = 0.0f;
        }
        else
        {
            // Fading in phase
            hazard->is_faded_out = false;
            float fade_in_time = current_time - (hazard->fade_opaque_duration + hazard->fade_out_duration + hazard->fade_out_interval);
            hazard->current_opacity = fade_in_time / hazard->fade_in_duration;
        }
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
    case HAZARD_LAVA_JET:
        hazard->can_move = false; // Lava jets don't move by default
        hazard->velocity = (Vector2){0, 0};
        break;
    case HAZARD_WIND_DAGGERS:
        hazard->can_move = true;
        hazard->velocity = (Vector2){speed, 0};
        break;
    }
}

void hazard_init_fade(Hazard *hazard, float fade_opaque_duration, float fade_out_duration, float fade_out_interval, float fade_in_duration)
{
    hazard->can_fade = true;
    hazard->fade_opaque_duration = fade_opaque_duration;
    hazard->fade_out_duration = fade_out_duration;
    hazard->fade_out_interval = fade_out_interval;
    hazard->fade_in_duration = fade_in_duration;
    hazard->fade_timer = 0.0f;
    hazard->current_opacity = 1.0f;
    hazard->is_faded_out = false;
}

bool hazard_is_dangerous(Hazard *hazard)
{
    // Hazard is dangerous if it's not faded out
    if (hazard->can_fade)
    {
        return !hazard->is_faded_out;
    }
    return true; // Non-fading hazards are always dangerous
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
        // Draw dust storm with opacity based on fade state
        unsigned char alpha = (unsigned char)(hazard->current_opacity * 150.0f);
        DrawTexturePro(hazard->texture,
                       (Rectangle){0, 0, (float)hazard->texture.width, (float)hazard->texture.height},
                       draw_rect,
                       (Vector2){0, 0},
                       0.0f,
                       (Color){255, 255, 255, alpha});
        break;
    }
    case HAZARD_LAVA_JET:
    {
        unsigned char alpha = (unsigned char)(hazard->current_opacity * 150.0f);
        DrawTexturePro(hazard->texture,
                       (Rectangle){0, 0, (float)hazard->texture.width, (float)hazard->texture.height},
                       draw_rect,
                       (Vector2){0, 0},
                       0.0f,
                       (Color){255, 255, 255, alpha});
        break;
    }
    case HAZARD_WIND_DAGGERS:
    {
        DrawTexturePro(hazard->texture,
                       (Rectangle){0, 0, (float)hazard->texture.width, (float)hazard->texture.height},
                       draw_rect,
                       (Vector2){0, 0},
                       0.0f,
                       (Color){255, 255, 255, 255});
        break;
    }
    }
}
