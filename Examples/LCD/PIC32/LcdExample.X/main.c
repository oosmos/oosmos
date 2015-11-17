/*
// OOSMOS - LCD Example PIC32 main program
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
#include "pin.h"
#include "lcd.h"

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

extern int main(void)
{
  oosmos_ClockSpeedInMHz(80);

  /*
  // Setup and drive an HD44870 LCD device. 
  */
  pin * pRS     = pinNew(IOPORT_E, BIT_0, pinOut, pinActiveHigh);
  pin * pE      = pinNew(IOPORT_E, BIT_1, pinOut, pinActiveHigh);
  pin * pData4  = pinNew(IOPORT_E, BIT_2, pinOut, pinActiveHigh);
  pin * pData5  = pinNew(IOPORT_E, BIT_3, pinOut, pinActiveHigh);
  pin * pData6  = pinNew(IOPORT_E, BIT_4, pinOut, pinActiveHigh);
  pin * pData7  = pinNew(IOPORT_E, BIT_5, pinOut, pinActiveHigh);

  lcdtestNew(pRS, pE, pData4, pData5, pData6, pData7);

  while (true)
    oosmos_RunStateMachines();
}
