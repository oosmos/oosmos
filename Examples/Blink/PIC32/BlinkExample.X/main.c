/*
// OOSMOS - PIC32 Blink main program
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

#include "oosmos.h"
#include "toggle.h"
#include "pin.h"

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

extern int main(void)
{
  oosmos_ClockSpeedInMHz(80);

  pin * pRED    = pinNew(IOPORT_D, BIT_0, pinOut, pinActiveHigh);
  pin * pYELLOW = pinNew(IOPORT_D, BIT_1, pinOut, pinActiveHigh);
  pin * pGREEN  = pinNew(IOPORT_D, BIT_2, pinOut, pinActiveHigh);

  toggleNew(pRED,   2000, 2000);
  toggleNew(pYELLOW, 100,  100);
  toggleNew(pGREEN,   50, 1500);

  while (true)
    oosmos_RunStateMachines();
}
