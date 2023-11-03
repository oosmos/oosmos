//
// OOSMOS pin Class
//
// Copyright (C) 2014-2020  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://www.oosmos.com/licensing/>.
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
  pinIn_Pullup,
  pinInOut
} pin_eDirection;

typedef enum
{
  pinActiveLow = 1,
  pinActiveHigh
} pin_eLogic;

#if defined(__cplusplus)
extern "C" {
#endif

	extern void pinOn(const pin* pPin);
	extern void pinOff(const pin* pPin);

	extern bool pinIsOn(const pin* pPin);
	extern bool pinIsOff(const pin* pPin);

#if defined(oosmos_PIN_ARDUINO) || defined(oosmos_PIN_WIRING)
	extern pin* pinNew(unsigned PinNumber, pin_eDirection, pin_eLogic Logic);
	extern pin* pinNew_Debounce(unsigned PinNumber, pin_eDirection, pin_eLogic Logic, uint8_t DebounceTimeMS);
	extern unsigned pinGetPinNumber(pin* pPin);
#endif

#if defined(oosmos_PIN_PIC32MX)
	extern pin* pinNew(IoPortId Port, unsigned Bit, pin_eDirection Direction, pin_eLogic Logic);
	extern pin* pinNew_Debounce(IoPortId Port, unsigned Bit, pin_eDirection, pin_eLogic Logic, uint8_t DebounceTimeMS);
#endif

#if defined(oosmos_PIN_MBED)
	extern pin* pinNew(PinName Pin, pin_eDirection, pin_eLogic Logic);
	extern pin* pinNew_Debounce(PinName Pin, pin_eDirection, pin_eLogic Logic, uint8_t DebounceTimeMS);
	extern unsigned pinGetPinName(pin* pPin);
#endif

#if defined(oosmos_PIN_IAR_SYSTEMS_ICC)
	extern pin* pinNew(GPIO_TypeDef* Port, uint16_t Pin, const pin_eDirection Direction, const pin_eLogic Logic);
	extern pin* pinNew_Debounce(GPIO_TypeDef* Port, const uint16_t Bit, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS);
#endif

#if defined(oosmos_PIN_KEY_WINDOWS) || defined(oosmos_PIN_KEY_LINUX)
	extern pin* pinNew_Key(char Key, pin_eLogic Logic);
	extern pin* pinNew_Key_Debounce(char Key, const pin_eLogic Logic, const uint8_t DebounceTimeMS);
#endif

#if defined(oosmos_PIN_SYSFS)
	extern pin* pinNew(int pinNumber, pin_eDirection direction, pin_eLogic Logic);
#endif

#if defined(oosmos_PIN_DUMMY)
	extern pin* pinNew_Dummy(void);
#endif

#if defined(oosmos_PIN_UM232H)
	extern pin* pinNew_UM232H(uint8_t pinNumber, pin_eDirection direction, pin_eLogic Logic);
	extern pin* pinNew_UM232H_Debounce(uint8_t PinNumber, pin_eDirection Direction, pin_eLogic Logic, const uint8_t DebounceTimeMS);
#endif

#if defined(__cplusplus)
}
#endif

#endif
