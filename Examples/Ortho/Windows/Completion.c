/*
// OOSMOS Completion Example
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
  MoveA_Event = 1,
  MoveB_Event,
  Stop_Event,
} eEvents;

typedef struct testTag test;

struct testTag
{
  oosmos_sStateMachine   (StateMachine, oosmos_sEvent, 10);
    oosmos_sOrtho         Ortho_State;
      oosmos_sOrthoRegion Ortho_RegionA_State;
        oosmos_sLeaf      Ortho_RegionA_Idle_State;
        oosmos_sLeaf      Ortho_RegionA_Moving_State;
        oosmos_sFinal     Ortho_RegionA_Final_State;
      oosmos_sOrthoRegion Ortho_RegionB_State;
        oosmos_sLeaf      Ortho_RegionB_Idle_State;
        oosmos_sLeaf      Ortho_RegionB_Moving_State;
        oosmos_sFinal     Ortho_RegionB_Final_State;
    oosmos_sLeaf          CompleteState;
};

#ifdef oosmos_DEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch(EventCode) {
      NameCase(MoveA_Event)
      NameCase(MoveB_Event)
      NameCase(Stop_Event)
      default: return "--No Event Name--";
    }
  }
#endif

static bool Ortho_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_COMPLETE:
      return oosmos_Transition(pRegion, &pTest->CompleteState);
  }

  return false;
}

static bool Ortho_RegionA_Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case MoveA_Event:
      return oosmos_Transition(pRegion, &pTest->Ortho_RegionA_Moving_State);
  }

  return false;
}

static bool Ortho_RegionA_Moving_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case Stop_Event:
      return oosmos_Transition(pRegion, &pTest->Ortho_RegionA_Final_State);
  }

  return false;
}

static bool Ortho_RegionB_Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case MoveB_Event:
      return oosmos_Transition(pRegion, &pTest->Ortho_RegionB_Moving_State);
  }

  return false;
}

static bool Ortho_RegionB_Moving_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case Stop_Event:
      return oosmos_Transition(pRegion, &pTest->Ortho_RegionB_Final_State);
  }

  return false;
}

static bool CompleteState_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  switch (pEvent->Code) {
    case oosmos_ENTER:
      printf("\n--Goal State Achieved--\n");
      return true;
  }

  return false;
}

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

  /*                                      StateName                   Parent                Default                   */
  /*                              =================================================================================== */
  oosmos_StateMachineInit         (pTest, StateMachine,               NULL,                 Ortho_State             );
    oosmos_OrthoInit              (pTest, Ortho_State,                StateMachine                                  );
      oosmos_OrthoRegionInitNoCode(pTest, Ortho_RegionA_State,        Ortho_State,          Ortho_RegionA_Idle_State);
        oosmos_LeafInit           (pTest, Ortho_RegionA_Idle_State,   Ortho_RegionA_State                           );
        oosmos_LeafInit           (pTest, Ortho_RegionA_Moving_State, Ortho_RegionA_State                           );
        oosmos_FinalInitNoCode    (pTest, Ortho_RegionA_Final_State,  Ortho_RegionA_State                           );
      oosmos_OrthoRegionInitNoCode(pTest, Ortho_RegionB_State,        Ortho_State,          Ortho_RegionB_Idle_State);
        oosmos_LeafInit           (pTest, Ortho_RegionB_Idle_State,   Ortho_RegionB_State                           );
        oosmos_LeafInit           (pTest, Ortho_RegionB_Moving_State, Ortho_RegionB_State                           );
        oosmos_FinalInitNoCode    (pTest, Ortho_RegionB_Final_State,  Ortho_RegionB_State                           );
    oosmos_LeafInit               (pTest, CompleteState,              StateMachine                                  );

  oosmos_Debug(&pTest->StateMachine, true, EventNames);

  return pTest;
}

static void QueueEvent(test * pTest, int EventCode)
{
  oosmos_SendEvent(pTest, EventCode);
  oosmos_RunStateMachines();
}

extern int main(void)
{
  test * pTest = testNew();

  QueueEvent(pTest, MoveA_Event);
  QueueEvent(pTest, MoveB_Event);
  QueueEvent(pTest, Stop_Event);

  if (oosmos_IsInState(&pTest->StateMachine, &pTest->CompleteState))
    printf("SUCCESS\n");
  else
    printf("FAILURE\n");

  return 0;
}
