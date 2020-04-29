#include "oosmos.h"
#include "pin.h"
#include "btn.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct allTag all;

//>>>EVENTS
enum {
  ev_a_Pressed = 1,
  ev_e_Pressed = 2,
  ev_g_Pressed = 3,
  ev_q_Pressed = 4
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case ev_a_Pressed: return "ev_a_Pressed";
      case ev_e_Pressed: return "ev_e_Pressed";
      case ev_g_Pressed: return "ev_g_Pressed";
      case ev_q_Pressed: return "ev_q_Pressed";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef union {
  oosmos_sEvent Event;
} uEvents;

struct allTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sComposite Running_State;
      oosmos_sLeaf Running_A_State;
      oosmos_sLeaf Running_B_State;
    oosmos_sLeaf Terminating_State;
//<<<DECL

  unsigned m_A;
  unsigned m_B;
};

static void action(int Char)
{
  printf("ACTION %c\n", Char);
}

static bool Guard(all * pAll)
{
  return pAll->m_A == pAll->m_B;
}

//>>>CODE
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  all * pAll = (all *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_q_Pressed: {
      return oosmos_Transition(pAll, pState, Terminating_State);
    }
  }

  return false;
}

static void OOSMOS_Action1(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  action('a');

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static void OOSMOS_Action2(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  action('g');

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static bool Running_A_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  all * pAll = (all *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_DEFAULT: {
      printf("DEFAULT\n");
      return true;
    }
    case ev_e_Pressed: {
      return oosmos_Transition(pAll, pState, Running_B_State);
    }
    case oosmos_POLL: {
      if (Guard(pAll)) {
        return oosmos_Transition(pAll, pState, Running_B_State);
      }
      return true;
    }
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 3);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pAll, pState, Running_B_State);
    }
    case ev_a_Pressed: {
      return oosmos_TransitionAction(pAll, pState, Running_B_State, pEvent, OOSMOS_Action1);
    }
    case ev_g_Pressed: {
      if (Guard(pAll)) {
        return oosmos_TransitionAction(pAll, pState, Running_B_State, pEvent, OOSMOS_Action2);
      }
      return true;
    }
  }

  return false;
}

static bool Running_B_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  all * pAll = (all *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pAll, pState, Running_A_State);
    }
  }

  return false;
}

static bool Terminating_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("Exiting...\n");
      oosmos_EndProgram(1);
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE

extern all * allNew(void)
{
  oosmos_Allocate(pAll, all, 1, NULL);

//>>>INIT
  oosmos_StateMachineInit(pAll, ROOT, NULL, Running_State);
    oosmos_CompositeInit(pAll, Running_State, ROOT, Running_A_State, Running_State_Code);
      oosmos_LeafInit(pAll, Running_A_State, Running_State, Running_A_State_Code);
      oosmos_LeafInit(pAll, Running_B_State, Running_State, Running_B_State_Code);
    oosmos_LeafInit(pAll, Terminating_State, ROOT, Terminating_State_Code);

  oosmos_Debug(pAll, OOSMOS_EventNames);
//<<<INIT

  pin * p_e_Pin    = pinNew('e', pinActiveHigh);
  btn * p_e_Button = btnNew(p_e_Pin);
  btnSubscribePressedEvent(p_e_Button,  oosmos_EventQueue(pAll), ev_e_Pressed,  NULL);

  pin * p_a_Pin    = pinNew('a', pinActiveHigh);
  btn * p_a_Button = btnNew(p_a_Pin);
  btnSubscribePressedEvent(p_a_Button,  oosmos_EventQueue(pAll), ev_a_Pressed,  NULL);

  pin * p_g_Pin    = pinNew('g', pinActiveHigh);
  btn * p_g_Button = btnNew(p_g_Pin);
  btnSubscribePressedEvent(p_g_Button,  oosmos_EventQueue(pAll), ev_g_Pressed,  NULL);

  pin * p_q_Pin    = pinNew('q', pinActiveHigh);
  btn * p_q_Button = btnNew(p_q_Pin);
  btnSubscribePressedEvent(p_q_Button,  oosmos_EventQueue(pAll), ev_q_Pressed,  NULL);

  pAll->m_A = 1;
  pAll->m_B = 2;

  return pAll;
}

extern int main(void)
{
  (void) allNew();

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
