//
// OOSMOS switchtest Class
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

//>>>EVENTS
enum {
  evClosed = 1,
  evOpen = 2
};

static const char * OOSMOS_EventNames(int EventCode)
{
  switch (EventCode) {
    case evClosed: return "evClosed";
    case evOpen: return "evOpen";
    default: return "--No Event Name--";
  }
}
//<<<EVENTS

typedef union {
    oosmos_sEvent Base;
} uEvents;

struct switchtestTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sLeaf Idle_State;
//<<<DECL
};

//>>>CODE
static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case evOpen: {
      printf("Open\n");
      return true;
    }
    case evClosed: {
      printf("Close\n");
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE

extern switchtest * switchtestNew(pin * pPin)
{
  oosmos_Allocate(pSwitchTest, switchtest, MAX_SWITCHTESTS, NULL);

//>>>INIT
  oosmos_StateMachineInit(pSwitchTest, ROOT, NULL, Idle_State);
    oosmos_LeafInit(pSwitchTest, Idle_State, ROOT, Idle_State_Code);

  oosmos_Debug(pSwitchTest, OOSMOS_EventNames);
//<<<INIT

  sw * pSwitch = swNew(pPin);

  swSubscribeOpenEvent(pSwitch, oosmos_EventQueue(pSwitchTest), evOpen, NULL);
  swSubscribeCloseEvent(pSwitch, oosmos_EventQueue(pSwitchTest), evClosed, NULL);

  return pSwitchTest;
}
