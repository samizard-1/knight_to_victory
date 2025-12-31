# macOS and Cross-Platform Build Solutions

## Problems Addressed

### 1. **Asset Path Issues**
**Problem:** The game hardcoded `assets/...` paths, which only work when running from the project root.

**Solution:** Implemented `asset_paths.c` that automatically detects the executable location and finds assets relative to it. This works across all platforms:
- **Windows:** Uses `GetModuleFileNameA()`
- **macOS:** Uses `_NSGetExecutablePath()`
- **Linux:** Uses `/proc/self/exe`

The solution includes a fallback to the relative `assets` path for compatibility.

### 2. **macOS App Bundle & Gatekeeper Issues**
**Problem:** 
- Gatekeeper prevents unsigned apps from running (the "not known as trustworthy" error)
- Simple executable bundles aren't user-friendly

**Solution:** The build system now creates a proper macOS `.app` bundle with:
- Correct bundle structure with `Contents/Resources/` for assets
- Proper Info.plist metadata
- Assets bundled inside the app

Users can now run the app by:
1. Downloading the `.dmg` file
2. Dragging the app to Applications folder
3. Right-clicking → Open (bypasses Gatekeeper for first-time run)

### 3. **Code Signing (Optional Enhancement)**
**If you want to distribute without Gatekeeper warnings:**
1. **Developer Certificate:** Get an Apple Developer account ($99/year)
2. **Sign Locally:** 
   ```bash
   codesign --deep --force --verify --verbose --sign "Developer ID Application" ./build/game.app
   ```
3. **In CI/CD:** Use signing certificates in GitHub Secrets

We haven't automated this yet, but the `.dmg` packaging makes distribution much cleaner.

## Changes Made

### New Files
- `include/asset_paths.h` - Header for asset path management
- `src/asset_paths.c` - Implementation for cross-platform asset finding

### Modified Files
- `CMakeLists.txt` - Added `asset_paths.c`, macOS bundle configuration, proper asset deployment
- `src/main.c` - Added `init_asset_paths()` call on startup
- `.github/workflows/build-release.yml` - Enhanced to:
  - Create `.dmg` files for macOS (professional distribution)
  - Properly bundle assets inside the app
  - Include fallback `.zip` packaging
  - Better error handling

## How It Works

### Asset Loading
When the game starts:
1. `main()` calls `init_asset_paths()` 
2. Detects the executable location
3. Sets up asset base path
4. All asset calls use `get_asset_path("filename")`

### macOS Bundle Structure
```
game.app/
├── Contents/
│   ├── MacOS/
│   │   └── game          (the executable)
│   ├── Resources/
│   │   └── assets/       (game assets)
│   └── Info.plist        (bundle metadata)
```

## Usage

### For End Users

**macOS:**
1. Download `raylib-game-macos.dmg`
2. Double-click to mount the disk image
3. Drag `game.app` to your Applications folder
4. **First launch**: 
   - Try to open the app by double-clicking
   - You'll see a warning that Apple cannot verify the app - close this dialog
   - Go to System Preferences (or System Settings) → Security & Privacy
   - You'll see a message about the game being blocked with an "Allow" or "Open Anyway" button
   - Click the button and enter your password when prompted
   - The app will now launch
5. After the first launch, you can open the app normally by double-clicking

**Note on Gatekeeper warning:** The app shows "Apple cannot verify game is free of malware" because it's not signed with a paid Apple Developer certificate. This is normal for indie/open-source games. The app is safe to run - Gatekeeper is just warning that we haven't paid Apple for code signing verification.

**Windows:**
1. Download `raylib-game-windows.zip`
2. Extract the ZIP file
3. Run `game.exe`

### For Developers (Local Testing)

**Build locally:**
```bash
mkdir build && cd build
cmake .. && cmake --build . --config Release
```

**macOS - Run the app bundle:**
```bash
open build/game.app
```

### macOS - Run the app bundle:
```bash
open build/game.app
```

**Linux/Windows - Run the executable directly:**
```bash
./build/game
```

Assets are now found automatically!

## Gatekeeper & Code Signing

### Why the Warning Appears

When you try to open the game on macOS, you'll see:
> "Apple cannot verify game is free of malware"

This warning appears because the app isn't signed with an Apple Developer certificate. **This is completely normal and safe** for indie/open-source projects.

### How to Open the App Despite the Warning

**Method 1: System Preferences (Most Reliable)**
1. Try to open `game.app` normally by double-clicking
2. You'll see the warning dialog - close it
3. Go to System Preferences (or System Settings on newer macOS) → Security & Privacy
4. You should see a message about the game being blocked, with an "Allow" or "Open Anyway" button
5. Click the button and enter your password when prompted
6. The app will now launch
7. After this, you can launch normally by double-clicking

**Method 2: Right-Click (May not work on all macOS versions)**
1. Go to Applications folder
2. Right-click `game.app`
3. Select "Open"
4. Click "Open" in the confirmation dialog
5. After this, you can launch normally by double-clicking

**Method 3: Terminal (For Advanced Users)**
```bash
xattr -d com.apple.quarantine /Applications/game.app
```
Then double-click the app normally.

### For Developers: Code Signing Options

If you want to distribute without warnings (requires payment):

#### Option 1: Apple Developer Certificate ($99/year)
1. Enroll in [Apple Developer Program](https://developer.apple.com/)
2. Create a Developer ID certificate
3. In GitHub Secrets, add:
   - `APPLE_CERTIFICATE_P12_BASE64` (your certificate in base64)
   - `APPLE_CERTIFICATE_PASSWORD` (certificate password)
4. Uncomment the code signing step in the workflow (can be added on request)
5. Optional: [Notarize the app](https://developer.apple.com/documentation/security/notarizing_macos_software_over_the_internet) for even better user experience

#### Option 2: Ad-hoc Signing (Current Approach)
- App is unsigned, users see Gatekeeper warning
- Users can easily bypass with right-click → Open
- No costs, no complexity
- Perfect for indie projects

#### Option 3: Distribute via Alternative Methods
- Homebrew formula (users install with `brew install`)
- MacPorts
- Direct website with user education about the warning

### If You Want Full Code Signing
1. Enroll in [Apple Developer Program](https://developer.apple.com/)
2. Create signing certificates
3. Add to GitHub Secrets: `APPLE_CERTIFICATE_P12_BASE64` and `APPLE_CERTIFICATE_PASSWORD`
4. Uncomment signing steps in workflow (can be added on request)

### Linux AppImage
If needed for Linux distributions:
- Can create AppImage format for Linux users
- Would require `appimagetool` in workflow
- Separate from macOS (which uses `.dmg`)

### Code of Conduct
Users can now right-click the app → Open on first launch to bypass Gatekeeper without needing certificates, making distribution much simpler.

## Testing

Test locally by:
1. Building with CMake
2. Running the `.app` bundle on macOS
3. Running the executable directly on Windows/Linux
4. Verify assets load in all cases

All asset paths are now platform-independent! 🎉
