//
// OOSMOS - The Object-Oriented State Machine Operating System
//
// Copyright (C) 2014-2016  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <http://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "oosmos.h"

//
// This is the master list of all state machines created inside
// of OOSMOS objects.  The master control loop visits each of
// these state machines forever.
//
static oosmos_sStateMachine * pStateMachineList;

//
// This is the master list of all active objects created in the system.
// The master control loop visits each of these objects forever.
//
static oosmos_sActiveObject * pActiveObjectList;

//
// Predefined events, pre-initialized for fast delivery.
//
static const oosmos_sEvent EventTIMEOUT  = { oosmos_TIMEOUT,  NULL };
static const oosmos_sEvent EventINSTATE  = { oosmos_INSTATE,  NULL };
static const oosmos_sEvent EventDEFAULT  = { oosmos_DEFAULT,  NULL };
static const oosmos_sEvent EventENTER    = { oosmos_ENTER,    NULL };
static const oosmos_sEvent EventEXIT     = { oosmos_EXIT,     NULL };
static const oosmos_sEvent EventCOMPLETE = { oosmos_COMPLETE, NULL };

static int IS_TIMEOUT_ACTIVE(const oosmos_sState * pState)
{
  return pState->Timeout.Start != 0 || pState->Timeout.End != 0;
}

static void RESET_TIMEOUT(oosmos_sState * pState)
{
  pState->Timeout.Start = pState->Timeout.End = 0;
}

static int IS_SYNC_TIMEOUT_ACTIVE(const oosmos_sState * pState)
{
  return pState->SyncTimeout.Start != 0 || pState->SyncTimeout.End != 0;
}

static void RESET_SYNC_TIMEOUT(oosmos_sState * pState)
{
  pState->SyncTimeout.Start = pState->SyncTimeout.End = 0;
}

#ifdef oosmos_DEBUG
  static oosmos_sStateMachine * GetStateMachine(oosmos_sState * pState)
  {
    oosmos_sState * pCandidateState = NULL;

    while (pState != NULL) {
      pCandidateState = pState;

      pState = pState->pParent;
    }

    return (oosmos_sStateMachine *) pCandidateState;
  }
#endif

static void SyncInit(oosmos_sState * pState)
{
  pState->SyncContext       = 0;
  pState->SyncDirtyEvent    = false;
  pState->HasYielded        = false;
  pState->HasSyncBlockBegin = 0;
  RESET_SYNC_TIMEOUT(pState);
}

static void CheckInSyncBlock(oosmos_sState * pState)
{
  if (!pState->HasSyncBlockBegin) {
    printf("Must bracket sync calls with oosmos_SyncBegin and oosmos_SyncEnd.\n");
    while (true);
  }
}

static bool DeliverEvent(oosmos_sRegion * pRegion, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  const OOSMOS_tCode pCode = pState->pCode;

  if (pCode == NULL) {
    return false;
  }

  return pCode(pRegion->pObject, pRegion, pEvent);
}

static bool OOSMOS_SyncTimeoutMS(oosmos_sRegion * pRegion, int MS)
{
  oosmos_sState * pCurrent = pRegion->pCurrent;

  CheckInSyncBlock(pCurrent);

  if (IS_SYNC_TIMEOUT_ACTIVE(pCurrent)) {
    if (oosmos_TimeoutHasExpired(&pCurrent->SyncTimeout)) {
      RESET_SYNC_TIMEOUT(pCurrent);

      DeliverEvent(pRegion, pCurrent, &EventTIMEOUT);
      return true;
    }
  }
  else {
    oosmos_TimeoutInMS(&pCurrent->SyncTimeout, MS);
  }

  return false;
}

//
// Deliver an event to all regions recursively.  Within each region, deliver the
// event to the current state and then up through all parent states recursively.
//
static bool ProcessEvent(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  oosmos_sState * pState;
  oosmos_sState * pCurrent = pRegion->pCurrent;

#ifdef oosmos_ORTHO
  switch (pCurrent->Type) {
    case OOSMOS_OrthoType: {
      oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;
      oosmos_sOrthoRegion * pOrthoRegion = pOrtho->pFirstOrthoRegion;
      int Handled = 0;

      for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->pNextOrthoRegion)
        Handled += ProcessEvent(&pOrthoRegion->Region, pEvent);

      if (Handled > 0)
        return true;

      break;
    }
  }
#endif

  for (pState = pCurrent; pState != pRegion->Composite.State.pParent; pState = pState->pParent) {
    if (DeliverEvent(pRegion, pState, pEvent)) {
      return true;
    }
  }

  return false;
}

static bool ProcessTimeouts(oosmos_sRegion * pRegion)
{
  oosmos_sState * pCurrent = pRegion->pCurrent;

  switch (pCurrent->Type) {
#ifdef oosmos_ORTHO
    case OOSMOS_OrthoType: {
      oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;
      oosmos_sOrthoRegion * pOrthoRegion = pOrtho->pFirstOrthoRegion;
      int Handled = 0;

      for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->pNextOrthoRegion)
        Handled += ProcessTimeouts(&pOrthoRegion->Region);

      if (Handled > 0)
        return true;

      break;
    }
#endif
    default: {
      oosmos_sState * pState;

      for (pState = pCurrent; pState != &pRegion->Composite.State; pState = pState->pParent) {
        if (IS_TIMEOUT_ACTIVE(pState) && oosmos_TimeoutHasExpired(&pState->Timeout)) {
          RESET_TIMEOUT(pState);

          if (DeliverEvent(pRegion, pState, &EventTIMEOUT))
            return true;
        }
      }
    }
  }

  return false;
}

