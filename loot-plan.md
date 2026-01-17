# Loot Framework Implementation Plan

## Overview
This plan describes a comprehensive loot system for the raylib game. The system allows:
- Monsters to drop loot items on death with configurable drop probabilities
- Loot items to fall from death position to ground level
- Players to collect loot through collision detection
- An inventory system to track collected loot by type
- Extension of the existing firewall pickup system to be a special case of the loot system

---

## Phase 1: Core Loot Data Structures

### 1.1 New Header File: `include/loot.h`

```c
#ifndef LOOT_H
#define LOOT_H

#include "raylib.h"

// Loot type enumeration - extensible for different item types
typedef enum
{
    LOOT_COIN,           // Currency/points
    LOOT_HEALTH_POTION,  // Restore health
    LOOT_SCROLL,         // Special item
    LOOT_FIREBALL,       // Projectile ammunition (formerly PICKUP_FIREBALL)
    LOOT_TYPE_COUNT      // Total number of loot types
} LootType;

// Define maximum items in inventory
#define MAX_INVENTORY_SLOTS 100

// Individual loot item definition (used in loot tables)
typedef struct
{
    LootType type;           // Type of loot item
    float drop_chance;       // Probability to drop (0.0 to 1.0)
    int value;               // Amount/quantity (coins, health, etc.)
    Texture2D texture;       // Visual representation
    float scale;             // Display scale
} LootItemDef;

// Loot table for a specific monster type
// Contains a list of possible loot drops with their probabilities
typedef struct
{
    LootItemDef *items;      // Array of possible loot items
    int item_count;          // Number of possible loot items
    int capacity;            // Allocated capacity
    const char *monster_type; // Which monster type uses this table (e.g., "skeleton", "dragon")
} LootTable;

// Active loot instance in the world
typedef struct
{
    Vector2 position;        // Current position
    Vector2 velocity;        // Current velocity (for falling animation)
    LootType type;           // Type of loot item
    int value;               // Quantity/value of this item
    Texture2D texture;       // Visual representation
    float scale;             // Display scale
    bool active;             // Whether this loot is still available
    float lifetime;          // Seconds remaining before despawn
    float max_lifetime;      // Total lifetime before despawn
    float rotation;          // Rotation for visual effect
    bool on_ground;          // Whether loot has landed on ground
} Loot;

// List container for active loot in the world
typedef struct
{
    Loot *loot;             // Array of active loot items
    int count;              // Current number of active items
    int capacity;           // Allocated capacity
} LootList;

// Player inventory system
typedef struct
{
    int counts[LOOT_TYPE_COUNT];  // Count of each loot type
    Texture2D loot_textures[LOOT_TYPE_COUNT];  // Textures for UI display
} Inventory;

// Loot system managing all loot tables across all monsters
typedef struct
{
    LootTable *tables;           // Array of loot tables for different monsters
    int table_count;             // Number of defined loot tables
    int table_capacity;          // Allocated capacity
    LootTable default_table;     // Default loot table for monsters without custom tables
    bool default_table_defined;  // Whether default table has been initialized
} LootSystem;

// ============ FUNCTION DECLARATIONS ============

// Loot Table Functions
LootSystem loot_system_create(void);
void loot_system_add_table(LootSystem *system, LootTable table);
void loot_system_set_default_table(LootSystem *system, LootTable table);
LootTable *loot_system_get_table(LootSystem *system, const char *monster_type);
LootTable *loot_system_get_table_or_default(LootSystem *system, const char *monster_type);
void loot_system_cleanup(LootSystem *system);

// Loot Table Helper
LootTable loot_table_create(const char *monster_type, int initial_capacity);
void loot_table_add_item(LootTable *table, LootItemDef item);
void loot_table_cleanup(LootTable *table);

// Active Loot Functions
Loot loot_create(LootType type, Vector2 spawn_pos, int value, const Inventory *inventory);
void loot_update(Loot *loot);
void loot_draw(Loot *loot, float camera_x);
void loot_apply_gravity(Loot *loot);

// Loot List Functions
LootList loot_list_create(int capacity);
void loot_list_add(LootList *list, Loot loot);
void loot_list_cleanup(LootList *list);
void loot_list_update(LootList *list);
void loot_list_draw(LootList *list, float camera_x);

// Inventory Functions
Inventory inventory_create(void);
void inventory_add_loot(Inventory *inv, LootType type, int value);
int inventory_get_count(const Inventory *inv, LootType type);
void inventory_cleanup(Inventory *inv);
void inventory_draw_ui(const Inventory *inv, int screen_width, int screen_height);

// Loot Drop Mechanics
LootList generate_loot_drops(Vector2 death_position, LootTable *table);
bool should_loot_drop(float drop_chance);

#endif // LOOT_H
```

