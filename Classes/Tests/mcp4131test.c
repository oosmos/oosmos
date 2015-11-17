/*
// OOSMOS mcp4131test Class
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

#include "mcp4131test.h"
#include "mcp4131.h"
#include "oosmos.h"
#include "pin.h"
#include "spi.h"

#ifndef mcp4131testMAX
#define mcp4131testMAX 2
#endif

struct mcp4131testTag {
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              RampingUp_State;
    oosmos_sLeaf              RampingDown_State;

  int       m_RampDelayTimeMS;
  int       m_Resistance;
  mcp4131 * m_pMCP4131;
};

static bool RampingUp_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  mcp4131test * pMCP4131test = (mcp4131test *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      pMCP4131test->m_Resistance = 127;
      return oosmos_StateTimeoutMS(pRegion, pMCP4131test->m_RampDelayTimeMS);
    case oosmos_TIMEOUT:
      mcp4131SetResistance(pMCP4131test->m_pMCP4131, pMCP4131test->m_Resistance--);

      if (pMCP4131test->m_Resistance == 0)
        return oosmos_Transition(pRegion, &pMCP4131test->RampingDown_State);

      return oosmos_StateTimeoutMS(pRegion, pMCP4131test->m_RampDelayTimeMS);
  }

  return false;
}

static bool RampingDown_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  mcp4131test * pMCP4131test = (mcp4131test *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      pMCP4131test->m_Resistance = 0;
      return oosmos_StateTimeoutMS(pRegion, pMCP4131test->m_RampDelayTimeMS);
    case oosmos_TIMEOUT:
      mcp4131SetResistance(pMCP4131test->m_pMCP4131, pMCP4131test->m_Resistance++);

      if (pMCP4131test->m_Resistance == 127)
        return oosmos_Transition(pRegion, &pMCP4131test->RampingUp_State);
      
      return oosmos_StateTimeoutMS(pRegion, pMCP4131test->m_RampDelayTimeMS);
  }

  return false;
}

extern mcp4131test * mcp4131testNew(spi * pSPI, pin * pCS, const int RampDelayTimeMS)
{
  oosmos_Allocate(pMCP4131test, mcp4131test, mcp4131testMAX, NULL);

  /*                                           StateName          Parent         Default          */
  /*                            ================================================================= */
  oosmos_StateMachineInitNoQueue(pMCP4131test, StateMachine,      NULL,          RampingUp_State);
    oosmos_LeafInit             (pMCP4131test, RampingUp_State,   StateMachine                  );
    oosmos_LeafInit             (pMCP4131test, RampingDown_State, StateMachine                  );

  spi_sSlave * pSlave = spiNewSlave(pSPI, pCS);

  pMCP4131test->m_RampDelayTimeMS = RampDelayTimeMS;
  pMCP4131test->m_pMCP4131        = mcp4131New(pSlave);

  return pMCP4131test;
}
