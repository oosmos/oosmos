//
// OOSMOS EnterExit Example
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
#include <stdbool.h>
#include <stdio.h>

//>>>EVENTS
enum {
  evTweak = 1
};

static const char * EventNames(int EventCode)
{
  switch (EventCode) {
    case evTweak: return "evTweak";
    default: return "--No Event Name--";
  }
}
//<<<EVENTS

typedef struct testTag test;

typedef union {
  oosmos_sEvent Base;
} uEvents;

struct testTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sOrtho Active_State;
      oosmos_sOrthoRegion Active_Region1_State;
        oosmos_sLeaf Active_Region1_Moving_State;
      oosmos_sOrthoRegion Active_Region2_State;
        oosmos_sComposite Active_Region2_Outer_State;
          oosmos_sLeaf Active_Region2_Outer_Inner_State;
      oosmos_sOrthoRegion Active_Region3_State;
        oosmos_sLeaf Active_Region3_Leaf_State;
        oosmos_sLeaf Active_Region3_Running_State;
//<<<DECL
};

//>>>CODE
static bool Active_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evTweak: {
      return oosmos_Transition(pTest, pState, Active_Region2_Outer_State);
    }
  }

  return false;
}

static bool Active_Region3_Leaf_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, Active_Region3_Running_State);
    }
  }

  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInit(pTest, ROOT, NULL, Active_State);
    oosmos_OrthoInit(pTest, Active_State, ROOT, Active_State_Code);
      oosmos_OrthoRegionInit(pTest, Active_Region1_State, Active_State, Active_Region1_Moving_State, NULL);
        oosmos_LeafInit(pTest, Active_Region1_Moving_State, Active_Region1_State, NULL);
      oosmos_OrthoRegionInit(pTest, Active_Region2_State, Active_State, Active_Region2_Outer_State, NULL);
        oosmos_CompositeInit(pTest, Active_Region2_Outer_State, Active_Region2_State, Active_Region2_Outer_Inner_State, NULL);
          oosmos_LeafInit(pTest, Active_Region2_Outer_Inner_State, Active_Region2_Outer_State, NULL);
      oosmos_OrthoRegionInit(pTest, Active_Region3_State, Active_State, Active_Region3_Leaf_State, NULL);
        oosmos_LeafInit(pTest, Active_Region3_Leaf_State, Active_Region3_State, Active_Region3_Leaf_State_Code);
        oosmos_LeafInit(pTest, Active_Region3_Running_State, Active_Region3_State, NULL);

  oosmos_Debug(pTest, EventNames);
//<<<INIT

  return pTest;
}

static void QueueEventRoutine(const test * pTest, int EventCode)
{
  oosmos_PushEventCode(pTest, EventCode);
  oosmos_RunStateMachines();
}

extern int main(void)
{
  test * pTest = testNew();
  QueueEventRoutine(pTest, evTweak);

  if (oosmos_IsInState(pTest, &pTest->Active_Region2_Outer_Inner_State))
    printf("SUCCESS\n");
  else
    printf("FAILURE\n");

  return 0;
}
