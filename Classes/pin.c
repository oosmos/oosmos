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

#ifndef pinMAX
#define pinMAX 18
#endif

//===================================

#include "pin.h"
#include "oosmos.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  Unknown_State = 1,
  On_State,
  Off_State,
  ConfirmingOn_State,
  ConfirmingOff_State
} eStates;

struct pinTag
{
  #if defined(ARDUINO) || defined(oosmos_RASPBERRY_PI)
    uint8_t m_PinNumber;
  #elif defined(__PIC32MX)
    IoPortId m_Port;
    uint16_t m_Bit;
  #elif defined(__MBED__)
    uint8_t m_Pin[sizeof(DigitalOut)];
    PinName m_PinName;
  #elif defined(__IAR_SYSTEMS_ICC__)
    GPIO_TypeDef* m_Port;
    uint16_t m_Pin;
  #elif defined(_MSC_VER)
    char m_Key;
  #else
    #error pin.c: Unsupported platform.
  #endif

  oosmos_sActiveObject m_ActiveObject;
  oosmos_sTimeout      m_Timeout;
  uint8_t              m_DebounceTimeMS;
  unsigned int         m_State:4;     // eStates
  unsigned int         m_Logic:4;     // pin_eLogic
  unsigned int         m_Direction:4; // pin_eDirection
};

static bool IsPhysicallyOn(const pin * pPin);

static bool IsPhysicallyOff(const pin * pPin)
{
  return !IsPhysicallyOn(pPin);
}

static void RunStateMachine(void * pObject)
{
  oosmos_POINTER_GUARD(pObject);

  pin * pPin = (pin *) pObject;

  switch (pPin->m_State) {
    case On_State: {
      if (IsPhysicallyOff(pPin)) {
        pPin->m_State = (unsigned int) ConfirmingOff_State;
        oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      }

      break;
    }

    case Off_State: {
      if (IsPhysicallyOn(pPin)) {
        pPin->m_State = (unsigned int) ConfirmingOn_State;
        oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      }

      break;
    }

    case ConfirmingOn_State: {
      if (!oosmos_TimeoutHasExpired(&pPin->m_Timeout)) {
        break;
      }

      if (!IsPhysicallyOn(pPin)) {
        pPin->m_State = (unsigned int) Unknown_State;
        break;
      }

      pPin->m_State = (unsigned int) On_State;
      break;
    }

    case ConfirmingOff_State: {
      if (!oosmos_TimeoutHasExpired(&pPin->m_Timeout)) {
        break;
      }

      if (!IsPhysicallyOff(pPin)) {
        pPin->m_State = (unsigned int) Unknown_State;
        break;
      }

      pPin->m_State = (unsigned int) Off_State;
      break;
    }

    case Unknown_State: {
      pPin->m_State = (unsigned int) (IsPhysicallyOn(pPin) ? ConfirmingOn_State : ConfirmingOff_State);
      oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      break;
    }

    default: {
      oosmos_FOREVER();
    }
  }
}

extern bool pinIsOn(const pin * pPin)
{
  oosmos_POINTER_GUARD(pPin);

  if (pPin->m_DebounceTimeMS == 0) {
    return IsPhysicallyOn(pPin);
  }

  return pPin->m_State == (unsigned int) On_State;
}

extern bool pinIsOff(const pin * pPin)
{
  return !pinIsOn(pPin);
}

