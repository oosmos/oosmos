/*
// OOSMOS encoder Class
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
#include "encoder.h"
#include "pin.h"

#ifndef encoderMAX
#define encoderMAX 3
#endif

typedef enum
{
  EncoderA_Unknown = 1,
  EncoderA_High,
  EncoderA_Low,
} eState;

struct encoderTag
{
  pin  * m_pPinA;
  pin  * m_pPinB;
  int    m_Count;
  int    m_Max;
  eState m_State;
  
  oosmos_sSubscriberList m_ChangeEventSubscribers[1];
  oosmos_sActiveObject   m_ActiveObject;
};

static void ActiveEncoder_Callback(void * pObject)
{
  encoder * pEncoder = (encoder *) pObject;
  
  switch (pEncoder->m_State) {
    case EncoderA_Unknown: {
      pEncoder->m_State = pinIsOn(pEncoder->m_pPinA) ? EncoderA_High : EncoderA_Low;
      break;
    }
    case EncoderA_High: {
      if (pinIsOff(pEncoder->m_pPinA)) {
        pEncoder->m_State = EncoderA_Low;
        
        if (pinIsOn(pEncoder->m_pPinB))
          pEncoder->m_Count = oosmos_Min(pEncoder->m_Max, pEncoder->m_Count + 1);
        else
          pEncoder->m_Count = oosmos_Max(0, pEncoder->m_Count - 1);
        
        oosmos_SubscriberListNotify(pEncoder->m_ChangeEventSubscribers);
      }
      break;
    }
    case EncoderA_Low: {
      if (pinIsOn(pEncoder->m_pPinA))
        pEncoder->m_State = EncoderA_High;
      break;    
    }
  }
}

extern void encoderReset(encoder * pEncoder)
{
  pEncoder->m_Count = 0;
}

extern void encoderSubscribeChangeEvent(encoder * pEncoder, oosmos_sQueue * pQueue, const int EventCode, void * pContext)
{
  oosmos_SubscriberListAdd(pEncoder->m_ChangeEventSubscribers, pQueue, EventCode, pContext);
}

extern int encoderGetCount(encoder * pEncoder)
{
  return pEncoder->m_Count;
}

extern encoder * encoderNew(pin * pPinA, pin * pPinB, int Max) 
{  
  oosmos_Allocate(pEncoder, encoder, encoderMAX, NULL);
    
  pEncoder->m_pPinA  = pPinA;
  pEncoder->m_pPinB  = pPinB; 
  pEncoder->m_Count  = 0;
  pEncoder->m_Max    = Max;
  pEncoder->m_State  = EncoderA_Unknown;
    
  oosmos_RegisterActiveObject(pEncoder, ActiveEncoder_Callback, &pEncoder->m_ActiveObject);
    
  return pEncoder;
}
