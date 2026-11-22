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

#ifndef PICNUSB_CDC_SENSE_H_
#define PICNUSB_CDC_SENSE_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  kSenseIncomplete,
  kSenseRead,
  kSenseInvalid,
} SenseCommand;

// Start a new session with a zero-initialized parser.
typedef struct {
  uint8_t matched;
  bool invalid;
} SenseParser;

enum { kSenseReplySize = sizeof("PIN7=0000 PIN3=0000\r\n") };

SenseCommand SenseFeed(SenseParser *parser, uint8_t byte);
void SenseFormatReply(char *reply, uint16_t pin7_count, uint16_t pin3_count);

#endif  // PICNUSB_CDC_SENSE_H_
