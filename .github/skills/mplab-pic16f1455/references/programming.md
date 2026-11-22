# Bring up and verify a PIC16F1455 over ICSP

## Before the first write

Read the device marking and identify the package. Use the ICSP table in the skill entry point for the 14-pin part. For example, probe header pin 4 goes to **MCU pin 10** (RC0/data), and probe header pin 5 goes to **MCU pin 9** (RC1/clock). Locate pin 1 on both connectors before attaching power. For another package, keep the signal names and take physical pin numbers from its drawing. USB D+/D− have a documented legacy programming use with specific entry conditions. Wire a new PICkit connection to the primary RC0/RC1 pair. [Family programming specification, sections 1.2 and 4.2.1](https://ww1.microchip.com/downloads/en/DeviceDoc/41620C.pdf)

With power off, connect MCLR/VPP, target VDD sense, common VSS, data and clock. Check for other circuitry driving the shared lines, especially an I²C bus on RC0/RC1. If the board can be powered through USB or a bench supply, identify which source will power it and avoid connecting independent supplies together. Apply target power and check VDD relative to VSS at the MCU. The PICkit Basic senses this voltage and requires an externally powered board.

## From build output to verified flash

1. Build the intended example. Record the project, active configuration and linked image path from the build output. PICnUSB's project action uses `./out/PICnUSB/default.elf`. An external programming workflow that asks for Intel HEX takes `out/PICnUSB/default.hex`. Older named HEX files can coexist in the same directory.
2. Select the connected unit under **MPLAB → Tools** and inspect Project Properties for **PICkit Basic**, **low** entry and **Min** programming speed. If two probes are present, match the serial selection to the unit wired to the board.
3. Run **MPLAB: Program device** when the task calls for writing that board. Read the output in order: probe connection, device identification, image programming and verification. Capture the first failing message and the last successful stage if programming stops. After verification, release reset or let the programming operation release the target before checking application behavior.
4. If the task specifically requires interactive debugging and the hardware combination supports it, use **Debug: Add Configuration**, select **MPLAB Debugger**, then start that configuration from **Run and Debug**. A debug launch can build and program a debug image. Rebuild the production image for a final standalone flash. [Microchip debug workflow](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/ides/extensions/workflow/)

## Diagnose by the last successful operation

| Observation | Next check and interpretation |
| --- | --- |
| Windows sees a USB device, but MPLAB cannot read the PICkit environment | Check the **MPLAB PICkit Basic ICD** interface in Device Manager. A working probe COM port only establishes that the serial interface enumerated. If the log fails at `Pre:GetFirmwareInfo` with `com.Send returned false`, investigate host-to-probe communication first. If the ICD driver provider is libwdi, the workspace README describes selecting Microchip's signed WinUSB driver for the ICD interface. |
| Probe responds, but target voltage is missing or the ID cannot be read | Check external power and the sense lead at the header, shared ground, header orientation, MCLR and continuity of data and clock. Isolate external loads on RC0/RC1 where the board provides jumpers. Match the configured chip to its marking. |
| Target was previously programmed with low-voltage entry disabled | The existing configuration controls entry. Low-voltage access cannot write a replacement image until access is restored. Consult the family programming specification and board design for compatible high-voltage recovery hardware. |
| An unexpected device ID or a verify mismatch appears | Record the reported ID or failing address and selected device, probe and image. Resolve mismatched targets, unstable supply or shared-pin interference before attempting another write. |
| Programming verifies but no CDC port appears | Confirm the target is running, the USB connection carries data, and the flashed build selected ADC or LED CDC mode. HID mode appears as a HID device. DAC mode stays in its waveform loop. For a CDC build inspect the 48 MHz startup, USB attachment and USB task polling in the selected branch of `main.c`. |
| CDC port opens but the terminal receives nothing | Select the board's CDC port, assert DTR and send a terminated command (CR or LF). The ADC application sends replies on request. LED 1 indicates an open, configured, awake CDC session in ADC mode. |
| Terminal answers `WAIT adc`, `ERR` or `WAIT start` | `WAIT adc` means no complete pair has been published yet. Retry the query, then inspect ADC task progress if it persists. `ERR` means the parser rejected the line. `WAIT start` on a status query identifies the RGB controller's startup gate and points to a different build mode. |

For bench evidence, record the actual identification and verify results from the programmer, followed by one observation appropriate to the running mode. With JP1 bridged, CDC Sense should report a lower pin 7 ADC count as illumination increases.
