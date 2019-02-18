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
  evTestEvent = 1
};

static const char * EventNames(int EventCode)
{
  switch (EventCode) {
    case evTestEvent: return "evTestEvent";
    default: return "--No Event Name--";
  }
}
//<<<EVENTS

typedef struct {
  oosmos_sEvent Event;
  int Value;
} sTestEvent;

typedef union {
  oosmos_sEvent Event;
  sTestEvent TestEvent;
} uEvents;

typedef struct testTag test;

struct testTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sOrtho A1_State;
      oosmos_sOrthoRegion A1_Region1_State;
        oosmos_sLeaf A1_Region1_B1_State;
      oosmos_sOrthoRegion A1_Region2_State;
        oosmos_sLeaf A1_Region2_EventDriver_State;
    oosmos_sLeaf Done_State;
//<<<DECL

};

static void A1Thread(test * pTest, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  const sTestEvent * pTestEvent;

  oosmos_ThreadBegin();
    oosmos_ThreadWaitEvent(evTestEvent);
    pTestEvent = (sTestEvent *) oosmos_GetCurrentEvent(pState);
    printf("ThreadA1: Received evTestEvent, Value %d %s\n", pTestEvent->Value, pTestEvent->Value == 998 ? "SUCCESS" : "FAILURE");

    oosmos_ThreadWaitEvent(evTestEvent);
    pTestEvent = (sTestEvent *) oosmos_GetCurrentEvent(pState);
    printf("ThreadA1: Received evTestEvent, Value %d %s\n", pTestEvent->Value, pTestEvent->Value == 999 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void EventDriverThread(test * pTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    printf("EventDriverThread: Enter\n");
    oosmos_ThreadDelayMS(1000);

    printf("EventDriverThread: Push 998\n");
    const sTestEvent TestEvent998 = { { evTestEvent }, 998 };
    oosmos_PushEvent(pTest, TestEvent998);

    printf("EventDriverThread: Push 999\n");
    const sTestEvent TestEvent999 = { { evTestEvent }, 999 };
    oosmos_PushEvent(pTest, TestEvent999);
  oosmos_ThreadEnd();
}

//>>>CODE
static bool A1_Region1_B1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      A1Thread(pTest, pState, pEvent);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
  }

  return false;
}

static bool Done_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      oosmos_EndProgram(1);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}

static bool A1_Region2_EventDriver_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      EventDriverThread(pTest, pState);
      return true;
    }
  }

  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInit(pTest, ROOT, NULL, A1_State);
    oosmos_OrthoInit(pTest, A1_State, ROOT, NULL);
      oosmos_OrthoRegionInit(pTest, A1_Region1_State, A1_State, A1_Region1_B1_State, NULL);
        oosmos_LeafInit(pTest, A1_Region1_B1_State, A1_Region1_State, A1_Region1_B1_State_Code);
      oosmos_OrthoRegionInit(pTest, A1_Region2_State, A1_State, A1_Region2_EventDriver_State, NULL);
        oosmos_LeafInit(pTest, A1_Region2_EventDriver_State, A1_Region2_State, A1_Region2_EventDriver_State_Code);
    oosmos_LeafInit(pTest, Done_State, ROOT, Done_State_Code);

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
