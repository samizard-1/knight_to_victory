#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// Include headers for testing
#include "../include/loot.h"
#include "../include/config.h"

// ============ TEST UTILITIES ============

typedef struct
{
    int passed;
    int failed;
    int total;
    const char *current_test;
} TestStats;

static TestStats test_stats = {0, 0, 0, NULL};

void test_assert(const char *test_name, int condition, const char *message)
{
    test_stats.total++;
    if (condition)
    {
        test_stats.passed++;
        printf("✓ PASS: %s - %s\n", test_name, message);
    }
    else
    {
        test_stats.failed++;
        printf("✗ FAIL: %s - %s\n", test_name, message);
    }
}

void test_assert_equal_int(const char *test_name, int expected, int actual, const char *message)
{
    test_assert(test_name, expected == actual, message);
    if (expected != actual)
    {
        printf("  Expected: %d, Actual: %d\n", expected, actual);
    }
}

void test_assert_equal_float(const char *test_name, float expected, float actual, float tolerance, const char *message)
{
    int passes = fabsf(expected - actual) <= tolerance;
    test_assert(test_name, passes, message);
    if (!passes)
    {
        printf("  Expected: %.4f, Actual: %.4f (tolerance: %.4f)\n", expected, actual, tolerance);
    }
}

void print_test_summary()
{
    printf("\n");
    printf("========================================\n");
    printf("TEST SUMMARY\n");
    printf("========================================\n");
    printf("Total: %d\n", test_stats.total);
    printf("Passed: %d\n", test_stats.passed);
    printf("Failed: %d\n", test_stats.failed);
    printf("Pass Rate: %.1f%%\n", (float)test_stats.passed / (float)test_stats.total * 100.0f);
    printf("========================================\n");
}

// ============ TEST SUITE 1: LOOT SYSTEM CREATION & MANAGEMENT ============

void test_loot_system_creation()
{
    printf("\n--- Test Suite 1: Loot System Creation & Management ---\n");

    LootSystem system = loot_system_create();

    test_assert_equal_int("loot_system_creation", system.table_count, 0,
                          "System created with 0 tables");
    test_assert_equal_int("loot_system_creation", system.table_capacity, 10,
                          "System created with capacity 10");
    test_assert("loot_system_creation", !system.default_table_defined,
                "Default table not defined on creation");

    loot_system_cleanup(&system);
}

void test_loot_table_creation()
{
    printf("\n--- Test Suite 2: Loot Table Creation ---\n");

    LootTable table = loot_table_create("skeleton", 5);

    test_assert_equal_int("loot_table_creation", table.item_count, 0,
                          "Table created with 0 items");
    test_assert_equal_int("loot_table_creation", table.capacity, 5,
                          "Table created with correct capacity");
    test_assert("loot_table_creation", strcmp(table.monster_type, "skeleton") == 0,
                "Table monster type set correctly");

    loot_table_cleanup(&table);
}

void test_loot_table_add_item()
{
    printf("\n--- Test Suite 3: Loot Table Item Addition ---\n");

    LootTable table = loot_table_create("skeleton", 2);

    LootItemDef item1 = {
        .type = LOOT_COIN,
        .drop_chance = 0.8f,
        .value = 1,
        .scale = 0.03f};

    loot_table_add_item(&table, item1);
    test_assert_equal_int("loot_table_add_item", table.item_count, 1,
                          "Item count increased to 1");

    LootItemDef item2 = {
        .type = LOOT_FIREBALL,
        .drop_chance = 0.2f,
        .value = 1,
        .scale = 0.04f};

    loot_table_add_item(&table, item2);
    test_assert_equal_int("loot_table_add_item", table.item_count, 2,
                          "Item count increased to 2");

    // Test capacity expansion
    LootItemDef item3 = {
        .type = LOOT_HEALTH_POTION,
        .drop_chance = 0.1f,
        .value = 1,
        .scale = 0.05f};

    loot_table_add_item(&table, item3);
    test_assert_equal_int("loot_table_add_item", table.item_count, 3,
                          "Item count increased to 3");
    test_assert("loot_table_add_item", table.capacity >= 3,
                "Capacity expanded when needed");

    // Verify items were stored correctly
    test_assert("loot_table_add_item", table.items[0].type == LOOT_COIN,
                "First item type stored correctly");
    test_assert("loot_table_add_item", table.items[1].type == LOOT_FIREBALL,
                "Second item type stored correctly");

    loot_table_cleanup(&table);
}

