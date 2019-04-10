//
// OOSMOS adc Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
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

#ifndef adc_h
#define adc_h

#include <stdint.h>

typedef struct adcTag adc;

extern uint32_t adcRead(const adc * pADC);

#if defined(ARDUINO)
  extern adc * adcNew(int PinNumber);
#elif defined(_MSC_VER)
  extern adc * adcNew(int PinNumber);
#else
  #error adc.h: Unsupported platform.
#endif

#endif