### 1.2 Data Structure Relationships

```
LootSystem
├── LootTable[] (one per monster type)
│   └── LootItemDef[] (possible drops for that monster)
│
Inventory
├── counts[LOOT_TYPE_COUNT] (how many of each item player has)
└── loot_textures[] (for UI display)

LootList (world state)
├── Loot[] (active items on the ground)
```

---

## Phase 2: Core Loot Implementation

### 2.1 Core Implementation File: `src/loot.c`

```c
#include "loot.h"
#include "asset_paths.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============ LOOT TABLE SYSTEM ============

LootSystem loot_system_create(void)
{
    LootSystem system = {0};
    system.tables = malloc(sizeof(LootTable) * 10);
    system.table_capacity = 10;
    system.table_count = 0;
    return system;
}

void loot_system_add_table(LootSystem *system, LootTable table)
{
    if (system->table_count >= system->table_capacity)
    {
        system->table_capacity *= 2;
        system->tables = realloc(system->tables, sizeof(LootTable) * system->table_capacity);
    }
    system->tables[system->table_count++] = table;
}

LootTable *loot_system_get_table(LootSystem *system, const char *monster_type)
{
    for (int i = 0; i < system->table_count; i++)
    {
        if (strcmp(system->tables[i].monster_type, monster_type) == 0)
        {
            return &system->tables[i];
        }
    }
    return NULL;
}

void loot_system_set_default_table(LootSystem *system, LootTable table)
{
    system->default_table = table;
    system->default_table_defined = true;
}

LootTable *loot_system_get_table_or_default(LootSystem *system, const char *monster_type)
{
    // First try to find a custom table for this monster type
    LootTable *custom_table = loot_system_get_table(system, monster_type);
    if (custom_table)
    {
        return custom_table;
    }
    
    // Fall back to default table if defined
    if (system->default_table_defined)
    {
        return &system->default_table;
    }
    
    // No table found
    return NULL;
}

void loot_system_cleanup(LootSystem *system)
{
    for (int i = 0; i < system->table_count; i++)
    {
        loot_table_cleanup(&system->tables[i]);
    }
    
    // Clean up default table
    if (system->default_table_defined)
    {
        loot_table_cleanup(&system->default_table);
    }
    
    free(system->tables);
}

// ============ LOOT TABLE FUNCTIONS ============

LootTable loot_table_create(const char *monster_type, int initial_capacity)
{
    LootTable table = {0};
    table.monster_type = monster_type;
    table.items = malloc(sizeof(LootItemDef) * initial_capacity);
    table.capacity = initial_capacity;
    table.item_count = 0;
    return table;
}

void loot_table_add_item(LootTable *table, LootItemDef item)
{
    if (table->item_count >= table->capacity)
    {
        table->capacity *= 2;
        table->items = realloc(table->items, sizeof(LootItemDef) * table->capacity);
    }
    table->items[table->item_count++] = item;
}

void loot_table_cleanup(LootTable *table)
{
    free(table->items);
    table->items = NULL;
}

// ============ ACTIVE LOOT FUNCTIONS ============

Loot loot_create(LootType type, Vector2 spawn_pos, int value, const Inventory *inventory)
{
    Loot loot = {0};
    loot.position = spawn_pos;
    loot.velocity = (Vector2){0, -200.0f};  // Move upward initially
    loot.type = type;
    loot.value = value;
    loot.active = true;
    loot.lifetime = 30.0f;  // 30 seconds max lifetime
    loot.max_lifetime = 30.0f;
    loot.rotation = 0.0f;
    loot.on_ground = false;

    // Set texture and scale based on type
    // (Implementation depends on asset paths)
    switch (type)
    {
    case LOOT_COIN:
        loot.scale = 0.03f;
        break;
    case LOOT_HEALTH_POTION:
        loot.scale = 0.05f;
        break;
    case LOOT_SCROLL:
        loot.scale = 0.04f;
        break;
    case LOOT_FIREBALL:
        loot.scale = 0.04f;
        break;
    default:
        loot.scale = 0.03f;
    }

    return loot;
}

void loot_apply_gravity(Loot *loot)
{
    if (loot->on_ground)
        return;

    float gravity = 300.0f;
    loot->velocity.y += gravity * GetFrameTime();

    // Check if reached ground level (GROUND_Y from config.h)
    float loot_bottom = loot->position.y;
    if (loot_bottom >= GROUND_Y)
    {
        loot->position.y = GROUND_Y;
        loot->velocity.y = 0;
        loot->on_ground = true;
    }
}

void loot_update(Loot *loot)
{
    if (!loot->active)
        return;

    // Apply physics
    loot_apply_gravity(loot);
    
    // Update position
    loot->position.x += loot->velocity.x * GetFrameTime();
    loot->position.y += loot->velocity.y * GetFrameTime();

    // Update rotation for visual effect
    loot->rotation += 5.0f * GetFrameTime();

    // Update lifetime
    loot->lifetime -= GetFrameTime();
    if (loot->lifetime <= 0.0f)
    {
        loot->active = false;
    }

    // Despawn if goes too far off-screen
    float screen_x = loot->position.x - GetScreenWidth() / 2.0f;
    if (screen_x < -1000.0f || screen_x > GetScreenWidth() + 1000.0f)
    {
        loot->active = false;
    }
}

void loot_draw(Loot *loot, float camera_x)
{
    if (!loot->active)
        return;

    float screen_x = loot->position.x - camera_x;
    float screen_y = loot->position.y;

    // TODO: Get texture from inventory or central texture storage
    // For now, draw a placeholder circle
    DrawCircle((int)screen_x, (int)screen_y, 8, GOLD);
}

// ============ LOOT LIST FUNCTIONS ============

LootList loot_list_create(int capacity)
{
    LootList list = {0};
    list.loot = malloc(sizeof(Loot) * capacity);
    list.capacity = capacity;
    list.count = 0;
    return list;
}

void loot_list_add(LootList *list, Loot loot)
{
    if (list->count >= list->capacity)
    {
        list->capacity *= 2;
        list->loot = realloc(list->loot, sizeof(Loot) * list->capacity);
    }
    list->loot[list->count++] = loot;
}

void loot_list_cleanup(LootList *list)
{
    free(list->loot);
    list->loot = NULL;
}

void loot_list_update(LootList *list)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->loot[i].active)
        {
            loot_update(&list->loot[i]);
        }
    }

    // Compact array to remove inactive items
    int write_idx = 0;
    for (int read_idx = 0; read_idx < list->count; read_idx++)
    {
        if (list->loot[read_idx].active)
        {
            list->loot[write_idx++] = list->loot[read_idx];
        }
    }
    list->count = write_idx;
}

void loot_list_draw(LootList *list, float camera_x)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->loot[i].active)
        {
            loot_draw(&list->loot[i], camera_x);
        }
    }
}

// ============ INVENTORY FUNCTIONS ============

Inventory inventory_create(void)
{
    Inventory inv = {0};
    // Initialize all counts to 0
    for (int i = 0; i < LOOT_TYPE_COUNT; i++)
    {
        inv.counts[i] = 0;
    }
    // Load textures for each loot type
    // TODO: Load from assets
    return inv;
}

void inventory_add_loot(Inventory *inv, LootType type, int value)
{
    if (type >= LOOT_TYPE_COUNT)
        return;
    
    inv->counts[type] += value;
}

int inventory_get_count(const Inventory *inv, LootType type)
{
    if (type >= LOOT_TYPE_COUNT)
        return 0;
    
    return inv->counts[type];
}

void inventory_cleanup(Inventory *inv)
{
    // Unload textures if they were loaded
    // TODO: Implement when textures are loaded
}

void inventory_draw_ui(const Inventory *inv, int screen_width, int screen_height)
{
    // TODO: Draw inventory UI showing all loot items the player has
    // Should display multiple rows of icons with counts
}

// ============ LOOT DROP MECHANICS ============

bool should_loot_drop(float drop_chance)
{
    if (drop_chance <= 0.0f)
        return false;
    if (drop_chance >= 1.0f)
        return true;
    
    float random_value = (float)GetRandomValue(0, 100) / 100.0f;
    return random_value < drop_chance;
}

LootList generate_loot_drops(Vector2 death_position, LootTable *table)
{
    LootList drops = loot_list_create(10);
    
    if (!table)
        return drops;

    // Iterate through all possible loot items in the table
    for (int i = 0; i < table->item_count; i++)
    {
        LootItemDef *item_def = &table->items[i];
        
        // Roll for each item independently
        if (should_loot_drop(item_def->drop_chance))
        {
            // Create loot instance at death position
            // Spread loot slightly horizontally
            Vector2 offset = {
                (float)(GetRandomValue(-50, 50)),
                0
            };
            Vector2 spawn_pos = {
                death_position.x + offset.x,
                death_position.y + offset.y
            };
            
            // TODO: Need inventory to get textures, or store in LootItemDef
            Loot loot = loot_create(item_def->type, spawn_pos, item_def->value, NULL);
            loot.texture = item_def->texture;
            loot.scale = item_def->scale;
            
            loot_list_add(&drops, loot);
        }
    }
    
    return drops;
}
```

