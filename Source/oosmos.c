//
// OOSMOS - The Object-Oriented State Machine Operating System
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

/*lint -e508 suppress "extern used with definition" */
/*lint -e537 suppress "repeated include" */
/*lint -e641 suppress "Converting enum 'OOSMOS_eTypes' to 'int'" */
/*lint -e788 suppress enum constant 'EEE' not used withing defaulted switch */

#include "oosmos.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//
// The master list of all state machines created in the system.
//
static oosmos_sStateMachine * pStateMachineList;

//
// The master list of all active objects created in the system.
//
static oosmos_sActiveObject * pActiveObjectList;

//
// The master list of all object threads in the system.
//
static oosmos_sObjectThread * pObjectThreadList;

//
// Predefined events, pre-initialized for fast delivery.
//
static const oosmos_sEvent EventTIMEOUT  = { oosmos_TIMEOUT,  NULL };
static const oosmos_sEvent EventPOLL     = { oosmos_POLL,     NULL };
static const oosmos_sEvent EventDEFAULT  = { oosmos_DEFAULT,  NULL };
static const oosmos_sEvent EventENTER    = { oosmos_ENTER,    NULL };
static const oosmos_sEvent EventEXIT     = { oosmos_EXIT,     NULL };
static const oosmos_sEvent EventCOMPLETE = { oosmos_COMPLETE, NULL };

//
// Special reserved event code used to mark an event as "spent" when handled so that
// the event isn't incorrectly recognized in subsequent oosmos_ThreadWaitEvent_*
// calls.
//
#define OOSMOS_EVENT_SPENT (-1)

static bool IS_TIMEOUT_ACTIVE(const oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  return (pState->m_Timeout.m_StartUS != 0) || (pState->m_Timeout.m_TimeoutUS != 0);
}

static void RESET_TIMEOUT(oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  pState->m_Timeout.m_StartUS   = 0;
  pState->m_Timeout.m_TimeoutUS = 0;
}

static bool IS_THREAD_TIMEOUT_ACTIVE(const oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  return (pState->m_ThreadTimeout.m_StartUS != 0) || (pState->m_ThreadTimeout.m_TimeoutUS != 0);
}

static void RESET_THREAD_TIMEOUT(oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  pState->m_ThreadTimeout.m_StartUS   = 0;
  pState->m_ThreadTimeout.m_TimeoutUS = 0;
}

static oosmos_sRegion * GetRegion(oosmos_sState * pState)
{
  oosmos_sState * pCandidateState = NULL;

  while (pState != NULL) {
    pCandidateState = pState;

    #if defined(oosmos_ORTHO)
      if (pCandidateState->m_Type == OOSMOS_OrthoRegionType) {
        /*lint -e826 suppress "Suspicious pointer-to-pointer conversion (area too small)" */
        /*lint -e740 suppress "Unusual pointer cast (incompatible indirect types)" */
        oosmos_sOrthoRegion * pOrthoRegion = (oosmos_sOrthoRegion *) pCandidateState;
        return &pOrthoRegion->m_Region;
      }
    #endif

    pState = pState->m_pParent;
  }

  // If we reached the top, then, by design, we are at the State Machine object.
  oosmos_POINTER_GUARD(pCandidateState);

  /*lint -e826 suppress "Suspicious pointer-to-pointer conversion (area too small)" */
  /*lint -e740 suppress "Unusual pointer cast (incompatible indirect types)" */
  oosmos_sStateMachine * pStateMachine = (oosmos_sStateMachine *) pCandidateState;
  return &pStateMachine->m_Region;
}

static oosmos_sStateMachine * GetStateMachine(const oosmos_sState * pState)
{
  const oosmos_sState * pCandidateState = NULL;

  while (pState != NULL) {
    pCandidateState = pState;

    pState = pState->m_pParent;
  }

  return (oosmos_sStateMachine *) pCandidateState;
}

#if defined(oosmos_DEBUG)
  static const char * GetFileName(const void * pObject)
  {
    const oosmos_sStateMachine * pStateMachine = GetStateMachine((const oosmos_sState *) pObject);

    oosmos_POINTER_GUARD(pStateMachine);

    const char * pFileName = pStateMachine->m_pFileName;
    const char * pSlash    = strrchr(pFileName, '/');

    if (pSlash != NULL) {
      return pSlash + 1;
    }

    const char * pBackSlash = strrchr(pFileName, '\\');

    if (pBackSlash != NULL) {
      return pBackSlash + 1;
    }

    return pFileName;
  }
#endif

static void ThreadInit(oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  pState->m_ThreadContext          = OOSMOS_THREAD_CONTEXT_BEGIN;
  pState->m_ThreadFunctionIsActive = false;
  RESET_THREAD_TIMEOUT(pState);
}

static bool DeliverEvent(oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  oosmos_POINTER_GUARD(pState);

  const OOSMOS_tCode pCode = pState->m_pCode;

  if (pCode == NULL) {
    return false;
  }

  return pCode(pState->m_pStateMachine->m_pObject, pState, pEvent);
}

static bool ThreadTimeoutMS(oosmos_sState * pState, uint32_t MS)
{
  oosmos_POINTER_GUARD(pState);

  if (IS_THREAD_TIMEOUT_ACTIVE(pState)) {
    if (oosmos_TimeoutHasExpired(&pState->m_ThreadTimeout)) {
      RESET_THREAD_TIMEOUT(pState);

      (void) DeliverEvent(pState, &EventTIMEOUT);
      return true;
    }
  }
  else {
    oosmos_TimeoutInMS(&pState->m_ThreadTimeout, MS);
  }

  return false;
}

//
// Deliver an event to all regions recursively.  Within each orthogonal region, deliver the
// event to the current state and then up through all parent states.
//
static bool PropagateEvent(const oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  oosmos_POINTER_GUARD(pRegion);
  oosmos_POINTER_GUARD(pEvent);

  oosmos_sState * pCurrent = pRegion->m_pCurrent;

  #if defined(oosmos_ORTHO)
    switch (pCurrent->m_Type) {
      case OOSMOS_OrthoType: {
        const oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;
        oosmos_sOrthoRegion * pOrthoRegion = pOrtho->m_pFirstOrthoRegion;
        int Handled = 0;

        for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->m_pNextOrthoRegion) {
          Handled += PropagateEvent(&pOrthoRegion->m_Region, pEvent);
        }

        if (Handled > 0) {
          return true;
        }

        break;
      }
      default: {
        break;
      }
    }
  #endif

  for (oosmos_sState * pState = pCurrent; pState != pRegion->m_Composite.m_State.m_pParent; pState = pState->m_pParent) {
    //
    // If a Thread sequence has already completed, don't deliver the event, which
    // would cause the sequence to be re-executed.
    //
    if (pEvent->m_Code == oosmos_POLL && pState->m_ThreadContext == OOSMOS_THREAD_CONTEXT_END) {
      continue;
    }

    //
    // Deliver the event to the state machine.  If the return code is true, then
    // stop propagating the event, unless it's a POLL event, in which case we
    // want to unconditionally propagate the poll event up the hierarchy.
    //
    if (DeliverEvent(pState, pEvent) && pEvent->m_Code != oosmos_POLL) {
      return true;
    }
  }

  return false;
}

