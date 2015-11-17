/*
// OOSMOS listener Class
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
#include "listener.h"
#include "sock.h"

struct listenerTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sLeaf              Running_State;

  sock * m_pSock;
  void (*m_pAcceptedFunc)(sock *);
};

static bool Running_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  listener * pListener = (listener *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      sockListen(pListener->m_pSock, 60009, 50);
      return true;

    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        while (true) {
          printf("Waiting for incoming connections...\n");

          sock * pNewSock;
          oosmos_SyncWaitCond(pRegion, sockAccepted(pListener->m_pSock, &pNewSock));
          pListener->m_pAcceptedFunc(pNewSock);
        }
      oosmos_SyncEnd(pRegion);
      return true;
  }

  return false;
}

extern listener * listenerNew(int Port, void (*pAcceptedFunc)(sock *))
{
  oosmos_Allocate(pListener, listener, 1, NULL);

  /*                                        StateName       Parent        Default        */
  /*                            ======================================================== */
  oosmos_StateMachineInitNoQueue(pListener, StateMachine,   NULL,         Running_State);
    oosmos_LeafInit             (pListener, Running_State,  StateMachine               );

  pListener->m_pSock = sockNew();
  pListener->m_pAcceptedFunc = pAcceptedFunc;

  return pListener;
}
