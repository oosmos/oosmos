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
#include "prt.h"
#include "threadtest.h"
#include <stdlib.h>
#include <stdbool.h>

//
// Change the following #undef to #define to enable debug output.
//
#undef threadtestDEBUG

//
// Adjust this in order to preallocate all 'threadtest' objects.
// Use 1 for a memory constrained environment.
//
#ifndef MAX_THREADTEST
#define MAX_THREADTEST 1
#endif

//>>>EVENTS
enum {
  evDone = 1,
  evPrint = 2,
  evTimedOut1 = 3,
  evTimedOut2 = 4
};
//<<<EVENTS

#ifdef threadtestDEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch(EventCode) {
      NameCase(evDone)
      NameCase(evTimedOut)
      NameCase(evPrint)
      default: return "--No Event Name--";
    }
  }
#endif

typedef union {
  oosmos_sEvent Base;
} uEvents;

struct threadtestTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sOrtho Running_State;
      oosmos_sOrthoRegion Running_Region1_State;
        oosmos_sLeaf Running_Region1_A_State;
        oosmos_sLeaf Running_Region1_B_State;
        oosmos_sLeaf Running_Region1_C_State;
        oosmos_sLeaf Running_Region1_D_State;
        oosmos_sLeaf Running_Region1_E_State;
        oosmos_sLeaf Running_Region1_F_State;
        oosmos_sLeaf Running_Region1_G_State;
      oosmos_sOrthoRegion Running_Region2_State;
        oosmos_sLeaf Running_Region2_Printing_State;
//<<<DECL
  int m_B_Successes;

  int m_C_RightPath;
  int m_C_WrongPath;

  int m_D_RightPath;
  int m_D_WrongPath;

  int m_E_RightPath;
  int m_E_WrongPath;

  int m_F_RightPath;
  int m_F_WrongPath;

  int m_G_RightPath;
  int m_G_WrongPath;
};

static int ConditionRandom(int Range)
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

static void TestA(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_UNUSED(pThreadTest);

  oosmos_ThreadBegin();
    prtFormatted("TestA threadDelayMS...\n");
    oosmos_ThreadDelayMS(3000);
    prtFormatted("TestA SUCCESS\n\n");
  oosmos_ThreadEnd();
}