static bool ProcessTimeouts(const oosmos_sRegion * pRegion)
{
  oosmos_POINTER_GUARD(pRegion);

  oosmos_sState * pCurrent = pRegion->m_pCurrent;

  switch (pCurrent->m_Type) {
    #if defined(oosmos_ORTHO)
      case OOSMOS_OrthoType: {
        const oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;
        oosmos_sOrthoRegion * pOrthoRegion = pOrtho->m_pFirstOrthoRegion;
        int Handled = 0;

        for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->m_pNextOrthoRegion) {
          Handled += ProcessTimeouts(&pOrthoRegion->m_Region);
        }

        if (Handled > 0) {
          return true;
        }
      }
    #endif

    /*lint -e764 suppress "switch statement does not have a case"*/
    /*lint -e616 suppress "control flows into case/default"*/
    /*lint -fallthrough */
    default: {
      for (oosmos_sState * pState = pCurrent; pState != &pRegion->m_Composite.m_State; pState = pState->m_pParent) {
        if (IS_TIMEOUT_ACTIVE(pState) && oosmos_TimeoutHasExpired(&pState->m_Timeout)) {
          RESET_TIMEOUT(pState);

          #if defined(oosmos_DEBUG)
            oosmos_DebugPrint("%s: EVENT TIMEOUT\n", GetFileName(pState));
          #endif

          if (DeliverEvent(pState, &EventTIMEOUT)) {
            return true;
          }
        }
      }
    }
  }

  return false;
}

static bool IsInState(const oosmos_sRegion * pRegion, const oosmos_sState * pQueriedState)
{
  oosmos_POINTER_GUARD(pRegion);

  const oosmos_sState * pCurrent = pRegion->m_pCurrent;

  if (pCurrent == NULL) {
    return false;
  }

  switch (pCurrent->m_Type) {
    #if defined(oosmos_ORTHO)
      case OOSMOS_OrthoType: {
        const oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;

        if (pCurrent == pQueriedState) {
          return true;
        }

        const oosmos_sOrthoRegion * pOrthoRegion = pOrtho->m_pFirstOrthoRegion;

        for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->m_pNextOrthoRegion) {
          if (IsInState(&pOrthoRegion->m_Region, pQueriedState)) {
            return true;
          }
        }

        break;
      }
    #endif

    /*lint -e764 suppress "switch statement does not have a case"*/
    default: {
      for (const oosmos_sState * pState = pCurrent; pState != NULL; pState = pState->m_pParent) {
        if (pState == pQueriedState) {
          return true;
        }
      }

      break;
    }
  }

  return false;
}

static void DefaultTransitions(oosmos_sRegion * pRegion, oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pRegion);
  oosmos_POINTER_GUARD(pState);

  (void) DeliverEvent(pState, &EventDEFAULT);
  pRegion->m_pCurrent = pState;

  #if defined(oosmos_DEBUG)
    if (pState->m_pStateMachine->m_Debug) {
      oosmos_DebugPrint("%s: ==> %s\n", GetFileName(pState), pState->m_pName);
    }
  #endif

  ThreadInit(pState);
  (void) DeliverEvent(pState, &EventENTER);

  switch (pState->m_Type) {
    case OOSMOS_CompositeType: {
      const oosmos_sComposite * pComposite = (oosmos_sComposite *) pState;
      DefaultTransitions(pRegion, pComposite->m_pDefault);
      break;
    }

    case OOSMOS_StateMachineType: {
      oosmos_sStateMachine * pStateMachine = (oosmos_sStateMachine *) pState;
      DefaultTransitions(&pStateMachine->m_Region, pStateMachine->m_Region.m_Composite.m_pDefault);
      break;
    }

    #if defined(oosmos_ORTHO)
      case OOSMOS_OrthoType: {
        const oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pState;
        oosmos_sOrthoRegion * pOrthoRegion = pOrtho->m_pFirstOrthoRegion;

        for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->m_pNextOrthoRegion) {
          DefaultTransitions(&pOrthoRegion->m_Region, pOrthoRegion->m_Region.m_Composite.m_pDefault);
        }

        break;
      }
    #endif

    case OOSMOS_LeafType: {
      //
      // This implements a NULL transition.
      //
      // Send a completion event to the leaf state, unless the state implements
      // the POLL event, in which case it is up to the writer of the state
      // code to eventually do an oosmos_ThreadComplete().
      //
      if (!DeliverEvent(pState, &EventPOLL)) {
        (void) DeliverEvent(pState, &EventCOMPLETE);
      }

      break;
    }

    default: {
      #if defined(oosmos_DEBUG)
        oosmos_DebugPrint("%s: Unhandled type %d in DefaultTransitions.\n", GetFileName(pState), pState->m_Type);
      #endif
      break;
    }
  }
}

static void StateInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  oosmos_POINTER_GUARD(pState);

  pState->m_pParent = pParent;
  pState->m_pCode   = pCode;

  RESET_TIMEOUT(pState);

  #if defined(oosmos_DEBUG)
    pState->m_pName = pName;
  #else
    oosmos_UNUSED(pName);
  #endif

  pState->m_pStateMachine = GetStateMachine(pState);
}

static void RegionInit(const char * pName, oosmos_sRegion * pRegion,
                       oosmos_sState * pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode)
{
  oosmos_POINTER_GUARD(pRegion);

  OOSMOS_CompositeInit(pName, &pRegion->m_Composite, pParent, pDefault, pCode);
  pRegion->m_pCurrent    = NULL;
}

