//
// OOSMOS StateTimeout Example
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
#include <stdbool.h>
#include <stdio.h>

typedef struct testTag test;

//<<<EVENTS
//>>>EVENTS

struct testTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf MS_State;
    oosmos_sLeaf Seconds_State;
    oosmos_sLeaf US_State;
    oosmos_sLeaf Done_State;
//<<<DECL
  int m_TimeMS;
};

//>>>CODE
static bool MS_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutMS(pState, (uint32_t) pTest->m_TimeMS);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Seconds_State);
    }
  }

  return false;
}

static bool Seconds_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 1);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, US_State);
    }
  }

  return false;
}

static bool US_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutUS(pState, (uint32_t) 100000);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
  }

  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pTest, ROOT, NULL, MS_State);
    oosmos_LeafInit(pTest, MS_State, ROOT, MS_State_Code);
    oosmos_LeafInit(pTest, Seconds_State, ROOT, Seconds_State_Code);
    oosmos_LeafInit(pTest, US_State, ROOT, US_State_Code);
    oosmos_LeafInit(pTest, Done_State, ROOT, NULL);
//<<<INIT

  oosmos_Debug(pTest, true, NULL);

  pTest->m_TimeMS = 1000;

  return pTest;
}

extern int main(void)
{
  test * pTest = testNew();

  for (;;) {
    oosmos_RunStateMachines();

    if (oosmos_IsInState(pTest, &pTest->Done_State)) {
      break;
    }

    oosmos_DelayMS(1);
  }

  printf("SUCCESS\n");

  return 0;
}
