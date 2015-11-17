/*
// OOSMOS client Class
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
#include "client.h"
#include "sock.h"

typedef enum
{
  ConnectionTimeoutEvent = 1,
} eEvents;

struct clientTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 1);
    oosmos_sLeaf       Running_State;

  const char * m_pHost;
  int          m_Port;
  sock       * m_pSock;
  char         m_Buffer[100];
};

static bool Running_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  client * pClient = (client *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        oosmos_SyncWaitCond_TimeoutMS_Event(pRegion, sockConnect(pClient->m_pSock, pClient->m_pHost, pClient->m_Port), 2000, ConnectionTimeoutEvent);

        printf("%p: CONNECTED\n", pClient->m_pSock);

        while (true) {
          size_t BytesReceived;

          printf("%p: Sending...\n", pClient->m_pSock);
          oosmos_SyncWaitCond(pRegion, sockSend(pClient->m_pSock, "123456", sizeof("123456")));

          printf("%p: Waiting for incoming data...\n", pClient->m_pSock);
           
          oosmos_SyncWaitCond(pRegion, sockReceive(pClient->m_pSock, pClient->m_Buffer, sizeof(pClient->m_Buffer), &BytesReceived));
          printf("%p: Client side Received '%s', BytesReceived: %u\n", pClient->m_pSock, pClient->m_Buffer, (unsigned) BytesReceived);

        }
      oosmos_SyncEnd(pRegion);
      return true;

    case ConnectionTimeoutEvent:
      printf("%p: Unable to connect to server: Timed out. Terminating.\n", pClient->m_pSock);
      exit(1);
  }

  return false;
}

extern client * clientNew(const char * pHost, int Port)
{
  client * pClient = (client *) malloc(sizeof(client));

  /*                               StateName       Parent        Default        */
  /*                     ====================================================== */
  oosmos_StateMachineInit(pClient, StateMachine,   NULL,         Running_State);
    oosmos_LeafInit      (pClient, Running_State,  StateMachine               );

  pClient->m_pSock = sockNew();
  pClient->m_pHost = pHost;
  pClient->m_Port  = Port;

  return pClient;
}

extern void clientDelete(client * pClient)
{
  printf("In clientDelete\n");
  oosmos_StateMachineDetach(pClient, StateMachine);
  free(pClient);
}
