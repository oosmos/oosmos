//
// OOSMOS reg Class
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

#ifndef reg_h
#define reg_h

#include <stddef.h>

typedef struct regTag reg;

typedef struct {
    float X;
    float Y;
} regSample;

#if defined(__cplusplus)
extern "C" {
#endif

extern reg*    regNew(regSample* sampleBuffer, size_t Samples);
extern void    regPushSample(reg* pReg, float X, float Y);
extern size_t  regGetSampleCount(const reg* pReg);
extern void    regCalculateRegression(reg* pReg);
extern void    regClearSamples(reg * pReg);
extern float   regPredictY(const reg* pReg, float X);
extern float   regSlope(const reg* pReg);
extern void    regIterateSamples(const reg* pReg, void (*pCallback)(const size_t Index, const regSample* pSample));

#if defined(__cplusplus)
}
#endif

#endif


