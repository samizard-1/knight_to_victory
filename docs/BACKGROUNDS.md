# Background System Guide

The game features a flexible background system that currently uses procedurally generated terrain. The system is designed to be extensible, allowing for different background types (tilemaps, textures, etc.) in the future.

## Current Background System

### Procedural Generation

The current background system generates terrain procedurally using sine waves and pseudo-random functions. This creates unique, continuous landscapes based on a seed variant.

**How it works:**
- Each level specifies a `BackgroundConfig` with a `variant` number
- Different variant numbers produce different mountain shapes and terrain patterns
- The generation is deterministic—the same variant always produces the same terrain
- Chunks are generated and cached on-demand for performance

### Background Variants

Background variants are simply integer seed values that modify the procedural generation algorithm:

```c
// Level 1: Flat terrain (variant 0)
BackgroundConfig background = {
    .type = BG_TYPE_PROCEDURAL,
    .variant = 0
};

// Level 2: Hilly terrain (variant 1)
BackgroundConfig background = {
    .type = BG_TYPE_PROCEDURAL,
    .variant = 1
};

// Level 3: Very hilly terrain (variant 2)
BackgroundConfig background = {
    .type = BG_TYPE_PROCEDURAL,
    .variant = 2
};
```

Each variant number (0, 1, 2, 3, etc.) creates a different landscape. Try different numbers to find the right difficulty and aesthetic for your level.

## Customizing Terrain Generation

To modify how terrain is generated, edit [../src/background.c](../src/background.c):

### Terrain Height

In the `generate_mountain_chunk()` function, adjust these values to change terrain shape:

```c
// These control the amplitude and frequency of the sine waves
float noise1 = sinf((x + seed_variant * 100) * 0.005f) * 80;   // Increase 80 for taller mountains
float noise2 = sinf((x + seed_variant * 50) * 0.002f) * 120;   // Increase 120 for more rolling hills
```

- **Larger multipliers** (80, 120) = taller/more dramatic terrain
- **Smaller frequencies** (0.005f, 0.002f) = gentler, rolling slopes
- **Larger frequencies** = frequent sharp changes

### Mountain Base Height

In `background_create_with_variant()`:

```c
bg.mountain_base_height = MOUNTAIN_BASE_HEIGHT;  // Defined in config.h
```

Lower values = higher mountains, higher values = lower terrain.

### Chunk Size

In `background.c`, the chunk size controls how wide each terrain section is:

```c
#define CHUNK_WIDTH 800  // Width of each terrain chunk in pixels
```

Smaller values = more detail but more calculation, larger values = less detail but faster.

## Adding New Background Types

The background system is designed to be extended with new types. To add a new background type:

### 1. Update [../include/level.h](../include/level.h)

Add your new type to the `BackgroundType` enum:

```c
typedef enum {
    BG_TYPE_PROCEDURAL,     // Current procedural terrain
    BG_TYPE_TILEMAP,        // Future: tile-based backgrounds
    BG_TYPE_TEXTURE,        // Future: single/parallax texture backgrounds
    BG_TYPE_CUSTOM          // Future: custom background type
} BackgroundType;
```

### 2. Update [../src/background.c](../src/background.c)

Add handling for your new type in the background creation function:

```c
Background background_create_with_variant(int seed_variant)
{
    Background bg;
    // ... existing initialization ...
    
    // Store the seed variant
    bg.seed_variant = seed_variant;
    
    // In the future, you could check a type and initialize differently:
    // if (type == BG_TYPE_TILEMAP) { ... }
    
    return bg;
}
```

### 3. Add Type-Specific Data

If your new background type needs additional data, extend the `Background` struct in [../include/background.h](../include/background.h):

```c
typedef struct
{
    BackgroundChunk *chunks;
    Cloud *clouds;
    int max_chunks;
    int max_clouds;
    int chunk_size;
    float mountain_base_height;
    int seed_variant;
    // Add your type-specific data here:
    // Texture2D tileset;
    // int tile_size;
    Camera2D camera;
} Background;
```

### 4. Implement Drawing

Modify `background_draw()` in [../src/background.c](../src/background.c) to handle your new type.

## Setting Background Variants in Levels

Each level file specifies its background configuration. To try different variants:

**In [../src/level1.c](../src/level1.c):**
```c
BackgroundConfig background = {
    .type = BG_TYPE_PROCEDURAL,
    .variant = 0  // Change this number to try different terrain
};
```

Try variant values like 0, 1, 2, 3, 5, 10, etc. to find the one you like for each level.

## Performance Considerations

- The procedural system caches terrain chunks to avoid recalculating
- Only visible chunks are rendered
- Cloud rendering is lightweight
- For very long levels, consider breaking into multiple levels with different variants instead of one massive level

## Future Extensions

The system is designed to support:
- **Tilemap backgrounds**: Grid-based tile placement for detailed level design
- **Texture backgrounds**: Static or parallax-scrolling background images
- **Hybrid backgrounds**: Mix of procedural terrain with texture overlays
- **Custom background systems**: Complete custom rendering per level

Each new type would follow the same pattern: add the enum value, extend the struct if needed, and implement the drawing logic.
