#include "accum.h"
#include "accumtest.h"
#include "oosmos.h"
#include <stdbool.h>
#include <stdio.h>

//>>>EVENTS
//<<<EVENTS

typedef union {
  oosmos_sEvent Event;
} uEvents;

struct accumtestTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf Waiting_State;
    oosmos_sLeaf Done_State;
//<<<DECL

  accum * m_pAccum;
};

//>>>CODE
static bool Waiting_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  accumtest * pAccumTest = (accumtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("ENTER Waiting for 3 days...\n");
      return true;
    }
    case oosmos_EXIT: {
      printf("EXIT Done waiting 3 days.\n");
      return true;
    }
    case oosmos_DEFAULT: {
        accumStart(pAccumTest->m_pAccum);
      return true;
    }
    case oosmos_POLL: {
      if (accumHasReachedDays(pAccumTest->m_pAccum, 3)) {
        return oosmos_Transition(pAccumTest, pState, Done_State);
      }
      return true;
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

extern accumtest * accumtestNew(void)
{
  oosmos_Allocate(pAccumTest, accumtest, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pAccumTest, ROOT, NULL, Waiting_State);
    oosmos_LeafInit(pAccumTest, Waiting_State, ROOT, Waiting_State_Code);
    oosmos_LeafInit(pAccumTest, Done_State, ROOT, Done_State_Code);

  oosmos_Debug(pAccumTest, NULL);
//<<<INIT

  pAccumTest->m_pAccum = accumNew();

  return pAccumTest;
}
