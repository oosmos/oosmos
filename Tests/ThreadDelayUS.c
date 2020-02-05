//
// OOSMOS ThreadDelayUS Test
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
#include <stdio.h>
#include <stdint.h>

typedef struct
{
  oosmos_sObjectThread m_ObjectThread;
  int      I;
  uint32_t Before;
  uint32_t After;
  uint32_t Diff;
} test;

static void Thread(test * pTest, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    printf("Starting Thread\n");

    pTest->Before = oosmos_GetFreeRunningUS();

    for (pTest->I = 1; pTest->I <= 5; pTest->I++) {
      oosmos_ThreadDelayUS(1000);
    }

    pTest->After = oosmos_GetFreeRunningUS();

    pTest->Diff = pTest->After - pTest->Before;

    printf("Diff: %u %u %u\n", (unsigned int) pTest->Diff, (unsigned int) pTest->Before, (unsigned int) pTest->After);

    if (pTest->Diff >= 5000 && pTest->Diff <= 5100) {
      printf("Success\n");
    }
    else {
      printf("Failed\n");
    }
  oosmos_ThreadEnd();
}

extern int main(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

  oosmos_ObjectThreadInit(pTest, m_ObjectThread, Thread, true);

  for (;;) {
    oosmos_RunStateMachines();
  }
}
