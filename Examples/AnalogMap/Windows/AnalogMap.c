//
// OOSMOS AnalogMap Example
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

#include "oosmos.h"
#include <stdint.h>
#include <stdio.h>

#define ITERATIONS 25

/*lint -e728 Suppress "Not explicitly initialized" */
static int FastDistribution[ITERATIONS+1];
/*lint -e728 Suppress "Not explicitly initialized" */
static int AccurateDistribution[ITERATIONS+1];

static void TestAccurate(double Value, double InMin, double InMax, double OutMin, double OutMax)
{
  const double Result = oosmos_AnalogMapAccurate(Value, InMin, InMax, OutMin, OutMax);

  printf("Value:%f, InMin:%f, InMax:%f, OutMin:%f, OutMax:%f, Result:%f\n", Value, InMin, InMax, OutMin, OutMax, Result);

  const long IntResult = (long) (Result + 0.5F);
  AccurateDistribution[IntResult]++;
}

static void TestFast(int32_t Value, int32_t InMin, int32_t InMax, int32_t OutMin, int32_t OutMax)
{
  const int32_t Result = oosmos_AnalogMapFast(Value, InMin, InMax, OutMin, OutMax);
  printf("Value:%ld, InMin:%ld, InMax:%ld, OutMin:%ld, OutMax:%ld, Result:%ld\n",
         (long) Value, (long) InMin, (long) InMax, (long) OutMin, (long) OutMax, (long) Result);

  FastDistribution[Result]++;
}

extern int main(void)
{
  const uint32_t Min = 1;
  const uint32_t Max = 1000;

  //
  // Test accurate implementation...
  //
  for (uint32_t I = Min; I <= Max; I++) {
    TestAccurate((double) I, (double) Min, (double) Max, 1.0, 25.0);
  }

  for (uint32_t I = 1; I <= ITERATIONS; I++) {
    printf("%d\n", AccurateDistribution[I]);
  }

  //
  // Test fast implementation...
  //
  for (uint32_t I = Min; I <= Max; I++) {
    TestFast(I, Min, Max, 1, 25);
  }

  for (uint32_t I = 1; I <= ITERATIONS; I++) {
    printf("%d\n", FastDistribution[I]);
  }

  return 0;
}
