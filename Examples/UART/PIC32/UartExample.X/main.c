//
// OOSMOS - PIC32 UART exmample main program.
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
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
#include "uart.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct testTag test;

enum
{
  uartIncomingByte,
};

struct testTag
{
  uart * m_pUART;

  oosmos_sStateMachine(StateMachine, uart_sReceivedByteEvent, 10);
    oosmos_sLeaf       Running_State;
};

static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  test * pTest = (test *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_ENTER:
      uartSendString(pTest->m_pUART, "\r\nStarting...\r\n");
      return oosmos_StateTimeoutSeconds(pState, 1);
    case oosmos_TIMEOUT:
      uartSendString(pTest->m_pUART, "Hello world.\r\n");
      return oosmos_StateTimeoutSeconds(pState, 1);
    case uartIncomingByte: {
      uart_sReceivedByteEvent * pUartEvent = (uart_sReceivedByteEvent *) pEvent;
      DBPRINTF("Incoming Byte: %c (%02x)\n", pUartEvent->Byte, pUartEvent->Byte);
      oosmos_UNUSED(pUartEvent);
      return true;
    }
  }

  return false;
}

static test * testNew(uart * pUART)
{
  oosmos_Allocate(pTest, test, 1, NULL);

  pTest->m_pUART = pUART;

  //                             StateName      Parent       Default
  //                     ===================================================
  oosmos_StateMachineInit(pTest, StateMachine,  NULL,         Running_State);
    oosmos_LeafInit      (pTest, Running_State, StateMachine               );

  return pTest;
}

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

extern int main(void)
{
  oosmos_ClockSpeedInMHz(80);

  uart * pUART = uartNew(2, 9600);
  test * pTest = testNew(pUART);

  uartSubscribe(pUART, oosmos_EventQueue(pTest), uartIncomingByte, NULL);
  uartStart(pUART);

  for (;;) {
    oosmos_RunStateMachines();
  }
}