---

## Phase 3: Integration with Existing Systems

### 3.1 Modify `include/game.h`

Add loot system to the GameState structure (global, not per-level):

```c
typedef struct
{
    // ... existing fields ...
    
    // NEW: Global loot system (shared across all levels)
    LootSystem loot_system;  // Loot tables for all monster types
    
} GameState;
```

### 3.2 Modify `include/level.h`

Add loot list to the Level structure:

```c
typedef struct Level
{
    // ... existing fields ...
    
    // NEW: Loot items active in this level
    LootList loot;           // Active loot in this level
    
} Level;
```

### 3.3 Modify `include/player.h`

Add inventory to Player structure:

```c
typedef struct
{
    // ... existing fields ...
    
    // NEW: Inventory system
    Inventory inventory;     // All items player has collected
    // Note: projectile_inventory now becomes inventory.counts[LOOT_FIREBALL]
    
} Player;
```

**Migration Strategy:**
- Keep `projectile_inventory` as a legacy field that syncs with `inventory.counts[LOOT_FIREBALL]`
- In `player_update()`, sync: `player->projectile_inventory = player->inventory.counts[LOOT_FIREBALL]`
- This maintains backward compatibility while supporting the new system

### 3.4 Monster Death Integration

When a monster dies in `src/game.c`, use the global loot system:

