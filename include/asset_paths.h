#ifndef ASSET_PATHS_H
#define ASSET_PATHS_H

#include <string.h>
#include <stdlib.h>

// Initialize asset paths based on executable location
void init_asset_paths(void);

// Get the full path to an asset file
const char* get_asset_path(const char* filename);

#endif // ASSET_PATHS_H
