//
// OOSMOS threadtest Class
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
#include "prt.h"
#include "threadtest.h"
#include <stdlib.h>
#include <stdbool.h>

//
// Adjust this in order to preallocate all 'threadtest' objects.
// Use 1 for a memory constrained environment.
//
#ifndef MAX_THREADTEST
#define MAX_THREADTEST 10
#endif

//>>>EVENTS
enum {
  evDone = 1,
  evPrint = 2,
  evTimedOut1 = 3,
  evTimedOut2 = 4
};

static const char * OOSMOS_EventNames(int EventCode)
{
  switch (EventCode) {
    case evDone: return "evDone";
    case evPrint: return "evPrint";
    case evTimedOut1: return "evTimedOut1";
    case evTimedOut2: return "evTimedOut2";
    default: return "--No Event Name--";
  }
}
//<<<EVENTS

typedef union {
  oosmos_sEvent Base;
} uEvents;

struct threadtestTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sOrtho Running_State;
      oosmos_sOrthoRegion Running_Region1_State;
        oosmos_sLeaf Running_Region1_DelayMS_State;
        oosmos_sLeaf Running_Region1_WaitCond_State;
        oosmos_sLeaf Running_Region1_WaitCond_TimeoutMS_State;
        oosmos_sLeaf Running_Region1_WaitCond_TimeoutMS_Event_State;
        oosmos_sLeaf Running_Region1_WaitCond_TimeoutMS_Exit_State;
        oosmos_sLeaf Running_Region1_WaitEvent_State;
        oosmos_sLeaf Running_Region1_WaitEvent_TimeoutMS_State;
        oosmos_sLeaf Running_Region1_WaitEvent_TimeoutMS_Event_State;
        oosmos_sLeaf Running_Region1_WaitEvent_TimeoutMS_Exit_State;
      oosmos_sOrthoRegion Running_Region2_State;
        oosmos_sLeaf Running_Region2_Printing_State;
//<<<DECL
  int m_WC_Timeout_Successes;

  int m_WCT_Event_RightPath;
  int m_WCT_Event_WrongPath;

  int m_D_RightPath;
  int m_D_WrongPath;

  int m_E_RightPath;
  int m_E_WrongPath;

  int m_WE_Timeout_Successes;

  int m_WET_Event_RightPath;
  int m_WET_Event_WrongPath;

  int m_WET_Exit_RightPath;
  int m_WET_Exit_WrongPath;
};

static bool ConditionRandom(int Range)
{
  return rand() % Range == 0;
}

static bool ConditionTrue(void)
{
  return true;
}

static bool ConditionFalse(void)
{
  return false;
}

static void DelayMS_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_UNUSED(pThreadTest);

  oosmos_ThreadBegin();
    prtFormatted("ThreadDelayMS...\n");
    oosmos_ThreadDelayMS(3000);
    prtFormatted("ThreadDelayMS SUCCESS\n\n");
  oosmos_ThreadEnd();
}

static void WaitCond_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    prtFormatted("ThreadWaitCond...\n");
    oosmos_ThreadWaitCond(ConditionRandom(2));
    prtFormatted("ThreadWaitCond SUCCESS\n\n");
  oosmos_ThreadEnd();
}