static bool IsInState(const oosmos_sRegion * pRegion, const oosmos_sState * pQueriedState)
{
  oosmos_sState * pCurrent = pRegion->pCurrent;

  if (pCurrent == NULL)
    return false;

  switch (pCurrent->Type) {
#ifdef oosmos_ORTHO
    case OOSMOS_OrthoType: {
      oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;
      oosmos_sOrthoRegion * pOrthoRegion = pOrtho->pFirstOrthoRegion;

      for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->pNextOrthoRegion)
        if (IsInState(&pOrthoRegion->Region, pQueriedState))
          return true;

      break;
    }
#endif
    default: {
      oosmos_sState * pState;

      for (pState = pCurrent; pState != &pRegion->Composite.State; pState = pState->pParent)
        if (pState == pQueriedState)
          return true;

      break;
    }
  }

  return false;
}

static void DefaultTransitions(oosmos_sRegion * pRegion, oosmos_sState * pState)
{
  if (pState == NULL)
    return;

  DeliverEvent(pRegion, pState, &EventDEFAULT);
  pRegion->pCurrent = pState;

  oosmos_DebugCode(
    if (pRegion->Composite.State.pStateMachine->Debug)
      oosmos_DebugPrint("--> %s (+default)\n", pState->pName);
  )

  SyncInit(pState);
  DeliverEvent(pRegion, pState, &EventENTER);

  switch (pState->Type) {
    case OOSMOS_CompositeType: {
      oosmos_sComposite * pComposite = (oosmos_sComposite *) pState;
      DefaultTransitions(pRegion, pComposite->pDefault);
      break;
    }
    case OOSMOS_StateMachineType: {
      oosmos_sStateMachine * pStateMachine = (oosmos_sStateMachine *) pState;
      DefaultTransitions(&pStateMachine->Region, pStateMachine->Region.Composite.pDefault);
      break;
    }
#ifdef oosmos_ORTHO
    case OOSMOS_OrthoType: {
      oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pState;
      oosmos_sOrthoRegion * pOrthoRegion = pOrtho->pFirstOrthoRegion;

      for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->pNextOrthoRegion)
        DefaultTransitions(&pOrthoRegion->Region, pOrthoRegion->Region.Composite.pDefault);

      break;
    }
#endif
    case OOSMOS_LeafType: {
      //
      // Always send a completion event to the leaf state.
      //
      DeliverEvent(pRegion, pState, &EventCOMPLETE);
      break;
    }
    default:
      oosmos_DebugPrint("Unhandled type %d in DefaultTransitions.\n", pState->Type);
      break;
  }
}

static void StateInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  pState->pParent       = pParent;
  pState->pCode         = pCode;

  RESET_TIMEOUT(pState);

  oosmos_DebugCode(
    pState->pName = pName;
    pState->pStateMachine = GetStateMachine(pState);
  )
}

static void RegionInit(const char * pName, oosmos_sRegion * pRegion,
                       oosmos_sState * pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode, void * pObject)
{
  OOSMOS_CompositeInit(pName, &pRegion->Composite, pParent, pDefault, pCode);
  pRegion->pCurrent    = NULL;
  pRegion->pObject     = pObject;
}

static void Complete(oosmos_sRegion * pRegion, oosmos_sState * pState)
{
#ifdef oosmos_ORTHO
  if (pState->Type == OOSMOS_OrthoRegionType)
    pState = pState->pParent;
#endif

  switch (pState->Type) {
    case OOSMOS_CompositeType: {
      oosmos_DebugCode(
        if (pRegion->Composite.State.pStateMachine->Debug)
          oosmos_DebugPrint("((( %s Complete )))\n", pState->pName);
      )

      DeliverEvent(pRegion, pState, &EventCOMPLETE);
      break;
    }
#ifdef oosmos_ORTHO
    case OOSMOS_OrthoType: {
      oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pState;
      oosmos_sOrthoRegion * pOrthoRegion = pOrtho->pFirstOrthoRegion;
      int Visited   = 0;
      int Completed = 0;

      for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->pNextOrthoRegion) {
        Visited += 1;

        if (pOrthoRegion->Region.pCurrent->Type == OOSMOS_FinalType)
          Completed += 1;
      }

      if (Completed == Visited) {
        oosmos_DebugCode(
          if (pRegion->Composite.State.pStateMachine->Debug)
            oosmos_DebugPrint("((( %s Complete )))\n", pState->pName);
        )

        DeliverEvent(pRegion, pState, &EventCOMPLETE);
      }

      break;
    }
#endif
    //
    // A Final state can be a child of a state machine, not just
    // an ortho.  Do nothing.
    //
    case OOSMOS_StateMachineType: {
      break;
    }

    default:
      oosmos_DebugPrint("Unhandled type %d in Complete.\n", pState->Type);
      break;
  }
}

