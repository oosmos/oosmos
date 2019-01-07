//
// OOSMOS sock Class
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

#ifndef sock_h
#define sock_h

#include "oosmos.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct sockTag sock;

extern sock * sockNew(void);
extern void sockDelete(sock * pSock);

extern int sockGetLastError(void);

extern bool sockConnect(sock * pSock, uint32_t IP_HostByteOrder, int Port);

extern bool sockListen(sock * pSock, int Port, int Backlog);
extern bool sockAccepted(sock * pListenerSock, sock ** ppNewSock);

extern bool sockSend(sock * pSock, const void * pData, size_t Bytes);

extern bool sockClose(sock * pSock);

extern bool sockReceive(sock * pSock, void * pBuffer, size_t BufferSize, size_t * pBytesReceived);
extern bool sockReceiveUntilContent(sock * pSock, void * pBufferArg, size_t BufferSize, const void * pContent,
                                    size_t ContentLength, size_t * pBytesReceived);

extern void sockSubscribeClosedEvent(sock * pSock, oosmos_sQueue * pEventQueue, int ClosedEventCode, void * pContext);

extern bool sockIsIpAddress(const char * pString);
extern uint32_t sockDotToIP_HostByteOrder(const char * pDot);

#endif
