/*
// OOSMOS switchtest Class
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

#include "oosmos.h"
#include "pin.h"
#include "sw.h"
#include "prt.h"
#include "switchtest.h"

#ifndef MAX_SWITCHTESTS 
#define MAX_SWITCHTESTS 2
#endif

typedef enum {
  OpenEvent = 1,
  ClosedEvent,
} eEvents;

struct switchtestTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 2);
    oosmos_sLeaf       Idle_State;
};

static bool Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  switchtest * pSwitchTest = (switchtest *) pObject;
  
  switch (pEvent->Code) {
    case OpenEvent:
      return prtFormatted("Switch %p opened\n", pSwitchTest);
    case ClosedEvent:
      return prtFormatted("Switch %p closed\n", pSwitchTest);
  }

  return false;
}

extern switchtest * switchtestNew(pin * pPin)
{
  oosmos_Allocate(pSwitchTest, switchtest, MAX_SWITCHTESTS, NULL);
  
  /*                                   StateName     Parent        Default     */
  /*                     ===================================================== */
  oosmos_StateMachineInit(pSwitchTest, StateMachine, NULL,         Idle_State);
    oosmos_LeafInit      (pSwitchTest, Idle_State,   StateMachine            );
  
  sw * pSwitch = swNew(pPin);
  
  swSubscribeOpenEvent(pSwitch, &pSwitchTest->EventQueue, OpenEvent, NULL);
  swSubscribeCloseEvent(pSwitch, &pSwitchTest->EventQueue, ClosedEvent, NULL);
  
  return pSwitchTest;
}