static void TestB(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    bool TimedOut;
    pThreadTest->m_B_Successes = 0;

    (void)prtFormatted("TestB threadWaitCond...\n");
    oosmos_ThreadWaitCond(ConditionRandom(2));
    pThreadTest->m_B_Successes += 1;

    (void)prtFormatted("TestB threadWaitCond_TimeoutMS...\n");

    oosmos_ThreadWaitCond_TimeoutMS(100, &TimedOut,
      ConditionTrue()
    );
    pThreadTest->m_B_Successes += (TimedOut == false);

    oosmos_ThreadWaitCond_TimeoutMS(100, &TimedOut,
      ConditionFalse()
    );
    pThreadTest->m_B_Successes += (TimedOut == true);
    prtFormatted("TestB %s\n\n", pThreadTest->m_B_Successes == 3 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void TestC(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_C_RightPath = 0;
    pThreadTest->m_C_WrongPath = 0;

    prtFormatted("TestC threadWaitCond_TimeoutMS_Event...\n");

    oosmos_ThreadWaitCond_TimeoutMS_Event(100, evTimedOut1,
      ConditionTrue()
    );
    pThreadTest->m_C_RightPath += 1;

    oosmos_ThreadWaitCond_TimeoutMS_Event(100, evTimedOut2,
      ConditionFalse()
    );
    pThreadTest->m_C_WrongPath += 1;
  oosmos_ThreadFinally();
    prtFormatted("TestC %s\n\n", pThreadTest->m_C_RightPath == 2 && pThreadTest->m_C_WrongPath == 0 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void TestD(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_D_RightPath = 0;
    pThreadTest->m_D_WrongPath = 0;

    prtFormatted("TestD threadWaitCond_TimeoutMS_Exit...\n");

    oosmos_ThreadWaitCond_TimeoutMS_Exit(100,
      ConditionTrue()
    );
    pThreadTest->m_D_RightPath += 1;

    oosmos_ThreadWaitCond_TimeoutMS_Exit(100,
      ConditionFalse()
    );
    pThreadTest->m_D_WrongPath += 1;

  oosmos_ThreadFinally();
    pThreadTest->m_D_RightPath += 1;
    prtFormatted("TestD %s\n\n", pThreadTest->m_D_RightPath == 2 && pThreadTest->m_D_WrongPath == 0 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void TestE(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_E_RightPath = 0;
    pThreadTest->m_E_WrongPath = 0;

    prtFormatted("TestE threadWaitCond_TimeoutMS_Exit...\n");

    oosmos_ThreadWaitCond_TimeoutMS_Exit(100,
      ConditionTrue()
    );
    pThreadTest->m_E_RightPath += 1;

    oosmos_ThreadWaitCond_TimeoutMS_Exit(100,
      ConditionFalse()
    );
    pThreadTest->m_E_WrongPath += 1;

  oosmos_ThreadFinally();
    pThreadTest->m_E_RightPath += 1;
    prtFormatted("TestE %s\n\n", pThreadTest->m_D_RightPath == 2 && pThreadTest->m_D_WrongPath == 0 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void TestF(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_F_RightPath = 0;
    pThreadTest->m_F_WrongPath = 0;

    prtFormatted("TestF threadWaitEvent_TimeoutMS_Event...\n");

    //
    // Enqueue 'evPrint' to the orthogonal state. We expect to get an 'evDone' well
    // before the timeout.
    //
    oosmos_PushEventCode(pThreadTest, evPrint);
    oosmos_ThreadWaitEvent_TimeoutMS_Event(100, evTimedOut1,
      evDone
    );
    pThreadTest->m_F_RightPath += 1;

    //
    // We don't enqueue 'evPrint' so we don't expect an 'evDone' therefore it should time out.
    //
    oosmos_ThreadWaitEvent_TimeoutMS_Event(100, evTimedOut2,
      evDone
    );
    pThreadTest->m_F_WrongPath += 1;
  oosmos_ThreadFinally();
    prtFormatted("TestF %s\n\n", pThreadTest->m_F_RightPath == 2 && pThreadTest->m_F_WrongPath == 0 ? "SUCCESS" : "FAILURE");
  oosmos_ThreadEnd();
}

static void TestG(threadtest * pThreadTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    pThreadTest->m_G_RightPath = 0;
    pThreadTest->m_G_WrongPath = 0;

    prtFormatted("TestG threadWaitEvent_TimeoutMS_Exit...\n");

    //
    // Enqueue 'evPrint' to the orthogonal state. We expect to get and 'evDone' well
    // before the timeout.
    //
    oosmos_PushEventCode(pThreadTest, evPrint);
    oosmos_ThreadWaitEvent_TimeoutMS_Exit(100,
      evDone
    );
    pThreadTest->m_G_RightPath += 1;

    //
    // We don't enqueue 'evPrint' so we don't expect an 'evDone' therefore it should time out.
    //
    oosmos_ThreadWaitEvent_TimeoutMS_Exit(100,
      evDone
    );
    pThreadTest->m_G_WrongPath += 1;

  oosmos_ThreadFinally();
    pThreadTest->m_G_RightPath += 1;
    prtFormatted("TestG %s\n\n", pThreadTest->m_F_RightPath == 2 && pThreadTest->m_F_WrongPath == 0 ? "SUCCESS" : "FAILURE");
    exit(1);
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Running_Region1_A_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      TestA(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_B_State);
    }
  }

  return false;
}

static bool Running_Region1_B_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      TestB(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_C_State);
    }
  }

  return false;
}

static bool Running_Region1_C_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      TestC(pThreadTest, pState);
      return true;
    }
    case evTimedOut1: {
      pThreadTest->m_C_WrongPath += 1;
      return true;
    }
    case evTimedOut2: {
      pThreadTest->m_C_RightPath += 1;
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_D_State);
    }
  }

  return false;
}

static bool Running_Region1_D_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      TestD(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_E_State);
    }
  }

  return false;
}

static bool Running_Region1_E_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      TestE(pThreadTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_F_State);
    }
  }

  return false;
}

static bool Running_Region1_F_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      TestF(pThreadTest, pState);
      return true;
    }
    case evTimedOut1: {
      pThreadTest->m_F_WrongPath += 1;
      return true;
    }
    case evTimedOut2: {
      pThreadTest->m_F_RightPath += 1;
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadTest, pState, Running_Region1_G_State);
    }
  }

  return false;
}

static bool Running_Region1_G_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      TestG(pThreadTest, pState);
      return true;
    }
  }

  return false;
}

static void OOSMOS_Action1(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadtest * pThreadTest = (threadtest *) pObject;

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
    oosmos_OrthoInitNoCode(pThreadTest, Running_State, ROOT);
      oosmos_OrthoRegionInitNoCode(pThreadTest, Running_Region1_State, Running_State, Running_Region1_A_State);
        oosmos_LeafInit(pThreadTest, Running_Region1_A_State, Running_Region1_State);
        oosmos_LeafInit(pThreadTest, Running_Region1_B_State, Running_Region1_State);
        oosmos_LeafInit(pThreadTest, Running_Region1_C_State, Running_Region1_State);
        oosmos_LeafInit(pThreadTest, Running_Region1_D_State, Running_Region1_State);
        oosmos_LeafInit(pThreadTest, Running_Region1_E_State, Running_Region1_State);
        oosmos_LeafInit(pThreadTest, Running_Region1_F_State, Running_Region1_State);
        oosmos_LeafInit(pThreadTest, Running_Region1_G_State, Running_Region1_State);
      oosmos_OrthoRegionInitNoCode(pThreadTest, Running_Region2_State, Running_State, Running_Region2_Printing_State);
        oosmos_LeafInit(pThreadTest, Running_Region2_Printing_State, Running_Region2_State);
//<<<INIT

#ifdef threadtestDEBUG
  oosmos_Debug(pThreadTest, true, EventNames);
#endif

  return pThreadTest;
}
