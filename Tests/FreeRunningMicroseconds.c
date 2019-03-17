#include "oosmos.h"
#include <stdio.h>
#include <stdint.h>

#define DELAY_MS 3000

extern int main(void)
{
  printf("Delaying %d milliseconds...\n", DELAY_MS);

  const uint32_t Start = oosmos_GetFreeRunningMicroseconds();
  oosmos_DelayMS(DELAY_MS);
  const uint32_t End   = oosmos_GetFreeRunningMicroseconds();

  //printf("Start: %I64d, End: %I32u, Diff: %I32u\n", Start, End, End - (Start + oosmos_MS2US(DELAY_MS)));

  //
  // Test within a practical tolerance.
  //
  if (End - (Start + oosmos_MS2US(DELAY_MS)) <= 4000) {
    printf("SUCCESS\n");
  } else {
    printf("FAILED\n");
  }

  return 0;
}
