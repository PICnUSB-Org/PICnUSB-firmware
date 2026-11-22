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

#include <assert.h>
#include <string.h>

static void ExpectCommands(SenseParser *parser, const char *bytes,
                           const SenseCommand *expected, size_t length) {
  size_t seen = 0;
  while (*bytes) {
    SenseCommand result = SenseFeed(parser, (uint8_t)*bytes++);
    if (result != kSenseIncomplete) {
      assert(seen < length);
      assert(result == expected[seen++]);
    }
  }
  assert(seen == length);
}

int main(void) {
  SenseParser parser = {0};
  const SenseCommand read[] = {kSenseRead};
  const SenseCommand two_reads[] = {kSenseRead, kSenseRead};
  const SenseCommand invalid_then_read[] = {kSenseInvalid, kSenseRead};

  ExpectCommands(&parser, "\r\nSE", NULL, 0);
  ExpectCommands(&parser, "NSE?\r", read, 1);
  ExpectCommands(&parser, "\nSENSE?\nSENSE?\r", two_reads, 2);
  ExpectCommands(&parser, "SENSE?xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\r\nSENSE?\n",
                 invalid_then_read, 2);
  ExpectCommands(&parser, "sense?\nSENSE?\n", invalid_then_read, 2);

  ExpectCommands(&parser, "SEN", NULL, 0);
  parser = (SenseParser){0};
  ExpectCommands(&parser, "SE?\nSENSE?\n", invalid_then_read, 2);
  assert(SenseFeed(&parser, 0) == kSenseIncomplete);
  assert(SenseFeed(&parser, '\n') == kSenseInvalid);

  char reply[kSenseReplySize + 1];
  reply[kSenseReplySize] = '!';
  SenseFormatReply(reply, 512, 1);
  assert(strcmp(reply, "PIN7=0512 PIN3=0001\r\n") == 0);
  SenseFormatReply(reply, 1023, 0);
  assert(strcmp(reply, "PIN7=1023 PIN3=0000\r\n") == 0);
  SenseFormatReply(reply, 99, 100);
  assert(strcmp(reply, "PIN7=0099 PIN3=0100\r\n") == 0);
  assert(reply[kSenseReplySize] == '!');
  return 0;
}