```c
// In the monster death handling section:
if (monster->hearts <= 0)
{
    monster->active = false;
    
    // NEW: Generate and add loot drops using global loot system
    // Pass monster_type (e.g., "skeleton", "dragon") or NULL to use default
    const char *monster_type = "skeleton";  // Determined by monster type
    LootTable *monster_loot_table = loot_system_get_table_or_default(
        &state->loot_system, 
        monster_type
    );
    
    // Generate loot drops for this monster death
    LootList drops = generate_loot_drops(monster->position, monster_loot_table);
    
    // Add all generated drops to level's loot list
    for (int i = 0; i < drops.count; i++)
    {
        loot_list_add(&current_level->loot, drops.loot[i]);
    }
    
    free(drops.loot);  // Free the temporary list
}
```

### 3.5 Pickup Collision in `src/game.c`

Replace current pickup logic with loot collision:

```c
// Check for loot collisions
for (int i = 0; i < current_level->loot.count; i++)
{
    Loot *loot = &current_level->loot.loot[i];
    if (loot->active)
    {
        Rectangle loot_rect = {
            loot->position.x - 8,
            loot->position.y - 8,
            16,
            16
        };

        if (CheckCollisionRecs(player_rect, loot_rect))
        {
            // Add to player inventory
            inventory_add_loot(&player.inventory, loot->type, loot->value);
            
            // Sync with legacy projectile system
            if (loot->type == LOOT_FIREBALL)
            {
                player.projectile_inventory = player.inventory.counts[LOOT_FIREBALL];
                if (player.projectile_inventory > player.max_projectiles)
                {
                    player.projectile_inventory = player.max_projectiles;
                    player.inventory.counts[LOOT_FIREBALL] = player.max_projectiles;
                }
            }

            // Deactivate loot
            loot->active = false;
        }
    }
}
```

