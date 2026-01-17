#include "loot.h"
#include "asset_paths.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Forward declaration
static Texture2D load_loot_texture(LootType type);

// ============ LOOT SYSTEM FUNCTIONS ============

LootSystem loot_system_create(void)
{
    LootSystem system = {0};
    system.tables = malloc(sizeof(LootTable) * 10);
    system.table_capacity = 10;
    system.table_count = 0;
    system.default_table_defined = false;
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
    loot.velocity = (Vector2){0, -200.0f}; // Move upward initially
    loot.type = type;
    loot.value = value;
    loot.active = true;
    loot.lifetime = 30.0f; // 30 seconds max lifetime
    loot.max_lifetime = 30.0f;
    loot.rotation = 0.0f;
    loot.on_ground = false;

    // Set texture and scale based on type
    if (inventory && type < LOOT_TYPE_COUNT)
    {
        // Use texture from inventory if available
        loot.texture = inventory->loot_textures[type];
    }
    else
    {
        // Fallback to loading texture directly
        loot.texture = load_loot_texture(type);
    }

    switch (type)
    {
    case LOOT_COIN:
        loot.scale = 0.03f;
        break;
    case LOOT_HEALTH_POTION:
        loot.scale = 0.05f;
        break;
    case PROTECTION_POTION:
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

    // Apply camera offset to center on screen (matching other entities)
    float screen_x = loot->position.x - camera_x + GetScreenWidth() / 2.0f;
    float screen_y = loot->position.y;

    // Draw texture if loaded, otherwise draw placeholder circle
    if (loot->texture.id != 0)
    {
        Rectangle source = {0, 0, (float)loot->texture.width, (float)loot->texture.height};
        Rectangle dest = {screen_x, screen_y,
                          loot->texture.width * loot->scale,
                          loot->texture.height * loot->scale};
        Vector2 origin = {dest.width / 2.0f, dest.height / 2.0f};

        DrawTexturePro(loot->texture, source, dest, origin, loot->rotation, WHITE);
    }
    else
    {
        // Fallback placeholder if texture failed to load
        DrawCircle((int)screen_x, (int)screen_y, 8, GOLD);
    }
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

// ============ TEXTURE LOADING FUNCTIONS ============

static Texture2D load_loot_texture(LootType type)
{
    const char *texture_path = NULL;

    switch (type)
    {
    case LOOT_COIN:
        texture_path = "loot_coin.png";
        break;
    case LOOT_HEALTH_POTION:
        texture_path = "health_potion.png"; // May need to create this asset
        break;
    case PROTECTION_POTION:
        texture_path = "protection_potion.png"; // May need to create this asset
        break;
    case LOOT_FIREBALL:
        texture_path = "fireball.png";
        break;
    default:
        return (Texture2D){0};
    }

    return LoadTexture(get_asset_path(texture_path));
}

// ============ INVENTORY FUNCTIONS ============

Inventory inventory_create(void)
{
    Inventory inv = {0};
    // Initialize all counts to 0
    for (int i = 0; i < LOOT_TYPE_COUNT; i++)
    {
        inv.counts[i] = 0;
        // Load texture for this loot type
        inv.loot_textures[i] = load_loot_texture((LootType)i);
    }
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

bool inventory_remove_loot(Inventory *inv, LootType type, int value)
{
    if (type >= LOOT_TYPE_COUNT)
        return false;

    if (inv->counts[type] < value)
        return false;

    inv->counts[type] -= value;
    return true;
}

void inventory_cleanup(Inventory *inv)
{
    // Unload textures if they were loaded
    // TODO: Implement when textures are loaded
}

void inventory_draw_ui(const Inventory *inv, int screen_width, int screen_height)
{
    float icon_size = 60.0f;
    float padding = 15.0f;
    float icons_per_row = 4;
    float row_height = icon_size + 5.0f;

    int icon_index = 0;
    for (int type = 0; type < LOOT_TYPE_COUNT; type++)
    {
        if (inv->counts[type] <= 0)
            continue; // Skip items with 0 count

        // Calculate position
        float col = icon_index % (int)icons_per_row;
        float row = icon_index / icons_per_row;

        float x = screen_width - padding - (col + 1) * icon_size - col * 5 - icon_size / 2;
        float y = padding + (row * row_height);

        // Draw icon background
        DrawRectangle((int)x, (int)y, (int)icon_size, (int)icon_size, LIGHTGRAY);

        // Draw texture if loaded
        if (inv->loot_textures[type].id != 0)
        {
            Rectangle source = {0, 0, (float)inv->loot_textures[type].width,
                                (float)inv->loot_textures[type].height};
            Rectangle dest = {x + 2, y + 2, icon_size - 4, icon_size - 4};
            Vector2 origin = {0, 0};
            DrawTexturePro(inv->loot_textures[type], source, dest, origin, 0, WHITE);
        }
        else
        {
            // Fallback to placeholder color based on type if texture not available
            Color icon_color;
            const char *type_name;

            switch (type)
            {
            case LOOT_COIN:
                icon_color = GOLD;
                type_name = "C";
                break;
            case LOOT_HEALTH_POTION:
                icon_color = RED;
                type_name = "H";
                break;
            case PROTECTION_POTION:
                icon_color = BLUE;
                type_name = "S";
                break;
            case LOOT_FIREBALL:
                icon_color = ORANGE;
                type_name = "F";
                break;
            default:
                icon_color = GRAY;
                type_name = "?";
            }

            // Fill the icon area with type color
            DrawRectangle((int)(x + 2), (int)(y + 2), (int)(icon_size - 4), (int)(icon_size - 4), icon_color);

            // Draw type initial in center
            int text_width = MeasureText(type_name, 20);
            DrawText(type_name, (int)(x + icon_size / 2 - text_width / 2), (int)(y + 10), 20, WHITE);
        }

        // Draw count text
        const char *count_text = TextFormat("%d", inv->counts[type]);
        int count_width = MeasureText(count_text, 14);
        DrawText(count_text, (int)(x + icon_size - count_width - 2),
                 (int)(y + icon_size - 16), 14, BLACK);

        icon_index++;
    }
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

LootList generate_loot_drops(Vector2 death_position, LootTable *table, const Inventory *inventory)
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
                0};
            Vector2 spawn_pos = {
                death_position.x + offset.x,
                death_position.y + offset.y};

            // Pass inventory to loot_create so it gets the proper texture
            Loot loot = loot_create(item_def->type, spawn_pos, item_def->value, inventory);
            loot.scale = item_def->scale;

            loot_list_add(&drops, loot);
        }
    }

    return drops;
}
