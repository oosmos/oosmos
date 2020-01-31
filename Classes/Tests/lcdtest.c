//
// OOSMOS lcdtest Class
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
#include "lcd.h"
#include "lcdtest.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef lcdtstMAX
#define lcdtestMAX 1
#endif

enum {
  DoneEvent = 1,
};

struct lcdtestTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 2);
    oosmos_sLeaf       Initializing_State;
    oosmos_sLeaf       Running_State;

  lcd * m_pLCD;
  long  m_Count;
};

static void InitThread(lcdtest * pLcdTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    lcdClear(pLcdTest->m_pLCD);
    oosmos_DelaySeconds(1);

    lcdSetCursor(pLcdTest->m_pLCD, 0, 0);
    lcdPrint(pLcdTest->m_pLCD, "Hello world");
  oosmos_ThreadEnd();
}

static void RunningThread(lcdtest * pLcdTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (pLcdTest->m_Count = 1;; pLcdTest->m_Count++) {
      lcdSetCursor(pLcdTest->m_pLCD, 1, 0);
      lcdPrint(pLcdTest->m_pLCD, "Count: %ld", pLcdTest->m_Count);

      oosmos_ThreadDelayMS(300);
    }
  oosmos_ThreadEnd();
}

static bool Initializing_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  lcdtest * pLcdTest = (lcdtest *) pObject;


  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      InitThread(pLcdTest, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pLcdTest, pState, Running_State);
    }
  }

  return false;
}

static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  lcdtest * pLcdTest = (lcdtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      RunningThread(pLcdTest, pState);
      return true;
    }
  }

  return false;
}

extern lcdtest * lcdtestNew(pin * pRS, pin * pE,
                            pin * pData4, pin * pData5, pin * pData6, pin * pData7)
{
  oosmos_Allocate(pLcdTest, lcdtest, lcdtestMAX, NULL);

  //                                StateName                  Parent
  //                      ======================================================================
  oosmos_StateMachineInit(pLcdTest, StateMachine,              NULL,         Initializing_State);
    oosmos_LeafInit      (pLcdTest, Initializing_State,        StateMachine, NULL              );
    oosmos_LeafInit      (pLcdTest, Running_State,             StateMachine, NULL              );

  lcd * pLCD = lcdNew(pRS, NULL, pE, pData4, pData5, pData6, pData7);

  pLcdTest->m_pLCD = pLCD;

  return pLcdTest;
}
