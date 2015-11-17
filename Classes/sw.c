/*
// OOSMOS sw Class
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

#ifndef swMaxSwitches
#define swMaxSwitches 20
#endif

#ifndef swMaxCloseSubscribers 
#define swMaxCloseSubscribers 1
#endif

#ifndef swMaxOpenSubscribers 
#define swMaxOpenSubscribers 1
#endif

/*===================================*/

#include "oosmos.h"
#include "pin.h"
#include "sw.h"

typedef enum {
  Unknown_State = 1,
  Open_State,
  Closed_State,
} eStates;

struct swTag
{
  pin      * m_pPin;
  eStates    m_State;
  
  oosmos_sActiveObject   m_ActiveObject;
  oosmos_sSubscriberList m_CloseEvent[swMaxCloseSubscribers];
  oosmos_sSubscriberList m_OpenEvent[swMaxOpenSubscribers];
};

extern sw * swNewDetached(pin * pPin)
{
  oosmos_Allocate(pSwitch, sw, swMaxSwitches, NULL);

  pSwitch->m_pPin  = pPin;
  pSwitch->m_State = Unknown_State;

  oosmos_SubscriberListInit(pSwitch->m_CloseEvent);
  oosmos_SubscriberListInit(pSwitch->m_OpenEvent);

  return pSwitch;
}

extern sw * swNew(pin * pPin)
{
  sw * pSwitch = swNewDetached(pPin);

  oosmos_RegisterActiveObject(pSwitch, swRunStateMachine, &pSwitch->m_ActiveObject);

  return pSwitch;
}

extern void swSubscribeCloseEvent(sw * pSwitch, oosmos_sQueue * pQueue, int CloseEventCode, void * pContext)
{
  oosmos_SubscriberListAdd(pSwitch->m_CloseEvent, pQueue, CloseEventCode, pContext);
}

extern void swSubscribeOpenEvent(sw * pSwitch, oosmos_sQueue * pQueue, int OpenEventCode, void * pContext)
{
  oosmos_SubscriberListAdd(pSwitch->m_OpenEvent, pQueue, OpenEventCode, pContext);
}

extern bool swIsOpen(const sw * pSwitch)
{
  return pSwitch->m_State == Open_State;
}

extern bool swIsClosed(const sw * pSwitch)
{
  return pSwitch->m_State == Closed_State;
}

extern void swRunStateMachine(void * pObject)
{
  sw * pSwitch = (sw *) pObject;
  
  switch (pSwitch->m_State) {
    case Open_State:
      if (pinIsOn(pSwitch->m_pPin)) {
        pSwitch->m_State = Closed_State;     
        oosmos_SubscriberListNotify(pSwitch->m_CloseEvent);
      }
      
      break;
    case Closed_State:
      if (pinIsOff(pSwitch->m_pPin)) {
        pSwitch->m_State = Open_State;
        oosmos_SubscriberListNotify(pSwitch->m_OpenEvent);
      }
  
      break;
    case Unknown_State:
      pSwitch->m_State = pinIsOn(pSwitch->m_pPin) ? Closed_State : Open_State;
      break;
  }
}
