//
// OOSMOS httptest Class
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

#include "dns.h"
#include "httptest.h"
#include "oosmos.h"
#include "sock.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
  ConnectionTimeoutEvent = 1,
  ClosedEvent,
};

struct httptestTag
{
  oosmos_sStateMachine(ROOT, oosmos_sEvent, 1);
    oosmos_sLeaf       Running_State;

  const char * m_pHost;
  unsigned     m_Port;
  sock       * m_pSock;
  char         m_Buffer[2000];
  dns        * m_pDNS;
  unsigned     m_ID;

  struct {
    uint32_t  m_IP_HostByteOrder[3];
    size_t    m_ContentLength;
  } Running;
};

static void Thread(httptest * pHttpTest, oosmos_sState * pState)
{
  size_t BytesReceived = 0;
  bool   TimedOut = false;

  oosmos_ThreadBegin();
    printf("%d: Getting IP from DNS lookup.\n", pHttpTest->m_ID);

    if (sockIsIpAddress(pHttpTest->m_pHost)) {
      pHttpTest->Running.m_IP_HostByteOrder[0] = sockDotToIP_HostByteOrder(pHttpTest->m_pHost);
    }
    else {

      oosmos_ThreadWaitCond_TimeoutMS(dnsQuery(pHttpTest->m_pDNS, pHttpTest->m_pHost, pHttpTest->Running.m_IP_HostByteOrder, 3), 8000, &TimedOut);

      if (TimedOut) {
        oosmos_PushEventCode(pHttpTest, ConnectionTimeoutEvent);
      }
    }

    printf("%d: Connecting...\n", pHttpTest->m_ID);

    oosmos_ThreadWaitCond_TimeoutMS(sockConnect(pHttpTest->m_pSock, pHttpTest->Running.m_IP_HostByteOrder[0], pHttpTest->m_Port), 2000, &TimedOut);

    printf("%d: CONNECTED\n", pHttpTest->m_ID);

    {
      static const char GET[] = "GET http://example.com/index.html HTTP/1.1\r\n\r\n";
      printf("%d: Sending GET...\n", pHttpTest->m_ID);
      oosmos_ThreadWaitCond(
        sockSend(pHttpTest->m_pSock, GET, strlen(GET))
      );
    }

    printf("%d: Waiting for Content-Length:...\n", pHttpTest->m_ID);

    //
    // Receive until we see "Content-Length: ".
    //
    {
      static const char   ContentLength[]   = "Content-Length: ";
      static const size_t ContentLengthSize = sizeof(ContentLength) - 1;

      oosmos_ThreadWaitCond(
        sockReceiveUntilContent(pHttpTest->m_pSock,
                                pHttpTest->m_Buffer, sizeof(pHttpTest->m_Buffer),
                                ContentLength, ContentLengthSize, &BytesReceived)
      );
    }

    //
    // Receive header until we see "\r\n\r\n".
    //
    {
      static const char   End[]     = "\r\n\r\n";
      static const size_t EndLength = sizeof(End) - 1;

      printf("%d: Waiting for end of header...\n", pHttpTest->m_ID);

      oosmos_ThreadWaitCond(
        sockReceiveUntilContent(pHttpTest->m_pSock,
                                pHttpTest->m_Buffer, sizeof(pHttpTest->m_Buffer),
                                End, EndLength, &BytesReceived)
      );
    }

    pHttpTest->Running.m_ContentLength = (size_t) strtoul(pHttpTest->m_Buffer, NULL, 10);
    printf("%u: Reading %u bytes...\n", pHttpTest->m_ID, (unsigned) pHttpTest->Running.m_ContentLength);

    //
    // Receive body.
    //
    while (pHttpTest->Running.m_ContentLength > 0) {
      oosmos_ThreadWaitCond(
        sockReceive(pHttpTest->m_pSock,
                    pHttpTest->m_Buffer, sizeof(pHttpTest->m_Buffer),
                    &BytesReceived)
      );

      pHttpTest->Running.m_ContentLength -= BytesReceived;
    }

    sockClose(pHttpTest->m_pSock);
    printf("%d: DONE...\n", pHttpTest->m_ID);
  oosmos_ThreadEnd();
}

static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  httptest * pHttpTest = (httptest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      Thread(pHttpTest, pState);
      return true;
    }
    case ClosedEvent: {
      printf("Server closed.  Terminating...\n");
      return true;
    }
    case ConnectionTimeoutEvent: {
      printf("%d: Unable to connect to server: Timed out. Terminating.\n", pHttpTest->m_ID);
      return true;
    }
  }

  return false;
}

extern httptest * httptestNew(const char * pHost, unsigned Port, unsigned ID)
{
  httptest * pHttpTest = (httptest *) malloc(sizeof(httptest));

  //                                 StateName       Parent
  //                     =======================================================
  oosmos_StateMachineInit(pHttpTest, ROOT,           NULL,   Running_State     );
    oosmos_LeafInit      (pHttpTest, Running_State,  ROOT,   Running_State_Code);

  pHttpTest->m_pSock = sockNew();
  pHttpTest->m_pHost = pHost;
  pHttpTest->m_Port  = Port;
  pHttpTest->m_pDNS  = dnsNew();
  pHttpTest->m_ID    = ID;

  sockSubscribeClosedEvent(pHttpTest->m_pSock, oosmos_EventQueue(pHttpTest), ClosedEvent, NULL);

  return pHttpTest;
}

extern void httptestDelete(httptest * pHttpTest)
{
  printf("In httptestDelete\n");
  oosmos_StateMachineDetach(pHttpTest, ROOT);
  free(pHttpTest);
}
