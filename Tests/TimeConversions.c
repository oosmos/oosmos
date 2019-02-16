#include "oosmos.h"
#include <stdio.h>

#define TEST(Func, Arg, TestValue) \
  { \
    unsigned long Result; \
    /*lint -e778 Suppress 'Constant expression evaluates to 0 in operation /'*/ \
    Result = Func(Arg); \
    printf("%s: %s Argument:%lu Result:%lu Value:%lu\n", Result == TestValue ? "SUCCESS" : "FAILURE", # Func, (Arg), (Result), (TestValue)); \
  }

extern int main(void)
{
  TEST(oosmos_Minutes2US,              1ul,   60000000ul);
  TEST(oosmos_Minutes2MS,              1ul,   60000ul);
  TEST(oosmos_Minutes2Seconds,         1ul,   60ul);
  TEST(oosmos_Minutes2Hours_Truncated, 59ul,  0ul);
  TEST(oosmos_Minutes2Hours_Truncated, 61ul,  1ul);
  TEST(oosmos_Minutes2Hours_Rounded,   29ul,  0ul);
  TEST(oosmos_Minutes2Hours_Rounded,   31ul,  1ul);
  TEST(oosmos_Minutes2Hours_Rounded,   60ul,  1ul);
  TEST(oosmos_Minutes2Hours_Rounded,   120ul, 2ul);

  TEST(oosmos_Seconds2MS,              1ul,    1000ul);
  TEST(oosmos_Seconds2US,              1ul,    1000000ul);
  TEST(oosmos_Seconds2Hours_Truncated, 3599ul, 0ul);
  TEST(oosmos_Seconds2Hours_Truncated, 3601ul, 1ul);
  TEST(oosmos_Seconds2Hours_Rounded,   3600ul, 1ul);
  TEST(oosmos_Seconds2Hours_Rounded,   100ul,  0ul);
  TEST(oosmos_Seconds2Hours_Rounded,   3599ul, 1ul);
  TEST(oosmos_Seconds2Hours_Rounded,   3601ul, 1ul);

  TEST(oosmos_US2MS_Rounded,           1000ul,       1ul);
  TEST(oosmos_US2MS_Rounded,           1400ul,       1ul);
  TEST(oosmos_US2MS_Rounded,           1600ul,       2ul);
  TEST(oosmos_US2Seconds_Rounded,      1400000ul,    1ul);
  TEST(oosmos_US2Seconds_Rounded,      1600000ul,    2ul);
  TEST(oosmos_US2Minutes_Truncated,    60000000ul-1, 0ul);
  TEST(oosmos_US2Minutes_Rounded,      60000000ul,   1ul);
  TEST(oosmos_US2Minutes_Rounded,      60000001ul,   1ul);

  TEST(oosmos_MS2US,                   1ul,        1000ul);
  TEST(oosmos_MS2Seconds_Truncated,    1400ul,     1ul);
  TEST(oosmos_MS2Seconds_Rounded,      1400ul,     1ul);
  TEST(oosmos_MS2Seconds_Rounded,      1600ul,     2ul);
  TEST(oosmos_MS2Minutes_Truncated,    60000ul-1,  0ul);
  TEST(oosmos_MS2Minutes_Truncated,    90000ul,    1ul);
  TEST(oosmos_MS2Minutes_Rounded,      120000ul-1, 2ul);

  return 0;
}