static void Complete(oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  #if defined(oosmos_ORTHO)
    if (pState->m_Type == OOSMOS_OrthoRegionType) {
      pState = pState->m_pParent;
    }
  #endif

  switch (pState->m_Type) {
    case OOSMOS_LeafType:
    case OOSMOS_CompositeType: {
      #if defined(oosmos_DEBUG)
        if (pState->m_pStateMachine->m_Debug) {
          oosmos_DebugPrint("%s: ((( %s Complete )))\n", GetFileName(pState), pState->m_pName);
        }
      #endif

      (void) DeliverEvent(pState, &EventCOMPLETE);
      break;
    }

    #if defined(oosmos_ORTHO)
      case OOSMOS_OrthoType: {
        const oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pState;
        const oosmos_sOrthoRegion * pOrthoRegion = pOrtho->m_pFirstOrthoRegion;
        int Visited   = 0;
        int Completed = 0;

        for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->m_pNextOrthoRegion) {
          Visited += 1;

          if (pOrthoRegion->m_Region.m_pCurrent->m_Type == OOSMOS_FinalType) {
            Completed += 1;
          }
        }

        if (Completed == Visited) {
          #if defined(oosmos_DEBUG)
            if (pState->m_pStateMachine->m_Debug) {
              oosmos_DebugPrint("%s: ((( %s Complete )))\n", GetFileName(pState), pState->m_pName);
            }
          #endif

          (void) DeliverEvent(pState, &EventCOMPLETE);
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

    default: {
      #if defined(oosmos_DEBUG)
        oosmos_DebugPrint("%s: Unhandled type %d in Complete.\n", GetFileName(pState), pState->m_Type);
      #endif
      break;
    }
  }
}

static oosmos_sState * GetLCA(oosmos_sState * pFrom, oosmos_sState * pTo)
{
  oosmos_POINTER_GUARD(pFrom);
  oosmos_POINTER_GUARD(pTo);

  if (pFrom == pTo) {
    return pFrom->m_pParent;
  }

  for (oosmos_sState * pFromPath = pFrom; pFromPath != NULL; pFromPath = pFromPath->m_pParent) {
    for (oosmos_sState * pToPath = pTo; pToPath != NULL; pToPath = pToPath->m_pParent) {
      if (pFromPath == pToPath) {
        return pFromPath;
      }
    }
  }

  return NULL;
}

static void Enter(oosmos_sRegion * pRegion, const oosmos_sState * pLCA, oosmos_sState * pTarget)
{
  oosmos_POINTER_GUARD(pRegion);
  oosmos_POINTER_GUARD(pLCA);
  oosmos_POINTER_GUARD(pTarget);

  #define MAX_STATE_NESTING 7

  oosmos_sState * pState;
  oosmos_sState * pStates[MAX_STATE_NESTING];
  oosmos_sState ** ppStates = pStates;

  switch (pTarget->m_Type) {
    case OOSMOS_HistoryShallowType: {
      oosmos_sComposite * pParent  = (oosmos_sComposite *) pTarget->m_pParent;
      oosmos_sState     * pHistory = pParent->m_pHistoryState;

      pTarget = pHistory;
      break;
    }

    case OOSMOS_HistoryDeepType: {
      oosmos_sComposite * pParent  = (oosmos_sComposite *) pTarget->m_pParent;
      oosmos_sState     * pHistory = pParent->m_pHistoryState;

      pTarget = pHistory;

      while (pTarget->m_Type == OOSMOS_CompositeType) {
        pParent = (oosmos_sComposite *) pTarget;
        pTarget = pParent->m_pHistoryState;
      }

      break;
    }

    default: {
      break;
    }
  }

  //
  // We are passed the target state (pTarget) which could be nested within other states.  We need
  // to invoke the oosmos_ENTER code for all states between the pTarget state and the pLCA, but in
  // reverse order.  To do this, we store the states from pTarget (innermost) to pLCA (outermost)
  // in an array; then, working backwards through the array, invoke the oosmos_ENTER on each
  // state from pLCA to pTarget.
  //
  {
    *ppStates = NULL;

    for (pState = pTarget; pState != pLCA; pState = pState->m_pParent) {
      oosmos_POINTER_GUARD(pState);

      *(++ppStates) = pState;
    }
  }

  while ((pState = *ppStates--) != NULL) {
    ThreadInit(pState);

    switch (pState->m_Type) {
      case OOSMOS_CompositeType:
      case OOSMOS_FinalType:
        #if defined(oosmos_ORTHO)
          case OOSMOS_OrthoType:
          case OOSMOS_OrthoRegionType:
        #endif
      case OOSMOS_LeafType: {
        pRegion->m_pCurrent = pState;

        #if defined(oosmos_DEBUG)
          if (pRegion->m_Composite.m_State.m_pStateMachine->m_Debug) {
            oosmos_DebugPrint("%s: --> %s\n", GetFileName(pState), pState->m_pName);
          }
        #endif

        (void) DeliverEvent(pState, &EventENTER);

        switch (pState->m_Type) {
          case OOSMOS_LeafType: {
            if (!DeliverEvent(pState, &EventPOLL)) {
              (void) DeliverEvent(pState, &EventCOMPLETE);
            }

            break;
          }
          case OOSMOS_FinalType: {
            Complete(pState->m_pParent);
            break;
          }
          default: {
            break;
          }
        }

        break;
      }

      default: {
        #if defined(oosmos_DEBUG)
          oosmos_DebugPrint("%s: Unhandled type %d in Enter().\n", GetFileName(pState), pTarget->m_Type);
        #endif
        break;
      }
    }
  }

  switch (pTarget->m_Type) {
    case OOSMOS_CompositeType: {
      const oosmos_sComposite * pComposite = (oosmos_sComposite*) pTarget;
      DefaultTransitions(pRegion, pComposite->m_pDefault);
      break;
    }

    #if defined(oosmos_ORTHO)
      case OOSMOS_OrthoRegionType: {
        const oosmos_sOrthoRegion * pOrthoRegion = (oosmos_sOrthoRegion *) pTarget;
        DefaultTransitions(pRegion, pOrthoRegion->m_Region.m_Composite.m_pDefault);
        break;
      }

      case OOSMOS_OrthoType: {
        const oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pTarget;
        oosmos_sOrthoRegion * pOrthoRegion = pOrtho->m_pFirstOrthoRegion;

        for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->m_pNextOrthoRegion) {
          DefaultTransitions(&pOrthoRegion->m_Region, pOrthoRegion->m_Region.m_Composite.m_pDefault);
        }

        break;
      }
    #endif

    case OOSMOS_StateMachineType: {
      const oosmos_sStateMachine * pStateMachine = (oosmos_sStateMachine*) pTarget;
      DefaultTransitions(pRegion, pStateMachine->m_Region.m_Composite.m_pDefault);
      break;
    }

    case OOSMOS_FinalType:
    case OOSMOS_LeafType: {
      break;
    }

    default: {
      #if defined(oosmos_DEBUG)
        oosmos_DebugPrint("%s: Unhandled type %d in Enter_ (2)\n", GetFileName(pState), pTarget->m_Type);
      #endif
      break;
    }
  }
}

