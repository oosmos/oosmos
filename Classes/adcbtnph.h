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

#ifndef adcbtnh_h
#define adcbtnh_h

#include "oosmos.h"
#include "adc.h"
#include <stdint.h>

typedef struct adcbtnphTag adcbtnph;

typedef struct {
  oosmos_sEvent Event;
  unsigned      Value;
} sAdcButtonEvent;


#if defined(ARDUINO)
  extern adcbtnph *adcbtnphNew(adc * pADC, uint32_t HoldTimeMS);

  extern void adcbtnphSubscribeButtonPress(adcbtnph *pAdcButton, oosmos_sQueue *pQueue, int PressedEventCode, void *pContext);
#else
#error adcbtnph.h: Unsupported platform.
#endif

#endif