### 3.6 Game Loop Integration

In `game_update()` add loot update call:

```c
void game_update(GameState *state)
{
    // ... existing update code ...
    
    Level *current_level = &state->levels[state->current_level_index];
    
    // NEW: Update all active loot
    loot_list_update(&current_level->loot);
    
    // ... rest of update ...
}
```

In `game_draw()` add loot drawing:

```c
void game_draw(GameState *state)
{
    // ... existing draw code ...
    
    Level *current_level = &state->levels[state->current_level_index];
    
    // NEW: Draw all active loot
    loot_list_draw(&current_level->loot, camera_x);
    
    // Draw updated inventory UI
    inventory_draw_ui(&player.inventory, state->screen_width, state->screen_height);
    
    // ... rest of draw ...
}
```

---

## Phase 4: Monster Type Loot Definition

### 4.1 Create Global Loot Tables by Monster Type

Loot tables should be initialized during game startup in `game_init()` or a dedicated initialization function:

```c
// Example: In game.c during game_init()

void setup_loot_system(GameState *state)
{
    // Create the global loot system
    state->loot_system = loot_system_create();

    // ===== Define Default Loot Table =====
    // This applies to any monster type without a custom table
    LootTable default_table = loot_table_create("DEFAULT", 3);
    
    loot_table_add_item(&default_table, (LootItemDef){
        .type = LOOT_COIN,
        .drop_chance = 0.6f,      // 60% chance to drop
        .value = 1,               // Drop 1 coin
        .scale = 0.03f
    });
    
    loot_table_add_item(&default_table, (LootItemDef){
        .type = LOOT_FIREBALL,
        .drop_chance = 0.1f,      // 10% chance to drop
        .value = 1,               // Drop 1 fireball
        .scale = 0.04f
    });
    
    loot_system_set_default_table(&state->loot_system, default_table);

    // ===== Define Skeleton Loot Table =====
    LootTable skeleton_table = loot_table_create("skeleton", 5);
    
    loot_table_add_item(&skeleton_table, (LootItemDef){
        .type = LOOT_COIN,
        .drop_chance = 0.8f,      // Skeletons drop coins often
        .value = 1,
        .scale = 0.03f
    });
    
    loot_table_add_item(&skeleton_table, (LootItemDef){
        .type = LOOT_FIREBALL,
        .drop_chance = 0.15f,
        .value = 2,
        .scale = 0.04f
    });
    
    loot_table_add_item(&skeleton_table, (LootItemDef){
        .type = LOOT_SCROLL,
        .drop_chance = 0.05f,     // Rare scroll drops
        .value = 1,
        .scale = 0.04f
    });
    
    loot_system_add_table(&state->loot_system, skeleton_table);

    // ===== Define Dragon Loot Table =====
    LootTable dragon_table = loot_table_create("dragon", 6);
    
    loot_table_add_item(&dragon_table, (LootItemDef){
        .type = LOOT_COIN,
        .drop_chance = 0.9f,      // Dragons always drop coins
        .value = 10,              // Bosses drop more coins
        .scale = 0.03f
    });
    
    loot_table_add_item(&dragon_table, (LootItemDef){
        .type = LOOT_FIREBALL,
        .drop_chance = 0.5f,
        .value = 5,               // More ammunition from boss
        .scale = 0.04f
    });
    
    loot_table_add_item(&dragon_table, (LootItemDef){
        .type = LOOT_HEALTH_POTION,
        .drop_chance = 0.3f,
        .value = 1,
        .scale = 0.05f
    });
    
    loot_table_add_item(&dragon_table, (LootItemDef){
        .type = LOOT_SCROLL,
        .drop_chance = 0.2f,
        .value = 1,
        .scale = 0.04f
    });
    
    loot_system_add_table(&state->loot_system, dragon_table);

    // Define other monster types by name: "goblin", "orc", "ghost", etc.
    // All monsters will automatically use default table if their type isn't defined
}
```

