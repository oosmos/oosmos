//
// OOSMOS threadtest Class
//
// Copyright (C) 2014-2020  OOSMOS, LLC
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
#include <stdio.h>

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
  evPrint = 2
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case evDone: return "evDone";
      case evPrint: return "evPrint";
      default: return "";
    }
  }
#endif
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
        oosmos_sLeaf Running_Region1_WaitEvent_State;
        oosmos_sLeaf Running_Region1_WaitEvent_TimeoutMS_State;
        oosmos_sLeaf Running_Region1_ThreadExit_State;
        oosmos_sLeaf Running_Region1_ThreadExitFinally_State;
        oosmos_sLeaf Running_Region1_Exiting_State;
      oosmos_sOrthoRegion Running_Region2_State;
        oosmos_sLeaf Running_Region2_Printing_State;
//<<<DECL
  unsigned m_WC_Timeout_Successes;
  unsigned m_WE_Timeout_Successes;

  unsigned m_ThreadExitCount;
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
    oosmos_ThreadWaitEvent_TimeoutMS(evPrint, 100, &TimedOut);
    pThreadTest->m_WE_Timeout_Successes += (TimedOut == false);

    oosmos_ThreadWaitEvent_TimeoutMS(evPrint, 100, &TimedOut);
    pThreadTest->m_WE_Timeout_Successes += (TimedOut == true);
    prtFormatted("ThreadWaitEvent_TimeoutMS %s\n\n", pThreadTest->m_WE_Timeout_Successes == 2 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void ThreadExit_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_ThreadExitCount = 0;
    oosmos_ThreadYield();
    pThreadTest->m_ThreadExitCount++;
    prtFormatted("ThreadExit SUCCESS\n");
    oosmos_ThreadExit();
  oosmos_ThreadEnd();
}

static void ThreadExitFinally_Thread(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_ThreadExitCount = 0;
    oosmos_ThreadYield();
    pThreadTest->m_ThreadExitCount++;
    oosmos_ThreadExit();
  oosmos_ThreadFinally();
    pThreadTest->m_ThreadExitCount++;

    if (pThreadTest->m_ThreadExitCount == 2) {
      prtFormatted("ThreadExitFinally SUCCESS\n");
    } else {
      prtFormatted("ThreadExitFinally FAILURE\n");
    }
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
      return oosmos_Transition(pThreadTest, pState, Running_Region1_ThreadExit_State);
    }
  }

  return false;
}

static bool Running_Region1_ThreadExit_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      ThreadExit_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_ThreadExitFinally_State);
    }
  }

  return false;
}

static bool Running_Region1_ThreadExitFinally_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      ThreadExitFinally_Thread(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_Exiting_State);
    }
  }

  return false;
}

static bool Running_Region1_Exiting_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      oosmos_EndProgram(1);
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
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
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitEvent_State, Running_Region1_State, Running_Region1_WaitEvent_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_WaitEvent_TimeoutMS_State, Running_Region1_State, Running_Region1_WaitEvent_TimeoutMS_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_ThreadExit_State, Running_Region1_State, Running_Region1_ThreadExit_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_ThreadExitFinally_State, Running_Region1_State, Running_Region1_ThreadExitFinally_State_Code);
        oosmos_LeafInit(pThreadTest, Running_Region1_Exiting_State, Running_Region1_State, Running_Region1_Exiting_State_Code);
      oosmos_OrthoRegionInit(pThreadTest, Running_Region2_State, Running_State, Running_Region2_Printing_State, NULL);
        oosmos_LeafInit(pThreadTest, Running_Region2_Printing_State, Running_Region2_State, Running_Region2_Printing_State_Code);

  oosmos_Debug(pThreadTest, OOSMOS_EventNames);
//<<<INIT

  return pThreadTest;
}