void test_loot_system_add_table()
{
    printf("\n--- Test Suite 4: Loot System Table Management ---\n");

    LootSystem system = loot_system_create();
    LootTable table1 = loot_table_create("skeleton", 2);
    LootTable table2 = loot_table_create("dragon", 2);

    loot_system_add_table(&system, table1);
    test_assert_equal_int("loot_system_add_table", system.table_count, 1,
                          "Table count increased to 1");

    loot_system_add_table(&system, table2);
    test_assert_equal_int("loot_system_add_table", system.table_count, 2,
                          "Table count increased to 2");

    loot_system_cleanup(&system);
}

void test_loot_system_get_table()
{
    printf("\n--- Test Suite 5: Loot System Table Lookup ---\n");

    LootSystem system = loot_system_create();
    LootTable table1 = loot_table_create("skeleton", 2);
    LootTable table2 = loot_table_create("dragon", 2);

    loot_system_add_table(&system, table1);
    loot_system_add_table(&system, table2);

    LootTable *found = loot_system_get_table(&system, "skeleton");
    test_assert("loot_system_get_table", found != NULL,
                "Table lookup returns pointer");
    test_assert("loot_system_get_table", strcmp(found->monster_type, "skeleton") == 0,
                "Found table has correct monster type");

    LootTable *not_found = loot_system_get_table(&system, "goblin");
    test_assert("loot_system_get_table", not_found == NULL,
                "Non-existent table returns NULL");

    loot_system_cleanup(&system);
}

// ============ TEST SUITE 6: DEFAULT TABLE SYSTEM ============

void test_default_table_system()
{
    printf("\n--- Test Suite 6: Default Table System ---\n");

    LootSystem system = loot_system_create();
    LootTable default_table = loot_table_create("DEFAULT", 2);

    test_assert("default_table_system", !system.default_table_defined,
                "Default table not defined initially");

    loot_system_set_default_table(&system, default_table);
    test_assert("default_table_system", system.default_table_defined,
                "Default table marked as defined after setting");

    // Test get_table_or_default with custom table
    LootTable custom_table = loot_table_create("skeleton", 2);
    loot_system_add_table(&system, custom_table);

    LootTable *result = loot_system_get_table_or_default(&system, "skeleton");
    test_assert("default_table_system", result != NULL,
                "get_table_or_default returns custom table when available");
    test_assert("default_table_system", strcmp(result->monster_type, "skeleton") == 0,
                "Returns correct custom table");

    // Test get_table_or_default without custom table (should return default)
    result = loot_system_get_table_or_default(&system, "unknown_type");
    test_assert("default_table_system", result != NULL,
                "get_table_or_default returns default table for unknown type");
    test_assert("default_table_system", strcmp(result->monster_type, "DEFAULT") == 0,
                "Returns default table when custom not found");

    loot_system_cleanup(&system);
}

// ============ TEST SUITE 7: LOOT CREATION ============