### 4.2 Monster Type Identification

When monsters are created, they need a type identifier. Modify monster creation:

```c
// In monster.h - add type field
typedef struct Monster
{
    // ... existing fields ...
    const char *type;  // Monster type identifier ("skeleton", "dragon", etc.)
} Monster;

// In monster_create() - set the type
Monster monster_create(float x, float y, /* ... other params ... */, const char *type)
{
    Monster m;
    // ... existing initialization ...
    m.type = type;  // Store monster type for loot lookup
    return m;
}
```

### 4.3 All Monster Type Definitions

Create loot tables for each unique monster type in your game:

```c
// In setup_loot_system(), define tables for:
// - "skeleton"
// - "goblin"
// - "orc"
// - "ghost"
// - "dragon"
// - "bat"
// - Any other monster types in your game

// Monsters without a specific table will use the default table
```

---

## Phase 5: Inventory UI

### 5.1 Inventory Display Approach

The inventory UI should display:
- Multiple rows of loot icons
- Count for each loot type
- Located in corner or sidebar of the screen
- Similar to existing projectile inventory display

```c
void inventory_draw_ui(const Inventory *inv, int screen_width, int screen_height)
{
    float icon_size = 40.0f;
    float padding = 10.0f;
    float icons_per_row = 4;
    float row_height = icon_size + 10.0f;

    int icon_index = 0;
    for (int type = 0; type < LOOT_TYPE_COUNT; type++)
    {
        if (inv->counts[type] <= 0)
            continue;  // Skip items with 0 count

        // Calculate position
        float col = icon_index % (int)icons_per_row;
        float row = icon_index / icons_per_row;
        
        float x = screen_width - (padding + icon_size * (icons_per_row - col) - 5);
        float y = padding + (row * row_height);

        // Draw icon background
        DrawRectangle((int)x, (int)y, (int)icon_size, (int)icon_size, LIGHTGRAY);
        
        // Draw icon
        if (inv->loot_textures[type].id > 0)
        {
            Rectangle src = {0, 0, 
                           (float)inv->loot_textures[type].width,
                           (float)inv->loot_textures[type].height};
            Rectangle dst = {x, y, icon_size, icon_size};
            DrawTexturePro(inv->loot_textures[type], src, dst, 
                          (Vector2){0, 0}, 0, WHITE);
        }

        // Draw count text
        const char *count_text = TextFormat("%d", inv->counts[type]);
        int text_width = MeasureText(count_text, 14);
        DrawText(count_text, (int)(x + icon_size - text_width - 2), 
                (int)(y + icon_size - 16), 14, BLACK);

        icon_index++;
    }
}
```

---

## Implementation Todo List

### Phase 1: Core Data Structures
- [ ] Create `include/loot.h` with all type definitions and function declarations
- [ ] Review and validate data structure designs
- [ ] Ensure compatibility with existing code patterns

### Phase 2: Core Implementation
- [ ] Create `src/loot.c` with basic implementations
- [ ] Implement loot table management functions
- [ ] Implement active loot list management
- [ ] Implement inventory tracking system
- [ ] Implement gravity physics for falling loot
- [ ] Implement loot drop probability system
- [ ] Test loot creation and updating in isolation

