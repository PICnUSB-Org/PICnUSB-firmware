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

#include "adc.h"

#include <xc.h>

#include "board.h"

void SenseAdcInit(void) {
  // Pin 7 uses AN7/RC3. Pin 3 uses AN3/RA4.
  TRISCbits.TRISC3 = 1;
  TRISAbits.TRISA4 = 1;
  ANSELCbits.ANSC3 = 1;
  ANSELAbits.ANSA4 = 1;
  WPUAbits.WPUA4 = 0;

  ADCON1 = 0;  // Use VDD and VSS as ADC references.
  ADCON1bits.ADFM = 1;
  ADCON1bits.ADCS = 6;  // FOSC/64 gives a 1.33 us ADC clock.
  ADCON0bits.ADON = 1;
}

static bool ReadChannel(uint8_t channel, uint16_t *count) {
  ADCON0bits.CHS = channel;
  __delay_us(20);
  ADCON0bits.GO = 1;
  for (uint8_t attempts = 0; attempts < 100; ++attempts) {
    if (!ADCON0bits.GO) {
      *count = ((uint16_t)ADRESH << 8) | ADRESL;
      return true;
    }
  }
  ADCON0bits.GO = 0;
  return false;
}

bool SenseAdcRead(uint16_t *pin7, uint16_t *pin3) {
  return ReadChannel(7, pin7) && ReadChannel(3, pin3);
}