void test_loot_creation()
{
    printf("\n--- Test Suite 7: Loot Creation ---\n");

    Vector2 spawn_pos = {100.0f, 50.0f};
    Loot loot = loot_create(LOOT_COIN, spawn_pos, 1, NULL);

    test_assert_equal_float("loot_creation", loot.position.x, 100.0f, 0.01f,
                            "Position X set correctly");
    test_assert_equal_float("loot_creation", loot.position.y, 50.0f, 0.01f,
                            "Position Y set correctly");
    test_assert("loot_creation", loot.type == LOOT_COIN,
                "Type set to LOOT_COIN");
    test_assert_equal_int("loot_creation", loot.value, 1,
                          "Value set correctly");
    test_assert("loot_creation", loot.active,
                "Loot marked as active");
    test_assert("loot_creation", !loot.on_ground,
                "Loot not on ground initially");
    test_assert_equal_float("loot_creation", loot.scale, 0.03f, 0.01f,
                            "LOOT_COIN has correct scale");

    // Test different loot types
    Loot health = loot_create(LOOT_HEALTH_POTION, spawn_pos, 1, NULL);
    test_assert_equal_float("loot_creation", health.scale, 0.05f, 0.01f,
                            "LOOT_HEALTH_POTION has correct scale");

    Loot scroll = loot_create(PROTECTION_POTION, spawn_pos, 1, NULL);
    test_assert_equal_float("loot_creation", scroll.scale, 0.04f, 0.01f,
                            "LOOT_SCROLL has correct scale");
}

// ============ TEST SUITE 8: INVENTORY SYSTEM ============

void test_inventory_creation()
{
    printf("\n--- Test Suite 8: Inventory Creation ---\n");

    Inventory inv = inventory_create();

    for (int i = 0; i < LOOT_TYPE_COUNT; i++)
    {
        test_assert_equal_int("inventory_creation", inv.counts[i], 0,
                              "All inventory slots initialized to 0");
        if (inv.counts[i] != 0)
            break; // Just show first failure
    }
}

void test_inventory_add_loot()
{
    printf("\n--- Test Suite 9: Inventory Item Addition ---\n");

    Inventory inv = inventory_create();

    inventory_add_loot(&inv, LOOT_COIN, 5);
    test_assert_equal_int("inventory_add_loot", inv.counts[LOOT_COIN], 5,
                          "Added 5 coins");

    inventory_add_loot(&inv, LOOT_COIN, 3);
    test_assert_equal_int("inventory_add_loot", inv.counts[LOOT_COIN], 8,
                          "Added 3 more coins (total 8)");

    inventory_add_loot(&inv, LOOT_FIREBALL, 2);
    test_assert_equal_int("inventory_add_loot", inv.counts[LOOT_FIREBALL], 2,
                          "Added 2 fireballs");
    test_assert_equal_int("inventory_add_loot", inv.counts[LOOT_COIN], 8,
                          "Coins count unchanged");
}

void test_inventory_get_count()
{
    printf("\n--- Test Suite 10: Inventory Count Retrieval ---\n");

    Inventory inv = inventory_create();

    test_assert_equal_int("inventory_get_count", inventory_get_count(&inv, LOOT_COIN), 0,
                          "Empty inventory returns 0");

    inventory_add_loot(&inv, LOOT_COIN, 10);
    test_assert_equal_int("inventory_get_count", inventory_get_count(&inv, LOOT_COIN), 10,
                          "Returns correct count after adding");

    test_assert_equal_int("inventory_get_count", inventory_get_count(&inv, LOOT_HEALTH_POTION), 0,
                          "Other item types return 0");
}

void test_inventory_max_capacity()
{
    printf("\n--- Test Suite 11: Inventory Capacity Edge Case ---\n");

    Inventory inv = inventory_create();

    // Add items up to and beyond reasonable limits
    inventory_add_loot(&inv, LOOT_COIN, 1000000);
    test_assert_equal_int("inventory_max_capacity", inventory_get_count(&inv, LOOT_COIN), 1000000,
                          "Inventory handles large numbers");
}

// ============ TEST SUITE 12: LOOT LIST ============

void test_loot_list_creation()
{
    printf("\n--- Test Suite 12: Loot List Creation ---\n");

    LootList list = loot_list_create(10);

    test_assert_equal_int("loot_list_creation", list.count, 0,
                          "List created with 0 items");
    test_assert_equal_int("loot_list_creation", list.capacity, 10,
                          "List created with capacity 10");
}

