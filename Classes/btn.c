//
// OOSMOS btn Class
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

#ifndef btnMaxButtons
#define btnMaxButtons 2
#endif

#ifndef btnMaxPressedSubscribers
#define btnMaxPressedSubscribers 1
#endif

#ifndef btnMaxReleasedSubscribers
#define btnMaxReleasedSubscribers 1
#endif

//===================================

#include "oosmos.h"
#include "pin.h"
#include "btn.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum {
  Pressed_State = 1,
  Released_State
} eStates;

struct btnTag
{
  pin      * m_pPin;
  eStates    m_State;

  oosmos_sActiveObject   m_ActiveObject;
  oosmos_sSubscriberList m_PressedEvent[btnMaxPressedSubscribers];
  oosmos_sSubscriberList m_ReleasedEvent[btnMaxReleasedSubscribers];
};

extern btn * btnNew(pin * pPin)
{
  oosmos_Allocate(pButton, btn, btnMaxButtons, NULL);

  pButton->m_pPin  = pPin;
  pButton->m_State = Released_State;

  oosmos_RegisterActiveObject(pButton, btnRunStateMachine, &pButton->m_ActiveObject);

  oosmos_SubscriberListInit(pButton->m_PressedEvent);
  oosmos_SubscriberListInit(pButton->m_ReleasedEvent);

  return pButton;
}

extern void btnSubscribePressedEvent(btn * pButton, oosmos_sQueue * pQueue, int PressedEventCode, void * pContext)
{
  oosmos_POINTER_GUARD(pButton);

  oosmos_SubscriberListAdd(pButton->m_PressedEvent, pQueue, PressedEventCode, pContext);
}

extern void btnSubscribeReleasedEvent(btn * pButton, oosmos_sQueue * pQueue, int ReleasedEventCode, void * pContext)
{
  oosmos_POINTER_GUARD(pButton);

  oosmos_SubscriberListAdd(pButton->m_ReleasedEvent, pQueue, ReleasedEventCode, pContext);
}

static eStates PhysicalButtonState(const btn * pButton)
{
  oosmos_POINTER_GUARD(pButton);

  return pinIsOn(pButton->m_pPin) ? Pressed_State : Released_State;
}

extern bool btnIsReleased(const btn * pButton)
{
  return !btnIsPressed(pButton);
}

extern bool btnIsPressed(const btn * pButton)
{
  return PhysicalButtonState(pButton) == Pressed_State;
}

extern void btnRunStateMachine(void * pObject)
{
  oosmos_POINTER_GUARD(pObject);

  btn * pButton = (btn *) pObject;

  switch (pButton->m_State) {
    case Released_State:
      if (pinIsOn(pButton->m_pPin)) {
        pButton->m_State = Pressed_State;
        oosmos_SubscriberListNotify(pButton->m_PressedEvent);
      }

      break;
    case Pressed_State:
      if (pinIsOff(pButton->m_pPin)) {
        pButton->m_State = Released_State;
        oosmos_SubscriberListNotify(pButton->m_ReleasedEvent);
      }

      break;
  }
}
