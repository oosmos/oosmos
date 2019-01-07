//
// OOSMOS matrixtest Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
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

enum
{
  PressedEvent = 1,
  ReleasedEvent,
  SpecialPressedEvent,
  SpecialReleasedEvent,
};

struct matrixtestTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 5);
    oosmos_sLeaf       Idle_State;

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

static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  const char * pString = (const char *) pEvent->m_pContext;

  switch (oosmos_EventCode(pEvent)) {
    case PressedEvent:
      prtFormatted("Pressed %s\n", pString);
      return true;
    case ReleasedEvent:
      prtFormatted("Released %s\n", pString);
      return true;
    case SpecialPressedEvent:
      prtFormatted("Pressed special %s\n", pString);
      return true;
    case SpecialReleasedEvent:
      prtFormatted("Released special %s\n", pString);
      return true;
  }

  return false;
}

extern matrixtest * matrixtestNew(pin * pRow1, pin * pRow2, pin * pRow3, pin * pCol1, pin * pCol2, pin * pCol3)
{
  oosmos_Allocate(pMatrixTest, matrixtest, 1, NULL);

  //                                   StateName     Parent        Default
  //                     =====================================================
  oosmos_StateMachineInit(pMatrixTest, StateMachine, NULL,         Idle_State);
    oosmos_LeafInit      (pMatrixTest, Idle_State,   StateMachine            );

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
