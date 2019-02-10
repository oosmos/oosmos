//
// OOSMOS AnalogMap Example
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <stdint.h>
#include "oosmos.h"

#define ITERATIONS 25

/*lint -e728 Suppress "Not explicitly initialized" */
static int FastDistribution[ITERATIONS+1];
/*lint -e728 Suppress "Not explicitly initialized" */
static int AccurateDistribution[ITERATIONS+1];

static void TestAccurate(float Value, float InMin, float InMax, float OutMin, float OutMax)
{
  const float Result = oosmos_AnalogMapAccurate(Value, InMin, InMax, OutMin, OutMax);

  printf("Value:%f, InMin:%f, InMax:%f, OutMin:%f, OutMax:%f, Result:%f\n", Value, InMin, InMax, OutMin, OutMax, Result);

  const long IntResult = (long) (Result + .5f);
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
  const long Min = 1;
  const long Max = 1000;

  //
  // Test accurate implementation...
  //
  for (int I = Min; I <= Max; I++) {
    TestAccurate((float) I, (float) Min, (float) Max, 1.0f, 25.0f);
  }

  for (int I = 1; I <= ITERATIONS; I++) {
    printf("%d\n", AccurateDistribution[I]);
  }

  //
  // Test fast implementation...
  //
  for (int I = Min; I <= Max; I++) {
    TestFast(I, Min, Max, 1, 25);
  }

  for (int I = 1; I <= ITERATIONS; I++) {
    printf("%d\n", FastDistribution[I]);
  }

  return 0;
}
