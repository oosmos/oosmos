//
// OOSMOS btn Class
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

struct btnTag
{
  pin * m_pPin;

  oosmos_sObjectThread   m_ObjectThread;
  oosmos_sSubscriberList m_PressedEvent[btnMaxPressedSubscribers];
  oosmos_sSubscriberList m_ReleasedEvent[btnMaxReleasedSubscribers];
};

static void Thread(const btn * pButton, oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pButton);
  oosmos_POINTER_GUARD(pState);

  oosmos_ThreadBegin();
    for (;;) {
      oosmos_ThreadWaitCond(pinIsOn(pButton->m_pPin));
      oosmos_SubscriberListNotify(pButton->m_PressedEvent);

      oosmos_ThreadWaitCond(pinIsOff(pButton->m_pPin));
      oosmos_SubscriberListNotify(pButton->m_ReleasedEvent);
    }
  oosmos_ThreadEnd();
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

extern btn * btnNew(pin * pPin)
{
  oosmos_Allocate(pButton, btn, btnMaxButtons, NULL);

  pButton->m_pPin = pPin;

  oosmos_ObjectThreadInit(pButton, m_ObjectThread, Thread, true);

  oosmos_SubscriberListInit(pButton->m_PressedEvent);
  oosmos_SubscriberListInit(pButton->m_ReleasedEvent);

  return pButton;
}
