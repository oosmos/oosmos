//
// OOSMOS reg Class - Linear regression
//
// Copyright (C) 2014-2024  OOSMOS, LLC
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
#include <string.h>

#ifndef regMax
#define regMax 1
#endif

struct regTag 
{
    double     m_Intercept;
    double     m_Slope;

    regSample* m_SampleFIFO;
    size_t     m_SampleCount;
    size_t     m_MaxSamples;
    size_t     m_StartIndex;
};

extern reg* regNew(regSample* sampleBuffer, size_t Samples) 
{
    oosmos_Allocate(pReg, reg, regMax, NULL);

    pReg->m_SampleFIFO = sampleBuffer;
    pReg->m_MaxSamples = Samples;
    pReg->m_SampleCount = 0;
    pReg->m_StartIndex = 0;
    return pReg;
}

extern void regPushSample(reg* pReg, const double X, const double Y) 
{
    const size_t InsertIndex = (pReg->m_StartIndex + pReg->m_SampleCount) % pReg->m_MaxSamples;

    pReg->m_SampleFIFO[InsertIndex].X = X;
    pReg->m_SampleFIFO[InsertIndex].Y = Y;

    if (pReg->m_SampleCount < pReg->m_MaxSamples) {
        pReg->m_SampleCount++;
    }
    else {
        pReg->m_StartIndex = (pReg->m_StartIndex + 1) % pReg->m_MaxSamples;
    }
}

static double MeanOfX(const reg* pReg) 
{
    double sum = 0.0;

    for (size_t i = 0; i < pReg->m_SampleCount; i++) {
        size_t index = (pReg->m_StartIndex + i) % pReg->m_MaxSamples;
        sum += pReg->m_SampleFIFO[index].X;
    }

    return sum / pReg->m_SampleCount;
}

static double MeanOfY(const reg* pReg) 
{
    double sum = 0.0;

    for (size_t i = 0; i < pReg->m_SampleCount; i++) {
        size_t index = (pReg->m_StartIndex + i) % pReg->m_MaxSamples;
        sum += pReg->m_SampleFIFO[index].Y;
    }

    return sum / pReg->m_SampleCount;
}

extern void regCalculateRegression(reg* pReg) 
{
    const double meanX = MeanOfX(pReg);
    const double meanY = MeanOfY(pReg);

    double sumXY = 0.0;
    double sumXX = 0.0;

    for (size_t i = 0; i < pReg->m_SampleCount; i++) {
        size_t index = (pReg->m_StartIndex + i) % pReg->m_MaxSamples;
        const regSample* pSample = &pReg->m_SampleFIFO[index];

        const double xiMinusMeanX = pSample->X - meanX;
        const double yiMinusMeanY = pSample->Y - meanY;

        sumXY += xiMinusMeanX * yiMinusMeanY;
        sumXX += xiMinusMeanX * xiMinusMeanX;
    }

    oosmos_ASSERT(sumXX > 0.0);

    pReg->m_Slope     = sumXY / sumXX;
    pReg->m_Intercept = meanY - pReg->m_Slope * meanX;
}

extern double regPredictY(const reg* pReg, double X) 
{
    return pReg->m_Slope * X + pReg->m_Intercept;
}

extern double regSlope(const reg* pReg) 
{
    return pReg->m_Slope;
}

extern size_t regGetSampleCount(const reg* pReg)
{
    return pReg->m_SampleCount;
}

extern void regClearSamples(reg* pReg)
{
    pReg->m_SampleCount = 0;
}
