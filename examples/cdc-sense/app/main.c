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

#include <stdint.h>
#include <string.h>
#include <xc.h>

#include "adc.h"
#include "board.h"
#include "board_led.h"
#include "sense.h"
#include "status_led.h"
#include "usb.h"
#include "usb_device_cdc.h"

// MLA exports the host's control line state from usb_device_cdc.c.
extern CONTROL_SIGNAL_BITMAP control_signal_bitmap;

static SenseParser parser;
static uint8_t rx[CDC_DATA_OUT_EP_SIZE];
static uint8_t rx_length;
static uint8_t rx_offset;
static char reply[kSenseReplySize];

static void ResetCommand(void) {
  parser.matched = 0;
  parser.invalid = false;
  rx_length = 0;
  rx_offset = 0;
}

bool USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *data,
                                     uint16_t size) {
  (void)data;
  (void)size;
  switch ((uint8_t)event) {
    case EVENT_CONFIGURED:
      control_signal_bitmap._byte = 0;
      ResetCommand();
      CDCInitEP();
      break;
    case EVENT_EP0_REQUEST:
      USBCheckCDCRequest();
      if (!control_signal_bitmap.DTE_PRESENT) {
        ResetCommand();
      }
      break;
    default:
      break;
  }
  return true;
}

static void SendSenseReply(SenseCommand command) {
  if (command == kSenseInvalid) {
    memcpy(reply, "ERR CMD\r\n", sizeof("ERR CMD\r\n"));
    putUSBUSART((uint8_t *)reply, sizeof("ERR CMD\r\n") - 1);
    return;
  }

  uint16_t pin7;
  uint16_t pin3;
  if (SenseAdcRead(&pin7, &pin3)) {
    SenseFormatReply(reply, pin7, pin3);
    putUSBUSART((uint8_t *)reply, kSenseReplySize - 1);
  } else {
    memcpy(reply, "ERR ADC\r\n", sizeof("ERR ADC\r\n"));
    putUSBUSART((uint8_t *)reply, sizeof("ERR ADC\r\n") - 1);
  }
}

static void CdcTasks(bool port_open) {
  CDCTxService();
  if (!port_open) {
    ResetCommand();
    // Drain OUT data from the closed session so it cannot form a new command.
    getsUSBUSART(rx, sizeof(rx));
    return;
  }
  if (!USBUSARTIsTxTrfReady()) {
    return;
  }

  if (rx_offset == rx_length) {
    rx_length = getsUSBUSART(rx, sizeof(rx));
    rx_offset = 0;
  }
  while (rx_offset < rx_length) {
    SenseCommand command = SenseFeed(&parser, rx[rx_offset++]);
    if (command != kSenseIncomplete) {
      SendSenseReply(command);
      CDCTxService();
      return;
    }
  }
}

void main(void) {
  BoardClockInit();
  BoardLedInit();
  SenseAdcInit();
  StatusLedInit();

  ACTCONbits.ACTSRC = 1;  // USB host start-of-frame packets.
  ACTCONbits.ACTEN = 1;
  USBDeviceInit();
  USBDeviceAttach();

  while (1) {
    USBDeviceTasks();
    bool configured = USBGetDeviceState() == CONFIGURED_STATE;
    bool suspended = USBIsDeviceSuspended();
    bool port_open = configured && control_signal_bitmap.DTE_PRESENT;
    if (!configured) {
      control_signal_bitmap._byte = 0;
      ResetCommand();
    } else if (!suspended) {
      CdcTasks(port_open);
    }
    StatusLedTasks(configured, port_open, suspended);
  }
}
