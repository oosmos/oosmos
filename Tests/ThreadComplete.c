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
    oosmos_sLeaf Running_State;
    oosmos_sLeaf Complete_State;
//<<<DECL

};

static void RunningThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    oosmos_ThreadDelayMS(1000);
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("\nWaiting for thread completion...\n");
      return true;
    }
    case oosmos_POLL: {
      RunningThread(pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, Complete_State);
    }
  }

  return false;
}

static bool Complete_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("\n--Goal State Achieved--\n");
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
  oosmos_StateMachineInitNoQueue(pTest, ROOT, NULL, Running_State);
    oosmos_LeafInit(pTest, Running_State, ROOT, Running_State_Code);
    oosmos_LeafInit(pTest, Complete_State, ROOT, Complete_State_Code);

  oosmos_Debug(pTest, NULL);
//<<<INIT

  return pTest;
}

extern int main(void)
{
  (void) testNew();

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
