//
// OOSMOS Button Test
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
#include "pin.h"
#include "btn.h"
#include "btnph.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct testTag test;

//>>>EVENTS
enum {
  ev_p_Held = 1,
  ev_p_Pressed = 2,
  ev_p_Released = 3,
  ev_q_Pressed = 4,
  ev_s_Pressed = 5,
  ev_s_Released = 6
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case ev_p_Held: return "ev_p_Held";
      case ev_p_Pressed: return "ev_p_Pressed";
      case ev_p_Released: return "ev_p_Released";
      case ev_q_Pressed: return "ev_q_Pressed";
      case ev_s_Pressed: return "ev_s_Pressed";
      case ev_s_Released: return "ev_s_Released";
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
    oosmos_sLeaf Running_State;
    oosmos_sLeaf S_Pressed_State;
    oosmos_sLeaf P_Pressed_State;
    oosmos_sLeaf P_Held_State;
    oosmos_sLeaf Done_State;
//<<<DECL
};

//>>>CODE
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_s_Pressed: {
      return oosmos_Transition(pTest, pState, S_Pressed_State);
    }
    case ev_q_Pressed: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
    case ev_p_Pressed: {
      return oosmos_Transition(pTest, pState, P_Pressed_State);
    }
    case ev_p_Held: {
      return oosmos_Transition(pTest, pState, P_Held_State);
    }
  }

  return false;
}

static bool S_Pressed_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_s_Released: {
      return oosmos_Transition(pTest, pState, Running_State);
    }
  }

  return false;
}

static bool P_Pressed_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_p_Released: {
      return oosmos_Transition(pTest, pState, Running_State);
    }
  }

  return false;
}

static bool P_Held_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_p_Released: {
      return oosmos_Transition(pTest, pState, Running_State);
    }
  }

  return false;
}

static bool Done_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
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
  oosmos_StateMachineInit(pTest, ROOT, NULL, Running_State);
    oosmos_LeafInit(pTest, Running_State, ROOT, Running_State_Code);
    oosmos_LeafInit(pTest, S_Pressed_State, ROOT, S_Pressed_State_Code);
    oosmos_LeafInit(pTest, P_Pressed_State, ROOT, P_Pressed_State_Code);
    oosmos_LeafInit(pTest, P_Held_State, ROOT, P_Held_State_Code);
    oosmos_LeafInit(pTest, Done_State, ROOT, Done_State_Code);

  oosmos_Debug(pTest, OOSMOS_EventNames);
//<<<INIT

  return pTest;
}

extern int main(void)
{
  test * pTest = testNew();

  pin * p_s_Pin    = pinNew('s', pinActiveHigh);
  btn * p_s_Button = btnNew(p_s_Pin);
  btnSubscribePressedEvent(p_s_Button,  oosmos_EventQueue(pTest), ev_s_Pressed,  NULL);
  btnSubscribeReleasedEvent(p_s_Button, oosmos_EventQueue(pTest), ev_s_Released, NULL);

  pin   * p_p_Pin    = pinNew('p', pinActiveHigh);
  btnph * p_p_Button = btnphNew(p_p_Pin, 600);
  btnphSubscribePressedEvent (p_p_Button, oosmos_EventQueue(pTest), ev_p_Pressed,  NULL);
  btnphSubscribeHeldEvent    (p_p_Button, oosmos_EventQueue(pTest), ev_p_Held,     NULL);
  btnphSubscribeReleasedEvent(p_p_Button, oosmos_EventQueue(pTest), ev_p_Released, NULL);

  pin * p_q_Pin    = pinNew('q', pinActiveHigh);
  btn * p_q_Button = btnNew(p_q_Pin);
  btnSubscribePressedEvent(p_q_Button,  oosmos_EventQueue(pTest), ev_q_Pressed,  NULL);

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
