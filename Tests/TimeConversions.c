//
// OOSMOS Time Conversions Test
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
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define TEST(Func, Arg, Value) \
  { \
    uint64_t Result; \
    /*lint -e778 Suppress 'Constant expression evaluates to 0 in operation /'*/ \
    Result = Func(Arg); \
    printf("%s: %s Argument:%" PRIu64 " Result:%" PRIu64 " ExpectedValue:%" PRIu64 "\n", \
         Result == (Value) ? "SUCCESS" : "FAILURE", # Func, (uint64_t) (Arg), (Result), (uint64_t) (Value)); \
  }

extern int main(void)
{
  TEST(oosmos_Days2Hours,   1ULL,           24ULL);
  TEST(oosmos_Days2Minutes, 1ULL,         1440ULL);
  TEST(oosmos_Days2Seconds, 1ULL,        86400ULL);
  TEST(oosmos_Days2MS,      1ULL,     86400000ULL);
  TEST(oosmos_Days2US,      1ULL,  86400000000ULL);

  printf("\n");

  TEST(oosmos_Hours2Days_Truncated, 24ULL,            1ULL);
  TEST(oosmos_Hours2Days_Rounded,   24ULL+24/2+1,     2ULL);
  TEST(oosmos_Hours2Minutes,        24ULL,         1440ULL);
  TEST(oosmos_Hours2Seconds,        24ULL,        86400ULL);
  TEST(oosmos_Hours2MS,             24ULL,     86400000ULL);
  TEST(oosmos_Hours2US,             24ULL,  86400000000ULL);

  printf("\n");

  TEST(oosmos_Minutes2Days_Truncated,  1440ULL,          1ULL);
  TEST(oosmos_Minutes2Days_Rounded,    1440ULL+1440/2+1, 2ULL);
  TEST(oosmos_Minutes2Hours_Truncated, 59ULL,  0ULL);
  TEST(oosmos_Minutes2Hours_Truncated, 61ULL,  1ULL);
  TEST(oosmos_Minutes2Hours_Rounded,   29ULL,  0ULL);
  TEST(oosmos_Minutes2Hours_Rounded,   31ULL,  1ULL);
  TEST(oosmos_Minutes2Hours_Rounded,   60ULL,  1ULL);
  TEST(oosmos_Minutes2Hours_Rounded,   120ULL, 2ULL);
  TEST(oosmos_Minutes2Seconds,         1ULL,   60ULL);
  TEST(oosmos_Minutes2MS,              1ULL,   60000ULL);
  TEST(oosmos_Minutes2US,              1ULL,   60000000ULL);

  printf("\n");

  TEST(oosmos_Seconds2Days_Truncated,  86400ULL, 1ULL);
  TEST(oosmos_Seconds2Days_Truncated,  86401ULL, 1ULL);
  TEST(oosmos_Seconds2Days_Rounded,    86400ULL + 86400 / 2 + 1, 2ULL);
  TEST(oosmos_Seconds2Hours_Truncated, 3599ULL, 0ULL);
  TEST(oosmos_Seconds2Hours_Truncated, 3601ULL, 1ULL);
  TEST(oosmos_Seconds2Hours_Rounded,   3600ULL, 1ULL);
  TEST(oosmos_Seconds2Hours_Rounded,   100ULL,  0ULL);
  TEST(oosmos_Seconds2Hours_Rounded,   3599ULL, 1ULL);
  TEST(oosmos_Seconds2Hours_Rounded,   3601ULL, 1ULL);
  TEST(oosmos_Seconds2US,              1ULL,    1000000ULL);
  TEST(oosmos_Seconds2MS,              1ULL,    1000ULL);

  printf("\n");

  TEST(oosmos_MS2Days_Truncated,       86400000ULL-1, 0ULL);
  TEST(oosmos_MS2Days_Truncated,       86400000ULL,   1ULL);
  TEST(oosmos_MS2Days_Rounded,         86400000ULL+1, 1ULL);
  TEST(oosmos_MS2Hours_Truncated,      86400000ULL,  24ULL);
  TEST(oosmos_MS2Hours_Rounded,        86400000ULL+1,24ULL);
  TEST(oosmos_MS2Minutes_Truncated,    60000ULL-1,  0ULL);
  TEST(oosmos_MS2Minutes_Truncated,    90000ULL,    1ULL);
  TEST(oosmos_MS2Minutes_Rounded,      120000ULL-1, 2ULL);
  TEST(oosmos_MS2Seconds_Truncated,    1400ULL,     1ULL);
  TEST(oosmos_MS2Seconds_Rounded,      1400ULL,     1ULL);
  TEST(oosmos_MS2Seconds_Rounded,      1600ULL,     2ULL);
  TEST(oosmos_MS2US,                   1ULL,        1000ULL);

  printf("\n");

  TEST(oosmos_US2Days_Truncated,       86400000000ULL-1, 0ULL);
  TEST(oosmos_US2Days_Truncated,       86400000000ULL,   1ULL);
  TEST(oosmos_US2Days_Rounded,         86400000000ULL+1, 1ULL);
  TEST(oosmos_US2Hours_Truncated,      86400000000ULL,  24ULL);
  TEST(oosmos_US2Hours_Rounded,        86400000000ULL+1,24ULL);
  TEST(oosmos_US2Minutes_Truncated,    60000000ULL-1, 0ULL);
  TEST(oosmos_US2Minutes_Rounded,      60000000ULL,   1ULL);
  TEST(oosmos_US2Minutes_Rounded,      60000001ULL,   1ULL);
  TEST(oosmos_US2Seconds_Rounded,      1400000ULL,    1ULL);
  TEST(oosmos_US2Seconds_Rounded,      1600000ULL,    2ULL);
  TEST(oosmos_US2MS_Rounded,           1000ULL,       1ULL);
  TEST(oosmos_US2MS_Rounded,           1400ULL,       1ULL);
  TEST(oosmos_US2MS_Rounded,           1600ULL,       2ULL);

  return 0;
}