#if defined(ARDUINO) || defined(oosmos_RASPBERRY_PI)
  static bool IsPhysicallyOn(const pin * pPin)
  {
    const int PinValue = digitalRead(pPin->m_PinNumber);
    return PinValue == (pPin->m_Logic == pinActiveHigh ? HIGH : LOW);
  }

  extern void pinOn(const pin * pPin)
  {
    const int PinNumber = pPin->m_PinNumber;

    if (pPin->m_Direction == pinInOut) {
      pinMode(PinNumber, pPin->m_Logic == pinActiveHigh ? INPUT : OUTPUT);
    }

    digitalWrite(PinNumber, pPin->m_Logic == pinActiveHigh ? HIGH : LOW);
  }

  extern void pinOff(const pin * pPin)
  {
    const int PinNumber = pPin->m_PinNumber;

    if (pPin->m_Direction == pinInOut) {
      pinMode(PinNumber, pPin->m_Logic == pinActiveHigh ? OUTPUT : INPUT);
    }

    digitalWrite(PinNumber, pPin->m_Logic == pinActiveHigh ? LOW : HIGH);
  }

  extern int pinGetPinNumber(pin * pPin)
  {
    return pPin->m_PinNumber;
  }

  extern pin * pinNew(const int PinNumber, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    #if defined(oosmos_RASPBERRY_PI)
      static int WiringPi_Initialized = 0;

      if (!WiringPi_Initialized) {
        wiringPiSetup();
        WiringPi_Initialized = 1;
      }
    #endif

    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_PinNumber      = PinNumber;
    pPin->m_Logic          = (unsigned int) Logic;
    pPin->m_State          = (unsigned int) Unknown_State;
    pPin->m_Direction      = (unsigned int) Direction;
    pPin->m_DebounceTimeMS = 0;

    switch (Direction) {
      case pinIn:
      case pinInOut: {
        pinMode(PinNumber, INPUT);
        break;
      }
      case pinOut: {
        pinMode(PinNumber, OUTPUT);
        break;
      }
    }

    return pPin;
  }

  extern pin * pinNew_Debounce(const int PinNumber, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(PinNumber, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }
#elif defined(__PIC32MX)
  static bool IsPhysicallyOn(const pin * pPin)
  {
    const uint32_t PinValue = PORTReadBits(pPin->m_Port, pPin->m_Bit);
    return (pPin->m_Logic == pinActiveHigh) ? PinValue != 0 : PinValue == 0;
  }

  extern void pinOn(const pin * pPin)
  {
    (pPin->m_Logic == pinActiveHigh ? PORTSetBits : PORTClearBits)(pPin->m_Port, pPin->m_Bit);
  }

  extern void pinOff(const pin * pPin)
  {
    (pPin->m_Logic == pinActiveHigh ? PORTClearBits : PORTSetBits)(pPin->m_Port, pPin->m_Bit);
  }

  extern pin * pinNew(const IoPortId Port, const int Bit, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_Port           = Port;
    pPin->m_Bit            = Bit;
    pPin->m_Logic          = (unsigned int) Logic;
    pPin->m_State          = (unsigned int) Unknown_State;
    pPin->m_Direction      = (unsigned int) Direction;
    pPin->m_DebounceTimeMS = 0;

    switch (pPin->m_Direction) {
      case pinOut: {
        pinOff(pPin);
        PORTSetPinsDigitalOut(Port, Bit);
        break;
      }
      case pinIn: {
        pinOff(pPin);
        PORTSetPinsDigitalIn(Port, Bit);
        break;
      }
    }

    return pPin;
  }

  extern pin * pinNew_Debounce(const IoPortId Port, const int Bit, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(Port, Bit, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }
#elif defined(__MBED__)
  #include "mbed.h"
  #include <new>

  extern pin * pinNew(const PinName Pin, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);
    ::new(&pPin->m_Pin) DigitalOut(Pin);

    pPin->m_PinName        = Pin;
    pPin->m_Logic          = (unsigned int) Logic;
    pPin->m_State          = (unsigned int) Unknown_State;
    pPin->m_Direction      = (unsigned int) Direction;
    pPin->m_DebounceTimeMS = 0;

    return pPin;
  }

  extern pin * pinNew_Debounce(const PinName PinNumber, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(PinNumber, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }

  static bool IsPhysicallyOn(const pin * pPin)
  {
    DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
    const int PinValue = pDigitalOut->read();
    return PinValue == (pPin->m_Logic == pinActiveHigh ? 1 : 0);
  }

  extern void pinOn(const pin * pPin)
  {
    DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
    pDigitalOut->write(pPin->m_Logic == pinActiveHigh ? 1 : 0);
  }

  extern void pinOff(const pin * pPin)
  {
    DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
    pDigitalOut->write(pPin->m_Logic == pinActiveHigh ? 0 : 1);
  }

  extern PinName pinGetPinName(pin * pPin)
  {
    return pPin->m_PinName;
  }
#elif defined(__IAR_SYSTEMS_ICC__)
  static bool IsPhysicallyOn(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    const GPIO_PinState PinValue = HAL_GPIO_ReadPin(pPin->m_Port, pPin->m_Pin);
    return (pPin->m_Logic == pinActiveHigh) ? (PinValue == GPIO_PIN_SET) : (PinValue == GPIO_PIN_RESET);
  }

  extern void pinOn(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    HAL_GPIO_WritePin(pPin->m_Port, pPin->m_Pin, pPin->m_Logic == pinActiveHigh ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }

  extern void pinOff(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    HAL_GPIO_WritePin(pPin->m_Port, pPin->m_Pin, pPin->m_Logic == pinActiveHigh ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }

  extern pin * pinNew(GPIO_TypeDef* Port, uint16_t Pin, const pin_eDirection Direction, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_Port           = Port;
    pPin->m_Pin            = Pin;
    pPin->m_Logic          = (unsigned int) Logic;
    pPin->m_State          = (unsigned int) Unknown_State;
    pPin->m_Direction      = (unsigned int) Direction;
    pPin->m_DebounceTimeMS = 0;

    pinOff(pPin);
    return pPin;
  }

  extern pin * pinNew_Debounce(GPIO_TypeDef* Port, const uint16_t Bit, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
  {
    pin * pPin = pinNew(Port, Bit, Direction, Logic);

    pPin->m_DebounceTimeMS = DebounceTimeMS;

    if (DebounceTimeMS > 0) {
      oosmos_ActiveObjectInit(pPin, m_ActiveObject, RunStateMachine);
    }

    return pPin;
  }
#elif defined(_MSC_VER)
  #include <windows.h>
  #include <stdio.h>

  static bool   pinFirst = true;
  static bool   KeyIsDown[0xFF];
  static HANDLE hStdin;

  static bool IsPhysicallyOn(const pin * pPin)
  {
    oosmos_POINTER_GUARD(pPin);

    DWORD         NumRead;
    INPUT_RECORD  InputRecord;

    while (PeekConsoleInput(hStdin, &InputRecord, 1, &NumRead) && NumRead > 0) {
      if (InputRecord.EventType == KEY_EVENT) {
        const KEY_EVENT_RECORD KER  = InputRecord.Event.KeyEvent;
        const CHAR             Char = KER.uChar.AsciiChar;

        KeyIsDown[Char] = (KER.bKeyDown == TRUE);
      }

      (void) ReadConsoleInput(hStdin, &InputRecord, 1, &NumRead);
    }

    const bool IsDown = KeyIsDown[pPin->m_Key];
    return pPin->m_Logic == ((unsigned int) pinActiveHigh) ? IsDown : !IsDown;
  }

  extern pin * pinNew(char Key, const pin_eLogic Logic)
  {
    oosmos_Allocate(pPin, pin, pinMAX, NULL);

    pPin->m_Key            = Key;
    pPin->m_Logic          = (unsigned int) Logic;
    pPin->m_State          = (unsigned int) Unknown_State;
    pPin->m_DebounceTimeMS = 0;

    if (pinFirst) {
      memset(KeyIsDown, false, sizeof(KeyIsDown));

      hStdin = GetStdHandle(STD_INPUT_HANDLE);
      pinFirst = false;
    }

    return pPin;
  }

  extern void pinOn(const pin * pPin)
  {
    #if defined(pin_DEBUG)
      printf("%p Pin ON\n", pPin);
    #else
      oosmos_UNUSED(pPin);
    #endif
  }

  extern void pinOff(const pin * pPin)
  {
    #if defined(pin_DEBUG)
      printf("%p Pin OFF\n", pPin);
    #else
      oosmos_UNUSED(pPin);
    #endif
  }

  void (*pin_pDummy)(void *) = RunStateMachine; // To satisfy compiler
#else
  #error pin.c: Unsupported platform.
#endif
