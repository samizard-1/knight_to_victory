#include "background.h"
#include "hazard.h"
#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CACHED_CHUNKS 10
#define MAX_CACHED_CLOUDS 20

// Simple pseudo-random number generator based on seed
static float pseudo_random(int seed)
{
    int x = seed * 73856093 ^ 19349663;
    x = (x ^ (x >> 16)) * 27408397;
    x = x ^ (x >> 15);
    return (float)(x & 0x7FFFFFFF) / 2147483647.0f;
}

// Generate mountain points for a chunk using deterministic algorithm
static void generate_mountain_chunk(BackgroundChunk *chunk, int chunk_index, float base_height, int seed_variant)
{
    chunk->chunk_index = chunk_index;
    // Generate one extra point to connect to next chunk smoothly
    chunk->mountain_point_count = CHUNK_WIDTH / 20 + 3; // Points every 20 pixels + overlap

    if (chunk->mountain_points)
    {
        free(chunk->mountain_points);
    }
    chunk->mountain_points = (Vector3 *)malloc(chunk->mountain_point_count * sizeof(Vector3));

    float start_x = chunk_index * CHUNK_WIDTH;
    float current_height = base_height;

    for (int i = 0; i < chunk->mountain_point_count; i++)
    {
        float x = start_x + i * 20;

        // Generate height variation using multiple sine waves for natural mountains
        // These are continuous functions based on world position, so they connect smoothly
        // Incorporate seed_variant to change the terrain shape
        float noise1 = sinf((x + seed_variant * 100) * 0.005f) * 80;
        float noise2 = sinf((x + seed_variant * 50) * 0.002f) * 120;
        float noise3 = pseudo_random((int)(x / 20) * 7919 + seed_variant * 1337) * 1;

        current_height = base_height - noise1 - noise2 - noise3;

        // Clamp mountain height to not go below ground level (600)
        if (current_height > 600.0f)
        {
            current_height = 600.0f;
        }

        chunk->mountain_points[i] = (Vector3){x, current_height, 0.0f};
    }

    chunk->generated = true;
}

// Get or create a chunk for a given index
static BackgroundChunk *get_chunk(Background *bg, int chunk_index)
{
    // Look for existing chunk
    for (int i = 0; i < MAX_CACHED_CHUNKS; i++)
    {
        if (bg->chunks[i].generated && bg->chunks[i].chunk_index == chunk_index)
        {
            return &bg->chunks[i];
        }
    }

    // Find empty slot or oldest chunk
    int slot = -1;
    for (int i = 0; i < MAX_CACHED_CHUNKS; i++)
    {
        if (!bg->chunks[i].generated)
        {
            slot = i;
            break;
        }
    }

    if (slot == -1)
    {
        slot = 0; // Reuse first slot if all full
    }

    // Generate new chunk
    generate_mountain_chunk(&bg->chunks[slot], chunk_index, bg->mountain_base_height, bg->seed_variant);
    return &bg->chunks[slot];
}

// Generate a cloud at a specific world position
static void generate_cloud(Cloud *cloud, int cloud_id)
{
    cloud->cloud_id = cloud_id;

    // Deterministically generate position based on cloud_id
    float base_x = cloud_id * CLOUD_SPACING;
    float rand_x = pseudo_random(cloud_id * 7919) * 150 - 75;           // Random offset within range
    float rand_y = pseudo_random(cloud_id * 1337 + 1) * 175 + 60;       // Y between 40-215
    float rand_size = pseudo_random(cloud_id * 9973 + 2) * 0.5f + 1.0f; // Size between 1.0-1.5

    cloud->position = (Vector2){base_x + rand_x, rand_y};
    cloud->size = rand_size;
    cloud->generated = true;
}

// Get or create a cloud for a given id
static Cloud *get_cloud(Background *bg, int cloud_id)
{
    // Look for existing cloud
    for (int i = 0; i < MAX_CACHED_CLOUDS; i++)
    {
        if (bg->clouds[i].generated && bg->clouds[i].cloud_id == cloud_id)
        {
            return &bg->clouds[i];
        }
    }

    // Find empty slot
    int slot = -1;
    for (int i = 0; i < MAX_CACHED_CLOUDS; i++)
    {
        if (!bg->clouds[i].generated)
        {
            slot = i;
            break;
        }
    }

    if (slot == -1)
    {
        slot = 0; // Reuse first slot if all full
    }

    // Generate new cloud
    generate_cloud(&bg->clouds[slot], cloud_id);
    return &bg->clouds[slot];
}

