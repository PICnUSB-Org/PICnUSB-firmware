# Develop PIC16F1455 firmware in XC8

## Identify the running application

One MPLAB configuration builds several applications. Read `cmake/PICnUSB/default/user.cmake` for the mode definitions and `main.c` for execution order. Use these explicit option combinations when reproducing a mode.

| Intended application | PICNUSB_ADC_TEST | PICNUSB_DAC_TEST | PICNUSB_LED_DEMO | Observable behavior |
| --- | --- | --- | --- | --- |
| Thermistor and light monitor | ON | OFF | ON | CDC answers `adc` with two raw counts |
| DAC bench test | OFF | ON | ON | Timer2 drives a stepped waveform on RC3, USB stays inactive |
| RGB pattern controller | OFF | OFF | ON | CDC accepts `start` and LED commands |
| Legacy audio controls | OFF | OFF | OFF | USB enumerates as HID |

For example, from `cmake/PICnUSB/default` run the following to restore the default application after a DAC test. Build after configure succeeds.

```powershell
cmake --preset PICnUSB_default_conf -DPICNUSB_ADC_TEST=ON -DPICNUSB_DAC_TEST=OFF -DPICNUSB_LED_DEMO=ON
```

ADC mode forces the effective LED option on and the DAC option off using ordinary CMake variables. Cached values may still show an earlier choice. Inspect the effective `-D` definitions on `main.c` in `_build/PICnUSB/default/compile_commands.json` when diagnosing a discrepancy.

## Trace a peripheral from pin to application

1. Establish the physical pin, alternate functions and active owners. The 14-pin board uses RA4/pin 3 for AN3 and RC3/pin 7 for AN7. RC3 also carries PWM2 or a DAC output in other modes. The ADC branch in `main.c` disables both before initializing the sensors. Check package-specific assignments before porting the circuit. [PIC16F1455 data sheet and errata](https://www.microchip.com/en-us/product/PIC16F1455)
2. Inspect configuration words and startup separately. `#pragma config` selects oscillator and programming defaults. At startup `OSCCON` selects 16 MHz HFINTOSC and the 3× PLL, then firmware waits for `OSCSTATbits.PLLRDY`. Defining `_XTAL_FREQ` supplies a frequency to XC8 delay macros. The oscillator registers and configuration bits determine the actual clock.
3. Inspect `ThermistorAdcInit()` in `src/status/thermistor_adc.c`. It sets the TRIS inputs, enables analog selection, disables the RA4 weak pull-up, selects VDD/VSS references and right justification, and starts AN3 after settling. At 48 MHz the FOSC/64 ADC clock gives about 1.33 µs per ADC clock. Re-evaluate acquisition time if the source impedance or input filter changes.
4. Follow `ThermistorAdcTasks()` and `ThermistorAdcLatest()`. The main loop returns to USB polling while conversion is busy. After completion the ADC task reads ADRESH/ADRESL, switches channel, allows 20 µs for acquisition, and starts again. It publishes the thermistor and light results as a pair once both conversions finish. The command handler reads that snapshot in main context.
5. For digital GPIO work, inspect analog selection, direction and peripheral ownership on that same pin. For an output, preload its idle level in LAT before changing TRIS to output. Read inputs through PORT. Keep RC0/RC1 available to ICSP when attaching peripherals, and check the device pin table for input-only pins.

## Worked change: add a read-only CDC command alias

Suppose the request is to make `sample` return the existing ADC pair in the default firmware. The peripheral driver and transport already provide that data. The edit belongs in the ADC branch of `SerialCommandsFeed()` in `src/usb/serial_commands.c`.

1. Add `strcmp(line, "sample") == 0` alongside the accepted `adc`, `status` and `?` comparisons, inside the existing parenthesized expression gated by `valid`. This preserves the overflow check, case folding, line termination, session reset and existing replies. The six-letter alias fits the current input buffer.
2. In `tests/test_adc_commands.c`, add `ExpectReply("sample\n", "TEMP=0512 LIGHT=0768\r\n");` beside the existing aliases with that fixture. The helper checks response length, contents and the bytes surrounding the output buffer. Existing tests also cover the first-sample wait and rejected commands.
3. From the firmware root use a **native C compiler** for these tests. Keep the host and PIC build directories distinct.

   ```powershell
   cmake -S tests -B _build/pattern-tests
   cmake --build _build/pattern-tests --config Debug
   ctest --test-dir _build/pattern-tests -C Debug -R adc_commands --output-on-failure
   ```

4. Build the ADC firmware with XC8. After a requested board flash, open CDC with DTR asserted and send `sample` followed by Enter. Expect two four-digit fields in the range 0000–1023. Test the format and plausible sensor response on hardware. The exact fixture values belong to the host test.

For a new driver, put its public contract in a self-contained header, keep private state in its C file and register that C file in the appropriate `target_sources` branch of `user.cmake`. Initialize it before the first consumer runs. Keep recurring work bounded so the main loop continues servicing USB.
