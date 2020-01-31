//
// OOSMOS History Test
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
#include "pin.h"
#include "btn.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct testTag test;

//>>>EVENTS
enum {
  ev_d_Pressed = 1,
  ev_d_Released = 2,
  ev_q_Pressed = 3,
  ev_s_Pressed = 4,
  ev_s_Released = 5
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case ev_d_Pressed: return "ev_d_Pressed";
      case ev_d_Released: return "ev_d_Released";
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
    oosmos_sComposite Running_State;
      oosmos_sHistory Running_History1_State;
      oosmos_sComposite Running_A_State;
        oosmos_sLeaf Running_A_AA_State;
        oosmos_sLeaf Running_A_AAA_State;
      oosmos_sLeaf Running_History2_State;
      oosmos_sComposite Running_B_State;
        oosmos_sLeaf Running_B_BBB_State;
        oosmos_sLeaf Running_B_BB_State;
      oosmos_sComposite Running_C_State;
        oosmos_sLeaf Running_C_CC_State;
        oosmos_sLeaf Running_C_CCC_State;
    oosmos_sLeaf Paused_State;
    oosmos_sLeaf Done_State;
//<<<DECL
};

//>>>CODE
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_s_Pressed: {
      return oosmos_Transition(pTest, pState, Paused_State);
    }
    case ev_q_Pressed: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
    case ev_d_Pressed: {
      return oosmos_Transition(pTest, pState, Paused_State);
    }
  }

  return false;
}

static bool Paused_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_s_Released: {
      return oosmos_Transition(pTest, pState, Running_History1_State);
    }
    case ev_d_Released: {
      return oosmos_Transition(pTest, pState, Running_History2_State);
    }
  }

  return false;
}

static bool Running_A_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 3);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Running_B_State);
    }
  }

  return false;
}

static bool Running_A_AA_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 2);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Running_A_AAA_State);
    }
  }

  return false;
}

static bool Running_B_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 3);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Running_C_State);
    }
  }

  return false;
}

static bool Running_B_BB_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 2);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Running_B_BBB_State);
    }
  }

  return false;
}

static bool Running_C_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 3);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Done_State);
    }
  }

  return false;
}

static bool Running_C_CC_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 2);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pTest, pState, Running_C_CCC_State);
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
    oosmos_CompositeInit(pTest, Running_State, ROOT, Running_A_State, Running_State_Code);
      oosmos_HistoryInit(pTest, Running_History1_State, Running_State, OOSMOS_HistoryShallowType);
      oosmos_CompositeInit(pTest, Running_A_State, Running_State, Running_A_AA_State, Running_A_State_Code);
        oosmos_LeafInit(pTest, Running_A_AA_State, Running_A_State, Running_A_AA_State_Code);
        oosmos_LeafInit(pTest, Running_A_AAA_State, Running_A_State, NULL);
      oosmos_HistoryInit(pTest, Running_History2_State, Running_State, OOSMOS_HistoryDeepType);
      oosmos_CompositeInit(pTest, Running_B_State, Running_State, Running_B_BB_State, Running_B_State_Code);
        oosmos_LeafInit(pTest, Running_B_BBB_State, Running_B_State, NULL);
        oosmos_LeafInit(pTest, Running_B_BB_State, Running_B_State, Running_B_BB_State_Code);
      oosmos_CompositeInit(pTest, Running_C_State, Running_State, Running_C_CC_State, Running_C_State_Code);
        oosmos_LeafInit(pTest, Running_C_CC_State, Running_C_State, Running_C_CC_State_Code);
        oosmos_LeafInit(pTest, Running_C_CCC_State, Running_C_State, NULL);
    oosmos_LeafInit(pTest, Paused_State, ROOT, Paused_State_Code);
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

  pin * p_d_Pin    = pinNew('d', pinActiveHigh);
  btn * p_d_Button = btnNew(p_d_Pin);
  btnSubscribePressedEvent(p_d_Button,  oosmos_EventQueue(pTest), ev_d_Pressed,  NULL);
  btnSubscribeReleasedEvent(p_d_Button, oosmos_EventQueue(pTest), ev_d_Released, NULL);

  pin * p_q_Pin    = pinNew('q', pinActiveHigh);
  btn * p_q_Button = btnNew(p_q_Pin);
  btnSubscribePressedEvent(p_q_Button,  oosmos_EventQueue(pTest), ev_q_Pressed,  NULL);

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