static void WaitCond_TimeoutMS_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    bool TimedOut;
    pThreadTest->m_WC_Timeout_Successes = 0;

    prtFormatted("ThreadWaitCond_TimeoutMS...\n");

    oosmos_ThreadWaitCond_TimeoutMS(ConditionTrue(), 100, &TimedOut);
    pThreadTest->m_WC_Timeout_Successes += (TimedOut == false);

    oosmos_ThreadWaitCond_TimeoutMS(ConditionFalse(), 100, &TimedOut);
    pThreadTest->m_WC_Timeout_Successes += (TimedOut == true);
    prtFormatted("ThreadWaitCond_TimeoutMS %s\n\n", pThreadTest->m_WC_Timeout_Successes == 2 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void WaitCond_TimeoutMS_Event_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_WCT_Event_RightPath = 0;
    pThreadTest->m_WCT_Event_WrongPath = 0;

    prtFormatted("ThreadWaitCond_TimeoutMS_Event...\n");

    oosmos_ThreadWaitCond_TimeoutMS_Event(ConditionTrue(), 100, evTimedOut1);
    pThreadTest->m_WCT_Event_RightPath += 1;

    oosmos_ThreadWaitCond_TimeoutMS_Event(ConditionFalse(), 100, evTimedOut2);
    pThreadTest->m_WCT_Event_WrongPath += 1;
  oosmos_ThreadFinally();
    prtFormatted("ThreadWaitCond_TimeoutMS_Event %s\n\n", pThreadTest->m_WCT_Event_RightPath == 2 && pThreadTest->m_WCT_Event_WrongPath == 0 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void WaitCond_TimeoutMS_Exit_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_D_RightPath = 0;
    pThreadTest->m_D_WrongPath = 0;

    prtFormatted("ThreadWaitCond_TimeoutMS_Exit...\n");

    oosmos_ThreadWaitCond_TimeoutMS_Exit(ConditionTrue(), 100);
    pThreadTest->m_D_RightPath += 1;

    oosmos_ThreadWaitCond_TimeoutMS_Exit(ConditionFalse(), 100);
    pThreadTest->m_D_WrongPath += 1;

  oosmos_ThreadFinally();
    pThreadTest->m_D_RightPath += 1;
    prtFormatted("ThreadWaitCond_TimeoutMS_Exit %s\n\n", pThreadTest->m_D_RightPath == 2 && pThreadTest->m_D_WrongPath == 0 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void WaitEvent_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    prtFormatted("ThreadWaitEvent...\n");

    oosmos_PushEventCode(pThreadTest, evPrint);
    oosmos_ThreadWaitEvent(evDone);
    prtFormatted("ThreadWaitEvent SUCCESS\n\n");
  oosmos_ThreadEnd();
}

static void WaitEvent_TimeoutMS_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    bool TimedOut;
    pThreadTest->m_WE_Timeout_Successes = 0;

    prtFormatted("ThreadWaitEvent_TimeoutMS...\n");

    oosmos_PushEventCode(pThreadTest, evPrint);
    oosmos_ThreadWaitEvent_TimeoutMS(ConditionTrue(), 100, &TimedOut);
    pThreadTest->m_WE_Timeout_Successes += (TimedOut == false);

    oosmos_ThreadWaitEvent_TimeoutMS(ConditionFalse(), 100, &TimedOut);
    pThreadTest->m_WE_Timeout_Successes += (TimedOut == true);
    prtFormatted("ThreadWaitEvent_TimeoutMS %s\n\n", pThreadTest->m_WE_Timeout_Successes == 2 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void WaitEvent_TimeoutMS_Event_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_WET_Event_RightPath = 0;
    pThreadTest->m_WET_Event_WrongPath = 0;

    prtFormatted("ThreadWaitEvent_TimeoutMS_Event...\n");

    //
    // Enqueue 'evPrint' to the orthogonal state. We expect to get an 'evDone' well
    // before the timeout.
    //
    oosmos_PushEventCode(pThreadTest, evPrint);
    oosmos_ThreadWaitEvent_TimeoutMS_Event(evDone, 100, evTimedOut1);
    pThreadTest->m_WET_Event_RightPath += 1;

    //
    // We don't enqueue 'evPrint' so we don't expect an 'evDone' therefore it should time out.
    //
    oosmos_ThreadWaitEvent_TimeoutMS_Event(evDone, 100, evTimedOut2);
    pThreadTest->m_WET_Event_WrongPath += 1;
  oosmos_ThreadFinally();
    prtFormatted("ThreadWaitEvent_TimeoutMS_Event %s\n\n", pThreadTest->m_WET_Event_RightPath == 2 && pThreadTest->m_WET_Event_WrongPath == 0 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void WaitEvent_TimeoutMS_Exit_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_WET_Exit_RightPath = 0;
    pThreadTest->m_WET_Exit_WrongPath = 0;

    prtFormatted("ThreadWaitEvent_TimeoutMS_Exit...\n");

    //
    // Enqueue 'evPrint' to the orthogonal state. We expect to get and 'evDone' well
    // before the timeout.
    //
    oosmos_PushEventCode(pThreadTest, evPrint);
    oosmos_ThreadWaitEvent_TimeoutMS_Exit(evDone, 100);
    pThreadTest->m_WET_Exit_RightPath += 1;

    //
    // We don't enqueue 'evPrint' so we don't expect an 'evDone' therefore it should time out and exit.
    //
    oosmos_ThreadWaitEvent_TimeoutMS_Exit(evDone, 100);
    pThreadTest->m_WET_Exit_WrongPath += 1;

  oosmos_ThreadFinally();
    pThreadTest->m_WET_Exit_RightPath += 1;
    prtFormatted("ThreadWaitEvent_TimeoutMS_Exit %s\n\n", pThreadTest->m_WET_Exit_RightPath == 2 && pThreadTest->m_WET_Exit_WrongPath == 0 ? "SUCCESS" : "FAILURE");
    exit(1);
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Running_Region1_DelayMS_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      DelayMS_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitCond_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitCond_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitCond_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitCond_TimeoutMS_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitCond_TimeoutMS_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitCond_TimeoutMS_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitCond_TimeoutMS_Event_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitCond_TimeoutMS_Event_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitCond_TimeoutMS_Event_Thread(pThreadTest, pState);
      return true;
    }
    case evTimedOut1: {
      pThreadTest->m_WCT_Event_WrongPath += 1;
      return true;
    }
    case evTimedOut2: {
      pThreadTest->m_WCT_Event_RightPath += 1;
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitCond_TimeoutMS_Exit_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitCond_TimeoutMS_Exit_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitCond_TimeoutMS_Exit_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitEvent_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitEvent_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitEvent_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitEvent_TimeoutMS_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitEvent_TimeoutMS_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitEvent_TimeoutMS_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitEvent_TimeoutMS_Event_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitEvent_TimeoutMS_Event_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitEvent_TimeoutMS_Event_Thread(pThreadTest, pState);
      return true;
    }
    case evTimedOut1: {
      pThreadTest->m_WET_Event_WrongPath += 1;
      return true;
    }
    case evTimedOut2: {
      pThreadTest->m_WET_Event_RightPath += 1;
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_WaitEvent_TimeoutMS_Exit_State);
    }
  }

  return false;
}

