# GitHub Pages Publishing Workflow

This document explains the GitHub Actions workflow for building and publishing your Knight To Victory game to GitHub Pages as a WebAssembly application.

## Overview

The `publish-web.yml` workflow automatically builds your raylib game to WebAssembly (WASM) and publishes it to GitHub Pages whenever you push changes to the `main` or `master` branch.

## Workflow Components

### 1. **Trigger Events**
The workflow runs automatically on:
- **Pushes to `main` or `master` branches** - When code, assets, or build files change
- **Manual trigger** - Via `workflow_dispatch` button in GitHub Actions tab

### 2. **Key Build Steps**

#### Checkout & Dependencies
- Checks out your repository code with submodules
- Clones the latest raylib library

#### Emscripten Setup
- Installs Emscripten SDK (v3.1.64) - the C/C++ to WebAssembly compiler
- Verifies installation with version checks

#### Ray lib Build
- Compiles raylib for WebAssembly in the raylib directory

#### CMake Configuration
- Uses `emcmake cmake` to configure the build with Emscripten toolchain
- Sets build type to Release for optimized output
- Automatically detects WebAssembly platform

#### Build & Compilation
- Compiles your game code to WebAssembly using Emscripten

#### Asset Handling
- Copies the `assets/` directory to the web output
- Assets are preloaded by Emscripten (see "Asset Loading" section below)
- Copies compiled artifacts (HTML, WASM, JS, Data files)

#### GitHub Pages Deployment
- Uploads the build artifacts to GitHub Pages
- Deploys automatically using GitHub's Pages deployment action

## Directory Structure

The workflow works with your existing project structure:

```
sam_scroll_game/
├── src/
│   ├── main.c              # Updated with Emscripten support
│   ├── minshell.html       # Custom HTML shell for web
│   ├── game.c
│   └── ... (other source files)
├── assets/                 # Game assets - copied to web build
│   ├── character.png
│   ├── filled_heart.png
│   └── ... (other assets)
├── CMakeLists.txt          # Updated with web build support
└── .github/workflows/
    └── publish-web.yml     # This workflow
```

## Code Changes Made

### 1. **CMakeLists.txt Enhancements**
- Added Emscripten platform detection
- Web-specific build options:
  - Preloads assets using `--preload-file` flag
  - Sets HTML output mode
  - Configures WebAssembly options (memory, filesystem, GLFW)
  - Optimization flags for Release builds

### 2. **main.c - Emscripten Integration**
- Added `#include <emscripten/emscripten.h>` for web builds
- Implemented `UpdateDrawFrame()` function for the game loop
- Uses `emscripten_set_main_loop()` for web builds
- Maintains original desktop game loop functionality

### 3. **src/asset_paths.c - Web Asset Handling**
- Added platform detection for web builds
- In web builds, assets path is set to `/assets` (where Emscripten preloads them)
- Maintains compatibility with desktop platforms (Windows, macOS, Linux)

### 4. **src/minshell.html - Custom Web Shell**
- Professional HTML wrapper for the game
- Includes game information and controls
- Status display for loading/errors
- Responsive design for mobile/desktop
- Styled with modern UI elements
- Emscripten Module configuration for canvas and logging

## Asset Loading

### How Assets Work in WebAssembly

1. **During Build**: `--preload-file assets@/assets` flag tells Emscripten to:
   - Include all files from the `assets/` directory
   - Map them to `/assets/` in the virtual filesystem

2. **At Runtime**: 
   - Your code uses `get_asset_path()` to construct paths
   - For web: Returns `/assets/filename.png`
   - For desktop: Returns `executable_dir/assets/filename.png`

3. **Emscripten Filesystem**: 
   - WebAssembly runs in a sandbox
   - Emscripten creates a virtual filesystem
   - Preloaded files are accessible via this filesystem

### Asset File Size Considerations

**Important**: The total size of all preloaded assets is baked into the HTML file. Larger asset directories will increase the initial download size. The current configuration uses:
- Memory: 256MB (can be reduced if needed)
- Stack: Default (sufficient for game logic)

