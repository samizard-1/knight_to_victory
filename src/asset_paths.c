#include "asset_paths.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>

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
    memset(executable_path, 0, sizeof(executable_path));

#ifdef _WIN32
    GetModuleFileNameA(NULL, executable_path, sizeof(executable_path));
    char *last_slash = strrchr(executable_path, '\\');
    if (last_slash)
        *last_slash = '\0';
#elif defined(__APPLE__)
    uint32_t size = sizeof(executable_path);
    if (_NSGetExecutablePath(executable_path, &size) != 0)
    {
        fprintf(stderr, "DEBUG: _NSGetExecutablePath failed, using fallback\n");
        strcpy(asset_base_path, "assets");
        return;
    }

    fprintf(stderr, "DEBUG: Executable path from _NSGetExecutablePath: %s\n", executable_path);

    char resolved_path[PATH_MAX];
    memset(resolved_path, 0, sizeof(resolved_path));
    if (realpath(executable_path, resolved_path) == NULL)
    {
        fprintf(stderr, "DEBUG: realpath failed, trying without it\n");
        strcpy(resolved_path, executable_path);
    }
    strcpy(executable_path, resolved_path);

    fprintf(stderr, "DEBUG: Resolved executable path: %s\n", executable_path);

    // In a macOS app bundle: /path/to/game.app/Contents/MacOS/game
    // Assets should be at: /path/to/game.app/Contents/Resources/assets

    // Look for Contents/MacOS/ in the path
    char *bundle_marker = strstr(executable_path, "/Contents/MacOS/");
    if (bundle_marker != NULL)
    {
        fprintf(stderr, "DEBUG: Detected app bundle (found /Contents/MacOS/)\n");
        *bundle_marker = '\0';
        snprintf(asset_base_path, sizeof(asset_base_path), "%s/Contents/Resources/assets", executable_path);
        fprintf(stderr, "DEBUG: Bundle assets path set to: %s\n", asset_base_path);
        return;
    }

    // Also look for Contents/MacOS without the trailing slash (just in case)
    bundle_marker = strstr(executable_path, "/Contents/MacOS");
    if (bundle_marker != NULL && strlen(bundle_marker) > strlen("/Contents/MacOS"))
    {
        // Make sure it's actually Contents/MacOS/something
        if (bundle_marker[strlen("/Contents/MacOS")] == '/')
        {
            fprintf(stderr, "DEBUG: Detected app bundle (found /Contents/MacOS)\n");
            *bundle_marker = '\0';
            snprintf(asset_base_path, sizeof(asset_base_path), "%s/Contents/Resources/assets", executable_path);
            fprintf(stderr, "DEBUG: Bundle assets path set to: %s\n", asset_base_path);
            return;
        }
    }

    fprintf(stderr, "DEBUG: Not in a bundle, using standard path detection\n");
    char *last_slash = strrchr(executable_path, '/');
    if (last_slash)
        *last_slash = '\0';
#else
    ssize_t len = readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);
    if (len == -1)
    {
        fprintf(stderr, "DEBUG: readlink failed, using fallback\n");
        strcpy(asset_base_path, "assets");
        return;
    }
    executable_path[len] = '\0';

    char *last_slash = strrchr(executable_path, '/');
    if (last_slash)
        *last_slash = '\0';
#endif

    snprintf(asset_base_path, sizeof(asset_base_path), "%s/assets", executable_path);
    fprintf(stderr, "DEBUG: Final asset base path set to: %s\n", asset_base_path);
}

const char *get_asset_path(const char *filename)
{
    static char full_path[PATH_MAX];

    if (asset_base_path[0] == '\0')
    {
        snprintf(full_path, sizeof(full_path), "assets/%s", filename);
    }
    else
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", asset_base_path, filename);
    }

    fprintf(stderr, "DEBUG: get_asset_path('%s') -> '%s'\n", filename, full_path);

    return full_path;
}