extern bool OOSMOS_Transition(oosmos_sRegion * pRegion, oosmos_sState * pToState)
{
  oosmos_sState * pFromState = pRegion->pCurrent;
  oosmos_sState * pLCA       = OOSMOS_LCA(pRegion->pCurrent, pToState);

  OOSMOS_Exit (pRegion, pFromState, pLCA);
  OOSMOS_Enter(pRegion, pLCA,       pToState);

  pFromState->TransitionOccurred = true;
  SyncInit(pFromState);

  return true;
}

extern oosmos_sState * OOSMOS_LCA(oosmos_sState * pFrom, oosmos_sState * pTo)
{
  oosmos_sState * pFromPath;
  oosmos_sState * pToPath;

  if (pFrom == pTo)
    return pFrom->pParent;

  for (pFromPath = pFrom; pFromPath != NULL; pFromPath = pFromPath->pParent)
    for (pToPath = pTo; pToPath != NULL; pToPath = pToPath->pParent)
      if (pFromPath == pToPath)
        return pFromPath;

  return NULL;
}

extern void OOSMOS_LeafInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  StateInit(pName, pState, pParent, pCode);
  pState->Type = OOSMOS_LeafType;
}

extern void OOSMOS_CompositeInit(const char * pName, oosmos_sComposite * pComposite,
                       oosmos_sState * pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode)
{
  StateInit(pName, &pComposite->State, pParent, pCode);
  pComposite->State.Type = OOSMOS_CompositeType;
  pComposite->pDefault = pDefault;
}

#ifdef oosmos_ORTHO
extern void OOSMOS_OrthoInit(const char * pName, oosmos_sOrtho * pOrtho, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  StateInit(pName, &pOrtho->State, pParent, pCode);
  pOrtho->State.Type = OOSMOS_OrthoType;
  pOrtho->pFirstOrthoRegion = NULL;
}

extern void OOSMOS_OrthoRegionInit(const char * pName, oosmos_sOrthoRegion * pOrthoRegion,
                              oosmos_sOrtho * pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode, void * pObject)
{
  oosmos_sOrtho * pOrtho = (oosmos_sOrtho*) pParent;

  oosmos_sRegion * pRegion = &pOrthoRegion->Region;
  RegionInit(pName, pRegion, (oosmos_sState*) pParent, pDefault, pCode, pObject);
  pRegion->Composite.State.Type = OOSMOS_OrthoRegionType;

  pOrthoRegion->pNextOrthoRegion = pOrtho->pFirstOrthoRegion;
  pOrtho->pFirstOrthoRegion = pOrthoRegion;
}
#endif

extern void OOSMOS_ChoiceInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  OOSMOS_LeafInit(pName, pState, pParent, pCode);
  pState->Type = OOSMOS_LeafType;
}

extern void OOSMOS_FinalInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  OOSMOS_LeafInit(pName, pState, pParent, pCode);
  pState->Type = OOSMOS_FinalType;
}

extern void OOSMOS_StateMachineInit(const char * pName, oosmos_sStateMachine *pStateMachine, oosmos_sState * pDefault, oosmos_sQueue * pEventQueue,
                     void * pCurrentEvent, size_t CurrentEventSize, void * pObject)
{
  oosmos_sRegion * pRegion = &pStateMachine->Region;
  RegionInit(pName, pRegion, NULL, pDefault, NULL, pObject);
  pRegion->Composite.State.Type = OOSMOS_StateMachineType;

  oosmos_DebugCode(
    pStateMachine->Debug = false;
  )

  pStateMachine->IsStarted        = false;
  pStateMachine->pEventQueue      = pEventQueue;
  pStateMachine->pCurrentEvent    = pCurrentEvent;
  pStateMachine->CurrentEventSize = (uint16_t) CurrentEventSize;

  pStateMachine->pNext            = pStateMachineList;
  pStateMachineList               = pStateMachine;
}

extern void OOSMOS_StateMachineDetach(oosmos_sStateMachine * pStateMachineToDetach)
{
  oosmos_sStateMachine  * pStateMachine;
  oosmos_sStateMachine ** ppStateMachineLink = &pStateMachineList;

  for (pStateMachine = pStateMachineList; pStateMachine != NULL; pStateMachine = pStateMachine->pNext) {
    if (pStateMachine == pStateMachineToDetach) {
      *ppStateMachineLink = pStateMachine->pNext;
      return;
    }

    ppStateMachineLink = &(pStateMachine->pNext);
  }
}

