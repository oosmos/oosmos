//
// OOSMOS - Interrupt structure example on Windows
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//
// Demonstrates structure of how to react to interrupts using OOSMOS.
//

typedef struct uartTag uart;

struct uartTag
{
  oosmos_sActiveObject m_ActiveObject;
  oosmos_sQueue        m_ReceiveDataQueue;
  uint8_t              m_UartId;
  uint8_t              m_ReceiveDataQueueData[10];
};

#define MAX_UARTS 5

static uart     UartList[MAX_UARTS];
static unsigned UartCount = 0;

static void ReceiverStateMachine(void * pObject)
{
  uart * pUART = (uart *) pObject;
  uint8_t Byte = 0;

  //DisableInterrupt(pUART);
    const bool PopSuccess = oosmos_QueuePop(&pUART->m_ReceiveDataQueue, &Byte, sizeof(Byte));
  //EnableInterrupt(pUART);

  if (!PopSuccess) {
    return;
  }

  printf("Received %d\n", Byte);
}

static void ISR(const int UartId)
{
  uart * pUART = UartList;

  for (unsigned Count = 0; Count < UartCount; pUART++, Count++) {
    if (UartId != pUART->m_UartId) {
      continue;
    }

    const uint8_t Byte = (uint8_t) UartId;
    oosmos_QueuePush(&pUART->m_ReceiveDataQueue, &Byte, sizeof(Byte));

    return;
  }
}

extern uart * uartNew(unsigned UartId)
{
  oosmos_AllocateVisible(pUART, uart, UartList, UartCount, MAX_UARTS, NULL);

  pUART->m_UartId = (uint8_t) UartId;

  oosmos_QueueConstruct(&pUART->m_ReceiveDataQueue, pUART->m_ReceiveDataQueueData, sizeof(pUART->m_ReceiveDataQueueData), sizeof(uint8_t));
  oosmos_ActiveObjectInit(pUART, m_ActiveObject, ReceiverStateMachine);

  return pUART;
}

extern int main(void)
{
  (void) uartNew(1);
  (void) uartNew(3);
  (void) uartNew(7);

  // Simulate random interrupts...

  ISR(3);
  oosmos_RunStateMachines();
  ISR(7);
  oosmos_RunStateMachines();
  ISR(1);
  oosmos_RunStateMachines();

  return 0;
}