If you add large assets (>50MB total), consider:
1. Optimizing image compression (PNG/WebP)
2. Using lazy loading for non-essential assets
3. Adjusting the `--preload-file` command

## GitHub Pages Setup

### Prerequisites
1. Your repository is public (or GitHub Pages is enabled for private repos)
2. GitHub Pages is enabled in repository settings

### Configuration Steps
1. Go to **Settings** → **Pages**
2. Ensure **Source** is set to **Deploy from a branch**
3. Branch should be **gh-pages** (the workflow creates this)
4. Save settings

### Access Your Game
After successful deployment, your game will be available at:
```
https://<username>.github.io/<repository-name>/
```

For this project:
```
https://<username>.github.io/sam_scroll_game/
```

## Monitoring Builds

### View Workflow Runs
1. Go to **Actions** tab in your repository
2. Select "Build and Publish to GitHub Pages" workflow
3. Click on individual runs to see details

### Common Issues

| Issue | Solution |
|-------|----------|
| Build fails | Check the workflow logs for Emscripten errors |
| Assets not loading | Verify `assets/` folder exists and CMakeLists.txt preload path is correct |
| Game crashes on web | Check browser console (F12) for JavaScript errors |
| Pages not updating | Clear browser cache (Ctrl+Shift+Del) or open in private/incognito mode |

## Customization

### Modify HTML Shell
Edit `src/minshell.html` to change:
- Game title and description
- Control instructions
- UI styling and colors
- Loading messages

### Adjust Build Settings
In `CMakeLists.txt`, modify web build options:
```cmake
"SHELL:-s TOTAL_MEMORY=256MB"      # Increase if game needs more memory
"SHELL:-s ALLOW_MEMORY_GROWTH=1"   # Allow dynamic memory allocation
```

### Change Trigger Conditions
Edit the `on:` section in `publish-web.yml` to trigger on:
- Different branches
- Tag pushes (releases)
- Scheduled builds
- Pull requests

## Performance Considerations

### WebAssembly Optimization
- Release builds are optimized with `-O3` flag
- Emscripten automatically minifies JavaScript
- File sizes are reasonable for web delivery

### Asset Loading
- Assets are embedded in the HTML file for offline play
- Consider the 256MB total memory limit
- Current configuration suitable for moderate-sized games

### Browser Compatibility
Works on:
- Chrome/Chromium (Full support)
- Firefox (Full support)
- Safari (Full support)
- Edge (Full support)

Requires:
- WebAssembly support (available in all modern browsers)
- WebGL for graphics
- HTML5 Canvas

## Troubleshooting

### Build Fails with CMake Error
- Ensure raylib is properly checked out in `vendor/raylib`
- Check that all source files are listed in CMakeLists.txt

### Game Won't Start
- Check browser console for errors (F12)
- Verify assets are in the correct directory
- Test with `--verbose` flag in Emscripten (advanced)

### Missing Assets
- Confirm `assets/` folder is in repository root
- Check file names match exactly (case-sensitive)
- Verify workflow correctly copies assets folder

### Performance Issues
- Profile with Chrome DevTools (about:blank → DevTools → Performance tab)
- Consider disabling debug assertions in Release builds
- Optimize graphics settings in-game

## Security Notes

- WebAssembly is sandboxed in the browser
- No system access (file system, networking restrictions apply)
- Assets are public (don't include sensitive data)
- Game logic runs client-side only

## Advanced: Local Web Development

To test the web build locally:

```bash
# Configure with Emscripten
emcmake cmake -B build -DCMAKE_BUILD_TYPE=Release -S .

# Build
cd build
make

# Serve locally (requires Python 3)
cd ..
python3 -m http.server 8000

# Open http://localhost:8000 in browser
```

## Support

For issues with:
- **Raylib**: Visit [raylib.com](https://www.raylib.com/)
- **Emscripten**: Visit [emscripten.org](https://emscripten.org/)
- **This workflow**: Check the workflow logs in GitHub Actions
