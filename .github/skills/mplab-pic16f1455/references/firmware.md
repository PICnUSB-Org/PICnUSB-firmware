# Develop PIC16F1455 firmware in XC8

## Select the application

The MPLAB `default` configuration builds CDC Sense from `examples/cdc-sense/`. Its file set selects the example and `src/board/`. The matching CMake hook selects the pinned USB and RGB libraries. When adding an example, give it a separate configuration, file set, CMake hook, and image path. See the [README](../../../../README.md#add-an-example).

## Trace CDC Sense

1. Follow `BoardClockInit()` in `src/board/board.c` for configuration words and 48 MHz startup. CDC Sense enables USB clock tuning before attaching. Keep USB polling in the main loop.
2. Follow `SenseAdcInit()` and `SenseAdcRead()` in `examples/cdc-sense/sense/adc.c` for the inputs. Pin 7 is RC3/AN7 and pin 3 is RA4/AN3 on the 14-pin package. Check TRIS, ANSEL and the reference settings when diagnosing measurements. [PIC16F1455 data sheet and errata](https://www.microchip.com/en-us/product/PIC16F1455)
3. Follow `SenseFeed()` and `SenseFormatReply()` for command framing and raw ADC count formatting. The parser preserves partial lines across USB packets. The serial port must assert DTR to start a session. Closing or reconfiguring it clears the parser state.
4. Follow `StatusLedTasks()` for the status patterns. Timer2 supplies elapsed milliseconds so LED blinking and USB polling can run in the same loop.

## Validate changes

Run the native CDC Sense tests in the README to check parsing and raw ADC count formatting. Build with XC8 3.10 and verify the link map for USB buffer placement and memory usage. For a board test, open the CDC port with DTR asserted and send `SENSE?` followed by Enter. Expect a line such as `PIN7=0512 PIN3=0768`. Vary the sensor inputs and record the ADC counts and LED transitions separately from the build result.
