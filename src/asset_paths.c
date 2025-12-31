#include "asset_paths.h"
#include <stdio.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

static char asset_base_path[PATH_MAX] = {0};

void init_asset_paths(void)
{
    char executable_path[PATH_MAX];

#ifdef _WIN32
    GetModuleFileNameA(NULL, executable_path, sizeof(executable_path));
    // Find the last backslash
    char *last_slash = strrchr(executable_path, '\\');
    if (last_slash)
        *last_slash = '\0';
#elif defined(__APPLE__)
    uint32_t size = sizeof(executable_path);
    if (_NSGetExecutablePath(executable_path, &size) != 0)
    {
        strcpy(asset_base_path, "assets");
        return;
    }
    // Resolve symlinks
    char resolved_path[PATH_MAX];
    if (realpath(executable_path, resolved_path) == NULL)
    {
        strcpy(asset_base_path, "assets");
        return;
    }
    strcpy(executable_path, resolved_path);

    // In a macOS app bundle, the executable is at:
    // /path/to/game.app/Contents/MacOS/game
    // We want: /path/to/game.app/Contents/Resources/assets

    // Check if we're in a bundle by looking for Contents/MacOS in the path
    char *bundle_marker = strstr(executable_path, "/Contents/MacOS/");
    if (bundle_marker != NULL)
    {
        // We're in a bundle - truncate at Contents and use Resources/assets
        *bundle_marker = '\0';
        snprintf(asset_base_path, sizeof(asset_base_path), "%s/Contents/Resources/assets", executable_path);
        return;
    }

    // Not in a bundle - find the last slash and look for assets next to the executable
    char *last_slash = strrchr(executable_path, '/');
    if (last_slash)
        *last_slash = '\0';
#else
    // Linux and other Unix-like systems
    ssize_t len = readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);
    if (len == -1)
    {
        strcpy(asset_base_path, "assets");
        return;
    }
    executable_path[len] = '\0';

    // Find the last slash
    char *last_slash = strrchr(executable_path, '/');
    if (last_slash)
        *last_slash = '\0';
#endif

    // Construct the assets path
    snprintf(asset_base_path, sizeof(asset_base_path), "%s/assets", executable_path);
}

const char *get_asset_path(const char *filename)
{
    static char full_path[PATH_MAX];

    // If asset_base_path is not initialized, try fallback
    if (asset_base_path[0] == '\0')
    {
        snprintf(full_path, sizeof(full_path), "assets/%s", filename);
    }
    else
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", asset_base_path, filename);
    }

    return full_path;
}
