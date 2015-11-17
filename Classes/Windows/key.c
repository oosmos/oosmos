/*
// OOSMOS key Class
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

#include <windows.h>

#include <stdio.h>
#include "oosmos.h"
#include "key.h"

typedef enum {
  Up_State = 1,
  Down_State,
} eStates;

struct keyTag
{
  char    m_Char;
  eStates m_State;

  oosmos_sActiveObject   m_ActiveObject;

  oosmos_sSubscriberList m_PressedEvent[1];
  oosmos_sSubscriberList m_ReleasedEvent[1];
};

#ifndef keyMAX
#define keyMAX 10
#endif

static HANDLE hStdin;

/*
// Poll the console for this object's character.  
//
// Peek at the buffer.  If it does not contain this object's character, return.
//
// If the buffer does contain this object's character, determine if the key is up 
// or down and store it in the by-reference pKeyState argument and clear the 
// buffer.
*/
static bool IsMyChar(key * pKey, eStates * pKeyState)
{
  DWORD            NumRead;
  INPUT_RECORD     irInBuf[128];
  DWORD            I;

  if (PeekConsoleInput(hStdin, irInBuf, 128, &NumRead) && NumRead == 0)
    return false;

  for (I = 0; I < NumRead; I++) {
    KEY_EVENT_RECORD KER = irInBuf[I].Event.KeyEvent;

    if (KER.uChar.AsciiChar != pKey->m_Char)
      continue;

    *pKeyState = KER.bKeyDown ? Down_State : Up_State;
    
    ReadConsoleInput(hStdin, irInBuf, 128, &NumRead);
    return true;
  }

  /*
  // Purge accumulated unrecognized characters.
  */
  if (NumRead > 10)
    ReadConsoleInput(hStdin, irInBuf, 128, &NumRead);

  return false;
}

static void StateMachine(void * pObject)
{
  key     * pKey = (key *) pObject;
  eStates   KeyState;

  switch (pKey->m_State) {
    case Up_State:
      if (!IsMyChar(pKey, &KeyState))
        return;

      if (KeyState == Down_State) {
        printf("key: ****************** %c pressed\n", pKey->m_Char);
        oosmos_SubscriberListNotify(pKey->m_PressedEvent);
        pKey->m_State = Down_State;
      }

      break;

    case Down_State:
      if (!IsMyChar(pKey, &KeyState))
        return;

      if (KeyState == Up_State) {
        printf("key: ****************** %c released\n", pKey->m_Char);
        oosmos_SubscriberListNotify(pKey->m_ReleasedEvent);
        pKey->m_State = Up_State;
      }

      break;
  }
}

extern key * keyNew(char Char)
{
  static bool Initialized = false;

  oosmos_Allocate(pKey, key, keyMAX, NULL);

  if (!Initialized) {
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    Initialized = true;
  }

  oosmos_RegisterActiveObject(pKey, StateMachine, &pKey->m_ActiveObject);

  pKey->m_Char  = Char;
  pKey->m_State = Up_State;

  oosmos_SubscriberListInit(pKey->m_PressedEvent);
  oosmos_SubscriberListInit(pKey->m_ReleasedEvent);

  return pKey;
}

extern void keySubscribePressedEvent(key * pKey, oosmos_sQueue * pQueue, int PressedEventCode)
{
  oosmos_SubscriberListAdd(pKey->m_PressedEvent, pQueue, PressedEventCode, NULL);
}

extern void keySubscribeReleasedEvent(key * pKey, oosmos_sQueue * pQueue, int ReleasedEventCode)
{
  oosmos_SubscriberListAdd(pKey->m_ReleasedEvent, pQueue, ReleasedEventCode, NULL);
}

