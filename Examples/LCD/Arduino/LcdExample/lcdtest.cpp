/*
// OOSMOS lcdtest Class
//
// Copyright (C) 2014-2015  OOSMOS, LLC
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
*/

#include "oosmos.h"
#include "pin.h"
#include "lcd.h"
#include "lcdtest.h"

#ifndef lcdtstMAX
#define lcdtestMAX 1
#endif

typedef enum {
  DoneEvent = 1,
} eEvents;

struct lcdtestTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 2);
    oosmos_sLeaf       Initializing_State;
    oosmos_sLeaf       Running_State;

  lcd * m_pLCD;
  long  m_Count;
};

static bool Initializing_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  lcdtest * pLcdTest = (lcdtest *) pObject;

  lcd * pLCD = pLcdTest->m_pLCD;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        lcdClear(pLCD);
        oosmos_DelaySeconds(1);

        lcdSetCursor(pLCD, 0, 0);
        lcdPrint(pLCD, "Hello world");
      oosmos_SyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pLcdTest->Running_State);
  }

  return false;
}

static bool Running_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  lcdtest * pLcdTest = (lcdtest *) pObject;

  lcd * pLCD = pLcdTest->m_pLCD;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        for (pLcdTest->m_Count = 1;; pLcdTest->m_Count++) {
          lcdSetCursor(pLcdTest->m_pLCD, 1, 0);         
          lcdPrint(pLcdTest->m_pLCD, "Count: %ld", pLcdTest->m_Count);

          oosmos_SyncDelayMS(pRegion, 300);
        }
      oosmos_SyncEnd(pRegion);

      return true;
  }

  return false;
}

extern lcdtest * lcdtestNew(pin * pRS, pin * pE,
                            pin * pData4, pin * pData5, pin * pData6, pin * pData7)
{
  oosmos_Allocate(pLcdTest, lcdtest, lcdtestMAX, NULL);

  /*                                StateName                  Parent                 Default          */
  /*                      ============================================================================ */
  oosmos_StateMachineInit(pLcdTest, StateMachine,              NULL,                  Initializing_State);
    oosmos_LeafInit      (pLcdTest, Initializing_State,        StateMachine                             );
    oosmos_LeafInit      (pLcdTest, Running_State,             StateMachine                             );

  lcd * pLCD = lcdNew(pRS, NULL, pE, pData4, pData5, pData6, pData7);

  pLcdTest->m_pLCD = pLCD;
  
  return pLcdTest;
}
