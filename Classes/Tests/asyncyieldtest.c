//
// OOSMOS asyncyieldtest class implementation
//
// Copyright (C) 2014-2016  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <http://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdlib.h>
#include "oosmos.h"
#include "prt.h"
#include "asyncyieldtest.h"

#ifndef MAX_ASYNC
#define MAX_ASYNC 2
#endif

struct asyncyieldtestTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              Running_State;
    oosmos_sLeaf              Final_State;

  const char * m_pID;
  int          m_Count;
  int          m_Iterations;
};

//
// Test oosmos_asyncyieldtest.
//
static bool Running_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asyncyieldtest * pAsyncYieldTest = (asyncyieldtest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        for (pAsyncYieldTest->m_Count = 1; pAsyncYieldTest->m_Count <= pAsyncYieldTest->m_Iterations; pAsyncYieldTest->m_Count++) {
          prtFormatted("Test asyncyieldtest, '%s'...\n", pAsyncYieldTest->m_pID);
          oosmos_AsyncYield(pRegion);
        }
      oosmos_AsyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pAsyncYieldTest->Final_State);
    case oosmos_EXIT:
      return prtFormatted("Test asyncyieldtest, '%s' DONE.\n", pAsyncYieldTest->m_pID);
  }

  return false;
}

extern asyncyieldtest * asyncyieldtestNew(const char * pID, int Iterations)
{  
  oosmos_Allocate(pAsyncyieldtest, asyncyieldtest, MAX_SYNC, NULL);

  //                                           StateName      Parent        Default
  //                              =======================================================
  oosmos_StateMachineInitNoQueue  (pAsyncyieldtest, StateMachine,  NULL,         Running_State);
    oosmos_LeafInit               (pAsyncyieldtest, Running_State, StateMachine               );
    oosmos_LeafInitNoCode         (pAsyncyieldtest, Final_State,   StateMachine               );

  pAsyncyieldtest->m_pID        = pID;
  pAsyncyieldtest->m_Iterations = Iterations;

  return pAsyncyieldtest;
}
