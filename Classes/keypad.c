//
// OOSMOS keypad Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
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
#include "sw.h"
#include "matrix.h"
#include "keypad.h"
#include "prt.h"
#include <stdbool.h>
#include <stddef.h>

enum
{
  PressedEvent = 1,
  ReleasedEvent,
  FourPressedEvent,
  FourReleasedEvent
};

struct keypadTag
{
  oosmos_sStateMachine(ROOT, oosmos_sEvent, 5);
    oosmos_sLeaf       Idle_State;
};

static sw * NewSwitch(matrix * pMatrix, int Row, int Column)
{
  pin * pColumnPin = matrixGetColumnPin(pMatrix, Column);
  sw  * pSwitch    = swNewDetached(pColumnPin);
  matrixAssignSwitch(pMatrix, pSwitch, Row, Column);

  return pSwitch;
}

static void NewSwitchWithContext(keypad * pKeypad, matrix * pMatrix, int Row, int Column, const char * pString)
{
  sw * pSwitch = NewSwitch(pMatrix, Row, Column);

  swSubscribeCloseEvent(pSwitch, oosmos_EventQueue(pKeypad), PressedEvent, (void *) pString);
  swSubscribeOpenEvent(pSwitch, oosmos_EventQueue(pKeypad), ReleasedEvent, (void *) pString);
}

static void NewSwitchWithCodes(keypad * pKeypad, matrix * pMatrix, int Row, int Column, int PressedEventCode, int ReleasedEventCode)
{
  sw * pSwitch = NewSwitch(pMatrix, Row, Column);

  swSubscribeCloseEvent(pSwitch, oosmos_EventQueue(pKeypad), PressedEventCode, NULL);
  swSubscribeOpenEvent(pSwitch, oosmos_EventQueue(pKeypad), ReleasedEventCode, NULL);
}

static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case PressedEvent: {
      const char * pString = (const char *) pEvent->m_pContext;
      prtFormatted("Pressed %s\n", pString);
      return true;
    }
    case ReleasedEvent: {
      const char * pString = (const char *) pEvent->m_pContext;
      prtFormatted("Released %s\n", pString);
      return true;
    }
    case FourPressedEvent: {
      prtFormatted("Pressed Four\n");
      return true;
    }
    case FourReleasedEvent: {
      prtFormatted("Released Four\n");
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);

  return false;
}

extern keypad * keypadNew(matrix * pMatrix)
{
  oosmos_Allocate(pKeypad, keypad, 1, NULL);

  //                               StateName     Parent
  //                     ================================================
  oosmos_StateMachineInit(pKeypad, ROOT,         NULL,        Idle_State);
    oosmos_LeafInit      (pKeypad, Idle_State,   ROOT,        Idle_State_Code);

  NewSwitchWithContext(pKeypad, pMatrix, 1, 1, "1");
  NewSwitchWithContext(pKeypad, pMatrix, 1, 2, "2");
  NewSwitchWithContext(pKeypad, pMatrix, 1, 3, "3");

  NewSwitchWithCodes  (pKeypad, pMatrix, 2, 1, FourPressedEvent, FourReleasedEvent);
  NewSwitchWithContext(pKeypad, pMatrix, 2, 2, "5");
  NewSwitchWithContext(pKeypad, pMatrix, 2, 3, "6");

  NewSwitchWithContext(pKeypad, pMatrix, 3, 1, "7");
  NewSwitchWithContext(pKeypad, pMatrix, 3, 2, "8");
  NewSwitchWithContext(pKeypad, pMatrix, 3, 3, "9");

  return pKeypad;
}
