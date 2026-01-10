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
