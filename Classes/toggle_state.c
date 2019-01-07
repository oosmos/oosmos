//
// OOSMOS toggle Class
//
// Copyright (C) 2014-2018  OOSMOS, LLC
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

#include "oosmos.h"
#include "toggle.h"
#include "pin.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef toggleMAX
#define toggleMAX 4
#endif

struct toggleTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf Off_State;
    oosmos_sLeaf On_State;
//<<<DECL

  pin      * m_pPin;
  uint32_t   m_TimeOnMS;
  uint32_t   m_TimeOffMS;
};

//>>>CODE
static bool Off_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  toggle * pToggle = (toggle *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutMS(pState, (uint32_t) pToggle->m_TimeOffMS);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pToggle, pState, On_State);
    }
  }

  return false;
}

static bool On_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  toggle * pToggle = (toggle *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      pinOn(pToggle->m_pPin);
      return oosmos_StateTimeoutMS(pState, (uint32_t) pToggle->m_TimeOnMS);
    }
    case oosmos_EXIT: {
      pinOff(pToggle->m_pPin);
      return true;
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pToggle, pState, Off_State);
    }
  }

  return false;
}
//<<<CODE

extern toggle * toggleNew(pin * pPin, uint32_t TimeOnMS, uint32_t TimeOffMS)
{
  oosmos_Allocate(pToggle, toggle, toggleMAX, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pToggle, ROOT, NULL, On_State);
    oosmos_LeafInit(pToggle, Off_State, ROOT);
    oosmos_LeafInit(pToggle, On_State, ROOT);
//<<<INIT

  pToggle->m_pPin      = pPin;
  pToggle->m_TimeOnMS  = TimeOnMS;
  pToggle->m_TimeOffMS = TimeOffMS;

#if 0
  oosmos_Debug(&pToggle->StateMachine, true, NULL);
#endif

  return pToggle;
}
