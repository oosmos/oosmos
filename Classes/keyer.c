//
// OOSMOS keyer Class
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

#include "oosmos.h"
#include "keyer.h"
#include "pin.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct keyerTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf DahSound_State;
    oosmos_sLeaf Silent_State;
    oosmos_sLeaf Choice2_State;
    oosmos_sLeaf Choice1_State;
    oosmos_sLeaf DitSound_State;
//<<<DECL

  pin * m_pDahPin;
  pin * m_pDitPin;
  pin * m_pSpeakerPin;

  uint32_t m_DitTimeMS;
  uint32_t m_DahTimeMS;
  uint32_t m_SpaceTimeMS;

  bool m_DitWasPressed;
  bool m_DahWasPressed;
};

static void CheckDahIsPressedPoll(keyer * pKeyer)
{
  if (!pKeyer->m_DahWasPressed && pinIsOn(pKeyer->m_pDahPin)) {
    pKeyer->m_DahWasPressed = true;
  }
}

static void CheckDitIsPressedPoll(keyer * pKeyer)
{
  if (!pKeyer->m_DitWasPressed && pinIsOn(pKeyer->m_pDitPin)) {
    pKeyer->m_DitWasPressed = true;
  }
}

static bool DahWasPressed(const keyer * pKeyer)
{
  return pKeyer->m_DahWasPressed;
}

static bool DitWasPressed(const keyer * pKeyer)
{
  return pKeyer->m_DitWasPressed;
}

static bool IsDitPressed(const keyer * pKeyer)
{
  return pinIsOn(pKeyer->m_pDitPin);
}

static bool IsDahPressed(const keyer * pKeyer)
{
  return pinIsOn(pKeyer->m_pDahPin);
}

static void DitThread(const keyer * pKeyer, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pinOn(pKeyer->m_pSpeakerPin);
    oosmos_ThreadDelayMS(pKeyer->m_DitTimeMS);
    pinOff(pKeyer->m_pSpeakerPin);

    oosmos_ThreadDelayMS(pKeyer->m_SpaceTimeMS);
  oosmos_ThreadEnd();
}

static void DahThread(const keyer * pKeyer, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pinOn(pKeyer->m_pSpeakerPin);
    oosmos_ThreadDelayMS(pKeyer->m_DahTimeMS);
    pinOff(pKeyer->m_pSpeakerPin);

    oosmos_ThreadDelayMS(pKeyer->m_SpaceTimeMS);
  oosmos_ThreadEnd();
}

//>>>CODE
static bool DahSound_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      pKeyer->m_DitWasPressed = false;
      return true;
    }
    case oosmos_POLL: {
      CheckDitIsPressedPoll(pKeyer);
      DahThread(pKeyer, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pKeyer, pState, Choice1_State);
    }
  }

  return false;
}

static bool Silent_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      if (IsDahPressed(pKeyer)) {
        return oosmos_Transition(pKeyer, pState, DahSound_State);
      }
      if (IsDitPressed(pKeyer)) {
        return oosmos_Transition(pKeyer, pState, DitSound_State);
      }
      return true;
    }
  }

  return false;
}

static bool Choice2_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  if (oosmos_EventCode(pEvent) == oosmos_ENTER) {
    if (DahWasPressed(pKeyer)) {
      return oosmos_Transition(pKeyer, pState, DahSound_State);
    }
    else {
      return oosmos_Transition(pKeyer, pState, Silent_State);
    }
  }

  return false;
}

static bool Choice1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  if (oosmos_EventCode(pEvent) == oosmos_ENTER) {
    if (DitWasPressed(pKeyer)) {
      return oosmos_Transition(pKeyer, pState, DitSound_State);
    }
    else {
      return oosmos_Transition(pKeyer, pState, Silent_State);
    }
  }

  return false;
}

static bool DitSound_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      pKeyer->m_DahWasPressed = false;
      return true;
    }
    case oosmos_POLL: {
      CheckDahIsPressedPoll(pKeyer);
      DitThread(pKeyer, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pKeyer, pState, Choice2_State);
    }
  }

  return false;
}
//<<<CODE

extern keyer * keyerNew(pin * pDahPin, pin * pDitPin, pin * pSpeakerPin, unsigned WPM)
{
  oosmos_Allocate(pKeyer, keyer, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pKeyer, ROOT, NULL, Silent_State);
    oosmos_LeafInit(pKeyer, DahSound_State, ROOT, DahSound_State_Code);
    oosmos_LeafInit(pKeyer, Silent_State, ROOT, Silent_State_Code);
    oosmos_LeafInit(pKeyer, Choice2_State, ROOT, Choice2_State_Code);
    oosmos_LeafInit(pKeyer, Choice1_State, ROOT, Choice1_State_Code);
    oosmos_LeafInit(pKeyer, DitSound_State, ROOT, DitSound_State_Code);

  oosmos_Debug(pKeyer, NULL);
//<<<INIT

  pKeyer->m_pDahPin     = pDahPin;
  pKeyer->m_pDitPin     = pDitPin;
  pKeyer->m_pSpeakerPin = pSpeakerPin;
  pKeyer->m_DitTimeMS   = 1200 / WPM;
  pKeyer->m_DahTimeMS   = pKeyer->m_DitTimeMS * 3;
  pKeyer->m_SpaceTimeMS = pKeyer->m_DitTimeMS;

  return pKeyer;
}
