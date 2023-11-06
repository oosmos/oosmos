//
// OOSMOS motion Example
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
#include <stdbool.h>
#include <stdio.h>

//>>>EVENTS
enum {
  evDownCommand = 1,
  evLowerLimitSwitchClosed = 2,
  evLowerLimitSwitchOpen = 3,
  evStopCommand = 4,
  evUpCommand = 5,
  evUpperLimitSwitchClosed = 6,
  evUpperLimitSwitchOpen = 7
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case evDownCommand: return "evDownCommand";
      case evLowerLimitSwitchClosed: return "evLowerLimitSwitchClosed";
      case evLowerLimitSwitchOpen: return "evLowerLimitSwitchOpen";
      case evStopCommand: return "evStopCommand";
      case evUpCommand: return "evUpCommand";
      case evUpperLimitSwitchClosed: return "evUpperLimitSwitchClosed";
      case evUpperLimitSwitchOpen: return "evUpperLimitSwitchOpen";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef struct motionTag motion;

typedef union {
  oosmos_sEvent Base;
} uEvents;

struct motionTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sOrtho Active_State;
      oosmos_sOrthoRegion Active_Region1_State;
        oosmos_sComposite Active_Region1_Limits_State;
          oosmos_sLeaf Active_Region1_Limits_AtUpperLimit_State;
          oosmos_sLeaf Active_Region1_Limits_InBounds_State;
          oosmos_sChoice Active_Region1_Limits_Choice1_State;
          oosmos_sLeaf Active_Region1_Limits_AtLowerLimit_State;
      oosmos_sOrthoRegion Active_Region2_State;
        oosmos_sLeaf Active_Region2_Stopped_State;
        oosmos_sComposite Active_Region2_Moving_State;
          oosmos_sLeaf Active_Region2_Moving_Up_State;
          oosmos_sLeaf Active_Region2_Moving_Down_State;
//<<<DECL
};

static int Failures = 0;

static bool IsUpperLimitSwitchOpen(void) {
  return true;
}

static bool IsLowerLimitSwitchOpen(void) {
  return false;
}

//>>>CODE
static bool Active_Region1_Limits_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motion * pMotion = (motion *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evUpperLimitSwitchOpen: {
      return oosmos_Transition(pMotion, pState, Active_Region1_Limits_AtUpperLimit_State);
    }
    case evUpperLimitSwitchClosed: {
      return oosmos_Transition(pMotion, pState, Active_Region1_Limits_InBounds_State);
    }
    case evLowerLimitSwitchClosed: {
      return oosmos_Transition(pMotion, pState, Active_Region1_Limits_InBounds_State);
    }
    case evLowerLimitSwitchOpen: {
      return oosmos_Transition(pMotion, pState, Active_Region1_Limits_AtLowerLimit_State);
    }
  }

  return false;
}

static bool Active_Region1_Limits_Choice1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motion * pMotion = (motion *) pObject;

  if (oosmos_EventCode(pEvent) == oosmos_ENTER) {
    if (IsUpperLimitSwitchOpen()) {
      return oosmos_Transition(pMotion, pState, Active_Region1_Limits_AtUpperLimit_State);
    }
    else if (IsLowerLimitSwitchOpen()) {
      return oosmos_Transition(pMotion, pState, Active_Region1_Limits_AtLowerLimit_State);
    }
    else {
      return oosmos_Transition(pMotion, pState, Active_Region1_Limits_InBounds_State);
    }
  }

  return false;
}

static bool Active_Region2_Stopped_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motion * pMotion = (motion *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evUpCommand: {
      return oosmos_Transition(pMotion, pState, Active_Region2_Moving_Up_State);
    }
    case evDownCommand: {
      return oosmos_Transition(pMotion, pState, Active_Region2_Moving_Down_State);
    }
  }

  return false;
}

static bool Active_Region2_Moving_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motion * pMotion = (motion *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evStopCommand: {
      return oosmos_Transition(pMotion, pState, Active_Region2_Stopped_State);
    }
  }

  return false;
}

static bool Active_Region2_Moving_Up_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motion * pMotion = (motion *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evDownCommand: {
      return oosmos_Transition(pMotion, pState, Active_Region2_Moving_Down_State);
    }
    case evUpperLimitSwitchOpen: {
      return oosmos_Transition(pMotion, pState, Active_Region2_Stopped_State);
    }
  }

  return false;
}