### Phase 3: Integration - Player System
- [ ] Add `inventory` field to `Player` struct
- [ ] Create wrapper functions to maintain backward compatibility with `projectile_inventory`
- [ ] Update `player_create()` to initialize inventory
- [ ] Update `player_cleanup()` if needed
- [ ] Test that existing fireball system still works

### Phase 3: Integration - Level System
- [ ] Add `loot` field to `Level` struct
- [ ] Initialize empty loot list for each level
- [ ] Update level cleanup to cleanup loot list

### Phase 3: Integration - Monster Death
- [ ] Identify all places where monsters die/are removed
- [ ] Add loot drop generation at monster death
- [ ] Test that loot drops appear at monster death position
- [ ] Verify loot falls to ground correctly

### Phase 3: Integration - Collision Detection
- [ ] Remove old pickup collision code
- [ ] Add new loot collision code in game loop
- [ ] Ensure inventory is updated on pickup
- [ ] Test that player can pick up loot
- [ ] Verify inventory count increases

### Phase 3: Integration - Game Loop
- [ ] Initialize global loot system in `game_init()` (call `setup_loot_system()`)
- [ ] Add `loot_list_update()` call in `game_update()` for current level
- [ ] Add `loot_list_draw()` call in `game_draw()` for current level
- [ ] Verify loot despawns after lifetime expires
- [ ] Test that loot doesn't render off-screen

### Phase 4: Monster Type Loot System Setup
- [ ] Add `type` field to `Monster` struct for type identification
- [ ] Update all `monster_create()` calls to include monster type
- [ ] Implement `setup_loot_system()` function in game.c
- [ ] Define default loot table using `loot_system_set_default_table()`
- [ ] Define loot tables for each unique monster type in game
- [ ] Implement `loot_system_get_table_or_default()` function
- [ ] Test that monsters without custom tables use default
- [ ] Set drop chances and values based on game balance
- [ ] Test different loot drop combinations
- [ ] Adjust values based on playtest feedback

### Phase 5: Inventory UI
- [ ] Implement basic `inventory_draw_ui()` function
- [ ] Create placeholder icons for each loot type
- [ ] Test UI layout and positioning
- [ ] Add icons/assets for each loot type
- [ ] Polish UI appearance and readability

### Phase 6: Testing & Refinement
- [x] Full playtest of loot drop system
- [x] Verify all collision detection works
- [x] Check inventory updates correctly
- [x] Test edge cases (max inventory, rapid pickups, etc.)
- [x] Balance loot drop rates and values
- [x] Memory profiling to ensure no leaks
- [x] Performance testing with many active loot items

✓ **PHASE 6 COMPLETE** - See docs/PHASE6_FINAL_REPORT.md for full results

### Phase 7: Future Extensions
- [ ] Add loot rarity system (common, rare, legendary)
- [ ] Implement loot type-specific behavior (e.g., healing potions auto-use)
- [ ] Add particle effects for loot drops
- [ ] Sound effects for loot pickup
- [ ] Loot shop/store system
- [ ] Character equipment system
- [ ] Quest rewards as special loot

---

## Code Examples: Monster-Specific Loot Tables

### Example 1: Default Loot Table (for any monster type without a custom table)

```c
// During setup_loot_system() in game.c
LootTable default_table = loot_table_create("DEFAULT", 3);

loot_table_add_item(&default_table, (LootItemDef){
    .type = LOOT_COIN,
    .drop_chance = 0.6f,   // Common
    .value = 1,
    .scale = 0.03f
});

loot_table_add_item(&default_table, (LootItemDef){
    .type = LOOT_FIREBALL,
    .drop_chance = 0.1f,   // Uncommon
    .value = 1,
    .scale = 0.04f
});

// Register as the default table
loot_system_set_default_table(&state->loot_system, default_table);
```

### Example 2: Skeleton Loot Table

