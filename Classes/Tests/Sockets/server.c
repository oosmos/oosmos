//
// OOSMOS server Class
//
// Copyright (C) 2014-2018  OOSMOS, LLC
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

#include <stdio.h>
#include <stdlib.h>

#include "oosmos.h"
#include "server.h"
#include "sock.h"

enum
{
  ClosedEvent = 1,
};

struct serverTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 1);
    oosmos_sLeaf       Running_State;

  sock * m_pSock;
  size_t m_BytesReceived;
  char   m_Buffer[100];
};

static void IncomingThread(server * pServer, oosmos_sState * pState)
{
  oosmos_ThreadBegin();
    for (;;) {
      printf("Waiting for incoming data...\n");
      oosmos_ThreadWaitCond(
        sockReceive(pServer->m_pSock, pServer->m_Buffer, sizeof(pServer->m_Buffer), &pServer->m_BytesReceived)
      );
      printf("Server side, String: '%s', BytesReceived: %u\n", pServer->m_Buffer, (unsigned) pServer->m_BytesReceived);

      oosmos_ThreadWaitCond(
        sockSend(pServer->m_pSock, pServer->m_Buffer, pServer->m_BytesReceived)
      );
    }
  oosmos_ThreadEnd();
}

static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  server * pServer = (server *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL:
      IncomingThread(pServer, pState);
      return true;

    case ClosedEvent:
      serverDelete(pServer);
      return true;
  }

  return false;
}

extern server * serverNew(sock * pSock)
{
  server * pServer = (server *) malloc(sizeof(server));

  printf("New Server %p.\n", (void *) pServer);

  //                               StateName       Parent        Default
  //                     ======================================================
  oosmos_StateMachineInit(pServer, StateMachine,   NULL,         Running_State);
    oosmos_LeafInit      (pServer, Running_State,  StateMachine               );

  pServer->m_pSock = pSock;
  sockSubscribeClosedEvent(pSock, oosmos_EventQueue(pServer), ClosedEvent, NULL);

  return pServer;
}

extern void serverDelete(server * pServer)
{
  oosmos_StateMachineDetach(pServer, StateMachine);

  printf("Delete Server %p.\n", (void *) pServer);
  sockDelete(pServer->m_pSock);
  free(pServer);
}
