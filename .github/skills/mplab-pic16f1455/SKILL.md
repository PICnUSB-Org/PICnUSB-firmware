---
name: mplab-pic16f1455
description: Configure Microchip MPLAB Extensions in VS Code, develop and build XC8 firmware for the PIC16F1455, and program or troubleshoot the chip over ICSP with a PICkit Basic. Use PICnUSB as the working example.
---

# Configure, build and program a PIC16F1455

Use the guide that matches the task. Paths in the guides start at the firmware workspace root unless stated otherwise.

- [Set up a fresh workspace or repair a build](./references/workspace-and-build.md). Follow the settings from the VS Code project through CMake to the effective XC8 invocation.
- [Write and test firmware](./references/firmware.md). Select a firmware mode, trace the ADC and USB polling loop, and follow a worked change from request through host test and board check.
- [Program a board and diagnose failures](./references/programming.md). Identify the probe, verify the target connection, program the intended image and isolate failures by the last successful operation.

## Configure the VS Code workspace

Open the firmware folder containing `.vscode/PICnUSB.mplab.json`. Select **PICnUSB / default** in the MPLAB Projects view. For a new project, run **MPLAB: Create New Project** and choose the device, compiler and hardware tool for that board. Use **MPLAB: Edit Project Properties (UI)** to inspect or change a configuration. The JSON view exposes the same shared settings. [Microchip extension workflow](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/ides/extensions/workflow/)

Install or enable the Microchip MPLAB Extension Pack. Use the following values to reproduce PICnUSB. Check the installed compiler and device pack as well as the requested versions in the project file. An older PICnUSB README table names the PIC16F1454. The shared configuration selects **PIC16F1455**, which matters for peripherals such as the ADC.

| Project property | PICnUSB value |
| --- | --- |
| Device | `PIC16F1455` |
| Toolchain group and provider | `default-XC8` → `microchip.toolchains:xc8@3.10` |
| Device family pack | `Microchip / PIC12-16F1xxx_DFP / 1.10.270` |
| Tool and tool properties | `PICkit Basic`, entry voltage `low`, programming speed `Min` |
| MPLAB imagePath | `./out/PICnUSB/default.elf` |

Use Ninja and CMake 3.25 or later for the version-6 preset. Let MPLAB discover installed tools. If discovery fails, set the local CMake executable (`cmake.cmakePath`) and Ninja executable (`cmake.configureSettings.CMAKE_MAKE_PROGRAM`) to their paths on this machine. For editor diagnostics, point `mplab.clangd.compileCommandsDir` at `${workspaceFolder}/_build/PICnUSB/default`. Keep executable paths and the selected probe serial number in the ignored local settings.

## Develop and build firmware

- Read `AGENTS.md`, `main.c` and the applicable module under `src/`. For peripheral work, check the PIC16F1455 pin table for the actual package and any shared functions. Use XC8's `<xc.h>` register names. For example, the default firmware samples AN3/RA4 and AN7/RC3. Check analog selection and pin direction before diagnosing ADC or digital input behavior. [Device data sheet and errata](https://www.microchip.com/en-us/product/PIC16F1455)
- Keep configuration words together in `main.c`. The USB modes use 16 MHz HFINTOSC with the 3× PLL, wait for PLL readiness and use USB clock tuning. Preserve USB servicing in the polling loop when adding peripheral work. Keep `LVP = ON` for this board's low-voltage programming setup.
- Register application C files in the file set in `.vscode/PICnUSB.mplab.json`. Both MPLAB for VS Code and MPLAB CLI load the pinned PIC16-RGB dependency from `cmake/PICnUSB/default/user.cmake`.
- On a fresh checkout or after a toolchain or pack change, run **MPLAB CMake: Build** (called **Full Build** in older releases). This generates the machine-local `.generated/toolchain.cmake` used by the preset. If that file is missing, repeat generation through MPLAB. CI also generates these files with `mplab project build`.

After generation, run these PowerShell commands from the firmware root with CMake and Ninja on PATH. Proceed to the build only after configure succeeds.

```powershell
Push-Location cmake/PICnUSB/default
cmake --preset PICnUSB_default_conf
Pop-Location
cmake --build _build/PICnUSB/default
```

Confirm XC8 compiled for `-mcpu=16F1455` in the build output or `_build/PICnUSB/default/compile_commands.json`. A successful link produces `out/PICnUSB/default.elf` for the configured MPLAB project and `out/PICnUSB/default.hex` for workflows that request Intel HEX. Check the build result and use the image from that build.

## Program and verify the board

1. For a requested hardware write, identify the board and probe. The PICkit Basic requires external target power. With power off, check header orientation and connect the five signals below. Pin numbers shown are for the **14-pin** PIC16F1455. Use the package drawing for other packages. [Microchip programming specification, Table 1-1](https://ww1.microchip.com/downloads/en/DeviceDoc/41620C.pdf)

   | PICkit ICSP header pin | Target signal | MCU pin |
   | --- | --- | --- |
   | 1 | MCLR/VPP (RA3) | 4 |
   | 2 | VDD, target voltage sense | 1 |
   | 3 | VSS, common ground | 14 |
   | 4 | ICSPDAT (RC0) | 10 |
   | 5 | ICSPCLK (RC1) | 9 |

2. Apply the board's intended supply. Find the connected PICkit Basic under **MPLAB → Tools** and select its serial number in Project Properties. MPLAB stores this binding in `.vscode/PICnUSB.mplab-local.json`. Check that the selected project and image match the target.
3. Run **MPLAB: Program device** for the selected configuration. Check the MPLAB output for device identification, programming completion and verification. If the ID mismatches or verification fails, stop and diagnose before writing again.
4. Let the target run and test the behavior of the image actually built. For the default ADC firmware, open the enumerated USB CDC port with DTR asserted, send `adc` followed by Enter, and look for a reply such as `TEMP=0512 LIGHT=0768`. Report compilation, programmer verification and the observed runtime result separately.

If the probe itself is missing or its environment cannot be read on Windows, inspect the **MPLAB PICkit Basic ICD** interface and consult the PICkit driver troubleshooting section of the workspace README. If the probe connects but the target ID cannot be read, check target VDD sense, ground, MCLR, data/clock orientation and loads sharing RC0/RC1. Low-voltage entry also depends on the chip's existing LVP bit. An image specifying `LVP = ON` cannot restore low-voltage access to a chip already configured with LVP disabled. Recovery requires a programmer and board that support high-voltage entry. [Microchip programming specification](https://ww1.microchip.com/downloads/en/DeviceDoc/41620C.pdf)