static void Exit(const oosmos_sRegion * pRegion, const oosmos_sState * pLCA)
{
  oosmos_POINTER_GUARD(pRegion);

  oosmos_sState * pCurrent = pRegion->m_pCurrent;

  #if defined(oosmos_ORTHO)
    switch (pCurrent->m_Type) {
      case OOSMOS_OrthoType: {
        const oosmos_sOrtho * pOrtho = (oosmos_sOrtho *) pCurrent;
        oosmos_sOrthoRegion * pOrthoRegion = pOrtho->m_pFirstOrthoRegion;

        for (; pOrthoRegion != NULL; pOrthoRegion = pOrthoRegion->m_pNextOrthoRegion) {
          Exit(&pOrthoRegion->m_Region, &pOrthoRegion->m_Region.m_Composite.m_State);
        }

        break;
      }
      default: {
        break;
      }
    }
  #endif

  for (oosmos_sState * pState = pCurrent; pState != pLCA; pState = pState->m_pParent) {
    RESET_TIMEOUT(pState);

    #if defined(oosmos_DEBUG)
      if (pState->m_pStateMachine->m_Debug) {
        oosmos_DebugPrint("%s:     %s -->\n", GetFileName(pState), pState->m_pName);
      }
    #endif

    oosmos_sComposite * pParent = (oosmos_sComposite *) pState->m_pParent;
    pParent->m_pHistoryState = pState;
    (void) DeliverEvent(pState, &EventEXIT);
  }
}

extern oosmos_sEvent * OOSMOS_GetCurrentEvent(const oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  return pState->m_pStateMachine->m_pCurrentEvent;
}

extern bool oosmos_ThreadComplete(oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  Complete(pState);
  return true;
}

extern bool OOSMOS_TransitionAction(oosmos_sState * pState, oosmos_sState * pToState, const oosmos_sEvent * pEvent, oosmos_tAction pActionCode)
{
  oosmos_POINTER_GUARD(pState);
  oosmos_POINTER_GUARD(pToState);

  oosmos_sRegion * pRegion = GetRegion(pState);
  oosmos_sState * pFromState = pRegion->m_pCurrent;

  oosmos_sState * pLCA = GetLCA(pFromState, pToState);

  if (pToState == pLCA) {
    pLCA = pLCA->m_pParent;
  }

  for (oosmos_sState * pS = pToState; pS != NULL; pS = pS->m_pParent) {
    /*lint -e826 suppress "Suspicious pointer-to-pointer conversion (area too small)" */
    if (pS->m_Type == OOSMOS_StateMachineType) {
      pRegion = (oosmos_sRegion *) pS;
      break;
    }

    #if defined(oosmos_ORTHO)
      if (pS->m_Type == OOSMOS_OrthoRegionType) {
        /*lint -e826 suppress "Suspicious pointer-to-pointer conversion (area too small)" */
        pRegion = (oosmos_sRegion *) pS;
        break;
      }
    #endif
  }

  Exit(pRegion, pLCA);

  pRegion->m_pCurrent = pLCA;

  if (pActionCode != NULL) {
    pActionCode(pState->m_pStateMachine->m_pObject, pState, pEvent);
  }

  Enter(pRegion, pLCA, pToState);
  ThreadInit(pFromState);
  return true;
}

extern bool OOSMOS_Transition(oosmos_sState * pState, oosmos_sState * pToState)
{
  return OOSMOS_TransitionAction(pState, pToState, NULL, NULL);
}

extern void OOSMOS_LeafInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  oosmos_POINTER_GUARD(pState);

  StateInit(pName, pState, pParent, pCode);
  pState->m_Type = OOSMOS_LeafType;
}

extern void OOSMOS_HistoryInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_eTypes Type)
{
  oosmos_POINTER_GUARD(pState);

  StateInit(pName, pState, pParent, NULL);
  pState->m_Type = Type;
}

extern void OOSMOS_CompositeInit(const char * pName, oosmos_sComposite * pComposite,
                       oosmos_sState * pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode)
{
  oosmos_POINTER_GUARD(pComposite);

  StateInit(pName, &pComposite->m_State, pParent, pCode);
  pComposite->m_State.m_Type = OOSMOS_CompositeType;
  pComposite->m_pDefault = pDefault;
  pComposite->m_pHistoryState = NULL;
}

#if defined(oosmos_ORTHO)
extern void OOSMOS_OrthoInit(const char * pName, oosmos_sOrtho * pOrtho, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  oosmos_POINTER_GUARD(pOrtho);

  StateInit(pName, &pOrtho->m_State, pParent, pCode);
  pOrtho->m_State.m_Type = OOSMOS_OrthoType;
  pOrtho->m_pFirstOrthoRegion = NULL;
}

extern void OOSMOS_OrthoRegionInit(const char * pName, oosmos_sOrthoRegion * pOrthoRegion,
                                   oosmos_sOrtho * pParent, oosmos_sState * pDefault, OOSMOS_tCode pCode)
{
  oosmos_POINTER_GUARD(pOrthoRegion);
  oosmos_POINTER_GUARD(pParent);

  oosmos_sOrtho * pOrtho = (oosmos_sOrtho*) pParent;

  oosmos_sRegion * pRegion = &pOrthoRegion->m_Region;
  RegionInit(pName, pRegion, (oosmos_sState*) pParent, pDefault, pCode);
  pRegion->m_Composite.m_State.m_Type = OOSMOS_OrthoRegionType;

  pOrthoRegion->m_pNextOrthoRegion = pOrtho->m_pFirstOrthoRegion;
  pOrtho->m_pFirstOrthoRegion = pOrthoRegion;
}
#endif

extern void OOSMOS_FinalInit(const char * pName, oosmos_sState * pState, oosmos_sState * pParent, OOSMOS_tCode pCode)
{
  oosmos_POINTER_GUARD(pState);

  OOSMOS_LeafInit(pName, pState, pParent, pCode);
  pState->m_Type = OOSMOS_FinalType;
}

extern void OOSMOS_StateMachineInit(const char * pFileName, const char * pName, oosmos_sStateMachine *pStateMachine,
                                    oosmos_sState * pDefault, oosmos_sQueue * pEventQueue, void * pCurrentEvent,
                                    size_t CurrentEventSize, void * pObject)
{
  oosmos_POINTER_GUARD(pStateMachine);

  oosmos_sRegion * pRegion = &pStateMachine->m_Region;
  RegionInit(pName, pRegion, NULL, pDefault, NULL);
  pRegion->m_Composite.m_State.m_Type = OOSMOS_StateMachineType;

  #if defined(oosmos_DEBUG)
    pStateMachine->m_Debug     = false;
    pStateMachine->m_pFileName = pFileName;
  #else
    oosmos_UNUSED(pFileName);
  #endif

  pStateMachine->m_IsStarted        = false;
  pStateMachine->m_pEventQueue      = pEventQueue;
  pStateMachine->m_pCurrentEvent    = (oosmos_sEvent *) pCurrentEvent;
  pStateMachine->m_CurrentEventSize = CurrentEventSize;
  pStateMachine->m_pObject          = pObject;

  pStateMachine->m_pNext            = pStateMachineList;
  pStateMachineList                 = pStateMachine;
}

