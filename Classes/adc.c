//
// OOSMOS adc Class
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

#ifndef adcMAX
#define adcMAX 2
#endif

//===================================

#include "adc.h"
#include "oosmos.h"
#include <stdint.h>
#include <stddef.h>

struct adcTag
{
  #if defined(ARDUINO)
    unsigned m_PinNumber;
  #elif defined(_MSC_VER)
    unsigned m_PinNumber;
  #else
    #error adc.c: Unsupported platform.
  #endif
};

#if defined(ARDUINO)
  extern uint32_t adcRead(const adc * pADC)
  {
    return analogRead(pADC->m_PinNumber);
  }

  extern adc * adcNew(unsigned PinNumber)
  {
    oosmos_Allocate(pADC, adc, adcMAX, NULL);
    pinMode(PinNumber, OUTPUT);
    pADC->m_PinNumber = PinNumber;

    return pADC;
  }
#elif defined(_MSC_VER)
  extern uint32_t adcRead(const adc * pADC)
  {
    oosmos_UNUSED(pADC);
    return 0;
  }

  extern adc * adcNew(unsigned PinNumber)
  {
    oosmos_Allocate(pADC, adc, adcMAX, NULL);
    pADC->m_PinNumber = PinNumber;

    return pADC;
  }
#else
  #error adc.c: Unsupported platform.
#endif
