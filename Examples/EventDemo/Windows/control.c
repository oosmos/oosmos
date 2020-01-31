//
// OOSMOS EventDemo example, control object
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
#include "control.h"
#include "motor.h"
#include "pump.h"
#include "pin.h"
#include "sw.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


//>>>EVENTS
enum {
  evMovePressed = 1,
  evOption1Pressed = 2,
  evOption2Pressed = 3,
  evPumpPressed = 4,
  evQuitPressed = 5,
  evStopPressed = 6,
  evStopReleased = 7
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case evMovePressed: return "evMovePressed";
      case evOption1Pressed: return "evOption1Pressed";
      case evOption2Pressed: return "evOption2Pressed";
      case evPumpPressed: return "evPumpPressed";
      case evQuitPressed: return "evQuitPressed";
      case evStopPressed: return "evStopPressed";
      case evStopReleased: return "evStopReleased";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef union {
  oosmos_sEvent Event;
} uEvents;

struct controlTag
{
  motor * m_pMotor;
  pump  * m_pPump;
  bool    m_Option1;
  bool    m_Option2;

//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sLeaf StartingUp_State;
    oosmos_sOrtho Operational_State;
      oosmos_sOrthoRegion Operational_Region1_State;
        oosmos_sLeaf Operational_Region1_Moving_State;
        oosmos_sLeaf Operational_Region1_Idle_State;
      oosmos_sOrthoRegion Operational_Region2_State;
        oosmos_sLeaf Operational_Region2_Idle_State;
        oosmos_sLeaf Operational_Region2_Pumping_State;
    oosmos_sLeaf StopPressed_State;
    oosmos_sLeaf Terminated_State;
//<<<DECL
};

static void ToggleOption1(control * pControl)
{
  pControl->m_Option1 = !(pControl->m_Option1);
  oosmos_DebugPrint("Option1: %d\n", pControl->m_Option1);
}

static void ToggleOption2(control * pControl)
{
  pControl->m_Option2 = !(pControl->m_Option2);
  oosmos_DebugPrint("Option2: %d\n", pControl->m_Option1);
}

//>>>CODE
static bool StartingUp_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      return oosmos_StateTimeoutSeconds(pState, (uint32_t) 1);
    }
    case oosmos_TIMEOUT: {
      return oosmos_Transition(pControl, pState, Operational_State);
    }
  }

  return false;
}

static bool Operational_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evStopPressed: {
      return oosmos_Transition(pControl, pState, StopPressed_State);
    }
    case evQuitPressed: {
      return oosmos_Transition(pControl, pState, Terminated_State);
    }
  }

  return false;
}

static bool StopPressed_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evOption1Pressed: {
      ToggleOption1(pControl);
      return true;
    }
    case evOption2Pressed: {
      ToggleOption2(pControl);
      return true;
    }
    case evStopReleased: {
      return oosmos_Transition(pControl, pState, Operational_State);
    }
  }

  return false;
}

static bool Operational_Region1_Moving_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      motorOn(pControl->m_pMotor);
      return true;
    }
    case oosmos_EXIT: {
      motorOff(pControl->m_pMotor);
      return true;
    }
    case evMovePressed: {
      return oosmos_Transition(pControl, pState, Operational_Region1_Idle_State);
    }
  }

  return false;
}

static bool Operational_Region1_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evMovePressed: {
      return oosmos_Transition(pControl, pState, Operational_Region1_Moving_State);
    }
  }

  return false;
}

static bool Terminated_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      exit(1);
    }
  }

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  return false;
}

static bool Operational_Region2_Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case evPumpPressed: {
      return oosmos_Transition(pControl, pState, Operational_Region2_Pumping_State);
    }
  }

  return false;
}

static bool Operational_Region2_Pumping_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  control * pControl = (control *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER: {
      pumpOn(pControl->m_pPump);
      return true;
    }
    case oosmos_EXIT: {
      pumpOff(pControl->m_pPump);
      return true;
    }
    case evPumpPressed: {
      return oosmos_Transition(pControl, pState, Operational_Region2_Idle_State);
    }
  }

  return false;
}
//<<<CODE