```c
LootTable skeleton_table = loot_table_create("skeleton", 5);

loot_table_add_item(&skeleton_table, (LootItemDef){
    .type = LOOT_COIN,
    .drop_chance = 0.8f,   // Very common
    .value = 1,
    .scale = 0.03f
});

loot_table_add_item(&skeleton_table, (LootItemDef){
    .type = LOOT_FIREBALL,
    .drop_chance = 0.15f,  // Uncommon
    .value = 1,
    .scale = 0.04f
});

loot_table_add_item(&skeleton_table, (LootItemDef){
    .type = LOOT_SCROLL,
    .drop_chance = 0.05f,  // Rare
    .value = 1,
    .scale = 0.04f
});

// Register in the global loot system
loot_system_add_table(&state->loot_system, skeleton_table);
```

### Example 3: Dragon Loot Table (Boss)

```c
LootTable dragon_table = loot_table_create("dragon", 6);

loot_table_add_item(&dragon_table, (LootItemDef){
    .type = LOOT_COIN,
    .drop_chance = 0.9f,   // Boss always drops coins
    .value = 10,           // More coins from boss
    .scale = 0.03f
});

loot_table_add_item(&dragon_table, (LootItemDef){
    .type = LOOT_FIREBALL,
    .drop_chance = 0.5f,   // Likely drop
    .value = 5,            // Many fireballs from boss
    .scale = 0.04f
});

loot_table_add_item(&dragon_table, (LootItemDef){
    .type = LOOT_HEALTH_POTION,
    .drop_chance = 0.3f,   // Occasional healing
    .value = 1,
    .scale = 0.05f
});

loot_table_add_item(&dragon_table, (LootItemDef){
    .type = LOOT_SCROLL,
    .drop_chance = 0.2f,   // Boss treasure
    .value = 1,
    .scale = 0.04f
});

// Register in the global loot system
loot_system_add_table(&state->loot_system, dragon_table);
```

### Example 4: Using the Global System During Monster Death

When a monster of a specific type dies, look up its custom table or use the default:

```c
// In game_update(), when monster->hearts <= 0
const char *monster_type = monster->type;  // e.g., "skeleton", "dragon"

LootTable *monster_loot_table = loot_system_get_table_or_default(
    &state->loot_system, 
    monster_type
);

if (monster_loot_table)
{
    LootList drops = generate_loot_drops(monster->position, monster_loot_table);
    
    for (int i = 0; i < drops.count; i++)
    {
        loot_list_add(&current_level->loot, drops.loot[i]);
    }
    
    free(drops.loot);
}
```

---

## Technical Considerations

### Memory Management
- Loot lists are dynamically allocated with doubling capacity
- Inactive loot items are removed from arrays each frame (compaction)
- Each level maintains its own loot system and list
- Clear loot when transitioning levels

### Performance
- Collision checks only for active loot items
- Lifetime-based despawning prevents unbounded growth
- Off-screen despawning saves draw calls
- Reasonable physics timestep for gravity

### Extensibility
- LootType enum can be extended without breaking existing code
- LootTable system allows per-monster-type customization
- Inventory array-based for efficient lookup
- Easy to add new loot behaviors in future

### Backward Compatibility
- Keep `projectile_inventory` field in Player
- Sync with `inventory.counts[LOOT_FIREBALL]`
- Existing pickup system becomes a special case of loot
- Game logic continues to work without changes

---

## Asset Requirements

### Textures Needed
- `loot_coin.png` - Currency item
- `loot_health_potion.png` - Healing item
- `loot_scroll.png` - Special item
- (Reuse `fireball.png` for LOOT_FIREBALL type)

### Optional Enhancements
- Particle effects on loot drop
- Glow/animation for rare items
- Pickup sound effects
- Floating text showing pickup ("+10 coins")

---

## Summary

This loot framework provides:

1. **Flexible Definition** - Loot tables per monster type
2. **Dynamic Spawning** - Probability-based drops on death
3. **Physics-based Movement** - Loot falls to ground realistically
4. **Inventory Tracking** - Player collects and stores items
5. **Extensible Design** - Easy to add new item types
6. **Backward Compatible** - Works with existing fireball system
7. **Scalable** - Handles many items without performance issues

The implementation can be done incrementally in phases, allowing for testing and refinement at each stage.
