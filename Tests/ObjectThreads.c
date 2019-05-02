//
// OOSMOS Object Thread Test
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

typedef struct testTag test;

struct testTag
{
  oosmos_sObjectThread m_ObjectThreadA;
  oosmos_sObjectThread m_ObjectThreadB;
};

static void ThreadA(void * pObject, oosmos_sState * pState)
{
  test * pTest = pObject;

  oosmos_ThreadBegin();
    for (;;) {
      printf("In Object Thread A, pTest = %p\n", pTest);
      oosmos_ThreadDelayMS(500);
    }
  oosmos_ThreadEnd();
}

static void ThreadB(void * pObject, oosmos_sState * pState)
{
  test * pTest = pObject;

  oosmos_ThreadBegin();
    for (;;) {
      printf("In Object Thread B, pTest = %p\n", pTest);
      oosmos_ThreadDelayMS(1500);
    }
  oosmos_ThreadEnd();
}

extern int main(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

  oosmos_ObjectThreadInit(pTest, m_ObjectThreadA, ThreadA);
  oosmos_ObjectThreadInit(pTest, m_ObjectThreadB, ThreadB);

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