extern control * controlNew(void)
{
  oosmos_Allocate(pControl, control, 1, NULL);

  oosmos_sQueue * const pControlEventQueue = &pControl->m_EventQueue;

  pin * pStopPin   = pinNew('s', pinActiveHigh);
  sw * pStopSwitch = swNew(pStopPin);
  swSubscribeCloseEvent(pStopSwitch,    pControlEventQueue, evStopPressed, NULL);
  swSubscribeOpenEvent(pStopSwitch,     pControlEventQueue, evStopReleased, NULL);

  pin * pMovePin   = pinNew('m', pinActiveHigh);
  sw * pMoveSwitch = swNew(pMovePin);
  swSubscribeCloseEvent(pMoveSwitch,    pControlEventQueue, evMovePressed, NULL);

  pin * pQuitPin   = pinNew('q', pinActiveHigh);
  sw * pQuitSwitch = swNew(pQuitPin);
  swSubscribeCloseEvent(pQuitSwitch,    pControlEventQueue, evQuitPressed, NULL);

  pin * pPumpPin   = pinNew('p', pinActiveHigh);
  sw * pPumpSwitch = swNew(pPumpPin);
  swSubscribeCloseEvent(pPumpSwitch,    pControlEventQueue, evPumpPressed, NULL);

  pin * pOption1Pin   = pinNew('1', pinActiveHigh);
  sw * pOption1Switch = swNew(pOption1Pin);
  swSubscribeCloseEvent(pOption1Switch, pControlEventQueue, evOption1Pressed, NULL);

  pin * pOption2Pin   = pinNew('2', pinActiveHigh);
  sw * pOption2Switch = swNew(pOption2Pin);
  swSubscribeCloseEvent(pOption2Switch, pControlEventQueue, evOption2Pressed, NULL);

  pin * pUpPin   = pinNew('u', pinActiveHigh);
  sw * pUpSwitch = swNew(pUpPin);

  pin * pDownPin   = pinNew('d', pinActiveHigh);
  sw * pDownSwitch = swNew(pDownPin);

  pControl->m_pMotor  = motorNew();
  pControl->m_pPump   = pumpNew(pUpSwitch, pDownSwitch);
  pControl->m_Option1 = false;
  pControl->m_Option2 = false;

//>>>INIT
  oosmos_StateMachineInit(pControl, ROOT, NULL, StartingUp_State);
    oosmos_LeafInit(pControl, StartingUp_State, ROOT, StartingUp_State_Code);
    oosmos_OrthoInit(pControl, Operational_State, ROOT, Operational_State_Code);
      oosmos_OrthoRegionInit(pControl, Operational_Region1_State, Operational_State, Operational_Region1_Idle_State, NULL);
        oosmos_LeafInit(pControl, Operational_Region1_Moving_State, Operational_Region1_State, Operational_Region1_Moving_State_Code);
        oosmos_LeafInit(pControl, Operational_Region1_Idle_State, Operational_Region1_State, Operational_Region1_Idle_State_Code);
      oosmos_OrthoRegionInit(pControl, Operational_Region2_State, Operational_State, Operational_Region2_Idle_State, NULL);
        oosmos_LeafInit(pControl, Operational_Region2_Idle_State, Operational_Region2_State, Operational_Region2_Idle_State_Code);
        oosmos_LeafInit(pControl, Operational_Region2_Pumping_State, Operational_Region2_State, Operational_Region2_Pumping_State_Code);
    oosmos_LeafInit(pControl, StopPressed_State, ROOT, StopPressed_State_Code);
    oosmos_LeafInit(pControl, Terminated_State, ROOT, Terminated_State_Code);

  oosmos_Debug(pControl, OOSMOS_EventNames);
//<<<INIT

  return pControl;
}
