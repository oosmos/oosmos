#include "oosmos.h"
#include "OrthoThreads.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//>>>EVENTS
//<<<EVENTS

struct orthoTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sOrtho Warning_State;
      oosmos_sOrthoRegion Warning_Region1_State;
        oosmos_sLeaf Warning_Region1_LED_State;
        oosmos_sFinal Warning_Region1_Final1_State;
      oosmos_sOrthoRegion Warning_Region2_State;
        oosmos_sLeaf Warning_Region2_Speaker_State;
        oosmos_sFinal Warning_Region2_Final2_State;
    oosmos_sLeaf Terminating_State;
//<<<DECL
};

static void BlinkingThread(ortho * pOrtho, oosmos_sState * pState)
{
  oosmos_UNUSED(pOrtho);

  oosmos_ThreadBegin();
    for (;;) {
      printf("Blinking...\n");
      oosmos_ThreadDelayMS(250);
    }
  oosmos_ThreadEnd();
}

static void BeepingThread(ortho * pOrtho, oosmos_sState * pState)
{
  oosmos_UNUSED(pOrtho);

  oosmos_ThreadBegin();
    for (;;) {
      printf("Beeping...\n");
      oosmos_ThreadDelayMS(500);
    }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool Warning_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pOrtho, pState, Terminating_State);
    }
  }

  return false;
}

// SR28
static bool Warning_Region1_LED_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      BlinkingThread(pOrtho, pState);
      return true;
    }
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 2);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pOrtho, pState, Warning_Region1_Final1_State);
    }
  }

  return false;
}

// SR28
static bool Warning_Region2_Speaker_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      BeepingThread(pOrtho, pState);
      return true;
    }
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 5);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pOrtho, pState, Warning_Region2_Final2_State);
    }
  }

  return false;
}

static bool Terminating_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      printf("\nSUCCESS!\n");
      oosmos_EndProgram(1);
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE

extern ortho * orthoNew(void)
{
  oosmos_Allocate(pOrtho, ortho, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pOrtho, ROOT, NULL, Warning_State);
    oosmos_OrthoInit(pOrtho, Warning_State, ROOT, Warning_State_Code);
      oosmos_OrthoRegionInit(pOrtho, Warning_Region1_State, Warning_State, Warning_Region1_LED_State, NULL);
        oosmos_LeafInit(pOrtho, Warning_Region1_LED_State, Warning_Region1_State, Warning_Region1_LED_State_Code);
        oosmos_FinalInit(pOrtho, Warning_Region1_Final1_State, Warning_Region1_State, NULL);
      oosmos_OrthoRegionInit(pOrtho, Warning_Region2_State, Warning_State, Warning_Region2_Speaker_State, NULL);
        oosmos_LeafInit(pOrtho, Warning_Region2_Speaker_State, Warning_Region2_State, Warning_Region2_Speaker_State_Code);
        oosmos_FinalInit(pOrtho, Warning_Region2_Final2_State, Warning_Region2_State, NULL);
    oosmos_LeafInit(pOrtho, Terminating_State, ROOT, Terminating_State_Code);

  oosmos_Debug(pOrtho, NULL);
//<<<INIT

  return pOrtho;
}

extern int main(void)
{
  (void) orthoNew();

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