static bool Running_Region1_WaitEvent_TimeoutMS_Exit_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      WaitEvent_TimeoutMS_Exit_Thread(pThreadTest, pState);
      return true;
    }
  }

  return false;
}

static void OOSMOS_Action1(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  printf("Printing...\n");
  oosmos_PushEventCode(pThreadTest, evDone);

  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static bool Running_Region2_Printing_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evPrint: {
      return oosmos_TransitionAction(pThreadTest, pState, Running_Region2_Printing_State, pEvent, OOSMOS_Action1);
    }
  }

  return false;
}
//<<<CODE

extern threadtest * threadtestNew(void)
{
  oosmos_Allocate(pThreadTest, threadtest, MAX_THREADTEST, NULL);

//>>>INIT
  oosmos_StateMachineInit(pThreadTest, ROOT, NULL, Running_State);
    oosmos_OrthoInit(pThreadTest, Running_State, ROOT, NULL);
      oosmos_OrthoRegionInit(pThreadTest, Running_Region1_State, Running_State, Running_Region1_DelayMS_State, NULL);
        oosmos_LeafInit(pThreadTest, Running_Region1_DelayMS_State, Running_Region1_State, Running_Region1_DelayMS_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitCond_State, Running_Region1_State, Running_Region1_WaitCond_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitCond_TimeoutMS_State, Running_Region1_State, Running_Region1_WaitCond_TimeoutMS_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitCond_TimeoutMS_Event_State, Running_Region1_State, Running_Region1_WaitCond_TimeoutMS_Event_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitCond_TimeoutMS_Exit_State, Running_Region1_State, Running_Region1_WaitCond_TimeoutMS_Exit_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitEvent_State, Running_Region1_State, Running_Region1_WaitEvent_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitEvent_TimeoutMS_State, Running_Region1_State, Running_Region1_WaitEvent_TimeoutMS_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitEvent_TimeoutMS_Event_State, Running_Region1_State, Running_Region1_WaitEvent_TimeoutMS_Event_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitEvent_TimeoutMS_Exit_State, Running_Region1_State, Running_Region1_WaitEvent_TimeoutMS_Exit_State_Code);
      oosmos_OrthoRegionInit(pThreadTest, Running_Region2_State, Running_State, Running_Region2_Printing_State, NULL);
        oosmos_LeafInit(pThreadTest, Running_Region2_Printing_State, Running_Region2_State, Running_Region2_Printing_State_Code);

  oosmos_Debug(pThreadTest, OOSMOS_EventNames);
//<<<INIT

  return pThreadTest;
}
