# Monster Customization System

## Overview

The monster system now supports custom behavior functions, allowing you to create specialized monsters with custom drawing, update, and cleanup logic. The dragon in Level 10 serves as the first example.

## How to Customize a Monster

### 1. Create a Custom Monster Header File

Create a new header file in the `include/` directory (e.g., `include/dragon.h`):

```c
#ifndef DRAGON_H
#define DRAGON_H

#include "monster.h"

// Custom functions for the dragon
void dragon_apply_customizations(Monster *dragon);
void dragon_draw_hearts(Monster *dragon, float screen_pos_x, float screen_pos_y);
void dragon_custom_update(Monster *dragon);
void dragon_cleanup(Monster *dragon);

#endif // DRAGON_H
```

### 2. Create a Custom Monster Implementation File

Create the corresponding source file in the `src/` directory (e.g., `src/dragon.c`):

```c
#include "dragon.h"
#include "raylib.h"

// Custom heart drawing function
void dragon_draw_hearts(Monster *dragon, float screen_pos_x, float screen_pos_y)
{
    // TODO: Implement custom heart drawing logic
    // Example customizations:
    // - Draw larger hearts
    // - Use different colors
    // - Add special effects
    // - Draw in different positions
    
    // For now, use default drawing:
    monster_draw_hearts_default(dragon, screen_pos_x, screen_pos_y);
}

// Optional: Custom update logic
void dragon_custom_update(Monster *dragon)
{
    // TODO: Implement custom movement/behavior
}

// Optional: Custom cleanup logic
void dragon_cleanup(Monster *dragon)
{
    // TODO: Clean up any custom resources
}

// Apply all customizations to the monster
void dragon_apply_customizations(Monster *dragon)
{
    dragon->draw_hearts = dragon_draw_hearts;
    // dragon->custom_update = dragon_custom_update;
    // dragon->custom_cleanup = dragon_cleanup;
}
```

### 3. Register Your Custom Monster in CMakeLists.txt

Add your custom monster source file to the CMakeLists.txt:

```cmake
add_executable(game
    # ... other source files ...
    src/dragon.c
    # ... rest of source files ...
)
```

### 4. Use the Custom Monster in Your Level

In your level file (e.g., `level10.c`):

```c
#include "level.h"
#include "dragon.h"

// In your level creation function:
Monster dragon = monster_create(
    800.0f,                       // x position
    350.0f,                       // y position
    70.0f,                        // width
    70.0f,                        // height
    20,                            // max hearts
    650.0f,                       // patrol left boundary
    850.0f,                       // patrol right boundary
    90.0f,                        // patrol speed
    "../assets/dragon.png",       // texture path
    0.5f                          // scale
);

// Apply dragon-specific customizations
dragon_apply_customizations(&dragon);

monster_list_add(&level.monsters, dragon);
```

## Available Customization Points

The `Monster` struct now includes the following function pointers:

### 1. `draw_hearts` - Custom Heart Drawing

```c
typedef void (*MonsterDrawHeartsFunc)(Monster *monster, float screen_pos_x, float screen_pos_y);
```

Use this to customize how health is displayed above the monster.

**Parameters:**
- `monster`: Pointer to the monster
- `screen_pos_x`, `screen_pos_y`: Screen position of the monster

**Default Implementation:**
Call `monster_draw_hearts_default()` which draws standard heart textures.

### 2. `custom_update` - Custom Update Logic

```c
typedef void (*MonsterUpdateFunc)(Monster *monster);
```

Use this to override the default patrol behavior with custom movement or AI logic.

**Parameters:**
- `monster`: Pointer to the monster

**Note:** If `custom_update` is set, it completely replaces the default patrol logic.

### 3. `custom_cleanup` - Custom Resource Cleanup

```c
typedef void (*MonsterCleanupFunc)(Monster *monster);
```

Use this to clean up any custom resources allocated in your monster type.

**Parameters:**
- `monster`: Pointer to the monster

**Note:** The default texture cleanup happens automatically after this function is called.

### 4. `custom_data` - Monster-Specific Data

```c
void *custom_data;
```

Use this to store any custom data specific to your monster type. Cast it to your custom struct as needed.

## Example: Creating a Special Boss Monster

Here's a template for creating a more complex custom monster:

### include/boss_monster.h

```c
#ifndef BOSS_MONSTER_H
#define BOSS_MONSTER_H

#include "monster.h"

typedef struct {
    int phase;
    float phase_timer;
    int attacks_count;
} BossData;

void boss_apply_customizations(Monster *boss);
void boss_draw_hearts(Monster *boss, float screen_pos_x, float screen_pos_y);
void boss_custom_update(Monster *boss);
void boss_cleanup(Monster *boss);

#endif
```

### src/boss_monster.c

```c
#include "boss_monster.h"
#include <stdlib.h>

void boss_draw_hearts(Monster *boss, float screen_pos_x, float screen_pos_y)
{
    // Draw larger health bar or custom hearts
    monster_draw_hearts_default(boss, screen_pos_x, screen_pos_y);
    
    // Add custom drawing here
}

void boss_custom_update(Monster *boss)
{
    BossData *data = (BossData *)boss->custom_data;
    
    // Update phase timer
    data->phase_timer += GetFrameTime();
    
    // Custom movement or attack logic
    if (data->phase_timer > 3.0f) {
        data->phase++;
        data->phase_timer = 0.0f;
    }
}

void boss_cleanup(Monster *boss)
{
    if (boss->custom_data) {
        free(boss->custom_data);
        boss->custom_data = NULL;
    }
}

void boss_apply_customizations(Monster *boss)
{
    // Allocate custom data
    boss->custom_data = malloc(sizeof(BossData));
    BossData *data = (BossData *)boss->custom_data;
    data->phase = 1;
    data->phase_timer = 0.0f;
    data->attacks_count = 0;
    
    // Set custom functions
    boss->draw_hearts = boss_draw_hearts;
    boss->custom_update = boss_custom_update;
    boss->custom_cleanup = boss_cleanup;
}
```

## Best Practices

1. **Always initialize all function pointers** in your `apply_customizations` function, even if set to NULL
2. **Use custom_data carefully** - remember to cast it to the correct type
3. **Call monster_draw_hearts_default()** if you want to combine custom drawing with default hearts
4. **Test your custom functions** with the game before deploying
5. **Document your custom behavior** in comments for future maintenance
6. **Follow the naming convention** of `monster_type_function_name` for consistency

## Existing Examples

- **Dragon (Level 10)**: See [include/dragon.h](../include/dragon.h) and [src/dragon.c](../src/dragon.c)

## Future Enhancements

This system supports adding:
- Custom AI behaviors
- Special attack patterns
- Phase-based mechanics
- Particle effects
- Sound effects
- Visual effects (glowing, pulsing, etc.)
- State machines for complex behaviors
