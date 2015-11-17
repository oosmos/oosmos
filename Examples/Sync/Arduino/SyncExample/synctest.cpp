/*
// OOSMOS synctest Class
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

#include <stdlib.h>
#include "oosmos.h"
#include "prt.h"
#include "synctest.h"

/*
// Change the following #undef to #define to enable debug output.
*/
#undef synctestDEBUG

/*
// Adjust this in order to preallocate all 'synctest' objects.  
// Use 1 for a memory constrained environment.
*/
#ifndef MAX_SYNCTEST
#define MAX_SYNCTEST 1
#endif

static int TotalTests      = 11;
static int SyncTestObjects = 0;
static int Successes       = 0;
static int Failures        = 0;

enum {
  eventDone = 1,
  eventTimedOut,
  eventPrint,
};

#ifdef synctestDEBUG
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

struct synctestTag
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


/*
// Test oosmos_SyncDelayMS.
*/
static bool Ortho_RegionA_TestA_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        prtFormatted("Test SyncDelayMS...\n");
        oosmos_SyncDelayMS(pRegion, 100);
      oosmos_SyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pSyncTest->Ortho_RegionA_TestB_State);
  }

  return false;
}

/*
// oosmos_SyncWaitCond
// oosmos_SyncWaitCond_TimeoutMS
*/
static bool Ortho_RegionA_TestB_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        bool TimedOut;
        prtFormatted("Test SyncWaitCond...\n");

        oosmos_SyncWaitCond(pRegion, ConditionRandom(2));

        prtFormatted("Test SyncWaitCond_TimeoutMS...\n");

        oosmos_SyncWaitCond_TimeoutMS(pRegion, ConditionTrue(), 100, &TimedOut);
        Successes += (TimedOut == false);

        oosmos_SyncWaitCond_TimeoutMS(pRegion, ConditionFalse(), 100, &TimedOut);
        Successes += (TimedOut == true);
      oosmos_SyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pSyncTest->Ortho_RegionA_TestC_State);
  }

  return false;
}

/*
// oosmos_SyncWaitCond_TimeoutMS_Event
*/
static bool Ortho_RegionA_TestC_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        prtFormatted("Test SyncWaitCond_TimeoutMS_Event...\n");

        oosmos_SyncWaitCond_TimeoutMS_Event(pRegion, ConditionTrue(), 100, eventTimedOut);
        Successes += 1;

        oosmos_SyncWaitCond_TimeoutMS_Event(pRegion, ConditionFalse(), 100, eventTimedOut);
        Failures += 1;
      oosmos_SyncEnd(pRegion);
      return oosmos_Transition(pRegion, &pSyncTest->Ortho_RegionA_TestD_State);

    case eventTimedOut:
      Successes += 1;
      return true;
  }

  return false;
}

/*
// oosmos_SyncWaitCond_TimeoutMS_Exit
*/
static bool Ortho_RegionA_TestD_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        prtFormatted("Test SyncWaitCond_TimeoutMS_Exit...\n");

        oosmos_SyncWaitCond_TimeoutMS_Exit(pRegion, ConditionTrue(), 100);
        Successes += 1;

        oosmos_SyncWaitCond_TimeoutMS_Exit(pRegion, ConditionFalse(), 100);
        Failures += 1;

      oosmos_SyncFinally(pRegion);
        Successes += 1;
        oosmos_Transition(pRegion, &pSyncTest->Ortho_RegionA_TestE_State);

      oosmos_SyncEnd(pRegion);
      return true;
  }

  return false;
}

/*
// oosmos_SyncWaitEvent
// oosmos_SyncWaitEvent_TimeoutMS
*/
static bool Ortho_RegionA_TestE_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        bool TimedOut;
        prtFormatted("Test SyncWaitEvent...\n");

        oosmos_SendEvent(pSyncTest, eventPrint);
        oosmos_SyncWaitEvent(pRegion, pEvent, eventDone);

        prtFormatted("Test SyncWaitEvent_TimeoutMS...\n");

        oosmos_SendEvent(pSyncTest, eventPrint);
        oosmos_SyncWaitEvent_TimeoutMS(pRegion, pEvent, eventDone, 100, &TimedOut);
        Successes += (TimedOut == false);

        oosmos_SyncWaitEvent_TimeoutMS(pRegion, pEvent, eventDone, 100, &TimedOut);
        Successes += (TimedOut == true);
      oosmos_SyncEnd(pRegion);

      return oosmos_Transition(pRegion, &pSyncTest->Ortho_RegionA_TestF_State);
  }

  return false;
}

