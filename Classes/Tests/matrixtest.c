//
// OOSMOS matrixtest Class
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
#include "pin.h"
#include "sw.h"
#include "prt.h"
#include "matrix.h"
#include "matrixtest.h"
#include <stdbool.h>
#include <stddef.h>

//>>>EVENTS
enum {
  PressedEvent = 1,
  ReleasedEvent = 2,
  SpecialPressedEvent = 3,
  SpecialReleasedEvent = 4
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case PressedEvent: return "PressedEvent";
      case ReleasedEvent: return "ReleasedEvent";
      case SpecialPressedEvent: return "SpecialPressedEvent";
      case SpecialReleasedEvent: return "SpecialReleasedEvent";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef union {
  oosmos_sEvent Base;
} uEvents;

struct matrixtestTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sLeaf State_State;
//<<<DECL

  matrix * m_pMatrix;
};

static void NewSwitch(matrixtest * pMatrixTest, int Row, int Column, int PressedEventCode, int ReleasedEventCode, const char * pString)
{
  matrix * pMatrix = pMatrixTest->m_pMatrix;

  pin * pColumnPin = matrixGetColumnPin(pMatrix, Column);
  sw  * pSwitch    = swNewDetached(pColumnPin);
  matrixAssignSwitch(pMatrix, pSwitch, Row, Column);

  swSubscribeCloseEvent(pSwitch, oosmos_EventQueue(pMatrixTest), PressedEventCode, (void *) pString);
  swSubscribeOpenEvent(pSwitch, oosmos_EventQueue(pMatrixTest), ReleasedEventCode, (void *) pString);
}

static void Print(const char * pMessage, const oosmos_sEvent * pEvent)
{
  const char * pString = (const char *) pEvent->m_pContext;
  prtFormatted("%s %s\n", pMessage, pString);
}

//>>>CODE
static bool State_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case PressedEvent: {
      Print("Pressed", pEvent);
      return true;
    }
    case ReleasedEvent: {
      Print("Released", pEvent);
      return true;
    }
    case SpecialPressedEvent: {
      Print("Pressed special", pEvent);
      return true;
    }
    case SpecialReleasedEvent: {
      Print("Released special", pEvent);
      return true;
    }
  }

  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE

extern matrixtest * matrixtestNew(pin * pRow1, pin * pRow2, pin * pRow3, pin * pCol1, pin * pCol2, pin * pCol3)
{
  oosmos_Allocate(pMatrixTest, matrixtest, 1, NULL);

//>>>INIT
  oosmos_StateMachineInit(pMatrixTest, ROOT, NULL, State_State);
    oosmos_LeafInit(pMatrixTest, State_State, ROOT, State_State_Code);

  oosmos_Debug(pMatrixTest, OOSMOS_EventNames);
//<<<INIT

  pMatrixTest -> m_pMatrix = matrixNew(3, 3, pRow1, pRow2, pRow3, pCol1, pCol2, pCol3);

  NewSwitch(pMatrixTest, 1, 1, PressedEvent, ReleasedEvent, "1");
  NewSwitch(pMatrixTest, 1, 2, PressedEvent, ReleasedEvent, "2");
  NewSwitch(pMatrixTest, 1, 3, PressedEvent, ReleasedEvent, "3");

  NewSwitch(pMatrixTest, 2, 1, SpecialPressedEvent, SpecialReleasedEvent, "4");
  NewSwitch(pMatrixTest, 2, 2, PressedEvent, ReleasedEvent, "5");
  NewSwitch(pMatrixTest, 2, 3, PressedEvent, ReleasedEvent, "6");

  NewSwitch(pMatrixTest, 3, 1, PressedEvent, ReleasedEvent, "7");
  NewSwitch(pMatrixTest, 3, 2, PressedEvent, ReleasedEvent, "8");
  NewSwitch(pMatrixTest, 3, 3, PressedEvent, ReleasedEvent, "9");

  return pMatrixTest;
}
