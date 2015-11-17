/*
// OOSMOS - EventDemo example, pump object
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

#include <stddef.h>
#include <stdio.h>
#include "oosmos.h"
#include "pump.h"

typedef enum
{
  StartEvent = 1,
  StopEvent,

  UpPressedEvent,
  DownPressedEvent,
} eEvents;

struct pumpTag 
{
  int PumpSpeed;

  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 3);
    oosmos_sLeaf       Idle_State;
    oosmos_sLeaf       Pumping_State;
};

#ifndef pumpMAX
#define pumpMAX 3
#endif

#ifdef oosmos_DEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch (EventCode) {
      NameCase(StartEvent)
      NameCase(StopEvent)
      NameCase(UpPressedEvent)
      NameCase(DownPressedEvent)
      default: return "--No Event Name--";
    }
  }
#endif

static bool Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  pump * pPump = (pump *) pObject;

  switch (pEvent->Code) {
    case StartEvent:
      return oosmos_Transition(pRegion, &pPump->Pumping_State);
  }

  return false;
}

static bool Pumping_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  pump * pPump = (pump *) pObject;

  switch (pEvent->Code) {
    case StopEvent:
      return oosmos_Transition(pRegion, &pPump->Idle_State);

    case UpPressedEvent:
      pPump->PumpSpeed = oosmos_Min(10, pPump->PumpSpeed+1);
      printf("pump: PUMPING SPEED %d...\n", pPump->PumpSpeed);
      return true;

    case DownPressedEvent:
      pPump->PumpSpeed = oosmos_Max(1, pPump->PumpSpeed-1);
      printf("pump: PUMPING SPEED %d...\n", pPump->PumpSpeed);
      return true;

    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        while (true) {
          printf("pump: PUMPING...\n");
          oosmos_SyncDelayMS(pRegion, (10-pPump->PumpSpeed) * 200);
        }
      oosmos_SyncEnd(pRegion);
      return true;
  }

  return false;
}

extern pump * pumpNew(key * pUpKey, key * pDownKey)
{
  oosmos_Allocate(pPump, pump, pumpMAX, NULL);

  pPump->PumpSpeed = 5;

  keySubscribePressedEvent(pUpKey,   &pPump->EventQueue, UpPressedEvent);
  keySubscribePressedEvent(pDownKey, &pPump->EventQueue, DownPressedEvent);

  /*                             StateName      Parent        Default     */
  /*                     ================================================ */
  oosmos_StateMachineInit(pPump, StateMachine,  NULL,         Idle_State);
    oosmos_LeafInit      (pPump, Idle_State,    StateMachine            );
    oosmos_LeafInit      (pPump, Pumping_State, StateMachine            );

  oosmos_DebugCode(
    oosmos_Debug(&pPump->StateMachine, true, EventNames);
  )

  return pPump;
}

extern void pumpOn(pump * pMotor)
{
  oosmos_SendEvent(pMotor, StartEvent);
}

extern void pumpOff(pump * pMotor)
{
  oosmos_SendEvent(pMotor, StopEvent);
}
