#include "oosmos.h"
#include "OrthoThreads.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef union {
  oosmos_sEvent Base;
} uEvents;

//>>>EVENTS
//<<<EVENTS

struct orthoTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sOrtho OuterRight_State;
      oosmos_sOrthoRegion OuterRight_Region1_State;
        oosmos_sLeaf OuterRight_Region1_Idle_State;
      oosmos_sOrthoRegion OuterRight_Region2_State;
        oosmos_sLeaf OuterRight_Region2_Idle_State;
      oosmos_sOrthoRegion OuterRight_Region3_State;
        oosmos_sLeaf OuterRight_Region3_Idle_State;
    oosmos_sOrtho OuterLeft_State;
      oosmos_sOrthoRegion OuterLeft_Region1_State;
        oosmos_sLeaf OuterLeft_Region1_Idle_State;
        oosmos_sLeaf OuterLeft_Region1_Settled_State;
      oosmos_sOrthoRegion OuterLeft_Region2_State;
        oosmos_sLeaf OuterLeft_Region2_Idle_State;
      oosmos_sOrthoRegion OuterLeft_Region3_State;
        oosmos_sLeaf OuterLeft_Region3_Idle_State;
//<<<DECL
};


//>>>CODE
static bool OuterLeft_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 2);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pOrtho, pState, OuterRight_State);
    }
  }

  return false;
}

static bool OuterLeft_Region1_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_COMPLETE: {
      return oosmos_Transition(pOrtho, pState, OuterLeft_Region1_Settled_State);
    }
  }

  return false;
}

static bool OuterRight_Region3_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  ortho * pOrtho = (ortho *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 1);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pOrtho, pState, OuterLeft_State);
    }
  }

  return false;
}
//<<<CODE

extern ortho * orthoNew(void)
{
  oosmos_Allocate(pOrtho, ortho, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pOrtho, ROOT, NULL, OuterLeft_State);
    oosmos_OrthoInit(pOrtho, OuterRight_State, ROOT, NULL);
      oosmos_OrthoRegionInit(pOrtho, OuterRight_Region1_State, OuterRight_State, OuterRight_Region1_Idle_State, NULL);
        oosmos_LeafInit(pOrtho, OuterRight_Region1_Idle_State, OuterRight_Region1_State, NULL);
      oosmos_OrthoRegionInit(pOrtho, OuterRight_Region2_State, OuterRight_State, OuterRight_Region2_Idle_State, NULL);
        oosmos_LeafInit(pOrtho, OuterRight_Region2_Idle_State, OuterRight_Region2_State, NULL);
      oosmos_OrthoRegionInit(pOrtho, OuterRight_Region3_State, OuterRight_State, OuterRight_Region3_Idle_State, NULL);
        oosmos_LeafInit(pOrtho, OuterRight_Region3_Idle_State, OuterRight_Region3_State, OuterRight_Region3_Idle_State_Code);
    oosmos_OrthoInit(pOrtho, OuterLeft_State, ROOT, OuterLeft_State_Code);
      oosmos_OrthoRegionInit(pOrtho, OuterLeft_Region1_State, OuterLeft_State, OuterLeft_Region1_Idle_State, NULL);
        oosmos_LeafInit(pOrtho, OuterLeft_Region1_Idle_State, OuterLeft_Region1_State, OuterLeft_Region1_Idle_State_Code);
        oosmos_LeafInit(pOrtho, OuterLeft_Region1_Settled_State, OuterLeft_Region1_State, NULL);
      oosmos_OrthoRegionInit(pOrtho, OuterLeft_Region2_State, OuterLeft_State, OuterLeft_Region2_Idle_State, NULL);
        oosmos_LeafInit(pOrtho, OuterLeft_Region2_Idle_State, OuterLeft_Region2_State, NULL);
      oosmos_OrthoRegionInit(pOrtho, OuterLeft_Region3_State, OuterLeft_State, OuterLeft_Region3_Idle_State, NULL);
        oosmos_LeafInit(pOrtho, OuterLeft_Region3_Idle_State, OuterLeft_Region3_State, NULL);

  oosmos_Debug(pOrtho, NULL);
//<<<INIT

  return pOrtho;
}

extern int main(void)
{
  (void) orthoNew();

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelaySeconds(1);
  }
}
