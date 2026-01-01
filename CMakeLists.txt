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
    src/asset_paths.c
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
    
    # Create macOS app bundle
    set_target_properties(game PROPERTIES
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
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_BUNDLE_CONTENT_DIR:game>/Resources/assets
        COMMENT "Copying assets to macOS bundle Resources"
    )
else()
    # For other platforms, copy next to the executable
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
    )
endif()