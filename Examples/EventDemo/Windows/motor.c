//
// OOSMOS - EventDemo example, motor object
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
#include "motor.h"
#include <stdbool.h>
#include <stdio.h>

//>>>EVENTS
enum {
  evStart = 1,
  evStop = 2
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case evStart: return "evStart";
      case evStop: return "evStop";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef union {
  oosmos_sEvent Event;
} uEvents;

struct motorTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sLeaf Idle_State;
    oosmos_sLeaf Moving_State;
//<<<DECL
};

#ifndef motorMAX
#define motorMAX 3
#endif

static void MovingThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("motor: MOVING...\n");
      oosmos_ThreadDelayMS(500);
    }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evStart: {
      return oosmos_Transition(pMotor, pState, Moving_State);
    }
  }

  return false;
}

static bool Moving_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      MovingThread(pState);
      return true;
    }
    case evStop: {
      return oosmos_Transition(pMotor, pState, Idle_State);
    }
  }

  return false;
}
//<<<CODE

extern motor * motorNew(void)
{
  oosmos_Allocate(pMotor, motor, motorMAX, NULL);

//>>>INIT
  oosmos_StateMachineInit(pMotor, ROOT, NULL, Idle_State);
    oosmos_LeafInit(pMotor, Idle_State, ROOT, Idle_State_Code);
    oosmos_LeafInit(pMotor, Moving_State, ROOT, Moving_State_Code);

  oosmos_Debug(pMotor, OOSMOS_EventNames);
//<<<INIT

  return pMotor;
}

extern void motorOn(const motor * pMotor)
{
  oosmos_PushEventCode(pMotor, evStart);
}

extern void motorOff(const motor * pMotor)
{
  oosmos_PushEventCode(pMotor, evStop);
}
