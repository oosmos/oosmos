//
// OOSMOS StateTimeout Example
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

#include "btn.h"
#include "oosmos.h"
#include "pin.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct testTag test;

//>>>EVENTS
enum {
  ev_b_Pressed = 1,
  ev_b_Released = 2,
  ev_q_Pressed = 3,
  ev_r_Pressed = 4,
  ev_r_Released = 5
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case ev_b_Pressed: return "ev_b_Pressed";
      case ev_b_Released: return "ev_b_Released";
      case ev_q_Pressed: return "ev_q_Pressed";
      case ev_r_Pressed: return "ev_r_Pressed";
      case ev_r_Released: return "ev_r_Released";
      default: return "";
    }
  }
#endif
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
        oosmos_sLeaf Active_Running_Flashing_State;
        oosmos_sLeaf Active_Running_Beeping_State;
      oosmos_sLeaf Active_Idle_State;
    oosmos_sLeaf Done_State;
//<<<DECL
};

static void RunningThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("RUNNING...\n");
      oosmos_ThreadDelayMS(750);
    }
  oosmos_ThreadEnd();
}

static void FlashingThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("Flashing...\n");
      oosmos_ThreadDelayMS(50);
    }
  oosmos_ThreadEnd();
}

static void BeepingThread(oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("Beeping...\n");
      oosmos_ThreadDelayMS(100);
    }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Active_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_q_Pressed: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
  }

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
    case ev_r_Released: {
      return oosmos_Transition(pTest, pState, Active_Idle_State);
    }
  }

  return false;
}

static bool Active_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_r_Pressed: {
      return oosmos_Transition(pTest, pState, Active_Running_State);
    }
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 15);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
  }

  return false;
}

static bool Active_Running_Flashing_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      FlashingThread(pState);
      return true;
    }
    case ev_b_Pressed: {
      return oosmos_Transition(pTest, pState, Active_Running_Beeping_State);
    }
  }

  return false;
}

static bool Active_Running_Beeping_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      BeepingThread(pState);
      return true;
    }
    case ev_b_Released: {
      return oosmos_Transition(pTest, pState, Active_Running_Flashing_State);
    }
  }

  return false;
}

static bool Done_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("Terminating.\n");
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
  oosmos_StateMachineInit(pTest, ROOT, NULL, Active_State);
    oosmos_CompositeInit(pTest, Active_State, ROOT, Active_Idle_State, Active_State_Code);
      oosmos_CompositeInit(pTest, Active_Running_State, Active_State, Active_Running_Flashing_State, Active_Running_State_Code);
        oosmos_LeafInit(pTest, Active_Running_Flashing_State, Active_Running_State, Active_Running_Flashing_State_Code);
        oosmos_LeafInit(pTest, Active_Running_Beeping_State, Active_Running_State, Active_Running_Beeping_State_Code);
      oosmos_LeafInit(pTest, Active_Idle_State, Active_State, Active_Idle_State_Code);
    oosmos_LeafInit(pTest, Done_State, ROOT, Done_State_Code);

  oosmos_Debug(pTest, OOSMOS_EventNames);
//<<<INIT

  return pTest;
}

extern int main(void)
{
  test * pTest = testNew();

  pin * p_r_Pin    = pinNew('r', pinActiveHigh);
  btn * p_r_Button = btnNew(p_r_Pin);
  btnSubscribePressedEvent(p_r_Button,  oosmos_EventQueue(pTest), ev_r_Pressed,  NULL);
  btnSubscribeReleasedEvent(p_r_Button, oosmos_EventQueue(pTest), ev_r_Released, NULL);

  pin * p_b_Pin    = pinNew('b', pinActiveHigh);
  btn * p_b_Button = btnNew(p_b_Pin);
  btnSubscribePressedEvent(p_b_Button,  oosmos_EventQueue(pTest), ev_b_Pressed,  NULL);
  btnSubscribeReleasedEvent(p_b_Button, oosmos_EventQueue(pTest), ev_b_Released, NULL);

  pin * p_q_Pin    = pinNew('q', pinActiveHigh);
  btn * p_q_Button = btnNew(p_q_Pin);
  btnSubscribePressedEvent(p_q_Button,  oosmos_EventQueue(pTest), ev_q_Pressed,  NULL);

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
