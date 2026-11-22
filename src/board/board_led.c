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

#include "board_led.h"

#include <ws2812.h>

static BoardLedColor current_color;

void BoardLedInit(void) {
  Ws2812Init(kWs2812ChainMain, 1);
  current_color = kBoardLedOff;
}

void BoardLedSet(BoardLedColor color) {
  if (color != current_color) {
    current_color = color;
    Ws2812SetColor(kWs2812ChainMain, 0, color == kBoardLedRed ? 255 : 0,
                   color == kBoardLedGreen ? 255 : 0, 0, 20);
  }
}
