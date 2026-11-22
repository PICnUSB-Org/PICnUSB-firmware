# Contributing to PICnUSB Firmware

## Licence

This repository is licensed under the [Apache License, Version 2.0](LICENSE).
By submitting a pull request, patch, or other contribution, you confirm that
you have the right to submit it under that licence.

Keep existing copyright and licence notices. Do not add third-party code
without recording its source, licence and required attribution.

## Developer Certificate of Origin (DCO)

As in the [hardware project](https://github.com/PICnUSB-Org/PICnUSB-hardware/blob/main/CONTRIBUTING.md),
every commit in a pull request must include a `Signed-off-by` trailer under
the [Developer Certificate of Origin](https://developercertificate.org/).
There is no separate Contributor License Agreement.

Use your real name and a reachable email address:

```sh
git commit -s -m "Describe your change"
```

## Changes and validation

Keep pull requests focused and explain the change and how you checked it.
Put each example under `examples/<name>/` and common board code under
`src/board/`. Keep its MPLAB configuration, CMake dependencies, and CI in sync.
Keep configuration words together in `src/board/board.c` and preserve `LVP = ON`
for the board's low-voltage programming setup.

Build with XC8 3.10 for PIC16F1455 before submitting. See the
[README](README.md) for VS Code and command-line builds. For hardware changes,
report the board, programmer and observed result separately from compilation.
Do not commit build output, generated MPLAB CMake files, local tool paths or
probe serial numbers.

## Licence headers

Install Google's [addlicense](https://github.com/google/addlicense), using
the same version as CI:

```sh
go install github.com/google/addlicense@v1.2.0
addlicense -c "Maxim Pavlov (MaxEELabs)" -y 2026 -l apache -s -ignore '_build/**' -ignore 'out/**' -ignore 'cmake/PICnUSB/**' .
```

For a new file owned by another contributor, use that contributor's copyright
name and the appropriate year instead. Check headers with:

```sh
addlicense -check -ignore '_build/**' -ignore 'out/**' -ignore 'cmake/PICnUSB/**' .
```

CI checks licence headers and builds the firmware on pushes and pull requests.
