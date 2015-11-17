/*
// OOSMOS AnalogMap Example
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

#include <stdio.h>
#include "oosmos.h"

#define ITERATIONS 25

static int FastDistribution[ITERATIONS+1];
static int AccurateDistribution[ITERATIONS+1];

static void TestAccurate(float Value, float InMin, float InMax, float OutMin, float OutMax)
{
  float Result = oosmos_AnalogMapAccurate(Value, InMin, InMax, OutMin, OutMax);
  printf("Value:%f, InMin:%f, InMax:%f, OutMin:%f, OutMax:%f, Result:%f\n", Value, InMin, InMax, OutMin, OutMax, Result);

  long IntResult = (long) (Result + .5f);
  AccurateDistribution[IntResult]++;
}

static void TestFast(long Value, long InMin, long InMax, long OutMin, long OutMax)
{
  long Result = oosmos_AnalogMapFast(Value, InMin, InMax, OutMin, OutMax);
  printf("Value:%ld, InMin:%ld, InMax:%ld, OutMin:%ld, OutMax:%ld, Result:%ld\n", Value, InMin, InMax, OutMin, OutMax, Result);

  FastDistribution[Result]++;
}

extern int main(void)
{
  int I;

  long Min = 1;
  long Max = 1000;
  
  /*
  // Test accurate implementation...
  */
  {
    for (I = Min; I <= Max; I++) 
      TestAccurate((float) I, (float) Min, (float) Max, 1, 25);

    for (I = 1; I <= ITERATIONS; I++)
      printf("%d\n", AccurateDistribution[I]);
  }

  /*
  // Test fast implementation...
  */
  {
    for (I = Min; I <= Max; I++) 
      TestFast(I, Min, Max, 1, 25);

    for (I = 1; I <= ITERATIONS; I++)
      printf("%d\n", FastDistribution[I]);
  }

  return 0;
}
