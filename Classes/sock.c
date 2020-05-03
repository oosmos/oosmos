//
// OOSMOS sock Class
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
#include "sock.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// The following preprocessor conditional is used to make the code
// that follows it portable.  Tested on Windows and Linux.
//

#if defined(_WIN32)
  #include <winsock2.h>
  #include <minwindef.h>  // for MAKEWORD, WORD
  #include <winerror.h>   // for WSAEWOULDBLOCK, WSAECONNREFUSED, WSAECONNABORTED
  #include <inaddr.h>     // for IN_ADDR, s_addr

  typedef SOCKET sock_tSocket;

  typedef int      socklen_t;
  typedef uint32_t sendsize_t;
  typedef uint32_t recvsize_t;
  #define IOCTL    ioctlsocket
  #define CLOSE    closesocket

  #define sockEWOULDBLOCK  WSAEWOULDBLOCK
  #define sockECONNRESET   WSAECONNRESET
  #define sockECONNREFUSED WSAECONNREFUSED
  #define sockEINPROGRESS  WSAEINPROGRESS
  #define sockECONNABORTED WSAECONNABORTED
  #define sockINVALID_SOCKET INVALID_SOCKET
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <errno.h>
  #include <arpa/inet.h>
  #include <sys/ioctl.h>
  #include <signal.h>
  #include <sys/select.h>

  typedef int sock_tSocket;

  // socklen_t is already defined in Linux
  typedef ssize_t sendsize_t;
  typedef ssize_t recvsize_t;
  #define IOCTL ioctl
  #define CLOSE close

  #define sockEWOULDBLOCK  EWOULDBLOCK
  #define sockECONNRESET   ECONNRESET
  #define sockECONNREFUSED ECONNREFUSED
  #define sockEINPROGRESS  EINPROGRESS
  #define sockECONNABORTED ECONNABORTED
  #define sockINVALID_SOCKET -1
#endif

#define RECEIVE_BUFFER_SIZE 500

struct sockTag
{
  sock_tSocket m_Socket;

  // Connect...
  bool m_FirstConnect;

  // Send...
  const char * m_pSendData;
  int          m_BytesToSend;

  bool         m_Closed;

  size_t       m_BytesReceived;
  char       * m_pReceiveBuffer;

  oosmos_sSubscriberList   m_ClosedEvent[1];
};

static void Init(void)
{
#if defined(_WIN32)
  static bool Started = false;

  if (!Started) {
    WSADATA wsaData;

    WORD wVersionRequested = MAKEWORD(1, 1);
    (void) WSAStartup(wVersionRequested, &wsaData);
    Started = true;
  }
#else
  signal(SIGPIPE, SIG_IGN);
#endif
}

static bool WouldBlock(sock * pSock, recvsize_t BytesReceived)
{
  if (BytesReceived == (recvsize_t) -1) {
    return sockGetLastError() == sockEWOULDBLOCK;
  }

  oosmos_UNUSED(pSock);

  return false;
}

static bool CheckSendError(sock * pSock, sendsize_t BytesSent)
{
  if (BytesSent <= 0) {
    const int Error = sockGetLastError();

    if (Error == sockEWOULDBLOCK) {
      return false;
    }

    oosmos_SubscriberListNotify(pSock->m_ClosedEvent);
    pSock->m_Closed = true;
    return true;
  }

  return false;
}

static bool CheckReceiveError(sock * pSock, recvsize_t BytesReceived)
{
  if (BytesReceived == -1) {
    const int Error = sockGetLastError();

    if (Error == sockEWOULDBLOCK) {
      return false;
    }

    oosmos_SubscriberListNotify(pSock->m_ClosedEvent);
    pSock->m_Closed = true;
    return true;
  }

  if (BytesReceived == 0) {
    oosmos_SubscriberListNotify(pSock->m_ClosedEvent);
    pSock->m_Closed = true;
    return true;
  }

  return false;
}

static sock * New(sock_tSocket Socket)
{
  sock * pSock = (sock *) malloc(sizeof(sock));

  //
  // Make the socket non-blocking.
  //
  {
    unsigned long IsNonBlocking = 1;
    (void) IOCTL(Socket, FIONBIO, &IsNonBlocking);
  }

  pSock->m_Socket         = Socket;
  pSock->m_Closed         = false;
  pSock->m_FirstConnect   = true;
  pSock->m_pSendData      = NULL;

  pSock->m_BytesReceived  = 0;
  pSock->m_pReceiveBuffer = (char *) malloc(RECEIVE_BUFFER_SIZE);

  oosmos_SubscriberListInit(pSock->m_ClosedEvent);

  return pSock;
}

