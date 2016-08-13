//
// OOSMOS asynctest Class
//
// Copyright (C) 2014-2016  OOSMOS, LLC
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
//

#include <stdlib.h>
#include "oosmos.h"
#include "prt.h"
#include "asynctest.h"

//
// Change the following #undef to #define to enable debug output.
//
#undef asynctestDEBUG

//
// Adjust this in order to preallocate all 'asynctest' objects.  
// Use 1 for a memory constrained environment.
//
#ifndef MAX_ASYNCTEST
#define MAX_ASYNCTEST 1
#endif

static int TotalTests       = 11;
static int AsyncTestObjects = 0;
static int Successes        = 0;
static int Failures         = 0;

enum {
  eventDone = 1,
  eventTimedOut,
  eventPrint,
};

#ifdef asynctestDEBUG
  #define NameCase(Name) case Name: return #Name;

  static const char * EventNames(int EventCode)
  {
    switch(EventCode) {
      NameCase(eventDone)
      NameCase(eventTimedOut)
      NameCase(eventPrint)
      default: return "--No Event Name--";
    }
  }
#endif

struct asynctestTag
{
  oosmos_sStateMachine   (StateMachine, oosmos_sEvent, 10);
    oosmos_sOrtho         Ortho_State;
      oosmos_sOrthoRegion Ortho_RegionA_State;
        oosmos_sLeaf      Ortho_RegionA_TestA_State;
        oosmos_sLeaf      Ortho_RegionA_TestB_State;
        oosmos_sLeaf      Ortho_RegionA_TestC_State;
        oosmos_sLeaf      Ortho_RegionA_TestD_State;
        oosmos_sLeaf      Ortho_RegionA_TestE_State;
        oosmos_sLeaf      Ortho_RegionA_TestF_State;
        oosmos_sLeaf      Ortho_RegionA_TestG_State;
        oosmos_sLeaf      Ortho_RegionA_Done_State;
      oosmos_sOrthoRegion Ortho_RegionB_State;
        oosmos_sLeaf      Ortho_RegionB_Idle_State;
};

static int ConditionRandom(int Range)
{
  return rand() % Range == 0;
}

static bool ConditionTrue(void)
{
  return true;
}

static bool ConditionFalse(void)
{
  return false;
}


//
// Test oosmos_AsyncDelayMS.
//
static bool Ortho_RegionA_TestA_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        prtFormatted("Test AsyncDelayMS...\n");
        oosmos_AsyncDelayMS(pRegion, 100);
      oosmos_AsyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pAsyncTest->Ortho_RegionA_TestB_State);
  }

  return false;
}

//
// oosmos_AsyncWaitCond
// oosmos_AsyncWaitCond_TimeoutMS
//
static bool Ortho_RegionA_TestB_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;
  bool TimedOut;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        prtFormatted("Test AsyncWaitCond...\n");

        oosmos_AsyncWaitCond(pRegion, ConditionRandom(2));

        prtFormatted("Test AsyncWaitCond_TimeoutMS...\n");

        oosmos_AsyncWaitCond_TimeoutMS(pRegion, 100, &TimedOut,
          ConditionTrue()
        );
        Successes += (TimedOut == false);

        oosmos_AsyncWaitCond_TimeoutMS(pRegion, 100, &TimedOut,
          ConditionFalse()
        );
        Successes += (TimedOut == true);
      oosmos_AsyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pAsyncTest->Ortho_RegionA_TestC_State);
  }

  return false;
}

//
// oosmos_AsyncWaitCond_TimeoutMS_Event
//
static bool Ortho_RegionA_TestC_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        prtFormatted("Test AsyncWaitCond_TimeoutMS_Event...\n");

        oosmos_AsyncWaitCond_TimeoutMS_Event(pRegion, 100, eventTimedOut,
          ConditionTrue()
        );
        Successes += 1;

        oosmos_AsyncWaitCond_TimeoutMS_Event(pRegion, 100, eventTimedOut,
          ConditionFalse()
        );
        Failures += 1;
      oosmos_AsyncEnd(pRegion);
      return oosmos_Transition(pRegion, &pAsyncTest->Ortho_RegionA_TestD_State);

    case eventTimedOut:
      Successes += 1;
      return true;
  }

  return false;
}

//
// oosmos_AsyncWaitCond_TimeoutMS_Exit
//
static bool Ortho_RegionA_TestD_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        prtFormatted("Test AsyncWaitCond_TimeoutMS_Exit...\n");

        oosmos_AsyncWaitCond_TimeoutMS_Exit(pRegion, 100,
          ConditionTrue()
        );
        Successes += 1;

        oosmos_AsyncWaitCond_TimeoutMS_Exit(pRegion, 100,
          ConditionFalse()
        );
        Failures += 1;

      oosmos_AsyncFinally(pRegion);
        Successes += 1;
        oosmos_Transition(pRegion, &pAsyncTest->Ortho_RegionA_TestE_State);

      oosmos_AsyncEnd(pRegion);
      return true;
  }

  return false;
}