extern void OOSMOS_StateMachineDetach(const oosmos_sStateMachine * pStateMachineToDetach)
{
  oosmos_sStateMachine ** ppStateMachineLink = &pStateMachineList;

  for (oosmos_sStateMachine * pStateMachine = pStateMachineList; pStateMachine != NULL; pStateMachine = pStateMachine->m_pNext) {
    if (pStateMachine == pStateMachineToDetach) {
      *ppStateMachineLink = pStateMachine->m_pNext;
      return;
    }

    ppStateMachineLink = &(pStateMachine->m_pNext);
  }
}

extern bool OOSMOS_IsInState(const oosmos_sStateMachine * pStateMachine, const oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pStateMachine);
  oosmos_POINTER_GUARD(pState);

  return IsInState(&pStateMachine->m_Region, pState);
}

extern bool oosmos_StateTimeoutUS(oosmos_sState * pState, uint32_t TimeoutInUS)
{
  oosmos_POINTER_GUARD(pState);

  oosmos_TimeoutInUS(&pState->m_Timeout, TimeoutInUS);
  return true;
}

extern bool oosmos_StateTimeoutMS(oosmos_sState * pState, uint32_t TimeoutInMS)
{
  oosmos_POINTER_GUARD(pState);

  return oosmos_StateTimeoutUS(pState, TimeoutInMS * 1000);
}

extern bool oosmos_StateTimeoutSeconds(oosmos_sState * pState, uint32_t TimeoutInSeconds)
{
  oosmos_POINTER_GUARD(pState);

  return oosmos_StateTimeoutMS(pState, TimeoutInSeconds * 1000);
}

extern void OOSMOS_PushEventCodeToStateMachine(const oosmos_sStateMachine * pStateMachine, int EventCode)
{
  oosmos_POINTER_GUARD(pStateMachine);

  const oosmos_sEvent Event = { EventCode, NULL };

  oosmos_QueuePush(pStateMachine->m_pEventQueue, &Event, sizeof(Event));
}

extern void OOSMOS_PushEventToStateMachine(const oosmos_sStateMachine * pStateMachine, const void * pEvent, size_t EventSize)
{
  oosmos_POINTER_GUARD(pStateMachine);
  oosmos_POINTER_GUARD(pEvent);

  oosmos_QueuePush(pStateMachine->m_pEventQueue, pEvent, EventSize);
}

extern void OOSMOS_SubscriberListInit(oosmos_sSubscriberList * pSubscriber, size_t ListElements)
{
  oosmos_POINTER_GUARD(pSubscriber);

  /*lint -e441 suppress "for clause irregularity: loop variable 'pSubscriber' not found in 2nd for expression" */
  for (; ListElements-- > 0; pSubscriber++) {
    pSubscriber->m_pNotifyQueue     = NULL;
    pSubscriber->m_Event.m_Code     = 0;
    pSubscriber->m_Event.m_pContext = NULL;
  }
}

extern void OOSMOS_SubscriberListNotify(const oosmos_sSubscriberList * pSubscriber, size_t ListElements)
{
  oosmos_POINTER_GUARD(pSubscriber);

  /*lint -e441 suppress "for clause irregularity: loop variable 'pSubscriber' not found in 2nd for expression" */
  for (; ListElements-- > 0; pSubscriber++) {
    if (pSubscriber->m_pNotifyQueue == NULL) {
      continue;
    }

    oosmos_QueuePush(pSubscriber->m_pNotifyQueue, &pSubscriber->m_Event, sizeof(pSubscriber->m_Event));
  }
}

extern void OOSMOS_SubscriberListNotifyWithArgs(const oosmos_sSubscriberList * pSubscriber, void * pEventArg, size_t EventSize, size_t ListElements)
{
  oosmos_POINTER_GUARD(pSubscriber);
  oosmos_POINTER_GUARD(pEventArg);

  oosmos_sEvent * pEvent = (oosmos_sEvent *) pEventArg;

  /*lint -e441 suppress "for clause irregularity: loop variable 'pSubscriber' not found in 2nd for expression" */
  for (; ListElements-- > 0; pSubscriber++) {
    if (pSubscriber->m_pNotifyQueue == NULL) {
      continue;
    }

    pEvent->m_Code     = pSubscriber->m_Event.m_Code;
    pEvent->m_pContext = pSubscriber->m_Event.m_pContext;

    oosmos_QueuePush(pSubscriber->m_pNotifyQueue, pEvent, EventSize);
  }
}

extern void OOSMOS_SubscriberListAdd(oosmos_sSubscriberList * pSubscriber, size_t ListElements, oosmos_sQueue * pNotifyQueue, int EventCode, void * pContext)
{
  oosmos_POINTER_GUARD(pSubscriber);
  oosmos_POINTER_GUARD(pNotifyQueue);

  /*lint -e441 suppress "for clause irregularity: loop variable 'pSubscriber' not found in 2nd for expression" */
  for (; ListElements-- > 0; pSubscriber++) {
    if (pSubscriber->m_pNotifyQueue == NULL) {
      pSubscriber->m_pNotifyQueue     = pNotifyQueue;
      pSubscriber->m_Event.m_Code     = EventCode;
      pSubscriber->m_Event.m_pContext = pContext;
      return;
    }
  }

  oosmos_FOREVER();
}

#if defined(oosmos_DEBUG)
  extern void OOSMOS_Debug(oosmos_sStateMachine * pStateMachine, const char * (*pEventNameConverter)(int))
  {
    oosmos_POINTER_GUARD(pStateMachine);

    pStateMachine->m_Debug = true;
    pStateMachine->m_pEventNameConverter = pEventNameConverter;
  }
#endif

