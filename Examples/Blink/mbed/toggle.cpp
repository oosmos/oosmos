/*
// OOSMOS toggle Class
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
#include "toggle.h"
#include "pin.h"

#ifndef toggleMAX
#define toggleMAX 4
#endif

struct toggleTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              Running_State;

  pin * m_pPin;
  int   m_TimeOnMS;
  int   m_TimeOffMS;
};

static bool Running_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  toggle * pToggle = (toggle *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        while (true) {
          pinOn(pToggle->m_pPin);
          oosmos_SyncDelayMS(pRegion, pToggle->m_TimeOnMS);

          pinOff(pToggle->m_pPin);
          oosmos_SyncDelayMS(pRegion, pToggle->m_TimeOffMS);
        }
      oosmos_SyncEnd(pRegion);
      return true; 
  }

  return false;
}

extern toggle * toggleNew(pin * pPin, const int TimeOnMS, const int TimeOffMS) 
{  
  oosmos_Allocate(pToggle, toggle, toggleMAX, NULL);
 
  /*                                      StateName       Parent          Default        */
  /*                            ======================================================== */
  oosmos_StateMachineInitNoQueue(pToggle, StateMachine,   NULL,           Running_State);
    oosmos_LeafInit             (pToggle, Running_State,  StateMachine                 );  
    
  pToggle->m_pPin      = pPin;
  pToggle->m_TimeOnMS  = TimeOnMS;
  pToggle->m_TimeOffMS = TimeOffMS; 
    
  oosmos_DebugCode(
    oosmos_Debug(&pToggle->StateMachine, true, NULL);
  )
    
  return pToggle;
}
