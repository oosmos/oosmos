#include "oosmos.h"
#include <stdio.h>
#include <stdint.h>

#define TEST(Func, Arg, TestValue) \
  { \
    unsigned long Result; \
    Result = Func(Arg); \
    printf("%s: %s Arg:%lu Result:%lu TestValue:%lu\n", Result == TestValue ? "SUCCESS" : "FAILURE", # Func, Arg, Result, TestValue); \
  }

extern int main()
{
  TEST(oosmos_Minutes2US,              1, 60000000);
  TEST(oosmos_Minutes2MS,              1, 60000);
  TEST(oosmos_Minutes2Seconds,         1, 60);
  TEST(oosmos_Minutes2Hours_Truncated, 59, 0);
  TEST(oosmos_Minutes2Hours_Truncated, 61, 1);
  TEST(oosmos_Minutes2Hours_Rounded,   29, 0);
  TEST(oosmos_Minutes2Hours_Rounded,   31, 1);
  TEST(oosmos_Minutes2Hours_Rounded,   60, 1);
  TEST(oosmos_Minutes2Hours_Rounded,   120, 2);

  TEST(oosmos_Seconds2MS,              1, 1000);
  TEST(oosmos_Seconds2US,              1, 1000000);
  TEST(oosmos_Seconds2Hours_Truncated, 3599, 0);
  TEST(oosmos_Seconds2Hours_Truncated, 3601, 1);
  TEST(oosmos_Seconds2Hours_Rounded,   3600, 1);
  TEST(oosmos_Seconds2Hours_Rounded,   100, 0);
  TEST(oosmos_Seconds2Hours_Rounded,   3599, 1);
  TEST(oosmos_Seconds2Hours_Rounded,   3601, 1);

  TEST(oosmos_US2MS_Rounded,           1000, 1);
  TEST(oosmos_US2MS_Rounded,           1400, 1);
  TEST(oosmos_US2MS_Rounded,           1600, 2);
  TEST(oosmos_US2Seconds_Rounded,      1400000, 1);
  TEST(oosmos_US2Seconds_Rounded,      1600000, 2);
  TEST(oosmos_US2Minutes_Truncated,    60000000-1, 0);
  TEST(oosmos_US2Minutes_Rounded,      60000000, 1);
  TEST(oosmos_US2Minutes_Rounded,      60000001, 1);


  TEST(oosmos_MS2US,                   1, 1000);
  TEST(oosmos_MS2Seconds_Truncated,    1400, 1);
  TEST(oosmos_MS2Seconds_Rounded,      1400, 1);
  TEST(oosmos_MS2Seconds_Rounded,      1600, 2);
  TEST(oosmos_MS2Minutes_Truncated,    60000-1, 0);
  TEST(oosmos_MS2Minutes_Truncated,    90000, 1);
  TEST(oosmos_MS2Minutes_Rounded,      120000-1, 2);

  return 0;
}
