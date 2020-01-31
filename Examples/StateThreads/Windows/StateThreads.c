//
// OOSMOS StateThreads Example
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
#include <stdbool.h>
#include <stdio.h>

typedef struct testTag test;

struct testTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sOrtho OrthoState_State;
      oosmos_sOrthoRegion OrthoState_Region1_State;
        oosmos_sLeaf OrthoState_Region1_LED_State;
      oosmos_sOrthoRegion OrthoState_Region2_State;
        oosmos_sLeaf OrthoState_Region2_Speaker_State;
//<<<DECL
};

static void LedThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("Blink...\n");
      oosmos_ThreadDelayMS(500);
    }
  oosmos_ThreadEnd();
}

static void SpeakerThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("Beep...\n");
      oosmos_ThreadDelayMS(1500);
    }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool OrthoState_Region1_LED_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      LedThread(pState);
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  return false;
}

static bool OrthoState_Region2_Speaker_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      SpeakerThread(pState);
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pTest, ROOT, NULL, OrthoState_State);
    oosmos_OrthoInit(pTest, OrthoState_State, ROOT, NULL);
      oosmos_OrthoRegionInit(pTest, OrthoState_Region1_State, OrthoState_State, OrthoState_Region1_LED_State, NULL);
        oosmos_LeafInit(pTest, OrthoState_Region1_LED_State, OrthoState_Region1_State, OrthoState_Region1_LED_State_Code);
      oosmos_OrthoRegionInit(pTest, OrthoState_Region2_State, OrthoState_State, OrthoState_Region2_Speaker_State, NULL);
        oosmos_LeafInit(pTest, OrthoState_Region2_Speaker_State, OrthoState_Region2_State, OrthoState_Region2_Speaker_State_Code);

  oosmos_Debug(pTest, NULL);
//<<<INIT

  return pTest;
}

extern int main(void)
{
  testNew();

  printf("Press CNTL-C to stop.\n\n");

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