extern void OOSMOS_RunStateMachine(oosmos_sStateMachine * pStateMachine)
{
  oosmos_POINTER_GUARD(pStateMachine);

  int EventsHandled = 0;
  oosmos_sRegion * pRegion = &pStateMachine->m_Region;

  if (!pStateMachine->m_IsStarted) {
    DefaultTransitions(&pStateMachine->m_Region, pStateMachine->m_Region.m_Composite.m_pDefault);
    pStateMachine->m_IsStarted = true;
  }

  if (pStateMachine->m_pEventQueue != NULL) {
    while (oosmos_QueuePop(pStateMachine->m_pEventQueue, pStateMachine->m_pCurrentEvent, pStateMachine->m_CurrentEventSize)) {
      oosmos_sEvent * pEvent = (oosmos_sEvent *) pStateMachine->m_pCurrentEvent;
      EventsHandled += 1;

      #if defined(oosmos_DEBUG)
        if (pStateMachine->m_Debug) {
          const int EventCode = pEvent->m_Code;

          if (pStateMachine->m_pEventNameConverter != NULL) {
            oosmos_DebugPrint("%s: EVENT: %s (%d)\n", GetFileName(pStateMachine), (pStateMachine->m_pEventNameConverter)(EventCode), EventCode);
          }
          else {
            oosmos_DebugPrint("%s: EVENT: %d\n", GetFileName(pStateMachine), EventCode);
          }
        }
      #endif

      //
      // Send POLL with the popped event to each state up the tree.
      // This supports oosmos_ThreadWaitEvent... functions.
      //
      (void) PropagateEvent(pRegion, &EventPOLL);

      //
      // Send the popped event to each region, which will deliver it from the current state
      // of each region up the hierarchy to the outer state of its region.
      //
      if (PropagateEvent(pRegion, pEvent)) {
        return;
      }
    }
  }

  //
  // If there were no popped events, then send a bare POLL
  // event to each state up the tree.
  //
  if (EventsHandled == 0) {
    (void) PropagateEvent(pRegion, &EventPOLL);
  }

  (void) ProcessTimeouts(pRegion);
}

//
// The user calls this function from their main loop.
// Continuously. Forever.
//
extern void oosmos_RunStateMachines(void)
{
  for (oosmos_sStateMachine * pStateMachine = pStateMachineList; pStateMachine != NULL; ) {
    oosmos_sStateMachine * pNext = pStateMachine->m_pNext;
    OOSMOS_RunStateMachine(pStateMachine);
    pStateMachine = pNext;
  }

  for (oosmos_sActiveObject * pActiveObject = pActiveObjectList; pActiveObject != NULL; pActiveObject = pActiveObject->m_pNext) {
    pActiveObject->m_pFunc(pActiveObject->m_pObject);
  }

  for (oosmos_sObjectThread * pObjectThread = pObjectThreadList; pObjectThread != NULL; pObjectThread = pObjectThread->m_pNext) {
    if (pObjectThread->m_bRunning) {
      pObjectThread->m_pFunc(pObjectThread->m_pObject, &pObjectThread->m_LeafState);
    }
  }
}

extern void oosmos_TimeoutInSeconds(oosmos_sTimeout * pTimeout, uint32_t TimeoutSeconds)
{
  oosmos_POINTER_GUARD(pTimeout);

  oosmos_TimeoutInMS(pTimeout, TimeoutSeconds * 1000);
}

extern void oosmos_TimeoutInMS(oosmos_sTimeout * pTimeout, uint32_t TimeoutMS)
{
  oosmos_POINTER_GUARD(pTimeout);

  oosmos_TimeoutInUS(pTimeout, TimeoutMS * 1000);
}

extern void oosmos_TimeoutInUS(oosmos_sTimeout * pTimeout, uint32_t TimeoutUS)
{
  oosmos_POINTER_GUARD(pTimeout);

  const uint32_t StartUS = oosmos_GetFreeRunningUS();

  pTimeout->m_StartUS   = StartUS;
  pTimeout->m_TimeoutUS = TimeoutUS;
}

extern bool oosmos_TimeoutHasExpired(const oosmos_sTimeout * pTimeout)
{
  oosmos_POINTER_GUARD(pTimeout);

  const uint32_t StartUS   = pTimeout->m_StartUS;
  const uint32_t TimeoutUS = pTimeout->m_TimeoutUS;
  const uint32_t NowUS     = oosmos_GetFreeRunningUS();

  return (NowUS - StartUS) >= TimeoutUS;
}

extern void OOSMOS_ActiveObjectInit(void * pObject, oosmos_sActiveObject * pActiveObject, oosmos_tActiveObjectFunc pFunc)
{
  oosmos_POINTER_GUARD(pFunc);
  oosmos_POINTER_GUARD(pActiveObject);

  pActiveObject->m_pObject = pObject;
  pActiveObject->m_pFunc   = pFunc;

  pActiveObject->m_pNext = pActiveObjectList;
  pActiveObjectList = pActiveObject;
}

extern void OOSMOS_ObjectThreadInit(void * pObject, oosmos_sObjectThread * pObjectThread, oosmos_tObjectThreadFunc pFunc, bool bRunning)
{
  pObjectThread->m_pFunc    = pFunc;
  pObjectThread->m_pObject  = pObject;
  pObjectThread->m_bRunning = bRunning;

  OOSMOS_LeafInit("", &pObjectThread->m_LeafState, NULL, NULL);

  ThreadInit(&pObjectThread->m_LeafState);

  pObjectThread->m_pNext = pObjectThreadList;
  pObjectThreadList = pObjectThread;
}

extern void oosmos_ObjectThreadStart(oosmos_sObjectThread * pObjectThread)
{
  pObjectThread->m_bRunning = true;
}

extern void oosmos_ObjectThreadStop(oosmos_sObjectThread * pObjectThread)
{
  pObjectThread->m_bRunning = false;
}

extern void oosmos_ObjectThreadRestart(oosmos_sObjectThread * pObjectThread)
{
  ThreadInit(&pObjectThread->m_LeafState);
}

extern void oosmos_QueueSetBehaviorFunc(oosmos_sQueue * pQueue, oosmos_eQueueFullBehavior (*pCallback)(void *), void * pContext)
{
  oosmos_POINTER_GUARD(pQueue);
  oosmos_POINTER_GUARD(pCallback);

  pQueue->m_pFullBehaviorFunc = pCallback;
  pQueue->m_pContext          = pContext;
}

extern void OOSMOS_QueueConstruct(oosmos_sQueue * pQueue, void * pQueueData, size_t QueueDataSize, size_t QueueElementSize)
{
  oosmos_POINTER_GUARD(pQueue);
  oosmos_POINTER_GUARD(pQueueData);

  pQueue->m_pHead            = pQueueData;
  pQueue->m_pTail            = pQueueData;
  pQueue->m_pQueueData       = pQueueData;
  pQueue->m_pEnd             = ((char*)pQueue->m_pHead) + QueueDataSize;
  pQueue->m_QueueDataSize    = QueueDataSize;
  pQueue->m_QueueElementSize = QueueElementSize;
  pQueue->m_ByteCount        = 0;

  pQueue->m_pFullBehaviorFunc = NULL;
  pQueue->m_pContext          = NULL;

  (void) memset(pQueueData, 0, QueueDataSize);
}

