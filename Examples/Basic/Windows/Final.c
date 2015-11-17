/*
// OOSMOS Final Example
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
#include "oosmos.h"

static const int isLeft  = 1;
static const int isRight = 0;

typedef struct testTag test;

struct testTag
{
  oosmos_sStateMachineNoQueue(StateMachine);
    oosmos_sComposite         A_State;
      oosmos_sChoice          Choice;
      oosmos_sLeaf            Left_State;
      oosmos_sLeaf            Right_State;
      oosmos_sFinal           FinalA_State;
    oosmos_sLeaf              B_State;
};

static bool A_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pThis = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_DEFAULT:
      printf("Initializing...\n");
      break;
    case oosmos_COMPLETE:
      oosmos_TransitionAction(pRegion, &pThis->B_State, { printf("A-to-B action\n"); } );
      return true;
  } 

  return false;
}

static bool Choice_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pThis = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_ENTER:
      if (isLeft)
        return oosmos_Transition(pRegion, &pThis->Left_State);
      else if (isRight)
        return oosmos_Transition(pRegion, &pThis->Right_State);
  }

  return false;
}

static bool Left_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pThis = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_COMPLETE:
      oosmos_TransitionAction(pRegion, &pThis->FinalA_State, { printf("Left-to-FinalA action\n"); } );
      return true;
  } 

  return false;
}

static bool Right_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  test * pThis = (test *) pObject;

  switch (pEvent->Code) {
    case oosmos_COMPLETE:
      oosmos_TransitionAction(pRegion, &pThis->FinalA_State, { printf("Right-to-FinalA action\n"); } );
      return true;
  } 

  return false;
}


static test * testNew(void)
{
  oosmos_Allocate(pTest, test, 1, NULL);

  /*                                       StateName     Parent          Default   */
  /*                            ================================================== */
  oosmos_StateMachineInitNoQueue(pTest,    StateMachine, NULL,           A_State );
    oosmos_CompositeInit        (pTest,    A_State,      StateMachine,   Choice  );
      oosmos_ChoiceInit         (pTest,    Choice,       A_State                 );
      oosmos_LeafInit           (pTest,    Left_State,   A_State                 );
      oosmos_LeafInit           (pTest,    Right_State,  A_State                 );
      oosmos_FinalInitNoCode    (pTest,    FinalA_State, A_State                 );
    oosmos_LeafInitNoCode       (pTest,    B_State,      StateMachine            );

  oosmos_Debug(&pTest->StateMachine, true, NULL);

  return pTest;
}

extern int main(void)
{
  test * pTest = (test *) testNew();

  while (true) {
    oosmos_RunStateMachines();

    if (oosmos_IsInState(&pTest->StateMachine, &pTest->B_State)) {
      printf("SUCCESS.\n");
      break;
    }
  }

  return 0;
}
