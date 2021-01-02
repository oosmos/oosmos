//
// OOSMOS encodertest Class
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
#include "pin.h"
#include "encoder.h"
#include "prt.h"
#include "encodertest.h"
#include <stdbool.h>
#include <stddef.h>

#define MAX_ENCODERTESTS 2

enum
{
  ChangeEvent = 1,
};

struct encodertestTag
{
  oosmos_sStateMachine(StateMachine, oosmos_sEvent, 5);
    oosmos_sLeaf       Idle_State;

  encoder * m_pEncoder;
};

static bool Idle_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  encodertest * pExample = (encodertest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ChangeEvent: {
      prtFormatted("Encoder %p %d\n", pExample, encoderGetCount(pExample->m_pEncoder));
      return true;
    }
  }

  return false;
}

extern encodertest * encodertestNew(pin * pPinA, pin * pPinB)
{
  oosmos_Allocate(pEncoderTest, encodertest, MAX_ENCODERTESTS, NULL);

  //                                    StateName     Parent
  //                     ======================================================
  oosmos_StateMachineInit(pEncoderTest, StateMachine, NULL,         Idle_State     );
    oosmos_LeafInit      (pEncoderTest, Idle_State,   StateMachine, Idle_State_Code);

  encoder * pEncoder = encoderNew(pPinA, pPinB);
  encoderSubscribeChangeEvent(pEncoder, oosmos_EventQueue(pEncoderTest), ChangeEvent, NULL);

  pEncoderTest->m_pEncoder = pEncoder;
  return pEncoderTest;
}
