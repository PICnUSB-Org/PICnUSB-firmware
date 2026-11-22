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

#include "status_led.h"

#include <stdint.h>
#include <xc.h>

#include "board_led.h"

static bool blinking;
static bool blink_on;
static uint16_t blink_ms;

void StatusLedInit(void) {
  // Timer2 ticks every ms at 48 MHz with 1:16 prescale and 1:3 postscale.
  T2CON = 0;
  TMR2 = 0;
  PR2 = 249;
  PIR1bits.TMR2IF = 0;
  T2CONbits.T2CKPS = 2;
  T2CONbits.T2OUTPS = 2;
  T2CONbits.TMR2ON = 1;
}

void StatusLedTasks(bool configured, bool port_open, bool suspended) {
  bool tick = PIR1bits.TMR2IF;
  if (tick) {
    PIR1bits.TMR2IF = 0;
  }

  BoardLedColor color = kBoardLedOff;
  if (!configured) {
    if (!blinking) {
      blink_on = true;
      blink_ms = 0;
    } else if (tick && ++blink_ms == 500) {
      blink_on = !blink_on;
      blink_ms = 0;
    }
    color = blink_on ? kBoardLedRed : kBoardLedOff;
  } else if (!suspended) {
    color = port_open ? kBoardLedGreen : kBoardLedRed;
  }
  blinking = !configured;

  BoardLedSet(color);
}
