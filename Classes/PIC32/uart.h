/*
// OOSMOS uart Class
//
// Copyright (C) 2014-2015  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <http://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef uart_h
#define uart_h

#include "oosmos.h"

typedef struct
{
  oosmos_sEvent Event;
  uint8_t       Byte;
} uart_sReceivedByteEvent;

typedef struct uartTag uart;

extern uart * uartNew(int UserUartID, int BaudRate);
extern void uartSendChar(uart * pUART, char Char);
extern void uartSendString(uart * pUART, const char * pString);
extern void uartSubscribe(uart * pUART, oosmos_sQueue * pQueue, const int EventCode, void * pContext);
extern void uartPrintf(uart * pUART, const char * pFormat, ...);
extern void uartStart(uart * pUART);

#endif