extern int sockGetLastError(void)
{
#if defined(_WIN32)
  return WSAGetLastError();
#else
  return errno;
#endif
}

extern uint32_t sockDotToIP_HostByteOrder(const char * pDot)
{
  unsigned A = 0;
  unsigned B = 0;
  unsigned C = 0;
  unsigned D = 0;
  sscanf(pDot, "%u.%u.%u.%u", &A, &B, &C, &D);
  return A << 24U | B << 16U | C << 8U | D;
}

extern bool sockIsIpAddress(const char * pString)
{
  while (*pString != '\0') {
    if (!(*pString == '.' || isdigit(*pString))) {
      return false;
    }

    pString++;
  }

  return true;
}

extern void sockSubscribeClosedEvent(sock * pSock, oosmos_sQueue * pEventQueue, int ClosedEventCode, void * pContext)
{
  oosmos_SubscriberListAdd(pSock->m_ClosedEvent, pEventQueue, ClosedEventCode, pContext);
}

extern bool sockListen(sock * pSock, unsigned Port, int Backlog)
{
  struct sockaddr_in Listener;

  Listener.sin_family      = AF_INET;
  Listener.sin_addr.s_addr = INADDR_ANY;
  Listener.sin_port        = htons((unsigned short) Port);

  if (bind(pSock->m_Socket, (struct sockaddr *) &Listener, sizeof(Listener)) < 0) {
    perror("bind failed. Error");
    return false;
  }

  (void) listen(pSock->m_Socket, Backlog);
  return true;
}

extern bool sockAccepted(sock * pListenerSock, sock ** ppNewSock)
{
  struct sockaddr_in Server;

  socklen_t ServerSize = sizeof(Server);
  const sock_tSocket ServerSocket = accept(pListenerSock->m_Socket, (struct sockaddr *) &Server, &ServerSize);

  if (ServerSocket == sockINVALID_SOCKET) {
    return false;
  }

  *ppNewSock = New(ServerSocket);
  return true;
}

extern bool sockReceive(sock * pSock, void * pBuffer, size_t BufferSize, size_t * pBytesReceived)
{
  if (pSock->m_Closed) {
    return false;
  }

  if (pSock->m_BytesReceived < BufferSize) {
    recvsize_t NewBytesReceived = recv(pSock->m_Socket,
                                       pSock->m_pReceiveBuffer+pSock->m_BytesReceived,
                                       (int) (RECEIVE_BUFFER_SIZE-pSock->m_BytesReceived),
                                       0);

    if (WouldBlock(pSock, NewBytesReceived)) {
      //
      // If we would block, then there are no bytes available to read, but there
      // may be bytes already in the receive buffer, so we need to continue to
      // allow those bytes to be consumed.
      //
      NewBytesReceived = 0;
    }
    else if (CheckReceiveError(pSock, NewBytesReceived)) {
      return false;
    }

    pSock->m_BytesReceived += NewBytesReceived;
  }

  if (pSock->m_BytesReceived > 0) {
    // Consume...
    const size_t Span = oosmos_Min(pSock->m_BytesReceived, BufferSize);
    (void) memcpy(pBuffer, pSock->m_pReceiveBuffer, Span);
    *pBytesReceived = Span;

    // Shift...
    pSock->m_BytesReceived -= Span;
    (void) memcpy(pSock->m_pReceiveBuffer, pSock->m_pReceiveBuffer+Span, pSock->m_BytesReceived);

    return true;
  }

  return false;
}

