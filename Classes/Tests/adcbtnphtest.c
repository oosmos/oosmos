//
// adcbtnphtest Class
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
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
#include "adcbtnph.h"
#include "prt.h"
#include "adcbtnphtest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
 	
#ifndef MAX_ADCBTNPHTESTS
#define MAX_ADCBTNPHTESTS 1
#endif
 	
//>>>EVENTS
enum {
  evPressed = 1
};
 	
#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case evPressed: return "evPressed";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef union {
  oosmos_sEvent Event;
  sAdcButtonEvent ButtonEvent;
} uEvents;

 	
struct adcbtnphtestTag
{
//>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 3);
    oosmos_sLeaf Idle_State;
//<<<DECL
};
 	
//>>>CODE
static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  switch (oosmos_EventCode(pEvent)) {
    case evPressed: {
      prtFormatted("Pressed Code %d Context %d Value %d\n", pEvent->m_Code, pEvent->m_pContext, ((sAdcButtonEvent *) pEvent)->Value);
      return true;
    
   }
  }
 	
  oosmos_UNUSED(pObject);
  oosmos_UNUSED(pState);
  return false;
}
//<<<CODE
 	
extern adcbtnphtest * adcbtnphtestNew(adc * pAdc, uint32_t HoldTimeMS)
{
  oosmos_Allocate(pAdcBtnPhTest, adcbtnphtest, MAX_ADCBTNPHTESTS, NULL);
 	
//>>>INIT
  oosmos_StateMachineInit(pAdcBtnPhTest, ROOT, NULL, Idle_State);
    oosmos_LeafInit(pAdcBtnPhTest, Idle_State, ROOT, Idle_State_Code);
 	
  oosmos_Debug(pAdcBtnPhTest, OOSMOS_EventNames);
//<<<INIT
 	
  adcbtnph * pAdcButton = adcbtnphNew(pAdc, HoldTimeMS);

	// Subscribe to event
  adcbtnphSubscribeButtonPress(pAdcButton, oosmos_EventQueue(pAdcBtnPhTest), evPressed, NULL);
 	
  return pAdcBtnPhTest;
}