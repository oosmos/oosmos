//
// OOSMOS lcdtest Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
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

#include "pid.h"
#include "pidtest.h"
#include "oosmos.h"
#include <stdlib.h>
#include <stdbool.h>

#define BASELINE 25

//>>>EVENTS
enum {
  evDone = 1
};
//<<<EVENTS

struct pidtestTag
{
//>>>DECL
  oosmos_sStateMachineNoQueue(ROOT);
    oosmos_sLeaf Running_State;
//<<<DECL

#if defined(pid_DEBUG)
  oosmos_sObjectThread m_ObjectThread;
#endif

  pid  * m_pPID;
  float  m_Output;
  float  m_Sample;
};

static int RandBetween(int Lower, int Upper)
{
  return rand() % (Upper - Lower + 1) + Lower;
}

static void PidThread(pidtest * pPidTest, oosmos_sState * pState)
{
  //
  // Input is always a different unit of measure than the Output.  For example,
  // you measure speed for the input, but the output unit would be the PWM duty cycle
  // that drives the motor.
  //
  // Attempt to simulate that the input is affected by each output.
  //
  // Also, vary the amount of time between each sample to test the dt element of the PID.
  //
  oosmos_ThreadBegin();
    for (;;) {
      pPidTest->m_Sample = oosmos_Divide_Integral_Rounded(pPidTest->m_Output, 2);
      pPidTest->m_Output += pidAdjustOutput(pPidTest->m_pPID, pPidTest->m_Sample);
      oosmos_ThreadDelayMS(RandBetween(10, 15));
    }
  oosmos_ThreadEnd();
}

#if defined(pid_DEBUG)
static void TestThread(pidtest * pPidTest, oosmos_sState * pState)
{
  //
  // Change the set point to various values and then let the PID controller settle
  // between each one.
  //
  oosmos_ThreadBegin();
    // Reset
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(1000);

    // Bump Test
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE + 25);
    oosmos_ThreadDelayMS(1000);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);

    // Reset
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(1000);

    // Step Test
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE + 25);
    oosmos_ThreadDelayMS(1000);

    // Reset
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(1000);

    // Doublet Test
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE + 25);
    oosmos_ThreadDelayMS(500);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE - 10);
    oosmos_ThreadDelayMS(500);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(500);

    // Reset
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(1000);

    // PRBA Test
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE + 25);
    oosmos_ThreadDelayMS(500);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(500);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE + 25);
    oosmos_ThreadDelayMS(750);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(250);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE + 25);
    oosmos_ThreadDelayMS(500);
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);

    // Reset
    pidSet_SetPoint(pPidTest->m_pPID, BASELINE);
    oosmos_ThreadDelayMS(1000);

    oosmos_EndProgram(1);
  oosmos_ThreadEnd();
}
#endif

//>>>CODE
static bool Running_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  pidtest * pPidTest = (pidtest *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PidThread(pPidTest, pState);
      return true;
    }
  }

  return false;
}
//<<<CODE

extern pidtest * pidtestNew(void)
{
  oosmos_Allocate(pPidTest, pidtest, 1, NULL);

//>>>INIT
  oosmos_StateMachineInitNoQueue(pPidTest, ROOT, NULL, Running_State);
    oosmos_LeafInit(pPidTest, Running_State, ROOT, Running_State_Code);
//<<<INIT

  pPidTest->m_pPID = pidNew(1.0f, 0.0f, 0.0f, BASELINE);
  pPidTest->m_Output = 0.0f;

  #if defined(pid_DEBUG)
    oosmos_ObjectThreadInit(pPidTest, m_ObjectThread, TestThread, true);
  #endif

  return pPidTest;
}
