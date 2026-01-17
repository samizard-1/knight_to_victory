cmake_minimum_required(VERSION 3.10)
project(raylib_game)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add raylib subdirectory (assumes raylib is in vendor/raylib)
add_subdirectory(vendor/raylib)

# Create the executable
if(WIN32)
    set(EXECUTABLE_NAME knight_to_victory)
else()
    set(EXECUTABLE_NAME game)
endif()

add_executable(${EXECUTABLE_NAME}
    src/main.c
    src/game.c
    src/player.c
    src/background.c
    src/level.c
    src/hazard.c
    src/monster.c
    src/projectile.c
    src/pickup.c
    src/asset_paths.c
    src/dragon.c
    src/damage.c
    src/loot.c
    src/level1.c
    src/level2.c
    src/level3.c
    src/level4.c
    src/level5.c
    src/level6.c
    src/level7.c
    src/level8.c
    src/level9.c
    src/level10.c
    src/level11.c
    src/level12.c
    src/level13.c
    src/level14.c
    src/level15.c
    src/level16.c
    src/level17.c
    src/level18.c
    src/level19.c
    src/level20.c
)

# Link raylib
target_link_libraries(${EXECUTABLE_NAME} raylib)

# Include directories
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
)

# Platform-specific settings
if(APPLE)
    target_link_libraries(${EXECUTABLE_NAME} "-framework IOKit")
    target_link_libraries(${EXECUTABLE_NAME} "-framework Cocoa")
    target_link_libraries(${EXECUTABLE_NAME} "-framework OpenGL")
    
    # Create macOS app bundle
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.raylib-game"
        MACOSX_BUNDLE_BUNDLE_NAME "Raylib Game"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_INFO_STRING "A Raylib Game"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_EXECUTABLE_NAME "game"
    )
    
    # Set deployment target for compatibility
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "macOS minimum deployment target")
endif()

# Copy assets to build directory (for both regular and bundle builds)
if(APPLE)
    # For macOS bundles, copy to Contents/Resources
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_BUNDLE_CONTENT_DIR:${EXECUTABLE_NAME}>/Resources/assets
        COMMENT "Copying assets to macOS bundle Resources"
    )
else()
    # For other platforms, copy next to the executable
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
    )
endif()

# Build test executable for loot system
add_executable(test_loot
    tests/test_loot.c
    src/loot.c
    src/asset_paths.c
)

target_link_libraries(test_loot PRIVATE raylib)

target_include_directories(test_loot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
)

# Build memory profiling test
add_executable(test_memory
    tests/test_memory.c
    src/loot.c
    src/asset_paths.c
)

target_link_libraries(test_memory PRIVATE raylib)

target_include_directories(test_memory PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
)

# Enable testing
enable_testing()
add_test(NAME LootSystemTests COMMAND test_loot)
add_test(NAME MemoryProfilingTests COMMAND test_memory)