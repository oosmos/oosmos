//
// OOSMOS switchtest Class
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
#include "pin.h"
#include "sw.h"
#include "prt.h"
#include "switchtest.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef MAX_SWITCHTESTS
#define MAX_SWITCHTESTS 2
#endif

enum {
  OpenEvent = 1,
  ClosedEvent,
};

struct switchtestTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 2);
    oosmos_sLeaf       Idle_State;
};

static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switchtest * pSwitchTest = (switchtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case OpenEvent:
      prtFormatted("Switch %p opened\n", pSwitchTest);
      return true;
    case ClosedEvent:
      prtFormatted("Switch %p closed\n", pSwitchTest);
      return true;
  }

  return false;
}

extern switchtest * switchtestNew(pin * pPin)
{
  oosmos_Allocate(pSwitchTest, switchtest, MAX_SWITCHTESTS, NULL);

  //                                   StateName     Parent        Default
  //                     =====================================================
  oosmos_StateMachineInit(pSwitchTest, StateMachine, NULL,         Idle_State);
    oosmos_LeafInit      (pSwitchTest, Idle_State,   StateMachine            );

  sw * pSwitch = swNew(pPin);

  swSubscribeOpenEvent(pSwitch, oosmos_EventQueue(pSwitchTest), OpenEvent, NULL);
  swSubscribeCloseEvent(pSwitch, oosmos_EventQueue(pSwitchTest), ClosedEvent, NULL);

  return pSwitchTest;
}
