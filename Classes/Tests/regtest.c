//
// OOSMOS regtest Class
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
#include "prt.h"
#include "regtest.h"

static void Predict(reg * pReg, float Value)
{
  prtFormatted("Predict value at time %.4f: %.4f\n", Value, regPredictY(pReg, Value));
}

extern void regtestNew(void)
{
  static regSample Samples[] = {
    { 2.0F, 3.0F },
    { 3.0F, 6.7F },
    { 4.0F, 7.0F },
    { 5.0F, 8.0F },
    { 6.0F, 9.0F },
  };
  
  const size_t numSamples = sizeof(Samples)/sizeof(Samples[0]);

  reg *pReg = regNew(Samples, numSamples);



  for (size_t i = 0; i < numSamples; ++i) {
    regPushSample(pReg, &Samples[i]);
  }

  regCalculateRegression(pReg);

  Predict(pReg, 5.0F);
  Predict(pReg, 6.0F);
  Predict(pReg, 7.0F);
  Predict(pReg, 8.0F);
  Predict(pReg, 9.0F);
}
