//
// OOSMOS btnph Class
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

#ifndef btnphMaxButtons
#define btnphMaxButtons 2
#endif

#ifndef btnphMaxPressedSubscribers
#define btnphMaxPressedSubscribers 1
#endif

#ifndef btnphMaxHeldSubscribers
#define btnphMaxHeldSubscribers 1
#endif

#ifndef btnphMaxReleasedSubscribers
#define btnphMaxReleasedSubscribers 1
#endif

//===================================

#include "btnph.h"
#include "oosmos.h"
#include "pin.h"
#include <stdbool.h>
#include <stddef.h>

struct btnphTag
{
  pin      * m_pPin;
  uint32_t   m_HoldTimeMS;

  oosmos_sObjectThread   m_ObjectThread;
  oosmos_sSubscriberList m_PressedEvent[btnphMaxPressedSubscribers];
  oosmos_sSubscriberList m_HeldEvent[btnphMaxHeldSubscribers];
  oosmos_sSubscriberList m_ReleasedEvent[btnphMaxReleasedSubscribers];
};

static void Thread(const btnph * pButton, oosmos_sState * pState)
{
  bool TimedOut = false;

  oosmos_POINTER_GUARD(pButton);
  oosmos_POINTER_GUARD(pState);

  oosmos_ThreadBegin();
    for (;;) {
      oosmos_ThreadWaitCond(pinIsOn(pButton->m_pPin));
      oosmos_ThreadWaitCond_TimeoutMS(pinIsOff(pButton->m_pPin), pButton->m_HoldTimeMS, &TimedOut);

      if (TimedOut) {
        oosmos_SubscriberListNotify(pButton->m_HeldEvent);
        oosmos_ThreadWaitCond(pinIsOff(pButton->m_pPin));
      } else {
        oosmos_SubscriberListNotify(pButton->m_PressedEvent);
      }

      oosmos_SubscriberListNotify(pButton->m_ReleasedEvent);
    }
  oosmos_ThreadEnd();
}

extern void btnphSubscribePressedEvent(btnph * pButton, oosmos_sQueue * pQueue, int PressedEventCode, void * pContext)
{
  oosmos_POINTER_GUARD(pButton);

  oosmos_SubscriberListAdd(pButton->m_PressedEvent, pQueue, PressedEventCode, pContext);
}

extern void btnphSubscribeHeldEvent(btnph * pButton, oosmos_sQueue * pQueue, int HeldEventCode, void * pContext)
{
  oosmos_POINTER_GUARD(pButton);

  oosmos_SubscriberListAdd(pButton->m_HeldEvent, pQueue, HeldEventCode, pContext);
}

extern void btnphSubscribeReleasedEvent(btnph * pButton, oosmos_sQueue * pQueue, int ReleasedEventCode, void * pContext)
{
  oosmos_POINTER_GUARD(pButton);

  oosmos_SubscriberListAdd(pButton->m_ReleasedEvent, pQueue, ReleasedEventCode, pContext);
}

extern btnph * btnphNew(pin * pPin, uint32_t HoldTimeMS)
{
  oosmos_Allocate(pButton, btnph, btnphMaxButtons, NULL);

  pButton->m_pPin       = pPin;
  pButton->m_HoldTimeMS = HoldTimeMS;

  oosmos_ObjectThreadInit(pButton, m_ObjectThread, Thread, true);

  oosmos_SubscriberListInit(pButton->m_PressedEvent);
  oosmos_SubscriberListInit(pButton->m_HeldEvent);
  oosmos_SubscriberListInit(pButton->m_ReleasedEvent);

  return pButton;
}
