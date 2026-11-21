# Set up MPLAB and trace a build

## Bring up a workstation

1. In VS Code Extensions (`Ctrl+Shift+X`), install **MPLAB Extension Pack** from Microchip. Open the firmware folder itself and look for **PICnUSB** under the MPLAB Activity Bar icon. If project commands are missing, check that **User Interfaces for MPLAB** and the MPLAB CMake extension are enabled in this workspace. [Microchip VS Code setup](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/ides/extensions/get-started/)
2. Install XC8 3.10 and make the pack `Microchip/PIC12-16F1xxx_DFP@1.10.270` available to MPLAB. For a downloaded device pack, use **MPLAB: Install local pack**. The DFP supplies device-specific headers and configuration data. The tool pack controls programmer support. Changing the tool pack cannot supply missing XC8 headers. [XC8 downloads](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers) · [Microchip packs](https://packs.download.microchip.com/)
3. For this checkout, open **MPLAB: Edit Project Properties (UI)** and select **PICnUSB / default**. For an empty folder, use **MPLAB: Create New Project**, choose PIC16F1455 and XC8, then set the programmer and pack in Project Properties. Use the new project's own name in its output paths and presets. A standalone GPIO project needs no MLA, RGB library or MCC configuration. [Microchip project workflow](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/ides/extensions/workflow/)
4. Run **MPLAB CMake: Build** from the Command Palette to generate the configuration and compile. For a standalone image to flash, select **MPLAB CMake: Build (Production)** when offered. Inspect the output for the actual configuration, target and image path. A programmer can remain disconnected throughout this step.

## Find the setting that controls the result

| Question | Where to look | What to check in PICnUSB |
| --- | --- | --- |
| Which compiler did the project request? | `.vscode/PICnUSB.mplab.json` | In `configurations[]`, `toolchain: default-XC8` points to the named `propertyGroups[]` entry whose provider is `microchip.toolchains:xc8@3.10`. The configuration itself holds the device and DFP version. |
| Which probe properties apply? | Same file | Follow `toolProperties: default-tool-base` to the group containing `programoptions.pgmentry.voltage` and `programoptions.pgmspeed`. The connected unit's serial selection is local. |
| Which source files enter the firmware? | `.vscode/PICnUSB.mplab.json` | MPLAB and CI use the project's file set. `cmake/PICnUSB/default/user.cmake` adds the RGB driver. |
| Where does the preset run? | `cmake/PICnUSB/default/CMakePresets.json` | Preset `PICnUSB_default_conf` uses Ninja and writes to `_build/PICnUSB/default`. Its toolchain comes from the adjacent `.generated` directory. |
| Which compiler ran? | Generated toolchain and `compile_commands.json` | Look for the local `xc8-cc` executable, `-mcpu=16F1455`, the DFP path and the feature definitions on the entry for the application source. |
| Why is the editor flagging a valid register? | `.vscode/settings.json` | Point `mplab.clangd.compileCommandsDir` at the active build directory. If needed, select the installed XC8 `xc8-clangd` via `mplab.clangd.path`. Rebuild to refresh the database after a mode or device change. |

For terminal builds, use the preset directory as the source directory. On a machine where MPLAB installed CMake and Ninja outside PATH, the following PowerShell example reads this checkout's existing local settings. Use it only when those settings contain valid absolute executable paths.

```powershell
$settings = Get-Content .vscode/settings.json -Raw | ConvertFrom-Json
$cmakeExe = $settings.'cmake.cmakePath'
$ninjaExe = $settings.'cmake.configureSettings'.CMAKE_MAKE_PROGRAM
Push-Location cmake/PICnUSB/default
try {
    & $cmakeExe --preset PICnUSB_default_conf "-DCMAKE_MAKE_PROGRAM=$ninjaExe"
    if ($LASTEXITCODE -ne 0) { throw 'Firmware configuration failed' }
} finally {
    Pop-Location
}
& $cmakeExe --build _build/PICnUSB/default
```

## Diagnose the first failing stage

- **Missing preset or toolchain file.** Check the working directory for the preset. Check `cmake/PICnUSB/default/.generated/toolchain.cmake` for the toolchain. A fresh checkout acquires that file through MPLAB generation. After moving a checkout, regenerate and inspect the absolute paths in the cache.
- **Ninja unavailable.** Check the configured `CMAKE_MAKE_PROGRAM` path actually exists. An integrated terminal can have a different PATH from the extension. Set the local executable path and rerun configure.
- **First configure fails downloading the PIC16-RGB archive.** Check its declaration in `cmake/PICnUSB/default/user.cmake`. For offline work, configure with `-DFETCHCONTENT_SOURCE_DIR_PIC16_RGB=<absolute RGB root>` using the pinned revision. Keep the archive hash intact.
- **Undefined symbol at link time.** Inspect the compile database for its implementation file and active feature macros. Add an omitted source to the MPLAB project file set, then rebuild.
- **Build succeeds with unexpected firmware behavior.** Inspect the `main.c` compile entry for effective definitions, then match the actual output path against the programmer's selected image. Multiple modes write the same `default.hex` name.
