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
#include "pin.h"
#include "btn.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct testTag test;

//>>>EVENTS
enum {
  evQuit = 1,
  evTogglePressed = 2,
  evToggleReleased = 3
};
//<<<EVENTS

typedef union {
  oosmos_sEvent Event;
} uEvents;

struct testTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sComposite Active_State;
      oosmos_sComposite Active_Running_State;
        oosmos_sLeaf Active_Running_Toggling_State;
      oosmos_sLeaf Active_Idle_State;
    oosmos_sLeaf Done_State;
//<<<DECL
};

static void RunningThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("true\n");
      oosmos_ThreadDelayMS(50);
      printf("false\n");
      oosmos_ThreadDelayMS(50);
    }
  oosmos_ThreadEnd();
}

static void TogglingThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("ON\n");
      oosmos_ThreadDelayMS(200);
      printf("OFF\n");
      oosmos_ThreadDelayMS(200);
    }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Active_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evQuit: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
  }

  return false;
}

static bool Done_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("Terminating\n");
      exit(1);
    }
  }

  oosmos_UNUSED(pState);
  return false;
}

static bool Active_Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      RunningThread(pState);
      return true;
    }
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 3);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
  }

  return false;
}

static bool Active_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evTogglePressed: {
      return oosmos_Transition(pTest, pState, Active_Running_Toggling_State);
    }
  }

  return false;
}

static bool Active_Running_Toggling_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("Start Toggling\n");
      return true;
    }
    case oosmos_EXIT: {
      printf("Stop Toggling\n");
      return true;
    }
    case oosmos_POLL: {
      TogglingThread(pState);
      return true;
    }
    case evToggleReleased: {
      return oosmos_Transition(pTest, pState, Active_Idle_State);
    }
  }

  return false;
}
//<<<CODE

static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

//>>>INIT
  oosmos_StateMachineInit(pTest, ROOT, NULL, Active_State);
    oosmos_CompositeInit(pTest, Active_State, ROOT, Active_Idle_State);
      oosmos_CompositeInit(pTest, Active_Running_State, Active_State, Active_Running_Toggling_State);
        oosmos_LeafInit(pTest, Active_Running_Toggling_State, Active_Running_State);
      oosmos_LeafInit(pTest, Active_Idle_State, Active_State);
    oosmos_LeafInit(pTest, Done_State, ROOT);
//<<<INIT

  oosmos_Debug(pTest, true, NULL);

  return pTest;
}

extern int main(void)
{
  test * pTest = testNew();

  pin * pTogglePin    = pinNew(' ', pinActiveHigh);
  btn * pToggleButton = btnNew(pTogglePin);
  btnSubscribePressedEvent(pToggleButton,  oosmos_EventQueue(pTest), evTogglePressed,  NULL);
  btnSubscribeReleasedEvent(pToggleButton, oosmos_EventQueue(pTest), evToggleReleased, NULL);

  pin * pQuitPin    = pinNew('q', pinActiveHigh);
  btn * pQuitButton = btnNew(pQuitPin);
  btnSubscribePressedEvent(pQuitButton,  oosmos_EventQueue(pTest), evQuit, NULL);

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(25);
  }
}
