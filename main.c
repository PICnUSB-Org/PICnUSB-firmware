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

#include <xc.h>

// Internal oscillator; no external clock or USB stack is required.
#pragma config FOSC = INTOSC
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config BOREN = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF

#pragma config WRT = OFF
#pragma config CPUDIV = NOCLKDIV
#pragma config USBLSCLK = 48MHz
#pragma config PLLMULT = 3x
#pragma config PLLEN = DISABLED
#pragma config STVREN = ON
#pragma config BORV = LO
#pragma config LPBOR = OFF
#pragma config LVP = ON

#define _XTAL_FREQ 16000000UL

void main(void) {
  // Select 16 MHz HFINTOSC, with the PLL disabled.
  OSCCON = 0x7C;

  // Add peripheral initialization here. Pins retain their reset input state.
  while (1) {
    // Add application tasks here.
  }
}
