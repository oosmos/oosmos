/*
// OOSMOS - EventDemo example, motor object
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

#include <stdio.h>
#include "oosmos.h"
#include "motor.h"

typedef enum
{
  StartEvent = 1,
  StopEvent,
} eEvents;

struct motorTag 
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 3);
    oosmos_sLeaf       Idle_State;
    oosmos_sLeaf       Moving_State;
};

#ifndef motorMAX
#define motorMAX 3
#endif

#ifdef oosmos_DEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch (EventCode) {
      NameCase(StartEvent)
      NameCase(StopEvent)
      default: return "--No Event Name--";
    }
  }
#endif

static bool Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (pEvent->Code) {
    case StartEvent:
      return oosmos_Transition(pRegion, &pMotor->Moving_State);
  }

  return false;
}

static bool Moving_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        while (true) {
          printf("motor: MOVING...\n");
          oosmos_SyncDelayMS(pRegion, 500);
        }
      oosmos_SyncEnd(pRegion);
      return true;
     
    case StopEvent:
      return oosmos_Transition(pRegion, &pMotor->Idle_State);
  }

  return false;
}

extern motor * motorNew(void)
{
  oosmos_Allocate(pMotor, motor, motorMAX, NULL);

  /*                              StateName     Parent        Default     */
  /*                     ================================================ */
  oosmos_StateMachineInit(pMotor, StateMachine, NULL,         Idle_State);
    oosmos_LeafInit      (pMotor, Idle_State,   StateMachine            );
    oosmos_LeafInit      (pMotor, Moving_State, StateMachine            );

  oosmos_DebugCode(
    oosmos_Debug(&pMotor->StateMachine, true, EventNames);
  )

  return pMotor;
}

extern void motorOn(motor * pMotor)
{
  oosmos_SendEvent(pMotor, StartEvent);
}

extern void motorOff(motor * pMotor)
{
  oosmos_SendEvent(pMotor, StopEvent);
}
