//
// OOSMOS pin Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef pin_h
#define pin_h

#include "oosmos.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct pinTag pin;

typedef enum
{
  pinOut = 1,
  pinIn,
  pinInOut
} pin_eDirection;

typedef enum
{
  pinActiveLow = 1,
  pinActiveHigh
} pin_eLogic;

extern void pinOn(const pin * pPin);
extern void pinOff(const pin * pPin);

extern bool pinIsOn(const pin * pPin);
extern bool pinIsOff(const pin * pPin);

#if defined(ARDUINO) || defined(oosmos_RASPBERRY_PI)
  extern pin * pinNew(int PinNumber, pin_eDirection, pin_eLogic Logic);
  extern pin * pinNew_Debounce(int PinNumber, pin_eDirection, pin_eLogic Logic, uint8_t DebounceTimeMS);
  extern int pinGetPinNumber(pin * pPin);
#elif defined(__PIC32MX)
  extern pin * pinNew(IoPortId Port, int Bit, pin_eDirection Direction, pin_eLogic Logic);
  extern pin * pinNew_Debounce(IoPortId Port, int Bit, pin_eDirection, pin_eLogic Logic, uint8_t DebounceTimeMS);
#elif defined(__MBED__)
  extern pin * pinNew(PinName Pin, pin_eDirection, pin_eLogic Logic);
  extern pin * pinNew_Debounce(PinName Pin, pin_eDirection, pin_eLogic Logic, uint8_t DebounceTimeMS);
  extern int pinGetPinName(pin * pPin);
#elif defined(__IAR_SYSTEMS_ICC__)
  extern pin * pinNew(GPIO_TypeDef* Port, uint16_t Pin, const pin_eDirection Direction, const pin_eLogic Logic);
  extern pin * pinNew_Debounce(GPIO_TypeDef* Port, const uint16_t Bit, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS);
#elif defined(_MSC_VER)
  extern pin * pinNew(char Key, pin_eLogic Logic);
#endif

#endif