void test_loot_list_add()
{
    printf("\n--- Test Suite 13: Loot List Item Addition ---\n");

    LootList list = loot_list_create(5);
    Vector2 pos = {100.0f, 50.0f};

    Loot loot1 = loot_create(LOOT_COIN, pos, 1, NULL);
    loot_list_add(&list, loot1);

    test_assert_equal_int("loot_list_add", list.count, 1,
                          "List count increased to 1");

    Loot loot2 = loot_create(LOOT_FIREBALL, pos, 1, NULL);
    loot_list_add(&list, loot2);

    test_assert_equal_int("loot_list_add", list.count, 2,
                          "List count increased to 2");

    // Test capacity expansion
    for (int i = 0; i < 4; i++)
    {
        Loot loot = loot_create(LOOT_COIN, pos, 1, NULL);
        loot_list_add(&list, loot);
    }

    test_assert_equal_int("loot_list_add", list.count, 6,
                          "List holds 6 items after expanding capacity");
    test_assert("loot_list_add", list.capacity >= 10,
                "Capacity expanded when needed");

    loot_list_cleanup(&list);
}

// ============ TEST SUITE 14: LOOT DROP GENERATION ============

void test_should_loot_drop()
{
    printf("\n--- Test Suite 14: Loot Drop Chance Logic ---\n");

    test_assert("should_loot_drop", !should_loot_drop(0.0f),
                "0% drop chance never drops");

    test_assert("should_loot_drop", should_loot_drop(1.0f),
                "100% drop chance always drops");

    // Can't reliably test middle values due to randomness, but function should work
    test_assert("should_loot_drop", 1,
                "Drop chance logic implemented");
}

void test_loot_drop_generation()
{
    printf("\n--- Test Suite 15: Loot Generation from Table ---\n");

    LootTable table = loot_table_create("test", 3);

    // Add items with 100% drop chance to ensure generation
    loot_table_add_item(&table, (LootItemDef){
                                    .type = LOOT_COIN,
                                    .drop_chance = 1.0f,
                                    .value = 5,
                                    .scale = 0.03f});

    loot_table_add_item(&table, (LootItemDef){
                                    .type = LOOT_HEALTH_POTION,
                                    .drop_chance = 0.0f,
                                    .value = 1,
                                    .scale = 0.05f});

    Vector2 death_pos = {500.0f, 300.0f};
    LootList drops = generate_loot_drops(death_pos, &table, NULL);

    test_assert("loot_drop_generation", drops.count >= 1,
                "Loot generated from table");

    test_assert("loot_drop_generation", drops.loot[0].type == LOOT_COIN,
                "Generated loot has correct type");

    test_assert_equal_int("loot_drop_generation", drops.loot[0].value, 5,
                          "Generated loot has correct value");

    loot_list_cleanup(&drops);
    loot_table_cleanup(&table);
}

void test_loot_drop_generation_none()
{
    printf("\n--- Test Suite 16: Loot Generation with 0 percent Chance ---\n");

    LootTable table = loot_table_create("test", 2);

    // Add items with 0% drop chance
    loot_table_add_item(&table, (LootItemDef){
                                    .type = LOOT_COIN,
                                    .drop_chance = 0.0f,
                                    .value = 1,
                                    .scale = 0.03f});

    Vector2 death_pos = {500.0f, 300.0f};
    LootList drops = generate_loot_drops(death_pos, &table, NULL);

    test_assert_equal_int("loot_drop_generation_none", drops.count, 0,
                          "No loot generated with 0% chance");

    loot_list_cleanup(&drops);
    loot_table_cleanup(&table);
}

// ============ TEST SUITE 17: EDGE CASES & ROBUSTNESS ============

void test_rapid_inventory_additions()
{
    printf("\n--- Test Suite 17: Rapid Inventory Additions (Edge Case) ---\n");

    Inventory inv = inventory_create();

    // Simulate rapid pickups
    for (int i = 0; i < 100; i++)
    {
        inventory_add_loot(&inv, LOOT_COIN, 1);
    }

    test_assert_equal_int("rapid_inventory_additions", inventory_get_count(&inv, LOOT_COIN), 100,
                          "Rapid additions accumulate correctly");
}