extern void OOSMOS_Enter(oosmos_sRegion * pRegion, const oosmos_sState * pLCA, oosmos_sState * pTarget)
{
  #define MAX_STATE_NESTING 7

  oosmos_sState * pState;
  oosmos_sState * pStates[MAX_STATE_NESTING];
  oosmos_sState ** ppState = pStates;

  SyncInit(pTarget);

  *ppState = NULL;

  for (pState = pTarget; pState != pLCA; pState = pState->pParent)
    *(++ppState) = pState;

  while ((pState = *ppState--) != NULL) {
    switch (pState->Type) {
      case OOSMOS_CompositeType:
      case OOSMOS_FinalType:
#ifdef oosmos_ORTHO
      case OOSMOS_OrthoType:
      case OOSMOS_OrthoRegionType:
#endif
      case OOSMOS_LeafType: {
        pRegion->pCurrent = pState;

        oosmos_DebugCode(
          if (pRegion->Composite.State.pStateMachine->Debug)
            oosmos_DebugPrint("--> %s\n", pState->pName);
        )

        DeliverEvent(pRegion, pState, &EventENTER);

        switch (pState->Type) {
          case OOSMOS_LeafType:
            DeliverEvent(pRegion, pState, &EventCOMPLETE);
            break;

          case OOSMOS_FinalType:
            Complete(pRegion, pState->pParent);
            break;
        }

        break;
      }
      default:
        oosmos_DebugPrint("Unhandled type %d in Enter_.\n", pTarget->Type);
        break;
    }
  }

  switch (pTarget->Type) {
    case OOSMOS_CompositeType: {
      oosmos_sComposite * pComposite = (oosmos_sComposite*) pTarget;
      DefaultTransitions(pRegion, pComposite->pDefault);
      break;
    }
#ifdef oosmos_ORTHO
    case OOSMOS_OrthoRegionType: {
      oosmos_sOrthoRegion * pOrthoRegion = (oosmos_sOrthoRegion *) pTarget;
      DefaultTransitions(pRegion, pOrthoRegion->Region.Composite.pDefault);
      break;
    }
    case OOSMOS_OrthoType: {
      oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pTarget;
      oosmos_sOrthoRegion * pOrthoRegion = pOrtho->pFirstOrthoRegion;

      for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->pNextOrthoRegion)
        DefaultTransitions(&pOrthoRegion->Region, pOrthoRegion->Region.Composite.pDefault);

      break;
    }
#endif
    case OOSMOS_StateMachineType: {
      oosmos_sStateMachine * pStateMachine = (oosmos_sStateMachine*) pTarget;
      DefaultTransitions(pRegion, pStateMachine->Region.Composite.pDefault);
      break;
    }
    case OOSMOS_FinalType:
    case OOSMOS_LeafType:
      break;
    default:
      oosmos_DebugPrint("Unhandled type %d in Enter_ (2)\n", pTarget->Type);
      break;
  }
}

extern void OOSMOS_Exit(oosmos_sRegion * pRegion, oosmos_sState * pSource, const oosmos_sState * pLCA)
{
  oosmos_sState * pState;
  oosmos_sState * pCurrent = pRegion->pCurrent;

#ifdef oosmos_ORTHO
  switch (pCurrent->Type) {
    case OOSMOS_OrthoType: {
      oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;
      oosmos_sOrthoRegion * pOrthoRegion = pOrtho->pFirstOrthoRegion;

      for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->pNextOrthoRegion)
        OOSMOS_Exit(&pOrthoRegion->Region, pOrthoRegion->Region.pCurrent, &pOrthoRegion->Region.Composite.State);

      break;
    }
  }
#endif

  for (pState = pCurrent; pState != pLCA; pState = pState->pParent) {
    RESET_TIMEOUT(pState);

    oosmos_DebugCode(
      if (pRegion->Composite.State.pStateMachine->Debug)
        oosmos_DebugPrint("    %s -->\n", pState->pName);
    )

    DeliverEvent(pRegion, pState, &EventEXIT);
  }
}

extern bool OOSMOS_IsInState(const oosmos_sStateMachine * pStateMachine, const oosmos_sState * pState)
{
  return IsInState(&pStateMachine->Region, pState);
}

extern bool oosmos_StateTimeoutUS(oosmos_sRegion * pRegion, uint32_t TimeoutInUS)
{
  oosmos_sState * pCurrent = pRegion->pCurrent;
  oosmos_TimeoutInUS(&pCurrent->Timeout, TimeoutInUS);
  return true;
}

extern bool oosmos_StateTimeoutMS(oosmos_sRegion * pRegion, uint32_t TimeoutInMS)
{
  return oosmos_StateTimeoutUS(pRegion, TimeoutInMS * 1000);
}

extern bool oosmos_StateTimeoutSeconds(oosmos_sRegion * pRegion, uint32_t TimeoutInSeconds)
{
  return oosmos_StateTimeoutMS(pRegion, TimeoutInSeconds * 1000);
}

extern bool oosmos_PushEventToQueue(oosmos_sQueue * pNotifyQueue, int EventCode)
{
  oosmos_sEvent Event;

  Event.Code     = EventCode;
  Event.pContext = NULL;

  oosmos_QueuePush(pNotifyQueue, &Event, sizeof(Event));
  return true;
}

extern bool oosmos_PushEventToStateMachine(oosmos_sStateMachine * pStateMachine, int EventCode)
{
  return oosmos_PushEventToQueue(pStateMachine->pEventQueue, EventCode);
}

extern void OOSMOS_SubscriberListInit(oosmos_sSubscriberList * pSubscriber, size_t ListElements)
{
  for (; ListElements-- > 0; pSubscriber++) {
    pSubscriber->pNotifyQueue   = NULL;
    pSubscriber->Event.Code     = 0;
    pSubscriber->Event.pContext = NULL;
  }
}

