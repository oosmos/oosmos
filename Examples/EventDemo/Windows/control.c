/*
// OOSMOS EventDemo example, control object
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

#include <stdlib.h>
#include <stdio.h>

#include "oosmos.h"
#include "control.h"
#include "key.h"
#include "motor.h"
#include "pump.h"

typedef enum
{
  StopPressedEvent = 1,
  StopReleasedEvent,
  MovePressedEvent,
  PumpPressedEvent,
  Option1PressedEvent,
  Option2PressedEvent,
  QuitPressedEvent,
} eEvents;

struct controlTag
{
  motor * m_pMotor;
  pump  * m_pPump;
  bool    m_Option1;
  bool    m_Option2;

  oosmos_sStateMachine   (StateMachine, oosmos_sEvent, 3);
    oosmos_sLeaf          StartingUp_State;
    oosmos_sOrtho         Operational_State;
      oosmos_sOrthoRegion Operational_MotorControl_State;
        oosmos_sLeaf      Operational_MotorControl_Idle_State;
        oosmos_sLeaf      Operational_MotorControl_Moving_State;
      oosmos_sOrthoRegion Operational_PumpControl_State;
        oosmos_sLeaf      Operational_PumpControl_Idle_State;
        oosmos_sLeaf      Operational_PumpControl_Pumping_State;
    oosmos_sLeaf          StopPressed_State;
    oosmos_sLeaf          Termination_State;
};

#ifdef oosmos_DEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch (EventCode) {
      NameCase(StopPressedEvent)
      NameCase(StopReleasedEvent)
      NameCase(MovePressedEvent)
      NameCase(PumpPressedEvent)
      NameCase(QuitPressedEvent)
      NameCase(Option1PressedEvent)
      NameCase(Option2PressedEvent)
      default: return "--No Event Name--";
    }
  }
#endif

static void ToggleOption1(control * pControl)
{
  pControl->m_Option1 = !(pControl->m_Option1);
}

static void ToggleOption2(control * pControl)
{
  pControl->m_Option2 = !(pControl->m_Option2);
}

static bool StartingUp_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
       return oosmos_StateTimeoutSeconds(pRegion, 1);
    case oosmos_TIMEOUT:
      return oosmos_Transition(pRegion, &pControl->Operational_State);
  }

  return false;
}

static bool Operational_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (pEvent->Code) {
    case StopPressedEvent:
      return oosmos_Transition(pRegion, &pControl->StopPressed_State);
    case QuitPressedEvent:
      return oosmos_Transition(pRegion, &pControl->Termination_State);
  }

  return false;
}

static bool Operational_MotorControl_Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (pEvent->Code) {
    case MovePressedEvent:
      return oosmos_Transition(pRegion, &pControl->Operational_MotorControl_Moving_State);
  }

  return false;
}

static bool Operational_MotorControl_Moving_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      motorOn(pControl->m_pMotor);
      return true;
    case oosmos_EXIT:
      motorOff(pControl->m_pMotor);
      return true;
    case MovePressedEvent:
      return oosmos_Transition(pRegion, &pControl->Operational_MotorControl_State);
  }

  return false;
}

static bool Operational_PumpControl_Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (pEvent->Code) {
    case PumpPressedEvent:
      return oosmos_Transition(pRegion, &pControl->Operational_PumpControl_Pumping_State);
  }

  return false;
}

static bool Operational_PumpControl_Pumping_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      pumpOn(pControl->m_pPump);
      return true;
    case oosmos_EXIT:
      pumpOff(pControl->m_pPump);
      return true;
    case PumpPressedEvent:
      return oosmos_Transition(pRegion, &pControl->Operational_PumpControl_State);
  }

  return false;
}

static bool StopPressed_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (pEvent->Code) {
    case StopReleasedEvent:
      return oosmos_Transition(pRegion, &pControl->Operational_State);
    case Option1PressedEvent:
      ToggleOption1(pControl);

      printf("control: Option1: %s\n", pControl->m_Option1 ? "true" : "false");
      return true;
    case Option2PressedEvent:
      ToggleOption2(pControl);

      printf("control: Option2: %s\n", pControl->m_Option2 ? "true" : "false");
      return true;
  }

  return false;
}

static bool Termination_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  switch (pEvent->Code) {
    case oosmos_ENTER:
      exit(1);
  }

  return false;
}

extern control * controlNew(void)
{
  oosmos_Allocate(pControl, control, 1, NULL);

  key * pStopKey    = keyNew('s');
  key * pMoveKey    = keyNew('m');
  key * pQuitKey    = keyNew('q');

  key * pPumpKey    = keyNew('p');
  key * pUpKey      = keyNew('u');
  key * pDownKey    = keyNew('d');
  key * pOption1Key = keyNew('1');
  key * pOption2Key = keyNew('2');

  {
    oosmos_sQueue * const pControlEventQueue = &pControl->EventQueue;

    keySubscribePressedEvent(pStopKey,    pControlEventQueue, StopPressedEvent);
    keySubscribeReleasedEvent(pStopKey,   pControlEventQueue, StopReleasedEvent);
    keySubscribePressedEvent(pMoveKey,    pControlEventQueue, MovePressedEvent);
    keySubscribePressedEvent(pPumpKey,    pControlEventQueue, PumpPressedEvent);
    keySubscribePressedEvent(pQuitKey,    pControlEventQueue, QuitPressedEvent);
    keySubscribePressedEvent(pOption1Key, pControlEventQueue, Option1PressedEvent);
    keySubscribePressedEvent(pOption2Key, pControlEventQueue, Option2PressedEvent);
  }

  pControl->m_pMotor  = motorNew();
  pControl->m_pPump   = pumpNew(pUpKey, pDownKey);
  pControl->m_Option1 = false;
  pControl->m_Option2 = false;

  /*                                         StateName                              Parent                          Default                              */
  /*                              ====================================================================================================================== */
  oosmos_StateMachineInit         (pControl, StateMachine,                          NULL,                           StartingUp_State                   );
    oosmos_LeafInit               (pControl, StartingUp_State,                      StateMachine                                                       );
    oosmos_OrthoInit              (pControl, Operational_State,                     StateMachine                                                       );
      oosmos_OrthoRegionInitNoCode(pControl, Operational_MotorControl_State,        Operational_State,              Operational_MotorControl_Idle_State);
        oosmos_LeafInit           (pControl, Operational_MotorControl_Idle_State,   Operational_MotorControl_State                                     );
        oosmos_LeafInit           (pControl, Operational_MotorControl_Moving_State, Operational_MotorControl_State                                     );
      oosmos_OrthoRegionInitNoCode(pControl, Operational_PumpControl_State,         Operational_State,              Operational_PumpControl_Idle_State );
        oosmos_LeafInit           (pControl, Operational_PumpControl_Idle_State,    Operational_PumpControl_State                                      );
        oosmos_LeafInit           (pControl, Operational_PumpControl_Pumping_State, Operational_PumpControl_State                                      );
    oosmos_LeafInit               (pControl, StopPressed_State,                     StateMachine                                                       );
    oosmos_LeafInit               (pControl, Termination_State,                     StateMachine                                                       );

  oosmos_DebugCode(
    oosmos_Debug(&pControl->StateMachine, true, EventNames);
  )

  return pControl;
}