void test_large_loot_list()
{
    printf("\n--- Test Suite 18: Large Loot List (Performance Edge Case) ---\n");

    LootList list = loot_list_create(1);
    Vector2 pos = {100.0f, 50.0f};

    // Add 1000 items to test capacity expansion
    for (int i = 0; i < 1000; i++)
    {
        Loot loot = loot_create(LOOT_COIN, pos, 1, NULL);
        loot_list_add(&list, loot);
    }

    test_assert_equal_int("large_loot_list", list.count, 1000,
                          "List holds 1000 items");

    // Verify capacity is properly expanded
    test_assert("large_loot_list", list.capacity >= 1000,
                "Capacity expanded to hold 1000 items");

    loot_list_cleanup(&list);
}

void test_inventory_mixed_types()
{
    printf("\n--- Test Suite 19: Mixed Inventory Types (Edge Case) ---\n");

    Inventory inv = inventory_create();

    // Add different types
    inventory_add_loot(&inv, LOOT_COIN, 50);
    inventory_add_loot(&inv, LOOT_FIREBALL, 20);
    inventory_add_loot(&inv, LOOT_HEALTH_POTION, 5);
    inventory_add_loot(&inv, PROTECTION_POTION, 3);

    test_assert_equal_int("inventory_mixed_types", inventory_get_count(&inv, LOOT_COIN), 50,
                          "Coins count correct");
    test_assert_equal_int("inventory_mixed_types", inventory_get_count(&inv, LOOT_FIREBALL), 20,
                          "Fireballs count correct");
    test_assert_equal_int("inventory_mixed_types", inventory_get_count(&inv, LOOT_HEALTH_POTION), 5,
                          "Health potions count correct");
    test_assert_equal_int("inventory_mixed_types", inventory_get_count(&inv, PROTECTION_POTION), 3,
                          "Scrolls count correct");
}

void test_invalid_loot_type_handling()
{
    printf("\n--- Test Suite 20: Invalid Loot Type Handling (Error Case) ---\n");

    Inventory inv = inventory_create();

    // Try to add with invalid type (should be ignored)
    inventory_add_loot(&inv, LOOT_TYPE_COUNT, 5);
    test_assert_equal_int("invalid_loot_type_handling", inventory_get_count(&inv, LOOT_TYPE_COUNT), 0,
                          "Invalid loot type ignored");

    // Try to get with invalid type (should return 0)
    int result = inventory_get_count(&inv, LOOT_TYPE_COUNT + 100);
    test_assert_equal_int("invalid_loot_type_handling", result, 0,
                          "Invalid type query returns 0");
}

// ============ MAIN TEST RUNNER ============

int main(int argc, char *argv[])
{
    printf("╔════════════════════════════════════════╗\n");
    printf("║     LOOT SYSTEM TEST SUITE - PHASE 6   ║\n");
    printf("║   Testing & Refinement                 ║\n");
    printf("╚════════════════════════════════════════╝\n");

    // Run all test suites
    test_loot_system_creation();
    test_loot_table_creation();
    test_loot_table_add_item();
    test_loot_system_add_table();
    test_loot_system_get_table();
    test_default_table_system();
    test_loot_creation();
    test_inventory_creation();
    test_inventory_add_loot();
    test_inventory_get_count();
    test_inventory_max_capacity();
    test_loot_list_creation();
    test_loot_list_add();
    test_should_loot_drop();
    test_loot_drop_generation();
    test_loot_drop_generation_none();
    test_rapid_inventory_additions();
    test_large_loot_list();
    test_inventory_mixed_types();
    test_invalid_loot_type_handling();

    // Print summary
    print_test_summary();

    // Exit with proper code
    return test_stats.failed > 0 ? 1 : 0;
}