extern bool OOSMOS_SubscriberListNotify(oosmos_sSubscriberList * pSubscriber, size_t ListElements)
{
  for (; ListElements-- > 0; pSubscriber++) {
    if (pSubscriber->pNotifyQueue == NULL)
      continue;

    oosmos_QueuePush(pSubscriber->pNotifyQueue, &pSubscriber->Event, sizeof(pSubscriber->Event));
  }

  return true;
}

extern bool OOSMOS_SubscriberListNotifyWithArgs(oosmos_sSubscriberList * pSubscriber, void * pEventArg, size_t EventSize, size_t ListElements)
{
  oosmos_sEvent * pEvent = (oosmos_sEvent *) pEventArg;

  for (; ListElements-- > 0; pSubscriber++) {
    if (pSubscriber->pNotifyQueue == NULL)
      continue;

    pEvent->Code     = pSubscriber->Event.Code;
    pEvent->pContext = pSubscriber->Event.pContext;

    oosmos_QueuePush(pSubscriber->pNotifyQueue, pEvent, EventSize);
  }

  return true;
}

extern void OOSMOS_SubscriberListAdd(oosmos_sSubscriberList * pSubscriber, size_t ListElements, oosmos_sQueue * pNotifyQueue, int EventCode, void * pContext)
{
  for (; ListElements-- > 0; pSubscriber++) {
    if (pSubscriber->pNotifyQueue == NULL) {
      pSubscriber->pNotifyQueue   = pNotifyQueue;
      pSubscriber->Event.Code     = EventCode;
      pSubscriber->Event.pContext = pContext;
      return;
    }
  }

  while (true);
}

oosmos_DebugCode(
  extern void oosmos_Debug(oosmos_sStateMachine * pStateMachine, bool Debug, const char * (*pEventNameConverter)(int))
  {
    pStateMachine->Debug = Debug;
    pStateMachine->pEventNameConverter = pEventNameConverter;
  }
)

extern void oosmos_RunStateMachine(oosmos_sStateMachine * pStateMachine)
{
  int EventsHandled = 0;
  oosmos_sRegion * pRegion = &pStateMachine->Region;

  if (!pStateMachine->IsStarted) {
    DefaultTransitions(&pStateMachine->Region, pStateMachine->Region.Composite.pDefault);
    pStateMachine->IsStarted = true;
  }

  if (pStateMachine->pEventQueue != NULL) {
    while (oosmos_QueuePop(pStateMachine->pEventQueue, pStateMachine->pCurrentEvent, pStateMachine->CurrentEventSize)) {
      oosmos_sEvent * pEvent = (oosmos_sEvent *) pStateMachine->pCurrentEvent;
      EventsHandled += 1;

      oosmos_DebugCode(
        if (pStateMachine->Debug) {
          const int EventCode = pEvent->Code;

          if (pStateMachine->pEventNameConverter != NULL)
            oosmos_DebugPrint("EVENT: %s (%d)\n", (pStateMachine->pEventNameConverter)(EventCode), EventCode);
          else
            oosmos_DebugPrint("EVENT: %d\n", EventCode);
        }
      )

      //
      // Send INSTATE with the popped event to each state up the tree.
      // This supports oosmos_SyncWaitEvent... functions.
      //
      {
        oosmos_sEvent LocalEventINSTATE;
        LocalEventINSTATE.Code     = oosmos_INSTATE;
        LocalEventINSTATE.pContext = pEvent;
        ProcessEvent(pRegion, &LocalEventINSTATE);
      }

      //
      // Send the popped event to each state up the tree.
      //
      if (ProcessEvent(pRegion, pEvent))
        return;
    }
  }

  //
  // If there were no popped events, then send a bare INSTATE
  // event to each state up the tree.
  //
  if (EventsHandled == 0) {
    ProcessEvent(pRegion, &EventINSTATE);
  }

  ProcessTimeouts(pRegion);
}

//
// The user calls this function from their main loop.
// Continuously. Forever.
//
extern void oosmos_RunStateMachines(void)
{
  oosmos_sStateMachine * pStateMachine;
  oosmos_sActiveObject * pActiveObject;

  for (pStateMachine = pStateMachineList; pStateMachine != NULL; ) {
    oosmos_sStateMachine * pNext = pStateMachine->pNext;
    oosmos_RunStateMachine(pStateMachine);
    pStateMachine = pNext;
  }

  for (pActiveObject = pActiveObjectList; pActiveObject != NULL; pActiveObject = pActiveObject->pNext) {
    pActiveObject->pFunction(pActiveObject->pObject);
  }
}

#if defined(ARDUINO)
  extern void oosmos_DelayUS(int US)
  {
    delayMicroseconds(US);
  }

  extern void oosmos_DelayMS(int MS)
  {
    delay(MS);
  }

  extern void oosmos_DelaySeconds(int Seconds)
  {
    delay(Seconds * 1000);
  }

  static uint32_t GetFreeRunningMicroseconds(void)
  {
    return micros();
  }

  #ifdef oosmos_DEBUG
    #include <stdarg.h>
    extern void OOSMOS_ArduinoPrintf(const char * pFormat, ...)
    {
      char Buffer[100];

      va_list Args;
      va_start(Args, pFormat);
        vsprintf(Buffer, pFormat, Args);
      va_end(Args);

      Serial.print(Buffer);
    }
  #endif
