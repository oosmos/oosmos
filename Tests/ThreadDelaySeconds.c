//
// OOSMOS ThreadDelaySeconds Test
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

typedef struct
{
  oosmos_sObjectThread m_ObjectThread;
} test;

static void Thread(test * pTest, oosmos_sState * pState)
{
  oosmos_UNUSED(pTest);

  oosmos_ThreadBegin();
    printf("Wait for 3 seconds...\n");

    oosmos_ThreadDelaySeconds(3);

    printf("Done.  Press CNTL-C to exit.\n");
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
