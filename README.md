# PICnUSB blank firmware

A minimal XC8 application for the PIC16F1455, configured for the Microchip
MPLAB extensions in Visual Studio Code. There are no USB, MCC, or external
library dependencies. The application runs an empty loop at 16 MHz using
HFINTOSC and leaves pins in their reset input state.

## Open and build

1. Open this folder in VS Code and enable the **Microchip MPLAB Extension Pack**.
   Copy [.vscode/settings.example.json](.vscode/settings.example.json) to
   `.vscode/settings.json` for MPLAB clangd and CMake settings. If tool discovery
   fails, set `cmake.cmakePath`, `cmake.configureSettings.CMAKE_MAKE_PROGRAM`
   and `mplab.clangd.path` to the installed executables in that local file.
2. In the MPLAB Projects view, select **PICnUSB / default**. If the project does
   not appear, run **Developer: Reload Window**.
3. Run **MPLAB: Edit Project Properties (UI)** to confirm the device is
   **PIC16F1455**, the compiler is **XC8 3.10**, and the device pack is
   **Microchip/PIC12-16F1xxx_DFP 1.10.270**.
4. Run **MPLAB CMake: Build**. Use **Build (Production)** when offered for a
   standalone image. MPLAB generates its CMake files and machine-local toolchain
   on the first build; CMake and Ninja must be available through MPLAB discovery
   or local VS Code settings.

The configured image is `out/PICnUSB/default.elf`; XC8 also produces the HEX
image for programming. A programmer does not need to be connected to build.

## Add your application

Start in [main.c](./main.c). `_XTAL_FREQ` matches the configured 16 MHz clock
for XC8 delay macros. Add C and header files under `src/`; the project's file
set includes them. Use Project Properties to add other source locations.
Keep persistent CMake customizations in `cmake/PICnUSB/default/user.cmake`
after the first MPLAB build, rather than editing generated files.

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

The root CMake project provides a standalone production build without VS Code
or generated MPLAB files. Install CMake 3.25 or later, Ninja, XC8 3.10 and
Microchip/PIC12-16F1xxx_DFP 1.10.270, then run:

```sh
cmake -S . -B _build/ci -G Ninja -DXC8_CC=/path/to/xc8-cc -DDFP_PATH=/path/to/PIC12-16F1xxx_DFP/1.10.270/xc8
cmake --build _build/ci
```

Replace the two example paths with your installed compiler and pack paths.
The output is `out/ci/PICnUSB/default.elf` and `default.hex`. This build uses
the same device and source file set as the blank MPLAB project. Keep both
configurations aligned when extending the project.

GitHub Actions runs Google's `addlicense` and builds the firmware using the
pinned XC8 and DFP versions. Successful builds upload ELF and HEX artifacts.

## Licence and contributions

Copyright 2026 Maxim Pavlov (MaxEELabs). Licensed under the
[Apache License, Version 2.0](LICENSE). See [NOTICE](NOTICE) for attribution
and [CONTRIBUTING.md](CONTRIBUTING.md) for the DCO and validation steps.
