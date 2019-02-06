//
// OOSMOS toggle Class
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

#ifndef toggleMAX
#define toggleMAX 1
#endif

//===================================

#include "oosmos.h"
#include "toggle.h"
#include "pin.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct toggleTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf Running_State;
//<<<DECL

  pin      * m_pPin;
  uint32_t   m_TimeOnMS;
  uint32_t   m_TimeOffMS;
};

static void ToggleThread(const toggle * pToggle, oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pToggle);
  oosmos_POINTER_GUARD(pState);

  oosmos_ThreadBegin();
    for (;;) {
      pinOn(pToggle->m_pPin);
      oosmos_ThreadDelayMS(pToggle->m_TimeOnMS);

      pinOff(pToggle->m_pPin);
      oosmos_ThreadDelayMS(pToggle->m_TimeOffMS);
    }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  toggle * pToggle = (toggle *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      ToggleThread(pToggle, pState);
      return true;
    }
  }

  return false;
}
//<<<CODE

extern toggle * toggleNew(pin * pPin, uint32_t TimeOnMS, uint32_t TimeOffMS)
{
  oosmos_Allocate(pToggle, toggle, toggleMAX, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pToggle, ROOT, NULL, Running_State);
    oosmos_LeafInit(pToggle, Running_State, ROOT, Running_State_Code);
//<<<INIT

  pToggle->m_pPin      = pPin;
  pToggle->m_TimeOnMS  = TimeOnMS;
  pToggle->m_TimeOffMS = TimeOffMS;

  return pToggle;
}
