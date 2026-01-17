#ifndef LOOT_H
#define LOOT_H

#include "raylib.h"

// Loot type enumeration - extensible for different item types
typedef enum
{
    LOOT_COIN,          // Currency/points
    LOOT_HEALTH_POTION, // Restore health
    PROTECTION_POTION,  // Special item
    LOOT_FIREBALL,      // Projectile ammunition (formerly PICKUP_FIREBALL)
    LOOT_TYPE_COUNT     // Total number of loot types
} LootType;

// Define maximum items in inventory
#define MAX_INVENTORY_SLOTS 100

// Individual loot item definition (used in loot tables)
typedef struct
{
    LootType type;     // Type of loot item
    float drop_chance; // Probability to drop (0.0 to 1.0)
    int value;         // Amount/quantity (coins, health, etc.)
    Texture2D texture; // Visual representation
    float scale;       // Display scale
} LootItemDef;

// Loot table for a specific monster type
// Contains a list of possible loot drops with their probabilities
typedef struct
{
    LootItemDef *items;       // Array of possible loot items
    int item_count;           // Number of possible loot items
    int capacity;             // Allocated capacity
    const char *monster_type; // Which monster type uses this table (e.g., "skeleton", "dragon")
} LootTable;

// Active loot instance in the world
typedef struct
{
    Vector2 position;   // Current position
    Vector2 velocity;   // Current velocity (for falling animation)
    LootType type;      // Type of loot item
    int value;          // Quantity/value of this item
    Texture2D texture;  // Visual representation
    float scale;        // Display scale
    bool active;        // Whether this loot is still available
    float lifetime;     // Seconds remaining before despawn
    float max_lifetime; // Total lifetime before despawn
    float rotation;     // Rotation for visual effect
    bool on_ground;     // Whether loot has landed on ground
} Loot;

// List container for active loot in the world
typedef struct
{
    Loot *loot;   // Array of active loot items
    int count;    // Current number of active items
    int capacity; // Allocated capacity
} LootList;

// Player inventory system
typedef struct
{
    int counts[LOOT_TYPE_COUNT];              // Count of each loot type
    Texture2D loot_textures[LOOT_TYPE_COUNT]; // Textures for UI display
} Inventory;

// Loot system managing all loot tables across all monsters
typedef struct
{
    LootTable *tables;          // Array of loot tables for different monsters
    int table_count;            // Number of defined loot tables
    int table_capacity;         // Allocated capacity
    LootTable default_table;    // Default loot table for monsters without custom tables
    bool default_table_defined; // Whether default table has been initialized
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
bool inventory_remove_loot(Inventory *inv, LootType type, int value);
void inventory_cleanup(Inventory *inv);
void inventory_draw_ui(const Inventory *inv, int screen_width, int screen_height);

// Loot Drop Mechanics
LootList generate_loot_drops(Vector2 death_position, LootTable *table, const Inventory *inventory);
bool should_loot_drop(float drop_chance);

#endif // LOOT_H
