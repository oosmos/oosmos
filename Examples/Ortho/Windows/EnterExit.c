/*
// OOSMOS EnterExit Example
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
  TweakEvent = 1,
} eEvents;

typedef struct testTag test;

struct testTag
{
  oosmos_sStateMachine   (StateMachine, oosmos_sEvent, 3);
    oosmos_sOrtho         Active_State;
      oosmos_sOrthoRegion Active_Top_State;
        oosmos_sLeaf      Active_Top_Moving_State;
      oosmos_sOrthoRegion Active_Middle_State;
        oosmos_sComposite Active_Middle_Idle_State;
          oosmos_sLeaf    Active_Middle_Idle_Inner_State;
      oosmos_sOrthoRegion Active_Base_State;
        oosmos_sLeaf      Active_Base_Leaf_State;
        oosmos_sLeaf      Active_Base_Running_State;
};

static bool Active_Middle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case TweakEvent:
      return oosmos_Transition(pRegion, &pTest->Active_Middle_Idle_State);
  }

  return false;
}

static bool Active_Base_Leaf_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_COMPLETE:
      return oosmos_Transition(pRegion, &pTest->Active_Base_Running_State);
  }

  return false;
}

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

  /*                                      StateName                       Parent                    Default                         */
  /*                              ================================================================================================= */
  oosmos_StateMachineInit         (pTest, StateMachine,                   NULL,                     Active_State                  );
    oosmos_OrthoInitNoCode        (pTest, Active_State,                   StateMachine                                            );
      oosmos_OrthoRegionInitNoCode(pTest, Active_Top_State,               Active_State,             Active_Top_Moving_State       );
        oosmos_LeafInitNoCode     (pTest, Active_Top_Moving_State,        Active_Top_State                                        );
      oosmos_OrthoRegionInit      (pTest, Active_Middle_State,            Active_State,             Active_Middle_Idle_State      );
        oosmos_CompositeInitNoCode(pTest, Active_Middle_Idle_State,       Active_Middle_State,      Active_Middle_Idle_Inner_State);
          oosmos_LeafInitNoCode   (pTest, Active_Middle_Idle_Inner_State, Active_Middle_Idle_State                                );
      oosmos_OrthoRegionInitNoCode(pTest, Active_Base_State,              Active_State,             Active_Base_Leaf_State        );
        oosmos_LeafInit           (pTest, Active_Base_Leaf_State,         Active_Base_State                                       );
        oosmos_LeafInitNoCode     (pTest, Active_Base_Running_State,      Active_Base_State                                       );

  oosmos_Debug(&pTest->StateMachine, true, NULL);

  return pTest;
}

static void QueueEventRoutine(test * pTest, int EventCode)
{
  oosmos_SendEvent(pTest, EventCode);
  oosmos_RunStateMachines();
}

extern int main(void)
{
  test * pTest = testNew(); 
  QueueEventRoutine(pTest, TweakEvent);

  if (oosmos_IsInState(&pTest->StateMachine, &pTest->Active_Middle_Idle_Inner_State))
    printf("SUCCESS\n");
  else 
    printf("FAILURE\n");

  return 0;
}
