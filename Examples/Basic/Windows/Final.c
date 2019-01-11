//
// OOSMOS Final Example
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
#include <stdio.h>
#include <stdbool.h>

//<<<EVENTS
//>>>EVENTS

typedef struct testTag test;

struct testTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sComposite A_State;
      oosmos_sLeaf A_Choice1_State;
      oosmos_sLeaf A_Left_State;
      oosmos_sLeaf A_Right_State;
      oosmos_sFinal A_Final1_State;
    oosmos_sLeaf B_State;
//<<<DECL
};

static bool IsLeft(void)
{
  return true;
}

static void Default(void)
{
  printf("In Default\n");
}

//>>>CODE
static bool A_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_DEFAULT: {
      Default();
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, B_State);
    }
  }

  return false;
}

static bool A_Choice1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  if (oosmos_EventCode(pEvent) == oosmos_ENTER) {
    if (IsLeft()) {
      return oosmos_Transition(pTest, pState, A_Left_State);
    }
    else {
      return oosmos_Transition(pTest, pState, A_Right_State);
    }
  }

  return false;
}

static bool A_Left_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, A_Final1_State);
    }
  }

  return false;
}

static bool A_Right_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, A_Final1_State);
    }
  }

  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pTest, ROOT, NULL, A_State);
    oosmos_CompositeInit(pTest, A_State, ROOT, A_Choice1_State, A_State_Code);
      oosmos_LeafInit(pTest, A_Choice1_State, A_State, A_Choice1_State_Code);
      oosmos_LeafInit(pTest, A_Left_State, A_State, A_Left_State_Code);
      oosmos_LeafInit(pTest, A_Right_State, A_State, A_Right_State_Code);
      oosmos_FinalInit(pTest, A_Final1_State, A_State, NULL);
    oosmos_LeafInit(pTest, B_State, ROOT, NULL);
//<<<INIT

  oosmos_Debug(pTest, true, NULL);

  return pTest;
}

extern int main(void)
{
  test * pTest = (test *) testNew();

  for (;;) {
    oosmos_RunStateMachines();

    if (oosmos_IsInState(pTest, &pTest->B_State)) {
      printf("SUCCESS.\n");
      break;
    }
  }
}
