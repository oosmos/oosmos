#include "oosmos.h"
#include <stdint.h>
#include <stdio.h>

#define DELAY_MS 3000

extern int main(void)
{
  printf("Delaying %d milliseconds...\n", DELAY_MS);

  const uint32_t Start = oosmos_GetFreeRunningUS();
  oosmos_DelayMS(DELAY_MS);
  const uint32_t End   = oosmos_GetFreeRunningUS();

  //printf("Start: %I64d, End: %I32u, Diff: %I32u\n", Start, End, End - (Start + oosmos_MS2US(DELAY_MS)));

  //
  // Test within a practical tolerance.
  //
  const uint32_t ElapsedMS = oosmos_US2MS_Truncated(End - Start);

  printf("ElapsedMS: %u\n", ElapsedMS);

  if (ElapsedMS >= 2000 && ElapsedMS <= 4000) {
    printf("SUCCESS.  Within tolerance.\n");
  } else {
    printf("FAILED.  Outside tolerance.\n");
  }

  return 0;
}
