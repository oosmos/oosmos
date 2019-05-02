//
// OOSMOS Time Conversions Test
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

#include "oosmos.h"
#include <stdio.h>
#include <stdint.h>

#define TEST(Func, Arg, Value) \
  { \
    uint64_t Result; \
    /*lint -e778 Suppress 'Constant expression evaluates to 0 in operation /'*/ \
    Result = Func(Arg); \
    printf("%s: %s Argument:%I64u Result:%I64u ExpectedValue:%I64u\n", \
         Result == Value ? "SUCCESS" : "FAILURE", # Func, (Arg), (Result), (Value)); \
  }

extern int main(void)
{
  TEST(oosmos_Days2Hours,   1ull,           24ull);
  TEST(oosmos_Days2Minutes, 1ull,         1440ull);
  TEST(oosmos_Days2Seconds, 1ull,        86400ull);
  TEST(oosmos_Days2MS,      1ull,     86400000ull);
  TEST(oosmos_Days2US,      1ull,  86400000000ull);

  printf("\n");

  TEST(oosmos_Hours2Days_Truncated, 24ull,            1ull);
  TEST(oosmos_Hours2Days_Rounded,   24ull+24/2+1,     2ull);
  TEST(oosmos_Hours2Minutes,        24ull,         1440ull);
  TEST(oosmos_Hours2Seconds,        24ull,        86400ull);
  TEST(oosmos_Hours2MS,             24ull,     86400000ull);
  TEST(oosmos_Hours2US,             24ull,  86400000000ull);

  printf("\n");

  TEST(oosmos_Minutes2Days_Truncated,  1440ull,          1ull);
  TEST(oosmos_Minutes2Days_Rounded,    1440ull+1440/2+1, 2ull);
  TEST(oosmos_Minutes2Hours_Truncated, 59ull,  0ull);
  TEST(oosmos_Minutes2Hours_Truncated, 61ull,  1ull);
  TEST(oosmos_Minutes2Hours_Rounded,   29ull,  0ull);
  TEST(oosmos_Minutes2Hours_Rounded,   31ull,  1ull);
  TEST(oosmos_Minutes2Hours_Rounded,   60ull,  1ull);
  TEST(oosmos_Minutes2Hours_Rounded,   120ull, 2ull);
  TEST(oosmos_Minutes2Seconds,         1ull,   60ull);
  TEST(oosmos_Minutes2MS,              1ull,   60000ull);
  TEST(oosmos_Minutes2US,              1ull,   60000000ull);

  printf("\n");

  TEST(oosmos_Seconds2Days_Truncated,  86400ull, 1ull);
  TEST(oosmos_Seconds2Days_Truncated,  86401ull, 1ull);
  TEST(oosmos_Seconds2Days_Rounded,    86400ull + 86400 / 2 + 1, 2ull);
  TEST(oosmos_Seconds2Hours_Truncated, 3599ull, 0ull);
  TEST(oosmos_Seconds2Hours_Truncated, 3601ull, 1ull);
  TEST(oosmos_Seconds2Hours_Rounded,   3600ull, 1ull);
  TEST(oosmos_Seconds2Hours_Rounded,   100ull,  0ull);
  TEST(oosmos_Seconds2Hours_Rounded,   3599ull, 1ull);
  TEST(oosmos_Seconds2Hours_Rounded,   3601ull, 1ull);
  TEST(oosmos_Seconds2US,              1ull,    1000000ull);
  TEST(oosmos_Seconds2MS,              1ull,    1000ull);

  printf("\n");

  TEST(oosmos_MS2Days_Truncated,       86400000ull-1, 0ull);
  TEST(oosmos_MS2Days_Truncated,       86400000ull,   1ull);
  TEST(oosmos_MS2Days_Rounded,         86400000ull+1, 1ull);
  TEST(oosmos_MS2Hours_Truncated,      86400000ull,  24ull);
  TEST(oosmos_MS2Hours_Rounded,        86400000ull+1,24ull);
  TEST(oosmos_MS2Minutes_Truncated,    60000ull-1,  0ull);
  TEST(oosmos_MS2Minutes_Truncated,    90000ull,    1ull);
  TEST(oosmos_MS2Minutes_Rounded,      120000ull-1, 2ull);
  TEST(oosmos_MS2Seconds_Truncated,    1400ull,     1ull);
  TEST(oosmos_MS2Seconds_Rounded,      1400ull,     1ull);
  TEST(oosmos_MS2Seconds_Rounded,      1600ull,     2ull);
  TEST(oosmos_MS2US,                   1ull,        1000ull);

  printf("\n");

  TEST(oosmos_US2Days_Truncated,       86400000000ull-1, 0ull);
  TEST(oosmos_US2Days_Truncated,       86400000000ull,   1ull);
  TEST(oosmos_US2Days_Rounded,         86400000000ull+1, 1ull);
  TEST(oosmos_US2Hours_Truncated,      86400000000ull,  24ull);
  TEST(oosmos_US2Hours_Rounded,        86400000000ull+1,24ull);
  TEST(oosmos_US2Minutes_Truncated,    60000000ull-1, 0ull);
  TEST(oosmos_US2Minutes_Rounded,      60000000ull,   1ull);
  TEST(oosmos_US2Minutes_Rounded,      60000001ull,   1ull);
  TEST(oosmos_US2Seconds_Rounded,      1400000ull,    1ull);
  TEST(oosmos_US2Seconds_Rounded,      1600000ull,    2ull);
  TEST(oosmos_US2MS_Rounded,           1000ull,       1ull);
  TEST(oosmos_US2MS_Rounded,           1400ull,       1ull);
  TEST(oosmos_US2MS_Rounded,           1600ull,       2ull);

  return 0;
}