Background background_create(void)
{
    return background_create_with_variant(0);
}

Background background_create_with_variant(int seed_variant)
{
    Background bg;
    bg.chunk_size = CHUNK_WIDTH;
    bg.mountain_base_height = MOUNTAIN_BASE_HEIGHT;
    bg.seed_variant = seed_variant;
    bg.max_chunks = MAX_CACHED_CHUNKS;
    bg.max_clouds = MAX_CACHED_CLOUDS;
    bg.chunks = (BackgroundChunk *)malloc(MAX_CACHED_CHUNKS * sizeof(BackgroundChunk));
    bg.clouds = (Cloud *)malloc(MAX_CACHED_CLOUDS * sizeof(Cloud));

    // Initialize chunks
    for (int i = 0; i < MAX_CACHED_CHUNKS; i++)
    {
        bg.chunks[i].generated = false;
        bg.chunks[i].mountain_points = NULL;
        bg.chunks[i].mountain_point_count = 0;
    }

    // Initialize clouds
    for (int i = 0; i < MAX_CACHED_CLOUDS; i++)
    {
        bg.clouds[i].generated = false;
    }

    // Initialize camera
    bg.camera.target = (Vector2){0, 0};
    bg.camera.offset = (Vector2){0, 0};
    bg.camera.rotation = 0.0f;
    bg.camera.zoom = 1.0f;

    return bg;
}

void background_update(Background *bg, Vector2 player_pos)
{
    // Update camera to follow player
    bg->camera.target = (Vector2){
        player_pos.x,
        150 // Fixed vertical position
    };
}

void background_draw(Background *bg)
{
    // Draw sky - from top to ground level
    DrawRectangle(0, 0, GetScreenWidth(), GROUND_Y, (Color){135, 206, 235, 255});

    // Determine which chunks are visible
    int center_chunk = (int)(bg->camera.target.x / CHUNK_WIDTH);

    // Draw mountains for visible chunks (draw before dirt so mountains sit on top of sky but under dirt)
    for (int chunk_offset = -2; chunk_offset <= 2; chunk_offset++)
    {
        int chunk_idx = center_chunk + chunk_offset;
        BackgroundChunk *chunk = get_chunk(bg, chunk_idx);

        if (chunk->mountain_point_count < 2)
            continue;

        // Draw mountain as filled polygon
        Vector2 *screen_points = (Vector2 *)malloc((chunk->mountain_point_count + 2) * sizeof(Vector2));

        // Add base points for filled area
        for (int i = 0; i < chunk->mountain_point_count; i++)
        {
            screen_points[i] = (Vector2){
                chunk->mountain_points[i].x - bg->camera.target.x + GetScreenWidth() / 2,
                chunk->mountain_points[i].y};
        }

        // Close the polygon with ground level
        screen_points[chunk->mountain_point_count] = (Vector2){
            chunk->mountain_points[chunk->mountain_point_count - 1].x - bg->camera.target.x + GetScreenWidth() / 2,
            GetScreenHeight()};
        screen_points[chunk->mountain_point_count + 1] = (Vector2){
            chunk->mountain_points[0].x - bg->camera.target.x + GetScreenWidth() / 2,
            GetScreenHeight()};

        // Fill mountain with vertical lines from outline to ground
        // For each mountain segment, draw vertical fill lines
        for (int i = 0; i < chunk->mountain_point_count - 1; i++)
        {
            Vector2 p1 = screen_points[i];
            Vector2 p2 = screen_points[i + 1];

            // Get integer bounds
            int x_start = (int)p1.x;
            int x_end = (int)p2.x;

            // Ensure proper order
            if (x_start > x_end)
            {
                int temp = x_start;
                x_start = x_end;
                x_end = temp;
            }

            // Draw vertical lines for each X position
            for (int x = x_start; x <= x_end; x++)
            {
                // Interpolate Y position along the mountain line
                float t = (x_end > x_start) ? (float)(x - x_start) / (x_end - x_start) : 0;
                float y_peak = p1.y + t * (p2.y - p1.y);

                // Draw vertical line from peak to ground
                DrawLine(x, (int)y_peak, x, GROUND_Y, (Color){230, 200, 130, 255});
            }
        }

        // Draw mountain outline
        for (int i = 0; i < chunk->mountain_point_count; i++)
        {
            Vector2 p1 = screen_points[i];
            Vector2 p2 = screen_points[i + 1];
            DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, (Color){200, 170, 100, 255});
        }

        free(screen_points);
    }

    // Draw dirt/ground below ground level (draw after mountains so dirt is on top of mountains)
    DrawRectangle(0, GROUND_Y, GetScreenWidth(), GetScreenHeight() - GROUND_Y, (Color){139, 90, 43, 255});

    // Draw clouds - generate visible clouds based on camera position
    int center_cloud = (int)(bg->camera.target.x / CLOUD_SPACING);
    for (int cloud_offset = -2; cloud_offset <= 3; cloud_offset++)
    {
        int cloud_id = center_cloud + cloud_offset;
        Cloud *cloud = get_cloud(bg, cloud_id);

        // Convert world position to screen position
        float screen_x = cloud->position.x - bg->camera.target.x + GetScreenWidth() / 2.0f;
        float screen_y = cloud->position.y;

        // Calculate cloud visual bounds
        float base_size = 20.0f * cloud->size;
        float cloud_width = 50.0f * cloud->size;
        float cloud_left = screen_x - base_size;
        float cloud_right = screen_x + cloud_width + base_size;

        // Draw if any part of cloud is visible (with margin for smooth appearance)
        if (cloud_right > -200 && cloud_left < GetScreenWidth() + 200)
        {
            // Draw cloud from circles (scaled by size)
            DrawCircle((int)(screen_x + 0), (int)screen_y, (int)base_size, WHITE);
            DrawCircle((int)(screen_x + 25 * cloud->size), (int)screen_y, (int)(base_size * 1.25f), WHITE);
            DrawCircle((int)(screen_x + 50 * cloud->size), (int)screen_y, (int)base_size, WHITE);
        }
    }

    // Draw ground line
    DrawLine(0, GROUND_Y, GetScreenWidth(), GROUND_Y, (Color){139, 90, 43, 255});
}

