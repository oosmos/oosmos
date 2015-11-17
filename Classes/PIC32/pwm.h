/*
// OOSMOS pwm Class
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

#ifndef pwm_h
#define pwm_h

#include "oosmos.h"
#include "pin.h"
#include "timer.h"

typedef struct pwmTag pwm;

extern pwm * pwmNew(int PwmNumber, pin * pPwmPin, timer * pTimer);
extern void pwmDutyCycle(pwm * pPWM, float FrequencyKHz, float DutyCyclePercent);
extern void pwmStart(pwm * pPWM);
extern void pwmStop(pwm * pPWM);

#endif
