/*
// OOSMOS syncyieldtest class implementation
//
// Copyright (C) 2014-2015  OOSMOS, LLC
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
*/

#include <stdlib.h>
#include "oosmos.h"
#include "prt.h"
#include "syncyieldtest.h"

#ifndef MAX_SYNC
#define MAX_SYNC 2
#endif

struct syncyieldtestTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              Running_State;
    oosmos_sLeaf              Final_State;

  const char * m_pID;
  int          m_Count;
  int          m_Iterations;
};

/*
// Test oosmos_syncyieldtest.
*/
static bool Running_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  syncyieldtest * pSyncYieldTest = (syncyieldtest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);

        for (pSyncYieldTest->m_Count = 1; pSyncYieldTest->m_Count <= pSyncYieldTest->m_Iterations; pSyncYieldTest->m_Count++) {
          prtFormatted("Test syncyieldtest, '%s'...\n", pSyncYieldTest->m_pID);
          oosmos_SyncYield(pRegion);
        }
      oosmos_SyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pSyncYieldTest->Final_State);
    case oosmos_EXIT:
      return prtFormatted("Test syncyieldtest, '%s' DONE.\n", pSyncYieldTest->m_pID);
  }

  return false;
}

extern syncyieldtest * syncyieldtestNew(const char * pID, int Iterations)
{  
  oosmos_Allocate(psyncyieldtest, syncyieldtest, MAX_SYNC, NULL);

  /*                                           StateName      Parent        Default      */
  /*                              =======================================================*/
  oosmos_StateMachineInitNoQueue  (psyncyieldtest, StateMachine,  NULL,         Running_State);
    oosmos_LeafInit               (psyncyieldtest, Running_State, StateMachine               );
    oosmos_LeafInitNoCode         (psyncyieldtest, Final_State,   StateMachine               );

  psyncyieldtest->m_pID        = pID;
  psyncyieldtest->m_Iterations = Iterations;

  return psyncyieldtest;
}
