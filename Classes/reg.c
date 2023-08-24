//
// OOSMOS reg Class
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

#include "reg.h"
#include "oosmos.h"
#include <stdint.h>

struct regTag
{
  float m_Intercept;
  float m_Slope;
};

static float MeanOfX(const regSample * pSamples, uint32_t Samples)
{
  float Sum = 0.0F;

  for (uint32_t SampleIndex = 0; SampleIndex < Samples; SampleIndex++) {
    Sum += pSamples[SampleIndex].X;
  }

  return Sum / Samples;
}

static float MeanOfY(const regSample * pSamples, uint32_t Samples)
{
  float Sum = 0.0F;

  for (uint32_t SampleIndex = 0; SampleIndex < Samples; SampleIndex++) {
    Sum += pSamples[SampleIndex].Y;
  }

  return Sum / Samples;
}

extern reg * regNew(void)
{
  static reg Reg[1];
  reg * pReg = &Reg[0];

  return pReg;
}

extern void regSamples(reg * pReg, const regSample * pSamples, uint32_t Samples)
{
  const float MeanX = MeanOfX(pSamples, Samples);
  const float MeanY = MeanOfY(pSamples, Samples);

  float SumXY = 0.0F;
  float SumXX = 0.0F;

  for (uint32_t SampleIndex = 0; SampleIndex < Samples; SampleIndex++) {
    const regSample * pSample = &pSamples[SampleIndex];

    const float XiMinusMeanX = pSample->X - MeanX;
    const float YiMinusMeanY = pSample->Y - MeanY;

    SumXY += XiMinusMeanX * YiMinusMeanY;
    SumXX += XiMinusMeanX * XiMinusMeanX;
  }

  oosmos_ASSERT(SumXX > 0.0F);

  pReg->m_Slope     = SumXY / SumXX;
  pReg->m_Intercept = MeanY - pReg->m_Slope * MeanX;
}

extern float regPredictY(const reg * pReg, float X)
{
  return pReg->m_Slope * X + pReg->m_Intercept;
}

extern float regSlope(const reg * pReg)
{
	return pReg->m_Slope;
}
