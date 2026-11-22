# PICnUSB firmware

Firmware examples for the PICnUSB board with a 14-pin PIC16F1455. Each example builds as a separate image through its MPLAB configuration.

| Example | MPLAB configuration | Outputs |
| --- | --- | --- |
| [CDC Sense](examples/cdc-sense/) | `default` | `out/PICnUSB/default.hex` and `out/PICnUSB/default.elf` |

## Try CDC Sense

Program the `default` image and connect the board by USB. Open **PICnUSB CDC Sense** or **USB Serial Device** in a terminal with DTR asserted and flow control disabled. Any baud rate works because the example uses native USB. Send `SENSE?` followed by CR, LF, or CRLF.

```text
SENSE?
PIN7=0512 PIN3=0768
```

Each value is a fresh 10-bit ADC count from 0 to 1023, printed as four decimal digits. Pin 7 is RC3/AN7 and pin 3 is RA4/AN3. The ADC uses VDD and VSS as its references. Bridge JP1 for the onboard phototransistor on pin 7 and JP2 for the onboard thermistor on pin 3. [Board schematic](https://github.com/PICnUSB-Org/PICnUSB-hardware/blob/main/electronics/exports/schematic.pdf)

The onboard LED blinks red before the host configures USB, stays red while the serial port is closed, and turns solid green when the host asserts DTR. Both colors use 20% brightness. The LED turns off during USB suspend.

The device replies with `ERR CMD` for an invalid command and `ERR ADC` if the ADC cannot produce a valid reading. All replies end in CRLF.

## Build in VS Code

1. Open the firmware checkout in VS Code and install the **Microchip MPLAB Extension Pack**. Copy [.vscode/settings.example.json](.vscode/settings.example.json) to `.vscode/settings.json` or merge it with your local settings. [Microchip setup guide](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/ides/extensions/get-started/)
2. In the MPLAB Projects view, select **PICnUSB / default**. Verify that Project Properties selects **PIC16F1455**, **XC8 3.10**, and **Microchip/PIC12-16F1xxx_DFP 1.10.270**. Install any missing tools or packs through MPLAB.
3. Run **MPLAB CMake: Build (Production)**. The first configure downloads the pinned PIC16-RGB and MLA USB archives. You can build with the programmer disconnected.

If CMake cannot find Ninja, add its directory to `cmake.configureEnvironment.PATH` in your local VS Code settings. Set `cmake.cmakePath` if MPLAB cannot find CMake.

## Build from the command line

Use Node.js 24 and install the tools and pack declared by the project. CI builds the same project and uploads the ELF and HEX.

```sh
npm install --global @mplab/cli@0.9.6
mplab apps install cmake@4.5.0
mplab apps install ninja@1.13.2
mplab apps install make@4.4.1
mplab apps install --project .vscode/PICnUSB.mplab.json
mplab packs install --project .vscode/PICnUSB.mplab.json
mplab project build .vscode/PICnUSB.mplab.json --configuration default
```

The tracked [CMake hook](cmake/PICnUSB/default/user.cmake) pins both libraries and sets the USB ID. To use local checkouts at the pinned revisions, generate the MPLAB files, then configure from `cmake/PICnUSB/default` with the absolute paths to the libraries.

```sh
cmake --preset PICnUSB_default_conf \
  "-DFETCHCONTENT_SOURCE_DIR_PIC16_RGB:PATH=/absolute/path/to/PIC16-RGB" \
  "-DFETCHCONTENT_SOURCE_DIR_MLA_USB:PATH=/absolute/path/to/mla_usb"
cmake --build ../../../_build/PICnUSB/default
```

CDC Sense uses Microchip's CDC demo ID `04D8:000A` for bench builds. Add `-DPICNUSB_USB_VID=<vid> -DPICNUSB_USB_PID=<pid>` with your assigned values to the CMake configure command to override it. [Microchip USB ID licensing](https://www.microchip.com/usblicensing/)

## Source layout

- `examples/cdc-sense/app/` contains the entry point, CDC session handling, and status LED behavior.
- `examples/cdc-sense/sense/` contains ADC sampling, command parsing, and raw count formatting.
- `examples/cdc-sense/usb/` contains the USB descriptors and MLA configuration.
- `src/board/` contains shared clock and onboard LED drivers.

## Add an example

1. Add `examples/<name>/app/main.c` and group the example's peripheral and protocol code in subdirectories of `examples/<name>/`. Reuse the clock and LED code under [src/board](src/board/).
2. Add a file set to [.vscode/PICnUSB.mplab.json](.vscode/PICnUSB.mplab.json) that selects the new example directory recursively and the files it uses under `src/board/`. Add a matching configuration with its own image path under `out/PICnUSB/`.
3. Put that configuration's dependencies and includes in `cmake/PICnUSB/<configuration>/user.cmake`. Follow the default hook to attach sources, includes, and pinned libraries to the generated compile target.
4. Select the configuration in VS Code or build it with `--configuration <configuration>`. Register its outputs in CI when adding it as a supported example.

## Program and validate

Connect the PICkit Basic to ICSP and power the target. Select the probe serial number in MPLAB Project Properties and run **MPLAB: Program device**. The project uses low-voltage entry and minimum programming speed. Local tool settings, generated CMake files, and build output are ignored.

To verify readings on hardware, send `SENSE?` while changing the light on the phototransistor or warming the thermistor. Check that the corresponding count changes. Close and reopen the terminal to verify the red and green transitions.

Run the parser and ADC count formatting tests with a native C compiler.

```sh
mkdir -p _build
cc -std=c99 -Wall -Wextra -Werror -I examples/cdc-sense/sense \
  tests/cdc_sense_test.c examples/cdc-sense/sense/sense.c -o _build/cdc-sense-test
_build/cdc-sense-test
```

## Licence and contributions

Copyright 2026 Maxim Pavlov (MaxEELabs). Licensed under the [Apache License, Version 2.0](LICENSE). See [NOTICE](NOTICE) for attribution and [CONTRIBUTING.md](CONTRIBUTING.md) for the DCO and validation steps.
