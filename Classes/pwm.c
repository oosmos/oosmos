//
// OOSMOS pwm Class
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

#ifndef pwmMAX
#define pwmMAX 2
#endif

//===================================

#include "pwm.h"
#include "oosmos.h"
#include <stdint.h>

#include "prt.h"

struct pwmTag
{
  #if defined(ARDUINO)
    int m_PinNumber;
  #elif defined(_MSC_VER)
    int m_PinNumber;
  #else
    #error pwm.c: Unsupported platform.
  #endif

  uint32_t m_DutyCycleValue;
};

#if defined(ARDUINO)
  extern void pwmDutyCyclePercent(pwm * pPWM, uint8_t DutyCyclePercent)
  {
    #ifdef ARDUINO_ESP8266_NODEMCU
      const uint32_t DutyCycleValue = oosmos_AnalogMapAccurate(DutyCyclePercent, 0, 100, 0, 1023);
    #else
      const uint32_t DutyCycleValue = oosmos_AnalogMapAccurate(DutyCyclePercent, 0, 100, 0, 255);
    #endif

    analogWrite(pPWM->m_PinNumber, DutyCycleValue);
    pPWM->m_DutyCycleValue = DutyCycleValue;

    #ifdef pwmDEBUG
      prtFormatted("pwmDutyCycle: pPWM: %p, pinNumber: %d, DutyCyclePercent: %u, DutyCycleValue: %u\n",
                  pPWM, pPWM->m_PinNumber, (unsigned int) DutyCyclePercent, (unsigned int) pPWM->m_DutyCycleValue);
    #endif
  }

  extern void pwmOn(pwm * pPWM)
  {
    analogWrite(pPWM->m_PinNumber, pPWM->m_DutyCycleValue);
  }

  extern void pwmOff(pwm * pPWM)
  {
    analogWrite(pPWM->m_PinNumber, 0);
  }

  extern pwm * pwmNew(int PinNumber)
  {
    oosmos_Allocate(pPWM, pwm, pwmMAX, NULL);
    pPWM->m_PinNumber = PinNumber;

    pinMode(PinNumber, OUTPUT);

    #ifdef pwmDEBUG
      prtFormatted("pwmNew: pPWM: %p, PinNumber: %d\n", pPWM, pPWM->m_PinNumber);
    #endif

    return pPWM;
  }
#elif defined(_MSC_VER)
  extern void pwmDutyCyclePercent(pwm * pPWM, uint8_t DutyCyclePercent)
  {
    pPWM->m_DutyCycleValue = (uint32_t) oosmos_AnalogMapAccurate(DutyCyclePercent, 0, 100, 0, 1023);

    #ifdef pwmDEBUG
      prtFormatted("pwmDutyCycle: pPWM: %p, pinNumber: %d, DutyCyclePercent: %u, DutyCycleValue: %u\n",
                  pPWM, pPWM->m_PinNumber, (unsigned int) DutyCyclePercent, (unsigned int) pPWM->m_DutyCycleValue);
    #endif
  }

  extern void pwmOn(pwm * pPWM)
  {
  }

  extern void pwmOff(pwm * pPWM)
  {
  }

  extern pwm * pwmNew(int PinNumber)
  {
    oosmos_Allocate(pPWM, pwm, pwmMAX, NULL);
    pPWM->m_PinNumber = PinNumber;

    #ifdef pwmDEBUG
      prtFormatted("pwmNew: pPWM: %p, PinNumber: %d\n", pPWM, pPWM->m_PinNumber);
    #endif

    return pPWM;
  }
#else
  #error pwm.c: Unsupported platform.
#endif
