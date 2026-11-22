# Copyright 2026 Maxim Pavlov (MaxEELabs)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

# MPLAB for VS Code and MPLAB CLI load this file after generation.
include_guard(GLOBAL)
include(FetchContent)

# MLA USB v2.18, pinned to an immutable commit and verified archive checksum.
FetchContent_Declare(mla_usb
    URL "https://codeload.github.com/MicrochipTech/mla_usb/zip/d28cc13c6b4a539ed2333348efbde7c6546fbfc6"
    URL_HASH SHA256=43a2dde40588220e75f83a4e97d8630e219f230859e63735b462fd760f5fe9c3
    TIMEOUT 60
    INACTIVITY_TIMEOUT 20
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE)

# PIC16-RGB Initial Commit; keep the immutable revision and archive hash together.
FetchContent_Declare(pic16_rgb
    URL "https://codeload.github.com/max-eelabs/PIC16-RGB/zip/ee87a0ce0d2452169456961c6a8f511150240213"
    URL_HASH SHA256=12d0cd890641e4bb610f61e6f89a0f2dc9b85795180f026a00b4381818a8d460
    TIMEOUT 60
    INACTIVITY_TIMEOUT 20
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE)

set(PIC16_RGB_MAX_LEDS 1 CACHE STRING "Buffered LEDs for the RGB chain")
set(PIC16_RGB_CHAINS "Main:RA5:1" CACHE STRING "RGB LED on RA5 (pin 2)")
FetchContent_MakeAvailable(pic16_rgb mla_usb)

set(PICNUSB_USB_VID "0x04D8" CACHE STRING "CDC Sense USB vendor ID")
set(PICNUSB_USB_PID "0x000A" CACHE STRING "CDC Sense USB product ID")

set(firmware_target PICnUSB_default_default_XC8_compile)
set(project_root "${CMAKE_CURRENT_LIST_DIR}/../../..")
target_link_libraries(${firmware_target} PRIVATE PIC16RGB::ws2812)
target_sources(${firmware_target} PRIVATE
    "${mla_usb_SOURCE_DIR}/src/usb_device.c"
    "${mla_usb_SOURCE_DIR}/src/usb_device_cdc.c")
target_include_directories(${firmware_target} PRIVATE
    "${project_root}/src/board"
    "${project_root}/examples/cdc-sense/sense"
    "${project_root}/examples/cdc-sense/usb"
    "${mla_usb_SOURCE_DIR}/inc")
target_compile_definitions(${firmware_target} PRIVATE
    PICNUSB_USB_VID=${PICNUSB_USB_VID}
    PICNUSB_USB_PID=${PICNUSB_USB_PID})
