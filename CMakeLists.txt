cmake_minimum_required(VERSION 3.10)
project(raylib_game)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add raylib subdirectory (assumes raylib is in vendor/raylib)
add_subdirectory(vendor/raylib)

# Create the executable
add_executable(game
    src/main.c
    src/game.c
    src/player.c
    src/background.c
    src/level.c
    src/hazard.c
    src/monster.c
    src/projectile.c
    src/pickup.c
    src/level1.c
    src/level2.c
    src/level3.c
    src/level4.c
    src/level5.c
    src/level6.c
    src/level7.c
    src/level8.c
)

# Link raylib
target_link_libraries(game raylib)

# Include directories
target_include_directories(game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
)

# Platform-specific settings
if(APPLE)
    target_link_libraries(game "-framework IOKit")
    target_link_libraries(game "-framework Cocoa")
    target_link_libraries(game "-framework OpenGL")
endif()
# Copy assets to build directory
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:game>/assets
    COMMENT "Copying assets to build directory"
)