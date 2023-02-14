#include "oosmos.h"
#include "OrthoThreads.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef union {
  oosmos_sEvent Base;
} uEvents;

//>>>EVENTS
enum {
  evTick = 1
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case evTick: return "evTick";
      default: return "";
    }
  }
#endif
//<<<EVENTS

struct orthoTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sOrtho Outer_State;
      oosmos_sOrthoRegion Outer_Region1_State;
        oosmos_sLeaf Outer_Region1_Metronome_State;
      oosmos_sOrthoRegion Outer_Region2_State;
        oosmos_sLeaf Outer_Region2_First_State;
      oosmos_sOrthoRegion Outer_Region3_State;
        oosmos_sLeaf Outer_Region3_Second_State;
//<<<DECL
};

static void MetronomeThread(ortho * pOrtho, oosmos_sState * pState)
{
  oosmos_UNUSED(pOrtho);

  oosmos_ThreadBegin();
    for (;;) {
      oosmos_ThreadDelayMS(2000);
	  oosmos_PushEventCode(pOrtho, evTick);
    }
  oosmos_ThreadEnd();
}

static void FirstThread(ortho * pOrtho, oosmos_sState * pState)
{
  oosmos_UNUSED(pOrtho);

  oosmos_ThreadBegin();
    for (;;) {
      oosmos_ThreadWaitEvent(evTick);
	  printf("FirstThread: Got evTick\n");
    }
  oosmos_ThreadEnd();
}

static void SecondThread(ortho * pOrtho, oosmos_sState * pState)
{
  oosmos_UNUSED(pOrtho);

  oosmos_ThreadBegin();
    for (;;) {
      oosmos_ThreadWaitEvent(evTick);
	  printf("SecondThread: Got evTick\n");
    }
  oosmos_ThreadEnd();
}

//>>>CODE
// SR28
static bool Outer_Region1_Metronome_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      MetronomeThread(pOrtho, pState);
      return true;
    }
  }

  return false;
}

// SR28
static bool Outer_Region2_First_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evTick: {
      return oosmos_Transition(pOrtho, pState, Outer_Region2_First_State);
    }
  }

  return false;
}

// SR28
static bool Outer_Region3_Second_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evTick: {
      return oosmos_Transition(pOrtho, pState, Outer_Region3_Second_State);
    }
  }

  return false;
}
//<<<CODE

extern ortho * orthoNew(void)
{
  oosmos_Allocate(pOrtho, ortho, 1, NULL);

//>>>INIT
  oosmos_StateMachineInit(pOrtho, ROOT, NULL, Outer_State);
    oosmos_OrthoInit(pOrtho, Outer_State, ROOT, NULL);
      oosmos_OrthoRegionInit(pOrtho, Outer_Region1_State, Outer_State, Outer_Region1_Metronome_State, NULL);
        oosmos_LeafInit(pOrtho, Outer_Region1_Metronome_State, Outer_Region1_State, Outer_Region1_Metronome_State_Code);
      oosmos_OrthoRegionInit(pOrtho, Outer_Region2_State, Outer_State, Outer_Region2_First_State, NULL);
        oosmos_LeafInit(pOrtho, Outer_Region2_First_State, Outer_Region2_State, Outer_Region2_First_State_Code);
      oosmos_OrthoRegionInit(pOrtho, Outer_Region3_State, Outer_State, Outer_Region3_Second_State, NULL);
        oosmos_LeafInit(pOrtho, Outer_Region3_Second_State, Outer_Region3_State, Outer_Region3_Second_State_Code);

  oosmos_Debug(pOrtho, OOSMOS_EventNames);
//<<<INIT

  return pOrtho;
}

extern int main(void)
{
  (void) orthoNew();

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelaySeconds(3);
  }
}
