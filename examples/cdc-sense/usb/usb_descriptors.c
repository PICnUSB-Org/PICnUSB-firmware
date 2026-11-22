/*
 * Copyright 2026 Maxim Pavlov (MaxEELabs)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usb.h"

const USB_DEVICE_DESCRIPTOR device_dsc = {
    .bLength = 18,
    .bDescriptorType = USB_DESCRIPTOR_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x02,  // CDC.
    .bMaxPacketSize0 = USB_EP0_BUFF_SIZE,
    .idVendor = PICNUSB_USB_VID,
    .idProduct = PICNUSB_USB_PID,
    .bcdDevice = 0x0100,
    .iProduct = 1,
    .bNumConfigurations = 1,
};

enum { kConfigLength = 67 };

static const uint8_t config_descriptor[] = {
    9, USB_DESCRIPTOR_CONFIGURATION, kConfigLength, 0, 2, 1, 0, 0x80, 50,

    // CDC communication interface and its class-specific descriptors.
    9, USB_DESCRIPTOR_INTERFACE, CDC_COMM_INTF_ID, 0, 1, 0x02, 0x02, 0x01, 0,
    5, 0x24, 0x00, 0x10, 0x01,  // CDC header, version 1.10.
    5, 0x24, 0x01, 0, CDC_DATA_INTF_ID,  // Call management.
    4, 0x24, 0x02, 0x02,  // ACM line coding and control line state.
    5, 0x24, 0x06, CDC_COMM_INTF_ID, CDC_DATA_INTF_ID,  // Union.
    7, USB_DESCRIPTOR_ENDPOINT, 0x80 | CDC_COMM_EP, 0x03,
    CDC_COMM_IN_EP_SIZE, 0, 16,

    // CDC data interface and bulk endpoints.
    9, USB_DESCRIPTOR_INTERFACE, CDC_DATA_INTF_ID, 0, 2, 0x0A, 0, 0, 0,
    7, USB_DESCRIPTOR_ENDPOINT, CDC_DATA_EP, 0x02, CDC_DATA_OUT_EP_SIZE, 0, 0,
    7, USB_DESCRIPTOR_ENDPOINT, 0x80 | CDC_DATA_EP, 0x02,
    CDC_DATA_IN_EP_SIZE, 0, 0,
};

static const uint8_t language[] = {4, USB_DESCRIPTOR_STRING, 0x09, 0x04};
static const uint8_t product[] = {
    36, USB_DESCRIPTOR_STRING,
    'P', 0, 'I', 0, 'C', 0, 'n', 0, 'U', 0, 'S', 0, 'B', 0, ' ', 0,
    'C', 0, 'D', 0, 'C', 0, ' ', 0, 'S', 0, 'e', 0, 'n', 0, 's', 0, 'e', 0,
};

const uint8_t *const USB_CD_Ptr[] = {config_descriptor};
const uint8_t *const USB_SD_Ptr[] = {language, product};
