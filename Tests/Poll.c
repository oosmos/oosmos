//
// OOSMOS Poll Test
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
//<<<EVENTS

typedef struct testTag test;

const int TargetCount = 3;

struct testTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf TestingNullTransitionWithoutPoll_State;
    oosmos_sOrtho Ortho_State;
      oosmos_sOrthoRegion Ortho_Region1_State;
        oosmos_sComposite Ortho_Region1_A1_State;
          oosmos_sComposite Ortho_Region1_A1_B1_State;
            oosmos_sLeaf Ortho_Region1_A1_B1_C1_State;
      oosmos_sOrthoRegion Ortho_Region2_State;
        oosmos_sComposite Ortho_Region2_A2_State;
          oosmos_sComposite Ortho_Region2_A2_B2_State;
            oosmos_sLeaf Ortho_Region2_A2_B2_C2_State;
    oosmos_sLeaf TestNullTransitionWithPoll_State;
    oosmos_sLeaf Done_State;
//<<<DECL

  int m_Ortho;

  int m_A1_Count;
  int m_B1_Count;
  int m_C1_Count;

  int m_A2_Count;
  int m_B2_Count;
  int m_C2_Count;
};

static bool CheckCounts(const test * pTest)
{
  return pTest->m_Ortho    > TargetCount &&
         pTest->m_A1_Count > TargetCount && pTest->m_B1_Count > TargetCount && pTest->m_C1_Count > TargetCount &&
         pTest->m_A2_Count > TargetCount && pTest->m_B2_Count > TargetCount && pTest->m_C2_Count > TargetCount;
}

static void ThreadTestNull(test * pTest, oosmos_sState * pState)
{
  oosmos_UNUSED(pTest);

  oosmos_ThreadBegin();
    printf("DELAY 50ms\n");
    oosmos_ThreadDelayMS(50);

    printf("DELAY 100ms\n");
    oosmos_ThreadDelayMS(100);
  oosmos_ThreadEnd();
}

static void PollOrtho(test * pTest)
{
  printf("Poll Ortho\n");
  pTest->m_Ortho++;
}

static void ThreadA1(test * pTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("Poll A1: %d\n", pTest->m_A1_Count);
      pTest->m_A1_Count++;
      oosmos_ThreadDelayMS(20);
    }
  oosmos_ThreadEnd();
}

static void PollB1(test * pTest)
{
  printf("Poll B1: %d\n", pTest->m_B1_Count);
  pTest->m_B1_Count++;
}

static void PollC1(test * pTest)
{
  printf("Poll C1: %d\n", pTest->m_C1_Count);
  pTest->m_C1_Count++;
}

static void PollA2(test * pTest)
{
  printf("Poll A2: %d\n", pTest->m_A2_Count);
  pTest->m_A2_Count++;
}

static void PollB2(test * pTest)
{
  printf("Poll B2: %d\n", pTest->m_B2_Count);
  pTest->m_B2_Count++;
}

static void PollC2(test * pTest)
{
  printf("Poll C2: %d\n", pTest->m_C2_Count);
  pTest->m_C2_Count++;
}


//>>>CODE
static bool TestingNullTransitionWithoutPoll_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pTest, pState, Ortho_State);
    }
  }

  return false;
}

static bool Ortho_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PollOrtho(pTest);
      if (CheckCounts(pTest)) {
        return oosmos_Transition(pTest, pState, TestNullTransitionWithPoll_State);
      }
      return true;
    }
  }

  return false;
}

static bool Ortho_Region1_A1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      ThreadA1(pTest, pState);
      return true;
    }
  }

  return false;
}

static bool Ortho_Region1_A1_B1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PollB1(pTest);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}

static bool Ortho_Region1_A1_B1_C1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PollC1(pTest);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}

static bool Ortho_Region2_A2_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PollA2(pTest);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}

static bool Ortho_Region2_A2_B2_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PollB2(pTest);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}

static bool Ortho_Region2_A2_B2_C2_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PollC2(pTest);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}

static bool TestNullTransitionWithPoll_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      ThreadTestNull(pTest, pState);
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
      printf("SUCCESS!\nTerminating program.\n");
      oosmos_EndProgram(1);
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
  oosmos_StateMachineInitNoQueue(pTest, ROOT, NULL, TestingNullTransitionWithoutPoll_State);
    oosmos_LeafInit(pTest, TestingNullTransitionWithoutPoll_State, ROOT, TestingNullTransitionWithoutPoll_State_Code);
    oosmos_OrthoInit(pTest, Ortho_State, ROOT, Ortho_State_Code);
      oosmos_OrthoRegionInit(pTest, Ortho_Region1_State, Ortho_State, Ortho_Region1_A1_State, NULL);
        oosmos_CompositeInit(pTest, Ortho_Region1_A1_State, Ortho_Region1_State, Ortho_Region1_A1_B1_State, Ortho_Region1_A1_State_Code);
          oosmos_CompositeInit(pTest, Ortho_Region1_A1_B1_State, Ortho_Region1_A1_State, Ortho_Region1_A1_B1_C1_State, Ortho_Region1_A1_B1_State_Code);
            oosmos_LeafInit(pTest, Ortho_Region1_A1_B1_C1_State, Ortho_Region1_A1_B1_State, Ortho_Region1_A1_B1_C1_State_Code);
      oosmos_OrthoRegionInit(pTest, Ortho_Region2_State, Ortho_State, Ortho_Region2_A2_State, NULL);
        oosmos_CompositeInit(pTest, Ortho_Region2_A2_State, Ortho_Region2_State, Ortho_Region2_A2_B2_State, Ortho_Region2_A2_State_Code);
          oosmos_CompositeInit(pTest, Ortho_Region2_A2_B2_State, Ortho_Region2_A2_State, Ortho_Region2_A2_B2_C2_State, Ortho_Region2_A2_B2_State_Code);
            oosmos_LeafInit(pTest, Ortho_Region2_A2_B2_C2_State, Ortho_Region2_A2_B2_State, Ortho_Region2_A2_B2_C2_State_Code);
    oosmos_LeafInit(pTest, TestNullTransitionWithPoll_State, ROOT, TestNullTransitionWithPoll_State_Code);
    oosmos_LeafInit(pTest, Done_State, ROOT, Done_State_Code);

  oosmos_Debug(pTest, NULL);
//<<<INIT

  pTest->m_Ortho    = 0;

  pTest->m_A1_Count = 0;
  pTest->m_B1_Count = 0;
  pTest->m_C1_Count = 0;

  pTest->m_A2_Count = 0;
  pTest->m_B2_Count = 0;
  pTest->m_C2_Count = 0;

  return pTest;
}

extern int main(void)
{
  (void) testNew();

  for (;;) {
    oosmos_RunStateMachines();
    printf("---\n");
    oosmos_DelayMS(1);
  }
}