extern void oosmos_QueuePush(oosmos_sQueue * pQueue, const void * pElement, size_t UserElementSize)
{
  oosmos_POINTER_GUARD(pQueue);
  oosmos_POINTER_GUARD(pElement);

  const size_t QueueElementSize = pQueue->m_QueueElementSize;

  if (UserElementSize > QueueElementSize) {
    oosmos_DebugPrint("Bad queue size in push.\n");
    oosmos_FOREVER();
  }

  if (pQueue->m_ByteCount < pQueue->m_QueueDataSize) {
    if (pQueue->m_pTail == pQueue->m_pEnd) {
      pQueue->m_pTail = pQueue->m_pQueueData;
    }

    /*lint -e420 suppress "Apparent access beyond array for function" */
    (void) memcpy(pQueue->m_pTail, pElement, UserElementSize);
    pQueue->m_pTail = ((char *) pQueue->m_pTail) + QueueElementSize;
    pQueue->m_ByteCount += QueueElementSize;
  }
  else {
    oosmos_eQueueFullBehavior Behavior = oosmos_QueueFull_TossNew;

    if (pQueue->m_pFullBehaviorFunc != NULL) {
      Behavior = (pQueue->m_pFullBehaviorFunc)(pQueue->m_pContext);
    }

    if (Behavior == oosmos_QueueFull_TossOld) {
      // Pop...
      {
        if (pQueue->m_pHead == pQueue->m_pEnd) {
          pQueue->m_pHead = pQueue->m_pQueueData;
        }

        pQueue->m_pHead = ((char *) pQueue->m_pHead) + QueueElementSize;
        pQueue->m_ByteCount -= QueueElementSize;
      }

      oosmos_QueuePush(pQueue, pElement, UserElementSize);
    }
  }
}

extern bool oosmos_QueuePop(oosmos_sQueue * pQueue, void * pElement, size_t UserElementSize)
{
  oosmos_POINTER_GUARD(pQueue);
  oosmos_POINTER_GUARD(pElement);

  const size_t QueueElementSize = pQueue->m_QueueElementSize;

  if (UserElementSize > QueueElementSize) {
    oosmos_DebugPrint("Bad queue size in pop.\n");
    oosmos_FOREVER();
  }

  if (pQueue->m_ByteCount > 0) {
    if (pQueue->m_pHead == pQueue->m_pEnd) {
      pQueue->m_pHead = pQueue->m_pQueueData;
    }

    (void) memcpy(pElement, pQueue->m_pHead, UserElementSize);
    pQueue->m_pHead = ((char *) pQueue->m_pHead) + QueueElementSize;
    pQueue->m_ByteCount -= QueueElementSize;
    return true;
  }

  return false;
}

extern bool OOSMOS_ThreadYield(oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pState);

  if (!pState->m_ThreadFunctionIsActive) {
    pState->m_ThreadFunctionIsActive = true;
    return false;
  }

  pState->m_ThreadFunctionIsActive = false;
  return true;
}

extern bool OOSMOS_ThreadDelayMS(oosmos_sState * pState, uint32_t MS)
{
  oosmos_POINTER_GUARD(pState);

  if (IS_THREAD_TIMEOUT_ACTIVE(pState)) {
    if (oosmos_TimeoutHasExpired(&pState->m_ThreadTimeout)) {
      RESET_THREAD_TIMEOUT(pState);
      return true;
    }
  }
  else {
    oosmos_TimeoutInMS(&pState->m_ThreadTimeout, MS);
  }

  return false;
}

extern bool OOSMOS_ThreadDelayUS(oosmos_sState * pState, uint32_t US)
{
  oosmos_POINTER_GUARD(pState);

  if (IS_THREAD_TIMEOUT_ACTIVE(pState)) {
    if (oosmos_TimeoutHasExpired(&pState->m_ThreadTimeout)) {
      RESET_THREAD_TIMEOUT(pState);
      return true;
    }
  }
  else {
    oosmos_TimeoutInUS(&pState->m_ThreadTimeout, US);
  }

  return false;
}


extern bool OOSMOS_ThreadWaitCond_TimeoutMS(oosmos_sState * pState, bool Condition, uint32_t TimeoutMS, bool * pTimeoutStatus)
{
  oosmos_POINTER_GUARD(pState);
  oosmos_POINTER_GUARD(pTimeoutStatus);

  if (!pState->m_ThreadFunctionIsActive) {
    RESET_THREAD_TIMEOUT(pState);
    pState->m_ThreadFunctionIsActive = true;
  }

  if (Condition) {
    *pTimeoutStatus = false;
    pState->m_ThreadFunctionIsActive = false;
    return true;
  }

  if (ThreadTimeoutMS(pState, TimeoutMS)) {
    *pTimeoutStatus = true;
    pState->m_ThreadFunctionIsActive = false;
    return true;
  }

  return false;
}

extern bool OOSMOS_ThreadWaitEvent(const oosmos_sState * pState, int WaitEventCode)
{
  oosmos_POINTER_GUARD(pState);

  oosmos_sEvent * pCurrentEvent = OOSMOS_GetCurrentEvent(pState);

  if (pCurrentEvent->m_Code != OOSMOS_EVENT_SPENT && pCurrentEvent->m_Code == WaitEventCode) {
    pCurrentEvent->m_Code = OOSMOS_EVENT_SPENT;
    return true;
  }

  return false;
}

extern bool OOSMOS_ThreadWaitEvent_TimeoutMS(oosmos_sState * pState, int WaitEventCode, uint32_t TimeoutMS, bool * pTimedOutStatus)
{
  oosmos_POINTER_GUARD(pState);
  oosmos_POINTER_GUARD(pTimedOutStatus);

  oosmos_sEvent * pCurrentEvent = OOSMOS_GetCurrentEvent(pState);

  if (pCurrentEvent->m_Code != OOSMOS_EVENT_SPENT && pCurrentEvent->m_Code == WaitEventCode) {
    *pTimedOutStatus = false;
    pCurrentEvent->m_Code = OOSMOS_EVENT_SPENT;
    return true;
  }

  if (OOSMOS_ThreadDelayMS(pState, TimeoutMS)) {
    *pTimedOutStatus = true;
    return true;
  }

  return false;
}

