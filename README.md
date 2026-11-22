# PICnUSB firmware

A PIC16F1455 firmware project for MPLAB in Visual Studio Code. The default
application uses PIC16-RGB to blink one WS2812 LED on RA5 (pin 2) at 20% red
brightness, with 500 ms on and 500 ms off. It uses the internal oscillator
and the 3× PLL to supply the driver's required 48 MHz clock.

## Set up a fresh checkout in VS Code

1. **Open the checkout.** In VS Code, use **File > Open Folder** and select the
   folder containing `.vscode/PICnUSB.mplab.json`. Install the
   **Microchip MPLAB Extension Pack** from Extensions. [Microchip setup guide](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/ides/extensions/get-started/)
2. **Set up the workspace.** Copy [.vscode/settings.example.json](.vscode/settings.example.json)
   to `.vscode/settings.json`. If you already have a local settings file, merge
   the example settings into it. In the MPLAB Projects view, select
   **PICnUSB / default**. If it is missing, run **Developer: Reload Window**.
3. **Install the compiler and device pack.** Make **MPLAB XC8 3.10** available
   on your machine. In **MPLAB: Edit Project Properties (UI)**, verify that
   **default** selects **PIC16F1455**, **XC8 3.10**, and
   **Microchip/PIC12-16F1xxx_DFP 1.10.270**. If the pack is missing, install that
   version from the MPLAB Packs view or download it and run **MPLAB: Install
   local pack**. [XC8 downloads](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-compilers) · [Microchip device packs](https://packs.download.microchip.com/)
4. **Generate and build.** Run **MPLAB CMake: Build (Production)** from the
   Command Palette. Older extensions label it **Full Build (Production)**.
   MPLAB needs CMake and Ninja and can discover them through its application
   manager. The first configure downloads the pinned PIC16-RGB archive, so
   it needs network access or the local override described below.
5. **Check the outputs.** A successful build creates
   `out/PICnUSB/default.elf` and `out/PICnUSB/default.hex`. MPLAB also creates
   `_build/PICnUSB/default/compile_commands.json` for clangd. You can build
   with the programmer disconnected.

If VS Code reports that it cannot find Ninja, add the directory containing
`ninja.exe` to CMake's configure environment in your local `.vscode/settings.json`.
Set `cmake.cmakePath` as well if CMake discovery fails. Replace the example
paths with your installed locations.

```json
{
  "cmake.cmakePath": "C:/path/to/cmake/bin/cmake.exe",
  "cmake.configureEnvironment": {
    "PATH": "C:/path/to/ninja;${env:PATH}"
  }
}
```

## Configure external libraries

The tracked [user.cmake](cmake/PICnUSB/default/user.cmake) is where MPLAB
loads firmware libraries for both VS Code and CI. It declares PIC16-RGB and
MLA USB with pinned archive URLs and SHA-256 hashes. The default firmware
fetches and uses PIC16-RGB. MLA USB is declared for future integration.

### Change the RGB LED configuration

1. Open `cmake/PICnUSB/default/user.cmake` and find `PIC16_RGB_CHAINS`. Its
   default value `Main:RA5:1` defines the chain name, data GPIO, and maximum
   number of LEDs. Separate multiple chain entries with semicolons.
2. Edit the chain entry to match your board. If you change the name, use the
   corresponding `kWs2812Chain<Name>` in [main.c](main.c). Pass the actual LED
   count to `Ws2812Init()`. It must fit within the configured capacity.
3. Run **MPLAB CMake: Clean and build (Production)**. If the cache still holds
   the previous value, run
   `cmake --preset PICnUSB_default_conf -U PIC16_RGB_CHAINS` from
   `cmake/PICnUSB/default` to reload the default, then rebuild.

### Use a local library checkout or build offline

1. Clone the library or extract its archive. For a reproducible offline
   build, use the revision pinned in `user.cmake`. For PIC16-RGB, the local
   source directory must contain its `CMakeLists.txt`.
2. Generate the MPLAB CMake files once. Run **MPLAB CMake: Build (Production)**
   if this is a fresh checkout. You can continue once
   `cmake/PICnUSB/default/CMakePresets.json` and
   `cmake/PICnUSB/default/.generated/toolchain.cmake` exist, even if the
   archive download failed.
3. From the repository root, run the following with CMake and Ninja on your
   terminal's PATH. Use an absolute path to your library directory.

   ```sh
   cd cmake/PICnUSB/default
   cmake --preset PICnUSB_default_conf "-DFETCHCONTENT_SOURCE_DIR_PIC16_RGB:PATH=/absolute/path/to/PIC16-RGB"
   cmake --build ../../../_build/PICnUSB/default
   ```

4. To return to the pinned download, run
   `cmake --preset PICnUSB_default_conf -U FETCHCONTENT_SOURCE_DIR_PIC16_RGB`
   from the same directory, then rebuild. The override is stored in the local
   build cache. [CMake FetchContent overrides](https://cmake.org/cmake/help/latest/module/FetchContent.html#variable:FETCHCONTENT_SOURCE_DIR_%3CuppercaseName%3E)

### Add or update a library

1. In `user.cmake`, add or update its `FetchContent_Declare()` entry. For
   archive dependencies, use an immutable release or commit and update the
   URL and its SHA-256 hash together.
2. Set any library options before calling `FetchContent_MakeAvailable()` with
   the dependency name. For a library that exports a CMake target, add that
   target to `target_link_libraries(PICnUSB_default_default_XC8_compile PRIVATE ...)`.
   PIC16-RGB's `PIC16RGB::ws2812` target supplies its sources and include paths.
   For a source-only library, add the required source files and include
   directories to that compile target with `target_sources()` and
   `target_include_directories()`. [CMake FetchContent guide](https://cmake.org/cmake/help/latest/module/FetchContent.html#typical-case)
3. Rebuild under MPLAB and check the build output or `compile_commands.json`
   for the expected sources and include paths. Commit the changes to
   `user.cmake` so CI uses the same dependency configuration.

## Add your application

Start in [main.c](main.c). The PIC16-RGB header supplies `_XTAL_FREQ` for
XC8 delay macros. Add C and header files under `src/` to use the existing
file set. Use Project Properties to add other source locations. Keep CMake
customizations in `cmake/PICnUSB/default/user.cmake` so they survive
MPLAB regeneration.

USB use requires additional clock and peripheral initialization and a USB stack.

## Program a board

The project selects **PICkit Basic**, low-voltage entry and minimum programming
speed. `LVP = ON` preserves low-voltage programming. Supply external power to
the target, connect ICSP, and select your connected probe's serial number in
Project Properties before running **MPLAB: Program device**. Probe selection is
stored in the ignored local project settings.

Local VS Code settings, build output and generated toolchain files are ignored
by Git so compiler paths and probe bindings stay specific to your machine.

## Command-line and CI build

GitHub Actions builds the same `.vscode/PICnUSB.mplab.json` project as VS Code.
To run the headless build locally, use Node.js 24 and install the tools
through MPLAB CLI:

```sh
npm install --global @mplab/cli@0.9.6
mplab apps install cmake@4.5.0
mplab apps install ninja@1.13.2
mplab apps install make@4.4.1
mplab apps install --project .vscode/PICnUSB.mplab.json
mplab packs install --project .vscode/PICnUSB.mplab.json
mplab project build .vscode/PICnUSB.mplab.json --configuration default
```

The project JSON defines the application source list, device, compiler, and
device pack. `cmake/PICnUSB/default/user.cmake` adds the pinned RGB driver and
sets its LED pin. MPLAB generates the other CMake files. GitHub Actions
uploads `out/PICnUSB/default.elf` and `out/PICnUSB/default.hex`.

## Licence and contributions

Copyright 2026 Maxim Pavlov (MaxEELabs). Licensed under the
[Apache License, Version 2.0](LICENSE). See [NOTICE](NOTICE) for attribution
and [CONTRIBUTING.md](CONTRIBUTING.md) for the DCO and validation steps.
