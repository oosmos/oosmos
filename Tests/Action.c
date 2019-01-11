//
// OOSMOS Poll Example
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
//<<<EVENTS

typedef struct testTag test;

static int Count;
static int ActionCount;

struct testTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sComposite A_State;
      oosmos_sLeaf A_AA_State;
      oosmos_sFinal A_Final1_State;
    oosmos_sComposite B_State;
      oosmos_sLeaf B_BB_State;
      oosmos_sFinal B_Final2_State;
    oosmos_sLeaf Exiting_State;
//<<<DECL

};

//>>>CODE
static void OOSMOS_Action1(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  printf("Count: %d\n", Count);
  ActionCount = Count;
  printf("%s\n", oosmos_IsInState(pTest, &pTest->ROOT) ? "SUCCESS" : "FAILURE");
  
  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static bool A_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_EXIT: {
      Count++;
      return true;
    }
    case oosmos_DEFAULT: {
      Count = 0; 
      printf("INITIAL\n");
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_TransitionAction(pTest, pState, B_State, pEvent, OOSMOS_Action1);
    }
  }

  return false;
}

static bool A_AA_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_EXIT: {
      Count++;
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, A_Final1_State);
    }
  }

  return false;
}

static bool B_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      Count++;
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, Exiting_State);
    }
  }

  return false;
}

static bool B_BB_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      Count++;
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, B_Final2_State);
    }
  }

  return false;
}

static bool Exiting_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("%s\n", ActionCount == 2 ? "SUCCESS" : "FAILURE");
      oosmos_EndProgram(1);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pTest, ROOT, NULL, A_State);
    oosmos_CompositeInit(pTest, A_State, ROOT, A_AA_State, A_State_Code);
      oosmos_LeafInit(pTest, A_AA_State, A_State, A_AA_State_Code);
      oosmos_FinalInit(pTest, A_Final1_State, A_State, NULL);
    oosmos_CompositeInit(pTest, B_State, ROOT, B_BB_State, B_State_Code);
      oosmos_LeafInit(pTest, B_BB_State, B_State, B_BB_State_Code);
      oosmos_FinalInit(pTest, B_Final2_State, B_State, NULL);
    oosmos_LeafInit(pTest, Exiting_State, ROOT, Exiting_State_Code);
//<<<INIT

  oosmos_Debug(pTest, true, NULL);

  return pTest;
}

extern int main(void)
{
  (void)testNew();

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(10);
  }
}