//
// oosmos_AsyncWaitEvent
// oosmos_AsyncWaitEvent_TimeoutMS
//
static bool Ortho_RegionA_TestE_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;
  bool TimedOut;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        prtFormatted("Test AsyncWaitEvent...\n");

        oosmos_SendEvent(pAsyncTest, eventPrint);
        oosmos_AsyncWaitEvent(pRegion, pEvent, eventDone);

        prtFormatted("Test AsyncWaitEvent_TimeoutMS...\n");

        oosmos_SendEvent(pAsyncTest, eventPrint);
        oosmos_AsyncWaitEvent_TimeoutMS(pRegion, pEvent, 100, &TimedOut,
          eventDone
        );
        Successes += (TimedOut == false);

        oosmos_AsyncWaitEvent_TimeoutMS(pRegion, pEvent, 100, &TimedOut,
          eventDone
        );
        Successes += (TimedOut == true);
      oosmos_AsyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pAsyncTest->Ortho_RegionA_TestF_State);
  }

  return false;
}

//
// oosmos_AsyncWaitEvent_TimeoutMS_Event
//
static bool Ortho_RegionA_TestF_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        prtFormatted("Test AsyncWaitEvent_TimeoutMS_Event...\n");

        //
        // Enqueue 'eventPrint' to the orthogonal state. We expect to get and 'eventDone' well 
        // before the timeout.
        //
        oosmos_SendEvent(pAsyncTest, eventPrint);
        oosmos_AsyncWaitEvent_TimeoutMS_Event(pRegion, pEvent, 100, eventTimedOut,
          eventDone
        );
        Successes += 1;

        //
        // We don't enqueue 'eventPrint' so we don't expect an 'eventDone' therefore it should time out.
        //
        oosmos_AsyncWaitEvent_TimeoutMS_Event(pRegion, pEvent, 100, eventTimedOut,
          eventDone
        );
        Failures += 1;
      oosmos_AsyncEnd(pRegion);
      return oosmos_Transition(pRegion, &pAsyncTest->Ortho_RegionA_TestG_State);

    case eventTimedOut:
      Successes += 1;
      return true;
  }

  return false;
}

//
// oosmos_AsyncWaitEvent_TimeoutMS_Exit
//
static bool Ortho_RegionA_TestG_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_AsyncBegin(pRegion);
        prtFormatted("Test AsyncWaitEvent_TimeoutMS_Exit...\n");

        //
        // Enqueue 'eventPrint' to the orthogonal state. We expect to get and 'eventDone' well 
        // before the timeout.
        //
        oosmos_SendEvent(pAsyncTest, eventPrint);
        oosmos_AsyncWaitEvent_TimeoutMS_Exit(pRegion, pEvent, 100,
          eventDone
        );
        Successes += 1;

        //
        // We don't enqueue 'eventPrint' so we don't expect an 'eventDone' therefore it should time out.
        //
        oosmos_AsyncWaitEvent_TimeoutMS_Exit(pRegion, pEvent, 100,
          eventDone
        );
        Failures += 1;

      oosmos_AsyncFinally(pRegion);
        oosmos_Transition(pRegion, &pAsyncTest->Ortho_RegionA_Done_State);

      oosmos_AsyncEnd(pRegion);
      return true;
  }

  return false;
}

static bool Ortho_RegionA_Done_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  switch (pEvent->Code) {
    case oosmos_ENTER:
      prtFormatted("TotalTests: %d, Successes: %d, Failures: %d\n", TotalTests, Successes, Failures);

      if (Failures > 0) {
        prtFormatted("FAILURE.\n");
        oosmos_EndProgram(0);
      }

      if (Successes == TotalTests * AsyncTestObjects) {
        prtFormatted("SUCCESS.\n");
        oosmos_EndProgram(0);
      }

      return true;
  }
  
  return false;
}

static bool Ortho_RegionB_Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  asynctest * pAsyncTest = (asynctest *) pObject;

  switch (pEvent->Code) {
    case eventPrint:
      return oosmos_SendEvent(pAsyncTest, eventDone);
  }

  return false;
}

extern asynctest * asynctestNew(void)
{  
  oosmos_Allocate(pAsyncTest, asynctest, MAX_ASYNCTEST, NULL);

  //                                          StateName                  Parent                 Default
  //                              ========================================================================================
  oosmos_StateMachineInit         (pAsyncTest, StateMachine,              NULL,                  Ortho_State              );
    oosmos_OrthoInitNoCode        (pAsyncTest, Ortho_State,               StateMachine                                    );
      oosmos_OrthoRegionInitNoCode(pAsyncTest, Ortho_RegionA_State,       Ortho_State,           Ortho_RegionA_TestA_State);
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_TestA_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_TestB_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_TestC_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_TestD_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_TestE_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_TestF_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_TestG_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionA_Done_State,  Ortho_RegionA_State                             );
      oosmos_OrthoRegionInitNoCode(pAsyncTest, Ortho_RegionB_State,       Ortho_State,           Ortho_RegionB_Idle_State );
        oosmos_LeafInit           (pAsyncTest, Ortho_RegionB_Idle_State,  Ortho_RegionB_State                             );

#ifdef asynctestDEBUG
  oosmos_Debug(&pAsyncTest->StateMachine, true, EventNames);
#endif

  AsyncTestObjects += 1;

  return pAsyncTest;
}
