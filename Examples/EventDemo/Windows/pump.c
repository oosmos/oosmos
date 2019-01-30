//
// OOSMOS - EventDemo example, pump object
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

#include "oosmos.h"
#include "pump.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

//>>>EVENTS
enum {
  evDownPressed = 1,
  evStart = 2,
  evStop = 3,
  evUpPressed = 4
};
//<<<EVENTS

typedef union {
  oosmos_sEvent Event;
} uEvents;

struct pumpTag
{
  uint32_t PumpSpeed;

//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sLeaf Idle_State;
    oosmos_sLeaf Pumping_State;
//<<<DECL
};

#ifndef pumpMAX
#define pumpMAX 3
#endif

#ifdef oosmos_DEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch (EventCode) {
      NameCase(evStart)
      NameCase(evStop)
      NameCase(evUpPressed)
      NameCase(evDownPressed)
      default: return "--No Event Name--";
    }
  }
#endif

static void Thread(pump * pPump, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("PUMPING...\n");
      oosmos_ThreadDelayMS(oosmos_Max(100, (10 - pPump->PumpSpeed) * 200));
    }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  pump * pPump = (pump *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evStart: {
      return oosmos_Transition(pPump, pState, Pumping_State);
    }
  }

  return false;
}

static bool Pumping_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  pump * pPump = (pump *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      Thread(pPump, pState);
      return true;
    }
    case evUpPressed: {
      pPump->PumpSpeed = oosmos_Min(10, pPump->PumpSpeed+1);
      return true;
    }
    case evDownPressed: {
      pPump->PumpSpeed = oosmos_Max(1, pPump->PumpSpeed-1);
      return true;
    }
    case evStop: {
      return oosmos_Transition(pPump, pState, Idle_State);
    }
  }

  return false;
}
//<<<CODE

extern pump * pumpNew(sw * pUpSwitch, sw * pDownSwitch)
{
  oosmos_Allocate(pPump, pump, pumpMAX, NULL);

  pPump->PumpSpeed = 5;

  swSubscribeCloseEvent(pUpSwitch,   oosmos_EventQueue(pPump), evUpPressed,   NULL);
  swSubscribeCloseEvent(pDownSwitch, oosmos_EventQueue(pPump), evDownPressed, NULL);

//>>>INIT
  oosmos_StateMachineInit(pPump, ROOT, NULL, Idle_State);
    oosmos_LeafInit(pPump, Idle_State, ROOT, Idle_State_Code);
    oosmos_LeafInit(pPump, Pumping_State, ROOT, Pumping_State_Code);
//<<<INIT

#if 1
  oosmos_Debug(pPump, true, EventNames);
#endif

  return pPump;
}

extern void pumpOn(const pump * pPump)
{
  oosmos_PushEventCode(pPump, evStart);
}

extern void pumpOff(const pump * pPump)
{
  oosmos_PushEventCode(pPump, evStop);
}
