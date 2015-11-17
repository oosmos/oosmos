/*
// OOSMOS pin Class
//
// Copyright (C) 2014-2015  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <http://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef pinMAX
#define pinMAX 20
#endif

#include "oosmos.h"
#include "pin.h"

typedef enum {
  Unknown_State = 1,
  On_State,
  Off_State,
  ConfirmingOn_State,
  ConfirmingOff_State,
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
  #endif
  
  oosmos_sActiveObject m_ActiveObject;
  oosmos_sTimeout      m_Timeout;
  uint8_t              m_DebounceTimeMS;
  eStates              m_State:4;
  pin_eLogic           m_Logic:4;
  pin_eDirection       m_Direction:4;
};

static bool IsPhysicallyOn(pin * pPin);

static bool IsPhysicallyOff(pin * pPin)
{
  return !IsPhysicallyOn(pPin);
}

static void RunStateMachine(void * pObject)
{
  pin * pPin = (pin *) pObject;
  
  switch (pPin->m_State) {
    case On_State: 
      if (IsPhysicallyOff(pPin)) {
        pPin->m_State = ConfirmingOff_State;
        oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      }

      break;
    case Off_State:
      if (IsPhysicallyOn(pPin)) {
        pPin->m_State = ConfirmingOn_State;
        oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      }
      
      break;
    case ConfirmingOn_State:
      if (!oosmos_TimeoutHasExpired(&pPin->m_Timeout))
        break;

      if (!IsPhysicallyOn(pPin)) {
        pPin->m_State = Unknown_State;
        break;
      }

      pPin->m_State = On_State;
      break;
    case ConfirmingOff_State:
      if (!oosmos_TimeoutHasExpired(&pPin->m_Timeout))
        break;

      if (!IsPhysicallyOff(pPin)) {
        pPin->m_State = Unknown_State;
        break;
      }

      pPin->m_State = Off_State;
      break;
    case Unknown_State:
      pPin->m_State = IsPhysicallyOn(pPin) ? ConfirmingOn_State : ConfirmingOff_State;
      oosmos_TimeoutInMS(&pPin->m_Timeout, pPin->m_DebounceTimeMS);
      break;
  }
}

extern bool pinIsOn(pin * pPin)
{
  if (pPin->m_DebounceTimeMS == 0)
    return IsPhysicallyOn(pPin);

  return pPin->m_State == On_State;
}

extern bool pinIsOff(pin * pPin)
{
  return !pinIsOn(pPin);
}

#if defined(ARDUINO) || defined(oosmos_RASPBERRY_PI)

static bool IsPhysicallyOn(pin * pPin)
{ 
  const int PinValue = digitalRead(pPin->m_PinNumber); 
  return PinValue == (pPin->m_Logic == pinActiveHigh ? HIGH : LOW);
}

extern void pinOn(pin * pPin)
{
  const int PinNumber = pPin->m_PinNumber;
  
  if (pPin->m_Direction == pinInOut) {
    pinMode(PinNumber, pPin->m_Logic == pinActiveHigh ? INPUT : OUTPUT);
  }
    
  digitalWrite(PinNumber, pPin->m_Logic == pinActiveHigh ? HIGH : LOW);
}

extern void pinOff(pin * pPin)
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
  pPin->m_Logic          = Logic;
  pPin->m_State          = Unknown_State;
  pPin->m_Direction      = Direction;
  pPin->m_DebounceTimeMS = 0;
  
  switch (Direction) {
    case pinIn:
    case pinInOut: 
      pinMode(PinNumber, INPUT);
      break;
    case pinOut:
      pinMode(PinNumber, OUTPUT);
      break;
  }
  
  return pPin;
}

extern pin * pinNew_Debounce(const int PinNumber, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
{
  pin * pPin = pinNew(PinNumber, Direction, Logic);

  pPin->m_DebounceTimeMS = DebounceTimeMS;

  if (DebounceTimeMS > 0)
    oosmos_RegisterActiveObject(pPin, RunStateMachine, &pPin->m_ActiveObject);

  return pPin;
}

#elif defined(__PIC32MX)

static bool IsPhysicallyOn(pin * pPin)
{
  const uint32_t PinValue = PORTReadBits(pPin->m_Port, pPin->m_Bit);
  return (pPin->m_Logic == pinActiveHigh) ? PinValue != 0 : PinValue == 0;
}

extern void pinOn(pin * pPin)
{
  (pPin->m_Logic == pinActiveHigh ? PORTSetBits : PORTClearBits)(pPin->m_Port, pPin->m_Bit);
}

extern void pinOff(pin * pPin)
{
  (pPin->m_Logic == pinActiveHigh ? PORTClearBits : PORTSetBits)(pPin->m_Port, pPin->m_Bit);
}

extern pin * pinNew(const IoPortId Port, const int Bit, const pin_eDirection Direction, const pin_eLogic Logic)
{
  oosmos_Allocate(pPin, pin, pinMAX, NULL);
  
  pPin->m_Port           = Port;
  pPin->m_Bit            = Bit;
  pPin->m_Logic          = Logic;
  pPin->m_State          = Unknown_State;
  pPin->m_Direction      = Direction;
  pPin->m_DebounceTimeMS = 0;

  switch (pPin->m_Direction) {
    case pinOut:
      pinOff(pPin);
      PORTSetPinsDigitalOut(Port, Bit);
      break;
    case pinIn:
      pinOff(pPin);
      PORTSetPinsDigitalIn(Port, Bit);
      break;
  }

  return pPin;
}

extern pin * pinNew_Debounce(const IoPortId Port, const int Bit, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
{
  pin * pPin = pinNew(Port, Bit, Direction, Logic);

  pPin->m_DebounceTimeMS = DebounceTimeMS;

  if (DebounceTimeMS > 0)
    oosmos_RegisterActiveObject(pPin, RunStateMachine, &pPin->m_ActiveObject);

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
  pPin->m_Logic          = Logic;
  pPin->m_State          = Unknown_State;
  pPin->m_Direction      = Direction;
  pPin->m_DebounceTimeMS = 0;  

  return pPin;
}

extern pin * pinNew_Debounce(const PinName PinNumber, const pin_eDirection Direction, const pin_eLogic Logic, const uint8_t DebounceTimeMS)
{
  pin * pPin = pinNew(PinNumber, Direction, Logic);

  pPin->m_DebounceTimeMS = DebounceTimeMS;

  if (DebounceTimeMS > 0)
    oosmos_RegisterActiveObject(pPin, RunStateMachine, &pPin->m_ActiveObject);

  return pPin;
}

static bool IsPhysicallyOn(pin * pPin)
{
  DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
  const int PinValue = pDigitalOut->read(); 
  return PinValue == (pPin->m_Logic == pinActiveHigh ? 1 : 0);
}

extern void pinOn(pin * pPin)
{
  DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
  pDigitalOut->write(pPin->m_Logic == pinActiveHigh ? 1 : 0);
}

extern void pinOff(pin * pPin)
{
  DigitalOut * pDigitalOut = (DigitalOut *) pPin->m_Pin;
  pDigitalOut->write(pPin->m_Logic == pinActiveHigh ? 0 : 1);
}

extern PinName pinGetPinName(pin * pPin)
{
  return pPin->m_PinName;  
}
#else
  #error pin.c: Unsupported platform.
#endif



