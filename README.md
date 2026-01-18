# Knight To Victory: Sidescrolling Game using Raylib

By: Samizard

## Project Structure

```
raylib_game/
├── CMakeLists.txt                    # Build configuration
├── include/                          # Header files
│   ├── game.h                       # Game state and functions
│   ├── player.h                     # Player structure and functions
│   ├── background.h                 # Background system
│   ├── level.h                      # Level structure and functions
│   ├── hazard.h                     # Hazard system
│   └── level_definitions.h          # Level creation function declarations
├── src/                              # Source files
│   ├── main.c                       # Entry point
│   ├── game.c                       # Game logic
│   ├── player.c                     # Player logic
│   ├── background.c                 # Background generation
│   ├── level.c                      # Level system implementation
│   ├── hazard.c                     # Hazard system implementation
│   ├── level1.c                     # Level 1 definition
│   ├── level2.c                     # Level 2 definition
│   └── level3.c                     # Level 3 definition
├── assets/                           # Game assets
│   ├── character.png
│   ├── filled_heart.png
│   └── empty_heart.png
└── vendor/                           # External libraries (raylib)
```

## Building

### Prerequisites
- CMake 3.10 or higher
- C99 compiler
- raylib (will be built as part of the project)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build .

# Run the game
./game
```

## Controls

- **A / Left Arrow** - Move left
- **D / Right Arrow** - Move right
- **W / Space / Up Arrow** - Jump
- **ESC** - Exit game

## Features Included

- Basic game loop with update/draw cycle
- Player character with movement and jumping
- Gravity and ground collision
- Screen boundary collision
- FPS counter
- Modular architecture for easy expansion
- Level system with different goal types (location-based and hazard-based)
- Procedurally generated backgrounds with per-level variants
- Heart-based health system with texture-based UI

## Level System

The game uses a modular level system where each level is defined in its own file. This makes it easy to manage level-specific features like backgrounds, goals, and future platforms/hazards.

**For detailed instructions on creating and configuring levels, see [docs/LEVELS.md](docs/LEVELS.md)**

### Quick Overview

- Each level lives in its own file (`src/levelN.c`)
- All level properties are defined together: background, goal, starting position
- Different background variants create different procedurally generated terrain
- Two goal types: reach a location or defeat hazards
- Levels automatically transition when the goal is reached

## Background System

The game features a procedurally generated background system with customizable terrain variants. The system is designed to be extensible for future background types.

**For detailed instructions on customizing backgrounds and adding new background types, see [docs/BACKGROUNDS.md](docs/BACKGROUNDS.md)**

### Quick Overview

- Backgrounds are configured per level
- Each variant number (0, 1, 2, 3, etc.) creates uniquely different terrain
- The system is built to support future extensions: tilemaps, textures, and more
- Terrain generation is deterministic—same variant always produces the same landscape

## Hazard System

The game includes a hazard system for placing obstacles and dangers in levels. Hazards can deal damage and trigger player respawning at the level start.

**For detailed instructions on adding hazards and creating new hazard types, see [docs/HAZARDS.md](docs/HAZARDS.md)**

### Quick Overview

- Lava pits, spikes, and other hazards can be placed in levels
- Each hazard has configurable damage values
- Player respawns at level start when hitting a hazard
- New hazard types can be added by extending the HazardType enum
- Level 1 features a lava pit obstacle as an example

## Loot System

The game includes a comprehensive loot system that handles item drops from defeated monsters. Each monster type can have a custom loot table defining what items drop and with what probability.

### Default Loot System

All monsters use a default loot table unless a custom table is defined:
- **Coins**: 80% drop chance (1 coin per drop)
- **Health Potions**: 30% drop chance
- **Protection Potions**: 15% drop chance
- **Fireballs**: 40% drop chance

### Adding Custom Loot Tables for Specific Monsters

To create a custom loot table for a monster or monster variant, follow these steps:

#### Step 1: Update the Monster Type Identifier
When creating a monster instance in a level file, give it a unique monster type string if you want custom loot. For example, in [src/level4.c](src/level4.c), bat3 uses "bat3" instead of "bat":

```c
Monster bat3 = monster_create(
    400.0f,    // x position
    475.0f,    // y position
    80.0f,     // width
    80.0f,     // height
    4,         // max hearts
    1000.0f,   // patrol left boundary
    1600.0f,   // patrol right boundary
    150.0f,    // patrol speed
    "bat.png", // texture path
    0.35f,     // scale
    "bat3"     // ← Use unique monster type identifier for custom loot
);
```

#### Step 2: Define the Custom Loot Table
Add your custom loot table definition in the `init_loot_system()` function in [src/game.c](src/game.c), after the default table is set:

```c
// Custom loot table for bat3 in level 4
LootTable bat3_table = loot_table_create("bat3", 4);

// Define individual loot drop entries with type, drop_chance, value, and scale
LootItemDef bat3_coin_def = {.type = LOOT_COIN, .drop_chance = 0.95f, .value = 2, .scale = 0.03f};
loot_table_add_item(&bat3_table, bat3_coin_def);

LootItemDef bat3_health_potion_def = {.type = LOOT_HEALTH_POTION, .drop_chance = 0.5f, .value = 1, .scale = 0.05f};
loot_table_add_item(&bat3_table, bat3_health_potion_def);

// ... add more loot items as needed ...

// Register the table with the loot system
loot_system_add_table(&state->loot_system, bat3_table);
```

#### Available Loot Types

From [include/loot.h](include/loot.h), the following loot types are available:
- `LOOT_COIN` - Currency/points
- `LOOT_HEALTH_POTION` - Restore health
- `PROTECTION_POTION` - Special defensive item
- `LOOT_FIREBALL` - Projectile ammunition

#### Configuration Parameters

For each `LootItemDef`:
- **type**: The loot type enum (see above)
- **drop_chance**: Probability of drop (0.0 to 1.0, where 1.0 = 100%)
- **value**: Amount/quantity (e.g., coin count, health points)
- **scale**: Display scale for UI/world rendering

### Example: Custom Loot Table Usage

The bat3 in level 4 demonstrates a complete example with enhanced drop rates to reward players for defeating a tougher boss-like variant:
- 95% chance to drop 2 coins (vs default 80% for 1)
- 50% chance for health potion (vs default 30%)
- 35% chance for protection potion (vs default 15%)
- 70% chance for fireball (vs default 40%)

## Next Steps

You can expand this framework by adding:
- Enemies to defeat
- Platforms and level design elements
- More hazard types (spike traps, moving obstacles, etc.)
- Collectibles
- Animations
- Sound effects
- Particle effects
- More complex gameplay mechanics
- Additional goal types