void background_draw_with_hazards(Background *bg, const void *hazard_list)
{
    // First, draw background normally
    background_draw(bg);

    // Then, draw gaps in the ground for lava pits
    if (hazard_list != NULL)
    {
        // Cast to HazardList
        const HazardList *hazards = (const HazardList *)hazard_list;

        for (int i = 0; i < hazards->count; i++)
        {
            const Hazard *hazard = &hazards->hazards[i];

            if (hazard->active && hazard->type == HAZARD_LAVA_PIT)
            {
                // Calculate screen position of the hazard gap
                float screen_x = hazard->bounds.x - bg->camera.target.x + GetScreenWidth() / 2.0f;
                float gap_width = hazard->bounds.width;

                // Draw a rectangle from GROUND_Y down (erase the ground)
                // This creates a visual gap in the ground
                DrawRectangle(
                    (int)screen_x,
                    GROUND_Y,
                    (int)gap_width,
                    GetScreenHeight() - GROUND_Y,
                    (Color){230, 200, 130, 255} // Mountain color to show through gap
                );

                // Draw edges of the gap for visual clarity
                DrawLine(
                    (int)screen_x,
                    GROUND_Y,
                    (int)screen_x,
                    GetScreenHeight(),
                    (Color){100, 100, 100, 255} // Dark gray edge
                );
                DrawLine(
                    (int)(screen_x + gap_width),
                    GROUND_Y,
                    (int)(screen_x + gap_width),
                    GetScreenHeight(),
                    (Color){100, 100, 100, 255} // Dark gray edge
                );
            }
        }
    }
}

void background_cleanup(Background *bg)
{
    for (int i = 0; i < MAX_CACHED_CHUNKS; i++)
    {
        if (bg->chunks[i].mountain_points)
        {
            free(bg->chunks[i].mountain_points);
        }
    }
    free(bg->chunks);
    free(bg->clouds);
}