#elif defined(__PIC32MX)
  #include <plib.h>
  static int PIC32_ClockSpeedInMHz;

  extern void oosmos_DelayUS(const int Microseconds)
  {
    const uint32_t CoreTimerTicks = (PIC32_ClockSpeedInMHz/2) * Microseconds;
    const uint32_t Start = ReadCoreTimer();
    const uint64_t End = Start + CoreTimerTicks;

    while (true) {
      uint64_t Now = ReadCoreTimer();

      if (Now < Start)
        Now += 0x100000000;

      if (Now >= End)
        break;
    }
  }

  extern void oosmos_DelayMS(const int Milliseconds)
  {
    oosmos_DelayUS(Milliseconds * 1000);
  }

  extern void oosmos_DelaySeconds(const int Seconds)
  {
    oosmos_DelayMS(Seconds * 1000);
  }

  extern void oosmos_ClockSpeedInMHz(int ClockSpeedInMHz)
  {
    PIC32_ClockSpeedInMHz = ClockSpeedInMHz;
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
  }

  extern int oosmos_GetClockSpeedInMHz(void)
  {
    return PIC32_ClockSpeedInMHz;
  }

  static uint32_t GetFreeRunningMicroseconds(void)
  {
    return ReadCoreTimer() / (PIC32_ClockSpeedInMHz / 2);
  }
#elif defined(_MSC_VER)
  #include <windows.h>

  extern void oosmos_DelayUS(int US)
  {
    printf("oosmos_DelayUS is not implemented on Windows.\n");
  }

  extern void oosmos_DelayMS(int MS)
  {
    Sleep(MS);
  }

  extern void oosmos_DelaySeconds(int Seconds)
  {
    Sleep(Seconds * 1000);
  }

  static uint32_t GetFreeRunningMicroseconds(void)
  {
    SYSTEMTIME st;
    uint64_t MS = 0;
    uint64_t US;
    GetSystemTime(&st);

    MS += st.wMilliseconds;
    MS += st.wSecond * 1000;
    MS += st.wMinute * 60000;
    MS += st.wHour   * 3600000;

    US = MS * 1000;

    return (uint32_t) US;
  }
#elif defined(__linux__) || defined(__APPLE__)
  #include "sys/time.h"
  #include <stddef.h>

  extern void oosmos_DelayUS(int US)
  {
    usleep(US);
  }

  extern void oosmos_DelayMS(int MS)
  {
    usleep(MS * 1000);
  }

  extern void oosmos_DelaySeconds(int Seconds)
  {
    sleep(Seconds);
  }

  static uint32_t GetFreeRunningMicroseconds(void)
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    const uint64_t US = tv.tv_sec * 1000000ULL + tv.tv_usec;
    return (uint32_t) US;
  }

#elif defined(__MBED__)
  #include "mbed.h"

  extern void oosmos_DelayUS(int US)
  {
    wait(US/1000000.0f);
  }

  extern void oosmos_DelayMS(int MS)
  {
    wait(MS/1000.0f);
  }

  extern void oosmos_DelaySeconds(int Seconds)
  {
    wait(Seconds);
  }

  static uint32_t GetFreeRunningMicroseconds(void)
  {
    static Timer TimerObj;
    static bool  Initialized = false;

    if (!Initialized) {
      TimerObj.start();
      Initialized = true;
    }

    return (uint32_t) TimerObj.read_us();
  }
#endif

extern void oosmos_TimeoutInSeconds(oosmos_sTimeout * pTimeout, uint32_t Seconds)
{
  oosmos_TimeoutInMS(pTimeout, Seconds * 1000);
}

extern void oosmos_TimeoutInMS(oosmos_sTimeout * pTimeout, uint32_t Milliseconds)
{
  oosmos_TimeoutInUS(pTimeout, Milliseconds * 1000);
}

extern void oosmos_TimeoutInUS(oosmos_sTimeout * pTimeout, uint32_t Microseconds)
{
  const uint32_t Start = GetFreeRunningMicroseconds();

  pTimeout->Start = Start;
  pTimeout->End   = Start + Microseconds;
}

extern bool oosmos_TimeoutHasExpired(const oosmos_sTimeout * pTimeout)
{
  const uint32_t Start = pTimeout->Start;
  uint64_t       End   = pTimeout->End;
  uint64_t       Now   = GetFreeRunningMicroseconds();

  if (End < Start)
    End += 0x100000000ULL;

  if (Now < Start)
    Now += 0x100000000ULL;

  return Now >= End;
}

extern void oosmos_RegisterActiveObject(void * pObject, void (*pFunction)(void *), oosmos_sActiveObject * pActiveObject)
{
  pActiveObject->pObject = pObject;
  pActiveObject->pFunction = pFunction;

  pActiveObject->pNext = pActiveObjectList;
  pActiveObjectList = pActiveObject;
}

#if defined(oosmos_DEBUG)
  extern void oosmos_DebugInit(void)
  {
    #if defined(PIC32)
      oosmos_DebugCode(DBINIT();)
    #endif
  }
#endif

