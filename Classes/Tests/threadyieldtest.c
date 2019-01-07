//
// OOSMOS threadyieldtest class implementation
//
// Copyright (C) 2014-2018  OOSMOS, LLC
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
#include "threadyieldtest.h"
#include <stdlib.h>
#include <stdbool.h>

#ifndef MAX_THREAD
#define MAX_THREAD 2
#endif

struct threadyieldtestTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              Running_State;
    oosmos_sLeaf              Final_State;

  const char * m_pID;
  int          m_Count;
  int          m_Iterations;
};

static void Thread(threadyieldtest * pThreadYieldTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (pThreadYieldTest->m_Count = 1; pThreadYieldTest->m_Count <= pThreadYieldTest->m_Iterations; pThreadYieldTest->m_Count++) {
      prtFormatted("Test threadyieldtest, '%s'...\n", pThreadYieldTest->m_pID);
      oosmos_ThreadYield();
    }
  oosmos_ThreadEnd();
}

//
// Test oosmos_threadyieldtest.
//
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadyieldtest * pThreadYieldTest = (threadyieldtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL:
      Thread(pThreadYieldTest, pState);
      return true;
    case oosmos_COMPLETE:
      return oosmos_Transition(pThreadYieldTest, pState, Final_State);
    case oosmos_EXIT:
      prtFormatted("Test threadyieldtest, '%s' DONE.\n", pThreadYieldTest->m_pID);
      return true;
  }

  return false;
}

extern threadyieldtest * threadyieldtestNew(const char * pID, int Iterations)
{
  oosmos_Allocate(pThreadYieldtest, threadyieldtest, MAX_THREAD, NULL);

  //                                           StateName      Parent        Default
  //                              =======================================================
  oosmos_StateMachineInitNoQueue  (pThreadYieldtest, StateMachine,  NULL,         Running_State);
    oosmos_LeafInit               (pThreadYieldtest, Running_State, StateMachine               );
    oosmos_LeafInitNoCode         (pThreadYieldtest, Final_State,   StateMachine               );

  pThreadYieldtest->m_pID        = pID;
  pThreadYieldtest->m_Iterations = Iterations;

  return pThreadYieldtest;
}