static bool Active_Region2_Moving_Down_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  motion * pMotion = (motion *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evUpCommand: {
      return oosmos_Transition(pMotion, pState, Active_Region2_Moving_Up_State);
    }
    case evLowerLimitSwitchOpen: {
      return oosmos_Transition(pMotion, pState, Active_Region2_Stopped_State);
    }
  }

  return false;
}
//<<<CODE

static motion * motionNew(void)
{
  oosmos_Allocate(pMotion, motion, 2, NULL);

//>>>INIT
  oosmos_StateMachineInit(pMotion, ROOT, NULL, Active_State);
    oosmos_OrthoInit(pMotion, Active_State, ROOT, NULL);
      oosmos_OrthoRegionInit(pMotion, Active_Region1_State, Active_State, Active_Region1_Limits_State, NULL);
        oosmos_CompositeInit(pMotion, Active_Region1_Limits_State, Active_Region1_State, Active_Region1_Limits_Choice1_State, Active_Region1_Limits_State_Code);
          oosmos_LeafInit(pMotion, Active_Region1_Limits_AtUpperLimit_State, Active_Region1_Limits_State, NULL);
          oosmos_LeafInit(pMotion, Active_Region1_Limits_InBounds_State, Active_Region1_Limits_State, NULL);
          oosmos_ChoiceInit(pMotion, Active_Region1_Limits_Choice1_State, Active_Region1_Limits_State, Active_Region1_Limits_Choice1_State_Code);
          oosmos_LeafInit(pMotion, Active_Region1_Limits_AtLowerLimit_State, Active_Region1_Limits_State, NULL);
      oosmos_OrthoRegionInit(pMotion, Active_Region2_State, Active_State, Active_Region2_Stopped_State, NULL);
        oosmos_LeafInit(pMotion, Active_Region2_Stopped_State, Active_Region2_State, Active_Region2_Stopped_State_Code);
        oosmos_CompositeInit(pMotion, Active_Region2_Moving_State, Active_Region2_State, Active_Region2_Moving_Up_State, Active_Region2_Moving_State_Code);
          oosmos_LeafInit(pMotion, Active_Region2_Moving_Up_State, Active_Region2_Moving_State, Active_Region2_Moving_Up_State_Code);
          oosmos_LeafInit(pMotion, Active_Region2_Moving_Down_State, Active_Region2_Moving_State, Active_Region2_Moving_Down_State_Code);

  oosmos_Debug(pMotion, OOSMOS_EventNames);
//<<<INIT

  return pMotion;
}

static void QueueEvent(motion * pMotion, int EventCode)
{
  oosmos_PushEventCode(pMotion, EventCode);
  oosmos_RunStateMachine(pMotion);
}

static void TestState(const motion * pMotion, const void * pState)
{
  if (!oosmos_IsInState(pMotion, pState)) {
    printf("FAILURE.\n");
    Failures += 1;
  }
}

extern int main(void)
{
  motion * pMotion = motionNew();

  QueueEvent(pMotion, evUpCommand);
  TestState(pMotion, &pMotion->Active_Region2_Moving_Up_State);

  QueueEvent(pMotion, evUpperLimitSwitchOpen);
  TestState(pMotion, &pMotion->Active_Region2_Stopped_State);
  TestState(pMotion, &pMotion->Active_Region1_Limits_AtUpperLimit_State);

  QueueEvent(pMotion, evDownCommand);
  TestState(pMotion, &pMotion->Active_Region2_Moving_Down_State);

  QueueEvent(pMotion, evLowerLimitSwitchClosed);
  TestState(pMotion, &pMotion->Active_Region1_Limits_InBounds_State);

  QueueEvent(pMotion, evUpCommand);
  TestState(pMotion, &pMotion->Active_Region2_Moving_Up_State);

  QueueEvent(pMotion, evStopCommand);
  TestState(pMotion, &pMotion->Active_Region2_Stopped_State);

  if (Failures > 0) {
    printf("%d FAILURES\n", Failures);
  }
  else {
    printf("\n");
    printf("SUCCESS!\n");
  }

  return 0;
}
