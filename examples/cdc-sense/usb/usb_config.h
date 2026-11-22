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

#ifndef PICNUSB_CDC_SENSE_USB_CONFIG_H_
#define PICNUSB_CDC_SENSE_USB_CONFIG_H_

#define USB_POLLING
#define USB_SUPPORT_DEVICE
#define USB_USE_CDC
#define USB_SPEED_OPTION USB_FULL_SPEED
#define USB_PULLUP_OPTION USB_PULLUP_ENABLE
#define USB_TRANSCEIVER_OPTION USB_INTERNAL_TRANSCEIVER
#define USB_PING_PONG_MODE USB_PING_PONG__NO_PING_PONG

#define USB_EP0_BUFF_SIZE 16
#define USB_MAX_NUM_INT 2
#define USB_MAX_EP_NUMBER 2
#define USB_NUM_STRING_DESCRIPTORS 2

#define CDC_COMM_INTF_ID 0
#define CDC_DATA_INTF_ID 1
#define CDC_COMM_EP 1
#define CDC_COMM_IN_EP_SIZE 10
#define CDC_DATA_EP 2
#define CDC_DATA_IN_EP_SIZE 32
#define CDC_DATA_OUT_EP_SIZE 32
#define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D1

#endif  // PICNUSB_CDC_SENSE_USB_CONFIG_H_
