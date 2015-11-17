/*
// OOSMOS sock Class
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

#ifndef sock_h
#define sock_h

typedef struct sockTag sock;

extern sock * sockNew(void);
extern void sockDelete(sock * pSock);

extern int sockGetLastError(void);

extern bool sockConnect(sock * pSock, const char * pHost, int Port);

extern void sockListen(sock * pSock, int Port, int Backlog);
extern bool sockAccepted(sock * pListenerSock, sock ** ppNewSock);

extern bool sockSend(sock * pSock, const void * pData, size_t Bytes);

extern bool sockReceive(sock * pSock, void * pBuffer, size_t BufferSize, size_t * pBytesReceived);
extern bool sockReceiveUntilLength(sock * pSock, void * pBufferArg, size_t BufferSize, size_t Length);
extern bool sockReceiveUntilContent(sock * pSock, void * pBufferArg, size_t BufferSize, const void * pContent, 
                                    size_t ContentLength, size_t * pBytesReceived);

#endif
