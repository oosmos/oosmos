//
// OOSMOS threadyieldtest class implementation
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
#include "threadyieldtest.h"
#include <stdlib.h>
#include <stdbool.h>

#ifndef MAX_THREAD
#define MAX_THREAD 2
#endif

struct threadyieldtestTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf Running_State;
    oosmos_sLeaf Done_State;
//<<<DECL

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

//>>>CODE
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadyieldtest * pThreadYieldTest = (threadyieldtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      Thread(pThreadYieldTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pThreadYieldTest, pState, Done_State);
    }
  }

  return false;
}

static bool Done_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  threadyieldtest * pThreadYieldTest = (threadyieldtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      prtFormatted("Test threadyieldtest, '%s' DONE.\n", pThreadYieldTest->m_pID);
      return true;
    }
  }

  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE

extern threadyieldtest * threadyieldtestNew(const char * pID, int Iterations)
{
  oosmos_Allocate(pThreadYieldTest, threadyieldtest, MAX_THREAD, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pThreadYieldTest, ROOT, NULL, Running_State);
    oosmos_LeafInit(pThreadYieldTest, Running_State, ROOT, Running_State_Code);
    oosmos_LeafInit(pThreadYieldTest, Done_State, ROOT, Done_State_Code);

  oosmos_Debug(pThreadYieldTest, NULL);
//<<<INIT

  pThreadYieldTest->m_pID        = pID;
  pThreadYieldTest->m_Iterations = Iterations;

  return pThreadYieldTest;
}
