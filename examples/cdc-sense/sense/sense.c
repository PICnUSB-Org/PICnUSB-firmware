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

#include "sense.h"

#include <string.h>

SenseCommand SenseFeed(SenseParser *parser, uint8_t byte) {
  static const char command[] = "SENSE?";
  const uint8_t command_length = sizeof(command) - 1;
  if (byte == '\r' || byte == '\n') {
    SenseCommand result = kSenseIncomplete;
    if (parser->invalid || parser->matched != 0) {
      result = parser->matched == command_length && !parser->invalid
                   ? kSenseRead
                   : kSenseInvalid;
    }
    parser->matched = 0;
    parser->invalid = false;
    return result;
  }

  if (!parser->invalid) {
    if (parser->matched < command_length && byte == command[parser->matched]) {
      ++parser->matched;
    } else {
      parser->invalid = true;
    }
  }
  return kSenseIncomplete;
}

static void FormatCount(char *digits, uint16_t count) {
  digits[0] = (char)('0' + count / 1000);
  count %= 1000;
  digits[1] = (char)('0' + count / 100);
  count %= 100;
  digits[2] = (char)('0' + count / 10);
  digits[3] = (char)('0' + count % 10);
}

void SenseFormatReply(char *reply, uint16_t pin7_count, uint16_t pin3_count) {
  memcpy(reply, "PIN7=0000 PIN3=0000\r\n", kSenseReplySize);
  FormatCount(reply + sizeof("PIN7=") - 1, pin7_count);
  FormatCount(reply + sizeof("PIN7=0000 PIN3=") - 1, pin3_count);
}