extern void oosmos_QueueSetBehaviorFunc(oosmos_sQueue * pQueue, oosmos_eQueueFullBehavior (*pFunc)(void *), void * pContext)
{
  pQueue->pFullBehaviorFunc = pFunc;
  pQueue->pContext          = pContext;
}

extern void OOSMOS_QueueConstruct(oosmos_sQueue * pQueue, void * pQueueData, size_t QueueDataSize, size_t QueueElementSize)
{
  pQueue->pHead            = pQueueData;
  pQueue->pTail            = pQueueData;
  pQueue->pQueueData       = pQueueData;
  pQueue->pEnd             = ((char*)pQueue->pHead) + QueueDataSize;
  pQueue->QueueDataSize    = (uint16_t) QueueDataSize;
  pQueue->QueueElementSize = (uint16_t) QueueElementSize;
  pQueue->ByteCount        = 0;

  pQueue->pFullBehaviorFunc = NULL;
  pQueue->pContext          = NULL;

  memset(pQueueData, 0, QueueDataSize);
}

extern bool oosmos_QueuePush(oosmos_sQueue * pQueue, const void * pElement, size_t UserElementSize)
{
  const size_t QueueElementSize = pQueue->QueueElementSize;

  if (UserElementSize > QueueElementSize) {
    oosmos_DebugPrint("Bad queue size in push.\n");
    while (true);
  }

  if (pQueue->ByteCount < pQueue->QueueDataSize) {
    if (pQueue->pTail == pQueue->pEnd)
      pQueue->pTail = pQueue->pQueueData;

    memcpy(pQueue->pTail, pElement, UserElementSize);
    pQueue->pTail = ((char *) pQueue->pTail) + QueueElementSize;
    pQueue->ByteCount += (uint16_t) QueueElementSize;
  }
  else {
    oosmos_eQueueFullBehavior Behavior = oosmos_QueueFull_TossNew;

    if (pQueue->pFullBehaviorFunc != NULL)
      Behavior = (pQueue->pFullBehaviorFunc)(pQueue->pContext);

    if (Behavior == oosmos_QueueFull_TossOld) {
      // Pop... 
      {
        if (pQueue->pHead == pQueue->pEnd)
          pQueue->pHead = pQueue->pQueueData;

        pQueue->pHead = ((char *) pQueue->pHead) + QueueElementSize;
        pQueue->ByteCount -= (uint16_t) QueueElementSize;
      }

      oosmos_QueuePush(pQueue, pElement, UserElementSize);
    }
  }

  return true;
}

extern bool oosmos_QueuePop(oosmos_sQueue * pQueue, void * pElement, size_t UserElementSize)
{
  const size_t QueueElementSize = pQueue->QueueElementSize;

  if (UserElementSize > QueueElementSize) {
    oosmos_DebugPrint("Bad queue size in pop.\n");
    while (true);
  }

  if (pQueue->ByteCount > 0) {
    if (pQueue->pHead == pQueue->pEnd)
      pQueue->pHead = pQueue->pQueueData;

    memcpy(pElement, pQueue->pHead, UserElementSize);
    pQueue->pHead = ((char *) pQueue->pHead) + QueueElementSize;
    pQueue->ByteCount -= (uint16_t) QueueElementSize;
    return true;
  }

  return false;
}

extern bool OOSMOS_SyncYield(oosmos_sRegion * pRegion)
{
  oosmos_sState * pState = pRegion->pCurrent;

  CheckInSyncBlock(pState);

  if (pState->SyncDirtyEvent) {
    pState->SyncDirtyEvent = false;
    pState->HasYielded     = true;
    return false;
  }

  if (!pState->HasYielded) {
    pState->HasYielded = true;
    return false;
  }

  pState->HasYielded = false;
  return true;
}

extern bool OOSMOS_SyncDelayMS(oosmos_sRegion * pRegion, int MS)
{
  oosmos_sState * pCurrent = pRegion->pCurrent;

  CheckInSyncBlock(pCurrent);

  if (IS_SYNC_TIMEOUT_ACTIVE(pCurrent)) {
    if (oosmos_TimeoutHasExpired(&pCurrent->SyncTimeout)) {
      RESET_SYNC_TIMEOUT(pCurrent);
      return true;
    }
  }
  else {
    oosmos_TimeoutInMS(&pCurrent->SyncTimeout, MS);
  }

  return false;
}


extern bool OOSMOS_SyncWaitCond_TimeoutMS(oosmos_sRegion * pRegion, int TimeoutMS, bool * pTimeoutStatus, 
                                          bool Condition)
{
  CheckInSyncBlock(pRegion->pCurrent);

  if (Condition) {
    *pTimeoutStatus = false;
    return true;
  }

  if (OOSMOS_SyncTimeoutMS(pRegion, TimeoutMS)) {
    *pTimeoutStatus = true;
    return true;
  }

  return false;
}

