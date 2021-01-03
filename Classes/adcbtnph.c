//
//  adcbtnph Class
//
// 1 ADC pin with 2 Buttons PullUp R 10k, R 1k
//
// used for ONE event with FOUR different values
//
// no button pressed       ADC value ~1023                           no event
// button 1 short press    ADC value 0                               event "pressed" value 1 
// button 2 short press    ADC value ~91 (1023 * 1k / (10k + 1k))    event "pressed" value 2          
// button 1 long press     ADC value 0                               event "pressed" value 3 
// button 2 long press     ADC value ~91 (1023 * 1k / (10k + 1k))    event "pressed" value 4 
//
//
// typedef struct {
//   oosmos_sEvent Event;
//   unsigned      Value;
// } sAdcButtonEvent;
//
// Event SubscriberListNotifyWithArgs  event and value
//
//                VCC
//                |
//                10k
//          /     |
//   +-----/ -----+---- ADC pin
//   |   button1  |
//   |            1k
//   |      /     |
//   +-----/ -----+
//   |   button2
//   |
//   |
//   GND
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef adcbtnphMaxButtons
#define adcbtnphMaxButtons 1
#endif

#ifndef adcbtnphMaxPressedSubscribers
#define adcbtnphMaxPressedSubscribers 1
#endif

//===================================

#include "adcbtnph.h"
#include "oosmos.h"
#include <stdint.h>
#include <stddef.h>

struct adcbtnphTag
{
#if defined(ARDUINO)
  adc *m_pAdc;
  uint32_t m_HoldTimeMS;
  uint32_t m_AdcVal;

  oosmos_sObjectThread m_ObjectThread;
  oosmos_sSubscriberList m_PressedEvent[adcbtnphMaxPressedSubscribers];
#else
#error adcbtnph.c: Unsupported platform.
#endif
};

#if defined(ARDUINO)

static void Thread(adcbtnph *pAdcButton, oosmos_sState *pState)
{
  bool TimedOut = false;

  oosmos_POINTER_GUARD(pAdcButton);
  oosmos_POINTER_GUARD(pState);

  oosmos_ThreadBegin();
  for (;;)
  {
    oosmos_ThreadWaitCond(adcRead(pAdcButton->m_pAdc) < 1000);
    pAdcButton->m_AdcVal = adcRead(pAdcButton->m_pAdc);
    oosmos_ThreadWaitCond_TimeoutMS(adcRead(pAdcButton->m_pAdc) > 1000, pAdcButton->m_HoldTimeMS, &TimedOut);

    sAdcButtonEvent sEvent;
    sEvent.Event = oosmos_EMPTY_EVENT;

    if (TimedOut)
    {
      if (pAdcButton->m_AdcVal == 0)
      {
        sEvent.Value = 3;
        oosmos_SubscriberListNotifyWithArgs(pAdcButton->m_PressedEvent, sEvent);
      }
      if ((pAdcButton->m_AdcVal >= 70) && (pAdcButton->m_AdcVal <= 110))
      {
        sEvent.Value = 4;
        oosmos_SubscriberListNotifyWithArgs(pAdcButton->m_PressedEvent, sEvent);
      }

      oosmos_ThreadWaitCond(adcRead(pAdcButton->m_pAdc) > 1000);
    }
    else
    {
      if (pAdcButton->m_AdcVal == 0)
      {
        sEvent.Value = 1;
        oosmos_SubscriberListNotifyWithArgs(pAdcButton->m_PressedEvent, sEvent);
      }
      if ((pAdcButton->m_AdcVal >= 70) && (pAdcButton->m_AdcVal <= 110))
      {
        sEvent.Value = 2;
        oosmos_SubscriberListNotifyWithArgs(pAdcButton->m_PressedEvent, sEvent);
      }
    }
  }
  oosmos_ThreadEnd();
}

extern void adcbtnphSubscribeButtonPress(adcbtnph *pAdcButton, oosmos_sQueue *pQueue, int PressedEventCode, void *pContext)
{
  oosmos_POINTER_GUARD(pAdcButton);

  oosmos_SubscriberListAdd(pAdcButton->m_PressedEvent, pQueue, PressedEventCode, pContext);
}

extern adcbtnph *adcbtnphNew(adc *pAdc, uint32_t HoldTimeMS)
{
  oosmos_Allocate(pAdcButton, adcbtnph, adcbtnphMaxButtons, NULL);

  pAdcButton->m_pAdc = pAdc;
  pAdcButton->m_HoldTimeMS = HoldTimeMS;

  oosmos_ObjectThreadInit(pAdcButton, m_ObjectThread, Thread, true);

  oosmos_SubscriberListInit(pAdcButton->m_PressedEvent);

  return pAdcButton;
}
#else
#error adcbtnph.c: Unsupported platform.
#endif
