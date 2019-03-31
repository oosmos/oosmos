//
// OOSMOS Completion Example
//
// Copyright (C) 2014-2019  OOSMOS, LLC
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
#include <stdbool.h>
#include <stdio.h>

//>>>EVENTS
enum {
  evA = 1,
  evB = 2,
  evStop = 3
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case evA: return "evA";
      case evB: return "evB";
      case evStop: return "evStop";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef struct testTag test;

typedef union {
  oosmos_sEvent Base;
} uEvents;

struct testTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sOrtho Ortho_State;
      oosmos_sOrthoRegion Ortho_Region1_State;
        oosmos_sLeaf Ortho_Region1_Moving_State;
        oosmos_sLeaf Ortho_Region1_Idle_State;
        oosmos_sFinal Ortho_Region1_Final1_State;
      oosmos_sOrthoRegion Ortho_Region2_State;
        oosmos_sLeaf Ortho_Region2_Idle_State;
        oosmos_sLeaf Ortho_Region2_Moving_State;
        oosmos_sFinal Ortho_Region2_Final2_State;
    oosmos_sLeaf Complete_State;
//<<<DECL
};

//>>>CODE
static bool Ortho_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, Complete_State);
    }
  }

  return false;
}

static bool Ortho_Region1_Moving_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evStop: {
      return oosmos_Transition(pTest, pState, Ortho_Region1_Final1_State);
    }
  }

  return false;
}

static bool Ortho_Region1_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evA: {
      return oosmos_Transition(pTest, pState, Ortho_Region1_Moving_State);
    }
  }

  return false;
}

static bool Ortho_Region2_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evB: {
      return oosmos_Transition(pTest, pState, Ortho_Region2_Moving_State);
    }
  }

  return false;
}

static bool Ortho_Region2_Moving_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evStop: {
      return oosmos_Transition(pTest, pState, Ortho_Region2_Final2_State);
    }
  }

  return false;
}

static bool Complete_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("\n--Goal State Achieved--\n");
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInit(pTest, ROOT, NULL, Ortho_State);
    oosmos_OrthoInit(pTest, Ortho_State, ROOT, Ortho_State_Code);
      oosmos_OrthoRegionInit(pTest, Ortho_Region1_State, Ortho_State, Ortho_Region1_Idle_State, NULL);
        oosmos_LeafInit(pTest, Ortho_Region1_Moving_State, Ortho_Region1_State, Ortho_Region1_Moving_State_Code);
        oosmos_LeafInit(pTest, Ortho_Region1_Idle_State, Ortho_Region1_State, Ortho_Region1_Idle_State_Code);
        oosmos_FinalInit(pTest, Ortho_Region1_Final1_State, Ortho_Region1_State, NULL);
      oosmos_OrthoRegionInit(pTest, Ortho_Region2_State, Ortho_State, Ortho_Region2_Idle_State, NULL);
        oosmos_LeafInit(pTest, Ortho_Region2_Idle_State, Ortho_Region2_State, Ortho_Region2_Idle_State_Code);
        oosmos_LeafInit(pTest, Ortho_Region2_Moving_State, Ortho_Region2_State, Ortho_Region2_Moving_State_Code);
        oosmos_FinalInit(pTest, Ortho_Region2_Final2_State, Ortho_Region2_State, NULL);
    oosmos_LeafInit(pTest, Complete_State, ROOT, Complete_State_Code);

  oosmos_Debug(pTest, OOSMOS_EventNames);
//<<<INIT

  return pTest;
}

static void QueueEvent(const test * pTest, int EventCode)
{
  oosmos_PushEventCode(pTest, EventCode);
  oosmos_RunStateMachines();
}

extern int main(void)
{
  test * pTest = testNew();

  QueueEvent(pTest, evA);
  QueueEvent(pTest, evB);
  QueueEvent(pTest, evStop);

  if (oosmos_IsInState(pTest, &pTest->Complete_State))
    printf("SUCCESS\n");
  else
    printf("FAILURE\n");

  return 0;
}
