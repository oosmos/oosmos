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

#include <stdio.h>
#include <stdlib.h>

#include "oosmos.h"
#include "sock.h"

/*
// The following preprocessor conditional is used to make the code 
// that follows it portable.  Tested on Windows and Raspberry Pi.
*/
  
#ifdef _WIN32
  #include <winsock2.h>

  typedef SOCKET sock_tSocket;

  typedef int socklen_t;
  #define IOCTL ioctlsocket
  #define CLOSE closesocket

  static void Init(void)
  {
    static bool Started = false;
  
    if (!Started) {
      WSADATA wsaData;
  
      WORD wVersionRequested = MAKEWORD(1, 1);
      WSAStartup(wVersionRequested, &wsaData);
      Started = true;
    }
  }

  extern int sockGetLastError(void)
  {
    return WSAGetLastError();
  }

  #define sockEWOULDBLOCK  WSAEWOULDBLOCK
  #define sockECONNRESET   WSAECONNRESET
  #define sockECONNREFUSED WSAECONNREFUSED
  #define sockEINPROGRESS  WSAEINPROGRESS

#else

  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <string.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <errno.h>
  #include <arpa/inet.h>
  #include <sys/ioctl.h>

  typedef int sock_tSocket;

  #define IOCTL ioctl
  #define CLOSE close

  static void Init(void) { }

  extern int sockGetLastError(void)
  {
    return errno;
  }
  
  #define sockEWOULDBLOCK  EWOULDBLOCK
  #define sockECONNRESET   ECONNRESET
  #define sockECONNREFUSED ECONNREFUSED
  #define sockEINPROGRESS  EINPROGRESS
  
#endif

#define RECEIVE_BUFFER_SIZE 1024

struct sockTag
{
  sock_tSocket m_Socket;

  /* Connect... */
  bool m_ConnectPrimed;

  /* Send... */
  const char * m_pSendData;
  int          m_BytesToSend;

  bool         m_Closed;

  size_t       m_BytesReceived;
  char       * m_pReceiveBuffer;
};

static sock * New(int Socket)
{
  sock * pSock = (sock *) malloc(sizeof(sock));

  {
    unsigned long IsNonBlocking = 1;
    IOCTL(Socket, FIONBIO, &IsNonBlocking);
  }

  pSock->m_Socket           = Socket;
  pSock->m_Closed           = false;
  pSock->m_ConnectPrimed    = false;
  pSock->m_pSendData        = NULL;

  pSock->m_BytesReceived    = 0;
  pSock->m_pReceiveBuffer   = (char *) malloc(RECEIVE_BUFFER_SIZE);

  return pSock;
}

extern void sockListen(sock * pSock, int Port, int Backlog) 
{
  struct sockaddr_in Listener;

  Listener.sin_family      = AF_INET;
  Listener.sin_addr.s_addr = INADDR_ANY;
  Listener.sin_port        = htons((short) Port);

  if (bind(pSock->m_Socket, (struct sockaddr *) &Listener, sizeof(Listener)) < 0) {
    perror("bind failed. Error");
  }
  else {
    listen(pSock->m_Socket, Backlog);
  }
}

extern bool sockAccepted(sock * pListenerSock, sock ** ppNewSock)
{
  struct sockaddr_in Server;

  socklen_t ServerSize   = sizeof(Server);
  const int ServerSocket = accept(pListenerSock->m_Socket, (struct sockaddr *) &Server, &ServerSize);

  if (ServerSocket == -1) 
    return false;

  *ppNewSock = New(ServerSocket);
  return true;
}

static void CheckError(sock * pSock)
{
  const int Error = sockGetLastError();

  if (Error == sockECONNRESET) {
    pSock->m_Closed = true;
  }
}

extern bool sockReceive(sock * pSock, void * pBuffer, size_t BufferSize, size_t * pBytesReceived)
{
  if (pSock->m_Closed) {
    return false;
  }

  const int BytesReceived = recv(pSock->m_Socket, (char *) pBuffer, BufferSize, 0);

  if (BytesReceived > 0) {
    *pBytesReceived = BytesReceived;
    return true;
  }

  CheckError(pSock);

  return false;
}

