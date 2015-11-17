/*
// OOSMOS StateTimeout Example
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

#include <stdio.h>
#include "oosmos.h"

typedef struct testTag test;

struct testTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              MS_State;
    oosmos_sLeaf              Seconds_State;
    oosmos_sLeaf              US_State;
    oosmos_sFinal             Done_State;
};

static bool MS_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      return oosmos_StateTimeoutMS(pRegion, 1000);
    case oosmos_TIMEOUT:
      return oosmos_Transition(pRegion, &pTest->Seconds_State);
  }

  return false;
}

static bool Seconds_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      return oosmos_StateTimeoutSeconds(pRegion, 1);
    case oosmos_TIMEOUT:
      return oosmos_Transition(pRegion, &pTest->US_State);
  }

  return false;
}

static bool US_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      return oosmos_StateTimeoutUS(pRegion, 1000 * 1000);
    case oosmos_TIMEOUT:
      return oosmos_Transition(pRegion, &pTest->Done_State);
  }

  return false;
}

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

  /*                                    StateName      Parent       Default   */
  /*                            ============================================= */
  oosmos_StateMachineInitNoQueue(pTest, StateMachine,  NULL,        MS_State);
    oosmos_LeafInit             (pTest, MS_State,      StateMachine         );
    oosmos_LeafInit             (pTest, Seconds_State, StateMachine         );
    oosmos_LeafInit             (pTest, US_State,      StateMachine         );
    oosmos_FinalInitNoCode      (pTest, Done_State,    StateMachine         );

  oosmos_Debug(&pTest->StateMachine, true, NULL);

  return pTest;
}

extern int main(void)
{
  test * pTest = testNew();

  while (true) {
    oosmos_sStateMachine * pStateMachine = &pTest->StateMachine;
    oosmos_RunStateMachines();

    if (oosmos_IsInState(pStateMachine, &pTest->Done_State))
      break;

    oosmos_DelayMS(100);
  }

  printf("SUCCESS\n");

  return 0;
}
