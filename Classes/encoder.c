//
// OOSMOS encoder Class
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


//
// reliable decoding of noisy encoders 
// 
// https://www.best-microcontroller-projects.com/rotary-encoder.html#Taming_Noisy_Rotary_Encoders
// Copyright John Main - best-microcontroller-projects.com
//

#include "oosmos.h"
#include "encoder.h"
#include "pin.h"
#include <stddef.h>
#include <stdbool.h>

#ifndef encoderMAX
#define encoderMAX 2
#endif

#ifndef encoderMAX_CHANGE_SUBSCRIBERS
#define endoderMAX_CHANGE_SUBSCRIBERS 2
#endif

struct encoderTag
{
  pin   * m_pPinA;
  pin   * m_pPinB;
  int32_t m_Count;

  int8_t m_PrevNextCode;
  int16_t m_Store;

  oosmos_sSubscriberList m_ChangeEventSubscribers[endoderMAX_CHANGE_SUBSCRIBERS];
  oosmos_sObjectThread   m_ObjectThread;
};

static const int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};


static void EncoderThread(encoder * pEncoder, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      pEncoder->m_PrevNextCode <<= 2;
      if (pinIsOn(pEncoder->m_pPinA)) pEncoder->m_PrevNextCode |= 0x02;
      if (pinIsOn(pEncoder->m_pPinB)) pEncoder->m_PrevNextCode |= 0x01;
      pEncoder->m_PrevNextCode &= 0x0f;
      if (rot_enc_table[pEncoder->m_PrevNextCode]){
        pEncoder->m_Store <<= 4;
        pEncoder->m_Store |= pEncoder->m_PrevNextCode;

        // for high quality ones: if (pEncoder->m_Store == 0xd42b) 
        if ((pEncoder->m_Store & 0xff) == 0x2b)
        {
          pEncoder->m_Count -= 1;
          oosmos_SubscriberListNotify(pEncoder->m_ChangeEventSubscribers);
        }

        // for high_quality ones: if (pEncoder->m_Store == 0xe817) 
        if ((pEncoder->m_Store & 0xff) == 0x17)
        {
          pEncoder->m_Count += 1;
          oosmos_SubscriberListNotify(pEncoder->m_ChangeEventSubscribers);
        }
      }

      oosmos_ThreadYield();
    }
  oosmos_ThreadEnd();
}

extern void encoderReset(encoder * pEncoder)
{
  pEncoder->m_Count = 0;
}

extern void encoderSubscribeChangeEvent(encoder * pEncoder, oosmos_sQueue * pQueue, const int EventCode, void * pContext)
{
  oosmos_SubscriberListAdd(pEncoder->m_ChangeEventSubscribers, pQueue, EventCode, pContext);
}

extern int32_t encoderGetCount(const encoder * pEncoder)
{
  return pEncoder->m_Count;
}

extern encoder * encoderNew(pin * pPinA, pin * pPinB)
{
  oosmos_Allocate(pEncoder, encoder, encoderMAX, NULL);

  pEncoder->m_pPinA  = pPinA;
  pEncoder->m_pPinB  = pPinB;
  pEncoder->m_Count  = 0;

  pEncoder->m_PrevNextCode = 0;
  pEncoder->m_Store = 0;

  oosmos_ObjectThreadInit(pEncoder, m_ObjectThread, EncoderThread, true);

  return pEncoder;
}