//
// Receive and return bytes up to and including the bytes specified by pContentArg & ContentLength.
//
extern bool sockReceiveUntilContent(sock * pSock, void * pBufferArg, size_t BufferSize,
                                    const void * pContentArg, size_t ContentLength, size_t * pBytesReceived)
{
  char * pBuffer  = (char * ) pBufferArg;
  char * pContent = (char * ) pContentArg;

  const recvsize_t BufferBytesAvailable = (recvsize_t) (RECEIVE_BUFFER_SIZE - pSock->m_BytesReceived);

  if (pSock->m_Closed) {
    return false;
  }

  if (BufferBytesAvailable > 0) {
    recvsize_t NewBytesReceived = recv(pSock->m_Socket,
                                       pSock->m_pReceiveBuffer + pSock->m_BytesReceived,
                                       BufferBytesAvailable, 0);

    if (WouldBlock(pSock, NewBytesReceived)) {
      //
      // If we would block, then there are no bytes available to read, but there
      // may be bytes already in the receive buffer, so we need to continue to
      // allow those bytes to be consumed.
      //
      NewBytesReceived = 0;
    }
    else if (CheckReceiveError(pSock, NewBytesReceived)) {
      return false;
    }

    pSock->m_BytesReceived += NewBytesReceived;
  }

  if (pSock->m_BytesReceived >= ContentLength) {
    const char * pFirst = pSock->m_pReceiveBuffer;
    const char * pLast  = pFirst + pSock->m_BytesReceived - ContentLength;

    //
    // Search for content bytes in the receive buffer.
    //
    for (; pFirst <= pLast; pFirst++) {
      if (memcmp(pFirst, pContent, ContentLength) == 0) {
        // Consume...
        const size_t Span = oosmos_Min((pFirst-pSock->m_pReceiveBuffer)+ContentLength, BufferSize);
        (void) memcpy(pBuffer, pSock->m_pReceiveBuffer, Span);
        *pBytesReceived = Span;

        // Shift...
        pSock->m_BytesReceived -= Span;
        (void) memcpy(pSock->m_pReceiveBuffer, pSock->m_pReceiveBuffer+Span, pSock->m_BytesReceived);
        return true;
      }
    }

    return false;
  }

  return false;
}

extern bool sockSend(sock * pSock, const void * pData, size_t Bytes)
{
  if (pSock->m_Closed) {
    return false;
  }

  if (pSock->m_pSendData == NULL) {
    pSock->m_pSendData = (const char *) pData;
    pSock->m_BytesToSend = (int) Bytes;
  }

  {
    const sendsize_t BytesSent = send(pSock->m_Socket, pSock->m_pSendData, pSock->m_BytesToSend, 0);

    if (CheckSendError(pSock, BytesSent)) {
      return false;
    }

    pSock->m_pSendData   += BytesSent;
    pSock->m_BytesToSend -= BytesSent;

    if (pSock->m_BytesToSend == 0) {
      pSock->m_pSendData = NULL;
      return true;
    }
  }

  return false;
}

extern bool sockConnect(sock * pSock, uint32_t IP_HostByteOrder, unsigned Port)
{
  if (pSock->m_Closed) {
    return false;
  }

  if (pSock->m_FirstConnect) {
    struct sockaddr_in server;

    server.sin_addr.s_addr = htonl(IP_HostByteOrder);
    server.sin_family = AF_INET;
    server.sin_port = htons((uint16_t) Port);

    connect(pSock->m_Socket, (struct sockaddr *) &server, sizeof(server));

    pSock->m_FirstConnect = false;
  }

  fd_set fd_out;
  FD_ZERO(&fd_out);
  FD_SET(pSock->m_Socket, &fd_out);

  const sock_tSocket largest_sock = pSock->m_Socket;

  struct timeval tv;
  tv.tv_sec  = 0;
  tv.tv_usec = 0;

  select((int) (largest_sock + 1), NULL, &fd_out, NULL, &tv);
  const int Writable = FD_ISSET(pSock->m_Socket, &fd_out);

  int Code = 0;
  socklen_t SizeofCode = sizeof(Code);

  getsockopt(pSock->m_Socket, SOL_SOCKET, SO_ERROR, (char *) &Code, &SizeofCode);

  if (Writable && Code == 0) {
    pSock->m_FirstConnect = true;
    return true;
  }

  if (Code == sockECONNREFUSED) {
    (void) sockClose(pSock);
  }

  return false;
}

extern bool sockClose(sock * pSock)
{
  CLOSE(pSock->m_Socket);
  pSock->m_Closed = true;

  return true;
}

extern sock * sockNew(void)
{
  Init();

  sock_tSocket Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  return New(Socket);
}

extern void sockDelete(sock * pSock)
{
  free(pSock->m_pReceiveBuffer);
  (void) sockClose(pSock);
}
