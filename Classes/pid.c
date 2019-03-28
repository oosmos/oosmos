//
// OOSMOS pid Class
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

#include "pid.h"
#include "oosmos.h"
#include <stdint.h>
#include <stdio.h>

struct pidTag {
  float    m_Kp;
  float    m_Ki;
  float    m_Kd;

  float    m_SetPoint;
  float    m_PreviousError;
  float    m_SumOfErrors;

  uint64_t m_PreviousTimeUS;

  bool     m_bFirst;
};

extern void pidSet_SetPoint(pid * pPID, float SetPoint)
{
  pPID->m_SetPoint    = SetPoint;
  pPID->m_SumOfErrors = 0.0f;
}

extern void pidSet_Kp(pid * pPID, float Kp)
{
  pPID->m_Kp = Kp;
}

extern void pidSet_Ki(pid * pPID, float Ki)
{
  pPID->m_Ki = Ki;
}

extern void pidSet_Kd(pid * pPID, float Kd)
{
  pPID->m_Kd = Kd;
}

extern float pidAdjustOutput(pid * pPID, float Input)
{
  const float Error = pPID->m_SetPoint - Input;

  uint64_t NowUS = oosmos_GetFreeRunningMicroseconds();

  //
  // Handle uint32_t wrap around.
  //
  if (NowUS < pPID->m_PreviousTimeUS) {
    NowUS += 0x100000000;
  }

  const float P = Error * pPID->m_Kp;
  float       I;
  float       D;

  uint32_t dtMS;

  if (pPID->m_bFirst) {
    pPID->m_SumOfErrors = Error;
    I = 0.0f;
    D = 0.0f;
    dtMS = 0;
    pPID->m_bFirst = false;

    #ifdef pidDEBUG
      printf("Input,Error,P,I,D,Integral,Output,SetPoint,dtMS\n");
    #endif
  } else {
    const uint32_t tdUS = (uint32_t) (NowUS - pPID->m_PreviousTimeUS);
    dtMS = oosmos_US2MS_Rounded(tdUS);

    pPID->m_SumOfErrors += Error * dtMS;
    I = pPID->m_SumOfErrors * pPID->m_Ki;

    const float Derivative = oosmos_Divide_Integral_Rounded(Error - pPID->m_PreviousError, (float) dtMS);
    D = Derivative * pPID->m_Kd;
  }

  const float Output = P + D + I;

  #ifdef pidDEBUG
    printf("%f,%f,%f,%f,%f,%f,%f,%f,%u\n", (double) Input, (double) Error,
                                           (double) P, (double) I, (double) D,
                                           (double) pPID->m_SumOfErrors, (double) Output,
                                           (double) pPID->m_SetPoint, dtMS);
  #else
    oosmos_UNUSED(dtMS);
  #endif

  pPID->m_PreviousError  = Error;
  pPID->m_PreviousTimeUS = NowUS;

  return Output;
}

extern pid * pidNew(float Kp, float Ki, float Kd, float SetPoint)
{
  oosmos_Allocate(pPID, pid, 1, NULL);

  pPID->m_Kp = Kp;
  pPID->m_Ki = Ki;
  pPID->m_Kd = Kd;

  pPID->m_SetPoint       = SetPoint;
  pPID->m_PreviousError  = 0.0f;
  pPID->m_SumOfErrors    = 0.0f;
  pPID->m_PreviousTimeUS = 0;
  pPID->m_bFirst         = true;

  return pPID;
}