extern double oosmos_AnalogMapAccurate(double Value, double InMin, double InMax, double OutMin, double OutMax)
{
  return (Value - InMin) * (OutMax - OutMin) / (InMax - InMin) + OutMin;
}

extern int32_t oosmos_AnalogMapFast(int32_t Value, int32_t InMin, int32_t InMax, int32_t OutMin, int32_t OutMax)
{
  return oosmos_Min((Value - InMin) * ((OutMax+1) - OutMin) / ((InMax+1) - InMin) + OutMin, OutMax);
}

extern void OOSMOS_EndProgram(int Code)
{
  #if defined(ARDUINO)
    oosmos_FOREVER();
  #else
    exit(Code);
  #endif
}

#if defined(ARDUINO)
  extern void oosmos_DelayUS(uint32_t US)
  {
    delayMicroseconds(US);
  }

  extern void oosmos_DelayMS(uint32_t MS)
  {
    delay(MS);
  }

  extern void oosmos_DelaySeconds(uint32_t Seconds)
  {
    delay(Seconds * 1000);
  }

  extern uint32_t oosmos_GetFreeRunningUS(void)
  {
    return micros();
  }

  #if defined(oosmos_DEBUG)
    #include <stdarg.h>
    extern void OOSMOS_ArduinoPrintf(const char * pFormat, ...)
    {
      char Buffer[100];

      #if defined(ARDUINO)
        #ifndef arduino_BaudRate
          #define arduino_BAUD_RATE 115200
        #endif

        static bool First = true;

        if (First) {
          First = false;

          Serial.end();
          Serial.begin(arduino_BAUD_RATE);
        }
      #endif

      va_list Args;
      va_start(Args, pFormat);
        vsprintf(Buffer, pFormat, Args);
      va_end(Args);

      Serial.print(Buffer);
      Serial.flush();
    }
  #endif
#elif defined(__PIC32MX)
  #include <plib.h>
  static int PIC32_ClockSpeedInMHz;

  extern void oosmos_DelayUS(uint32_t DelayMS)
  {
    const uint32_t CoreTimerTicks = (PIC32_ClockSpeedInMHz / 2) * DelayMS;
    const uint32_t Start = ReadCoreTimer();
    const uint64_t End = Start + CoreTimerTicks;

    for (;;) {
      uint64_t Now = ReadCoreTimer();

      if (Now < Start) {
        Now += 0x100000000;
      }

      if (Now >= End) {
        break;
      }
    }
  }

  extern void oosmos_DelayMS(uint32_t DelayMS)
  {
    oosmos_DelayUS(Milliseconds * 1000);
  }

  extern void oosmos_DelaySeconds(uint32_t DelaySeconds)
  {
    oosmos_DelayMS(Seconds * 1000);
  }

  extern void oosmos_ClockSpeedInMHz(uint32_t ClockSpeedInMHz)
  {
    PIC32_ClockSpeedInMHz = ClockSpeedInMHz;
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
  }

  extern uint32_t oosmos_GetClockSpeedInMHz(void)
  {
    return PIC32_ClockSpeedInMHz;
  }

  extern uint32_t oosmos_GetFreeRunningUS(void)
  {
    return ReadCoreTimer() / (PIC32_ClockSpeedInMHz / 2);
  }
#elif defined(_WIN32)
  #include <windows.h>

  extern void oosmos_DelayUS(uint32_t US)
  {
    (void) printf("oosmos_DelayUS is not implemented on Windows.\n");

    oosmos_UNUSED(US);
  }

  extern void oosmos_DelayMS(uint32_t MS)
  {
    Sleep(MS);
  }

  extern void oosmos_DelaySeconds(uint32_t Seconds)
  {
    Sleep(Seconds * 1000);
  }

  extern uint32_t oosmos_GetFreeRunningUS(void)
  {
    SYSTEMTIME st;
    GetSystemTime(&st);

    uint64_t MS = 0;
    MS += st.wMilliseconds;
    MS += st.wSecond * 1000ULL;
    MS += st.wMinute * 60000ULL;
    MS += st.wHour   * 3600000ULL;

    const uint64_t US = MS * 1000;

    return (uint32_t) US;
  }

#elif defined(__linux__) || defined(__APPLE__)
  #include <sys/time.h>
  #include <stddef.h>
  #include <time.h>

  #if _POSIX_C_SOURCE >= 199309L
    #include <time.h>   // for nanosleep
  #else
    #include <unistd.h> // for usleep
#endif

  extern void oosmos_DelayUS(uint32_t US)
  {
    #if _POSIX_C_SOURCE >= 199309L
      struct timespec ts;
      const uint32_t MS = US / 1000UL;
      ts.tv_sec  = MS / 1000UL;
      ts.tv_nsec = (MS % 1000UL) * 1000000UL;

      nanosleep(&ts, NULL);
    #else
      usleep(US);
    #endif
  }

  extern void oosmos_DelayMS(uint32_t MS)
  {
    oosmos_DelayUS(MS * 1000);
  }

  extern void oosmos_DelaySeconds(uint32_t Seconds)
  {
    sleep(Seconds);
  }

  extern uint32_t oosmos_GetFreeRunningUS(void)
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    const uint64_t US = tv.tv_sec * 1000000ULL + tv.tv_usec;
    return (uint32_t) US;
  }

#elif defined(__MBED__)
  #include "mbed.h"

  extern void oosmos_DelayUS(uint32_t US)
  {
    wait(US/1000000.0f);
  }

  extern void oosmos_DelayMS(uint32_t MS)
  {
    wait(MS/1000.0f);
  }

  extern void oosmos_DelaySeconds(uint32_t Seconds)
  {
    wait(Seconds);
  }

  extern uint32_t oosmos_GetFreeRunningUS(void)
  {
    static Timer TimerObj;
    static bool  Initialized = false;

    if (!Initialized) {
      TimerObj.start();
      Initialized = true;
    }

    return (uint32_t) TimerObj.read_us();
  }
#elif defined(__IAR_SYSTEMS_ICC__)
  #if 0
    extern void oosmos_DelayUS(uint32_t US)
    {
      // Not implemented
      oosmos_FOREVER();
      oosmos_UNUSED(US);
    }
  #endif

  extern void oosmos_DelayMS(uint32_t MS)
  {
    const uint32_t StartingTickMS = HAL_GetTick();

    while ((HAL_GetTick() - StartingTickMS) < MS) {
      continue;
    }
  }

  extern void oosmos_DelaySeconds(uint32_t Seconds)
  {
    oosmos_DelayMS(Seconds * 1000);
  }

  extern uint32_t oosmos_GetFreeRunningUS(void)
  {
    return HAL_GetTick() * 1000;
  }
#endif