/*
// oosmos_SyncWaitEvent_TimeoutMS_Event
*/
static bool Ortho_RegionA_TestF_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        prtFormatted("Test SyncWaitEvent_TimeoutMS_Event...\n");

        /*
        // Enqueue 'eventPrint' to the orthogonal state. We expect to get and 'eventDone' well 
        // before the timeout.
        */
        oosmos_SendEvent(pSyncTest, eventPrint);
        oosmos_SyncWaitEvent_TimeoutMS_Event(pRegion, pEvent, eventDone, 100, eventTimedOut);
        Successes += 1;

        /*
        // We don't enqueue 'eventPrint' so we don't expect an 'eventDone' therefore it should time out.
        */
        oosmos_SyncWaitEvent_TimeoutMS_Event(pRegion, pEvent, eventDone, 100, eventTimedOut);
        Failures += 1;
      oosmos_SyncEnd(pRegion);
      return oosmos_Transition(pRegion, &pSyncTest->Ortho_RegionA_TestG_State);

    case eventTimedOut:
      Successes += 1;
      return true;
  }

  return false;
}

/*
// oosmos_SyncWaitEvent_TimeoutMS_Exit
*/
static bool Ortho_RegionA_TestG_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case oosmos_INSTATE:
      oosmos_SyncBegin(pRegion);
        prtFormatted("Test SyncWaitEvent_TimeoutMS_Exit...\n");

        /*
        // Enqueue 'eventPrint' to the orthogonal state. We expect to get and 'eventDone' well 
        // before the timeout.
        */
        oosmos_SendEvent(pSyncTest, eventPrint);
        oosmos_SyncWaitEvent_TimeoutMS_Exit(pRegion, pEvent, eventDone, 100);
        Successes += 1;

        /*
        // We don't enqueue 'eventPrint' so we don't expect an 'eventDone' therefore it should time out.
        */
        oosmos_SyncWaitEvent_TimeoutMS_Exit(pRegion, pEvent, eventDone, 100);
        Failures += 1;

      oosmos_SyncFinally(pRegion);
        oosmos_Transition(pRegion, &pSyncTest->Ortho_RegionA_Done_State);

      oosmos_SyncEnd(pRegion);
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

      if (Successes == TotalTests * SyncTestObjects) {
        prtFormatted("SUCCESS.\n");
        oosmos_EndProgram(0);
      }

      return true;
  }
  
  return false;
}

static bool Ortho_RegionB_Idle_State_Code(void * pObject, oosmos_sRegion * pRegion, const oosmos_sEvent * pEvent)
{
  synctest * pSyncTest = (synctest *) pObject;

  switch (pEvent->Code) {
    case eventPrint:
      return oosmos_SendEvent(pSyncTest, eventDone);
  }

  return false;
}

extern synctest * synctestNew(void)
{  
  oosmos_Allocate(pSyncTest, synctest, MAX_SYNCTEST, NULL);

  /*                                          StateName                  Parent                 Default                    */
  /*                              ======================================================================================== */
  oosmos_StateMachineInit         (pSyncTest, StateMachine,              NULL,                  Ortho_State              );
    oosmos_OrthoInitNoCode        (pSyncTest, Ortho_State,               StateMachine                                    );
      oosmos_OrthoRegionInitNoCode(pSyncTest, Ortho_RegionA_State,       Ortho_State,           Ortho_RegionA_TestA_State);
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_TestA_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_TestB_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_TestC_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_TestD_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_TestE_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_TestF_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_TestG_State, Ortho_RegionA_State                             );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionA_Done_State,  Ortho_RegionA_State                             );
      oosmos_OrthoRegionInitNoCode(pSyncTest, Ortho_RegionB_State,       Ortho_State,           Ortho_RegionB_Idle_State );
        oosmos_LeafInit           (pSyncTest, Ortho_RegionB_Idle_State,  Ortho_RegionB_State                             );

#ifdef synctestDEBUG
  oosmos_Debug(&pSyncTest->StateMachine, true, EventNames);
#endif

  SyncTestObjects += 1;

  return pSyncTest;
}
