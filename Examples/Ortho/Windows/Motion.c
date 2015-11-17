/*
// OOSMOS motion Example
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

typedef enum
{
  UpCommandEvent = 1,
  DownCommandEvent,
  StopCommandEvent,

  UpperLimitSwitchOpenEvent,
  UpperLimitSwitchClosedEvent,

  LowerLimitSwitchOpenEvent,
  LowerLimitSwitchClosedEvent,
} eEvents;

typedef struct motorTag motor;

struct motorTag
{
  oosmos_sStateMachine   (StateMachine, oosmos_sEvent, 3);
    oosmos_sOrtho         Ortho_State;
      oosmos_sOrthoRegion Limits_State;
        oosmos_sChoice    Limits_Choice_State;
        oosmos_sLeaf      Limits_InBounds_State;
        oosmos_sLeaf      Limits_AtUpperLimit_State;
        oosmos_sLeaf      Limits_AtLowerLimit_State;
      oosmos_sOrthoRegion Motion_State;
        oosmos_sLeaf      Motion_Stopped_State;
        oosmos_sComposite Motion_Moving_State;
          oosmos_sLeaf    Motion_Moving_Up_State;
          oosmos_sLeaf    Motion_Moving_Down_State;
};

static int Failures = 0;

#ifdef oosmos_DEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch(EventCode) {
      NameCase(UpCommandEvent)
      NameCase(DownCommandEvent)
      NameCase(StopCommandEvent)

      NameCase(UpperLimitSwitchClosedEvent)
      NameCase(UpperLimitSwitchOpenEvent)

      NameCase(LowerLimitSwitchClosedEvent)
      NameCase(LowerLimitSwitchOpenEvent)
      default: return "--No Event Name--";
    }
  }
#endif

static bool Limits_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;
 
  switch (pEvent->Code) {
    case LowerLimitSwitchOpenEvent:
      return oosmos_Transition(pRegion, &pMotor->Limits_AtLowerLimit_State);

    case UpperLimitSwitchOpenEvent:
      return oosmos_Transition(pRegion, &pMotor->Limits_AtUpperLimit_State);

    case LowerLimitSwitchClosedEvent:
    case UpperLimitSwitchClosedEvent:
      return oosmos_Transition(pRegion, &pMotor->Limits_InBounds_State);
  }

  return false;
}

static bool Limits_Choice_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (pEvent->Code) {
  case oosmos_ENTER:
/*
//    if (0)                     
//      return oosmos_Transition(pRegion, &pMotor->Limits_AtUpperLimit_State);
//    else if (0)
//      return oosmos_Transition(pRegion, &pMotor->Limits_AtLowerLimit_State);
//    else
*/
        return oosmos_Transition(pRegion, &pMotor->Limits_InBounds_State);
  }

  return false;
}

static bool Motion_Stopped_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (pEvent->Code) {
    case UpCommandEvent:
      return oosmos_Transition(pRegion, &pMotor->Motion_Moving_Up_State);
    case DownCommandEvent:
      return oosmos_Transition(pRegion, &pMotor->Motion_Moving_Down_State);
  }

  return false;
}

static bool Motion_Moving_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (pEvent->Code) {
    case StopCommandEvent:
      return oosmos_Transition(pRegion, &pMotor->Motion_Stopped_State);
  }

  return false;
}

static bool Motion_Moving_Up_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (pEvent->Code) {
    case DownCommandEvent:
      return oosmos_Transition(pRegion, &pMotor->Motion_Moving_Down_State);
    case UpperLimitSwitchOpenEvent:
      return oosmos_Transition(pRegion, &pMotor->Motion_Stopped_State);
  }

  return false;
}

static bool Motion_Moving_Down_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  motor * pMotor = (motor *) pObject;

  switch (pEvent->Code) {
    case UpCommandEvent:
      return oosmos_Transition(pRegion, &pMotor->Motion_Moving_Up_State);
    case LowerLimitSwitchOpenEvent:
      return oosmos_Transition(pRegion, &pMotor->Motion_Stopped_State);
  }

  return false;
}

static motor * motorNew(void)
{
  oosmos_Allocate(pMotor, motor, 2, NULL);

  /*                                          StateName                  Parent              Default               */
  /*                                 ============================================================================= */
  oosmos_StateMachineInit            (pMotor, StateMachine,              NULL,               Ortho_State         );
    oosmos_OrthoInitNoCode           (pMotor, Ortho_State,               StateMachine                            );
      oosmos_OrthoRegionInit         (pMotor, Limits_State,              Ortho_State,        Limits_Choice_State );
        oosmos_ChoiceInit            (pMotor, Limits_Choice_State,       Limits_State                            );
        oosmos_LeafInitNoCode        (pMotor, Limits_InBounds_State,     Limits_State                            );
        oosmos_LeafInitNoCode        (pMotor, Limits_AtUpperLimit_State, Limits_State                            );
        oosmos_LeafInitNoCode        (pMotor, Limits_AtLowerLimit_State, Limits_State                            );
      oosmos_OrthoRegionInitNoCode   (pMotor, Motion_State,              Ortho_State,        Motion_Stopped_State);
        oosmos_LeafInit              (pMotor, Motion_Stopped_State,      Motion_State                            );
        oosmos_CompositeInitNoDefault(pMotor, Motion_Moving_State,       Motion_State                            );
          oosmos_LeafInit            (pMotor, Motion_Moving_Up_State,    Motion_Moving_State                     );
          oosmos_LeafInit            (pMotor, Motion_Moving_Down_State,  Motion_Moving_State                     );

  oosmos_Debug(&pMotor->StateMachine, true, EventNames);

  return pMotor;
}

static void QueueEvent(motor * pMotor, int EventCode)
{
  oosmos_SendEvent(pMotor, EventCode); 
  oosmos_RunStateMachine(&pMotor->StateMachine); 
}

static void TestState(motor * pMotor, void * pState)
{
  if (!oosmos_IsInState(&pMotor->StateMachine, pState)) {
    printf("FAILURE.\n");
    Failures += 1;
  }
}

extern int main(void)
{
  motor * pMotor = motorNew();

  QueueEvent(pMotor, UpCommandEvent);
  TestState(pMotor, &pMotor->Motion_Moving_Up_State);

  QueueEvent(pMotor, UpperLimitSwitchOpenEvent);
  TestState(pMotor, &pMotor->Motion_Stopped_State);
  TestState(pMotor, &pMotor->Limits_AtUpperLimit_State);

  QueueEvent(pMotor, DownCommandEvent);
  TestState(pMotor, &pMotor->Motion_Moving_Down_State);

  QueueEvent(pMotor, LowerLimitSwitchClosedEvent);
  TestState(pMotor, &pMotor->Limits_InBounds_State);

  QueueEvent(pMotor, UpCommandEvent);
  TestState(pMotor, &pMotor->Motion_Moving_Up_State);

  QueueEvent(pMotor, StopCommandEvent);
  TestState(pMotor, &pMotor->Motion_Stopped_State);

  if (Failures > 0) {
    printf("%d FAILURES\n", Failures);
  }
  else {
    printf("\n");
    printf("SUCCESS!\n");
  }

  return 0;
}
