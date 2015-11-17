/*
// OOSMOS pwm Class
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
#include "pwm.h"

struct pwmTag
{
  timer     * m_pTimer;
  timerCalc   m_TimerCalc;
  pin       * m_pPwmPin;
  uint16_t    m_DutyCycleTicks;
  uint16_t    m_OcTimerSource;
  uint16_t    m_PwmNumber;
};

#ifndef pwmMAX
#define pwmMAX 5
#endif

extern pwm * pwmNew(const int PwmNumber, pin * pPwmPin, timer * pTimer)
{
  oosmos_Allocate(pPWM, pwm, pwmMAX, NULL);

  pPWM->m_PwmNumber = PwmNumber;
  pPWM->m_pTimer    = pTimer;
  pPWM->m_pPwmPin   = pPwmPin;

  switch (timerGetTimerNumber(pTimer)) {
    case 2:
      pPWM->m_OcTimerSource = OC_TIMER2_SRC;
      break;
    case 3:
      pPWM->m_OcTimerSource = OC_TIMER3_SRC;
      break;
    default:
      while (true);
  }

  return pPWM;
}

extern void pwmDutyCycle(pwm * pPWM, const float FrequencyKHz, const float DutyCyclePercent)
{
  const float PeriodInUS = 1000.0f / FrequencyKHz;
  timerCalcPeriodUS(pPWM->m_pTimer, PeriodInUS, &pPWM->m_TimerCalc);
  timerStart(pPWM->m_pTimer, &pPWM->m_TimerCalc);

  pPWM->m_DutyCycleTicks = pPWM->m_TimerCalc.Ticks * (DutyCyclePercent / 100.0f);
}

extern void pwmStart(pwm * pPWM)
{
  const uint16_t OcTimerSource  = pPWM->m_OcTimerSource;
  const uint16_t DutyCycleTicks = pPWM->m_DutyCycleTicks;

  switch (pPWM->m_PwmNumber) {
    #ifdef _OCMP1
      case 1: OpenOC1(OC_ON | OcTimerSource | OC_PWM_FAULT_PIN_DISABLE, DutyCycleTicks, 0); break;
    #endif

    #ifdef _OCMP2
      case 2: OpenOC2(OC_ON | OcTimerSource | OC_PWM_FAULT_PIN_DISABLE, DutyCycleTicks, 0); break;
    #endif

    #ifdef _OCMP3
      case 3: OpenOC3(OC_ON | OcTimerSource | OC_PWM_FAULT_PIN_DISABLE, DutyCycleTicks, 0); break;
    #endif

    #ifdef _OCMP4
      case 4: OpenOC4(OC_ON | OcTimerSource | OC_PWM_FAULT_PIN_DISABLE, DutyCycleTicks, 0); break;
    #endif

    #ifdef _OCMP5
      case 5: OpenOC5(OC_ON | OcTimerSource | OC_PWM_FAULT_PIN_DISABLE, DutyCycleTicks, 0); break;
    #endif

    default: while (true);
  }
}

extern void pwmStop(pwm * pPWM)
{
  switch (pPWM->m_PwmNumber) {
    #ifdef _OCMP1
      case 1: CloseOC1(); break;
    #endif

    #ifdef _OCMP2
      case 2: CloseOC2(); break;
    #endif

    #ifdef _OCMP3
      case 3: CloseOC3(); break;
    #endif

    #ifdef _OCMP4
      case 4: CloseOC4(); break;
    #endif

    #ifdef _OCMP5
      case 5: CloseOC5(); break;
    #endif

    default: while (true);
  }
}
