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
#include <ws2812.h>

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

void main(void) {
  
  // The WS2812 driver requires a stable 48 MHz system clock.
  OSCCONbits.SCS = 0;
  OSCCONbits.IRCF = 0xF;  // 16 MHz HFINTOSC.
  OSCCONbits.SPLLMULT = 1;  // 3x PLL.
  OSCCONbits.SPLLEN = 1;

  while (!OSCSTATbits.PLLRDY) {
  }

  Ws2812Init(kWs2812ChainMain, 1);

  while (1) {
    Ws2812SetColor(kWs2812ChainMain, 0, 255, 0, 0, 20);
    __delay_ms(500);
    Ws2812Clear(kWs2812ChainMain);
    __delay_ms(500);
  }
}