extern bool OOSMOS_SyncWaitCond_TimeoutMS_Event(oosmos_sRegion * pRegion, int TimeoutMS, int NotificationEventCode,
                                                bool Condition)
{
  oosmos_sState * pState = pRegion->pCurrent;

  CheckInSyncBlock(pState);

  if (Condition) {
    RESET_SYNC_TIMEOUT(pState);
    return true;
  }

  if (OOSMOS_SyncTimeoutMS(pRegion, TimeoutMS)) {
    oosmos_sEvent TimeoutEvent;
    TimeoutEvent.Code     = NotificationEventCode;
    TimeoutEvent.pContext = NULL;

    pState->TransitionOccurred = false;

    DeliverEvent(pRegion, pState, &TimeoutEvent);

    if (!pState->TransitionOccurred)
      pState->SyncContext = -2;

    return false;
  }

  return false;
}

extern bool OOSMOS_SyncWaitCond_TimeoutMS_Exit(oosmos_sRegion * pRegion, int TimeoutMS,
                                               bool Condition)
{
  oosmos_sState * pState = pRegion->pCurrent;

  CheckInSyncBlock(pState);

  if (Condition) {
    RESET_SYNC_TIMEOUT(pState);
    return true;
  }

  if (OOSMOS_SyncTimeoutMS(pRegion, TimeoutMS)) {
    pState->SyncContext = -2;
    return false;
  }

  return false;
}

extern bool OOSMOS_SyncWaitEvent(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent, 
                                 int WaitEventCode)
{
  oosmos_sState * pState = pRegion->pCurrent;

  CheckInSyncBlock(pState);

  if (pState->SyncDirtyEvent) {
    pState->SyncDirtyEvent = false;
    return false;
  }

  oosmos_sEvent * pDeliveredEvent = (oosmos_sEvent *) pEvent->pContext;

  if (pDeliveredEvent != NULL && pDeliveredEvent->Code == WaitEventCode) {
    pState->SyncDirtyEvent = true;
    return true;
  }

  return false;
}

extern bool OOSMOS_SyncWaitEvent_TimeoutMS(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent, int TimeoutMS, bool * pTimedOut,
                                           int WaitEventCode)
{
  oosmos_sState * pState = pRegion->pCurrent;

  CheckInSyncBlock(pState);

  if (pState->SyncDirtyEvent) {
    pState->SyncDirtyEvent = false;
    return false;
  }

  oosmos_sEvent * pDeliveredEvent = (oosmos_sEvent *) pEvent->pContext;

  if (pDeliveredEvent != NULL && pDeliveredEvent->Code == WaitEventCode) {
    *pTimedOut = false;
    pState->SyncDirtyEvent = true;
    return true;
  }

  if (OOSMOS_SyncDelayMS(pRegion, TimeoutMS)) {
    *pTimedOut = true;
    pState->SyncDirtyEvent = true;
    return true;
  }

  return false;
}

extern bool OOSMOS_SyncWaitEvent_TimeoutMS_Event(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent, int TimeoutMS, int NotificationEventCode,
                                                 int WaitEventCode)
{
  oosmos_sState * pState = pRegion->pCurrent;

  CheckInSyncBlock(pState);

  if (pState->SyncDirtyEvent) {
    pState->SyncDirtyEvent = false;
    return false;
  }

  oosmos_sEvent * pDeliveredEvent = (oosmos_sEvent *) pEvent->pContext;

  if (pDeliveredEvent != NULL && pDeliveredEvent->Code == WaitEventCode) {
    RESET_SYNC_TIMEOUT(pState);
    pState->SyncDirtyEvent = true;
    return true;
  }

  if (OOSMOS_SyncDelayMS(pRegion, TimeoutMS)) {
    oosmos_sEvent TimeoutEvent;

    TimeoutEvent.Code     = NotificationEventCode;
    TimeoutEvent.pContext = NULL;

    pState->TransitionOccurred = false;

    DeliverEvent(pRegion, pState, &TimeoutEvent);

    if (!pState->TransitionOccurred)
      pState->SyncContext = -2;

    return false;
  }

  return false;
}

extern bool OOSMOS_SyncWaitEvent_TimeoutMS_Exit(oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent, int TimeoutMS,
                                                int WaitEventCode)
{
  oosmos_sState * pState = pRegion->pCurrent;

  CheckInSyncBlock(pState);

  if (pState->SyncDirtyEvent) {
    pState->SyncDirtyEvent = false;
    return false;
  }

  oosmos_sEvent * pDeliveredEvent = (oosmos_sEvent *) pEvent->pContext;

  if (pDeliveredEvent != NULL && pDeliveredEvent->Code == WaitEventCode) {
    RESET_SYNC_TIMEOUT(pState);
    pState->SyncDirtyEvent = true;
    return true;
  }

  if (OOSMOS_SyncDelayMS(pRegion, TimeoutMS)) {
    pState->SyncContext = -2;
    return false;
  }

  return false;
}

extern float oosmos_AnalogMapAccurate(float Value, float InMin, float InMax, float OutMin, float OutMax)
{
  return (Value - InMin) * (OutMax - OutMin) / (InMax - InMin) + OutMin;
}

extern long oosmos_AnalogMapFast(long Value, long InMin, long InMax, long OutMin, long OutMax)
{
  return oosmos_Min((Value - InMin) * ((OutMax+1) - OutMin) / ((InMax+1) - InMin) + OutMin, OutMax);
}

extern void OOSMOS_EndProgram(int Code)
{
#if defined(ARDUINO)
  while(1);
#else
  exit(Code);
#endif
}

