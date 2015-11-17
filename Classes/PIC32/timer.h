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

#ifndef timer_h
#define timer_h

#include "oosmos.h"

typedef struct timerTag timer;

typedef struct
{
  uint16_t Ticks;
  uint16_t OverflowCount;
  uint16_t PrescalerBits;
  uint16_t PriorityBits;
} timerCalc;

typedef void (*timer_tCallback)(void * pContext);

extern timer * timerNew(int TimerNumber, int Priority);
extern void timerSetCallback(timer * pTimer, timer_tCallback, void * pContext);

extern void timerStartPeriodicUS(timer * pTimer, float WallClockTimeUS);
extern void timerStartPeriodicMS(timer * pTimer, float WallClockTimeMS);
extern void timerStartPeriodicSec(timer * pTimer, float WallClockTimeSec);

extern void timerCalcPeriodUS(timer * pTimer, float WallClockTimeUS, timerCalc * pTimerCalc);
extern void timerCalcPeriodMS(timer * pTimer, float WallClockTimeMS, timerCalc * pTimerCalc);
extern void timerCalcPeriodSec(timer * pTimer, float WallClockTimeSec, timerCalc * pTimerCalc);

extern int timerDisableInterrupt(timer * pTimer);
extern void timerEnableInterrupt(timer * pTimer, int PreviousSetting);

extern void timerStart(timer * pTimer, const timerCalc * pTimerCalc);
extern void timerStop(timer * pTimer);

extern int timerGetTimerNumber(timer * pTimer);

#endif
