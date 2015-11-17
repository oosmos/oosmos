/*
// OOSMOS timer Class
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

#include "timer.h"
#include "oosmos.h"

struct timerTag
{
  timer_tCallback m_pCallback;
  void          * m_pContext;
  timerCalc       m_TimerCalc;
  uint16_t        m_OverflowCount;
  uint8_t         m_Priority;
  uint8_t         m_TimerNumber;
  uint8_t         m_TimerIndex;
  bool            m_InUse;
};

#define TIMERS      5
#define PRIORITIES  8

static timer TimerList[TIMERS];

typedef struct
{
  volatile unsigned int * pPeriod;
  uint16_t Vector;
  uint16_t Priority[PRIORITIES];
} sTimerData;

static const sTimerData TimerData[] = {
  {
    &PR1,
    _TIMER_1_VECTOR,
    { T1_INT_PRIOR_0,T1_INT_PRIOR_1,T1_INT_PRIOR_2,T1_INT_PRIOR_3,T1_INT_PRIOR_4,T1_INT_PRIOR_5,T1_INT_PRIOR_6,T1_INT_PRIOR_7 },
  },
  {
    &PR2,
    _TIMER_2_VECTOR,
    { T2_INT_PRIOR_0,T2_INT_PRIOR_1,T2_INT_PRIOR_2,T2_INT_PRIOR_3,T2_INT_PRIOR_4,T2_INT_PRIOR_5,T2_INT_PRIOR_6,T2_INT_PRIOR_7 },
  },
  {
    &PR3,
    _TIMER_3_VECTOR,
    { T3_INT_PRIOR_0,T3_INT_PRIOR_1,T3_INT_PRIOR_2,T3_INT_PRIOR_3,T3_INT_PRIOR_4,T3_INT_PRIOR_5,T3_INT_PRIOR_6,T3_INT_PRIOR_7 },
  },
  {
    &PR4,
    _TIMER_4_VECTOR,
    { T4_INT_PRIOR_0,T4_INT_PRIOR_1,T4_INT_PRIOR_2,T4_INT_PRIOR_3,T4_INT_PRIOR_4,T4_INT_PRIOR_5,T4_INT_PRIOR_6,T4_INT_PRIOR_7 },
  },
  {
    &PR5,
    _TIMER_5_VECTOR,
    { T5_INT_PRIOR_0,T5_INT_PRIOR_1,T5_INT_PRIOR_2,T5_INT_PRIOR_3,T5_INT_PRIOR_4,T5_INT_PRIOR_5,T5_INT_PRIOR_6,T5_INT_PRIOR_7 },
  },
};

typedef struct
{
  int      PrescalerInt;
  float    PrescalerFloat;
  uint16_t Bits[TIMERS];
} sPrescalerData;

static const sPrescalerData PrescalerData[] = {
  {   1,   1.0f, {   T1_PS_1_1,   T2_PS_1_1,   T3_PS_1_1,   T4_PS_1_1,   T5_PS_1_1 } },
  {   2,   2.0f, {   000000000,   T2_PS_1_2,   T3_PS_1_2,   T4_PS_1_2,   T5_PS_1_2 } },
  {   4,   4.0f, {   000000000,   T2_PS_1_4,   T3_PS_1_4,   T4_PS_1_4,   T5_PS_1_4 } },
  {   8,   8.0f, {   T1_PS_1_8,   T2_PS_1_8,   T3_PS_1_8,   T4_PS_1_8,   T5_PS_1_8 } },
  {  16,  16.0f, {   000000000,  T2_PS_1_16,  T3_PS_1_16,  T4_PS_1_16,  T5_PS_1_16 } },
  {  32,  32.0f, {   000000000,  T2_PS_1_32,  T3_PS_1_32,  T4_PS_1_32,  T5_PS_1_32 } },
  {  64,  64.0f, {  T1_PS_1_64,  T2_PS_1_64,  T3_PS_1_64,  T4_PS_1_64,  T5_PS_1_64 } },
  { 256, 256.0f, { T1_PS_1_256, T2_PS_1_256, T3_PS_1_256, T4_PS_1_256, T5_PS_1_256 } },
};

#define PRESCALERS (sizeof(PrescalerData) / sizeof(PrescalerData[0]))

static void ISR(timer * pTimer, const int TimerIndex)
{
  if (pTimer->m_OverflowCount == 0) {
    pTimer->m_pCallback(pTimer->m_pContext);

    if (pTimer->m_TimerCalc.OverflowCount > 0) {
      pTimer->m_OverflowCount = pTimer->m_TimerCalc.OverflowCount;
      *TimerData[TimerIndex].pPeriod = pTimer->m_TimerCalc.Ticks;
    }
  }
  else {
    pTimer->m_OverflowCount--;
    *TimerData[TimerIndex].pPeriod = 0xFFFF;
  }
}

static void CalculateTimerConfiguration(const float WallClockTimeSec,
                   const sPrescalerData * pPrescalerData, const int TimerResolution,
                   uint32_t * pOverflowCount, float * pRemainingTicks)
{
  const float PrescalerFloat = pPrescalerData->PrescalerFloat;
  const float Frequency = (oosmos_GetClockSpeedInMHz()*1000000) / PrescalerFloat;
  const float TotalTicks = WallClockTimeSec * Frequency;
  
  *pOverflowCount  = TotalTicks / (1<<TimerResolution);
  *pRemainingTicks = TotalTicks - (*pOverflowCount * (1<<TimerResolution));
}

static void CalcPeriodicSec(timer * pTimer, float WallClockTimeSec, timerCalc * pTimerCalc)
{
  uint32_t OverflowCount = 0;
  float    RemainingTicks = 0.0f;
  const sPrescalerData * pPrescalerData;

  for (pPrescalerData = PrescalerData; pPrescalerData < &PrescalerData[PRESCALERS];  pPrescalerData++) {
    CalculateTimerConfiguration(WallClockTimeSec, pPrescalerData, 16, &OverflowCount, &RemainingTicks);

    if (OverflowCount == 0 || pPrescalerData->PrescalerInt == 256)
      break;
  }

  const uint16_t Ticks = RemainingTicks + .5f;
  const int TimerIndex = pTimer->m_TimerIndex;

  pTimerCalc->OverflowCount = OverflowCount;
  pTimerCalc->Ticks         = Ticks;
  pTimerCalc->PrescalerBits = pPrescalerData->Bits[TimerIndex];
  pTimerCalc->PriorityBits  = TimerData[TimerIndex].Priority[pTimer->m_Priority];
}

extern void timerStart(timer * pTimer, const timerCalc * pTimerCalc)
{
  pTimer->m_TimerCalc = *pTimerCalc;

  const uint16_t        PrescalerBits = pTimer->m_TimerCalc.PrescalerBits;
  const uint16_t        PriorityBits  = pTimer->m_TimerCalc.PriorityBits;
  const uint16_t        Ticks         = pTimer->m_TimerCalc.Ticks;
  const timer_tCallback pCallback     = pTimer->m_pCallback;

  pTimer->m_OverflowCount = pTimer->m_TimerCalc.OverflowCount;

  switch (pTimer->m_TimerNumber) {
    case 1: 
      OpenTimer1(T1_ON | T1_SOURCE_INT | PrescalerBits, Ticks);
      ConfigIntTimer1((pCallback == NULL ? T1_INT_OFF : T1_INT_ON) | PriorityBits);
      break;
    case 2: 
      OpenTimer2(T2_ON | T2_SOURCE_INT | PrescalerBits, Ticks);
      ConfigIntTimer2((pCallback == NULL ? T2_INT_OFF : T2_INT_ON) | PriorityBits);
      break;
    case 3: 
      OpenTimer3(T3_ON | T3_SOURCE_INT | PrescalerBits, Ticks);
      ConfigIntTimer3((pCallback == NULL ? T3_INT_OFF : T3_INT_ON) | PriorityBits);
      break;
    case 4: 
      OpenTimer4(T4_ON | T4_SOURCE_INT | PrescalerBits, Ticks);
      ConfigIntTimer4((pCallback == NULL ? T4_INT_OFF : T4_INT_ON) | PriorityBits);
      break;
    case 5: 
      OpenTimer5(T5_ON | T5_SOURCE_INT | PrescalerBits, Ticks);
      ConfigIntTimer5((pCallback == NULL ? T5_INT_OFF : T5_INT_ON) | PriorityBits);
      break;
  }
}

extern void timerSetCallback(timer * pTimer, timer_tCallback pCallback, void * pContext)
{
  pTimer->m_pCallback = pCallback;
  pTimer->m_pContext  = pContext;
}

extern void timerStop(timer * pTimer)
{
  switch(pTimer->m_TimerNumber) {
    case 1: CloseTimer1(); break;
    case 2: CloseTimer2(); break;
    case 3: CloseTimer3(); break;
    case 4: CloseTimer4(); break;
    case 5: CloseTimer5(); break;
  }
}

extern void timerStartPeriodicUS(timer * pTimer, float WallClockTimeUS)
{
  timerStartPeriodicSec(pTimer, WallClockTimeUS / 1000000.0f);
}

extern void timerStartPeriodicMS(timer * pTimer, float WallClockTimeMS)
{
  timerStartPeriodicSec(pTimer, WallClockTimeMS / 1000.0f);
}

extern void timerStartPeriodicSec(timer * pTimer, float WallClockTimeSec)
{
  timerCalc TimerCalc;
  CalcPeriodicSec(pTimer,  WallClockTimeSec, &TimerCalc);
  timerStart(pTimer, &TimerCalc);
}

extern void timerCalcPeriodUS(timer * pTimer, float WallClockTimeUS, timerCalc * pTimerCalc)
{
  CalcPeriodicSec(pTimer,  WallClockTimeUS / 1000000.0f, pTimerCalc);
}

extern void timerCalcPeriodMS(timer * pTimer, float WallClockTimeMS, timerCalc * pTimerCalc)
{
  CalcPeriodicSec(pTimer, WallClockTimeMS / 1000.0f, pTimerCalc);
}

extern void timerCalcPeriodSec(timer * pTimer, float WallClockTimeSec, timerCalc * pTimerCalc)
{
  CalcPeriodicSec(pTimer, WallClockTimeSec, pTimerCalc);
}

extern int timerDisableInterrupt(timer * pTimer)
{
  int PreviousSetting;

  switch(pTimer->m_TimerNumber) {
    case 1: PreviousSetting = mT1GetIntEnable(); mT1IntEnable(0); break;
    case 2: PreviousSetting = mT2GetIntEnable(); mT2IntEnable(0); break;
    case 3: PreviousSetting = mT3GetIntEnable(); mT3IntEnable(0); break;
    case 4: PreviousSetting = mT4GetIntEnable(); mT4IntEnable(0); break;
    case 5: PreviousSetting = mT5GetIntEnable(); mT5IntEnable(0); break;
    default: while (true);
  }

  return PreviousSetting;
}

extern void timerEnableInterrupt(timer * pTimer, const int PreviousSetting)
{
  switch(pTimer->m_TimerNumber) {
    case 1: mT1IntEnable(PreviousSetting); break;
    case 2: mT2IntEnable(PreviousSetting); break;
    case 3: mT3IntEnable(PreviousSetting); break;
    case 4: mT4IntEnable(PreviousSetting); break;
    case 5: mT5IntEnable(PreviousSetting); break;
    default: while (true);
  }
}

extern int timerGetTimerNumber(timer * pTimer)
{
  return pTimer->m_TimerNumber;
}

extern timer * timerNew(const int TimerNumber, const int Priority)
{
  const int TimerIndex = TimerNumber-1;
  timer * pTimer = &TimerList[TimerIndex];

  pTimer->m_pCallback   = NULL;
  pTimer->m_pContext    = NULL;
  pTimer->m_TimerNumber = TimerNumber;
  pTimer->m_TimerIndex  = TimerNumber-1;
  pTimer->m_InUse       = true;
  pTimer->m_Priority    = Priority;
  
  const uint16_t Vector = TimerData[TimerIndex].Vector;
  INTSetVectorPriority(Vector, Priority);

  return pTimer;
}

#define timerVector(N) void __ISR(_TIMER_##N##_VECTOR) IntTimer##N##Handler(void) \
                       { INTClearFlag(INT_T##N); ISR(&TimerList[N-1], N-1); }

timerVector(1)
timerVector(2)
timerVector(3)
timerVector(4)
timerVector(5)