extern bool sockReceiveUntilLength(sock * pSock, void * pBufferArg, size_t BufferSize, size_t Length)
{
  char * pBuffer = (char * ) pBufferArg;

  if (pSock->m_Closed) {
    return false;
  }

  if (pSock->m_BytesReceived < Length) {
    const int NewBytesReceived = recv(pSock->m_Socket, 
                                      pSock->m_pReceiveBuffer+pSock->m_BytesReceived, 
                                      RECEIVE_BUFFER_SIZE-pSock->m_BytesReceived, 
                                      0);

    if (NewBytesReceived < 0) {
      CheckError(pSock);
      return false;
    }

    pSock->m_BytesReceived += NewBytesReceived;
  }

  if (pSock->m_BytesReceived >= Length) {
    memcpy(pBuffer, pSock->m_pReceiveBuffer, Length);
    pSock->m_BytesReceived -= Length;
    memcpy(pSock->m_pReceiveBuffer, pSock->m_pReceiveBuffer+Length, pSock->m_BytesReceived);
    return true;
  }

  return false;
}

extern bool sockReceiveUntilContent(sock * pSock, void * pBufferArg, size_t BufferSize, const void * pContent, 
                                    size_t Length, size_t * pBytesReceived)
{
  char * pBuffer = (char * ) pBufferArg;

  if (pSock->m_Closed) {
    return false;
  }

  const int NewBytesReceived = recv(pSock->m_Socket, 
                                    pSock->m_pReceiveBuffer+pSock->m_BytesReceived, 
                                    RECEIVE_BUFFER_SIZE-pSock->m_BytesReceived, 
                                    0);

  if (NewBytesReceived > 0) {
    pSock->m_BytesReceived += NewBytesReceived;

    const char * pStart = pSock->m_pReceiveBuffer;
    const char * pEnd   = pSock->m_pReceiveBuffer + pSock->m_BytesReceived - Length;

    for (; pStart <= pEnd; pStart++) {
      if (memcmp(pStart, pContent, Length) == 0) {
        const size_t Span = pStart-pSock->m_pReceiveBuffer+Length;
        memcpy(pBuffer, pSock->m_pReceiveBuffer, Span);
        pSock->m_BytesReceived -= Span;
        memcpy(pSock->m_pReceiveBuffer, pSock->m_pReceiveBuffer+Span, pSock->m_BytesReceived);
        *pBytesReceived = Span;
        return true;
      }
    }

    return false;
  }

  CheckError(pSock);

  return false;
}

extern bool sockSend(sock * pSock, const void * pData, size_t Bytes)
{
  if (pSock->m_pSendData == NULL) {
    pSock->m_pSendData = (char *) pData;
    pSock->m_BytesToSend = Bytes;
  }

  if (pSock->m_Closed) {
    return false;
  }

  const long BytesSent = send(pSock->m_Socket, pSock->m_pSendData, pSock->m_BytesToSend, 0);

  if (BytesSent <= 0) {
    const int Error = sockGetLastError();

    if (Error == sockECONNRESET)
      pSock->m_Closed = true;

    return false;
  }

  pSock->m_pSendData   += BytesSent;
  pSock->m_BytesToSend -= BytesSent;

  if (pSock->m_BytesToSend == 0) {
    pSock->m_pSendData = NULL; 
    return true;
  }

  return false;
}

extern bool sockConnect(sock * pSock, const char * pHost, int Port)
{
  if (!pSock->m_ConnectPrimed) {
    struct sockaddr_in server;

    server.sin_addr.s_addr = inet_addr(pHost);
    server.sin_family = AF_INET;
    server.sin_port = htons((short) Port);
  
    connect(pSock->m_Socket, (struct sockaddr *) &server, sizeof(server));

    pSock->m_ConnectPrimed = true;
  }

  if (pSock->m_Closed) {
    return false;
  }

  fd_set fd_out;
  struct timeval tv;

  FD_ZERO( &fd_out );
  FD_SET( pSock->m_Socket, &fd_out );
   
  const int largest_sock = pSock->m_Socket;
   
  tv.tv_sec  = 0;
  tv.tv_usec = 0;
   
  select(largest_sock+1, NULL, &fd_out, NULL, &tv); 
  const int Writable = FD_ISSET(pSock->m_Socket, &fd_out);

  int Code;
  socklen_t SizeofCode = sizeof(Code);

  getsockopt(pSock->m_Socket, SOL_SOCKET, SO_ERROR, (char *) &Code, &SizeofCode);

  if (Writable && Code == 0) {
    return true;
  }

  if (Code == sockECONNREFUSED) {
    CLOSE(pSock->m_Socket);  
    pSock->m_Closed = true;
  }

  return false;
}

extern sock * sockNew(void)
{
  Init();
  const int Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  return New(Socket);
}

extern void sockDelete(sock * pSock)
{
  free(pSock->m_pReceiveBuffer);
  CLOSE(pSock->m_Socket);
}
